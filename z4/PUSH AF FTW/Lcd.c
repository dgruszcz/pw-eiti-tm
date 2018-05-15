#include "Lcd.h"

Lcd *lcdInit() {
	P4DIR = 0xff;	// segment selection
	P3DIR = 0xff;	// digit selection
    P3OUT = 0xff;
    P4OUT = 0xff;

    Lcd *lcd = malloc(sizeof(Lcd));
    lcd->nextDigit = 0;
    int i;
    for (i = 0; i < 8; i++) {
    	lcd->digits[i] = 0;
    }
    return lcd;
}

void lcdUpdateDigit(Lcd *lcd, int digit) {
	P3OUT = 0xff;

	if (digit == 2) {
		P4OUT = 112 + lcd->digits[digit];
	} else {
		P4OUT = 224 + lcd->digits[digit];
	}

	P3OUT = ~(1 << digit);
}
