#include "keyboard.hpp"
#include "log.hpp"

#include <cctype>
#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include <linux/uinput.h>
#include <sstream>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>
#include <unordered_map>
#include <vector>

int keyboard = -1;

Key str_to_keycode(const std::string &key) {
  static const std::unordered_map<std::string, int> key_map = {
      {"RETURN", KEY_ENTER},
      {"ENTER", KEY_ENTER},
      {"TAB", KEY_TAB},
      {"SPACE", KEY_SPACE},
      {"BACKSPACE", KEY_BACKSPACE},
      {"ESC", KEY_ESC},
      {"ESCAPE", KEY_ESC},

      {"L_SHIFT", KEY_LEFTSHIFT},
      {"R_SHIFT", KEY_RIGHTSHIFT},
      {"SHIFT", KEY_LEFTSHIFT},

      {"L_CTRL", KEY_LEFTCTRL},
      {"R_CTRL", KEY_RIGHTCTRL},
      {"CTRL", KEY_LEFTCTRL},

      {"L_ALT", KEY_LEFTALT},
      {"R_ALT", KEY_RIGHTALT},
      {"ALT", KEY_LEFTALT},

      {"L_SUPER", KEY_LEFTMETA},
      {"R_SUPER", KEY_RIGHTMETA},
      {"SUPER", KEY_LEFTMETA},

      {"F1", KEY_F1},
      {"F2", KEY_F2},
      {"F3", KEY_F3},
      {"F4", KEY_F4},
      {"F5", KEY_F5},
      {"F6", KEY_F6},
      {"F7", KEY_F7},
      {"F8", KEY_F8},
      {"F9", KEY_F9},
      {"F10", KEY_F10},
      {"F11", KEY_F11},
      {"F12", KEY_F12},

      {"UP", KEY_UP},
      {"DOWN", KEY_DOWN},
      {"LEFT", KEY_LEFT},
      {"RIGHT", KEY_RIGHT},

      {"CAPSLOCK", KEY_CAPSLOCK},
      {"NUMLOCK", KEY_NUMLOCK},
      {"SCROLLLOCK", KEY_SCROLLLOCK},

      {"INSERT", KEY_INSERT},
      {"DELETE", KEY_DELETE},
      {"HOME", KEY_HOME},
      {"END", KEY_END},
      {"PGUP", KEY_PAGEUP},
      {"PGDOWN", KEY_PAGEDOWN},

      {"PRTSCR", KEY_PRINT},
      {"PAUSE", KEY_PAUSE},
  };

  auto it = key_map.find(key);

  return {(it != key_map.end()) ? it->second : -1, false};
}

Key char_to_keycode(char c) {
  switch (c) {
  case '!':
    return {KEY_1, true};
  case '@':
    return {KEY_2, true};
  case '#':
    return {KEY_3, true};
  case '$':
    return {KEY_4, true};
  case '%':
    return {KEY_5, true};
  case '^':
    return {KEY_6, true};
  case '&':
    return {KEY_7, true};
  case '*':
    return {KEY_8, true};
  case '(':
    return {KEY_9, true};
  case ')':
    return {KEY_0, true};
  case '-':
    return {KEY_MINUS, false};
  case '_':
    return {KEY_MINUS, true};
  case '+':
    return {KEY_EQUAL, true};
  case '=':
    return {KEY_EQUAL, false};
  case '{':
    return {KEY_LEFTBRACE, true};
  case '}':
    return {KEY_RIGHTBRACE, true};
  case '[':
    return {KEY_LEFTBRACE, false};
  case ']':
    return {KEY_RIGHTBRACE, false};
  case '"':
    return {KEY_APOSTROPHE, true};
  case '\'':
    return {KEY_APOSTROPHE, false};
  case '\\':
    return {KEY_BACKSLASH, false};
  case '|':
    return {KEY_BACKSLASH, true};
  case '<':
    return {KEY_COMMA, true};
  case ',':
    return {KEY_COMMA, false};
  case '>':
    return {KEY_DOT, true};
  case '.':
    return {KEY_DOT, false};
  case '?':
    return {KEY_SLASH, true};
  case '/':
    return {KEY_SLASH, false};
  case '`':
    return {KEY_GRAVE, false};
  case '~':
    return {KEY_GRAVE, true};
  case '\n':
    return {KEY_ENTER, false};
  case ' ':
    return {KEY_SPACE, false};
  case '\t':
    return {KEY_TAB, false};
  case '\b':
    return {KEY_BACKSPACE, false};
  }

  if (std::isalpha(c)) {
    if (std::isupper(c)) {
      return {KEY_A + (c - 'A'), true};
    } else {
      return {KEY_A + (std::toupper(c) - 'A'), false};
    }
  } else if (std::isdigit(c)) {
    return {KEY_1 + (c - '1'), false};
  }

  return {-1, false};
}

void emit(uint16_t type, uint16_t code, int32_t value) {
  struct input_event ie{};
  ie.type = type;
  ie.code = code;
  ie.value = value;
  ie.time.tv_sec = 0;
  ie.time.tv_usec = 0;
  write(keyboard, &ie, sizeof(ie));
}

void init_keyboard() {
  keyboard = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
  if (keyboard < 0) {
    error("Can not open /dev/uinput. Run as root or check permission");
    return;
  }

  ioctl(keyboard, UI_SET_EVBIT, EV_KEY);

  for (int key = KEY_ESC; key <= KEY_MAX; key++) {
    ioctl(keyboard, UI_SET_KEYBIT, key);
  }

  struct uinput_setup usetup{};
  memset(&usetup, 0, sizeof(usetup));
  usetup.id.bustype = BUS_USB;
  usetup.id.vendor = 0x1234;
  usetup.id.product = 0x5678;
  strcpy(usetup.name, "MacroDeck Keyboard");

  ioctl(keyboard, UI_DEV_SETUP, &usetup);
  ioctl(keyboard, UI_DEV_CREATE);

  sleep(1);
}

void clean_keyboard() {
  if (keyboard < 0) {
    ioctl(keyboard, UI_DEV_DESTROY);
    close(keyboard);
  }
}

void press_key(uint16_t code) {
  emit(EV_KEY, code, 1);
  emit(EV_SYN, SYN_REPORT, 0);
}

void release_key(uint16_t code) {
  emit(EV_KEY, code, 0);
  emit(EV_SYN, SYN_REPORT, 0);
}

void key_press(const std::string &combination) {
  if (keyboard < 0) {
    error("Keyboard is not initialized");
    return;
  }

  std::istringstream iss(combination);
  std::vector<std::string> tokens;
  std::string token;

  while (std::getline(iss, token, ' ')) {
    tokens.push_back(token);
  }

  if (tokens.empty()) {
    warning("Invalid key combination: " + combination);
    return;
  }

  for (size_t i = 0; i < tokens.size(); i++) {
    Key key;
    if (tokens[i].length() == 1) {
      key = char_to_keycode(tokens[i][0]);
    } else {
      key = str_to_keycode(tokens[i]);
    }

    if (key.keycode < 0) {
      error("Unknown key: " + tokens[i]);
    } else {
      if (key.shift) {
        press_key(str_to_keycode("SHIFT").keycode);
      }
      press_key(key.keycode);
    }
  }
}

void key_release(const std::string &combination) {
  if (keyboard < 0) {
    error("Keyboard is not initialized");
    return;
  }

  std::istringstream iss(combination);
  std::vector<std::string> tokens;
  std::string token;

  while (std::getline(iss, token, ' ')) {
    tokens.push_back(token);
  }

  if (tokens.empty()) {
    warning("Invalid key combination: " + combination);
    return;
  }

  for (size_t i = 0; i < tokens.size(); i++) {
    Key key;
    if (tokens[i].length() == 1) {
      key = char_to_keycode(tokens[i][0]);
    } else {
      key = str_to_keycode(tokens[i]);
    }

    if (key.keycode < 0) {
      error("Unknown key: " + tokens[i]);
    } else {
      if (key.shift) {
        release_key(str_to_keycode("SHIFT").keycode);
      }
      release_key(key.keycode);
    }
  }
}

void key_click(const std::string &combination) {
  key_press(combination);
  usleep(50000);
  key_release(combination);
}

void key_type(const std::string &text) {
  for (const char &c : text) {
    key_click(std::string(1, c));
  }
}
