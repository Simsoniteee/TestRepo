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

//
// #Defines
//


//
// Prototypes
//
void systemInit(void);
void uartInit(void);
void gpiosInit(void);

__interrupt void sciaRXFIFOISR(void);
__interrupt void sciaTXFIFOISR(void);

//
// Global Variables
//
uint16_t rDataA[2];
uint16_t rFlag;

//
// Main
//
void main(void)
{
    systemInit();

    while(1)
    {

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
     // ISR functions for sci RX and TX interrupt
     //
     Interrupt_register(INT_SCIA_RX, sciaRXFIFOISR);
     Interrupt_register(INT_SCIA_TX, sciaTXFIFOISR);

     //
     // Initialize the UART and the other peripherals
     //
     uartInit();

     //
     // Enable sci interrupts and clear ack groups
     //
     Interrupt_enable(INT_SCIA_RX);
     Interrupt_disable(INT_SCIA_TX);

     Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);

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


// uartInit - Initialize the UART: baudrate=9600, wordlength=8, stopbit=1, paritiy=none
// Input: -
// Output: -
void uartInit(void)
{
    SCI_performSoftwareReset(SCIA_BASE);
    //
    // 8 char bits, 1 stop bit, no parity. Baud rate is 9600. Enable FIFOs
    //
    SCI_setConfig(SCIA_BASE, DEVICE_LSPCLK_FREQ, 9600, (SCI_CONFIG_WLEN_8 | SCI_CONFIG_STOP_ONE | SCI_CONFIG_PAR_NONE));
    SCI_enableFIFO(SCIA_BASE);

    //
    // Enable the Interrupts (RX, RXERR) and set the FIFO Level Interrupts
    //
    SCI_enableInterrupt(SCIA_BASE, (SCI_INT_RXFF | SCI_INT_TXFF | SCI_INT_RXERR));
    SCI_setFIFOInterruptLevel(SCIA_BASE, SCI_FIFO_TX4, SCI_FIFO_RX2);

    //
    // Resets
    //
    SCI_performSoftwareReset(SCIA_BASE);
    SCI_resetChannels(SCIA_BASE);
    SCI_resetRxFIFO(SCIA_BASE);
    SCI_resetTxFIFO(SCIA_BASE);

    //
    // Initialize the data buffer
    //
    uint16_t i;
    for(i = 0; i < 2; i++)
    {
        rDataA[i] = 0;
    }

    //
    // Finally, enable the module
    //
    SCI_enableModule(SCIA_BASE);
}

// Name - Info
// Input:
// Output:
__interrupt void sciaTXFIFOISR(void)
{
    //SCI_writeCharArray(SCIA_BASE, rDataA, 2);

    Interrupt_disable(INT_SCIA_TX);

    SCI_clearInterruptStatus(SCIA_BASE, SCI_INT_TXFF);

    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}

// sciaRXFIFOISR - Info
// Input: -
// Output: -
__interrupt void sciaRXFIFOISR(void)
{

    SCI_readCharArray(SCIA_BASE, rDataA, 2);

    SCI_clearOverflowStatus(SCIA_BASE);

    SCI_clearInterruptStatus(SCIA_BASE, (SCI_INT_RXFF|SCI_INT_RXERR));

    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}

//
// End of File
//
