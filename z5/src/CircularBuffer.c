#include "CircularBuffer.h"

CircularBuffer *circularBufferInit(size_t size) {
	CircularBuffer *circularBuffer = malloc((size_t) sizeof(CircularBuffer));
	circularBuffer->buffer = malloc(size * sizeof(char));

	circularBuffer->size = size;
	circularBuffer->read = circularBuffer->buffer;
	circularBuffer->write = circularBuffer->buffer;
  circularBuffer->readPointer = 0;
  circularBuffer->writePointer = 0;

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

size_t freeSpace(CircularBuffer *self) {
	size_t space = self->read - self->write;
	return space > 0 ? space : self->size - space;
}
//------------------------------------------------------------------------------------------
// Funkcje w wersji testowej
//------------------------------------------------------------------------------------------

size_t freeSpace1(CircularBuffer *self) {
	return self->size - (self->writePointer - self->readPointer);
}

size_t circularBufferRead1(CircularBuffer *self, char *data, size_t length) {
	size_t read = 0;
	int dataLength = self->writePointer - self->readPointer; // Ilosc bajtow zapisanych w buforze

	dataLength = length > dataLength ? dataLength : length;  // Ilosc bajtow do oczytania
	read = dataLength;
	while (dataLength--){
    *(data++) = self->buffer[self->readPointer++ % self->size];
	}

	if (self->readPointer >= self->size && self->writePointer >= self->size){
    self->readPointer = self->readPointer % self->size;
    self->writePointer = self->writePointer % self->size;
	}

	return read;
}

size_t circularBufferWrite1(CircularBuffer *self,char *data, size_t length) {
	size_t written = 0;

	int dataLength = self->size - (self->writePointer - self->readPointer); // Dostepne miejsce w buforze

	dataLength = length > dataLength ? dataLength : length;  // Ilosc bajtow mozliwych do zapisania
	written = dataLength;
	while (dataLength--){
    self->buffer[self->writePointer++ % self->size] = *(data++);
	}
	return written;
}


