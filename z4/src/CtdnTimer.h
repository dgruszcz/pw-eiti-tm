#ifndef CTDN_TIMER_H_
#define CTDN_TIMER_H_

#include <stdint.h>
#include <stdlib.h>

// Licznik sekund
typedef struct CtdnTimer {
  uint16_t seconds;
} CtdnTimer;

CtdnTimer *ctdnTimerInit();

// Metoda obslugujaca cykliczna dekrementacje licznika
uint8_t ctdnTimerDecrement(CtdnTimer *ctdnTimer);

// Metoda obslugujaca ustawianie licznika
void
ctdnTimerSetSeconds(CtdnTimer *ctdnTimer, int16_t seconds);

#endif /* CTDN_TIMER_H_ */
