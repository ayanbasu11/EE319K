// Fifo.h
// Runs on TM4C123
// Provide functions that implement the Software FiFo Buffer
// Last Modified: 1/17/2021 
// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly


#include <stdint.h>

// *********** Fifo_Init**********
// Initializes a software FIFO of a
// fixed size and sets up indexes for
// put and get operations
void Fifo_Init(void);

// *********** Fifo_Put**********
// Adds an element to the FIFO
// Input: Character to be inserted
// Output: 1 for success and 0 for failure
//         failure is when the buffer is full
uint32_t Fifo_Put(char data);

// *********** Fifo_Get**********
// Gets an element from the FIFO
// Input: none
// Output: removed character from FIFO
//         0 failure is when the buffer is empty
char Fifo_Get(void);

// *********** Fifo_Status**********
// returns number of elements in the FIFO
// Input: none
// Output: number of entries in FIFO
//         0 failure is when the FIFO is empty
uint32_t Fifo_Status(void);

