#include <msp430.h> 
#include <stdio.h>
#include "hd44780.h"
#include "CircularBuffer.h"

#define BUFFER_SIZE 500

uint16_t measurementPending = 0;
uint8_t mode = 0;
uint8_t held = 0;
uint16_t bufferContents[BUFFER_SIZE];
CircularBuffer *buffer;

int main(void) {
	WDTCTL = WDTPW | WDTHOLD;

	P1DIR = 0xFF;
	P2DIR = (0x01 | 0x02);
	P6SEL |= BIT0;

	TACCR1 = 0x9E8;
	TACCTL1 = OUTMOD_4 | CCIE;
	TACTL = TASSEL_2 | MC_2 | TACLR;

	ADC12CTL0 = ADC12ON | ADC12SC;
	ADC12CTL1 = SHP | SHS_1 | CONSEQ_2;
	ADC12MCTL0 |= EOS;
	ADC12IE |= BIT0;
	
	FCTL2 |= FWKEY + FSSEL_1;

	buffer = circularBufferInit(BUFFER_SIZE, bufferContents);
	hd44780_clear_screen();

	__bis_SR_register(GIE);

	ADC12CTL0 |= ENC;

	while (1) {
		LPM1;
		if (measurementPending) {
			circularBufferWrite(buffer, ADC12MEM0);
			circularBufferUpdateMax(buffer);
			circularBufferUpdateMean(buffer);
			char bar[16] = "                ";
			uint8_t bars = mode ? buffer->max : buffer->mean * 17L / 4096L;
			int i;
			for (i = 0; i < bars; i++) {
				bar[i] = '\xFF';
			}
			hd44780_write_string(bar, 2, 1, NO_CR_LF);
			hd44780_timer_isr();
			measurementPending = 0;
		}
	}
}

#pragma vector = TIMERA1_VECTOR
__interrupt void timer_0_a1_isr( void ) {
	held = (~P3IN & BIT0) ? held + 1 : 0;
	if (held == 10) {
		mode = mode ? 0 : 1;
		FCTL3 = FWKEY;
		FCTL1 = FWKEY + WRT;
		
		*(0x1943) = mode;
		FCTL1 = FWKEY;
		FCTL3 = FWKEY + LOCK;
	}
	TACCR1 += 1000;
	TACCTL1 &= ~CCIFG;
}

#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR(void) {
	measurementPending = 1;
	ADC12IFG = 0;
	LPM1_EXIT;
}
