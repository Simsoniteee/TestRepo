//*****************************************************************************
/*

File: UART0.c
Hardware: TM4C123GH6PM on TM4C123GXL
Project:
Author: Simon
Date: 15.05.2016
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

#include "SYSTEM.h"
#include "UART0.h"

// Marko to calculate IBRD and FBRD for BAUDRATE
#define BRD (float)BUSFREQUENCY/(16*BAUDRATE)

/*****************************************************
			UART0 Input/Output Functions
******************************************************/

//********************* UART0_Init *********************
// Initialize the UART for BAUDRATE 8 bit word length,
// no parity bits, one stop bit, FIFOs enabled
// Input: none
// Output: none
// Info:
void UART0_Init(void)
{
	uint32_t IBRD, FBRD;
	IBRD = (int)BRD;
	FBRD = (int)((BRD-(float)IBRD)*64+0.5);

	SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R0;						// activate UART0
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;						// activate port A
	UART0_Rxfifo_Init();											// initialize a software RX Fifo for UART0
	UART0_Txfifo_Init();											// initialize a software TX Fifo for UART0
	while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R0) == 0){}; 				// wait for port A to be ready

	UART0_CTL_R &= ~UART_CTL_UARTEN;								// disable UART
	UART0_IBRD_R = IBRD;											// IBRD = int(40,000,000 / (16 * 115,200)) = int(21.7014)
	UART0_FBRD_R = FBRD;											// FBRD = int(0.7014 * 64 + 0.5) = 45
	UART0_LCRH_R = (UART_LCRH_WLEN_8|UART_LCRH_FEN);				// 8 bit word length (no parity bits, one stop bit, FIFOs)
	UART0_IFLS_R &= ~(UART_IFLS_RX_M|UART_IFLS_TX_M); 				// clear TX and RX interrupt FIFO level fields
	UART0_IFLS_R += (UART_IFLS_TX1_8); 								// configure interrupt for TX FIFO <= 1/8 full
	UART0_IFLS_R += (UART_IFLS_RX7_8); 								// configure interrupt for RX FIFO >= 7/8 full
	UART0_IM_R |= (UART_IM_RXIM|UART_IM_TXIM|UART_IM_RTIM);			// enable TX and RX FIFO interrupts and RX time-out interrupt
	UART0_CTL_R |= UART_CTL_UARTEN;       							// enable UART

	GPIO_PORTA_AFSEL_R |= 0x03;           							// enable alt funct on PA1-0
	GPIO_PORTA_DEN_R |= 0x03;             							// enable digital I/O on PA1-0
	GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R&0xFFFFFF00)+0x00000011;	// configure PA1-0 as UART
	GPIO_PORTA_AMSEL_R &= ~0x03;          							// disable analog functionality on PA

	NVIC_PRI1_R = (NVIC_PRI1_R&0xFFFF00FF)|0x00004000; 				// UART0=priority 2 -> bits 13-15
	NVIC_EN0_R = NVIC_UART0_INT;           							// enable interrupt 5 in NVIC
}

//********************* UART0_OutCRLF *********************
// Output a CR,LF to UART to go to a new line
// Input: none
// Output: none
// Info:
void UART0_OutCRLF(void)
{
	UART0_OutChar(CR);
	UART0_OutChar(LF);
}

//********************* UART0_InChar *********************
// Waits for new serial port input
// Input: none
// Output: one byte of data
// Info:
uint8_t UART0_InChar(void)
{
	uint8_t ui8Byte;
	while(!UART0_Rxfifo_get(&ui8Byte)){};	// spin if there is no new data
	return ui8Byte;
}

//********************* UART0_InCharNonBlocking *********************
// Immediately return input or 0 if no input
// Input: none
// Output: one byte of data
// Info:
uint8_t UART0_InCharNonBlocking(void)
{
	uint8_t ui8Byte = 0;
	UART0_Rxfifo_get(&ui8Byte);
	return ui8Byte;
}

//********************* UART0_OutChar *********************
// Writes one word (8 bit) to serial port
// Input: data is an 8bit character to be transfered
// Output: none
// Info: spin if tx fifo is full
void UART0_OutChar(uint8_t ui8Byte)
{
	while(!UART0_Txfifo_put(ui8Byte)){};
	UART0_IM_R &= ~UART_IM_TXIM;          // disable Tx interrupt
	UART0_CopySW2HW();
	UART0_IM_R |= UART_IM_TXIM;           // enable Tx interrupt
}

//********************* UART0_InString *********************
// Accepts ASCII characters from the serial port
// and adds them to a string until <enter> is typed
// or until ui32MaxLength of the string is reached.
// Input: pointer to empty buffer
// Output:	0 if typed string is longer then ui32MaxLength
//			1 if Return was typed
// Info: each char is echoed
uint8_t UART0_InString(uint8_t *ui8Ptr, uint32_t ui32MaxLength)
{
	// initialize local variables
	uint32_t ui32Length = 0;
	uint8_t ui8Letter;

	ui8Letter = UART0_InChar();

	while (ui8Letter != CR)					// as long as Return is not pressed
	{
		if(ui8Letter == DEL)				// DEL was pressed -> what about Backspace?
		{
			if(ui32Length)					// only if valid chars have allready been typed
			{
				ui8Ptr--;					// move back in the string
				ui32Length--;
			}
		}
		else if(ui32Length < ui32MaxLength)	// current length is within maximum allowed size
		{
			*ui8Ptr = ui8Letter;
			ui8Ptr++;
			ui32Length++;
		}
		else								// ui32Length has reached max allowed size
		{
			return 0;
		}
		UART0_OutChar(ui8Letter);			// echo
		ui8Letter = UART0_InChar();			// grab next char
	}

	*ui8Ptr=0;								// Null-Termination for string
	return 1;
}

//********************* UART0_OutString *********************
// Outputs a string of ASCII characters to the UART0
// Input: pointer to a NULL-terminated string to be transfered
// Output: none
// Info:
void UART0_OutString(uint8_t *ui8Ptr)
{
	while(*ui8Ptr)
	{
		UART0_OutChar(*ui8Ptr);
		ui8Ptr++;
	}
}

//********************* UART0_InDec *********************
// Accepts ASCII input in unsigned decimal format
// Input: none
// Output: 32 bit unsigned integer
// Info: valid range is 0 to 4294967295 (2^32-1)
// Everything above will return an incorrect value
uint32_t UART0_InDec(void)
{
	// initialize local variables
	uint32_t ui32Length = 0;
	uint32_t ui32Number = 0;
	uint8_t ui8Character;

	ui8Character = UART0_InChar();								// grab the first character

	while (ui8Character != CR)									// as long as Return is not pressed
	{
		if(ui8Character >= 0x30 && ui8Character <= 0x39)		// valid data: 0x30 = '0' and 0x39 = '9'
		{
			ui32Number = ui32Number*10+(ui8Character - 0x30);	// New = Old * 10 + (Input-Offset)
			ui32Length++;
			UART0_OutChar(ui8Character);
		}
		else if(ui8Character == DEL && ui32Length)				// Backspace and valid chars have allready been typed
		{
			ui32Number /= 10;									// New = Old/10;
			ui32Length--;
			UART0_OutChar(ui8Character);
		}
		ui8Character = UART0_InChar();							// grab the next character
	}
	return ui32Number;
}

//********************* UART0_OutDec *********************
// Output a 32 bit number in unsigned decimal format
// Input: 32 bit number to be transfered
// Output: none
// Info: recursive function to convert decimal number
// of unspecified length as ASCII string
void UART0_OutDec(uint32_t ui32DecNum)
{
	if(ui32DecNum >= 10)
	{
		UART0_OutDec(ui32DecNum/10);
		ui32DecNum = ui32DecNum%10;
	}
	UART0_OutChar(ui32DecNum+0x30); /* n is between 0 and 9 */
}

//********************* UART0_InHex *********************
// Accepts ASCII input in unsigned hexadecimal (base 16) format
// Input: none
// Output: 32 bit unsigned number
// Info:
/*
uint32_t UART0_InHex(void)
{
	uint32_t ui32HexNum;
	// code to be written
	return ui32HexNum;
}
*/

//********************* UART0_OutHex *********************
// Output a 32 bit number in unsigned hexadecimal format
// Input: 32 bit number to be transferred
// Output: none
// Info: Variable format 1 to 8 digits with no space before or after (recursiv)
void UART0_OutHex(uint32_t ui32HexNum)
{
	if(ui32HexNum >= 0x10)
	{
		UART0_OutHex(ui32HexNum/0x10);
		UART0_OutHex(ui32HexNum%0x10);
	}
	else
	{
		if(ui32HexNum < 0xA)
		{
			UART0_OutChar(ui32HexNum+'0');
		}
		else
		{
			UART0_OutChar((ui32HexNum-0x0A)+'A');
		}
	}
}

//********************* UART0_HardToSoft *********************
// Copys data from Hardware to Software (e.g. HW-Fifo to SW-Fifo)
// Input: none
// Output: none
// Info:
void UART0_CopyHW2SW(void)
{
	while(((UART0_FR_R&UART_FR_RXFE) == 0) && UART0_Rxfifo_put(UART0_DR_R));	// as long as there is data in the HW-Fifo and SW-Fifo is not full
}

//********************* UART0_SoftToHard *********************
// Copys data from Software to Hardware (e.g. SW-Fifo to HW-Fifo)
// Input: none
// Output: none
// Info:
void UART0_CopySW2HW(void)
{
	uint8_t ui8Byte;
	while(((UART0_FR_R&UART_FR_TXFF) == 0) && UART0_Txfifo_get(&ui8Byte))	// as long HW-Fifo is not full and SW-Fifo is not empty
	{
		UART0_DR_R = ui8Byte;
	}
}

//********************* UART0_Handler *********************
// Interrupt handler for UART0 Tx and Rx and Rx-Timeout.
// Input: none
// Output: none
// Info: This is externally declared in the startup file
void UART0_Handler(void)
{
	if(UART0_RIS_R&UART_RIS_RXRIS)		// Rx interrupt
	{
		UART0_ICR_R = UART_ICR_RXIC;	// acknowledge Rx interrupt
		UART0_CopyHW2SW();
	}
	if(UART0_RIS_R&UART_RIS_TXRIS)		// Tx interrupt
	{
		UART0_ICR_R = UART_ICR_TXIC;	// acknowledge Rx interrupt
		UART0_CopySW2HW();
	    if(UART0_Txfifo_size() == 0)	// software TX FIFO is empty
	    {
	      UART0_IM_R &= ~UART_IM_TXIM;  // disable TX FIFO interrupt
	    }
	}
	if(UART0_RIS_R&UART_RIS_RTRIS)		// receiver timed out
	{
	    UART0_ICR_R = UART_ICR_RTIC;    // acknowledge receiver time out
	    UART0_CopyHW2SW();
	}
}

/*****************************************************
				UART0 Software Fifo
******************************************************/

//############### Rx Fifo ###############//

//********************* UART0_Rxfifo_Init() *********************
// Initialize Rx index fifo
// Input:
// Output:
// Info:
void UART0_Rxfifo_Init(void)
{
	UART0_RxPut = UART0_RxGet = 0;
}

//********************* UART0_Rxfifo_put() *********************
// Puts a byte of data in the rx software fifo
// Input: databyte
// Output: Returns FAIL if fifo is full. Returns SUCCESS otherwise
// Info:
uint8_t UART0_Rxfifo_put(uint8_t data)
{
	uint8_t next = ((UART0_RxPut + 1) & _UART0_FIFOMASK_);
	if (UART0_RxGet == next)
	{
		return(FAIL);
	}
	UART0_RxBuffer[UART0_RxPut] = data;
	UART0_RxPut = next;
	return(SUCCESS);
}

//********************* UART0_Rxfifo_get() *********************
// Grabs a byte of data from the rx fifo
// Input: pointer to variable
// Output: returns SUCCESS if grabbing data was successful. Returns FAIL if fifo is empty
// Info:
uint8_t  UART0_Rxfifo_get(uint8_t *data)
{
	if ( UART0_RxGet == UART0_RxPut)
	{
		return(FAIL);
	}
	*data = UART0_RxBuffer[UART0_RxGet];
	UART0_RxGet = (UART0_RxGet + 1) & _UART0_FIFOMASK_;
	return(SUCCESS);
}

//********************* UART0_Rxfifo_size() *********************
// Calculates the size of rx fifo
// Input: none
// Output: fifosize = bytes in fifo
// Info:
uint8_t UART0_Rxfifo_size(void)
{
	return (UART0_RxPut - UART0_RxGet);
}

//############### Tx Fifo ###############//

//********************* UART0_Txfifo_Init() *********************
// Initialize tx index fifo
// Input:
// Output:
// Info:
void UART0_Txfifo_Init(void)
{
	UART0_TxPut = UART0_TxGet = 0;
}

//********************* UART0_Txfifo_put() *********************
// Puts a byte of data in the tx software fifo
// Input: databyte
// Output: Returns FAIL if fifo is full. Returns SUCCESS otherwise
// Info:
uint8_t UART0_Txfifo_put(uint8_t data)
{
	uint8_t next = ((UART0_TxPut + 1) & _UART0_FIFOMASK_);
	if (UART0_TxGet == next)
	{
		return(FAIL);
	}
	UART0_TxBuffer[UART0_TxPut] = data;
	UART0_TxPut = next;
	return(SUCCESS);
}

//********************* UART0_Txfifo_get() *********************
// Grabs a byte of data from the tx fifo
// Input: pointer to variable
// Output: returns SUCCESS if grabbing data was successful. Returns FAIL if fifo is empty
// Info:
uint8_t  UART0_Txfifo_get(uint8_t *data)
{
	if ( UART0_TxGet == UART0_TxPut)
	{
		return(FAIL);
	}
	*data = UART0_TxBuffer[UART0_TxGet];
	UART0_TxGet = (UART0_TxGet + 1) & _UART0_FIFOMASK_;
	return(SUCCESS);
}

//********************* UART0_Txfifo_size() *********************
// Calculates the size of tx fifo
// Input: none
// Output: fifosize = bytes in fifo
// Info:
uint8_t UART0_Txfifo_size(void)
{
	return (UART0_TxPut - UART0_TxGet);
}

