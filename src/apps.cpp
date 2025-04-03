#include "apps.hpp"
#include "log.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <sstream>
#include <string>
#include <sys/wait.h>

std::string hyprland_get_class_name(const std::string &partial) {
  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen("hyprctl clients", "r"),
                                                pclose);
  if (!pipe) {
    error("Failed to execute: hyprctl clients");
    return "";
  }

  std::string exact_class;
  char buffer[256];
  while (fgets(buffer, sizeof(buffer), pipe.get()) != nullptr) {
    std::string line(buffer);
    if (line.find("class: ") != std::string::npos) {
      std::istringstream iss(line);
      std::string key, value;
      iss >> key >> value;

      if (value.find(partial) != std::string::npos) {
        exact_class = value;
        break;
      }
    }
  }
  return exact_class;
}

void app_open(const std::string &name, const std::vector<std::string> &args) {
  pid_t pid = fork();
  if (pid == -1) {
    error(std::string("Failed to fork process") + strerror(errno));
    return;
  }

  if (pid == 0) {
    std::vector<const char *> c_args = {name.c_str()};

    for (const auto &arg : args) {
      c_args.push_back(arg.c_str());
    }
    c_args.push_back(nullptr);

    execvp(c_args[0], const_cast<char *const *>(c_args.data()));

    error("Failed to execute" + name + ": " + strerror(errno));
    exit(1);
  } else {
    int status;
    waitpid(pid, &status, WNOHANG);
  }
  return;
}

void app_close(const std::string &name) {
  const char *wayland_env = getenv("WAYLAND_DISPLAY");
  bool is_wayland = (wayland_env != nullptr);

  pid_t pid = fork();
  if (pid == -1) {
    error(std::string("Failed to fork process") + strerror(errno));
    return;
  }

  if (pid == 0) {
    if (is_wayland) {
      log("Executing: pkill -x '" + name + "'");
      execlp("pkill", "pkill", "-x", name.c_str(), nullptr);
      error(std::string("Failed to execute pkill: ") + strerror(errno));
    } else {
      log("Executing: xdotool search --class '" + name + "' windowclose");
      execlp("xdotool", "xdotool", "search", "--class", name.c_str(),
             "windowclose", nullptr);
      error(std::string("Failed to execute xdotool: ") + strerror(errno));
    }
    exit(1);
  } else {
    int status;
    waitpid(pid, &status, WNOHANG);
  }
}

void app_switch(const std::string &name) {
  const char *wayland_env = getenv("WAYLAND_DISPLAY");
  bool is_wayland = (wayland_env != nullptr);

  pid_t pid = fork();
  if (pid == -1) {
    error(std::string("Failed to fork process") + strerror(errno));
    return;
  }

  if (pid == 0) {
    if (is_wayland) {
      const char *sway_env = getenv("SWAYSOCK");
      if (sway_env) {
        log("Executing: swaymsg '[app_id=\"" + name + "\"] focus");
        execlp("swaymsg", "swaymsg", ("[app_id=\"" + name + "\"]").c_str(),
               "focus", nullptr);
        error(std::string("Failed to execute swaymsg: ") + strerror(errno));
      }

      const char *hypr_env = getenv("HYPRLAND_INSTANCE_SIGNATURE");
      if (hypr_env) {
        std::string full_class = hyprland_get_class_name(name);
        if (full_class.empty()) {
          warning("No matching window found for: " + name);
          return;
        }

        log("Executing: hyprctl dispatch focuswindow class:" + full_class);
        execlp("hyprctl", "hyprctl", "dispatch", "focuswindow",
               ("class:" + full_class).c_str(), nullptr);
        error(std::string("Failed to execute hyprctl: ") + strerror(errno));
      }
    } else {
      log("Executing: xdotool search --class '" + name + "' windowfocus");
      execlp("xdotool", "xdotool", "search", "--class", name.c_str(),
             "windowfocus", nullptr);
      error(std::string("Failed to execute xdotool: ") + strerror(errno));

      log("(Fallback) Executing: wmctrl -x -a '" + name + "'");
      execlp("wmctrl", "wmctrl", "-x", "-a", name.c_str(), nullptr);
      error(std::string("Failed to execute wmctrl: ") + strerror(errno));
    }
    exit(1);
  } else {
    int status;
    waitpid(pid, &status, WNOHANG);
  }
}
