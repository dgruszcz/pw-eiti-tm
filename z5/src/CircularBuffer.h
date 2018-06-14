#ifndef CIRCULARBUFFER_H_
#define CIRCULARBUFFER_H_

#include <msp430.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct CircularBuffer {
	uint16_t *buffer;
	size_t size;
	int writePointer;
	uint16_t mean, max;
} CircularBuffer;

CircularBuffer *circularBufferInit(size_t size, uint16_t *bufBuf);
void circularBufferWrite(CircularBuffer *self, uint16_t data);
void circularBufferUpdateMax(CircularBuffer *self);
void circularBufferUpdateMean(CircularBuffer *self);

#endif /* CIRCULARBUFFER_H_ */
