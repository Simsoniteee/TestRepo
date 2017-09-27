//*****************************************************************************
/*

File: main.c
Hardware: TM4C123GH6PM on TM4C123GXL
Project: UART_Protocol
Author: Simon
Date: 08.06.2016
Revison: v01
Info: Main file to test UART Protocol

*/
//*****************************************************************************

#include "SYSTEM.h"
#include "PLL.h"
#include "UART0.h"
#include "PROTOCOL.h"

int main(void)
{
	PLL_Init();
	UART0_Init();

	UART0_OutString("UART Protocol Test Program");
	UART0_OutCRLF();


	uint8_t InBuffer[12] = { 0 };
	MCSP_PACKET_TYPE InPacket = { 0, 0, InBuffer };

	uint8_t OutBuffer[12] = { 0 };
	MCSP_PACKET_TYPE OutPacket = { 0, 0, OutBuffer };

	// variable to grab uart data
	uint8_t ui8TempData = 0;

	while(1)
	{
		// as long as there is data in the SW Rx fifo
		while(UART0_Rxfifo_get(&ui8TempData))
		{
			// Decode data and store in InPacket
			uint8_t ui8MCSP_Return = MCSP_Decode(&InPacket, ui8TempData);
			// echo if packet is received correctly
			if(ui8MCSP_Return == CRC_OK)
			{
				ReceiveMsg_MotorCtr(&InPacket);
				SendMsg_MotorCtr(&OutPacket);
				MCSP_Encode(&OutPacket);

				// time to send some measurement values:
				MeasValues.MotorCurrent = 2.2f;
				MeasValues.MotorVoltage = 32.0f;
				MeasValues.MotorRpm = 11230.0f;

				SendMsg_MeasVal(&OutPacket);
				MCSP_Encode(&OutPacket);
			}
		}
	}
}

