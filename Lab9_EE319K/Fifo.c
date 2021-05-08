// FiFo.c
// Runs on TM4C123
// Provide functions that implement the Software FiFo Buffer
// Last Modified: 1/17/2021 
// Student names: Ayan Basu (Ayush Roychowdhury was my partner, but he Q-Dropped the EE319K class)
// Last modification 04/20/2021
#include <stdint.h>

// Declare state variables for FiFo
//        size, buffer, put and get indexes
#define SIZE 16
#define NUM 1

static int *pointer;
static int FiFo[NUM * SIZE];
static int *Put;
static int *Get;
static int Buffer[NUM * SIZE];

// *********** FiFo_Init**********
// Initializes a software FIFO of a
// fixed size and sets up indexes for
// put and get operations
void Fifo_Init() {
//Complete this
	Put = &FiFo[0];
	Get = &FiFo[1];
	pointer = &FiFo[0];
}

// *********** FiFo_Put**********
// Adds an element to the FIFO
// Input: Character to be inserted
// Output: 1 for success and 0 for failure
//         failure is when the buffer is full
uint32_t Fifo_Put(char data) {
  //Complete this routine
	int success = 0;
	if(Put >= &FiFo[15])
	{
		success = 1;
		return success;
	}
	*Put = data;
	Put++;
	
	return success;
}

// *********** Fifo_Get**********
// Gets an element from the FIFO
// Input: none
// Output: removed character from FIFO
//         0 failure is when the buffer is empty
char Fifo_Get(void){char data;
  //Complete this routine
  int success = 0;
	if(Get >= &FiFo[15])
	{
		success = 1;
		return success;
	}
	data = *Get;
	if(*Get != 0)
	{
		Get++;
	}
	return success;
}

// *********** Fifo_Status**********
// returns number of elements in the FIFO
// Input: none
// Output: number of entries in FIFO
//         0 failure is when the FIFO is empty
uint32_t Fifo_Status(void){
  //Complete this routine
	return SIZE;   
}



