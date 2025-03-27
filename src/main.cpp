#include <fstream>
#include <ios>
#include <sstream>
#define CROW_USE_BOOST 1

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
#include <ifaddrs.h>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/types.h>
#include <unordered_map>

using json = nlohmann::json;
namespace fs = std::filesystem;

std::unordered_map<std::string, Macro *> loaded_macros;
std::vector<std::array<std::string, 2>> icons;

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

int main() {
  setup();
  std::atexit(cleanup);
  std::signal(SIGINT, sig_handler);

  log("Getting config");
  json config = load_config();

  if (config == nullptr) {
    error("Could not load config from '~/.config/macrodeck/config.json'");
    return -1;
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
        log("Opened connection with: " + conn.get_remote_ip());
      })

      .onclose([&](crow::websocket::connection &conn, const std::string &reason,
                   uint16_t) {
        log("Closed connection with: " + conn.get_remote_ip() +
            " with reason: " + reason);
      })

      .onmessage([&](crow::websocket::connection &conn, const std::string &data,
                     bool is_binary) {
        if (!is_binary) {
          if (data == "get-config") {
            std::string jsonString = config.dump();
            conn.send_text("config:" + jsonString);
          } else if (data.length() > 10 &&
                     data.compare(0, 10, "run-macro:") == 0) {
            std::string macro_name = data.substr(10);

            if (loaded_macros.find(macro_name) != loaded_macros.end()) {
              info("Running macro: " + macro_name);
              loaded_macros[macro_name]->run();
            } else {
              error("Invalid macro: " + macro_name);
            }
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
        // res.set_static_file_info_unsafe(icon[0]);
        // res.end();
        // return;

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

  app.loglevel(crow::LogLevel::Warning);
  app.port(5000).multithreaded().run();
}
