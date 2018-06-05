#include "Synth.h"

Synth *synthInit(uint16_t *pitches, uint16_t *durations,
                 uint16_t length) {
  Synth *synth = malloc(sizeof(Synth));
  synth->pitches = pitches;
  synth->durations = durations;
  synth->length = length;
  synth->index = 0;
  synth->paused = 1;

  return synth;
}

uint16_t synthUpdatePitch(Synth *synth) {
  return synth->pitches[synth->index - 1];
}

uint16_t synthUpdateDuration(Synth *synth) {
  if (synth->paused || synth->index >= synth->length) {
    synth->paused = 1;
    return 0;
  }

  return synth->durations[synth->index++];
}

void synthStop(Synth *synth) {
  synth->paused = 1;
}

void synthStart(Synth *synth) {
  synth->index = 0;
  synth->paused = 0;
}
