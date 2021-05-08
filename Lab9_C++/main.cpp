// main.c
// Runs on TM4C123
// Student names: put your names here
// Last modification date: change this to the last modification date or look very silly
// Last Modified: 1/11/2021 

// Analog Input connected to PD2=ADC5
// displays on SSD1306 
// PF3, PF2, PF1 are heartbeats
// EE319K Lab 9, use U1Rx connected to PC4 interrupt
// EE319K Lab 9, use U1Tx connected to PC5 busy wait
// EE319K Lab 9 hardware
// System 1        System 2
//   PC4 ----<<----- PC5
//   PC5 ---->>----- PC4
//   Gnd ----------- Gnd
// To run with one LaunchPad, connect PC4 to PC5
// main1 Understand UART interrupts
// main2 Implement and test the FIFO class on the receiver end 
//   import SSD1306 code from Lab 7,8
// main3 convert UART0 to UART1, implement busy-wait on transmission
// final main for Lab 9
//   Import SlidePot and ADC code from Lab8. 
//   Figure out what to do in UART1_Handler ISR (receive message)
//   Figure out what to do in SysTickHandler (sample, convert, transmit message)
//   Figure out what to do in main (LCD output)


#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "TExaS.h"
#include "SSD1306.h"
#include "SlidePot.h"
#include "print.h"
#include "UART1.h"
#include "FIFO.h"

SlidePot my(150,0);

extern "C" void DisableInterrupts(void);
extern "C" void EnableInterrupts(void);
extern "C" void SysTick_Handler(void);
extern "C" void LogicAnalyzerTask(void);
#define PC54                  (*((volatile uint32_t *)0x400060C0)) // bits 5-4
#define PF321                 (*((volatile uint32_t *)0x40025038)) // bits 3-1
// TExaSdisplay logic analyzer shows 7 bits 0,PC5,PC4,PF3,PF2,PF1,0 
void LogicAnalyzerTask(void){
  UART0_DR_R = 0x80|PF321|PC54; // sends at 10kHz
}

// PF1 toggled in UART ISR (receive data)
// PF2 toggled in SysTick ISR (1 Hz sampling)
// PF3 toggled in main program when outputing to OLED
#define PF1  (*((volatile uint32_t *)0x40025008))
#define PF2  (*((volatile uint32_t *)0x40025010))
#define PF3  (*((volatile uint32_t *)0x40025020))
#define PF4  (*((volatile uint32_t *)0x40025040))

// **************SysTick_Init*********************
// Initialize Systick periodic interrupts
// Input: interrupt period
//        Units of period are 12.5ns
//        Maximum is 2^24-1
//        Minimum is determined by length of ISR
// Output: none
void SysTick_Init(unsigned long period){
    // write this

}

// Initialize Port F so PF1, PF2 and PF3 are heartbeats
void PortF_Init(void){
  SYSCTL_RCGCGPIO_R |= 0x20;      // activate port F
  while((SYSCTL_PRGPIO_R&0x20) != 0x20){};
  GPIO_PORTF_DIR_R |=  0x0E;   // output on PF3,2,1 (built-in LED)
  GPIO_PORTF_PUR_R |= 0x10;
  GPIO_PORTF_DEN_R |=  0x1E;   // enable digital I/O on PF
}


// main1 test the FIFO class
// FIFO.h is prototype
// FIFO.cpp is implementation
Queue FIFO;
int main1(void){
  char data = 0; char out;
  DisableInterrupts();
  TExaS_Init(&LogicAnalyzerTask);
  SSD1306_Init(SSD1306_SWITCHCAPVCC);
  SSD1306_OutClear(); 
  PortF_Init();
  
  while(1){
    int count=0;
    for(int i=0; i<7; i++){
      if(FIFO.Put(data)){
        count++;
        data++;
      }
    }
    FIFO.Print();
    for(int i=0; i<count; i++){
      FIFO.Get(&out);
    }   
    PF3 ^= 0x08;   
  }
}


// main2 used to test UART1
// Connect PC5=PC4
// Use queue class in receiver interrupt
// UART1 receiver interrupt to 1/2 full 
// UART1 transmitter is busy wait
// PF1 toggles in UART ISR
// PF3 toggles in main
int main2(void){
  char OutData = '0'; 
  char InData;
  uint32_t time=0;
  DisableInterrupts();
  TExaS_Init(&LogicAnalyzerTask);
  SSD1306_Init(SSD1306_SWITCHCAPVCC);
  SSD1306_OutClear(); 
  PortF_Init();
  UART1_Init();       // enable UART
  EnableInterrupts();
  while(1){           
    time++;
    if((time%100000)==0){
      UART1_OutChar(STX);
      UART1_OutChar('1');
      UART1_OutChar('.');
      UART1_OutChar('2');
      UART1_OutChar(OutData);
      if(OutData == '9'){
        OutData = '0';
      }else{
        OutData++;
      }
      UART1_OutChar(' ');
      UART1_OutChar(CR);
      UART1_OutChar(ETX);
    }
    if(UART1_InStatus()){
      InData = UART1_InChar();
      SSD1306_OutChar(InData);
      PF3 ^= 0x08;
    }
  }
}

// final main program for bidirectional communication
// Sender sends using SysTick Interrupt
// Receiver receives using RX
int main(void){  // valvano version
  DisableInterrupts();
  TExaS_Init(&LogicAnalyzerTask);
  SSD1306_Init(SSD1306_SWITCHCAPVCC);
  SSD1306_OutClear(); 
  ADC_Init(SAC_32);    // initialize to sample ADC
  PortF_Init();
  UART1_Init();    // initialize UART
  // other initializations
//Enable SysTick Interrupt by calling SysTick_Init()
  EnableInterrupts();

  while(1){ 
      // write this

 
  }
}




void SysTick_Handler(void){ 

  
    // write this

}

