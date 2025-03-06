#pragma once

#include <string>

void init_x11();   // implemented
void clean_x11();  // implemented
void init_alsa();  // implemented
void clean_alsa(); // implemented

void app_open(const std::string &name);   //
void app_close(const std::string &name);  //
void app_switch(const std::string &name); //

void key_press(const std::string &combination);   // implemented
void key_release(const std::string &combination); // implemented
void key_click(const std::string &combination);   // implemented
void key_type(const std::string &text);           // implemented

void media_play();   //
void media_pause();  //
void media_toggle(); //
void media_next();   //
void media_prev();   //

void volume_inc(int amount); // implemented
void volume_dec(int amount); // implemented
void volume_set(int amount); // implemented
void volume_mute();          // implemented
void volume_unmute();        // implemented
void volume_toggle();        // implemented
