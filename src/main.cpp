#define CROW_USE_BOOST 1

#include "api.hpp"
#include "crow.h"
#include "macro.hpp"
#include "nlohmann/json.hpp"
#include "opcode.hpp"

#include <arpa/inet.h>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <fstream>
#include <ifaddrs.h>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/types.h>
#include <unordered_map>

using json = nlohmann::json;

void setup() {
  std::cout << "Initializing X11 display" << std::endl;
  init_x11();
  std::cout << "Initializing master volume control" << std::endl;
  init_alsa();
}

void cleanup() {
  std::cout << "Cleaning X11 display" << std::endl;
  clean_x11();
  std::cout << "Cleaning master volume control" << std::endl;
  clean_alsa();
}

void sig_handler(int signal) {
  cleanup();
  std::exit(signal);
}

int main() {
  struct ifaddrs *ifaddr;
  if (getifaddrs(&ifaddr) == -1) {
    std::cerr << "Failed to get interface addresses" << std::endl;
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

      std::cout << "Interface: " << ifa->ifa_name << ", IP: " << ip
                << std::endl;
    }
  }

  freeifaddrs(ifaddr);

  setup();
  std::atexit(cleanup);
  std::signal(SIGINT, sig_handler);

  Macro setup = {{
      {KEY_CLICK, {"Super+3"}},
      {WAIT, {100}},
      {KEY_CLICK, {"Super+Return"}},
      {WAIT, {100}},
      {KEY_CLICK, {"Super+4"}},
      {WAIT, {100}},
      {KEY_CLICK, {"Super+n"}},
  }};

  Macro volume_up = {{{VOLUME_INC, {10}}}};
  Macro volume_down = {{{VOLUME_DEC, {10}}}};
  Macro volume_set = {{{VOLUME_SET, {75}}}};
  Macro volume_mute = {{{VOLUME_MUTE, {}}}};
  Macro volume_unmute = {{{VOLUME_UNMUTE, {}}}};
  Macro volume_toggle = {{{VOLUME_TOGGLE, {}}}};

  std::unordered_map<std::string, Macro *> loaded_macros;

  loaded_macros["setup"] = &setup;
  loaded_macros["volume_up"] = &volume_up;
  loaded_macros["volume_down"] = &volume_down;
  loaded_macros["volume_set"] = &volume_set;
  loaded_macros["volume_mute"] = &volume_mute;
  loaded_macros["volume_unmute"] = &volume_unmute;
  loaded_macros["volume_toggle"] = &volume_toggle;

  crow::SimpleApp app;

  CROW_WEBSOCKET_ROUTE(app, "/ws")
      .onopen([&](crow::websocket::connection &conn) {
        std::cout << "Opened connection with: " << conn.get_remote_ip()
                  << std::endl;
      })

      .onclose([&](crow::websocket::connection &conn, const std::string &reason,
                   uint16_t) {
        std::cout << "Closed connection with: " << conn.get_remote_ip()
                  << " with reason: " << reason << std::endl;
      })

      .onmessage([&](crow::websocket::connection &conn, const std::string &data,
                     bool is_binary) {
        if (!is_binary) {
          if (data == "get-config") {
            std::cout << "Getting config: " << conn.get_remote_ip()
                      << std::endl;
            std::ifstream file("config.json");
            if (!file.is_open()) {
              conn.send_text("conf-err-1");
              std::cerr << "Failed to open config" << std::endl;
              return;
            }

            json data;
            try {
              file >> data;
            } catch (const std::exception &e) {
              conn.send_text("conf-err-2");
              std::cerr << "Failed to parse json: " << e.what() << std::endl;
              return;
            }

            std::string jsonString = data.dump();
            conn.send_text("config:" + jsonString);
          } else if (data.length() > 10 &&
                     data.compare(0, 10, "run-macro:") == 0) {
            std::string macro_name = data.substr(10);

            if (loaded_macros.find(macro_name) != loaded_macros.end()) {
              std::cout << "Running: " << macro_name << std::endl;
              loaded_macros[macro_name]->run();
            } else {
              std::cerr << "Invalid macro: " << macro_name << std::endl;
            }
          }
        }
      });

  CROW_ROUTE(app, "/")([]() {
    auto page = crow::mustache::load_text("index.html");
    return page;
  });

  app.port(5000).multithreaded().run();
}
