#include "MeasBuffer.h"

MeasBuffer *
circularBufferInit(size_t size, uint16_t *bufBuf) {
  MeasBuffer *measBuffer = malloc(
    (size_t) sizeof(MeasBuffer));
  measBuffer->buffer = bufBuf;

  measBuffer->size = size;

  return measBuffer;
}

void circularBufferUpdateMax(MeasBuffer *self) {
  self->max = self->buffer[0];
  int i;
  for (i = 1; i < self->size; ++i) {
    self->max =
      self->buffer[i] > self->max ? self->buffer[i]
                                  : self->max;
  }
}

void circularBufferUpdateMean(MeasBuffer *self) {
  uint32_t sum = self->buffer[0];
  int i;
  for (i = 1; i < self->size; ++i) {
    sum += self->buffer[i];
  }

  self->mean = sum / self->size;
}
