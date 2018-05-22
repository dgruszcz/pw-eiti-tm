#include "CtdnTimer.h"

CtdnTimer *ctdnTimerInit() {
	CtdnTimer *ctdnTimer = malloc(sizeof(CtdnTimer));
	ctdnTimer->seconds = 0;
	return ctdnTimer;
}

void ctdnTimerDecrement(CtdnTimer *ctdnTimer) {
	if (ctdnTimer->seconds != 0) {
		ctdnTimer->seconds--;
	}
}

void ctdnTimerSetSeconds(CtdnTimer *ctdnTimer, uint16_t seconds) {
	if (seconds <= 5999) {
		ctdnTimer->seconds = seconds;
	} else {
		ctdnTimer->seconds = 0;
	}
}
