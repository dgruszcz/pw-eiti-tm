	ORG 1800h
	JR  init

	DS 0x1838-$, 0

isr:
	CALL     clear
	EI
	RET

	DS 0x1860-$, 0

init:
	LD   SP, 1FFFh
	IM   1
	CALL clear
	EI

loop:
	EI
	IN   A, (0)
	LD   E, A
	LD   A, D

s0:
	CP  0
	JR  NZ, s1
	LD  A, E
	BIT 0, A
	JR  NZ, loop
	LD  D, 1
	JR  loop

s1:
	CP  1
	JR  NZ, s2
	LD  A, E
	BIT 0, A
	JR  NZ, s1_released
	INC C
	DI
	LD  A, C
	CP  255
	JR  NZ, loop
	INC B
	CALL disp
	LD  C, 0
	LD  D, 2
	JR  loop

s1_released:
	LD C, 0
	LD D, 0
	JR loop

s2:
	CP  2
	JR  NZ, s3
	LD  A, E
	BIT 0, A
	JR  Z, loop
	LD  D, 3
	JR  loop

s3:
	CP  3
	JR  NZ, loop
	LD  A, E
	BIT 0, A
	JR  Z, s3_pushed
	INC C
	LD  A, C
	CP  255
	JR  NZ, loop
	LD  C, 0
	LD  D, 0
	JR  loop

s3_pushed:
	LD C, 0
	LD D, 2
	JR loop

disp:
	PUSH AF
	LD   A, B
	AND  0fh
	OUT  (0), A
	POP  AF
	RET

clear:
	LD   B, 0
	LD   C, 0
	LD   D, 2
	CALL disp
	RET
