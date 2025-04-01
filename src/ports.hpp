#pragma once

#include <string>
#include <vector>

std::vector<std::string> get_com_ports();
std::string scan_com_ports(const std::vector<std::string> &ports);
