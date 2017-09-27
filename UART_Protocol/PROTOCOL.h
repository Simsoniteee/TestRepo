//*****************************************************************************
/*

File: PROTOCOL.h
Hardware: Generic
Project: UART_Protocol
Author: Simon
Date: 08.06.2016
Revison: v01
Info: Motor Control Serial Protocol (MCSP)

 	 Framesetup: |SOF|DLC|ID|DATA|CRC|
 	 	 	 	 SOF 	=	8bit "Start of frame" identifier 	->	indicates start of a frame
 	 	 	 	 DLC	= 	8bit DLC							-> 	number of raw data bytes
 	 	 	 	 ID		= 	8bit "Identifier" 					-> 	unique identifier for a global variable
 	 	 	 	 DATA	= 	n bit data 							-> 	actual data
 	 	 	 	 CRC	= 	8bit Checksum 						-> 	CRC-8

*/
//*****************************************************************************

/*****************************************************
 *
 * 					Includes
 *
******************************************************/

#ifndef _PROTOCOL_H
#define _PROTOCOL_H

#include <stdint.h>

//****************************************************
//
//	MCSP specific defines
//
//****************************************************

// SOF ID
#define MCSP_SOF_ID			0xFF
// header consists of DLC and ID
#define MCSP_HEADERSIZE 	2

//****************************************************
//
//	Macros for restoring data from received messages
//
//****************************************************
// 1. Motor Control
#define MOTOR_CTR_RUNSTATE_MASK 0x01
#define MOTOR_CTR_DIRECTION_MASK 0x03
#define MOTOR_CTR_DUTYCYCLE_MASK 0x1FF

// 2. Measurement Values
#define MEAS_VAL_MTRCURRENT_MASK	0x3FF

/*****************************************************************************
 *
 * 						Variables and Definitions
 *
*****************************************************************************/

//****************************************************
//
//	Unique packet IDs for an MCSP message
//
//****************************************************
typedef enum {
	MtrCtr_ID = 0x01,
	MeasValues_ID
}MCSP_ID_TYPE;

//****************************************************
//
//	Data Length code (DLC) for MCSP messages
//
//****************************************************
typedef enum {
	// 1. Motor Control
	MtrCtr_DLC = 2,
	// 2. Measurement Values
	MeasValues_DLC = 6
}MCSP_DLC_TYPE;

//****************************************************
//
//	Start positions for data in a specific MCSP message
//
//****************************************************
typedef enum {
	// 1. Motor Control
	Start_MtrCtr_State = 0, Start_MtrCtr_Dir = 1, Start_MtrCtr_Duty = 3,
	// 2. Measurement Values
	Start_MeasVal_Current = 0, Start_MeasVal_Voltage = 10, Start_MeasVal_Rpm = 20
}MCSP_STARTPOSITION_TYPE;

//****************************************************
//
//
//
//****************************************************
typedef enum {
	// 1. Motor Control
	Length_MtrCtr_State = 1, Length_MtrCtr_Dir = 2, Length_MtrCtr_Duty = 9,
	// 2. Measurement Values
	Length_MeasVal_Current = 10, Length_MeasVal_Voltage = 10, Length_MeasVal_Rpm = 16
}MCSP_DATALENGTH_TYPE;

//****************************************************
//
//	Coefficients for calculation: y = Coefficient * x + Offset
//
//****************************************************

typedef enum {
	// 1. Motor Control
	Fac_DutyCycle = 5,
	// 2. Measurement Values
	Fac_Current = 100, Fac_Voltage = 10, Fac_Rpm = 2
}MCSP_SENDCOEFFICIENT_TYPE;

//****************************************************
//
//	Bit-Offset for calculation: y = Coefficient * x + Offset
//
//****************************************************
typedef enum {
	// 1. Motor Control

	// 2. Measurement Values
	Offset_Mtr_Current = 511, Offset_Mtr_Voltage = 511, Offset_Mtr_Rpm = 32767
}MCSP_OFFSET_TYPE;


//****************************************************
//
//	States of the decode state machine
//
//****************************************************
typedef enum
{
	STATE_GET_SOF,
	STATE_GET_HEADER,
	STATE_GET_PAYLOAD,
	STATE_CHECK_CRC,
}MCSP_STATE_TYPE;

//****************************************************
//
//	Return values of the decode state machine
//
//****************************************************
typedef enum
{
	NO_SOF,				// no SOF was received
	GOT_SOF,			// received SOF
	GOT_HEADER_BYTE,	// received a header byte
	GOT_HEADER,			// received complete header
	GOT_PAYLOAD_BYTE,	// received a data byte
	GOT_PAYLOAD,		// received complete data
	CRC_OK,				// CRC check passed
	CRC_FAIL			// CRC check failed
}MCSP_RETURNVALUE_TYPE;

//****************************************************
//
//	MCSP packet structure. This is how a packet is set up
//
//****************************************************
typedef struct
{
	uint8_t ui8DLC;
	uint8_t ui8ID;
	uint8_t *ui8BUFFER;
}MCSP_PACKET_TYPE;

//****************************************************
//
//	External declaration
//
//****************************************************

extern MCSP_PACKET_TYPE InPacket;
extern MCSP_PACKET_TYPE OutPacket;

/*****************************************************************************
 *
 * 						MCSP function declarations
 *
*****************************************************************************/

//********************* MCSP_Decode() *********************
// Function contains FSM for deconding incomming data concering the MCSP protocol specifications
// Input: Address of MCSP packet, byte from UART
// Output: returnvalue depending on current state, secified in PROTOCOL.h
// Info:
uint8_t MCSP_Decode(MCSP_PACKET_TYPE *Packet, uint8_t ui8Byte);

//********************* MCSP_Encode() *********************
// Function to encode data to a MCSP packet
// Input: address of packet
// Output:
// Info: über eine Abfrage könnte über einen weiteren Parameter der UART ausgesucht werden
void MCSP_Encode(MCSP_PACKET_TYPE *Packet);

//********************* GrabFromBuffer() *********************
// Restores bits from buffer dependent on startposition and length inside frame
// Input: Startposition of data inside frame, datalenght in bits, buffer to grab data from
// Output: returns a 32-bit variable containing dthe desired data
// Info: only 32-bit wide variables can be restored
uint32_t GrabFromBuffer(MCSP_STARTPOSITION_TYPE RightShift, MCSP_DATALENGTH_TYPE Bitlength, uint8_t *Buffer);

//********************* ShiftInBuffer() *********************
// Recursive function to store SendMsg data in send buffer
// Input: value to store, start position in buffer, pointer to buffer
// Output: -
// Info:
void ShiftInBuffer(uint32_t temp, MCSP_STARTPOSITION_TYPE LeftShift, uint8_t *Buffer);

/*****************************************************************************
 *
 * 						Function declarations for receiving
 *
*****************************************************************************/

//********************* ReceiveMsg_MotorCtr() *********************
//
// Input:
// Output:
// Info:
void ReceiveMsg_MotorCtr(MCSP_PACKET_TYPE *Packet);

/*****************************************************************************
 *
 * 						Function declarations for sending
 *
*****************************************************************************/

//********************* SendMsg_MotorCtr() *********************
//
// Input:
// Output:
// Info:
void SendMsg_MotorCtr(MCSP_PACKET_TYPE *Packet);

//********************* SendMsg_MeasVal() *********************
//
// Input:
// Output:
// Info:
void SendMsg_MeasVal(MCSP_PACKET_TYPE *Packet);

#endif
