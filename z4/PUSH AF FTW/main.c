#include <msp430.h>
#include <stdint.h>
#include "Lcd.h"
#include "CtdnTimer.h"
#include "Buttons.h"

#define ONE_SECOND 32768
#define REFRESH_PERIOD ONE_SECOND / 60 / 4
#define BUTTON_PERIOD ONE_SECOND / 1000
#define SET_PERIOD ONE_SECOND / 2

Lcd *lcd;
CtdnTimer *ctdnTimer;
Buttons *buttons;

int main(void) {
     WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    TACTL = TASSEL_1 | MC_1;
    TACCTL0 = CCIE;
    TACCR0 = ONE_SECOND;

    TBCTL = TASSEL_1 | MC_2;
    TBCCTL0 = CCIE;
    TBCCR0 = REFRESH_PERIOD;
    TBCCTL1 = CCIE;
    TBCCR1 = BUTTON_PERIOD;
    TBCCTL2 = CCIE;
    TBCCR2 = SET_PERIOD;

    lcd = lcdInit();
    ctdnTimer = ctdnTimerInit();
    buttons = buttonsInit();

    __bis_SR_register(GIE);

    while (1) {
    	LPM1;
    	buttons->held = (buttons->states[0] | buttons->states[1]) * (buttons->held + 1);
    	if (buttons->held == 1) {
    		TBCCTL2 |= CCIFG;
    	}
    	if (buttons->states[0] || buttons->states[1]) {
    		TACTL &= ~(MC_1 | MC_0);
    		TAR = 0;
    	} else {
    		TACTL |= MC_1;
    	}
    	if (buttons->states_pending[0]) {
    		ctdnTimerSetSeconds(ctdnTimer, ctdnTimer->seconds - (buttons->held > 2500 ? 10 + (ctdnTimer->seconds % 10) : 1));
    	} else if (buttons->states_pending[1]) {
    		ctdnTimerSetSeconds(ctdnTimer, ctdnTimer->seconds + (buttons->held > 2500 ? 10 - (ctdnTimer->seconds % 10) : 1));
    	}
    	buttonsReset(buttons);
    	lcdDisplaySeconds(lcd, ctdnTimer->seconds);
    }
	
	return 0;
}

#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A0 (void) {  // licznik sekund
	ctdnTimerDecrement(ctdnTimer);
	LPM1_EXIT;
}

#pragma vector=TIMERB0_VECTOR
__interrupt void Timer_B0 (void) {  // odswiezanie lcd
	lcdUpdateDigit(lcd);
	TBCCR0 += REFRESH_PERIOD;
	LPM1_EXIT;
}

#pragma vector=TIMERB1_VECTOR
__interrupt void Timer_B1 (void) {
	uint16_t tbiv = TBIV;
	if (tbiv == TBIV_TBCCR1) {	// interwal sprawdzania stanu przyciskow
		buttons->states[0] = (~P2IN & (1 << 0)) ? 1 : 0;
		buttons->states[1] = (~P2IN & (1 << 1)) ? 1 : 0;
		TBCCR1 += BUTTON_PERIOD;
		LPM1_EXIT;
	}
	if (tbiv == TBIV_TBCCR2) {	// interwal ustawiania timera
		if (buttons->states[0]) {
			buttons->states_pending[0] = 1;
		}
		if (buttons->states[1]) {
			buttons->states_pending[1] = 1;
		}
		TBCCR2 += SET_PERIOD / (buttons->held > 10000 ? 101 : (buttons->held / 100) + 1);
		LPM1_EXIT;
	}
}
