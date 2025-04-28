#pragma once

#include <string>

enum Opcode {
  NOP,

  // Application Control
  APP_OPEN,
  APP_CLOSE,
  APP_SWITCH,

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

  // Capture Control
  CAPTURE_INC,
  CAPTURE_DEC,
  CAPTURE_SET,
  CAPTURE_MUTE,
  CAPTURE_UNMUTE,
  CAPTURE_TOGGLE,

  WAIT,
};

Opcode str_to_op(const std::string &str);
