#pragma once

#include "api.hpp"
#include "log.hpp"
#include "opcode.hpp"

#include <chrono>
#include <string>
#include <thread>
#include <variant>
#include <vector>

struct Action {
  Opcode opcode;
  std::vector<std::variant<int, std::string>> args;

  Action(Opcode op) : opcode(op) {}
  Action(Opcode op, std::vector<std::variant<int, std::string>> args)
      : opcode(op), args(args) {}

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

  int i(uint index) const {
    if (index < args.size()) {
      return std::get<int>(args[index]);
    }
    return 0;
  }

  std::string s(uint index) const {
    if (index < args.size()) {
      return std::get<std::string>(args[index]);
    }
    return nullptr;
  }

  void execute() const {
    switch (opcode) {
    case NOP:
      break;
    case APP_OPEN: {
      if (args.size() == 0 || is_int(0)) {
        md_error("Invalid argument for APP_OPEN");
        return;
      }

      std::vector<std::string> app_args;
      for (uint i = 1; i < args.size(); i++) {
        if (is_int(i)) {
          md_error("Invalid argument for APP_OPEN");
          return;
        }
        app_args.push_back(s(i));
      }

      app_open(s(0), app_args);
    } break;
    case APP_CLOSE: {
      if (args.size() != 1 || is_int(0)) {
        md_error("Invalid argument for APP_CLOSE");
        return;
      }
      app_close(s(0));
    } break;
    case APP_SWITCH: {
      if (args.size() != 1 || is_int(0)) {
        md_error("Invalid argument for APP_SWITCH");
        return;
      }
      app_switch(s(0));
    } break;
    case KEY_PRESS: {
      if (args.size() != 1 || is_int(0)) {
        md_error("Invalid argument for KEY_PRESS");
        return;
      }
      key_press(s(0));
    } break;
    case KEY_RELEASE: {
      if (args.size() != 1 || is_int(0)) {
        md_error("Invalid argument for KEY_RELEASE");
        return;
      }
      key_release(s(0));
    } break;
    case KEY_CLICK: {
      if (args.size() != 1 || is_int(0)) {
        md_error("Invalid argument for KEY_CLICK");
        return;
      }
      key_click(s(0));
    } break;
    case KEY_TYPE: {
      if (args.size() != 1 || is_int(0)) {
        md_error("Invalid argument for KEY_TYPE");
        return;
      }
      key_type(s(0));
    } break;
    case MEDIA_PLAY:
      break;
    case MEDIA_PAUSE:
      break;
    case MEDIA_TOGGLE:
      break;
    case MEDIA_NEXT:
      break;
    case MEDIA_PREV:
      break;
    case VOLUME_INC: {
      if (args.size() != 1 || is_str(0)) {
        md_error("Invalid argument for VOLUME_INC");
        return;
      }
      volume_inc(i(0));
    } break;
    case VOLUME_DEC: {
      if (args.size() != 1 || is_str(0)) {
        md_error("Invalid argument for VOLUME_DEC");
        return;
      }
      volume_dec(i(0));
    } break;
    case VOLUME_SET: {
      if (args.size() != 1 || is_str(0)) {
        md_error("Invalid argument for VOLUME_SET");
        return;
      }
      volume_set(i(0));
    } break;
    case VOLUME_MUTE: {
      if (args.size() != 0) {
        md_error("Invalid argument for VOLUME_MUTE");
        return;
      }
      volume_mute();
    } break;
    case VOLUME_UNMUTE: {
      if (args.size() != 0) {
        md_error("Invalid argument for VOLUME_MUTE");
        return;
      }
      volume_unmute();
    } break;
    case VOLUME_TOGGLE: {
      if (args.size() != 0) {
        md_error("Invalid argument for VOLUME_MUTE");
        return;
      }
      volume_toggle();
    } break;
    case WAIT: {
      if (args.size() != 1 || is_str(0)) {
        md_error("Invalid argument for WAIT");
        return;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(i(0)));
    } break;
    }
  };
};
