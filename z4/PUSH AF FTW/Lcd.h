#include <msp430.h>

typedef struct Lcd {
	int nextDigit;
	char digits[8];
} Lcd;

Lcd *lcdInit();
void lcdUpdateDigit(Lcd *lcd, int digit);
