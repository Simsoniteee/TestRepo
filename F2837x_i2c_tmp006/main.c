//###################################################################

// File: main.c
// Hardware: TMS320F28379D on LAUNCHXL-F28379D with MKII Boosterpack
// Project: F2837x_i2c_tmp006
// Author: Simon
// Date: 12.08.2017
// Revison:
// Info: Using the tmp006 temperature sensor on MKII Boosterpack to test i2c module
// Conventions: 1) self written functions start with a lower case letter e.g. systemInit()
//
// Anmerkung: Mosfettemperatur evtl. mit tmp006 überwachen!
//
//###################################################################

//
// Included Files
//
#include "driverlib.h"
#include "device.h"
#include "TMP006.h"

//
// #Defines
//


//
// Prototypes
//
void systemInit(void);
void gpiosInit(void);
void i2caInit(void);
__interrupt void tmp006ISR(void);

//
// Global Variables
//
float fTemperature;
uint16_t ui16IntCounter, ui16Ready2Read;
//
// Main
//
void main(void)
{
    ui16IntCounter = 0;
    ui16Ready2Read = 0;

    systemInit();

    tmp006StartConversion();

    while(1)
    {
        if(ui16Ready2Read)
        {
            ui16Ready2Read = 0;
            tmp006StartConversion();
        }
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
     // Interrupts that are used in this example are re-mapped to ISR functions
     // found within this file.
     //
     Interrupt_register(INT_XINT1, &tmp006ISR);

     //
     //Initialize Peripherals
     //
     gpiosInit();
     i2caInit();

     //
     // Enable PIE interrupts
     //
     Interrupt_enable(INT_XINT1);

     //
     // Enable Global Interrupt (INTM) and real time interrupt (DBGM)
     //
     EINT;
     ERTM;
}

// gpiosInit - Initialize the GPIOs
// Input:
// Output:
void gpiosInit(void)
{
    //
    // Configure pin 29 as DRDY Pin of tmp006. Use Qualification for debouncing. Map to XINT1
    //
    GPIO_setPadConfig(TMP006INT, GPIO_PIN_TYPE_PULLUP);
    GPIO_setPinConfig(GPIO_29_GPIO29);
    GPIO_setDirectionMode(TMP006INT, GPIO_DIR_MODE_IN);
    GPIO_setQualificationMode(TMP006INT, GPIO_QUAL_ASYNC);
    GPIO_setInterruptPin(TMP006INT,GPIO_INT_XINT1);
    GPIO_setInterruptType(GPIO_INT_XINT1, GPIO_INT_TYPE_FALLING_EDGE);
    GPIO_enableInterrupt(GPIO_INT_XINT1);

    //
    // Configure the I2C pins (P105/SCL and P104/SDA) as shown in the gpio_ex1_setup example
    //
    GPIO_setPadConfig(104, GPIO_PIN_TYPE_PULLUP);
    GPIO_setPinConfig(GPIO_104_SDAA);
    GPIO_setQualificationMode(104, GPIO_QUAL_ASYNC);
    GPIO_setPadConfig(105, GPIO_PIN_TYPE_PULLUP);
    GPIO_setPinConfig(GPIO_105_SCLA);
    GPIO_setQualificationMode(105, GPIO_QUAL_ASYNC);
}

// i2caInit - Initialize the I2CA module
// Input:
// Output:
void i2caInit(void)
{
    //
    // Must put I2C into reset before configuring it
    //
    I2C_disableModule(I2CA_BASE);

    //
    // I2C configuration. Use a 100kHz I2CCLK with a 50% duty cycle.
    //
    I2C_initMaster(I2CA_BASE, DEVICE_SYSCLK_FREQ, 100000, I2C_DUTYCYCLE_50);
    I2C_setBitCount(I2CA_BASE, I2C_BITCOUNT_8);
    I2C_setSlaveAddress(I2CA_BASE, TMP006_ADDRESS);
    I2C_setEmulationMode(I2CA_BASE, I2C_EMULATION_FREE_RUN);

    //
    // FIFO and interrupt configuration
    //
    I2C_enableFIFO(I2CA_BASE);
    I2C_clearInterruptStatus(I2CA_BASE, I2C_INT_RXFF | I2C_INT_TXFF);

    //
    // Configuration complete. Enable the module.
    //
    I2C_enableModule(I2CA_BASE);
}

// tmp006ISR - ISR for tmp006 interrupt triggered by DRDY pin
// Input:
// Output:
__interrupt void tmp006ISR(void)
{
    //ui16IntCounter++;
    ui16Ready2Read = 1;

    //
    // Get data from Sensor
    //
    fTemperature = tmp006Temperature();
    //tmp006SoftwareReset();

    //
    // Acknowledge this interrupt to receive more interrupts from group 1
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}

//
// End of File
//
