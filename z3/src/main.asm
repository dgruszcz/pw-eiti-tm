;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
;
;-------------------------------------------------------------------------------
            .cdecls C,LIST,"msp430.h"       ; Include device header file
            
;-------------------------------------------------------------------------------
            .def    RESET                   ; Export program entry-point to
                                            ; make it known to linker.
;-------------------------------------------------------------------------------
            .text                           ; Assemble into program memory.
            .retain                         ; Override ELF conditional linking
                                            ; and retain current section.
            .retainrefs                     ; And retain any sections that have
                                            ; references to current section.

;-------------------------------------------------------------------------------

RESET       mov.w   #__STACK_END,SP         ; Initialize stackpointer
StopWDT     mov.w   #WDTPW|WDTHOLD,&WDTCTL  ; Stop watchdog timer
			mov.b	#0xff, P3DIR			; Set P3 as output

			mov.b	#0x01, P1IES
			mov.b	#0x01, P1IE

			mov.b	#0x08, P2IES
			mov.b	#0x08, P2IE

			mov.w	#0x00, R5 				; Debouncing register

			mov.b	#0x00, P3OUT

;-------------------------------------------------------------------------------
; Main loop here
;-------------------------------------------------------------------------------
loop:
	 		bis 	#GIE+CPUOFF+SCG1+SCG0, SR
loop_1:
			eint
			bit.b	#0x01, P1IN
			jz		loop_1
			bit.b	#0x01, R6
			jnz		loop_3
 			bit.b	#0x08, P2IN
			jnz 	loop_3
			inc 	R5
			dint
			cmp		#0x04FF, R5
			jnz		loop_1
			cmp		P4IN, P3OUT
			jl		loop_2
			mov.b	#0x00, P3OUT
			jmp		loop_3
loop_2:
			inc 	P3OUT
loop_3:
			eint
			clr		R5
			mov.b	#0x00, P2IFG
			dint
			clr     R6
			mov.b	#0x08, P2IE
			jmp		loop


;-------------------------------------------------------------------------------
; ISRs
;-------------------------------------------------------------------------------
isrP1:  ; Reset
			mov.b	#0x00, P3OUT
			mov.b	#0x00, P1IFG
			mov.b	#0x00, P2IFG
			mov.b	#0x00, P2IE
			clr     R5
			bis.w   #0x01, R6
isrP1_1:
			bic		#CPUOFF+SCG1+SCG0, 0(SP)
			reti

isrP2:  ; Licz
			bic		#CPUOFF+SCG1+SCG0, 0(SP)
			mov.b	#0x00, P2IE
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
            .sect	".int04"
            .short  isrP1
            .sect	".int01"
            .short  isrP2
