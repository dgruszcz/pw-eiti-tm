            .cdecls C,LIST,"msp430.h"
            .def    RESET
            .text
            .retain
            .retainrefs
RESET       mov.w   #__STACK_END,SP
StopWDT     mov.w   #WDTPW|WDTHOLD,&WDTCTL

;-------------------------------------------------------------------------------
; Init
;-------------------------------------------------------------------------------
CLK_BUT .set 0x02       ; Przycisk CLK
CLR_BUT .set 0x01       ; Przycisk CLR
DEB_TIM .set 0x04ff		; czas debouncingu

            mov.b   #0xff, P3DIR    ; Set P3 as output

            bis.b   #CLR_BUT, P1IES ; Konfiguracja przerwan CLR
            mov.b   #CLR_BUT, P1IE

            bis.b   #CLK_BUT, P2IES ; Konfiguracja przerwan CLK
            mov.b   #CLK_BUT, P2IE

            mov.w   #0x00, R5       ; Debouncing register
            mov.w   #0x00, R6       ; Rejestr pomocniczy

            mov.b   #0x00, P3OUT
;-------------------------------------------------------------------------------
; Main loop
;-------------------------------------------------------------------------------
loop:
            bis     #GIE+CPUOFF+SCG1+SCG0, SR
loop_1:
            eint
            bit.b   #CLR_BUT, P1IN
            jz      loop_1
            bit.b   #0x01, R6
            jnz     loop_3
            bit.b   #CLK_BUT, P2IN
            jnz     no_inc
            inc     R5
no_inc:
			inc		R7
            dint
            cmp     #DEB_TIM, R7
            jnz     loop_1
            cmp 	#0, R5
            jz		loop_3
            cmp     R5, R7
            jz		main_inc
			clr		R5
			clr 	R7

			jmp		loop_1




main_inc:
			bit.w	#0x02, R6
        	jnz		loop_3
            bis.w	#0x02, R6
            cmp     P4IN, P3OUT
            jl      loop_2
            mov.b   #0x00, P3OUT
           	clr		R5
            clr		R7
            jmp     loop_1
loop_2:
            inc     P3OUT
            clr		R5
            clr		R7
            jmp		loop_1

loop_3:
            eint
            clr     R5
            clr 	R7
            bic.b   #CLK_BUT, P2IFG
            dint
            clr     R6
            bis.b   #CLK_BUT, P2IE
            jmp     loop
;-------------------------------------------------------------------------------
; ISRs
;-------------------------------------------------------------------------------
isrP1:      ; Przerwania przycisku CLR
            mov.b   #0x00, P3OUT
            bic.b   #CLR_BUT, P1IFG
            bic.b   #CLK_BUT, P2IFG
            bic.b   #CLK_BUT, P2IE
            clr     R5
            bis.w   #0x01, R6
            bic     #CPUOFF+SCG1+SCG0, 0(SP)
            reti

isrP2:      ; Przerwania przycisku CLK
            bic     #CPUOFF+SCG1+SCG0, 0(SP)
            bic.b   #CLK_BUT, P2IE
            reti
;-------------------------------------------------------------------------------
; Stack Pointer definition
;-------------------------------------------------------------------------------
            .global __STACK_END
            .sect   .stack
;-------------------------------------------------------------------------------
; Interrupt Vectors
;-------------------------------------------------------------------------------
            .sect   ".reset"                ; MSP430 RESET Vector
            .short  RESET
            .sect   ".int04"
            .short  isrP1
            .sect   ".int01"
            .short  isrP2

