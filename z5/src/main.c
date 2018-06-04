#include <msp430.h>
#include <stdint.h>
#include "CircularBuffer.h"
#include <ctype.h>

#define CTS BIT2
#define TxD BIT4
#define RxD BIT5

#define BUFFER_SIZE 128
#define BUFFER_MARGIN 64

char txBufBuf[BUFFER_SIZE];
char rxBufBuf[BUFFER_SIZE];

CircularBuffer *rxBuf;
CircularBuffer *txBuf;

void send(char *data, size_t length);

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    // Konfiguracja pinow
    P5DIR |= BIT4;
    P5SEL |= BIT4;
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

    // Przelaczane MCLK na rezonator kwarcowy
 	BCSCTL1 |= XTS;
 	BCSCTL2 |= SELM_2;
 	IFG1 &= ~OFIFG;

    ME1 = UTXE0 | URXE0;
    IE1 = UTXIE0 | URXIE0;
    IFG1 &= ~UTXIFG0;


    IFG1 &= ~OFIFG;

    rxBuf = circularBufferInit(BUFFER_SIZE, txBufBuf);
    txBuf = circularBufferInit(BUFFER_SIZE, rxBufBuf);


//    // DMA Init
//    DMACTL1 |= DMAONFETCH;
//
//    // Kanal 0 DMA - odbiornik
//    DMACTL0 |= DMA0TSEL_3;
//    DMA0CTL |= DMADT_4 | DMADSTINCR_0 | DMASRCINCR_0 | DMADSTBYTE | DMASRCBYTE | DMAEN | DMAIE// Single repeated, albo DMADT_0 Single
//    DMA0SA = U0RXBUF;
//    DMA0DA = rxBuf->write;
//    DMA0SZ = 0x01;
//
//    // Kanal 1 DMA - nadajnik
//    DMACTL0 |= DMA1TSEL_4;
//    DMA1CTL |= DMADT_4 | DMADSTINCR_0 | DMASRCINCR_0 | DMADSTBYTE | DMASRCBYTE | DMAEN | DMAIE// Single repeated DMADT_4 , albo Single DMADT_0
//    DMA1SA = rxBuf->read;
//    DMA1DA = U0TXBUF;
//    DMA1SZ = 0x01;

    __bis_SR_register(GIE);

    char last[] = "\00\00\00";

    while (1) {
    	if (!circularBufferRead1(rxBuf, last+2, 1)) {
    		if (freeSpace1(rxBuf) == rxBuf->size) {
    			P1OUT &= ~CTS;
    		}
			continue;
    	}

//    // Poczatek pliku lub nowa linia
//		if (last[1] == '\00') {
//			last[2] = (char)toupper(last[2]);
//			last[0] = last[1];
//			last[1] = last[2];
//			continue;
//		}
//
//		// Usuwanie powtorzonych znakow
//		if (toupper(last[1]) == toupper(last[2]) && last[2] != '\015') {
//			continue;
//		}
//
//		// Wykrywanie spacji przed kropka i innymi takimi
//		if (last[1] == ' ' && (last[2] == '.'|| last[2] == ',' || last[2] == ':' || last[2] == ';' || last[2] == ')' || last[2] == '!' || last[2] == '?')){
//			last[1]=last[2];
//			continue;
//		}
//
//		// Spacja po kropce
//		if((last[1] == '.' || last[1] == ',' || last[1] == ':' || last[1] == ';' || last[1] == ')' || last[1] == '!' || last[1] == '?') && last[2] != ' '){
//			 send(last + 1, 1);
//		    	int i = 0;
//		    	last[0] = last[1];
//		    	last[1]= ' ';
//		}
//
//		// Spacja przed otwarciem nawiasu
//		if (last[2] == '(' && last[1] != ' '){
//			send(last + 1, 1);
//			int i = 0;
//			last[0] = last[1];
//			last[1]= ' ';
//		}
//
//		// Duzy znak po spacji i kropce
//		if (last[1] == ' ' &&  last[0] == '.') {
//			last[2] = (char)toupper(last[2]);
//		}
//
//		// Znak nowej linii
//		if (last[2] == '\015') {
//			send(last+1, 1);
//			last[1] = 10;
//			send(last+1, 2);
//			last[1]='\00';
//			last[2]='\00';
//			continue;
//		}

		send(last + 1, 1);
    	int i = 0;
    	for (i; i < 3 - 1; i++) {
    		last[i] = last[i + 1];
    	}
    }
}

// Funkcja wpisuje dane do bufora nadajnika, a takze inicjalizuje transmisje jesli nie jest zainicjalizowana
void send(char *data, size_t length) {

	circularBufferWrite1(txBuf, data, length);
	if (U0TCTL & TXEPT) {
		circularBufferRead1(txBuf, (char *) &TXBUF0, 1);
		U0TCTL |= UTXE0;
	}
  if (freeSpace1(txBuf) == BUFFER_MARGIN) {
		P1OUT |= CTS;
	}
}

//#pragma vector=DACDMA_VECTOR
//__interrupt void DMA (void){
//	if(DMA0CTL & DMAIFG){
//
//		if (freeSpace(txBuf) <= 1) {
//			P1OUT &= ~CTS;
//		}
//	} else if (DMA1CTL & DMAIFG) {
//	}
//}

#pragma vector=USART0RX_VECTOR
__interrupt void USART0_RX (void) {  // Przerwania odbiornika
	char received = RXBUF0;
	circularBufferWrite1(rxBuf, &received, 1);
	if (freeSpace1(rxBuf) == BUFFER_MARGIN) {
		P1OUT |= CTS;
	}
}

#pragma vector=USART0TX_VECTOR
__interrupt void USART0_TX (void) {  // Przerwania nadajnika
	U0TCTL &= ~UTXE0;
	if (circularBufferRead1(txBuf, (char *) &TXBUF0, 1)) {
		U0TCTL |= UTXE0;
	}
}
