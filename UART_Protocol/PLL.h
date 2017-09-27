//*****************************************************************************
/*

File: PLL.h
Hardware: TM4C123GH6PM on TM4C123GXL
Project:
Author: Simon
Date: 26.04.2016
Revison: v01
Info: Software functions to change the bus frequency using the PLL.

*/
//*****************************************************************************

#ifndef _PLL_H
#define _PLL_H

//********************* PLL_Init *********************
// Configure the system to get its clock from the PLL at BUSFREQUENCY
// Input: none
// Output: none
// Info:
void PLL_Init(void);

#endif
