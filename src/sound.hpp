#pragma once

void init_alsa();
void clean_alsa();

void volume_inc(int amount);
void volume_dec(int amount);
void volume_set(int amount);
void volume_mute();
void volume_unmute();
void volume_toggle();

void capture_inc(int amount);
void capture_dec(int amount);
void capture_set(int amount);
void capture_mute();
void capture_unmute();
void capture_toggle();
