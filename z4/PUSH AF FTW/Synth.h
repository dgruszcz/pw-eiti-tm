#ifndef SYNTH_H_
#define SYNTH_H_

#include <stdint.h>
#include <stdlib.h>

#define C2	500
#define Cs2	472
#define D2	446
#define Ds2	421
#define E2	397
#define F2	375
#define Fs2	354
#define G2	334
#define Gs2	315
#define A2	297
#define As2	281
#define B2	265
#define C3	250
#define Cs3	236
#define D3	223
#define Ds3	210
#define E3	198
#define F3	187
#define Fs3	177
#define G3	167
#define Gs3	157
#define A3	148
#define As3	140
#define B3	132
#define C4	125
#define Cs4	118
#define D4	111
#define Ds4	105
#define E4	99
#define F4	93
#define Fs4	88
#define G4	83
#define Gs4	78
#define A4	74
#define As4	70
#define B4	66
#define C5	62
#define Cs5	59
#define D5	55
#define Ds5	52
#define E5	49
#define F5	46
#define Fs5	44
#define G5	41
#define Gs5	39
#define A5	37
#define As5	35
#define B5	33
#define C6	31
#define Cs6	29
#define D6	27
#define Ds6	26
#define E6	24
#define F6	23
#define Fs6	22
#define G6	20
#define Gs6	19
#define A6	18
#define As6	17
#define B6	16
#define C7	15

// Durations @ 160 BPM @ 4/4
#define EIGTH 6144
#define QUARTER 12288
#define HALF 24576

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
void synthStop(Synth *synth);
void synthStart(Synth *synth);

#endif /* SYNTH_H_ */
