#include <msp430.h>
#include <stdio.h>
#include "hd44780.h"
#include "MeasBuffer.h"

#define BUFFER_SIZE 150

uint16_t measurementPending = 0;
uint8_t mode = 0;
uint8_t held = 0;
uint16_t bufferContents[BUFFER_SIZE];
MeasBuffer *buffer;
uint8_t *flashPointer = 0;
uint8_t wdtifg = 0;

void findFlashPointer();
void writeNonvolatile(uint8_t data);
void eraseNonvolatile();

int main(void) {
  wdtifg = IFG1 & WDTIFG;
  IFG1 &= ~WDTIFG;
  WDTCTL = WDTPW | WDTCNTCL | WDTSSEL;

  P1DIR = 0xFF;
  P2DIR = (0x01 | 0x02);
  P6SEL |= BIT0;

  TACTL = TASSEL_1 | MC_2 | TACLR;
  TACCTL1 = OUTMOD_4 | CCIE;
  TACCTL2 = CCIE;
  TACCR1 = 32768 / 50;
  TACCR2 = 32768 / 200;

  ADC12CTL0 = ADC12ON | ADC12SC;
  ADC12CTL1 = SHP | SHS_1 | CONSEQ_2;
  ADC12MCTL0 |= EOS;

  buffer = measBufferInit(BUFFER_SIZE, bufferContents);
  hd44780_clear_screen();

  // DMA Init
  DMACTL1 |= DMAONFETCH;

  // Kanal 0 DMA - odbiornik
  DMA0SA = &ADC12MEM0;
  DMA0DA = buffer->buffer;
  DMA0SZ = buffer->size;
  DMACTL0 |= DMA0TSEL_6;
  DMA0CTL |= DMADT_4 | DMADSTINCR_3 | DMALEVEL | DMAEN;

  __bis_SR_register(GIE);

  ADC12CTL0 |= ENC;
  findFlashPointer();

  WDTCTL = WDTPW | WDTCNTCL | WDTSSEL;

  while (1) {
    LPM1;
    if (measurementPending) {
      measBufferUpdateMax(buffer);
      measBufferUpdateMean(buffer);
      char info[] = "                ";
      char bar[] = "                ";
      sprintf(info, "%s  %d mV           ",
        mode ? "MAX" : "AVG",
        mode ? buffer->max * 3300L / 4096L :
        buffer->mean * 3300L / 4096L
      );
      if (wdtifg) {
        info[15] = 'W';
      }
      hd44780_write_string(info, 1, 1, NO_CR_LF);
      uint8_t bars = mode ? buffer->max * 17L / 4096L :
        buffer->mean * 17L / 4096L;
      int i;
      for (i = 0; i < bars; i++) {
        bar[i] = '\xFF';

      }
      hd44780_write_string(bar, 2, 1, NO_CR_LF);
      measurementPending = 0;
    }
    if (P3IN & BIT7) {
      WDTCTL = WDTPW | WDTCNTCL | WDTSSEL;
    }
  }
}

#pragma vector = TIMERA1_VECTOR
__interrupt void timer_0_a1_isr(void) {
  uint8_t reason = TAIV;
  if (reason == TAIV_TACCR1) {
    TACCR1 += 32768 / 50;
    measurementPending = 1;
    LPM1_EXIT;
  }
  if (reason == TAIV_TACCR2) {
    hd44780_timer_isr();
    held = (~P3IN & BIT0) ? held + 1 : 0;
    if (held == 10) {
      mode = mode ? 0 : 1;
      writeNonvolatile(mode);
    }
    TACCR2 += 32768 / 200;
  }
}

void findFlashPointer() {
  uint8_t *a = 0x01000;
  int i;
  for (i = 127; i >= 0; i--) {
    if ((*(a + i) & 0xFE) == 0xAC) {
      if (i == 127) {
        flashPointer = a;
        mode = *(a + i) & BIT0;
        eraseNonvolatile();
      } else {
        flashPointer = a + i + 1;
        mode = *(a + i) & BIT0;
      }
      break;
    }
  }
  if (!flashPointer) {
    eraseNonvolatile();
    flashPointer = a;
  }

}

void writeNonvolatile(uint8_t data) {
  if (!flashPointer) {
    return;
  }

  WDTCTL = WDTPW | WDTHOLD;
  __bic_SR_register(GIE);

  FCTL2 = FWKEY | FSSEL_1 | FN0;
  FCTL3 = FWKEY;
  FCTL1 = FWKEY | WRT;
  *(flashPointer) = 0xAC | (mode & BIT0);
  FCTL1 = FWKEY;
  FCTL3 = FWKEY | LOCK;
  flashPointer++;
  if (flashPointer > 0x0107F) {
    eraseNonvolatile();
    flashPointer = 0x01000;
    writeNonvolatile(mode);
  }

  __bis_SR_register(GIE);
  WDTCTL = WDTPW | WDTCNTCL | WDTSSEL;
}

void eraseNonvolatile() {
  WDTCTL = WDTPW | WDTHOLD;
  __bic_SR_register(GIE);

  FCTL2 = FWKEY | FSSEL_1 | FN0;
  FCTL3 = FWKEY;
  FCTL1 = FWKEY | ERASE;
  uint8_t *segment = 0x01000;
  *(segment) = 0;
  FCTL3 = FWKEY | LOCK;

  __bis_SR_register(GIE);
  WDTCTL = WDTPW | WDTCNTCL | WDTSSEL;
}


