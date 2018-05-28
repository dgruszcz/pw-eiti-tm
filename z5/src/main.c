#include <msp430.h> 
#include <stdint.h>
#include "CircularBuffer.h"

CircularBuffer *rxBuf;
CircularBuffer *txBuf;

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

    rxBuf = circularBufferInit(128);
    txBuf = circularBufferInit(128);

    __bis_SR_register(GIE);

    while (1) {

    }
}

#pragma vector=USART0RX_VECTOR
__interrupt void USART0_RX (void) {
	char *received = malloc(sizeof(char));
	*received = RXBUF0;
	circularBufferWrite(rxBuf, received, 1);
	TXBUF0 = *received;
	if (TXEPT) {
//		circularBufferRead(rxBuf, (char *) received, 1);

	}
	free(received);
}

#pragma vector=USART0TX_VECTOR
__interrupt void USART0_TX (void) {
//	if (TXEPT) {
//		circularBufferRead(rxBuf, (char *) &TXBUF0, 1);
//	}
}
