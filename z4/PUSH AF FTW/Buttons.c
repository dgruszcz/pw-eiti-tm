#include "Buttons.h"

Buttons *buttonsInit() {
	Buttons *buttons = malloc(sizeof(Buttons));
	buttons->states[0] = 0;
	buttons->states[1] = 0;
	buttons->held = 0;
}

void buttonsReset(Buttons *buttons) {
	buttons->states_pending[0] = 0;
	buttons->states_pending[1] = 0;
}
