#ifndef SYNTH_H_
#define SYNTH_H_

#include <stdint.h>
#include <stdlib.h>

typedef struct Synth {
  uint16_t *pitches;
  uint16_t *durations;
  uint16_t length;
  uint16_t index;
  uint8_t paused;
} Synth;

Synth *synthInit(uint16_t *pitches, uint16_t *durations,
                 uint16_t length);

// Generowanie interwalu dla kanalu obslugujacego wysokosc dzwieku
uint16_t synthUpdatePitch(Synth *synth);

// Generowanie interwalu dla kanalu obslugujacego dlugosc dzwieku
uint16_t synthUpdateDuration(Synth *synth);

void synthStop(Synth *synth);

void synthStart(Synth *synth);

#endif /* SYNTH_H_ */
