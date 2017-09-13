//###################################################################

// File: main.c
// Hardware: TMS320F28379D on LAUNCHXL-F28379D
// Project: F2837x_gpio_led
// Author: Simon
// Date: 07.08.2017
// Revison:
// Info: toggle blue LED on MKII Boosterpack by S1 interrupt
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
#define MKII_LED_BLUE 3U
#define MKII_S1 16U

//
// Prototypes
//
void systemInit(void);
__interrupt void gpio16ISR(void);

//
// Global Variables
//

//
// Main
//
void main(void)
{
    systemInit();

    while(1)
    {
        NOP;
    }

}

// systemInit - This function initializes the system by setting up the clock
// initializing the GPIOs
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

     //
     // Make GPIO3 an output and GPIO16 an input
     //
     GPIO_setPadConfig(MKII_LED_BLUE, GPIO_PIN_TYPE_STD);
     GPIO_setPinConfig(GPIO_3_GPIO3);
     GPIO_setDirectionMode(MKII_LED_BLUE, GPIO_DIR_MODE_OUT);

     GPIO_setPadConfig(MKII_S1, GPIO_PIN_TYPE_PULLUP);
     GPIO_setPinConfig(GPIO_16_GPIO16);
     GPIO_setDirectionMode(MKII_S1, GPIO_DIR_MODE_IN);       // use qualification for debouncing
     GPIO_setQualificationMode(MKII_S1, GPIO_QUAL_6SAMPLE);
     GPIO_setQualificationPeriod(MKII_S1, 510);

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
     // Interrupts that are used in this example are re-mapped to
     // ISR functions found within this file.
     //
     Interrupt_register(INT_XINT1, &gpio16ISR);
     Interrupt_enable(INT_XINT1);
     GPIO_setInterruptPin(MKII_S1,GPIO_INT_XINT1);
     GPIO_setInterruptType(GPIO_INT_XINT1, GPIO_INT_TYPE_RISING_EDGE);
     GPIO_enableInterrupt(GPIO_INT_XINT1);

     //
     // Enable Global Interrupt (INTM) and realtime interrupt (DBGM)
     //
     EINT;
     ERTM;
}

//
// gpio16ISR - ISR for MKII S1 pushbutton
//
__interrupt void
gpio16ISR(void)
{
    GPIO_togglePin(MKII_LED_BLUE);
    //
    // Acknowledge this interrupt to receive more interrupts from group 1
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}
//
// End of File
//
