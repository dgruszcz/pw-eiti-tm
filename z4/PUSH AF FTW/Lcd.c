#include "Lcd.h"

Lcd *lcdInit() {
	P4DIR = 0xff;	// segment selection
	P3DIR = 0xff;	// digit selection
    P3OUT = 0xff;
    P4OUT = 0xff;

    Lcd *lcd = malloc(sizeof(Lcd));
    lcd->nextDigit = 0;
    int i;
    for (i = 0; i < 4; i++) {
    	lcd->digits[i] = 0;
    }
    return lcd;
}

void lcdUpdateDigit(Lcd *lcd) {
	P3OUT = 0xff;

	if (lcd->nextDigit == 2) {
		P4OUT = 112 + lcd->digits[lcd->nextDigit];
	} else {
		P4OUT = 224 + lcd->digits[lcd->nextDigit];
	}

	P3OUT = ~(1 << lcd->nextDigit);

	lcd->nextDigit = (lcd->nextDigit + 1) % 4;
}

void lcdDisplaySeconds(Lcd *lcd, int seconds) {
	lcd->digits[0] = seconds % 60 % 10;
	lcd->digits[1] = seconds % 60 / 10;
	lcd->digits[2] = (seconds / 60) % 10;
	lcd->digits[3] = (seconds / 60 / 10) % 10;
}
