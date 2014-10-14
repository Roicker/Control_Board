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

struct SPI_Slave stMS5611_Handle = {IMU_SPI_BASE, CS_MS5611_PORT, CS_MS5611_PIN};
uint32_t ui32MS5611_Temperature = 0;
uint32_t ui32MS5611_Pressure = 0;

//*****************************************************************************
//
//		Functions
//
//*****************************************************************************

void MS5611_Init()
{
	// Arrays to hold the data to be written or read via SPI
	uint32_t ui32WriteBuffer[4] = {0,0,0,0};
	uint32_t ui32ReadBuffer[4] = {0,0,0,0};

	// Reset MS5611
	ui32WriteBuffer[0] = 0x1E;
	SPI_Write(&stMS5611_Handle, ui32WriteBuffer, 1);

	// Wait until MS5611 is ready (3ms @ 80 Mhz)
	DELAY_MS(6);

	// MS5611 - Trigger ADC Conversion (Pressure)
	ui32WriteBuffer[0] = 0x48;
	SPI_Write(&stMS5611_Handle, ui32WriteBuffer, 1);

	// Wait until MS5611 is ready (9ms @ 80 Mhz)
	DELAY_MS(18);

	// Clear FIFO
	SPI_ClearFIFO(&stMS5611_Handle, ui32ReadBuffer[0]);

	// MS5611 - Read ADC Conversion (Pressure) - CS Must be low and CLK toggle is needed for 32 Bits
	ui32WriteBuffer[0] = 0x00;
	SPI_Write(&stMS5611_Handle, ui32WriteBuffer, 4);

	// Wait
	SysCtlDelay(SPI_DELAY);

	// Receive 4 Byte response and store them in the same variable
	if ( false == SPI_ReadFIFO(&stMS5611_Handle, ui32ReadBuffer, 4) )
	{
#ifdef DEBUG_CB
		// Send error message
		UARTprintf("\nError: FIFO doesn't contain the amount of Bytes expected\n");
#endif
	}

	// Update Pressure value from the values read by SPI from the MS5611
	ui32MS5611_Pressure = (ui32ReadBuffer[1] << 16) | (ui32ReadBuffer[2] << 8) | ui32ReadBuffer[3];			// Create 24 Bit Output (Ignore first Byte)
}

void MS5611_UpdatePressure()
{
	// Update Pressure value
	//ui32MS5611_Pressure = NEW VALUE
}

void MS5611_UpdateTemp()
{
	// Update Temperature value
	//ui32MS5611_Temperature = NEW VALUE
}
