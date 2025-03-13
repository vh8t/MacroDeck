#include "log.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

#define RESET "\x1b[0m"
#define BOLD "\x1b[1m"

#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define MAGENTA "\x1b[35m"
#define CYAN "\x1b[36m"
#define WHITE "\x1b[37m"

#define LOG "[LOG    ]"
#define INFO "[INFO   ]"
#define ERROR "[ERROR  ]"
#define WARNING "[WARNING]"

std::string current_time() {
  auto now = std::chrono::system_clock::now();
  std::time_t now_c = std::chrono::system_clock::to_time_t(now);
  std::tm local_time = *std::localtime(&now_c);

  std::ostringstream oss;
  oss << std::put_time(&local_time, "%Y-%m-%d %H:%M:%S");
  return oss.str();
}

void log(const std::string &message) {
  std::cout << CYAN << "(" << current_time() << ") " << RESET << BOLD << GREEN
            << LOG << RESET << " " << message << std::endl;
}

void info(const std::string &message) {
  std::cout << CYAN << "(" << current_time() << ") " << RESET << BOLD << MAGENTA
            << INFO << RESET << " " << message << std::endl;
}

void error(const std::string &message) {
  std::cerr << CYAN << "(" << current_time() << ") " << RESET << BOLD << RED
            << ERROR << RESET << " " << message << std::endl;
}

void warning(const std::string &message) {
  std::cout << CYAN << "(" << current_time() << ") " << RESET << BOLD << YELLOW
            << WARNING << RESET << " " << message << std::endl;
}

#undef RESET
#undef BOLD

#undef RED
#undef GREEN
#undef YELLOW
#undef MAGENTA
#undef CYAN
#undef WHITE

#undef LOG
#undef INFO
#undef ERROR
#undef WARNING
