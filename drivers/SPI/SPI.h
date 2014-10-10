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
//		Function Prototypes
//
//*****************************************************************************

bool SPI_Init(uint8_t ui8SPI_Module_Sel);

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

// CS Line Config for MS5611
#define CS_MS5611_PERIPH			SYSCTL_PERIPH_GPIOB						// Intentify on which Peripheral is the CS line of the MS5611
#define CS_MS5611_PORT				GPIO_PORTB_BASE							// Intentify on which Port is the CS line of the MS5611
#define CS_MS5611_PIN				GPIO_PIN_6								// Intentify on which Pin is the CS line of the MS5611

// CS Line Config for MPU9250
#define CS_MPU9250_PERIPH			SYSCTL_PERIPH_GPIOA						// Intentify on which Peripheral is the CS line of the MPU9250
#define CS_MPU9250_PORT				GPIO_PORTA_BASE							// Intentify on which Port is the CS line of the MPU9250
#define CS_MPU9250_PIN				GPIO_PIN_3								// Intentify on which Pin is the CS line of the MPU9250

#endif /* SPI_H_ */
