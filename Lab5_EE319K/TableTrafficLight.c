// TableTrafficLight.c solution to EE319K Lab 5, spring 2021
// Runs on TM4C123
// Moore finite state machine to operate a traffic light.  
// Ayush RoyChowdhury, Ayan Basu
// March 8, 2021

/* 

 Copyright 2021 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// east/west red light connected to PB5
// east/west yellow light connected to PB4
// east/west green light connected to PB3
// north/south facing red light connected to PB2
// north/south facing yellow light connected to PB1
// north/south facing green light connected to PB0
// pedestrian detector connected to PE2 (1=pedestrian present)
// north/south car detector connected to PE1 (1=car present)
// east/west car detector connected to PE0 (1=car present)
// "walk" light connected to PF3-1 (built-in white LED)
// "don't walk" light connected to PF1 (built-in red LED)

#include <stdint.h>
#include "SysTick.h"
#include "TExaS.h"
#include "../inc/tm4c123gh6pm.h"



struct state{
	uint8_t outTrafficLED;	// Represents the bit number of the output; this output is the port for the traffic light LEDs (represents PORT B)
	uint8_t outWalkLED;		  // Represents the bit number of the output; this output is the port for the walk LED (represents PORT F)
	uint8_t wait;				    // Represents the delay time between each signal or each SysTick
	uint8_t next[8]; 			  // Number of NEXT States cooresponding to each input 
};

//typedef const struct state state_t; // Typedef that is used as a name to define data type "state"; a placeholder name (ALIAS)

// All of the states in the FSM
#define start 0		// Start state --> Walk LED, South & West are all RED
#define gW		1 	// Green West
#define yW		2		// Yellow West
#define rW		3 	// Red West
#define gS		4		// Green South
#define yS		5		// Yellow South
#define rS		6		// Red South	
#define walk	7		// Walk LED --> PF1, PF2, PF3 are all on, --> 00001110
#define on1		8		// First Warning LED ON
#define off1	9		// First Warning LED OFF
#define on2		10	// Second Warning LED ON
#define off2	11	// Second Warning LED OFF
#define on3		12	// Third Warning LED ON
#define off3	13	// Third Warning LED OFF
#define dWalk	14	// Don't Walk LED --> Red LED kept ON --> PF1 is ON

// input values for port E: 
// 0x0=000= no sensors
// 0x1=001= west sensor
// 0x2=010= south sensor
// 0x3=011= west & south sensors
// 0x4=100= walk sensor
// 0x5=101= walk & west sensors
// 0x6=110= walk & south sensors
// 0x7=111= walk & west & south sensors

// output values for port B:
// 0x0C=00001100; - PB2&PB3 --> Red West / Green South
// 0x14=00010100; - PB2&PB4 --> Red West / Yellow South
// 0x21=00100001; - PB0&PB5 --> Green West / Red South
// 0x22=00100010; - PB1&PB5 --> Yellow West / Red South
// 0x24=00100100; - PB2&PB5 --> Red West / Red South

// output values for port F:
// 0x2=0010; --> PF1 is ON -> don't walk
// 0x8=1000; --> PF3 is ON -> do walk
// 0xE=1110; --> PF3, PF2, PF1 is ON --> WHITE LED
// Next state notation as follows: {000, 001, 010, 011, 100, 101, 110, 111}
// Array that represents my state definitions
// With typedef commented in line (43), I could use "const struct state_t" FSM[# of state definitions] {initializing & defining state definitions}
struct state FSM[15] = { 			/* Number of States in the FSM*/
	{0x24, 0x2, 10, {start, gW, gS, start, walk, gW, gS, gW}},	// "Start" state, walkLED, southLED, & westLED all RED
	{0x21, 0x2, 10, {yW, gW, yW, yW, yW, yW, yW, yW}},															//gW - Green West
	{0x22, 0x2, 10, {rW, rW, rW, rW, rW, rW, rW, rW}},															//yW - Yellow West
	{0x24, 0x2, 10, {start, start, gS, gS, walk, walk, gS, gS}},										//rW - Red West
	{0x0C, 0x2, 10, {yS, yS, gS, yS, yS, yS, yS, yS}},															//gS - Green South
	{0x14, 0x2, 10, {rS, rS, rS, rS, rS, rS, rS, rS}},															//yS - Yellow South
	{0x24, 0x2, 10, {start, gW, start, gW, walk, gW, walk, walk}},									//rS - Red South
	{0x24, 0xE, 10, {on1, on1, on1, on1, walk, on1, on1, on1}},											//walk - Walk
	{0x24, 0x2, 100, {off1, off1, off1, off1, on1, off1, off1, off1}},							//on1
	{0x24, 0x0, 100, {on2, on2, on2, on2, off1, on2, on2, on2}},										//off1
	{0x24, 0x2, 100, {off2, off2, off2, off2, on2, off2, off2, off2}},							//on2
	{0x24, 0x0, 100, {on3, on3, on3, on3, off2, on3, on3, on3}},										//off2
	{0x24, 0x2, 100, {off3, off3, off3, off3, on3, off3, off3, off3}},							//on3
	{0x24, 0x0, 100, {dWalk, dWalk, dWalk, dWalk, off3, dWalk, dWalk, dWalk}},			//off3
	{0x24, 0x2, 100, {start, start, start, start, start, start, start, start}},			//dWalk	- Don't Walk
};

void DisableInterrupts(void);
void EnableInterrupts(void);

#define PB543210                (*((volatile uint32_t *)0x400050FC)) // bits 5-0
#define PE210                   (*((volatile uint32_t *)0x4002401C)) // bits 2-0
#define PF321                   (*((volatile uint32_t *)0x40025038)) // bits 3-1


void LogicAnalyzerTask(void){
  UART0_DR_R = 0x80|GPIO_PORTB_DATA_R;
}

// run this version in the simulator
int main(void){volatile uint32_t delay;
  DisableInterrupts();
  TExaS_Init(&LogicAnalyzerTask); // Breadboard
  //PLL_Init(); // PLL on at 80 MHz // Simulator
  SYSCTL_RCGC2_R |= 0x32; // Ports B,E,F
  delay = SYSCTL_RCGC2_R;
  SysTick_Init();   // Initialize SysTick for software waits
// **************************************************
// weird old bug in the traffic simulator
// run next two lines on real board to turn on F E B clocks
  SYSCTL_RCGCGPIO_R |= 0x32;  // real clock register 
//  while((SYSCTL_PRGPIO_R&0x32)!=0x32){};
// run next two lines on simulator to turn on F E B clocks
  SYSCTL_RCGC2_R |= 0x32;  // LM3S legacy clock register
  delay = SYSCTL_RCGC2_R;
	GPIO_PORTB_DIR_R |= 0x3F;
	GPIO_PORTB_DEN_R |= 0x3F;
	GPIO_PORTE_DIR_R &= 0x00;
	GPIO_PORTE_DEN_R |= 0x07;
	GPIO_PORTF_DIR_R |= 0x0E;
	GPIO_PORTF_DEN_R |= 0x0E;
// **************************************************
 
  EnableInterrupts();
	
// CREATING THE FSM ENGINE
	
	uint32_t input; // Input is what drives the transition
	uint32_t current = start;	// Now, currentState is OFF
    
  while(1){
		GPIO_PORTB_DATA_R = FSM[current].outTrafficLED;  // Output for Traffic LED Lights (north/south & east/west)
		GPIO_PORTF_DATA_R = FSM[current].outWalkLED;		 // Output	for Walk LED Lights
		SysTick_Wait10ms(FSM[current].wait);						 // Wait/Delay Timing with SysTick
		input = GPIO_PORTE_DATA_R & 0x7;											 // Input
		current = FSM[current].next[input];							 // Transition - Set current state to my new state
  }
}


