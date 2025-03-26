#pragma once

#include <string>

struct Key {
  int keycode;
  bool shift;
};

void init_keyboard();
void clean_keyboard();

void key_press(const std::string &combination);
void key_release(const std::string &combination);
void key_click(const std::string &combination);
void key_type(const std::string &text);
