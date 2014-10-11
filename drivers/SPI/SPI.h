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

#define SPI_DELAY					700

#define SPI_COMM_LENGTH				8										// Possible values [16]
#define SPI_COMM_LENGTH_BYTES		(SPI_COMM_LENGTH/8)

//*****************************************************************************
//
//		Structures
//
//*****************************************************************************

struct SPI_Slave
{
	uint32_t ui32SPI_BaseAddress;
	uint32_t ui32SPI_CS_Port;
	uint32_t ui32SPI_CS_Pin;
};

//*****************************************************************************
//
//		Function Prototypes
//
//*****************************************************************************

bool SPI_Init(uint8_t ui8SPI_Module_Sel);
bool SPI_Write(struct SPI_Slave stHandle, uint32_t* ui32WriteBuffer, uint8_t ui8Length);

#endif /* SPI_H_ */
