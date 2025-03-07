#pragma once

#define CROW_USE_BOOST 1

#include "crow.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

json load_config(crow::websocket::connection &conn);
