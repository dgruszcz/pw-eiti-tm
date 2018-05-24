#ifndef SYNTH_H
#define SYNTH_H

#include <stdint.h>
#include <stdlib.h>

// Pitches TODO: calculate remaining pitches
#define C3  250
#define Cs3 0
#define D3  223
#define Ds3 0
#define E3  199
#define F3  188
#define Fs3 0
#define G3  167
#define Gs3 0
#define A3  0
#define As3 0
#define B3  0
#define C4  125
#define Cs4 0
#define D4  112
#define Ds4 0
#define E4  99
#define F4  94
#define Fs4 0
#define G4  84
#define Gs4 0
#define A4  0
#define As4 0
#define B4  0
#define C5  0

// Durations @ 120 BPM @ 4/4
#define EIGTH 8192
#define QUARTER 16384
#define HALF 32768

typedef struct Synth {
  uint16_t *pitches;
  uint16_t *durations;
  uint16_t length;
  uint16_t index;
  uint8_t paused;
} Synth;

Synth *synthInit(uint16_t *pitches, uint16_t *durations, uint16_t length);
uint16_t synthUpdatePitch(Synth *synth);
uint16_t synthUpdateDuration(Synth *synth);
uint16_t synthStop(Synth *synth);
uint16_t synthStart(Synth *synth);

#endif //SYNTH_H
