#include "loader.hpp"
#include "log.hpp"
#include "opcode.hpp"

#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fstream>
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

json load_config() {
  std::string home_dir;
  try {
    home_dir = get_home_dir();
  } catch (const std::exception &e) {
    error(e.what());
    return nullptr;
  }

  fs::path config_path = fs::path(home_dir) / ".config/macrodeck/config.json";

  if (!fs::exists(config_path)) {
    error("Failed to open config");
    return nullptr;
  }

  std::ifstream file(config_path);
  if (!file.is_open()) {
    error("Failed to open config");
    return nullptr;
  }

  json data;
  try {
    file >> data;
  } catch (const std::exception &e) {
    error(std::string("Failed to parse json: ") + e.what());
    return nullptr;
  }

  return data;
}

std::array<std::string, 2> get_icon(const std::string &name) {
  std::string home_dir;
  try {
    home_dir = get_home_dir();
  } catch (const std::exception &e) {
    error(e.what());
    return {"", ""};
  }

  fs::path logo_path_png =
      fs::path(home_dir) / ".config/macrodeck/icons" / (name + ".png");
  fs::path logo_path_jpg =
      fs::path(home_dir) / ".config/macrodeck/icons" / (name + ".jpg");
  fs::path logo_path_jpeg =
      fs::path(home_dir) / ".config/macrodeck/icons" / (name + ".jpeg");

  if (fs::exists(logo_path_png)) {
    return {logo_path_png.string(), name + ".png"};
  } else if (fs::exists(logo_path_jpg)) {
    return {logo_path_jpg.string(), name + ".jpg"};
  } else if (fs::exists(logo_path_jpeg)) {
    return {logo_path_jpeg.string(), name + ".jpeg"};
  }

  return {"", ""};
}

Macro *load_macro(const std::string &name) {
  std::string home_dir;
  try {
    home_dir = get_home_dir();
  } catch (const std::exception &e) {
    error(e.what());
    return nullptr;
  }

  fs::path macro_path =
      fs::path(home_dir) / ".config/macrodeck/macros/" / (name + ".json");

  if (!fs::exists(macro_path)) {
    error("Failed to load macro: " + name);
    return nullptr;
  }

  std::ifstream file(macro_path);
  if (!file.is_open()) {
    error("Failed to load macro: " + name);
    return nullptr;
  }

  json data;
  try {
    file >> data;
  } catch (const std::exception &e) {
    error(std::string("Failed to parse json: ") + e.what());
    return nullptr;
  }

  if (data.contains("macro") && data["macro"].is_array()) {
    Macro *macro = new Macro();
    for (const auto &raw_action : data["macro"]) {
      if (!raw_action.is_array() || raw_action.empty() ||
          !raw_action[0].is_string()) {
        error("Invalid action format in macro");
        delete macro;
        return nullptr;
      }

      Opcode op = str_to_op(raw_action[0].get<std::string>());
      Action action(op);

      for (size_t i = 1; i < raw_action.size(); ++i) {
        if (raw_action[i].is_string()) {
          action.args.push_back(raw_action[i].get<std::string>());
        } else if (raw_action[i].is_number_integer()) {
          action.args.push_back(raw_action[i].get<int>());
        } else {
          error("Invalid argument type in action");
          delete macro;
          return nullptr;
        }
      }

      macro->macro.push_back(action);
    }

    return macro;
  }

  return nullptr;
}
