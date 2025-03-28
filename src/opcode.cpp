#include "opcode.hpp"
#include "log.hpp"

Opcode str_to_op(const std::string &str) {
  if (str == "app_open")
    return APP_OPEN;
  if (str == "app_open_if_closed")
    return APP_OPEN_IF_CLOSED;
  if (str == "app_close")
    return APP_CLOSE;
  if (str == "app_close_if_open")
    return APP_CLOSE_IF_OPEN;
  if (str == "app_switch")
    return APP_SWITCH;
  if (str == "app_switch_if_open")
    return APP_SWITCH_IF_OPEN;
  if (str == "key_press")
    return KEY_PRESS;
  if (str == "key_release")
    return KEY_RELEASE;
  if (str == "key_click")
    return KEY_CLICK;
  if (str == "key_type")
    return KEY_TYPE;
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
