#include <msp430.h>
#include <stdint.h>
#include "Lcd.h"
#include "CtdnTimer.h"
#include "Buttons.h"
#include "Synth.h"
#include "melodies.h

#define ONE_SECOND 32768
#define REFRESH_PERIOD ONE_SECOND / 60 / 5
#define BUTTON_PERIOD ONE_SECOND / 100
#define SET_PERIOD ONE_SECOND
#define DEBOUNCING_PERIOD 2


uint16_t pitches[] = MEL_THELICK_P;
uint16_t durations[] = MEL_THELICK_D;
uint8_t length = MEL_THELICK_L;

Lcd *lcd;
CtdnTimer *ctdnTimer;
Buttons *buttons;
Synth *synth;

void startSynth();

void main(void) {
	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

	BCSCTL1 = RSEL2 | RSEL1 | RSEL0;
	DCOCTL = DCO2 | DCO1 | DCO0;

	P4DIR |= BIT4;
	P4SEL |= BIT4;

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

    lcd = lcdInit(&P6DIR, &P6OUT, &P3DIR, &P3OUT);
    ctdnTimer = ctdnTimerInit();
    buttons = buttonsInit();
    synth = synthInit(pitches, durations, length);

    __bis_SR_register(GIE);

    while (1) {
    	LPM1;
    	buttons->held = (buttons->states[0] ^ buttons->states[1]) * (buttons->held + 1);
    	if (buttons->held == DEBOUNCING_PERIOD + 1) {
    		TBCCTL2 |= CCIFG;
    	}

    	if (buttons->states[0] || buttons->states[1]) {
    		TACTL &= ~(MC_1 | MC_0);
    		TACTL |= TACLR;
    	} else {
    		TACTL |= MC_1;
    	}

    	if (buttons->states[0] || buttons->states[1] || buttons->states[2]) {
    		synthStop(synth);
    	}

    	if (buttons->states[2]) {
    		ctdnTimerSetSeconds(ctdnTimer, 0);
    	}
    	else if (buttons->statesPending[0] && buttons->held > DEBOUNCING_PERIOD) {
    		ctdnTimerSetSeconds(ctdnTimer, ctdnTimer->seconds - (buttons->held > 25 * BUTTON_PERIOD ? 10 + (ctdnTimer->seconds % 10) : 1));
    	} else if (buttons->statesPending[1] && buttons->held >= DEBOUNCING_PERIOD) {
    		ctdnTimerSetSeconds(ctdnTimer, ctdnTimer->seconds + (buttons->held > 25 * BUTTON_PERIOD ? 10 - (ctdnTimer->seconds % 10) : 1));
    	}

    	buttonsReset(buttons);
    	lcdDisplaySeconds(lcd, ctdnTimer->seconds);
    }
}

void startSynth() {
	synthStart(synth);
	TBCCTL3 |= CCIE;
	TBCCTL3 |= CCIFG;
}

#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A0 (void) {  // licznik sekund
	if (ctdnTimerDecrement(ctdnTimer)) {
		startSynth();
	}
	LPM1_EXIT;
}

#pragma vector=TIMERB0_VECTOR
__interrupt void Timer_B0 (void) {  // odswiezanie lcd
	lcdUpdateDigit(lcd, TACTL & MC_1);
	TBCCR0 += REFRESH_PERIOD;
	LPM1_EXIT;
}

#pragma vector=TIMERB1_VECTOR
__interrupt void Timer_B1 (void) {
	uint16_t tbiv = TBIV;
	if (tbiv == TBIV_TBCCR1) {	// interwal sprawdzania stanu przyciskow
		buttons->states[0] = (~P2IN & (1 << 6)) ? 1 : 0;
		buttons->states[1] = (~P2IN & (1 << 7)) ? 1 : 0;
		buttons->states[2] = (~P2IN & (1 << 5)) ? 1 : 0;
		TBCCR1 += BUTTON_PERIOD;
		LPM1_EXIT;
	}
	if (tbiv == TBIV_TBCCR2) {	// interwal ustawiania timera
		if (buttons->states[0]) {
			buttons->statesPending[0] = 1;
		}
		if (buttons->states[1]) {
			buttons->statesPending[1] = 1;
		}
		if (buttons->states[2]) {
			buttons->statesPending[2] = 1;
		}
		TBCCR2 += SET_PERIOD / (buttons->held > 1000 * BUTTON_PERIOD ? 10 * BUTTON_PERIOD + 1 : (buttons->held / 100) + 1);
		LPM1_EXIT;
	}
	if (tbiv == TBIV_TBCCR3) { // interwal dlugosci nuty
		uint16_t duration = synthUpdateDuration(synth);
		if (duration == 0) {
			TBCCTL3 &= ~CCIE;
			TBCCTL4 &= ~CCIE;
			TBCCTL4 &= ~OUTMOD_4;
			TBCCTL4 &= ~OUT;
			synth->index = 0;
		} else {
			uint16_t pitch = synthUpdatePitch(synth);
			if (pitch == 0) {
				TBCCTL4 &= ~CCIE;
				TBCCTL4 &= ~OUTMOD_4;
				TBCCTL4 &= ~OUT;
			} else if (synth->index == 1 || synth->pitches[synth->index - 2] == 0) {
				TBCCTL4 |= CCIE;
				TBCCTL4 |= OUTMOD_4;
				TBCCR4 = TBR + pitch / 2;
			}
			if (synth->index == 1) {
				TBCCR3 =TBR;
			}
			TBCCR3 += duration;
		}
	}
	if (tbiv == TBIV_TBCCR4) { // interwal czestotliwosci nuty
		uint16_t pitch = synthUpdatePitch(synth);
		TBCCR4 += pitch / 2;
	}
}
