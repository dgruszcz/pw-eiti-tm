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
			mov.b	#0x02, P2IES
			mov.b	#0x02, P2IE

			mov.w	#0x00, R5 				; Debouncing register

			mov.b	#0x00, P3OUT

;-------------------------------------------------------------------------------
; Main loop here
;-------------------------------------------------------------------------------
loop:
	 		bis 	#GIE+CPUOFF+SCG1+SCG0, SR
loop_1:

 			bit.b	#0x02, P2IN
			jnz 	loop_2

			inc 	R5
			cmp		#0xFFF, R5
			jnz		loop_1
			inc 	P3OUT
loop_2:
			clr		R5
			mov.b	#0x00, P2IFG
			dint
			mov.b	#0x02, P2IE
			jmp		loop

;-------------------------------------------------------------------------------
; ISRs
;-------------------------------------------------------------------------------
isrP1:  ; reset
			mov.b	#0x00, P3OUT
			bit.b	#0x01, P1IN

			jz 	isrP1_1
			mov.b	#0x00, P1IFG
			mov.b	#0x00, P2IFG
isrP1_1:
			mov.w   #loop_2, 2(SP)
			reti
isrP2:  ; licz
			mov.b	#0x00, P2IFG
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
