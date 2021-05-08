// UART1.c
// Runs on TM4C123
// Use UART1 to implement bidirectional data transfer to and from 
// another microcontroller in Lab 9.  This time, interrupts and FIFOs
// are used.
// Student names: Ayan Basu (Ayush Roychowdhury was my partner, but he Q-Dropped the EE319K class)
// Last modification date: 04/20/2021
// Last Modified: 1/17/2021 

/* Lab 9 code 
 http://users.ece.utexas.edu/~valvano/
*/

// U1Rx (VCP receive) connected to PC4
// U1Tx (VCP transmit) connected to PC5
// When running on one board connect PC4 to PC5

#include <stdint.h>
#include "Fifo.h"
#include "UART1.h"
#include "../inc/tm4c123gh6pm.h"

const uint8_t dataSize[4] = {0, 3, 2, 2};

uint32_t dataLost;
uint32_t RxCounter;

// Initialize UART1
// Baud rate is 1000 bits/sec
// Interrupt on Rx hardware FIFO 1/2 full
// Busy-wait for Tx
void UART1_Init(void){
 // write this
	//SYSCTL_RCGC1_R |= 0x00000002;	// Activate UART1	// Step 1 in clock Initialization
	//SYSCTL_RCGC2_R |= 0x00000004;	// Activate Port C // Step 2 in Clock Initialization
	SYSCTL_RCGCUART_R |= 0x00000002; // Activate UART1
	SYSCTL_RCGCGPIO_R |= 0x00000004; // Activate port C
	UART1_CTL_R &= ~0x00000001;		// Disable UART during initialization
	//while((SYSCTL_PRGPIO_R & 0x01) == 0) {}
	UART1_IBRD_R = 5000; // IBRD = int(80,000,000 / (16 * 1000)) = int(5000.000)
	UART1_FBRD_R = 0;		 // FBRD = round(000 & 64) = 0
	UART1_LCRH_R = 0x00000070;	// 8-bit, no parity bits, one stop, FIFOs
	UART1_CTL_R |= 0x00000301;	// Enable UART --> Enable transmitter & receiver for UART1
	GPIO_PORTC_AFSEL_R |= 0x30;	// Enable Alt function on PC5 & PC4
	GPIO_PORTC_DEN_R |= 0x30;		// Configure PC5 & PC4 as UART1 --> digital I/O on PC5 & PC4
	GPIO_PORTC_PCTL_R = (GPIO_PORTC_PCTL_R & 0xFF00FFFF) + 0x00220000;
	UART1_IFLS_R &= ~0x010; //(UART1_IFLS_R & ~0x38) + 0x10; //0x020;	// Bits 4,5,3 to 010 (1/2 full --> 0x020)
	UART1_IM_R |= 0x10; // Arm RxRIS --> Receiver initialization		
	NVIC_PRI1_R |= 0x00E00000;	// Bits 21-23
	NVIC_EN0_R |= 0x40; // Enable interrupt 6 in NVIC
	RxCounter = 0;
	dataLost = 0;
	Fifo_Init();
		
}

//------------UART1_InChar------------
// Wait for new input, interrupt driven
// then return ASCII code
// Checks the software receive FIFO, not the hardware
// Input: none
// Output: char read from UART
char UART1_InChar(void){
   // write this
	while((UART1_FR_R & 0x0010) != 0); // Wait until RxFE is 0
	return ((unsigned char) (UART1_DR_R & 0xFF));
}

// Lab 9
// check software RxFifo
uint32_t UART1_InStatus(void){
   // write this
  return Fifo_Status();
}

//------------UART1_InMessage------------
// Accepts ASCII characters from the serial port
//    and adds them to a string until ETX is received
//    or until max length of the string is reached.
// Input: pointer to empty buffer of 8 characters
// Output: Null terminated string
// Note: strips off STX CR and ETX
void UART1_InMessage(char *bufPt){
  // write this
	
	//calls Fifo(Get)()
	uint8_t count = 0;
	while(UART1_InChar() != 0x03)
	{
		if(UART1_InChar() != 0x02)
		{
			bufPt[count] = Fifo_Get();
			count++;
		}
	}
}
//------------UART1_OutChar------------
// Output 8-bit to serial port
// Input: letter is an 8-bit ASCII character to be transferred
// Output: none
// Busy-wait synchronization
// Interesting note for Lab9: it will never wait
void UART1_OutChar(char data){
  // write this
	while((UART1_FR_R & 0x0020) != 0);	// Wait until TxFF is 0
	UART1_DR_R = data;
}

// hardware RX FIFO goes from 7 to 8 or more items
// Interrupts after receiving entire message

#define PF1       (*((volatile uint32_t *)0x40025008))
void UART1_Handler(void){
  PF1 ^= 0x02;
  // write this
	uint8_t i;
	for(i = 0; i < dataSize[i]; i++)
	{
		if(Fifo_Put(UART1_InChar()) == 0)	// Put received data into FIFO
		{
			dataLost++;
			break;
		}
	}
	UART1_ICR_R = 0x10; // Ask the interrupt --> Must do; failing to do this causes repeated interrupts
}




