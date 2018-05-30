#include <msp430.h> 
#include <stdint.h>
#include "CircularBuffer.h"
#include <ctype.h>

CircularBuffer *rxBuf;
CircularBuffer *txBuf;


void send(char *data, size_t length);

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    P1DIR |= BIT4;
    P1SEL |= BIT4;

    P3DIR |= BIT4;
    P3SEL |= BIT5 | BIT4;

	BCSCTL1 = RSEL2 | RSEL0;
	DCOCTL = DCO0;

    U0CTL = CHAR;
    U0TCTL = SSEL1 | SSEL0;
    U0RCTL = URXEIE;
    U0BR1 = 0;
    U0BR0 = 0x09;
    U0MCTL = 0x08;
    ME1 = UTXE0 | URXE0;
    IE1 = UTXIE0 | URXIE0;
    IFG1 &= ~UTXIFG0;

    rxBuf = circularBufferInit(5);
    txBuf = circularBufferInit(5);

    __bis_SR_register(GIE);

    char last[4] = "\00\00\00\00";

    while (1) {
    	if (!circularBufferRead(rxBuf, last+3, 1)) {
			continue;
    	}

 		char current = last[2];
		if (current == '\00') {
			int i = 0;
			for (i; i < 4 - 1; i++) {
				last[i] = last[i + 1];
			}
			continue;
		}

		// Usuwanie podwojnych liter
		if (last[2] == last[3]) {
			continue;
		}

		// Wykrywanie \s + "."
		if (last[2] == ' ' && last[3] == '.') {
			last[2]=last[3];
			continue;
		}

		// Spacja po kropce
		if(last[2] == '.' && last[3] != ' '){
			 send(last + 2, 1);
		    	int i = 0;
		    	for (i; i < 2; i++) {
		    		last[i] = last[i + 1];
		    	}
		    	last[2]= ' ';
		}

		// Duzy znak po spacji
		if (last[2] == ' ') {
			last[3] = (char)toupper(last[3]);
		}

		send(last + 2, 1);

    	int i = 0;
    	for (i; i < 4 - 1; i++) {
    		last[i] = last[i + 1];
    	}
    }
}

void send(char *data, size_t length) {
	circularBufferWrite(txBuf, data, length);
	// TODO
	if (U0TCTL & TXEPT) {
		circularBufferRead(txBuf, (char *) &TXBUF0, 1);
		U0TCTL |= UTXE0;
	}
}

#pragma vector=USART0RX_VECTOR
__interrupt void USART0_RX (void) {
	circularBufferWrite(rxBuf, (char *) &RXBUF0, 1);
}

#pragma vector=USART0TX_VECTOR
__interrupt void USART0_TX (void) {
	U0TCTL &= ~UTXE0;
	// TODO
	if (circularBufferRead(txBuf, (char *) &TXBUF0, 1)) {
		U0TCTL |= UTXE0;
	}
}
