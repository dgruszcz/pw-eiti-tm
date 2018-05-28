#include <msp430.h>
#include <stdint.h>
#include "Lcd.h"
#include "CtdnTimer.h"
#include "Buttons.h"
#include "Synth.h"
#include "melodies.h"

#define ONE_SECOND 32768
#define REFRESH_PERIOD (ONE_SECOND / 60 / 5)
#define BUTTON_PERIOD (ONE_SECOND / 100)
#define SET_PERIOD ONE_SECOND / 2
#define DEBOUNCING_PERIOD 4
#define T_MAX_SPD 7

#define HPS (ONE_SECOND / BUTTON_PERIOD)

#define BTN_PORT_IN		P2IN

uint16_t pitches[] = MEL_THELICKEX_P;
uint16_t durations[] = MEL_THELICKEX_D;
uint8_t length = MEL_THELICKEX_L;

Lcd *lcd;
CtdnTimer *ctdnTimer;
Buttons *buttons;
Synth *synth;

void startSynth();

void main(void) {
	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

	// Konfiguracja zegara CPU
	BCSCTL1 = RSEL2 | RSEL1 | RSEL0;
	DCOCTL = DCO2 | DCO1 | DCO0;

	// Konfiguracja wyjscia kanalu 4 Timera B
	P4DIR |= BIT4;
	P4SEL |= BIT4;

	// Konfiguracja interwalow przerwan
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

    	// Wstrzymywanie Timera A przy wcisnietym przycisku
    	if (buttons->states[0] || buttons->states[1]) {
    		TACTL &= ~(MC_1 | MC_0);
    		TACTL |= TACLR;
    	} else {
    		TACTL |= MC_1;
    	}

    	if (buttons->states[0] || buttons->states[1] || buttons->states[2]) {
    		synthStop(synth);
    	}

    	// Obsluga przyciskow (inkrementacja / dekrementacja / zerowanie)
    	if (buttons->states[2]) {
    		ctdnTimerSetSeconds(ctdnTimer, 0);
    	}
    	else if (buttons->statesPending[0] && buttons->held >= DEBOUNCING_PERIOD) {
    		ctdnTimerSetSeconds(ctdnTimer, ctdnTimer->seconds - (buttons->held > 2 * T_MAX_SPD * HPS ? 10 + (ctdnTimer->seconds % 10) : 1));
    	} else if (buttons->statesPending[1] && buttons->held >= DEBOUNCING_PERIOD) {
    		ctdnTimerSetSeconds(ctdnTimer, ctdnTimer->seconds + (buttons->held > 2 * T_MAX_SPD * HPS ? 10 - (ctdnTimer->seconds % 10) : 1));
    	}

    	buttonsReset(buttons);
    	lcdDisplaySeconds(lcd, ctdnTimer->seconds);
    }
}

void startSynth() {
	synthStart(synth);

	// Wlacz przerwania od dlugosci dzwieku
	TBCCTL3 |= CCIE;
	TBCCTL3 |= CCIFG;
}

#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A0 (void) {  // Licznik sekund
	if (ctdnTimerDecrement(ctdnTimer)) {
		startSynth();
	}
	LPM1_EXIT;
}

#pragma vector=TIMERB0_VECTOR
__interrupt void Timer_B0 (void) {  // Odswiezanie LCD
	lcdUpdateDigit(lcd, TACTL & MC_1);
	TBCCR0 += REFRESH_PERIOD;
	LPM1_EXIT;
}

#pragma vector=TIMERB1_VECTOR
__interrupt void Timer_B1 (void) {
	uint16_t tbiv = TBIV;
	if (tbiv == TBIV_TBCCR1) {	// Interwal sprawdzania stanu przyciskow
		buttons->states[0] = (~BTN_PORT_IN & (1 << 6)) ? 1 : 0;
		buttons->states[1] = (~BTN_PORT_IN & (1 << 7)) ? 1 : 0;
		buttons->states[2] = (~BTN_PORT_IN & (1 << 5)) ? 1 : 0;
		buttons->held = (buttons->states[0] ^ buttons->states[1]) * (buttons->held + 1);
		if (buttons->held == DEBOUNCING_PERIOD + 1) {
			TBCCTL2 |= CCIFG;  // Obsluga przycisku bez opoznienia
		}
		TBCCR1 += BUTTON_PERIOD;
		LPM1_EXIT;
	}
	if (tbiv == TBIV_TBCCR2) {	// Interwal inkrementacji / dekrementacji timera
		if (buttons->states[0]) {
			buttons->statesPending[0] = 1;
		}
		if (buttons->states[1]) {
			buttons->statesPending[1] = 1;
		}

		// Proporcjonalne zwiekszanie czestotliwosci wywolywania przerwania
		TBCCR2 += SET_PERIOD / (buttons->held > T_MAX_SPD * HPS ? 50 : (50 * buttons->held / (T_MAX_SPD * HPS) + 1));

		LPM1_EXIT;
	}
	if (tbiv == TBIV_TBCCR3) { // Interwal dlugosci nuty
		uint16_t duration = synthUpdateDuration(synth);
		if (duration == 0) {
			// Zakanczanie pracy syntezatora
			TBCCTL3 &= ~CCIE;
			TBCCTL4 &= ~CCIE;
			TBCCTL4 &= ~OUTMOD_4;
			TBCCTL4 &= ~OUT;
			synth->index = 0;
		} else {
			uint16_t pitch = synthUpdatePitch(synth);
			if (pitch == 0) {
				// Odgrywanie pauzy
				TBCCTL4 &= ~CCIE;
				TBCCTL4 &= ~OUTMOD_4;
				TBCCTL4 &= ~OUT;
			} else if (synth->index == 1 || synth->pitches[synth->index - 2] == 0) {
				// Odgrywanie pierwszego cyklu nuty
				TBCCTL4 |= CCIE;
				TBCCTL4 |= OUTMOD_4;
				TBCCR4 = TBR + pitch / 2;
			}
			if (synth->index == 1) {
				TBCCR3 = TBR;  // Synchronizacja kanalu
			}
			TBCCR3 += duration;
		}
	}
	if (tbiv == TBIV_TBCCR4) { // interwal czestotliwosci nuty
		uint16_t pitch = synthUpdatePitch(synth);
		TBCCR4 += pitch / 2;
	}
}
