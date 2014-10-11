//*****************************************************************************
//
// MPU9250.c - MPU9250 Driver
//
// Copyright (c) 2014 Raymundo Magaña Gomez - http://rmg.mx
//
// Software License Agreement
//
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
//
//*****************************************************************************

//*****************************************************************************
//
//		Local Header Files
//
//*****************************************************************************

#include "drivers/MPU9250/MPU9250.h"

//*****************************************************************************
//
//		Global Variables
//
//*****************************************************************************

static struct SPI_Slave stMPU9250_Handle = {IMU_SPI_BASE, CS_MPU9250_PORT, CS_MPU9250_PIN};

//*****************************************************************************
//
//		Functions
//
//*****************************************************************************

void MPU9250_Init()
{
	if( (stMPU9250_Handle.ui32SPI_BaseAddress == IMU_SPI_BASE) && (stMPU9250_Handle.ui32SPI_CS_Pin == CS_MPU9250_PIN) && (stMPU9250_Handle.ui32SPI_CS_Port == CS_MPU9250_PORT) )
	{
		DELAY_MS(1);
	}
	// Init code for MPU9250
}
