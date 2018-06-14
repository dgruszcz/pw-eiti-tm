#ifndef MEASBUFFER_H_
#define MEASBUFFER_H_

#include <msp430.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct MeasBuffer {
  uint16_t *buffer;
  size_t size;
  uint16_t mean, max;
} MeasBuffer;

MeasBuffer *
measBufferInit(size_t size, uint16_t *bufBuf);

void measBufferUpdateMax(MeasBuffer *self);

void measBufferUpdateMean(MeasBuffer *self);

#endif /* MEASBUFFER_H_ */
