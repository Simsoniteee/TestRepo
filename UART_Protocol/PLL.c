//*****************************************************************************
/*

File: PLL.c
Hardware: TM4C123GH6PM on TM4C123GXL
Project:
Author: Simon
Date: 26.04.2016
Revison: v01
Info: Software functions to change the bus frequency using the PLL.

*/
//*****************************************************************************

/*****************************************************
					Preprocessor
******************************************************/

#include "SYSTEM.h"
#include "PLL.h"

#define SYSDIV (200000000UL/BUSFREQUENCY)-1	// when using the PLL: busfrequncy = 200Mhz/(SYSDIV+1)

/*****************************************************
					PLL Functions
******************************************************/

//********************* PLL_Init *********************
// Configure the system to get its clock from the PLL at BUSFREQUENCY
// Clock aource is the externel 16Mhz XTAL
// Input: none
// Output: none
// Info:
void PLL_Init(void)
{
	SYSCTL_RCC_R |= SYSCTL_RCC_BYPASS; // bypass PLL
	SYSCTL_RCC_R &= ~SYSCTL_RCC_USESYSDIV; // disable system clock divider while initializing
	SYSCTL_RCC_R &= ~SYSCTL_RCC_XTAL_M;   // clear XTAL field
	SYSCTL_RCC_R += SYSCTL_RCC_XTAL_16MHZ; // 16MHz crystal

	SYSCTL_RCC_R &= ~SYSCTL_RCC_OSCSRC_M; // clear oscillator source field
	SYSCTL_RCC_R |= SYSCTL_RCC_OSCSRC_MAIN; // use main oscillator
	SYSCTL_RCC_R &= ~SYSCTL_RCC_PWRDN; // clear power down bit

	SYSCTL_RCC_R &= ~0x07800000; // clear system clock divider field
	SYSCTL_RCC_R |= (SYSDIV<<23); // configure for BUSFREQUENCY

	while((SYSCTL_RIS_R&SYSCTL_RIS_PLLLRIS)==0){}; // wait for the PLL to lock by polling PLLLRIS

	SYSCTL_RCC_R &= ~SYSCTL_RCC_BYPASS; // enable use of PLL
}
