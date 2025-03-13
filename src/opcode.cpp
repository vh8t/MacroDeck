#include "opcode.hpp"
#include "log.hpp"

Opcode str_to_op(const std::string &str) {
  if (str == "app_open")
    return APP_OPEN;
  if (str == "app_close")
    return APP_CLOSE;
  if (str == "app_switch")
    return APP_SWITCH;
  if (str == "key_press")
    return KEY_PRESS;
  if (str == "key_release")
    return KEY_RELEASE;
  if (str == "key_click")
    return KEY_CLICK;
  if (str == "key_type")
    return KEY_TYPE;
  if (str == "media_play")
    return MEDIA_PLAY;
  if (str == "media_pause")
    return MEDIA_PAUSE;
  if (str == "media_toggle")
    return MEDIA_TOGGLE;
  if (str == "media_next")
    return MEDIA_NEXT;
  if (str == "media_prev")
    return MEDIA_PREV;
  if (str == "volume_inc")
    return VOLUME_INC;
  if (str == "volume_dec")
    return VOLUME_DEC;
  if (str == "volume_set")
    return VOLUME_SET;
  if (str == "volume_mute")
    return VOLUME_MUTE;
  if (str == "volume_unmute")
    return VOLUME_UNMUTE;
  if (str == "volume_toggle")
    return VOLUME_TOGGLE;
  if (str == "wait")
    return WAIT;
  warning("Unknown action: " + str);
  return NOP;
}
