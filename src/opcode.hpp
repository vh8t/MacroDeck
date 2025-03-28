#pragma once

#include <string>

enum Opcode {
  NOP,

  // Application Control
  APP_OPEN,
  APP_OPEN_IF_CLOSED,
  APP_CLOSE,
  APP_CLOSE_IF_OPEN,
  APP_SWITCH,
  APP_SWITCH_IF_OPEN,

  // Keyboard Actions
  KEY_PRESS,
  KEY_RELEASE,
  KEY_CLICK,
  KEY_TYPE,

  // Volume Control
  VOLUME_INC,
  VOLUME_DEC,
  VOLUME_SET,
  VOLUME_MUTE,
  VOLUME_UNMUTE,
  VOLUME_TOGGLE,

  WAIT,
};

Opcode str_to_op(const std::string &str);
