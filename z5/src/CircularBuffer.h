#ifndef CIRCULARBUFFER_H_
#define CIRCULARBUFFER_H_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct CircularBuffer {
	char *buffer;
	size_t size;
	char *read;
	char *write;
} CircularBuffer;

CircularBuffer *circularBufferInit(size_t size);
size_t circularBufferRead(CircularBuffer *self, char *data, size_t length);
size_t circularBufferWrite(CircularBuffer *self, char *data, size_t length);
size_t freeSpace(CircularBuffer *self);

#endif /* CIRCULARBUFFER_H_ */
