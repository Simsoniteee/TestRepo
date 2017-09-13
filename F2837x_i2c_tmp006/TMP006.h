//###################################################################

// File: TMP006.h
// Hardware: TMS320F28379D on LAUNCHXL-F28379D
// Project:
// Author: Simon
// Date: 26.08.2017
// Revison:
// Info:

//###################################################################


#ifndef TMP006_H_
#define TMP006_H_

//
// Included Files
//
#include "driverlib.h"

//
// #Defines
//
#define TMP006_ADDRESS 0x40     // address on MKII-Boosterpack
#define TMP006INT 29            // DRDY Pin is 29

//
// Prototypes
//

void tmp006Init(void);
void tmp006StartConversion(void);
float tmp006Temperature(void);
void tmp006SoftwareReset(void);

//
// Global Variables
//

#endif /* TMP006_H_ */
