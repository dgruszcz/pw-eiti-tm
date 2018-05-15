#include "CtdnTimer.h"

CtdnTimer *ctdnTimerInit() {
	CtdnTimer *ctdnTimer = malloc(sizeof(CtdnTimer));
	ctdnTimer->seconds = 0;
	return ctdnTimer;
}
