#include "api.hpp"
#include "log.hpp"

#include <X11/X.h>
#include <X11/XF86keysym.h>
#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#include <X11/keysym.h>
#include <algorithm>
#include <alsa/asoundlib.h>
#include <cmath>
#include <cstring>
#include <sstream>
#include <sys/wait.h>
#include <unordered_map>

Display *display = nullptr;
snd_mixer_t *mixer = nullptr;
snd_mixer_selem_id_t *sid = nullptr;
snd_mixer_elem_t *elem = nullptr;

std::unordered_map<std::string, KeySym> modifier_map = {
    {"Alt", XK_Alt_L},
    {"Ctrl", XK_Control_L},
    {"Shift", XK_Shift_L},
    {"Super", XK_Super_L},
    {"VolumeDown", XF86XK_AudioLowerVolume},
    {"VolumeUp", XF86XK_AudioRaiseVolume},
    {"VolumeMute", XF86XK_AudioMute},
    {"MicMute", XF86XK_AudioMicMute},
};

std::unordered_map<char, KeySym> special_char_map = {
    {'!', XK_exclam},       {'"', XK_quotedbl},    {'#', XK_numbersign},
    {'$', XK_dollar},       {'%', XK_percent},     {'&', XK_ampersand},
    {'\'', XK_apostrophe},  {'(', XK_parenleft},   {')', XK_parenright},
    {'*', XK_asterisk},     {'+', XK_plus},        {',', XK_comma},
    {'-', XK_minus},        {'.', XK_period},      {'/', XK_slash},
    {':', XK_colon},        {';', XK_semicolon},   {'<', XK_less},
    {'=', XK_equal},        {'>', XK_greater},     {'?', XK_question},
    {'@', XK_at},           {'[', XK_bracketleft}, {'\\', XK_backslash},
    {']', XK_bracketright}, {'^', XK_asciicircum}, {'_', XK_underscore},
    {'`', XK_grave},        {'{', XK_braceleft},   {'|', XK_bar},
    {'}', XK_braceright},   {'~', XK_asciitilde},  {'\n', XK_Return},
    {'\t', XK_Tab},         {' ', XK_space}};

Window find_window(const std::string &app) {
  if (!display) {
    md_error("Display is not initialized");
    return 0;
  }

  Window root = DefaultRootWindow(display);
  Window parent;
  Window *children;
  uint nchildren;

  if (XQueryTree(display, root, &root, &parent, &children, &nchildren) == 0) {
    return 0;
  }

  Atom atom = XInternAtom(display, "WM_CLASS", True);

  for (uint i = 0; i < nchildren; i++) {
    char *prop = nullptr;
    Atom actual_type;
    int actual_format;
    ulong nitems, bytes_after;
    unsigned char *prop_return = nullptr;

    if (XGetWindowProperty(display, children[i], atom, 0, 1024, False,
                           AnyPropertyType, &actual_type, &actual_format,
                           &nitems, &bytes_after, &prop_return) == Success &&
        prop_return) {
      prop = reinterpret_cast<char *>(prop_return);

      if (prop && std::strstr(prop, app.c_str())) {
        XFree(prop);
        return children[i];
      }

      XFree(prop);
    }
  }

  return 0;
}

void simulate_key_press(KeySym key_sym) {
  if (!display) {
    md_error("Display is not initialized");
    return;
  }

  KeyCode key_code = XKeysymToKeycode(display, key_sym);

  XTestFakeKeyEvent(display, key_code, True, 0);
  XFlush(display);
}

void simulate_key_release(KeySym key_sym) {
  if (!display) {
    md_error("Display is not initialized");
    return;
  }

  KeyCode key_code = XKeysymToKeycode(display, key_sym);

  XTestFakeKeyEvent(display, key_code, False, 0);
  XFlush(display);
}

void init_x11() {
  display = XOpenDisplay(nullptr);
  if (!display) {
    md_error("Unable to open X display");
  }
}

void clean_x11() {
  if (display) {
    XCloseDisplay(display);
  }
}

void init_alsa() {
  snd_mixer_open(&mixer, 0);
  snd_mixer_attach(mixer, "default");
  snd_mixer_selem_register(mixer, nullptr, nullptr);
  snd_mixer_load(mixer);

  snd_mixer_selem_id_alloca(&sid);
  snd_mixer_selem_id_set_index(sid, 0);
  snd_mixer_selem_id_set_name(sid, "Master");

  elem = snd_mixer_find_selem(mixer, sid);
  if (!elem) {
    md_error("Unable to find master volume control");
  }
}

void clean_alsa() {
  if (mixer) {
    snd_mixer_close(mixer);
  }
}

void app_open(const std::string &name, const std::vector<std::string> &args) {
  pid_t pid = fork();
  if (pid == -1) {
    md_error(std::string("Failed to fork process") + strerror(errno));
    return;
  }

  if (pid == 0) {
    std::vector<const char *> c_args = {name.c_str()};

    for (const auto &arg : args) {
      c_args.push_back(arg.c_str());
    }
    c_args.push_back(nullptr);

    execvp(c_args[0], const_cast<char *const *>(c_args.data()));

    md_error("Failed to execute" + name + ": " + strerror(errno));
    exit(1);
  } else {
    int status;
    waitpid(pid, &status, WNOHANG);
  }
  return;

  if (fork() == 0) {
    std::vector<const char *> c_args;
    c_args.push_back(name.c_str());

    for (const auto &arg : args) {
      c_args.push_back(arg.c_str());
    }
    c_args.push_back(nullptr);

    execvp(c_args[0], const_cast<char *const *>(c_args.data()));

    md_error("Failed to execute: " + name + strerror(errno));
    exit(1);
  } else {
    md_error("Failed to fork process");
  }
}

void app_close(const std::string &name) {
  pid_t pid = fork();
  if (pid == -1) {
    md_error(std::string("Failed to fork process") + strerror(errno));
    return;
  }

  if (pid == 0) {
    md_log("Executing: xdotool search --class '" + name + "' windowclose");

    std::vector<const char *> args = {"xdotool",    "search",      "--class",
                                      name.c_str(), "windowclose", nullptr};

    execvp("xdotool", const_cast<char *const *>(args.data()));

    md_error(std::string("Failed to execute xdotool: ") + strerror(errno));
    exit(1);
  } else {
    int status;
    waitpid(pid, &status, WNOHANG);
  }
}

void app_switch(const std::string &name) {
  pid_t pid = fork();
  if (pid == -1) {
    md_error(std::string("Failed to fork process") + strerror(errno));
    return;
  }

  if (pid == 0) {
    md_log("Executing: xdotool search --class '" + name + "' windowfocus");

    std::vector<const char *> args = {"xdotool",    "search",      "--class",
                                      name.c_str(), "windowfocus", nullptr};

    execvp("xdotool", const_cast<char *const *>(args.data()));

    md_error(std::string("Failed to execute xdotool: ") + strerror(errno));
    exit(1);
  } else {
    int status;
    waitpid(pid, &status, WNOHANG);
  }
}

void key_press(const std::string &combination) {
  if (!display) {
    md_error("Display is not initialized");
    return;
  }

  std::istringstream iss(combination);
  std::vector<std::string> tokens;
  std::string token;

  while (std::getline(iss, token, '+')) {
    tokens.push_back(token);
  }

  if (tokens.empty()) {
    md_error("Invalid key combination: " + combination);
    return;
  }

  for (uint i = 0; i < tokens.size() - 1; ++i) {
    const std::string &modifier = tokens[i];
    if (modifier_map.find(modifier) != modifier_map.end()) {
      simulate_key_press(modifier_map[modifier]);
    } else {
      md_error("Unknown modifier: " + modifier);
    }
  }

  const std::string &main_key = tokens.back();
  KeySym main_key_sym = XStringToKeysym(main_key.c_str());

  if (main_key_sym == NoSymbol) {
    md_error("Unknown key: " + main_key);
  } else {
    simulate_key_press(main_key_sym);
  }
}

void key_release(const std::string &combination) {
  if (!display) {
    md_error("Display is not initialized");
    return;
  }

  std::istringstream iss(combination);
  std::vector<std::string> tokens;
  std::string token;

  while (std::getline(iss, token, '+')) {
    tokens.push_back(token);
  }

  if (tokens.empty()) {
    md_error("Invalid key combination: " + combination);
    return;
  }

  for (uint i = 0; i < tokens.size() - 1; ++i) {
    const std::string &modifier = tokens[i];
    if (modifier_map.find(modifier) != modifier_map.end()) {
      simulate_key_release(modifier_map[modifier]);
    } else {
      md_error("Unknown modifier: " + modifier);
    }
  }

  const std::string &main_key = tokens.back();
  KeySym main_key_sym = XStringToKeysym(main_key.c_str());

  if (main_key_sym == NoSymbol) {
    md_error("Unknown key: " + main_key);
  } else {
    simulate_key_release(main_key_sym);
  }
}

void key_click(const std::string &combination) {
  if (!display) {
    md_error("Display is not initialized");
    return;
  }

  std::istringstream iss(combination);
  std::vector<std::string> tokens;
  std::string token;

  while (std::getline(iss, token, '+')) {
    tokens.push_back(token);
  }

  if (tokens.empty()) {
    md_error("Invalid key combination: " + combination);
    return;
  }

  for (uint i = 0; i < tokens.size() - 1; ++i) {
    const std::string &modifier = tokens[i];
    if (modifier_map.find(modifier) != modifier_map.end()) {
      simulate_key_press(modifier_map[modifier]);
    } else {
      md_error("Unknown modifier: " + modifier);
    }
  }

  const std::string &main_key = tokens.back();
  KeySym main_key_sym = XStringToKeysym(main_key.c_str());

  if (main_key_sym == NoSymbol) {
    md_error("Unknown key: " + main_key);
  } else {
    simulate_key_press(main_key_sym);
    simulate_key_release(main_key_sym);
  }

  for (int i = tokens.size() - 2; i >= 0; --i) {
    const std::string &modifier = tokens[i];
    if (modifier_map.find(modifier) != modifier_map.end()) {
      simulate_key_release(modifier_map[modifier]);
    } else {
      md_error("Unknown modifier: " + modifier);
    }
  }
}

void key_type(const std::string &text) {
  if (!display) {
    md_error("Display is not initialized");
    return;
  }

  if (text.empty()) {
    md_error("Invalid text: " + text);
    return;
  }

  int xkb_event_type;
  if (!XkbQueryExtension(display, nullptr, &xkb_event_type, nullptr, nullptr,
                         nullptr)) {
    md_error("Xkb extension not available");
    return;
  }

  XkbStateRec state;
  XkbGetState(display, XkbUseCoreKbd, &state);

  for (char ch : text) {
    KeySym key_sym;
    if (special_char_map.find(ch) != special_char_map.end()) {
      key_sym = special_char_map[ch];
    } else {
      char str[2] = {ch, '\0'};
      key_sym = XStringToKeysym(str);
    }

    if (key_sym == NoSymbol) {
      md_error("Unknown key: " + std::string(1, ch));
    } else {
      bool shift = false;
      KeySym lower_sym =
          XkbKeycodeToKeysym(display, XKeysymToKeycode(display, key_sym), 0, 0);
      KeySym upper_sym =
          XkbKeycodeToKeysym(display, XKeysymToKeycode(display, key_sym), 0, 1);

      if (upper_sym == key_sym && lower_sym != key_sym) {
        shift = true;
      }

      if (shift) {
        simulate_key_press(XK_Shift_L);
      }

      simulate_key_press(key_sym);
      simulate_key_release(key_sym);

      if (shift) {
        simulate_key_release(XK_Shift_L);
      }
    }
  }
}

void volume_inc(int amount) {
  if (!elem) {
    md_error("Master volume control is not initialized");
    return;
  }

  long min, max;
  snd_mixer_selem_get_playback_volume_range(elem, &min, &max);

  long volume;
  snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_FRONT_LEFT, &volume);

  float amount_f = std::clamp(static_cast<float>(amount) / 100.0f, 0.0f, 1.0f);

  volume += (max - min) * amount_f;
  if (volume > max)
    volume = max;

  snd_mixer_selem_set_playback_volume_all(elem, volume);
}

void volume_dec(int amount) {
  if (!elem) {
    md_error("Master volume control is not initialized");
    return;
  }

  long min, max;
  snd_mixer_selem_get_playback_volume_range(elem, &min, &max);

  long volume;
  snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_FRONT_LEFT, &volume);

  float amount_f = std::clamp(static_cast<float>(amount) / 100.0f, 0.0f, 1.0f);

  volume -= (max - min) * amount_f;
  if (volume > max)
    volume = max;

  snd_mixer_selem_set_playback_volume_all(elem, volume);
}

void volume_set(int amount) {
  if (!elem) {
    md_error("Master volume control is not initialized");
    return;
  }

  long min, max;
  snd_mixer_selem_get_playback_volume_range(elem, &min, &max);

  float amount_f = std::clamp(static_cast<float>(amount) / 100.0f, 0.0f, 1.0f);

  long volume = min + static_cast<long>(std::round((max - min) * amount_f));

  snd_mixer_selem_set_playback_volume_all(elem, volume);
}

void volume_mute() {
  if (!elem) {
    md_error("Master volume control is not initialized");
    return;
  }

  snd_mixer_selem_set_playback_switch_all(elem, 0);
}

void volume_unmute() {
  if (!elem) {
    md_error("Master volume control is not initialized");
    return;
  }

  snd_mixer_selem_set_playback_switch_all(elem, 1);
}

void volume_toggle() {
  if (!elem) {
    md_error("Master volume control is not initialized");
    return;
  }

  int mute_state;
  snd_mixer_selem_get_playback_switch(elem, SND_MIXER_SCHN_FRONT_LEFT,
                                      &mute_state);
  snd_mixer_selem_set_playback_switch_all(elem, !mute_state);
}
