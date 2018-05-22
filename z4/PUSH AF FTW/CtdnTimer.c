#include "CtdnTimer.h"

CtdnTimer *ctdnTimerInit() {
	CtdnTimer *ctdnTimer = malloc((size_t) sizeof(CtdnTimer));
	ctdnTimer->seconds = 0;
	return ctdnTimer;
}

void ctdnTimerDecrement(CtdnTimer *ctdnTimer) {
	if (ctdnTimer->seconds != 0) {
		ctdnTimer->seconds--;
	}
}

void ctdnTimerSetSeconds(CtdnTimer *ctdnTimer, int16_t seconds) {
	if (seconds < 0) {
		ctdnTimer->seconds = 0;
	} else if (seconds > 5999) {
		ctdnTimer->seconds = 5999;
	} else {
		ctdnTimer->seconds = seconds;
	}
}
