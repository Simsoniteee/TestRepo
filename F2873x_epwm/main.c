//###################################################################

// File: main.c
// Hardware: TMS320F28379D on LAUNCHXL-F28379D
// Project: F2837x_epwm
// Author: Simon
// Date: 08.08.2017
// Revison:
// Info: output pwm on ePWM1A/GPIO0 and ePWM1B/GPIO1
// For SYSCLK above 100 MHz, the EPWMCLK must be half of SYSCLK (see table 5-12 in datasheet)
//
//
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
#define TBPRD 2500>>1 // period

//
// Prototypes
//
void systemInit(void);
void initEPWM1(void);
void epwm1DutyCycle(uint16_t dutycycle);

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
     // Initializes device clock and peripherals
     //
     Device_init();

     //
     // Configures the GPIO pin as a push-pull output
     //
     Device_initGPIO();
     //GPIO_setPadConfig(0, GPIO_PIN_TYPE_PULLUP);     // Enable pullup on GPIO0
     GPIO_setPadConfig(0, GPIO_PIN_TYPE_STD);
     //GPIO_setPadConfig(1, GPIO_PIN_TYPE_PULLUP);     // Enable pullup on GPIO1
     GPIO_setPadConfig(1, GPIO_PIN_TYPE_STD);
     GPIO_setPinConfig(GPIO_0_EPWM1A);               // GPIO0 = PWM1A
     GPIO_setPinConfig(GPIO_1_EPWM1B);               // GPIO1 = PWM1B

     //
     // Initializes PIE and clears PIE registers. Disables CPU interrupts.
     //
     Interrupt_initModule();

     //
     // Initializes the PIE vector table with pointers to the shell Interrupt
     // Service Routines (ISR).
     //
     Interrupt_initVectorTable();

     //
     // Disable sync(Freeze clock to PWM as well)
     //
     SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);

     //
     // Initialize the ePWM
     //
     initEPWM1();

     //
     // Enable sync and clock to PWM
     //
     SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);

     //
     // Enable Global Interrupt (INTM) and realtime interrupt (DBGM)
     //
     EINT;
     ERTM;
}

//
// initEPWM1 - initialize ePWM1A and ePWM1B on GPIO0 and GPIO1
//
void initEPWM1(void)
{
    //
    // Setup Time Base Submodule following function order in driver library user guide
    //
    EPWM_setTimeBaseCounter(EPWM1_BASE, 0U); // reset the pwm counter
    EPWM_setClockPrescaler(EPWM1_BASE, EPWM_CLOCK_DIVIDER_2,
    EPWM_HSCLOCK_DIVIDER_10); // prescale the pwm clock TBCLK = EPWMCLK/(highSpeedPrescaler * pre-scaler) -> results in a 2kHz pwm signal
    EPWM_disablePhaseShiftLoad(EPWM1_BASE); // Disable phase shift load
    EPWM_setTimeBaseCounterMode(EPWM1_BASE, EPWM_COUNTER_MODE_UP_DOWN); // configure in up/dowm mode
    EPWM_setPhaseShift(EPWM1_BASE, 0U); // no phase shift
    EPWM_setTimeBasePeriod(EPWM1_BASE, TBPRD); // period (TBPRD) is 2500 -> TPWM = 2*TBPRD*T_PWM (Figure 14-6 technical rev. manual)

    //
    // Setup Counter Compare Submodule following function order in driver library user guide
    //
    EPWM_setCounterCompareShadowLoadMode(EPWM1_BASE,EPWM_COUNTER_COMPARE_A, EPWM_COMP_LOAD_ON_CNTR_ZERO); // load new cmp value when ctr is zero
    EPWM_setCounterCompareShadowLoadMode(EPWM1_BASE,EPWM_COUNTER_COMPARE_B, EPWM_COMP_LOAD_ON_CNTR_ZERO); // load new cmp value when ctr is zero
    EPWM_setCounterCompareValue(EPWM1_BASE, EPWM_COUNTER_COMPARE_A, TBPRD>>1);  // set counter compare values. here the dc is 50%
    EPWM_setCounterCompareValue(EPWM1_BASE, EPWM_COUNTER_COMPARE_B, TBPRD>>1);  // set counter compare values. here the dc is 50%

    //
    // Setup Action Qualifier Submodule following function order in driver library user guide
    //
    EPWM_setActionQualifierAction(EPWM1_BASE,
                                  EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_HIGH,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA); // counting up: output high on cmpa
    EPWM_setActionQualifierAction(EPWM1_BASE,
                                  EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_LOW,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPA); // counting down: output low on cmpa
    EPWM_setActionQualifierAction(EPWM1_BASE,
                                  EPWM_AQ_OUTPUT_B,
                                  EPWM_AQ_OUTPUT_HIGH,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB); // counting up: output high on cmpb
    EPWM_setActionQualifierAction(EPWM1_BASE,
                                  EPWM_AQ_OUTPUT_B,
                                  EPWM_AQ_OUTPUT_LOW,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPB); // counting down: output low on cmpb
}

//
// epwm1DutyCycle - update the dutycycle
//
void epwm1DutyCycle(uint16_t dutycycle)
{

}

//
// End of File
//
