// UART1.h
// Runs on TM4C123
// Provides Prototypes for functions implemented in UART1.c
// Last Modified: 1/12/2021
// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly

// U1Rx (VCP receive) connected to PC4
// U1Tx (VCP transmit) connected to PC5
// When running on one board connect PC4 to PC5

#include <stdint.h>
#define STX 0x02
#define ETX 0x03
#define CR  0x0D

// UART1 initialization function 
// Input: none
// Output: none
// Baud rate is 1000 bits/sec
// Interrupt on Rx hardware FIFO 1/2 full
// Busy-wait for Tx
void UART1_Init(void);

//------------UART1_InChar------------
// Wait for new input, interrupt driven
// then return ASCII code
// Checks the software receive FIFO, not the hardware
// Input: none
// Output: char read from UART
char UART1_InChar(void);

// Lab 9
// check software RxFifo
uint32_t UART1_InStatus(void);

//------------UART1_InMessage------------
// Accepts ASCII characters from the serial port
//    and adds them to a string until ETX is received
//    or until max length of the string is reached.
// Input: pointer to empty buffer of 8 characters
// Output: Null terminated string
// Note: strips off STX CR and ETX
void UART1_InMessage(char *bufPt);

//------------UART1_OutChar------------
// Output 8-bit to serial port
// Input: letter is an 8-bit ASCII character to be transferred
// Output: none
// Busy-wait synchronization
// Interesting note for Lab9: it will never wait
void UART1_OutChar(char data);
