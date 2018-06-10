#ifndef CIRCULARBUFFER_H_
#define CIRCULARBUFFER_H_

#include <msp430.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct CircularBuffer {
	char *buffer;
	size_t size;
	int readPointer;
	int writePointer;
} CircularBuffer;

CircularBuffer *circularBufferInit(size_t size, char *bufBuf);
size_t circularBufferRead(CircularBuffer *self, char *data, size_t length);
size_t circularBufferWrite(CircularBuffer *self, char *data, size_t length);
size_t freeSpace(CircularBuffer *self);

#endif /* CIRCULARBUFFER_H_ */
