#include <stdint.h>
#include <stdlib.h>

typedef struct CtdnTimer {
	uint16_t seconds;
} CtdnTimer;

CtdnTimer *ctdnTimerInit();
void ctdnTimerDecrement(CtdnTimer *ctdnTimer);
void ctdnTimerSetSeconds(CtdnTimer *ctdnTimer, int16_t seconds);
