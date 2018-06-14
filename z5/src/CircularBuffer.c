#include "CircularBuffer.h"

CircularBuffer *circularBufferInit(size_t size, uint16_t *bufBuf) {
	CircularBuffer *circularBuffer = malloc((size_t) sizeof(CircularBuffer));
	circularBuffer->buffer = bufBuf;

	circularBuffer->size = size;
	circularBuffer->writePointer = 0;

	return circularBuffer;
}

void circularBufferWrite(CircularBuffer *self, uint16_t data) {
	self->buffer[self->writePointer++] = data;


	if (self->writePointer >= self->size) {
		self->writePointer = 0;

	}
}

void circularBufferUpdateMax(CircularBuffer *self) {
	self->max = self->buffer[0];
	int i;
	for (i = 1; i < self->size; ++i) {
		self->max = self->buffer[i] > self->max ? self->buffer[i] : self->max;
	}
}

void circularBufferUpdateMean(CircularBuffer *self) {
	uint32_t sum = self->buffer[0];
	int i;
	for (i = 1; i < self->size; ++i) {
		sum += self->buffer[i];

	}

	self->mean = sum / self->size;
}
