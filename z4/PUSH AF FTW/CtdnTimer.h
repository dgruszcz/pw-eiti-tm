#include <stdint.h>

typedef struct CtdnTimer {
	uint16_t seconds;
} CtdnTimer;

CtdnTimer *ctdnTimerInit();
void ctdnTimerDecrement(CtdnTimer *ctdnTimer);
void ctdnTimerSetSeconds(CtdnTimer *ctdnTimer, uint16_t seconds);
