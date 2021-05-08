// IO.c
// This software configures the switch and LED
// You are allowed to use any switch and any LED, 
// although the Lab suggests the SW1 switch PF4 and Red LED PF1
// Runs on TM4C123
// Program written by: Ayan Basu & Ayush Roychowdhury
// Date Created: 
// Last Modified:  
// Lab number: 7


#include "../inc/tm4c123gh6pm.h"
#include <stdint.h>
#include "../inc/CortexM.h"

// Bit-specific addressing
#define PF2 (*((volatile unsigned long *)0x40025010)) // PF2
#define PF4 (*((volatile unsigned long *)0x40025040))  // PF4




//------------IO_Init------------
// Initialize GPIO Port for a switch and an LED
// Input: none
// Output: none

void IO_Init(void) { volatile uint32_t delay;
 // --UUU-- Code to initialize PF4 and PF2
	SYSCTL_RCGCGPIO_R |= 0x20;	 // 1) Activate clock for Port - Set bit 5
	delay = SYSCTL_RCGCGPIO_R;	 // Allow time for the clock to start
	GPIO_PORTF_DIR_R &= 0xEF;    // 2) PF4 in, PF2 out ->	Set for Output (=1), Reset for Input (>= 0)
	GPIO_PORTF_DIR_R |= 0x04;    // 2) PF4 in, PF2 out ->	Set for Output (=1), Reset for Input (>= 0)
	GPIO_PORTF_PUR_R |= 0x10;		 // 3) Set pull up register for PF4 --> set bit 4
	GPIO_PORTF_DEN_R |= 0x14;		 // 4) Enable digital I/O on PF4, PF2 --> Set bit 2 & 4
}

//------------IO_HeartBeat------------
// Toggle the output state of the PF2 LED.
// Input: none
// Output: none
void IO_HeartBeat(void) {
 // --UUU-- 
  GPIO_PORTF_DATA_R ^= 0x04;		// Toggle PF2 value (Stored in port)
	Clock_Delay1ms(100);					// Used this function for a delay
}

//------------IO_Touch------------
// wait for release and press of the switch
// Input: none
// Output: none
// 1) wait for release; 
// 2) delay for 5ms; 
// 3) wait for press; and then 
// 4) delay for another 5ms
void IO_Touch(void) {
 // --UUU-- 
  while(GPIO_PORTF_DATA_R>>4 == 0) // Wait while PF4 is 0
	{
	}
	int delay = 0;									 // Use delay function 
	while(delay < 200000 )					 // Delay for 5ms
	{
		delay++;
	}
	while(GPIO_PORTF_DATA_R>>4 == 1) // Wait while PF4 is 1
	{
	}
}  


