#pragma once

#include "macro.hpp"
#include "nlohmann/json.hpp"

#include <string>

using json = nlohmann::json;

json load_config();
Macro *load_macro(const std::string &name);
