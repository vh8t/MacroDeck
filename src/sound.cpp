#include "sound.hpp"
#include "log.hpp"

#include <algorithm>
#include <alsa/asoundlib.h>
#include <cmath>

snd_mixer_t *out_mixer = nullptr;
snd_mixer_selem_id_t *out_sid = nullptr;
snd_mixer_elem_t *out_elem = nullptr;

snd_mixer_t *in_mixer = nullptr;
snd_mixer_selem_id_t *in_sid = nullptr;
snd_mixer_elem_t *in_elem = nullptr;

void init_alsa() {
  snd_mixer_open(&out_mixer, 0);
  snd_mixer_attach(out_mixer, "default");
  snd_mixer_selem_register(out_mixer, nullptr, nullptr);
  snd_mixer_load(out_mixer);

  snd_mixer_open(&in_mixer, 0);
  snd_mixer_attach(in_mixer, "default");
  snd_mixer_selem_register(in_mixer, nullptr, nullptr);
  snd_mixer_load(in_mixer);

  snd_mixer_selem_id_alloca(&out_sid);
  snd_mixer_selem_id_set_index(out_sid, 0);
  snd_mixer_selem_id_set_name(out_sid, "Master");

  snd_mixer_selem_id_alloca(&in_sid);
  snd_mixer_selem_id_set_index(in_sid, 0);
  snd_mixer_selem_id_set_name(in_sid, "Capture");

  out_elem = snd_mixer_find_selem(out_mixer, out_sid);
  if (!out_elem) {
    error("Unable to find master volume control");
  }

  in_elem = snd_mixer_find_selem(in_mixer, in_sid);
  if (!in_elem) {
    error("Unable to find master capture control");
  }
}

void clean_alsa() {
  if (out_mixer) {
    snd_mixer_close(out_mixer);
  }
  if (in_mixer) {
    snd_mixer_close(in_mixer);
  }
}

void volume_inc(int amount) {
  if (!out_elem) {
    error("Master volume control is not initialized");
    return;
  }

  long min, max;
  snd_mixer_selem_get_playback_volume_range(out_elem, &min, &max);

  long volume;
  snd_mixer_selem_get_playback_volume(out_elem, SND_MIXER_SCHN_FRONT_LEFT,
                                      &volume);

  float amount_f = std::clamp(static_cast<float>(amount) / 100.0f, 0.0f, 1.0f);

  volume += (max - min) * amount_f;
  if (volume > max)
    volume = max;

  snd_mixer_selem_set_playback_volume_all(out_elem, volume);
}

void volume_dec(int amount) {
  if (!out_elem) {
    error("Master volume control is not initialized");
    return;
  }

  long min, max;
  snd_mixer_selem_get_playback_volume_range(out_elem, &min, &max);

  long volume;
  snd_mixer_selem_get_playback_volume(out_elem, SND_MIXER_SCHN_FRONT_LEFT,
                                      &volume);

  float amount_f = std::clamp(static_cast<float>(amount) / 100.0f, 0.0f, 1.0f);

  volume -= (max - min) * amount_f;
  if (volume > max)
    volume = max;

  snd_mixer_selem_set_playback_volume_all(out_elem, volume);
}

void volume_set(int amount) {
  if (!out_elem) {
    error("Master volume control is not initialized");
    return;
  }

  long min, max;
  snd_mixer_selem_get_playback_volume_range(out_elem, &min, &max);

  float amount_f = std::clamp(static_cast<float>(amount) / 100.0f, 0.0f, 1.0f);

  long volume = min + static_cast<long>(std::round((max - min) * amount_f));

  snd_mixer_selem_set_playback_volume_all(out_elem, volume);
}

void volume_mute() {
  if (!out_elem) {
    error("Master volume control is not initialized");
    return;
  }

  snd_mixer_selem_set_playback_switch_all(out_elem, 0);
}

void volume_unmute() {
  if (!out_elem) {
    error("Master volume control is not initialized");
    return;
  }

  snd_mixer_selem_set_playback_switch_all(out_elem, 1);
}

void volume_toggle() {
  if (!out_elem) {
    error("Master volume control is not initialized");
    return;
  }

  int mute_state;
  snd_mixer_selem_get_playback_switch(out_elem, SND_MIXER_SCHN_FRONT_LEFT,
                                      &mute_state);
  snd_mixer_selem_set_playback_switch_all(out_elem, !mute_state);
}

void capture_inc(int amount) {
  if (!in_elem) {
    error("Master capture control is not initialized");
    return;
  }

  long min, max;
  snd_mixer_selem_get_capture_volume_range(in_elem, &min, &max);

  long volume;
  snd_mixer_selem_get_capture_volume(in_elem, SND_MIXER_SCHN_FRONT_LEFT,
                                     &volume);

  float amount_f = std::clamp(static_cast<float>(amount) / 100.0f, 0.0f, 1.0f);

  volume += (max - min) * amount_f;
  if (volume > max)
    volume = max;

  snd_mixer_selem_set_capture_volume_all(in_elem, volume);
}

void capture_dec(int amount) {
  if (!in_elem) {
    error("Master capture control is not initialized");
    return;
  }

  long min, max;
  snd_mixer_selem_get_capture_volume_range(in_elem, &min, &max);

  long volume;
  snd_mixer_selem_get_capture_volume(in_elem, SND_MIXER_SCHN_FRONT_LEFT,
                                     &volume);

  float amount_f = std::clamp(static_cast<float>(amount) / 100.0f, 0.0f, 1.0f);

  volume -= (max - min) * amount_f;
  if (volume > max)
    volume = max;

  snd_mixer_selem_set_capture_volume_all(in_elem, volume);
}

void capture_set(int amount) {
  if (!in_elem) {
    error("Master capture control is not initialized");
    return;
  }

  long min, max;
  snd_mixer_selem_get_capture_volume_range(in_elem, &min, &max);

  float amount_f = std::clamp(static_cast<float>(amount) / 100.0f, 0.0f, 1.0f);

  long volume = min + static_cast<long>(std::round((max - min) * amount_f));

  snd_mixer_selem_set_capture_volume_all(in_elem, volume);
}

void capture_mute() {
  if (!in_elem) {
    error("Master capture control is not initialized");
    return;
  }

  snd_mixer_selem_set_capture_switch_all(in_elem, 0);
}

void capture_unmute() {
  if (!in_elem) {
    error("Master capture control is not initialized");
    return;
  }

  snd_mixer_selem_set_capture_switch_all(in_elem, 1);
}

void capture_toggle() {
  if (!in_elem) {
    error("Master capture control is not initialized");
    return;
  }

  int mute_state;
  snd_mixer_selem_get_capture_switch(in_elem, SND_MIXER_SCHN_FRONT_LEFT,
                                     &mute_state);
  snd_mixer_selem_set_capture_switch_all(in_elem, !mute_state);
}
