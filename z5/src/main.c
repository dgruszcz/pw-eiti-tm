#include <msp430.h>
#include <stdint.h>
#include "CircularBuffer.h"
#include <ctype.h>

#define CTS BIT2
#define TxD BIT4
#define RxD BIT5

CircularBuffer *rxBuf;
CircularBuffer *txBuf;

void send(char *data, size_t length);

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
    // Konfiguracja pinow
    P1DIR |= CTS;
    P3DIR |= TxD;
    P3SEL |= RxD | TxD;
  // Konfiguracji zegara oraz UARTa
  BCSCTL1 = RSEL2 | RSEL0;
	DCOCTL = DCO0;
    U0CTL = CHAR;
    U0TCTL = SSEL1 | SSEL0 | TXEPT;
    U0RCTL = URXEIE;
    U0BR1 = 0;
    U0BR0 = 0x09;
    U0MCTL = 0x08;

    ME1 = UTXE0 | URXE0;
    IE1 = UTXIE0 | URXIE0;
    IFG1 &= ~UTXIFG0;

  // Przelaczane MCLK na rezonator kwarcowy
	BCSCTL1 |= XTS | XT2OFF;
	BCSCTL2 |= SELM_2;

    rxBuf = circularBufferInit(22);
    txBuf = circularBufferInit(22);

    __bis_SR_register(GIE);

    char last[] = "\00\00\00";

    while (1) {
    	__bic_SR_register(GIE);
    	if (!circularBufferRead(rxBuf, last+2, 1)) {
    		if (freeSpace(rxBuf) > 1) {
    			P1OUT |= CTS;
    		}
    		__bis_SR_register(GIE);
			continue;
    	}
    	__bis_SR_register(GIE);

    // Poczatek pliku lub nowa linia
		if (last[1] == '\00') {
			last[2] = (char)toupper(last[2]);
			last[0] = last[1];
			last[1] = last[2];
			continue;
		}

		// Usuwanie powtorzonych znakow
		if (toupper(last[1]) == toupper(last[2])) {
			continue;
		}

		// Wykrywanie spacji przed kropka i innymi takimi
		if (last[1] == ' ' && (last[2] == '.'|| last[2] == ',' || last[2] == ':' || last[2] == ';' || last[2] == ')' || last[2] == '!' || last[2] == '?')){
			last[1]=last[2];
			continue;
		}

		// Spacja po kropce
		if((last[1] == '.' || last[1] == ',' || last[1] == ':' || last[1] == ';' || last[1] == ')' || last[1] == '!' || last[1] == '?') && last[2] != ' '){
			 send(last + 1, 1);
		    	int i = 0;
		    	last[0] = last[1];
		    	last[1]= ' ';
		}

		// Spacja przed otwarciem nawiasu
		if (last[2] == '(' && last[1] != ' '){
			send(last + 1, 1);
			int i = 0;
			last[0] = last[1];
			last[1]= ' ';
		}

		// Duzy znak po spacji i kropce
		if (last[1] == ' ' &&  last[0] == '.') {
			last[2] = (char)toupper(last[2]);
		}

		// Znak nowej linii
		if (last[2] == '\015') {
			send(last+1, 1);
			last[1] = 10;
			send(last+1, 2);
			last[1]='\00';
			last[2]='\00';
			continue;
		}

		send(last + 1, 1);
    	int i = 0;
    	for (i; i < 3 - 1; i++) {
    		last[i] = last[i + 1];
    	}
    }
}

// Funkcja wpisuje dane do bufora nadajnika, a takze inicjalizuje transmisje jesli nie jest zainicjalizowana
void send(char *data, size_t length) {
	__bic_SR_register(GIE);
	circularBufferWrite(txBuf, data, length);
	if (BIT0 & TXEPT) {
		circularBufferRead(txBuf, (char *) &TXBUF0, 1);
		__bis_SR_register(GIE);
		U0TCTL |= UTXE0;
	}
  if (freeSpace(txBuf) =< 1) {
		P1OUT &= ~CTS;
	}
}

#pragma vector=USART0RX_VECTOR
__interrupt void USART0_RX (void) {  // Przerwania odbiornika
	char received = RXBUF0;
	circularBufferWrite(rxBuf, (char *) &received, 1);
	if (freeSpace(rxBuf) <= 1) {
		P1OUT &= ~CTS;
	}
}

#pragma vector=USART0TX_VECTOR
__interrupt void USART0_TX (void) {  // Przerwania nadajnika
	U0TCTL &= ~UTXE0;
	if (circularBufferRead(txBuf, (char *) &TXBUF0, 1)) {
		U0TCTL |= UTXE0;
	}
}
