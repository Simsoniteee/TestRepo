//###################################################################

// File: main.c
// Hardware: TMS320F28379D on LAUNCHXL-F28379D +  TLC549 8-bit ADC with SPI
// Project: F2837x_spi_tlc549
// Author: Simon
// Date: 11.08.2017
// Revison:
// Info:  Use SPI module to interface the TLC549 8-bit ADC. This example uses polling
// Conventions: 1) self written functions start with a lower case letter e.g. systemInit()
//
// TLC549 Connection:
//          ----------------
//          |              |
//    3V3---|REF+       VCC|---3V3
//    Sig---|IN         CLK|---SPIB CLK/Pin 47
//    GND---|REF-       OUT|---SPIB MISO/Pin 54
//    GND---|GND        CS!|---SPIB CS/Pin 59
//          |              |
//          ----------------
//
//###################################################################

//
// Included Files
//
#include "driverlib.h"
#include "device.h"

//
// #Defines
//
// #define SPIMOSI     63   //not used for TLC549
#define TLC549DATA  64
#define TLC549CLK   65
#define TLC549CS    66
#define TLC549DATA_MASK 0x00FF  // 8-bit result only

//
// Prototypes
//
void systemInit(void);
void tlc549Init(void);
void tlc594_RxData(uint16_t *ui16DataPtr);
//
// Global Variables
//
uint16_t ui16TLC594Data;
float fVoltage;

//
// Main
//
void main(void)
{
    systemInit();

    while(1)
    {
        DEVICE_DELAY_US(100);
        tlc594_RxData(&ui16TLC594Data);
        fVoltage = ((float)ui16TLC594Data)*0.01289;
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
     // Enable SPI-B on GPI63 - GPIO66
     //
     //GPIO_setPadConfig(SPIMOSI, GPIO_PIN_TYPE_PULLUP); // Pullup on GPIO19 (SPISTEA)
     GPIO_setPadConfig(TLC549DATA, GPIO_PIN_TYPE_PULLUP); // Pullup on GPIO18 (SPICLKA)
     GPIO_setPadConfig(TLC549CLK, GPIO_PIN_TYPE_PULLUP); // Pullup on GPIO16 (SPISIMOA)
     GPIO_setPadConfig(TLC549CS, GPIO_PIN_TYPE_PULLUP); // Pullup on GPIO17 (SPIS0MIA)
     //GPIO_setQualificationMode(SPISIMO, GPIO_QUAL_ASYNC); // asynch input
     GPIO_setQualificationMode(TLC549DATA, GPIO_QUAL_ASYNC); // asynch input
     GPIO_setQualificationMode(TLC549CLK, GPIO_QUAL_ASYNC); // asynch input
     GPIO_setQualificationMode(TLC549CS, GPIO_QUAL_ASYNC); // asynch input
     //GPIO_setPinConfig(GPIO_63_SPISIMOB);            // GPIO16 = SPISIMOA
     GPIO_setPinConfig(GPIO_64_SPISOMIB);            // GPIO17 = SPIS0MIA
     GPIO_setPinConfig(GPIO_65_SPICLKB);             // GPIO18 = SPICLKA
     GPIO_setPinConfig(GPIO_66_SPISTEB);             // GPIO19 = SPISTEA

     //
     // Initialize PIE and clear PIE registers. Disables CPU interrupts.
     //
     Interrupt_initModule();

     //
     // Initialize the PIE vector table with pointers to the shell Interrupt
     // Service Routines (ISR).
     //
     Interrupt_initVectorTable();

     // Initialize device communication with tlc549
     tlc549Init();

     //
     // Enable Global Interrupt (INTM) and realtime interrupt (DBGM)
     //
     EINT;
     ERTM;
}

// tlc549Init - Initialize the tlc549 with SPI (no FIFO, ...)
// Input: -
// Output: -
void tlc549Init(void)
{
    //
    // Must put SPI into reset before configuring it
    //
    SPI_disableModule(SPIB_BASE);

    //
    // SPI configuration. Use a 1MHz SPICLK and 8-bit word size. Freescale SPI with: CLKPOLARITY=0; CLK_PHASE=0
    //
    SPI_setConfig(SPIB_BASE, DEVICE_LSPCLK_FREQ, SPI_PROT_POL0PHA0,
                  SPI_MODE_MASTER, 1000000, 8);

    //
    // Configuration complete. Enable the module.
    //
    SPI_enableModule(SPIB_BASE);
}

// Name tlc594_RxData - Trigger ADC conversion via SPI and read result (polling)
// Input: Pointer to variable to store 8bit ADC result from tlc549
// Output: -
void tlc594_RxData(uint16_t *ui16DataPtr)
{
    SPI_writeDataNonBlocking(SPIB_BASE, 0xFF);                          // start communication
    *ui16DataPtr = SPI_readDataNonBlocking(SPIB_BASE)&TLC549DATA_MASK;  // store data
}

//
// End of File
//
