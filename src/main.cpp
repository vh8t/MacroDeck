#define CROW_USE_BOOST 1

#include "api.hpp"
#include "crow.h"
#include "loader.hpp"
#include "log.hpp"
#include "macro.hpp"
#include "nlohmann/json.hpp"

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

std::unordered_map<std::string, Macro *> loaded_macros;

void setup() {
  log("Initializing X11 display");
  init_x11();
  log("Initializing master volume control");
  init_alsa();
}

void cleanup() {
  std::cout << "\n";
  log("Cleaning X11 display");
  clean_x11();
  log("Cleaning master volume control");
  clean_alsa();

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

int main() {
  struct ifaddrs *ifaddr;
  if (getifaddrs(&ifaddr) == -1) {
    error("Failed to get interface addresses");
    return 1;
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

      info(std::string("Interface: ") + ifa->ifa_name + ", IP: " + ip);
    }
  }

  info("Port: 5000");

  freeifaddrs(ifaddr);

  setup();
  std::atexit(cleanup);
  std::signal(SIGINT, sig_handler);

  log("Getting config");
  json config = load_config();

  if (config == nullptr) {
    error("Could not load config from '~/.config/macrodeck/config.json'");
    return -1;
  }

  if (config.contains("buttons") && config["buttons"].is_array()) {
    for (const auto &button : config["buttons"]) {
      if (!button.is_object()) {
        warn("Invalid button format");
        continue;
      }

      if (button.contains("macro") && button["macro"].is_string()) {
        std::string macro_name = button["macro"].get<std::string>();

        Macro *macro = load_macro(macro_name);
        if (macro) {
          log("Loaded macro: " + macro_name);
          loaded_macros[macro_name] = macro;
        } else {
          warn("Failed to load macro: " + macro_name);
        }
      }
    }
  }

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
              log("Running macro: " + macro_name);
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

  app.loglevel(crow::LogLevel::Warning);
  app.port(5000).multithreaded().run();
}
