//###################################################################

// File: main.c
// Hardware: TMS320F28379D on LAUNCHXL-F28379D
// Project: F2837x_uart_fifo
// Author: Simon
// Date: 12.08.2017
// Revison:
// Info: Echo example with UART. Use terminal software like Putty for input and output.
// Conventions: 1) self written functions start with a lower case letter e.g. systemInit()

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
// Prototypes
//
void systemInit(void);
void gpiosInit(void);

//
// Global Variables
//

//
// Main
//
void main(void)
{
    systemInit();

    uartOutString("Testing of Uart I/O Functions");
    uartOutCRLF();
    uartOutString("**************************");
    uartOutCRLF();
    uartOutCRLF();

    while(1)
    {
/*        // Testing uartInString() and uartOutString()
        char cTestArray[30];
        uartOutString("Type a String and press Enter. String is limited to 30 characters: ");
        uartInString(cTestArray, sizeof(cTestArray));
        uartOutCRLF();
        uartOutString("You typed: ");
        uartOutString(cTestArray);
        uartOutCRLF();

        // Testing uartInDec() and uartOutDec()
        uint32_t ui32TestDec;
        uartOutString("Type an unsigned decimal number from 0 to 4294967295: ");
        ui32TestDec = uartInDec();
        uartOutCRLF();
        uartOutString("You typed: ");
        uartOutDec(ui32TestDec);
        uartOutCRLF();*/

        int32_t i;
        for(i=0; i<100; i++)
        {
            uartOutDec(i);
            uartOutCRLF();
            DEVICE_DELAY_US(100000);
        }/*
        char cData=0;
        while((cData=uartInChar())==0){}
        uartOutChar(cData);*/
    }

}

// systemInit - This function initializes the system by setting up the clock,
// initializing the GPIOs and initializing PIE and the PIE vector table
// User initializations are also done here
// Input: none
// Output: none
void systemInit(void)
{
     //
     // Initialize device clock and peripherals
     //
     Device_init();

     //
     // Initialize GPIO and configure the GPIO pin as a push-pull output
     //
     Device_initGPIO();
     gpiosInit();

     //
     // Initialize PIE and clear PIE registers. Disables CPU interrupts.
     //
     Interrupt_initModule();

     //
     // Initialize the PIE vector table with pointers to the shell Interrupt
     // Service Routines (ISR).
     //
     Interrupt_initVectorTable();

     //
     // ISR functions for Uart Rx and Tx interrupt. The interrupts are defined in UART.c
     //
     //Interrupt_register(INT_SCIA_RX, sciaRxISR);
     //Interrupt_register(INT_SCIA_TX, sciaTxISR);

     //
     // Initialize the UART and the other peripherals
     //
     uartInit();

     //
     // Enable sci interrupts and clear ack group
     //
     //Interrupt_enable(INT_SCIA_RX);
     //Interrupt_enable(INT_SCIA_TX);
     //Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);

     //
     // Enable Global Interrupt (INTM) and realtime interrupt (DBGM)
     //
     EINT;
     ERTM;
}

// gpiosInit - Initialize the GPIOs used in this example
// Input: -
// Output: -
void gpiosInit(void)
{
    //
    // GPIO43 is the SCI Rx pin on LAUNCHXL-F28379D.
    //
    GPIO_setMasterCore(43, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_43_SCIRXDA);
    GPIO_setDirectionMode(43, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(43, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(43, GPIO_QUAL_ASYNC);

    //
    // GPIO43 is the SCI Tx pin on LAUNCHXL-F28379D.
    //
    GPIO_setMasterCore(42, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_42_SCITXDA);
    GPIO_setDirectionMode(42, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(42, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(42, GPIO_QUAL_ASYNC);
}

//
// End of File
//
