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
  case 'a':
    return {KEY_A, false};
  case 'b':
    return {KEY_B, false};
  case 'c':
    return {KEY_C, false};
  case 'd':
    return {KEY_D, false};
  case 'e':
    return {KEY_E, false};
  case 'f':
    return {KEY_F, false};
  case 'g':
    return {KEY_G, false};
  case 'h':
    return {KEY_H, false};
  case 'i':
    return {KEY_I, false};
  case 'j':
    return {KEY_J, false};
  case 'k':
    return {KEY_K, false};
  case 'l':
    return {KEY_L, false};
  case 'm':
    return {KEY_M, false};
  case 'n':
    return {KEY_N, false};
  case 'o':
    return {KEY_O, false};
  case 'p':
    return {KEY_P, false};
  case 'q':
    return {KEY_Q, false};
  case 'r':
    return {KEY_R, false};
  case 's':
    return {KEY_S, false};
  case 't':
    return {KEY_T, false};
  case 'u':
    return {KEY_U, false};
  case 'v':
    return {KEY_V, false};
  case 'w':
    return {KEY_W, false};
  case 'x':
    return {KEY_X, false};
  case 'y':
    return {KEY_Y, false};
  case 'z':
    return {KEY_Z, false};
  case 'A':
    return {KEY_A, true};
  case 'B':
    return {KEY_B, true};
  case 'C':
    return {KEY_C, true};
  case 'D':
    return {KEY_D, true};
  case 'E':
    return {KEY_E, true};
  case 'F':
    return {KEY_F, true};
  case 'G':
    return {KEY_G, true};
  case 'H':
    return {KEY_H, true};
  case 'I':
    return {KEY_I, true};
  case 'J':
    return {KEY_J, true};
  case 'K':
    return {KEY_K, true};
  case 'L':
    return {KEY_L, true};
  case 'M':
    return {KEY_M, true};
  case 'N':
    return {KEY_N, true};
  case 'O':
    return {KEY_O, true};
  case 'P':
    return {KEY_P, true};
  case 'Q':
    return {KEY_Q, true};
  case 'R':
    return {KEY_R, true};
  case 'S':
    return {KEY_S, true};
  case 'T':
    return {KEY_T, true};
  case 'U':
    return {KEY_U, true};
  case 'V':
    return {KEY_V, true};
  case 'W':
    return {KEY_W, true};
  case 'X':
    return {KEY_X, true};
  case 'Y':
    return {KEY_Y, true};
  case 'Z':
    return {KEY_Z, true};
  }

  if (std::isdigit(c)) {
    return {KEY_1 + (c - '1'), false};
  }

  return {-1, false};
}

void kb_emit(uint16_t type, uint16_t code, int32_t value) {
  struct input_event ie{};
  ie.type = type;
  ie.code = code;
  ie.value = value;
  gettimeofday(&ie.time, nullptr);
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
  strcpy(usetup.name, "MacroDeck Keyboard");

  usetup.id.bustype = BUS_USB;
  usetup.id.vendor = 0x1234;
  usetup.id.product = 0x5678;

  if (ioctl(keyboard, UI_DEV_SETUP, &usetup) < 0) {
    error("Failed to setup keyboard");
    close(keyboard);
    keyboard = -1;
    return;
  }

  if (ioctl(keyboard, UI_DEV_CREATE) < 0) {
    error("Failed to create keyboard");
    close(keyboard);
    keyboard = -1;
    return;
  }

  sleep(1);
}

void clean_keyboard() {
  if (keyboard >= 0) {
    ioctl(keyboard, UI_DEV_DESTROY);
    close(keyboard);
    keyboard = -1;
  }
}

void press_key(uint16_t code) {
  kb_emit(EV_KEY, code, 1);
  kb_emit(EV_SYN, SYN_REPORT, 0);
}

void release_key(uint16_t code) {
  kb_emit(EV_KEY, code, 0);
  kb_emit(EV_SYN, SYN_REPORT, 0);
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
    Key key = char_to_keycode(c);

    if (key.keycode < 0) {
      error("Unknown key: " + std::string(1, c));
    } else {
      if (key.shift) {
        press_key(str_to_keycode("SHIFT").keycode);
      }
      press_key(key.keycode);

      usleep(50000);

      if (key.shift) {
        release_key(str_to_keycode("SHIFT").keycode);
      }
      release_key(key.keycode);
    }
  }
}
