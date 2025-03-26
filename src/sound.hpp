#pragma once

void init_alsa();
void clean_alsa();

void volume_inc(int amount);
void volume_dec(int amount);
void volume_set(int amount);
void volume_mute();
void volume_unmute();
void volume_toggle();
