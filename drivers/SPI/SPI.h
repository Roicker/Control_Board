/*
 * SPI.h
 *
 *  Created on: 09/10/2014
 *      Author: Ray
 */

#ifndef SPI_H_
#define SPI_H_

//*****************************************************************************
//
//		Local Header Files
//
//*****************************************************************************

#include "Config.h"

//*****************************************************************************
//
//		Definitions
//
//*****************************************************************************

#define SPI_MODULE_0				0
#define SPI_MODULE_1				1
#define SPI_MODULE_2				2
#define SPI_MODULE_3				3

#define SPI_DELAY					750  // 700
#define I2C_DELAY					7000 // 6000

#define SPI_COMM_LENGTH				8										// Possible values [16]
#define SPI_COMM_LENGTH_BYTES		(SPI_COMM_LENGTH/8)

#define SPI_FIFO_LENGTH				8
#define SPI_FIFO_DATA_BYTES			7

//*****************************************************************************
//
//		Structures
//
//*****************************************************************************

struct SPI_Slave
{
	const uint32_t ui32SPI_BaseAddress;
	const uint32_t ui32SPI_CSPort;
	const uint32_t ui32SPI_CSPin;
};

//*****************************************************************************
//
//		Function Prototypes
//
//*****************************************************************************

bool SPI_Init(uint8_t ui8SPI_Module_Sel);
bool SPI_Write(struct SPI_Slave* stDeviceHandle, uint32_t* ui32WriteBuffer, uint16_t ui16Length);
void SPI_ClearFIFO(struct SPI_Slave* stDeviceHandle, uint32_t ui32ReadVar);
bool SPI_ReadFIFO(struct SPI_Slave* stDeviceHandle, uint32_t* ui32ReadBuffer, uint16_t ui16Length);

#endif /* SPI_H_ */
