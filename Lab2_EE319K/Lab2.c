// ****************** Lab2.c ***************
// Program written by: put your names here
// Date Created: 1/18/2017 
// Last Modified: 1/17/2021 
// Put your name and EID in the next two line
char Name[] = "Ayan Basu";
char EID[] = "ab73287";
// Brief description of the Lab: 
// An embedded system is capturing temperature data from a
// sensor and performing analysis on the captured data.
// The controller part of the system is periodically capturing size
// readings of the temperature sensor. Your task is to write three
// analysis routines to help the controller perform its function
//   The three analysis subroutines are:
//    1. Calculate the mean of the temperature readings 
//       rounded down to the nearest integer
//    2. Convert from Centigrate to Farenheit using integer math 
//    3. Check if the captured readings are a non-increasing monotonic series
//       This simply means that the readings are sorted in non-increasing order.
//       We do not say "decreasing" because it is possible for consecutive values
//       to be the same, hence the term "non-increasing". The controller performs 
//       some remedial operation and the desired effect of the operation is to 
//       raise the the temperature of the sensed system. This routine helps 
//       verify whether this has indeed happened
#include <stdint.h>
#define True 1
#define False 0

// Return the computed Mean
// Inputs: Data is an array of 16-bit unsigned distance measurements
//         N is the number of elements in the array
// Output: Average of the data
// Notes: you do not need to implement rounding
uint16_t Average(const uint16_t Data[],const uint32_t N){
// Replace this following line with your solution
	uint16_t sum = 0;
	for (int i=0; i < N; i++) {
		sum += Data[i]; 
	} 
	return sum / N;
}

// Convert temperature in Farenheit to temperature in Centigrade
// Inputs: temperature in Farenheit 
// Output: temperature in Centigrade
// Notes: you do not need to implement rounding
int16_t FtoC(int16_t const TinF){
// Replace this following line with your solution
  
  return (((TinF - 32) * 5) / 9); 
}

// Return True of False based on whether the readings
// are an increasing monotonic series
// Inputs: Readings is an array of 16-bit distance measurements
//         N is the number of elements in the array
// Output: true if monotonic increasing, false if nonmonotonic
int IsMonotonic(uint16_t const Data[], int32_t const N){
// Replace this following line with your solution	
	// Check if values in Readings array are monotonic
	for (int i = 0; i <= N-2; i++)
	{
		if (Data[i] > Data[i+1])
		{ // Return False if value is greater than next value, i.e. not monotonic increasing
			return False;
		}
	}
	// Return True if no values were greater than next value
  return True;
}


