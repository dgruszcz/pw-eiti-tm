#include <msp430.h>
#include <stdlib.h>

typedef struct Lcd {
	volatile unsigned char *segDir, *segOut, *digitDir, *digitOut;
	int nextDigit;
	char digits[8];
} Lcd;

Lcd *lcdInit(volatile unsigned char *segDir, volatile unsigned char *segOut, volatile unsigned char *digitDir, volatile unsigned char *digitOut);
void lcdUpdateDigit(Lcd *lcd);
void lcdDisplaySeconds(Lcd *lcd, int seconds);
