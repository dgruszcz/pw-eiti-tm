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

void InitUART ();
void InitDMA ();

CircularBuffer *rxBuf;
CircularBuffer *txBuf;

void send(char *data, size_t length);

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    // Konfiguracja pinow
    P1DIR |= CTS;
    P3DIR |= TxD;
    P3SEL |= RxD | TxD;

    InitUART();

    // Przelaczane MCLK na XT2CLK
 	BCSCTL1 |= XTS;
 	BCSCTL2 |= SELM_2;
 	int i;
 	do{
 		IFG1 &= ~OFIFG;
 		for(i = 0; i < 0xFF; ++i);
 	} while (IFG1 & OFIFG);

    rxBuf = circularBufferInit(BUFFER_SIZE, txBufBuf);
    txBuf = circularBufferInit(BUFFER_SIZE, rxBufBuf);

    InitDMA();

    __bis_SR_register(GIE);

    char last[] = "\00\00\00";

    while (1) {
    	if (freeSpace(rxBuf) == BUFFER_SIZE && freeSpace(txBuf) == BUFFER_SIZE) {
    		if (freeSpace(rxBuf) == rxBuf->size) {
    			P1OUT &= ~CTS;
    		}
    		__bis_SR_register(LPM1_bits);
    	}

    	DMA0CTL &= ~DMAEN;
    	if (!circularBufferRead(rxBuf, last+2, 1)) {
    		if (freeSpace(rxBuf) == rxBuf->size) {
    			P1OUT &= ~CTS;
    		}
    		DMA0CTL |= DMAEN;
			continue;
    	}
    	DMA0CTL |= DMAEN;

    // Poczatek pliku lub nowa linia
		if (last[1] == '\00') {
			if (last[2] == '\012' || last[2] == '\015'){
				send(last+2, 1);
				last[1]='\00';
				last[2]='\00';
				continue;
			}

			if (last[2] == ' ') {
				continue;
			}

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
		    	last[0] = last[1];
		    	last[1]= ' ';
		}

		// Spacja przed otwarciem nawiasu
		if (last[2] == '(' && last[1] != ' '){
			send(last + 1, 1);
			last[0] = last[1];
			last[1]= ' ';
		}

		// Duzy znak po spacji i kropce
		if (last[1] == ' ' &&  last[0] == '.') {
			last[2] = (char)toupper(last[2]);
		}

		// Znak nowej linii
		if (last[2] == '\012' || last[2] == '\015') {
			send(last+1, 2);
			last[1]='\00';
			last[2]='\00';
			continue;
		}

		// Maly znak w srodku zdania
		if (last[0] != '.' && last[1] != ' '){
			last[2]=tolower(last[2]);
		}

		send(last + 1, 1);
		last[0] = last[1];
		last[1] = last[2];
    }
}

void InitUART (){
    // Konfiguracji zegara oraz UARTa
    BCSCTL1 = RSEL2 | RSEL0;
	DCOCTL = DCO0;
    U0CTL = CHAR;
    U0TCTL = SSEL1 | SSEL0 | TXEPT;
    U0BR1 = 0;
    U0BR0 = 0x09;
    U0MCTL = 0x08;

    ME1 = URXE0;
    IFG1 &= ~UTXIFG0;
}

void InitDMA (){
    // DMA Init
    DMACTL1 |= DMAONFETCH;

    // Kanal 0 DMA - odbiornik
    DMA0SA = &U0RXBUF;
    DMA0DA = rxBuf->buffer;
    DMA0SZ = 0x01;
    DMACTL0 |= DMA0TSEL_3;
    DMA0CTL |= DMADT_0 | DMADSTBYTE | DMASRCBYTE| DMALEVEL | DMAEN | DMAIE;

    // Kanal 1 DMA - nadajnik
    DMA1SA = txBuf->buffer;
    DMA1DA = &U0TXBUF;
    DMA1SZ = 0x01;
    DMACTL0 |= DMA1TSEL_0;
    DMA1CTL |= DMADT_0 | DMADSTBYTE | DMASRCBYTE | DMALEVEL | DMAEN | DMAIE;

}
// Funkcja wpisuje dane do bufora nadajnika, a takze inicjalizuje transmisje jesli nie jest zainicjalizowana
void send(char *data, size_t length) {

	DMA1CTL &= ~DMAEN;
	circularBufferWrite(txBuf, data, length);
	DMA1CTL |= DMAEN;

	if (((DMACTL0 & 240) == 0) && (U0TCTL & TXEPT)) {
		DMA1CTL |= DMAEN;
		DMA1CTL |= DMAREQ;
	}
	if (freeSpace(txBuf) == BUFFER_MARGIN) {
		P1OUT |= CTS;
	}
}


#pragma vector=DACDMA_VECTOR
__interrupt void DMA (void) {
	if(DMA0CTL & DMAIFG){  // Przerwania odbiornika
		if(freeSpace(rxBuf) > 0){
			++rxBuf->writePointer;
		}
		if(rxBuf->writePointer < BUFFER_SIZE) DMA0DA = rxBuf->buffer + rxBuf->writePointer;
		else DMA0DA = rxBuf->buffer + rxBuf->writePointer - BUFFER_SIZE;

		if (freeSpace(rxBuf) == BUFFER_MARGIN) {
			P1OUT |= CTS;
		}
		DMA0CTL &= ~DMAIFG;
		DMA0CTL |= DMAEN;
	} else if (DMA1CTL & DMAIFG) {  // Przerwania nadajnika
		ME1 |= UTXE0;
		++txBuf->readPointer;
		if(txBuf->readPointer < BUFFER_SIZE) DMA1SA = txBuf->buffer + txBuf->readPointer;
		else DMA1SA = txBuf->buffer + txBuf->readPointer - BUFFER_SIZE;


		if (txBuf->readPointer >= txBuf->size && txBuf->writePointer >= txBuf->size){
			txBuf->readPointer = txBuf->readPointer - txBuf->size;
			txBuf->writePointer = txBuf->writePointer - txBuf->size;
		}

		if(freeSpace(txBuf) == BUFFER_SIZE){
			DMACTL0 &= ~DMA1TSEL_15;
			DMACTL0 |= DMA1TSEL_0;
		} else {
			DMACTL0 &= ~DMA1TSEL_15;
			DMACTL0 |= DMA1TSEL_4;
		}
		DMA1CTL &= ~DMAREQ;
		DMA1CTL &= ~DMAIFG;
		ME1 &= ~UTXE0;
		DMA1CTL |= DMAEN;
	}
	LPM1_EXIT;
}
