#pragma once

#include <string>
#include <vector>

void app_open(const std::string &name, const std::vector<std::string> &args);
void app_close(const std::string &name);
void app_switch(const std::string &name);
