#include <msp430.h>

typedef struct Lcd {
	int nextDigit;
	char digits[8];
} Lcd;

Lcd *lcdInit();
void lcdUpdateDigit(Lcd *lcd);
void lcdDisplaySeconds(Lcd *lcd, int seconds);
