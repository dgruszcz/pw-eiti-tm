#include <msp430.h> 
#include "hd44780.h"

int volt;

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
//    BCSCTL1 = CALBC1_1MHZ;                                    // Set range to calibrated 1MHz
//      DCOCTL  = CALDCO_1MHZ;                                    // Set DCO step and modulation to calibrated 1MHz

      P1DIR = 0xFF;                                              // Set P1.0 (D0) to P1.7 (D7) to output (HD44780 data lines)
      P2DIR = (0x01 | 0x02);                                    // Set P2.0 (E) and P2.1 (RS) to output (HD44780 control lines - R/W is tied to GND)

      TACCR1  = 0x3E8;                                          // Set CCR1 value for 1 ms interrupt (1000 / 1 MHz) = 0.001
      TACCTL1 = CCIE;                                          // Compare interrupt enable
      TACTL   = (TASSEL_2 | MC_2 | TACLR);                     // SMCLK as clock source, continuous mode, timer clear

      __bis_SR_register( GIE );                                 // Enable global interrupts
      volt = ADC12MEM15;
      hd44780_clear_screen();                                   // Clear display content

      while( 1 )                                                // Endless loop - main program
      {
        hd44780_write_string( "Hello world!", 1, 1, NO_CR_LF ); // Write text string to first row and first column of display
      }
}

#pragma vector = TIMERA1_VECTOR
__interrupt void timer_0_a1_isr( void )                     // Timer 0 A1 interrupt service routine
{
  switch( TAIV )                                           // Determine interrupt source
  {
    case 2:                                                 // CCR1 caused the interrupt
    {
      TACCR1 += 1000;                                      // Add CCR1 value for next interrupt in 1 ms

      hd44780_timer_isr();                                  // Call HD44780 state machine

      break;                                                // CCR1 interrupt handling done
    }
  }
}
