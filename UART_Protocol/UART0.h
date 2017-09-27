//*****************************************************************************
/*

File: UART0.h
Hardware: TM4C123GH6PM on TM4C123GXL
Project:
Author: Simon
Date: 26.04.2016
Revison: v01
Info: Communication via UART and Software FIFO
Interrupts and HW Fifos are used
This connection occurs in the USB debugging cable:
	U0Rx (PA0) connected to serial port on PC
	U0Tx (PA1) connected to serial port on PC
	Ground connected ground in the USB cable

*/
//*****************************************************************************

/*****************************************************
					Preprocessor
******************************************************/

#ifndef _UART0_H
#define _UART0_H

#include <stdint.h>

// standard ASCII symbols
#define CR   0x0D
#define LF   0x0A
#define BS   0x08
#define ESC  0x1B
#define SP   0x20
#define DEL  0x7F

#define NVIC_UART0_INT 0x00000020	// Interrupt 5 in NVIC EN0 register is UART0

/*****************************************************
				UART0 Input/Output Functions
******************************************************/

//********************* UART0_Init *********************
// Initialize the UART for BAUDRATE 8 bit word length,
// no parity bits, one stop bit, FIFOs enabled
// Input: none
// Output: none
// Info:
void UART0_Init(void);

//********************* UART0_OutCRLF *********************
// Output a CR,LF to UART to go to a new line
// Input: none
// Output: none
// Info:
void UART0_OutCRLF(void);

//********************* UART0_InChar *********************
// Waits for new serial port input
// Input: none
// Output: one byte of data
// Info:
uint8_t UART0_InChar(void);

//********************* UART0_InCharNonBlocking *********************
// Immediately return input or 0 if no input
// Input: none
// Output: one byte of data
// Info:
uint8_t UART0_InCharNonBlocking(void);

//********************* UART0_OutChar *********************
// Writes one word (8 bit) to serial port
// Input: data is an 8bit character to be transfered
// Output: none
// Info:
void UART0_OutChar(uint8_t ui8Byte);

//********************* UART0_InString *********************
// Accepts ASCII characters from the serial port
// and adds them to a string until <enter> is typed
// or until ui32MaxLength of the string is reached.
// Input: pointer to empty buffer
// Output: null terminated string
// Info: each char is echoed
uint8_t UART0_InString(uint8_t *ui8Ptr, uint32_t ui32MaxLength);

//********************* UART0_OutString *********************
// Outputs a string of ASCII characters to the UART0
// Input: pointer to a NULL-terminated string to be transfered
// Output: none
// Info:
void UART0_OutString(uint8_t *ui8Ptr);

//********************* UART0_InDec *********************
// Accepts ASCII input in unsigned decimal format
// Input: none
// Output: 32 bit unsigned integer
// Info: valid range is 0 to 4294967295 (2^32-1)
// Everything above will return an incorrect value
uint32_t UART0_InDec(void);

//********************* UART0_OutDec *********************
// Output a 32 bit number in unsigned decimal format
// Input: 32 bit number to be transfered
// Output: none
// Info: recursive function to convert decimal number
// of unspecified length as ASCII string
void UART0_OutDec(uint32_t ui32DecNum);

//********************* UART0_InHex *********************
// Accepts ASCII input in unsigned hexadecimal (base 16) format
// Input: none
// Output: 32 bit unsigned number
// Info:
uint32_t UART0_InHex(void);

//********************* UART0_OutHex *********************
// Output a 32 bit number in unsigned hexadecimal format
// Input: 32 bit number to be transferred
// Output: none
// Info: Variable format 1 to 8 digits with no space before or after (recursiv)
void UART0_OutHex(uint32_t ui32HexNum);

//********************* UART0_InReal *********************
// Accepts ASCII input in unsigned real number format "x.xxx"
// with three decimal places
// Input: none
// Output: floating point number
// Info:
float UART0_InReal(void);

//********************* UART0_OutReal *********************
// Outputs a 32bit integer on the UART in the format "x.xxx"
// with three decimal places
// Input: 32 bit decimal value
// Output: none
// Info:
void UART0_OutReal(float fRealNum);

//********************* UART0_HardToSoft *********************
// Copys data from Hardware to Software (e.g. HW-Fifo to SW-Fifo)
// Input: none
// Output: none
// Info:
void UART0_CopyHW2SW(void);

//********************* UART0_SoftToHard *********************
// Copys data from Software to Hardware (e.g. SW-Fifo to HW-Fifo)
// Input: none
// Output: none
// Info:
void UART0_CopySW2HW(void);

/*****************************************************
				UART0 Software Fifo
******************************************************/

// Makros for UART0 Software Fifo
#define _UART0_FIFOSIZE_ 64	// can hold 5 because of full/empty indication in fifo_put and fifo_get
#define _UART0_FIFOMASK_ (_UART0_FIFOSIZE_-1)

//############### Rx Fifo ###############//

static uint8_t UART0_RxBuffer[_UART0_FIFOSIZE_];
uint8_t UART0_RxPut;
uint8_t UART0_RxGet;

//********************* UART0_Rxfifo_Init() *********************
// Initialize Rx index fifo
// Input:
// Output:
// Info:
void UART0_Rxfifo_Init(void);

//********************* UART0_Rxfifo_put() *********************
// Puts a byte of data in the rx software fifo
// Input: databyte
// Output: Returns FAIL if fifo is full. Returns SUCCESS otherwise
// Info:
uint8_t UART0_Rxfifo_put(uint8_t data);

//********************* UART0_Rxfifo_get() *********************
// Grabs a byte of data from the rx fifo
// Input: pointer to variable
// Output: returns SUCCESS if grabbing data was successful. Returns FAIL if fifo is empty
// Info:
uint8_t  UART0_Rxfifo_get(uint8_t *data);

//********************* UART0_Rxfifo_size() *********************
// Calculates the size of rx fifo
// Input: none
// Output: fifosize = bytes in fifo
// Info:
uint8_t UART0_Rxfifo_size(void);

//############### Tx Fifo ###############//

static uint8_t UART0_TxBuffer[_UART0_FIFOSIZE_];
uint8_t UART0_TxPut;
uint8_t UART0_TxGet;

//********************* UART0_Txfifo_Init() *********************
// Initialize tx index fifo
// Input:
// Output:
// Info:
void UART0_Txfifo_Init(void);

//********************* UART0_Txfifo_put() *********************
// Puts a byte of data in the tx software fifo
// Input: databyte
// Output: Returns FAIL if fifo is full. Returns SUCCESS otherwise
// Info:
uint8_t UART0_Txfifo_put(uint8_t data);

//********************* UART0_Txfifo_get() *********************
// Grabs a byte of data from the tx fifo
// Input: pointer to variable
// Output: returns SUCCESS if grabbing data was successful. Returns FAIL if fifo is empty
// Info:
uint8_t  UART0_Txfifo_get(uint8_t *data);

//********************* UART0_Txfifo_size() *********************
// Calculates the size of tx fifo
// Input: none
// Output: fifosize = bytes in fifo
// Info:
uint8_t UART0_Txfifo_size(void);

#endif
