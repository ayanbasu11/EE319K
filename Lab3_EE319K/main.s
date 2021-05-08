;****************** main.s ***************
; Program written by: Ayan Basu & Ayush Roychowdhury
; Date Created: 2/4/2017
; Last Modified: 1/17/2021
; Brief description of the program
;   The LED toggles at 2 Hz and a varying duty-cycle
; Hardware connections (External: One button and one LED)
;  PE1 is Button input  (1 means pressed, 0 means not pressed)
;  PE2 is LED output (1 activates external LED on protoboard)
;  PF4 is builtin button SW1 on Launchpad (Internal) 
;        Negative Logic (0 means pressed, 1 means not pressed)
; Overall functionality of this system is to operate like this
;   1) Make PE2 an output and make PE1 and PF4 inputs.
;   2) The system starts with the the LED toggling at 2Hz,
;      which is 2 times per second with a duty-cycle of 30%.
;      Therefore, the LED is ON for 150ms and off for 350 ms.
;   3) When the button (PE1) is pressed-and-released increase
;      the duty cycle by 20% (modulo 100%). Therefore for each
;      press-and-release the duty cycle changes from 30% to 70% to 70%
;      to 90% to 10% to 30% so on
;   4) Implement a "breathing LED" when SW1 (PF4) on the Launchpad is pressed:
;      a) Be creative and play around with what "breathing" means.
;         An example of "breathing" is most computers power LED in sleep mode
;         (e.g., https://www.youtube.com/watch?v=ZT6siXyIjvQ).
;      b) When (PF4) is released while in breathing mode, resume blinking at 2Hz.
;         The duty cycle can either match the most recent duty-
;         cycle or reset to 30%.
;      TIP: debugging the breathing LED algorithm using the real board.
; PortE device registers
GPIO_PORTE_DATA_R  EQU 0x400243FC
GPIO_PORTE_DIR_R   EQU 0x40024400
GPIO_PORTE_AFSEL_R EQU 0x40024420
GPIO_PORTE_DEN_R   EQU 0x4002451C
; PortF device registers
GPIO_PORTF_DATA_R  EQU 0x400253FC
GPIO_PORTF_DIR_R   EQU 0x40025400
GPIO_PORTF_AFSEL_R EQU 0x40025420
GPIO_PORTF_PUR_R   EQU 0x40025510
GPIO_PORTF_DEN_R   EQU 0x4002551C
GPIO_PORTF_LOCK_R  EQU 0x40025520
GPIO_PORTF_CR_R    EQU 0x40025524
GPIO_LOCK_KEY      EQU 0x4C4F434B  ; Unlocks the GPIO_CR register
SYSCTL_RCGCGPIO_R  EQU 0x400FE608

       IMPORT  TExaS_Init
       THUMB
       AREA    DATA, ALIGN=2
;global variables go here


       AREA    |.text|, CODE, READONLY, ALIGN=2
       THUMB

       EXPORT  Start

Start
;  PE1 is Button input  (1 means pressed, 0 means not pressed)
;  PE2 is LED output (1 activates external LED on protoboard)
;  PF4 is builtin button SW1 on Launchpad (Internal)
	 LDR R0, =SYSCTL_RCGCGPIO_R
	 LDRB R1, [R0]
	 ORR R1, #0x30
	 STRB R1, [R0]
	 
	 NOP
	 NOP
	 
	 LDR R0, =GPIO_PORTE_DIR_R
	 LDRB R1, [R0]
	 AND R1, #0xFD
	 ORR R1, #0x04
	 STRB R1, [R0]
	 
	 LDR R0, =GPIO_PORTE_DEN_R
	 LDRB R1, [R0]
	 ORR R1, #0x06
	 STRB R1, [R0]
	 
	 LDR R0, =GPIO_PORTF_DIR_R
	 LDRB R1, [R0]
	 AND R1, #0xFF
	 STRB R1, [R0]
	 
	 LDR R0, =GPIO_PORTF_DEN_R
	 LDRB R1, [R0]
	 ORR R1, #0x10
	 STRB R1, [R0]
	 
	 LDR R0, =GPIO_PORTF_PUR_R
	 LDR R1, [R0]
	 ORR R1, #0x10
	 STR R1, [R0] 
     
	 BL  TExaS_Init
	 
	 LDR R1, =150

;breathe algorithm - 10% - 30% - 50%
; voltmeter, scope on PD3
 ; Initialization goes here
    

     CPSIE  I    ; TExaS voltmeter, scope runs on interrupts
loop  
	LDR R2, =GPIO_PORTE_DATA_R
	LDR R3, [R2]
	AND R4, R3, #0x02 ;PE1 Isolated: Possible Values -
	LDR R7, =GPIO_PORTF_DATA_R
	LDR R8, [R7]
	AND R9, R8, #0x10
	
check_for_pf4_press
    PUSH {R1, R2}
	CMP R9, #0x10 
	BNE breathe
	POP {R1, R2}
	BEQ check_for_button_press

breathe
	BL LED_ON
	BL delay_for_R1ms
	BL LED_OFF
	LDR R2, =500
	SUBS R1, R2, R1 
	BL delay_for_R1ms
	CMP R9, #0x10 
	BEQ breathe_increment
	BNE check_for_button_press
	BL breathe_increment

breathe_increment
	LDR R6, =450
	SUBS R5, R6, R1
	CMP R5, #0x0
	BEQ breathe_decrement
	ADD R1, R1, #100
	CMP R9, #0x10 
	BEQ breathe
	BNE check_for_button_press

breathe_decrement
	LDR R6, =50
	SUBS R5, R1, R6
	CMP R5, #0x0
	BEQ breathe_increment
	SUBS R1, R1, #100
	CMP R9, #0x10
	BEQ breathe
	BNE check_for_button_press
	
	
check_for_button_press 
	CMP R4, #0x02
	BEQ check_for_release
	BNE duty_cycle

check_for_release
	LDR R2, =GPIO_PORTE_DATA_R
	LDR R3, [R2]
	AND R4, R3, #0x02 ;PE1 Isolated: Possible Values -	
	CMP R4,#0x0
	BEQ increment_duty_cycle
	BNE check_for_button_press
	
increment_duty_cycle
	LDR R6, =450
	SUBS R5, R6, R1
	CMP R5, #0x0
	BEQ change_to_tenpercent
	ADD R1, R1, #100
	BL duty_cycle

change_to_tenpercent
	LDR R1, =50
	BL duty_cycle

duty_cycle
	PUSH {R0, R1} 
	BL LED_ON
	BL delay_for_R1ms
	BL LED_OFF
	LDR R2, =500
	SUBS R1, R2, R1 
	BL delay_for_R1ms
	POP {R0, R1}
	B loop
	
LED_ON ;
	PUSH {R2, R3}
	LDR R2, =GPIO_PORTE_DATA_R
	ORR R3, R3, #0x04 ; LED ON
	STRB R3, [R2]
	POP {R2, R3}
	
	BX LR
	
LED_OFF
	PUSH {R2, R3}
	LDR R2, =GPIO_PORTE_DATA_R
	BIC R3, R3, #0x04
	STRB R3, [R2]
	POP {R2, R3}
	BX LR
	
delay_for_R1ms
	PUSH {R0, R1}
loop_1
	LDR R0, =80000/4
loop_2
	SUBS R0, R0, #1
	BNE loop_2
	SUBS R1, R1, #1
	BNE loop_1
	POP {R0, R1}
	BX LR
	
	
	 B    loop
     

      
     ALIGN      ; make sure the end of this section is aligned
     END        ; end of file



