//###################################################################

// File: UART.c
// Hardware: TMS320F28379D on LAUNCHXL-F28379D
// Project:
// Author: Simon
// Date: 21.09.2017
// Revison:
// Info:

//###################################################################

//
// Included Files
//
#include "driverlib.h"
#include "device.h"
#include "UART.h"

//
// #Defines
//

//
// Local Prototypes
//

//
// Global Variables
//

//
// Function Definitions
//

// uartInit - Initialize the UART: baudrate=9600, wordlength=8, stopbit=1, parity=none, FIFOs enabled
// Input: none
// Output: none
void uartInit(void)
{
    //
    // After a system reset, re-enable the SCI
    //
    SCI_performSoftwareReset(SCIA_BASE);

    //
    // 8 char bits, 1 stop bit, no parity. Baud rate is 9600. Enable FIFOs
    //
    SCI_setConfig(SCIA_BASE, DEVICE_LSPCLK_FREQ, 9600, (SCI_CONFIG_WLEN_8 | SCI_CONFIG_STOP_ONE | SCI_CONFIG_PAR_NONE));

    //
    // enable and reset FIFOs
    //
    SCI_enableFIFO(SCIA_BASE);
    SCI_resetRxFIFO(SCIA_BASE);
    SCI_resetTxFIFO(SCIA_BASE);
    SCI_resetChannels(SCIA_BASE);

    //
    // Finally, enable the module
    //
    SCI_enableModule(SCIA_BASE);
}

//###################################################################
//							Output Functions
//###################################################################

// uartOutChar - Writes one word (8bit) to serial port
// Input: cData is an 8bit character to be transfered
// Output: none
void uartOutChar(char cData)
{
    SCI_writeCharBlockingFIFO(SCIA_BASE, cData);
}

// uartOutCRLF - Output a CR and LF to serial port.
// Input: none
// Output: none
void uartOutCRLF(void)
{
    uartOutChar(CR);
    uartOutChar(LF);
}

// uartOutString - Outputs a string of ASCII characters to the serial port
// Input: pointer to a NULL-terminated string to be transfered
// Output: none
void uartOutString(char *cPtr)
{
    while(*cPtr)
    {
        uartOutChar(*cPtr);
        cPtr++;
    }
}

// uartOutDec - Output a 32 bit number in unsigned decimal format
// This is a recursive function to convert decimal number of unspecified length as ASCII string
// Input: 32 bit unsigned number to be transfered
// Output: none
void uartOutDec(uint32_t ui32DecNum)
{
    if(ui32DecNum >= 10)
    {
        uartOutDec(ui32DecNum/10);
        ui32DecNum = ui32DecNum%10;
    }
    uartOutChar(ui32DecNum+0x30); /* n is between 0 and 9 */
}

//###################################################################
//							Input Functions
//###################################################################

// uartInChar - Grabs one char from serial port
// Input: none
// Output: one character from serial port
char uartInChar(void)
{
    char cData;
    cData = (char)SCI_readCharBlockingFIFO(SCIA_BASE);
    return cData;
}

// uartInString - Accepts ASCII characters from the serial port
// and adds them to a string until <enter> is typed
// or until ui32MaxLength of the string is reached.
// Input: Pointer to empty buffer, Length of the buffer
// Output:  0 if typed string is longer then ui32MaxLength
//          1 if Return was typed
char uartInString(char *cPtr, uint32_t ui32MaxLength)
{
    // initialize local variables
    uint32_t ui32Length = 0;
    char cLetter;

    cLetter = uartInChar();

    while (cLetter != CR)                   // as long as Return is not pressed
    {
        if(cLetter == DEL)                  // DEL was pressed -> what about Backspace?
        {
            if(ui32Length)                  // only if valid chars have already been typed
            {
                cPtr--;                     // move back in the string
                ui32Length--;
            }
        }
        else if(ui32Length < ui32MaxLength) // current length is within maximum allowed size
        {
            *cPtr = cLetter;
            cPtr++;
            ui32Length++;
        }
        else                                // ui32Length has reached max allowed size
        {
            return 0;
        }
        uartOutChar(cLetter);               // echo
        cLetter = uartInChar();             // grab next char
    }

    *cPtr=0;                                // Null-Termination for string
    return 1;
}

// uartInDec - Accepts ASCII input in unsigned decimal format.
// Valid range is 0 to 4294967295 (2^32-1). Everything above will return an incorrect value
// Input: none
// Output: 32bit unsigned integer
uint32_t uartInDec(void)
{
    // initialize local variables
    uint32_t ui32Length = 0;
    uint32_t ui32Number = 0;
    char cData;

    cData = uartInChar();                              // grab the first character

    while (cData != CR)                                  // as long as Return is not pressed
    {
        if(cData >= 0x30 && cData <= 0x39)        // valid data: 0x30 = '0' and 0x39 = '9'
        {
            ui32Number = ui32Number*10+(cData - 0x30);   // New = Old * 10 + (Input-Offset)
            ui32Length++;
            uartOutChar(cData);
        }
        else if(cData == DEL && ui32Length)              // Backspace and valid chars have allready been typed
        {
            ui32Number /= 10;                                   // New = Old/10;
            ui32Length--;
            uartOutChar(cData);
        }
        cData = uartInChar();                          // grab the next character
    }
    return ui32Number;
}

//###################################################################
//							ISRs
//###################################################################

// sciaTxISR - Info
// Input: none
// Output: none
__interrupt void sciaTxISR(void)
{
    //
    // acknowledge
    //
    SCI_clearInterruptStatus(SCIA_BASE, SCI_INT_TXFF);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}

// sciaRXFIFOISR - Info
// Input: none
// Output: none
__interrupt void sciaRxISR(void)
{
    //
    // acknowledge
    //
    SCI_clearInterruptStatus(SCIA_BASE, (SCI_INT_RXFF|SCI_INT_RXERR));
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}
