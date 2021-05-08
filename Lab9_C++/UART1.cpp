// UART1.cpp
// Runs on TM4C123

// EE319H tasks
// 1) Initialize UART1 on PC4 PC5
// 2) Implement the FIFO as a class
// 3) Run transmitter with busy-wait synchronization
// 4) Run receiver with 1/2 full FIFO interrupt
// Daniel and Jonathan Valvano
// 1/12/2021

// EE319K Lab 9, use U1Rx connected to PC4 interrupt
// EE319K Lab 9, use U1Tx connected to PC5 busy wait
// EE319K Lab 9 hardware
// System 1        System 2
//   PC4 ----<<----- PC5
//   PC5 ---->>----- PC4
//   Gnd ----------- Gnd
// When running on one board connect PC4 to PC5

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

#include "FIFO.h"
#include "UART1.h"

#define PF1  (*((volatile uint32_t *)0x40025008))

extern "C" void DisableInterrupts(void); // Disable interrupts
extern "C" long StartCritical (void);    // previous I bit, disable interrupts
extern "C" void EndCritical(long sr);    // restore I bit to previous value
extern "C" void WaitForInterrupt(void);  // low power mode
extern "C" void EnableInterrupts(void);

extern "C" void UART1_Handler(void);
#define NVIC_EN0_INT6   0x00000040  // Interrupt 6 enable
Queue RxFifo;
// Initialize UART1
// Baud rate is 1000 bits/sec
// Lab 9
void UART1_Init(void){
  // write this
}


// input ASCII character from UART
// spin if software RxFifo is empty
// Lab 9
char UART1_InChar(void){
    // write this
 return 0; // remove this line
}
// Lab 9
// check software RxFifo
bool UART1_InStatus(void){
  // write this
 return false; // remove this line
}

// output ASCII character to UART
// busy wait
// spin if hardware not ready
// Lab 9
// in Lab 9 this will never wait
void UART1_OutChar(char data){
    // write this

}
// one of two things has happened:
// 1) hardware RX FIFO goes from 7 to 8 or more items
// 2) receiver timeout (data in hardware Rx but no activity)
// Lab 9
void UART1_Handler(void){
  PF1  ^= 0x02; // single toggle debugging
    // write this


}

//------------UART1_InMessage------------
// Accepts ASCII characters from the serial port
//    and adds them to a string until ETX is typed
//    or until max length of the string is reached.
// Input: pointer to empty buffer of 8 characters
// Output: Null terminated string
// removes STX CR ETX
void UART1_InMessage(char *bufPt){
  // write this


}
