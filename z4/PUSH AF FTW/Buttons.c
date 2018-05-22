#include "Buttons.h"

Buttons *buttonsInit() {
	Buttons *buttons = malloc((size_t) sizeof(Buttons));
	buttons->states[0] = 0;
	buttons->states[1] = 0;
	buttons->held = 0;

	return buttons;
}

void buttonsReset(Buttons *buttons) {
	buttons->statesPending[0] = 0;
	buttons->statesPending[1] = 0;
}
