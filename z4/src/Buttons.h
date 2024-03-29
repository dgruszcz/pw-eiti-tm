#ifndef BUTTONS_H_
#define BUTTONS_H_

#include <stdint.h>
#include <stdlib.h>

// Stan przyciskow
typedef struct Buttons {
  uint8_t states[3];
  uint8_t statesPending[2];
  uint32_t held;
} Buttons;

Buttons *buttonsInit();

void buttonsReset(Buttons *buttons);

#endif /* BUTTONS_H_ */
