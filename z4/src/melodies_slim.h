#ifndef SONGS_H_
#define SONGS_H_

#define C2  500
#define Cs2  472
#define D2  446
// ...
#define As6  17
#define B6  16
#define C7  15

// Durations @ 160 BPM @ 4/4
#define EIGTH 6144
#define QUARTER 12288
#define HALF 24576

#define MEL_THELICK_P {A4, D5, E5, F5, G5, E5, C5, D5, D5}
#define MEL_THELICK_D {EIGTH, EIGTH, EIGTH, EIGTH, EIGTH, QUARTER, EIGTH, EIGTH, HALF}
#define MEL_THELICK_L 9

#define MEL_THELICKEX_P { \
    A4, \
    D5, E5, F5, G5, E5, C5, D5, \
    D5, 0, 0, A4, \
    D5, E5, F5, G5, E5, C5, D5, \
    D5, 0, 0, A4, \
    D5, E5, F5, G5, E5, C5, Cs5, \
    Cs5, Cs5, \
    C6, As5, F5, Cs5, C5, As4, G4, Gs4, \
    A4, 0, 0, A4, \
    D5, E5, F5, G5, A5, E5, F5, \
    F5, 0, 0, A4, \
    D5, E5, F5, G5, A5, E5, F5, \
    F5, 0, 0, A4, \
    D5, E5, F5, G5, E5, C5, Cs5, \
    Cs5, 0, 0, A4, \
    As4, Cs5, D5, E5, F5, E5, D5, \
    D5, D5 \
}
#define MEL_THELICKEX_D { \
    EIGTH, \
    EIGTH, EIGTH, EIGTH, EIGTH, QUARTER, EIGTH, EIGTH, \
    HALF, QUARTER, EIGTH, EIGTH, \
    EIGTH, EIGTH, EIGTH, EIGTH, QUARTER, EIGTH, EIGTH, \
    HALF, QUARTER, EIGTH, EIGTH, \
    EIGTH, EIGTH, EIGTH, EIGTH, QUARTER, EIGTH, EIGTH, \
    HALF, HALF, \
    EIGTH, EIGTH, EIGTH, EIGTH, EIGTH, EIGTH, EIGTH, EIGTH, \
    HALF, QUARTER, EIGTH, EIGTH, \
    EIGTH, EIGTH, EIGTH, EIGTH, QUARTER, EIGTH, EIGTH, \
    HALF, QUARTER, EIGTH, EIGTH, \
    EIGTH, EIGTH, EIGTH, EIGTH, QUARTER, EIGTH, EIGTH, \
    HALF, QUARTER, EIGTH, EIGTH, \
    EIGTH, EIGTH, EIGTH, EIGTH, QUARTER, EIGTH, EIGTH, \
    HALF, QUARTER, EIGTH, EIGTH, \
    EIGTH, EIGTH, EIGTH, EIGTH, QUARTER, EIGTH, EIGTH, \
    HALF, HALF \
}
#define MEL_THELICKEX_L 89

#endif /* SONGS_H_ */
