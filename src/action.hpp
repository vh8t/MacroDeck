#pragma once

#include "apps.hpp"
#include "keyboard.hpp"
#include "log.hpp"
#include "opcode.hpp"
#include "sound.hpp"

#include <chrono>
#include <string>
#include <thread>
#include <variant>
#include <vector>

struct Action {
  Opcode opcode;
  std::vector<std::variant<int, std::string>> args;

  Action(Opcode op) : opcode(op) {
  }
  Action(Opcode op, std::vector<std::variant<int, std::string>> args)
      : opcode(op), args(args) {
  }

  bool is_int(uint index) const {
    if (index < args.size()) {
      return std::holds_alternative<int>(args[index]);
    }
    return false;
  }

  bool is_str(uint index) const {
    if (index < args.size()) {
      return std::holds_alternative<std::string>(args[index]);
    }
    return false;
  }

  int get_int(uint index) const {
    if (index < args.size()) {
      return std::get<int>(args[index]);
    }
    return 0;
  }

  std::string get_string(uint index) const {
    if (index < args.size()) {
      return std::get<std::string>(args[index]);
    }
    return nullptr;
  }

  void execute() const {
    switch (opcode) {
    case NOP:
      warning("Invalid opcode " + std::to_string(opcode));
      break;
    case APP_OPEN: {
      if (args.size() == 0 || is_int(0)) {
        error("Invalid argument for APP_OPEN");
        return;
      }

      std::vector<std::string> app_args;
      for (uint i = 1; i < args.size(); i++) {
        if (is_int(i)) {
          error("Invalid argument for APP_OPEN");
          return;
        }
        app_args.push_back(get_string(i));
      }

      app_open(get_string(0), app_args);
    } break;
    case APP_CLOSE: {
      if (args.size() != 1 || is_int(0)) {
        error("Invalid argument for APP_CLOSE");
        return;
      }
      app_close(get_string(0));
    } break;
    case APP_SWITCH: {
      if (args.size() != 1 || is_int(0)) {
        error("Invalid argument for APP_SWITCH");
        return;
      }
      app_switch(get_string(0));
    } break;
    case KEY_PRESS: {
      if (args.size() != 1 || is_int(0)) {
        error("Invalid argument for KEY_PRESS");
        return;
      }
      key_press(get_string(0));
    } break;
    case KEY_RELEASE: {
      if (args.size() != 1 || is_int(0)) {
        error("Invalid argument for KEY_RELEASE");
        return;
      }
      key_release(get_string(0));
    } break;
    case KEY_CLICK: {
      if (args.size() != 1 || is_int(0)) {
        error("Invalid argument for KEY_CLICK");
        return;
      }
      key_click(get_string(0));
    } break;
    case KEY_TYPE: {
      if (args.size() != 1 || is_int(0)) {
        error("Invalid argument for KEY_TYPE");
        return;
      }
      key_type(get_string(0));
    } break;
    case VOLUME_INC: {
      if (args.size() != 1 || is_str(0)) {
        error("Invalid argument for VOLUME_INC");
        return;
      }
      volume_inc(get_int(0));
    } break;
    case VOLUME_DEC: {
      if (args.size() != 1 || is_str(0)) {
        error("Invalid argument for VOLUME_DEC");
        return;
      }
      volume_dec(get_int(0));
    } break;
    case VOLUME_SET: {
      if (args.size() != 1 || is_str(0)) {
        error("Invalid argument for VOLUME_SET");
        return;
      }
      volume_set(get_int(0));
    } break;
    case VOLUME_MUTE: {
      if (args.size() != 0) {
        error("Invalid argument for VOLUME_MUTE");
        return;
      }
      volume_mute();
    } break;
    case VOLUME_UNMUTE: {
      if (args.size() != 0) {
        error("Invalid argument for VOLUME_MUTE");
        return;
      }
      volume_unmute();
    } break;
    case VOLUME_TOGGLE: {
      if (args.size() != 0) {
        error("Invalid argument for VOLUME_MUTE");
        return;
      }
      volume_toggle();
    } break;
    case CAPTURE_INC: {
      if (args.size() != 1 || is_str(0)) {
        error("Invalid argument for CAPTURE_INC");
        return;
      }
      capture_inc(get_int(0));
    } break;
    case CAPTURE_DEC: {
      if (args.size() != 1 || is_str(0)) {
        error("Invalid argument for CAPTURE_DEC");
        return;
      }
      capture_dec(get_int(0));
    } break;
    case CAPTURE_SET: {
      if (args.size() != 1 || is_str(0)) {
        error("Invalid argument for CAPTURE_SET");
        return;
      }
      capture_set(get_int(0));
    } break;
    case CAPTURE_MUTE: {
      if (args.size() != 0) {
        error("Invalid argument for CAPTURE_MUTE");
        return;
      }
      capture_mute();
    } break;
    case CAPTURE_UNMUTE: {
      if (args.size() != 0) {
        error("Invalid argument for CAPTURE_MUTE");
        return;
      }
      capture_unmute();
    } break;
    case CAPTURE_TOGGLE: {
      if (args.size() != 0) {
        error("Invalid argument for CAPTURE_MUTE");
        return;
      }
      capture_toggle();
    } break;
    case WAIT: {
      if (args.size() != 1 || is_str(0)) {
        error("Invalid argument for WAIT");
        return;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(get_int(0)));
    } break;
    }
  };
};
