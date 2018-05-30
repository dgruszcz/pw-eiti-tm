#include "CircularBuffer.h"

CircularBuffer *circularBufferInit(size_t size) {
	CircularBuffer *circularBuffer = malloc((size_t) sizeof(CircularBuffer));
	circularBuffer->buffer = malloc(size * sizeof(char));

	circularBuffer->size = size;
	circularBuffer->read = circularBuffer->buffer;
	circularBuffer->write = circularBuffer->buffer;

	return circularBuffer;
}

size_t circularBufferRead(CircularBuffer *self, char *data, size_t length) {
	size_t toRead;
	size_t read = 0;

	int dataLength = self->write - self->read;

	size_t phaseOne = dataLength >= 0 ? dataLength : self->buffer + self->size - self->read;
	toRead = length > phaseOne ? phaseOne : length;
	memcpy(data, self->read, toRead * sizeof(char));
	read += toRead;
	self->read += toRead;

	if (toRead < length && self->read != self->write) {
		self->read = self->buffer;
		toRead = self->write - self->read;
		toRead = length - read < toRead ? length - read : toRead;
		memcpy(data + read, self->read, toRead * sizeof(char));
		read += toRead;
		self->read += toRead;
	}

	return read;
}

size_t circularBufferWrite(CircularBuffer *self,char *data, size_t length) {
	size_t toWrite;
	size_t written = 0;

	int dataLength = self->read - self->write;

	size_t phaseOne = dataLength > 0 ? dataLength : self->buffer + self->size - self->write;
	toWrite = length > phaseOne ? phaseOne : length;
	memcpy(self->write, data, toWrite);
	written += toWrite;
	self->write += toWrite;

	if (self->write == self->buffer + self->size) self->write = self->buffer;

	if (toWrite < length && self->read != self->write) {
		toWrite = self->read - self->write;
		toWrite = length - written < toWrite ? length - written : toWrite;
		memcpy(self->write, data + written, toWrite);
		written += toWrite;
		self->write += toWrite;
	}

	return written;
}
