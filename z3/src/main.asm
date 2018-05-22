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
CLK_BUT .set 0x08                       ; przycisk CLK
CLR_BUT .set 0x01                       ; przycisk CLR
DEB_TIM .set 0x00ff                     ; czas debouncingu

            mov.b   #0xff, &P3DIR       ; ustaw P3 jako wyjscie

            bis.b   #CLR_BUT, &P1IES    ; konf. przerwan CLR
            mov.b   #CLR_BUT, &P1IE

            bis.b   #CLK_BUT, &P2IES    ; konf. przerwan CLK
            mov.b   #CLK_BUT, &P2IE

            mov.w   #0x00, R5           ; rejestr do debouncingu
            mov.w   #0x00, R6           ; rejestr pomocniczy

            mov.b   #0x00, &P3OUT
;-------------------------------------------------------------------------------
; Main loop
;-------------------------------------------------------------------------------
loop:
            bis     #GIE+CPUOFF+SCG1+SCG0, SR
loop_1:
            eint
            bit.b   #CLR_BUT, &P1IN
            jz      loop_1
            bit.b   #0x01, R6
            dint
            jnz     loop_3
            eint
            bit.b   #CLK_BUT, &P2IN
            jnz     no_inc
            inc     R5
no_inc:
            inc     R7
            cmp     #DEB_TIM, R7
            jnz     loop_1
            dint
            cmp     #0, R5
            jz      loop_3
            cmp     R5, R7
            jz      main_inc
            eint
            call    #clear
            jmp     loop_1

main_inc:
            bit.w   #0x02, R6
            call    #clear
            jnz     loop_1
            bis.w   #0x02, R6
            cmp     &P4IN, &P3OUT
            jl      loop_2
            mov.b   #0x00, &P3OUT
            call    #clear
            jmp     loop_1
loop_2:
            inc     &P3OUT
            call    #clear
            jmp     loop_1

loop_3:
            call    #clear
            bic.b   #CLK_BUT, &P2IFG
            clr     R6
            bis.b   #CLK_BUT, &P2IE
            jmp     loop

clear:
            clr     R5
            clr     R7
            ret
;-------------------------------------------------------------------------------
; ISRs
;-------------------------------------------------------------------------------
isrP1:      ; przerwania przycisku CLR
            mov.b   #0x00, &P3OUT
            bic.b   #CLR_BUT, &P1IFG
            bic.b   #CLK_BUT, &P2IFG
            bic.b   #CLK_BUT, &P2IE
            mov     #0x01, R5
            bis.w   #0x01, R6
            bic     #CPUOFF+SCG1+SCG0, 0(SP)
            reti

isrP2:      ; przerwania przycisku CLK
            bic     #CPUOFF+SCG1+SCG0, 0(SP)
            bic.b   #CLK_BUT, &P2IE
            reti
;-------------------------------------------------------------------------------
; Stack Pointer definition
;-------------------------------------------------------------------------------
            .global __STACK_END
            .sect   .stack
;-------------------------------------------------------------------------------
; Interrupt Vectors
;-------------------------------------------------------------------------------
            .sect   ".reset"    ; MSP430 RESET Vector
            .short  RESET
            .sect   ".int04"
            .short  isrP1
            .sect   ".int01"
            .short  isrP2
