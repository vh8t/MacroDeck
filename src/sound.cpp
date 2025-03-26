#include "log.hpp"

#include <algorithm>
#include <alsa/asoundlib.h>
#include <cmath>

snd_mixer_t *mixer = nullptr;
snd_mixer_selem_id_t *sid = nullptr;
snd_mixer_elem_t *elem = nullptr;

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
    error("Unable to find master volume control");
  }
}

void clean_alsa() {
  if (mixer) {
    snd_mixer_close(mixer);
  }
}

void volume_inc(int amount) {
  if (!elem) {
    error("Master volume control is not initialized");
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
    error("Master volume control is not initialized");
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
    error("Master volume control is not initialized");
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
    error("Master volume control is not initialized");
    return;
  }

  snd_mixer_selem_set_playback_switch_all(elem, 0);
}

void volume_unmute() {
  if (!elem) {
    error("Master volume control is not initialized");
    return;
  }

  snd_mixer_selem_set_playback_switch_all(elem, 1);
}

void volume_toggle() {
  if (!elem) {
    error("Master volume control is not initialized");
    return;
  }

  int mute_state;
  snd_mixer_selem_get_playback_switch(elem, SND_MIXER_SCHN_FRONT_LEFT,
                                      &mute_state);
  snd_mixer_selem_set_playback_switch_all(elem, !mute_state);
}
