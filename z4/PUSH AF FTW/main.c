#include <msp430.h>
#include "Lcd.h"

#define ONE_SECOND 32768

Lcd *lcd;

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    TACTL = TASSEL_1 | MC_1;
    TAR = 0;
    TACCTL0 = CCIE;
    TACCR0 = ONE_SECOND;
    TACCTL1 = CCIE;
    TACCR1 = ONE_SECOND / 60 / 8;

    lcd = lcdInit();
    lcd->digits[2] = 0;
    lcdUpdateDigit(lcd, 0);

    __bis_SR_register(GIE);

    while (1) { }
	
	return 0;
}

#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A0 (void) {
	lcd->digits[2]++;
	if (lcd->digits[2] > 9) {
		lcd->digits[2] = 0;
		lcd->digits[3]++;
	}
}

#pragma vector=TIMERA1_VECTOR
__interrupt void Timer_A1 (void) {
	if (TAIV == TAIV_TACCR1) {
		lcdUpdateDigit(lcd, lcd->nextDigit);
		lcd->nextDigit++;
		if (lcd->nextDigit > 7) {
			lcd->nextDigit = 0;
		}
		TACCR1 += ONE_SECOND / 60 / 8;
		TACCR1 = TACCR1 % (ONE_SECOND + 1);
	}
}
