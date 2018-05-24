#include <stdint.h>
#include <stdlib.h>

typedef struct CtdnTimer {
	uint16_t seconds;
} CtdnTimer;

CtdnTimer *ctdnTimerInit();
uint8_t ctdnTimerDecrement(CtdnTimer *ctdnTimer);
void ctdnTimerSetSeconds(CtdnTimer *ctdnTimer, int16_t seconds);
