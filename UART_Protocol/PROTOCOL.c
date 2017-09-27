//*****************************************************************************
/*

File: PROTOCOL.c
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

/*****************************************************************************
 *
 * 							Preprocessor
 *
*****************************************************************************/

#include "SYSTEM.h"
#include "PROTOCOL.h"
#include "UART0.h"

//****************************************************
//
//	Macro for clearing the Tx/Rx Buffers
//
//****************************************************
#define CLEAR_BUFFER() uint8_t i; for (i = 0; i < sizeof(Packet->ui8BUFFER); i++) { Packet->ui8BUFFER[i] = 0x00; }

//****************************************************
//
//	Macro for calculating CRC-8 via LUT
//
//****************************************************
#define MCSP_CRC(ui8CRC, ui8Data) (ui8CRC8Table[ui8CRC ^ ui8Data])

//****************************************************
//
//	LUT for CRC-8 with polynom x^8+x^2+x^1+1
//
//****************************************************
uint8_t ui8CRC8Table[256] =
{
	0x00, 0x07, 0x0e, 0x09, 0x1c, 0x1b, 0x12, 0x15,
	0x38, 0x3f, 0x36, 0x31, 0x24, 0x23, 0x2a, 0x2d,
	0x70, 0x77, 0x7e, 0x79, 0x6c, 0x6b, 0x62, 0x65,
	0x48, 0x4f, 0x46, 0x41, 0x54, 0x53, 0x5a, 0x5d,
	0xe0, 0xe7, 0xee, 0xe9, 0xfc, 0xfb, 0xf2, 0xf5,
	0xd8, 0xdf, 0xd6, 0xd1, 0xc4, 0xc3, 0xca, 0xcd,
	0x90, 0x97, 0x9e, 0x99, 0x8c, 0x8b, 0x82, 0x85,
	0xa8, 0xaf, 0xa6, 0xa1, 0xb4, 0xb3, 0xba, 0xbd,
	0xc7, 0xc0, 0xc9, 0xce, 0xdb, 0xdc, 0xd5, 0xd2,
	0xff, 0xf8, 0xf1, 0xf6, 0xe3, 0xe4, 0xed, 0xea,
	0xb7, 0xb0, 0xb9, 0xbe, 0xab, 0xac, 0xa5, 0xa2,
	0x8f, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9d, 0x9a,
	0x27, 0x20, 0x29, 0x2e, 0x3b, 0x3c, 0x35, 0x32,
	0x1f, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0d, 0x0a,
	0x57, 0x50, 0x59, 0x5e, 0x4b, 0x4c, 0x45, 0x42,
	0x6f, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7d, 0x7a,
	0x89, 0x8e, 0x87, 0x80, 0x95, 0x92, 0x9b, 0x9c,
	0xb1, 0xb6, 0xbf, 0xb8, 0xad, 0xaa, 0xa3, 0xa4,
	0xf9, 0xfe, 0xf7, 0xf0, 0xe5, 0xe2, 0xeb, 0xec,
	0xc1, 0xc6, 0xcf, 0xc8, 0xdd, 0xda, 0xd3, 0xd4,
	0x69, 0x6e, 0x67, 0x60, 0x75, 0x72, 0x7b, 0x7c,
	0x51, 0x56, 0x5f, 0x58, 0x4d, 0x4a, 0x43, 0x44,
	0x19, 0x1e, 0x17, 0x10, 0x05, 0x02, 0x0b, 0x0c,
	0x21, 0x26, 0x2f, 0x28, 0x3d, 0x3a, 0x33, 0x34,
	0x4e, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5c, 0x5b,
	0x76, 0x71, 0x78, 0x7f, 0x6A, 0x6d, 0x64, 0x63,
	0x3e, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2c, 0x2b,
	0x06, 0x01, 0x08, 0x0f, 0x1a, 0x1d, 0x14, 0x13,
	0xae, 0xa9, 0xa0, 0xa7, 0xb2, 0xb5, 0xbc, 0xbb,
	0x96, 0x91, 0x98, 0x9f, 0x8a, 0x8D, 0x84, 0x83,
	0xde, 0xd9, 0xd0, 0xd7, 0xc2, 0xc5, 0xcc, 0xcb,
	0xe6, 0xe1, 0xe8, 0xef, 0xfa, 0xfd, 0xf4, 0xf3
};

/*****************************************************************************
 *
 * 						Initial packet definitions
 *
*****************************************************************************/

//****************************************************
//
//	Motor is initially off, dir=coast, dutycycle=0
//
//****************************************************
MotorCtrType MotorCtr = { OFF, COAST, 0 };

//****************************************************
//
//	Measurement values are initially zero
//
//****************************************************
MeasValType MeasValues = { 0 };

/*****************************************************************************
 *
 * 						MCSP function definitions
 *
*****************************************************************************/

//********************* MCSP_Decode() *********************
// Function contains FSM for deconding incomming data concering the MCSP protocol specifications
// Input: Address of MCSP packet, byte from UART
// Output: returnvalue depending on current state, secified in PROTOCOL.h
// Info:
uint8_t MCSP_Decode(MCSP_PACKET_TYPE *Packet, uint8_t ui8Byte)
{
	MCSP_RETURNVALUE_TYPE ui8ReturnVal;
	static uint8_t ui8Count = 0;
	static uint8_t ui8CRC = 0;
	// initial state is GET_SOF
	static MCSP_STATE_TYPE ui8State = STATE_GET_SOF;
	uint8_t *ui8Raw = (uint8_t*)Packet;

	// Entering state machine
	switch(ui8State)
	{
		case STATE_GET_SOF:
			// ckeck if received byte if SOF
			if(ui8Byte == MCSP_SOF_ID)
			{
				// a new frame has been received
				ui8ReturnVal = GOT_SOF;
				// go to next state
				ui8State = STATE_GET_HEADER;
				// reset counter
				ui8Count = 0;
			}
			// no new frame received
			else{ui8ReturnVal = NO_SOF;}
		break;
		case STATE_GET_HEADER:
			// store data
			ui8Raw[ui8Count++] = ui8Byte;
			// calculate CRC
			ui8CRC = MCSP_CRC(ui8CRC, ui8Byte);
			// check for complete header
			if(ui8Count == MCSP_HEADERSIZE)
			{
				// complete header received
				ui8ReturnVal = GOT_HEADER;
				// go to next state
				ui8State = STATE_GET_PAYLOAD;
				// reset counter
				ui8Count = 0;
			}
			// part of header received
			else{ui8ReturnVal = GOT_HEADER_BYTE;}
		break;
		case STATE_GET_PAYLOAD:
			// store data
			Packet->ui8BUFFER[ui8Count] = ui8Byte;
			// calculate CRC
			ui8CRC = MCSP_CRC(ui8CRC, ui8Byte);
			if(++ui8Count < Packet->ui8DLC)
			{
				// part of payload received
				ui8ReturnVal = GOT_PAYLOAD_BYTE;
			}
			else
			{
				// whole payload received
				ui8ReturnVal = GOT_PAYLOAD;
				// go to next state
				ui8State = STATE_CHECK_CRC;
			}
		break;
		case STATE_CHECK_CRC:
			// in any case wait for next packet
			ui8State = STATE_GET_SOF;
			if(ui8Byte == ui8CRC)
			{
				ui8ReturnVal = CRC_OK;
				ui8CRC = 0;
			}
			else
			{
				ui8ReturnVal = CRC_FAIL;
				ui8CRC = 0;
			}
		break;
		default:
			// should never be reached
		break;
	}
	return (uint8_t)ui8ReturnVal;
}

//********************* MCSP_Encode() *********************
// Function to encode data to a MCSP packet. Data is stored in software transmit fifo and bursted out eventually
// Input: address of packet
// Output:
// Info: über eine Abfrage könnte über einen weiteren Parameter der UART ausgesucht werden
void MCSP_Encode(MCSP_PACKET_TYPE *Packet)
{
	// disable TX FIFO interrupt
	UART0_IM_R &= ~UART_IM_TXIM;

	uint8_t ui8CRC = 0;

	// SOF
	UART0_Txfifo_put(MCSP_SOF_ID);
	// DLC
	UART0_Txfifo_put((uint8_t)Packet->ui8DLC); ui8CRC = MCSP_CRC(ui8CRC, (uint8_t)Packet->ui8DLC);
	// ID
	UART0_Txfifo_put((uint8_t)Packet->ui8ID); ui8CRC = MCSP_CRC(ui8CRC, (uint8_t)Packet->ui8ID);

	// Payload
	int i;
	for (i = 0; i < (uint8_t)Packet->ui8DLC; i++)
	{
		UART0_Txfifo_put((uint8_t)Packet->ui8BUFFER[i]); ui8CRC = MCSP_CRC(ui8CRC, (uint8_t)Packet->ui8BUFFER[i]);
	}

	// CRC
	UART0_Txfifo_put(ui8CRC);

	// ready to send
	UART0_CopySW2HW();
	// enable Tx interrupt
	UART0_IM_R |= UART_IM_TXIM;
}

//********************* GrabFromBuffer() *********************
// Restores bits from buffer dependent on startposition and length inside frame
// Input: Startposition of data inside frame, datalenght in bits, buffer to grab data from
// Output: returns a 32-bit variable containing dthe desired data
// Info: only 32-bit wide variables can be restored
uint32_t GrabFromBuffer(MCSP_STARTPOSITION_TYPE RightShift, MCSP_DATALENGTH_TYPE Bitlength, uint8_t *Buffer)
{
	uint32_t temp = 0;

	// divide startposition by 8 (bits) to find start element of buffer for current value
	Buffer = Buffer + (RightShift >> 3);
	temp |= *Buffer >> RightShift;

	// calculate the remaining bits
	int8_t remainder = Bitlength + RightShift - 8;

	// get the remaining bits
	uint8_t i = 1;
	while (remainder > 0)
	{
		Buffer = Buffer + 1;
		temp |= *Buffer << (8*i-RightShift);
		remainder -= 8;
		i++;
	}
	return temp;
}

//********************* ShiftInBuffer() *********************
// Recursive function to store SendMsg data in send buffer
// Input: value to store, start position in buffer, pointer to buffer
// Output: -
// Info:
void ShiftInBuffer(uint32_t temp, MCSP_STARTPOSITION_TYPE LeftShift, uint8_t *Buffer)
{
	// divide startposition by 8 (bits) to find start element of buffer for current value
	Buffer = Buffer + (LeftShift>>3);

	// store least significant byte
	*Buffer |= (temp << (LeftShift%8));

	// calculate carry
	uint32_t carry = 0;
	carry = (*Buffer ^ (temp << (LeftShift % 8)))>>8;

	// check if a carry exists
	if (carry)
	{
		Buffer = Buffer + 1;
		// recursive call
		ShiftInBuffer(carry, (MCSP_STARTPOSITION_TYPE)0, Buffer);
	}
}

/*****************************************************************************
 *
 * 						Function definitions for receiving
 *
*****************************************************************************/

//********************* ReceiveMsg_MotorCtr() *********************
//
// Input:
// Output:
// Info:
void ReceiveMsg_MotorCtr(MCSP_PACKET_TYPE *Packet)
{
	uint32_t ui32temp = 0;

	// Runstate
	ui32temp = GrabFromBuffer(Start_MtrCtr_State, Length_MtrCtr_State, Packet->ui8BUFFER);
	MotorCtr.RunState = (Bit_RunState)(ui32temp&MOTOR_CTR_RUNSTATE_MASK);
	// Direction
	ui32temp = GrabFromBuffer(Start_MtrCtr_Dir, Length_MtrCtr_Dir, Packet->ui8BUFFER);
	MotorCtr.Direction = (Bit_Direction)((ui32temp)&MOTOR_CTR_DIRECTION_MASK);
	// Dutycycle
	ui32temp = GrabFromBuffer(Start_MtrCtr_Duty, Length_MtrCtr_Duty, Packet->ui8BUFFER);
	MotorCtr.DutyCycle = ((Unit_Percent)(ui32temp&MOTOR_CTR_DUTYCYCLE_MASK))*0.2f;

	// clear the buffer
	CLEAR_BUFFER()
}

/*****************************************************************************
 *
 * 						Function definitions for sending
 *
*****************************************************************************/

//********************* SendMsg_MotorCtr() *********************
//
// Input: Sendpacket
// Output:
// Info:
void  SendMsg_MotorCtr(MCSP_PACKET_TYPE *Packet)
{
	// clear the buffer
	CLEAR_BUFFER()

	// Set ID and DLC
	Packet->ui8ID = MtrCtr_ID;
	Packet->ui8DLC = MtrCtr_DLC;

	uint32_t ui32temp;

	// Runstate
	ui32temp = (uint32_t)(MotorCtr.RunState);
	ShiftInBuffer(ui32temp, Start_MtrCtr_State, Packet->ui8BUFFER);

	// Direction
	ui32temp = (uint32_t)(MotorCtr.Direction);
	ShiftInBuffer(ui32temp, Start_MtrCtr_Dir, Packet->ui8BUFFER);

	// Duty Cycle
	ui32temp = (uint32_t)(MotorCtr.DutyCycle * Fac_DutyCycle);
	ShiftInBuffer(ui32temp, Start_MtrCtr_Duty, Packet->ui8BUFFER);
}

//********************* SendMsg_MeasVal() *********************
//
// Input: Sendpacket
// Output:
// Info:
void  SendMsg_MeasVal(MCSP_PACKET_TYPE *Packet)
{
	// clear the buffer
	CLEAR_BUFFER()

	// Set ID and DLC
	Packet->ui8ID = MeasValues_ID;
	Packet->ui8DLC = MeasValues_DLC;

	uint32_t ui32temp;

	// Current
	ui32temp = ((uint32_t)(MeasValues.MotorCurrent * Fac_Current) + Offset_Mtr_Current);
	ShiftInBuffer(ui32temp, Start_MeasVal_Current, Packet->ui8BUFFER);

	// Voltage
	ui32temp = ((uint32_t)(MeasValues.MotorVoltage * Fac_Voltage) + Offset_Mtr_Voltage);
	ShiftInBuffer(ui32temp, Start_MeasVal_Voltage, Packet->ui8BUFFER);

	// Rpm
	ui32temp = ((uint32_t)(MeasValues.MotorRpm * Fac_Rpm) + Offset_Mtr_Rpm);
	ShiftInBuffer(ui32temp, Start_MeasVal_Rpm, Packet->ui8BUFFER);
}


