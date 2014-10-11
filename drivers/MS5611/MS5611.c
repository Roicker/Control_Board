//*****************************************************************************
//
// MS5611.c - MS5611 Driver
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

#include "drivers/MS5611/MS5611.h"

//*****************************************************************************
//
//		Global Variables
//
//*****************************************************************************

static struct SPI_Slave stMS5611_Handle = {IMU_SPI_BASE, CS_MS5611_PORT, CS_MS5611_PIN};

//*****************************************************************************
//
//		Functions
//
//*****************************************************************************

void MS5611_Init()
{
	// Init code for MS5611
}

