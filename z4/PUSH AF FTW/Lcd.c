#include "Lcd.h"

Lcd *lcdInit(volatile unsigned char *segDir, volatile unsigned char *segOut, volatile unsigned char *digitDir, volatile unsigned char *digitOut) {
	*segDir = 0xff;	// segment selection
	*digitDir = 0xff;	// digit selection
    *segOut = 0xff;
    *digitOut = 0xff;

    Lcd *lcd = malloc((size_t) sizeof(Lcd));
    lcd->segDir = segDir;
    lcd->segOut = segOut;
    lcd->digitDir = digitDir;
	lcd->digitOut = digitOut;
    lcd->nextDigit = 0;
    int i;
    for (i = 0; i < 4; i++) {
    	lcd->digits[i] = 0;
    }
    return lcd;
}

void lcdUpdateDigit(Lcd *lcd, char isCounting) {
	*(lcd->digitOut) = 0xff;

	if (lcd->nextDigit == 2) {
		*(lcd->segOut) = 112 + lcd->digits[lcd->nextDigit];
	} else {
		*(lcd->segOut) = 224 + lcd->digits[lcd->nextDigit];
	}

	if (lcd->nextDigit == 4) {
		*(lcd->segOut) = 224 + (isCounting ? 14 : 10);
	}

	*(lcd->digitOut) = ~(1 << lcd->nextDigit);

	lcd->nextDigit = (lcd->nextDigit + 1) % 5;
}

void lcdDisplaySeconds(Lcd *lcd, int seconds) {
	lcd->digits[0] = seconds % 60 % 10;
	lcd->digits[1] = seconds % 60 / 10;
	lcd->digits[2] = (seconds / 60) % 10;
	lcd->digits[3] = (seconds / 60 / 10) % 10;
}
