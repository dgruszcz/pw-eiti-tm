#include "CircularBuffer.h"

CircularBuffer *circularBufferInit(size_t size, char *bufBuf) {
	CircularBuffer *circularBuffer = malloc((size_t) sizeof(CircularBuffer));
	circularBuffer->buffer = bufBuf;

	circularBuffer->size = size;
	circularBuffer->readPointer = 0;
	circularBuffer->writePointer = 0;

	return circularBuffer;
}

size_t freeSpace(CircularBuffer *self) {
	return self->size - (self->writePointer - self->readPointer);
}

size_t circularBufferRead(CircularBuffer *self, char *data, size_t length) {
	size_t read = 0;
	int dataLength = self->writePointer - self->readPointer; // Ilosc bajtow zapisanych w buforze

	dataLength = length > dataLength ? dataLength : length;  // Ilosc bajtow do oczytania
	read = dataLength;
	while (dataLength--){
		*(data++) = self->buffer[self->readPointer++ % self->size];
	}

	if (self->readPointer >= self->size && self->writePointer >= self->size){
		self->readPointer = self->readPointer - self->size;
		self->writePointer = self->writePointer - self->size;
	}
	return read;
}

size_t circularBufferWrite(CircularBuffer *self,char *data, size_t length) {
	size_t written = 0;
	int dataLength = self->size - (self->writePointer - self->readPointer); // Dostepne miejsce w buforze

	dataLength = length > dataLength ? dataLength : length;  // Ilosc bajtow mozliwych do zapisania
	written = dataLength;
	while (dataLength--){
		self->buffer[self->writePointer++ % self->size] = *(data++);
	}
	return written;
}
