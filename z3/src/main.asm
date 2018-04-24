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
			mov.b	#0x01, P2IES
			mov.b	#0x01, P2IE
			mov.w   #(0x08),SR

			mov.b	#0x01, P3OUT
			;mov.w   #isr, &0xffe2

;-------------------------------------------------------------------------------
; Main loop here
;-------------------------------------------------------------------------------
loop:
			jmp		loop


isrP1:
			mov.b	#0xFF, P3OUT
			mov.b	#0x00, P1IFG
			RETI
isrP2:
			mov.b	#0x00, P3OUT
			mov.b	#0x00, P2IFG
			RETI
;-------------------------------------------------------------------------------
; Stack Pointer definition
;-------------------------------------------------------------------------------
            .global __STACK_END
            .sect   .stack
            
;-------------------------------------------------------------------------------
; Interrupt Vectors
;-------------------------------------------------------------------------------
            .sect   ".reset"                ; MSP430 RESET Vector
            .short  RESET                   ; dalsze to tabelka od dolu!!!
            .sect	".int04"
            .short  isrP1
            .sect	".int01"
            .short  isrP2





