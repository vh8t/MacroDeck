#include "loader.hpp"

#include <boost/asio/config.hpp>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <pwd.h>
#include <stdexcept>
#include <string>
#include <unistd.h>

namespace fs = std::filesystem;

std::string get_home_dir() {
  const char *home_dir = std::getenv("HOME");
  if (home_dir)
    return home_dir;

  uid_t uid = getuid();
  struct passwd *pw = getpwuid(uid);
  if (!pw)
    throw std::runtime_error(
        "Unable to determine home directory: getpwuid failed");

  return std::string("/home/") + pw->pw_name;
}

json load_config(crow::websocket::connection &conn) {
  std::string home_dir;
  try {
    home_dir = get_home_dir();
  } catch (const std::exception &e) {
    conn.send_text("conf-err-1");
    std::cerr << "Failed to get home directory" << std::endl;
    return nullptr;
  }

  fs::path config_path = fs::path(home_dir) / ".config/macrodeck/config.json";

  if (!fs::exists(config_path)) {
    conn.send_text("conf-err-2");
    std::cerr << "Failed to open config" << std::endl;
    return nullptr;
  }

  std::ifstream file(config_path);
  if (!file.is_open()) {
    conn.send_text("conf-err-2");
    std::cerr << "Failed to open config" << std::endl;
    return nullptr;
  }

  json data;
  try {
    file >> data;
  } catch (const std::exception &e) {
    conn.send_text("conf-err-3");
    std::cerr << "Failed to parse json: " << e.what() << std::endl;
    return nullptr;
  }

  return data;
}
