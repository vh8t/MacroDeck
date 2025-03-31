#define CROW_USE_BOOST 1

#include "argparse.hpp"
#include "crow.h"
#include "keyboard.hpp"
#include "loader.hpp"
#include "log.hpp"
#include "macro.hpp"
#include "nlohmann/json.hpp"
#include "sound.hpp"

#include <arpa/inet.h>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <ifaddrs.h>
#include <ios>
#include <iostream>
#include <mutex>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <sys/types.h>
#include <unordered_map>

#define VERSION "0.0.4"
#define TAG "-dev"

using json = nlohmann::json;
namespace fs = std::filesystem;

std::unordered_map<std::string, Macro *> loaded_macros;
std::vector<std::array<std::string, 2>> icons;

std::unordered_map<crow::websocket::connection *, bool> authenticated_devices;
std::mutex auth_mutex;

std::string get_base_dir() {
  std::string exe_dir = fs::canonical("/proc/self/exe").parent_path().string();
  return fs::path(exe_dir).parent_path().string();
}

void setup() {
  log("Initializing master volume control");
  init_alsa();
  log("Initializing virtual keyboard");
  init_keyboard();
}

void cleanup() {
  std::cout << "\n";
  log("Cleaning master volume control");
  clean_alsa();
  log("Cleaning virtual keyboard");
  clean_keyboard();

  for (auto &[name, macro] : loaded_macros) {
    log("Deallocating macro: " + name);
    delete macro;
  }
  loaded_macros.clear();
}

void sig_handler(int signal) {
  cleanup();
  std::exit(signal);
}

void get_interfaces() {
  struct ifaddrs *ifaddr;
  if (getifaddrs(&ifaddr) == -1) {
    error("Failed to get interface addresses");
    return;
  }

  for (struct ifaddrs *ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
    if (ifa->ifa_addr == nullptr) {
      continue;
    }

    if (ifa->ifa_addr->sa_family == AF_INET) {
      char ip[INET_ADDRSTRLEN];
      struct sockaddr_in *addr =
          reinterpret_cast<struct sockaddr_in *>(ifa->ifa_addr);
      inet_ntop(AF_INET, &addr->sin_addr, ip, INET_ADDRSTRLEN);

      if (strncmp(ip, "127.", 4) == 0) {
        continue;
      }

      info(std::string("Interface: ") + ifa->ifa_name + ", IP: " + ip);
    }
  }

  freeifaddrs(ifaddr);
}

int main(int argc, char **argv) {
  argparse::ArgumentParser program("MacroDeck", std::string(VERSION) + TAG,
                                   argparse::default_arguments::none);

  program.add_argument("--list-configs")
      .help("list all config names from config file")
      .flag();

  program.add_argument("-c", "--config")
      .help("use specifis config file")
      .default_value(std::string("-"))
      .metavar("<path>");

  program.add_argument("-a", "--auth")
      .help("set a authentication password")
      .default_value(std::string(""))
      .metavar("<password>");

  program.add_argument("-V", "--verbose")
      .help("increase output verbosity")
      .flag();

  program.add_argument("-v", "--version")
      .help("get the current MacroDeck version")
      .flag();

  program.add_argument("-h", "--help").help("show this message").flag();

  try {
    program.parse_args(argc, argv);
  } catch (const std::exception &e) {
    error(e.what());
    std::cerr << program;
    return 1;
  }

  bool should_exit = false;
  if (program["--list-configs"] == true) {
    json config = load_config("-");

    if (config == nullptr) {
      error("Could not load config from '~/.config/macrodeck/config.json'");
      return -1;
    }

    if (config.is_array()) {
      for (size_t i = 0; i < config.size(); i++) {
        if (config[i].contains("name") && config[i]["name"].is_string()) {
          std::cout << i + 1 << ". " << config[i]["name"].get<std::string>()
                    << "\n";

          if (config[i].contains("buttons") &&
              config[i]["buttons"].is_array()) {
            for (const auto &button : config[i]["buttons"]) {
              if (button.contains("macro") && button["macro"].is_string()) {
                std::cout << "  - " << button["macro"].get<std::string>()
                          << "\n";
              }
            }
          }
        }
      }
      std::cout.flush();
    } else {
      if (config.contains("name") && config["name"].is_string()) {
        std::cout << "1. " << config["name"].get<std::string>() << std::endl;

        if (config.contains("buttons") && config["buttons"].is_array()) {
          for (const auto &button : config["buttons"]) {
            if (button.contains("macro") && button["macro"].is_string()) {
              std::cout << "  - " << button["macro"].get<std::string>() << "\n";
            }
          }
        }
      }
    }
    should_exit = true;
  }

  if (program["--version"] == true) {
    std::cout << "MacroDeck version " << VERSION << TAG << std::endl;
    should_exit = true;
  }

  if (program["--help"] == true) {
    std::cout << program;
    should_exit = true;
  }

  if (should_exit)
    return 0;

  std::string confing_path = program.get("--config");
  std::string password = program.get("--auth");

  setup();
  std::atexit(cleanup);
  std::signal(SIGINT, sig_handler);

  log("Getting config");
  json config = load_config(confing_path);

  if (config == nullptr) {
    error("Could not load config from '~/.config/macrodeck/config.json'");
    return 1;
  }

  if (config.is_array()) {
    for (const auto &conf : config) {
      if (conf.contains("buttons") && conf["buttons"].is_array()) {
        for (const auto &button : conf["buttons"]) {
          if (!button.is_object()) {
            warning("Invalid button format");
            continue;
          }

          if (button.contains("macro") && button["macro"].is_string()) {
            std::string macro_name = button["macro"].get<std::string>();
            if (loaded_macros.find(macro_name) != loaded_macros.end()) {
              continue;
            }

            Macro *macro = load_macro(macro_name);

            if (macro) {
              log("Loaded macro: " + macro_name);
              loaded_macros[macro_name] = macro;

              std::array<std::string, 2> icon = get_icon(macro_name);
              if (icon[0] != "" && icon[1] != "") {
                icons.push_back(icon);
              }
            } else {
              warning("Failed to load macro: " + macro_name);
            }
          }
        }
      }
    }
  } else if (config.is_object()) {
    if (config.contains("buttons") && config["buttons"].is_array()) {
      for (const auto &button : config["buttons"]) {
        if (!button.is_object()) {
          warning("Invalid button format");
          continue;
        }

        if (button.contains("macro") && button["macro"].is_string()) {
          std::string macro_name = button["macro"].get<std::string>();
          if (loaded_macros.find(macro_name) != loaded_macros.end()) {
            continue;
          }

          Macro *macro = load_macro(macro_name);

          if (macro) {
            log("Loaded macro: " + macro_name);
            loaded_macros[macro_name] = macro;

            std::array<std::string, 2> icon = get_icon(macro_name);
            if (icon[0] != "" && icon[1] != "") {
              icons.push_back(icon);
            }
          } else {
            warning("Failed to load macro: " + macro_name);
          }
        }
      }
    }
  }

  std::string base_dir = get_base_dir();
  std::string template_dir = base_dir + "/templates";
  std::string static_dir = base_dir + "/static";

  log("Setting base dir: " + base_dir);
  log("Setting template dir: " + template_dir);
  log("Setting static dir: " + static_dir);

  crow::mustache::set_global_base(template_dir);

  crow::SimpleApp app;

  CROW_WEBSOCKET_ROUTE(app, "/ws")
      .onopen([&](crow::websocket::connection &conn) {
        std::lock_guard<std::mutex> lock(auth_mutex);
        if (password.empty()) {
          authenticated_devices[&conn] = true;
          conn.send_text("auth-not-required");
        } else {
          authenticated_devices[&conn] = false;
          conn.send_text("auth-required");
        }
        log("Opened connection with: " + conn.get_remote_ip());
      })

      .onclose([&](crow::websocket::connection &conn, const std::string &reason,
                   uint16_t) {
        std::lock_guard<std::mutex> lock(auth_mutex);
        authenticated_devices.erase(&conn);
        log("Closed connection with: " + conn.get_remote_ip() +
            " with reason: " + reason);
      })

      .onmessage([&](crow::websocket::connection &conn, const std::string &data,
                     bool is_binary) {
        std::lock_guard<std::mutex> lock(auth_mutex);

        if (authenticated_devices[&conn]) {
          if (!is_binary) {
            if (data == "get-config") {
              std::string jsonString = config.dump();
              conn.send_text("config:" + jsonString);
            } else if (data.length() > 10 &&
                       data.substr(0, 10) == "run-macro:") {
              std::string macro_name = data.substr(10);

              if (loaded_macros.find(macro_name) != loaded_macros.end()) {
                info("Running macro: " + macro_name);
                loaded_macros[macro_name]->run();
              } else {
                error("Invalid macro: " + macro_name);
              }
            }
          }
        } else {
          if (data.length() > 5 && data.substr(0, 5) == "auth:" &&
              data.substr(5) == password) {
            info("Client " + conn.get_remote_ip() + " authenticated");
            authenticated_devices[&conn] = true;
            conn.send_text("auth-success");
          } else {
            info("Failed to authenticate with message: " + data);
            conn.send_text("auth-fail");
            conn.close();
          }
        }
      });

  CROW_ROUTE(app, "/")([]() {
    auto page = crow::mustache::load_text("index.html");
    return page;
  });

  CROW_ROUTE(app, "/assets/<path>")
  ([static_dir](const crow::request &req, crow::response &res,
                std::string path) {
    std::string file_path = static_dir + "/" + path;
    if (!fs::exists(file_path)) {
      res.code = 404;
      res.write("File not found");
      error("File not found: " + file_path);
    } else {
      res.set_static_file_info_unsafe(file_path);
    }
    res.end();
  });

  CROW_ROUTE(app, "/icon/<path>")
  ([](const crow::request &req, crow::response &res, std::string path) {
    for (const auto &icon : icons) {
      if (icon[1].size() >= path.size() &&
          icon[1].substr(0, path.size()) == path) {

        std::ifstream file(icon[0], std::ios::binary);
        if (!file.is_open()) {
          res.code = 404;
          res.write("File not found");
          res.end();
          return;
        }

        std::string extension;
        size_t dotPos = icon[0].find_last_of('.');
        if (dotPos != std::string::npos) {
          extension = icon[0].substr(dotPos + 1);
        }

        static const std::unordered_map<std::string, std::string> mimeTypes = {
            {"png", "image/png"},
            {"jpg", "image/jpeg"},
            {"jpeg", "image/jpeg"}};

        auto it = mimeTypes.find(extension);
        if (it != mimeTypes.end()) {
          res.set_header("Content-Type", it->second);
        } else {
          res.set_header("Content-Type", "application/octet-stream");
        }

        std::ostringstream buffer;
        buffer << file.rdbuf();
        res.write(buffer.str());
        res.end();
        return;
      }
    }
    res.code = 404;
    res.write("File not found");
    res.end();
  });

  get_interfaces();
  info("App ready on port 5000");

  if (program["--verbose"] != true)
    app.loglevel(crow::LogLevel::Warning);

  app.port(5000).multithreaded().run();
}
