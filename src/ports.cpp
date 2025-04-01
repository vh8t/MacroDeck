#include "ports.hpp"
#include "log.hpp"

#include <boost/asio.hpp>
#include <chrono>
#include <filesystem>

namespace fs = std::filesystem;

std::vector<std::string> get_com_ports() {
  std::vector<std::string> ports;
  std::string path = "/dev/";

  for (const auto &entry : fs::directory_iterator(path)) {
    std::string filename = entry.path().string();
    if (filename.find("ttyUSB") != std::string::npos ||
        filename.find("ttyS") != std::string::npos) {
      ports.push_back(filename);
    }
  }

  return ports;
}

std::string scan_com_ports(const std::vector<std::string> &ports) {
  boost::asio::io_context io;
  const int scan_duration = 20 * 1000;
  const int interval = 250;

  auto start = std::chrono::steady_clock::now();

  while (std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::steady_clock::now() - start)
             .count() < scan_duration) {
    for (const auto &port : ports) {
      try {
        boost::asio::serial_port serial(io, port);
        serial.set_option(boost::asio::serial_port_base::baud_rate(115200));

        char ping;
        boost::asio::deadline_timer timer(
            io, boost::posix_time::milliseconds(interval));
        boost::system::error_code ec;

        boost::asio::async_read(
            serial, boost::asio::buffer(&ping, 1),
            [&](const boost::system::error_code &error, std::size_t) {
              ec = error;
              timer.cancel();
            });

        timer.async_wait([&](const boost::system::error_code &error) {
          if (!error)
            serial.cancel();
        });

        io.run();
        io.restart();

        if (!ec && ping == 'P') {
          info("Device found on " + port);
          return port;
        }
      } catch (...) {
      }
    }
  }

  return "";
}
