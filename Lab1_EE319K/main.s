;****************** main.s ***************
; Program initially written by: Yerraballi and Valvano
; Author: Ayan Basu (EID: ab73287)
; Date Created: 1/15/2018 
; Last Modified: 1/17/2021 
; Brief description of the program: Solution to Lab1
; The objective of this system is to implement odd-bit counting system
; Hardware connections: 
;  Output is positive logic, 1 turns on the LED, 0 turns off the LED
;  Inputs are negative logic, meaning switch not pressed is 1, pressed is 0
;    PE0 is an input 
;    PE1 is an input 
;    PE2 is an input 
;    PE4 is the output
; Overall goal: 
;   Make the output 1 if there is an even number of switches pressed, 
;     otherwise make the output 0

; The specific operation of this system 
;   Initialize Port E to make PE0,PE1,PE2 inputs and PE4 an output
;   Over and over, read the inputs, calculate the result and set the output
; PE2  PE1 PE0  | PE4
; 0    0    0   |  0    3 switches pressed, odd 
; 0    0    1   |  1    2 switches pressed, even
; 0    1    0   |  1    2 switches pressed, even
; 0    1    1   |  0    1 switch pressed, odd
; 1    0    0   |  1    2 switches pressed, even
; 1    0    1   |  0    1 switch pressed, odd
; 1    1    0   |  0    1 switch pressed, odd
; 1    1    1   |  1    no switches pressed, even
;There are 8 valid output values for Port E: 0x00,0x11,0x12,0x03,0x14,0x05,0x06, and 0x17. 

; NOTE: Do not use any conditional branches in your solution. 
;       We want you to think of the solution in terms of logical and shift operations

GPIO_PORTE_DATA_R  EQU 0x400243FC
GPIO_PORTE_DIR_R   EQU 0x40024400
GPIO_PORTE_DEN_R   EQU 0x4002451C
SYSCTL_RCGCGPIO_R  EQU 0x400FE608

    THUMB
    AREA    DATA, ALIGN=2
;global variables go here
    ALIGN
    AREA    |.text|, CODE, READONLY, ALIGN=2
    EXPORT  Start
Start
    ;code to run once that initializes PE3,PE2,PE1,PE0
	LDR R0, =SYSCTL_RCGCGPIO_R ; R0 points to GPIO clock
	LDRB R1, [R0]	; Read SYSCTL_RCGCGPIO_R into R1
	ORR R1, #0x10	; Turns on Port E clock
	STRB R1, [R0]	; Write back to SYSCTL_RCGCGPIO_R
	
	NOP				; Wait for clock to stabalize
	NOP
	
	LDR R0, =GPIO_PORTE_DIR_R ;//direction register 
	LDRB R1, [R0]	;
	AND R1, #0xF8
	ORR R1, #0x10 ;//outputs are 1, inputs are 0
	STRB R1, [R0]
	
	LDR R0, =GPIO_PORTE_DEN_R
	LDRB R1, [R0]
	ORR R1, #0x1F
	STRB R1, [R0]
	
loop
    ;code that runs over and over
	LDR R0, =GPIO_PORTE_DATA_R
	LDRB R0, [R0]
	AND R1, R0, #0x01
	AND R2, R0, #0x02
	LSR R2, #1
	AND R3, R0, #0x04
	LSR R3, #2
	EOR R1, R1, R2
	EOR R1, R1, R3
	LSL R1, #4
	
	LDR R0, =GPIO_PORTE_DATA_R
	STRB R1, [R0]
	
    B    loop

    ALIGN        ; make sure the end of this section is aligned
      END          ; end of file