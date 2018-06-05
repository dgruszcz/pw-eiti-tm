#ifndef LCD_H_
#define LCD_H_

#include <msp430.h>
#include <stdlib.h>

typedef struct Lcd {
  volatile unsigned char *segDir, *segOut
  volatile unsigned char *digitDir, *digitOut;
  int nextDigit;
  char digits[8];
} Lcd;

// Metoda inicjujaca wyswietlacz
// segDir 	-- Rejestr kierunku portu obslugujacego wyswietlanie segmentow
// segOut 	-- Rejestr wyjsciowy portu obslugujacego wyswietlanie segmentow
// digitDir	-- Rejestr kierunku portu obslugujacego wybor znaku
// digitOut	-- Rejestr wyjsciowy portu obslugujacego wybor znaku
Lcd *lcdInit(volatile unsigned char *segDir,
             volatile unsigned char *segOut,
             volatile unsigned char *digitDir,
             volatile unsigned char *digitOut);

// Metoda obslugujaca okresowe odswiezanie kolejnych znakow
void lcdUpdateDigit(Lcd *lcd, char isCounting);

// Metoda obslugujaca ustawianie zawartosci wyswietlacza
void lcdDisplaySeconds(Lcd *lcd, int seconds);

#endif /* LCD_H_ */
