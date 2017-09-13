//###################################################################

// File: TMP006.c
// Hardware: TMS320F28379D on LAUNCHXL-F28379D
// Project:
// Author: Simon
// Date: 26.08.2017
// Revison:
// Info:

//###################################################################

//
// Included Files
//
#include "TMP006.h"

//
// #Defines
//


//
// Local Prototypes
//

//
// Global Variables
//

// configure Configuration Register (0x02) for (data sheet figure 16):
// 15    RST        software reset bit      0b = normal operation
// 14-12 MOD        mode of operation     111b = sensor and die continuous conversion
// 11-9  CR         ADC conversion rate   000b = 4 sample/sec
// 8     EN         interrupt pin enable    1b = ~DRDY pin enabled (J2.11/P3.6)
// 7     ~DRDY      data ready bit          0b (read only, automatic clear)
// 6-0   reserved                      000000b (reserved)
uint16_t g_Tmp006TxArray[3] = {0x02, 0x71, 0x00};

//
// Function Definitions
//

// tmp006Init - Info
// Input:
// Output:
void tmp006Init(void)
{
    // nothing to be done. Can be used to initialize the DRDY Pin
    // but this can also be done in the gpioInit() function in main for more consistency
}

// tmp006StartConversion - Info
// Input:
// Output:
void tmp006StartConversion(void)
{
    while(I2C_getStopConditionStatus(I2CA_BASE)){};

    I2C_setSlaveAddress(I2CA_BASE, TMP006_ADDRESS);

    while(I2C_isBusBusy(I2CA_BASE)){};

    I2C_setDataCount(I2CA_BASE, 3);

    uint16_t i;
    for(i = 0; i < 3; i++)
    {
        I2C_putData(I2CA_BASE, g_Tmp006TxArray[i]);
    }

    //
    // Send start as master transmitter
    //
    I2C_setConfig(I2CA_BASE, I2C_MASTER_SEND_MODE);
    I2C_sendStartCondition(I2CA_BASE);
    I2C_sendStopCondition(I2CA_BASE);
}

// tmp006Temperature - Info
// Input:
// Output:
float tmp006Temperature(void)
{
    uint16_t ui16Raw[2] = {0,0};
    uint16_t ui16Temp = 0;

    //
    // Wait for STOP
    //
    while(I2C_getStopConditionStatus(I2CA_BASE)){};

    //
    // Set slave address
    //
    I2C_setSlaveAddress(I2CA_BASE, TMP006_ADDRESS);

    //
    // Wait for bus to be idle
    //
    while(I2C_isBusBusy(I2CA_BASE)){};

    //
    // Setup number of bytes to send
    //
    I2C_setDataCount(I2CA_BASE, 1);

    //
    // Setup data to send
    //
    I2C_putData(I2CA_BASE, 0x01);

    //
    // Send start as master transmitter
    //
    I2C_setConfig(I2CA_BASE, I2C_MASTER_SEND_MODE);
    I2C_sendStartCondition(I2CA_BASE);
    I2C_sendStopCondition(I2CA_BASE);

    //------------------------------------------------//

    //
    // Wait for STOP
    //
    while(I2C_getStopConditionStatus(I2CA_BASE)){};

    //
    // Set slave address
    //
    I2C_setSlaveAddress(I2CA_BASE, TMP006_ADDRESS);

    //
    // Wait for bus to be idle
    //
    while(I2C_isBusBusy(I2CA_BASE)){};

    //
    // Address setup phase has completed. Now setup how many bytes expected
    // and send restart as master-receiver.
    //
    I2C_setDataCount(I2CA_BASE, 2);
    I2C_setConfig(I2CA_BASE, I2C_MASTER_RECEIVE_MODE);
    I2C_sendStartCondition(I2CA_BASE);
    I2C_sendStopCondition(I2CA_BASE);

    //
    // Wait for STOP
    //
    while((I2C_getStatus(I2CA_BASE) & I2C_STS_STOP_CONDITION) == 0){}

    uint16_t i;
    for(i = 0; i < 2; i++)
    {
        ui16Raw[i] = I2C_getData(I2CA_BASE);
    }

    // Right shift because T0 is at position 2 (data sheet figure 15)
    ui16Temp = ((ui16Raw[0]<<8)+ui16Raw[1])>>2;

    // LSB = 1 / 32°C = 0.03125
    return (float)ui16Temp*0.03125;
}

void tmp006SoftwareReset(void)
{
    while(I2C_getStopConditionStatus(I2CA_BASE)){};

    I2C_setSlaveAddress(I2CA_BASE, TMP006_ADDRESS);

    while(I2C_isBusBusy(I2CA_BASE)){};

    I2C_setDataCount(I2CA_BASE, 3);

    I2C_putData(I2CA_BASE, 0x02);
    I2C_putData(I2CA_BASE, 0x80);
    I2C_putData(I2CA_BASE, 0x00);

    //
    // Send start as master transmitter
    //
    I2C_setConfig(I2CA_BASE, I2C_MASTER_SEND_MODE);
    I2C_sendStartCondition(I2CA_BASE);
    I2C_sendStopCondition(I2CA_BASE);
}

