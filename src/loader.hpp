#pragma once

#include "macro.hpp"
#include "nlohmann/json.hpp"

#include <string>

using json = nlohmann::json;

json load_config(const std::string &path);
Macro *load_macro(const std::string &name);
std::array<std::string, 2> get_icon(const std::string &name);
