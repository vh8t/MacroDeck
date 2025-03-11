#include "log.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

#define RESET "\033[0m"
#define BOLD "\033[1m"
#define DIM "\033[2m"

#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define RED "\033[31m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"

std::string current_time() {
  auto now = std::chrono::system_clock::now();
  std::time_t now_c = std::chrono::system_clock::to_time_t(now);
  std::tm local_time = *std::localtime(&now_c);

  std::ostringstream oss;
  oss << std::put_time(&local_time, "%Y-%m-%d %H:%M:%S");
  return oss.str();
}

void md_log(const std::string &message) {
  std::cout << DIM << CYAN << "(" << current_time() << ") " << RESET << BOLD
            << GREEN << "[LOG] " << RESET << message << std::endl;
}

void md_warn(const std::string &message) {
  std::cout << DIM << CYAN << "(" << current_time() << ") " << RESET << BOLD
            << YELLOW << "[WARN] " << RESET << message << std::endl;
}

void md_error(const std::string &message) {
  std::cerr << DIM << CYAN << "(" << current_time() << ") " << RESET << BOLD
            << RED << "[ERROR] " << RESET << message << std::endl;
}

#undef RESET
#undef BOLD
#undef DIM

#undef GREEN
#undef YELLOW
#undef RED
#undef CYAN
#undef WHITE
