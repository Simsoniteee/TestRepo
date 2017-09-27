//###################################################################

// File: UART.h
// Hardware: TMS320F28379D on LAUNCHXL-F28379D
// Project:
// Author: Simon
// Date: 21.09.2017
// Revison:
// Info:

//###################################################################

#ifndef _UART_H
#define _UART_H

//
// Included Files
//

//
// #Defines
//
#define CR   0x0D
#define LF   0x0A
#define BS   0x08
#define ESC  0x1B
#define SP   0x20
#define DEL  0x7F

//
// Prototypes
//
void uartInit(void);

void uartOutChar(char cData);
void uartOutCRLF(void);
void uartOutString(char *cPtr);
void uartOutDec(uint32_t ui32DecNum);

char uartInChar(void);
char uartInString(char *cPtr, uint32_t ui32MaxLength);
uint32_t uartInDec(void);

__interrupt void sciaRxISR(void);
__interrupt void sciaTxISR(void);

//
// Global Variables
//

#endif /* UART_H */
