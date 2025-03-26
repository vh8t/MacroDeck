#pragma once

#include <string>
#include <vector>

void app_open(const std::string &name,
              const std::vector<std::string> &args); // implemented
void app_close(const std::string &name);             // implemented
void app_switch(const std::string &name);            // implemented

void media_play();   //
void media_pause();  //
void media_toggle(); //
void media_next();   //
void media_prev();   //
