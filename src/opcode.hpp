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

  // Media Control
  MEDIA_PLAY,
  MEDIA_PAUSE,
  MEDIA_TOGGLE,
  MEDIA_NEXT,
  MEDIA_PREV,

  // Volume Control
  VOLUME_INC,
  VOLUME_DEC,
  VOLUME_SET,
  VOLUME_MUTE,
  VOLUME_UNMUTE,
  VOLUME_TOGGLE,

  WAIT,
};

Opcode str_to_op(std::string str);
