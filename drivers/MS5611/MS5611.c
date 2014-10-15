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
struct MS5611 stMS5611_Values = {0, 0, 0, 0, 0, 0, 0, 0, 0};

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
	uint16_t ui16TempVar[7];

	// Variable for loop
	uint8_t i = 0;

	// Reset MS5611
	ui32WriteBuffer[0] = MS5611_RESET;
	SPI_Write(&stMS5611_Handle, ui32WriteBuffer, 1);

	// Wait until MS5611 is ready (3ms @ 80 Mhz)
	DELAY_MS(6);

	// Clear FIFO
	SPI_ClearFIFO(&stMS5611_Handle, ui32ReadBuffer[0]);

	//
	// Read Calibration Parameters
	//

	// Init PROM Read Address to first calibration parameter
	ui32WriteBuffer[0] = MS5611_PROM_READ_BASE | MS5611_PRESSURE_SENS;

	// Cycle to read the calibration parameters
	for(i = 0; i < MS5611_NUMBER_CALIBRATION_PARAM; i++)
	{
		// MS5611 - Send instruction to Read Calibration Param i
		SPI_Write(&stMS5611_Handle, ui32WriteBuffer, 3);

		// Delay between SPI Operations
		SysCtlDelay(SPI_DELAY);

		// Receive 3 Byte response and store them in the same variable
		if ( false == SPI_ReadFIFO(&stMS5611_Handle, ui32ReadBuffer, 3) )
		{
#ifdef DEBUG_CB
			// Send error message
			UARTprintf("\nError: FIFO doesn't contain the amount of Bytes expected\n");
#endif
		}

		// Store value read in temp variable
		ui16TempVar[i] = (ui32ReadBuffer[1] << 8) | ui32ReadBuffer[2];

		// Increment the Address to read from PROM
		ui32WriteBuffer[0]++;
	}

	// Store calibration parameters
	stMS5611_Values.Pressure_Sens = ui16TempVar[0];
	stMS5611_Values.Pressure_Offset = ui16TempVar[1];
	stMS5611_Values.Temp_CS = ui16TempVar[2];
	stMS5611_Values.Temp_CO = ui16TempVar[3];
	stMS5611_Values.Temp_Reference = ui16TempVar[4];
	stMS5611_Values.Temp_Sens = ui16TempVar[5];
	stMS5611_Values.CRC = ui16TempVar[6];
}

void MS5611_UpdatePressure()
{
	// Arrays to hold the data to be written or read via SPI
	uint32_t ui32WriteBuffer[4] = {0,0,0,0};
	uint32_t ui32ReadBuffer[4] = {0,0,0,0};

	// Update Pressure value

	// MS5611 - Trigger ADC Conversion (Pressure with OSR 4096)
	ui32WriteBuffer[0] = MS5611_CONVERT_D1_OSR_4096;
	SPI_Write(&stMS5611_Handle, ui32WriteBuffer, 1);

	// Wait until MS5611 is ready (9ms @ 80 Mhz)
	DELAY_MS(18);

	// Clear FIFO
	SPI_ClearFIFO(&stMS5611_Handle, ui32ReadBuffer[0]);

	// MS5611 - Read ADC Conversion (Pressure) - CS Must be low and CLK toggle is needed for 32 Bits
	ui32WriteBuffer[0] = MS5611_READ_ADC;
	SPI_Write(&stMS5611_Handle, ui32WriteBuffer, 4);

	// Delay between SPI Operations
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
	stMS5611_Values.Pressure = (ui32ReadBuffer[1] << 16) | (ui32ReadBuffer[2] << 8) | ui32ReadBuffer[3];			// Create 24 Bit Output (Ignore first Byte)
}

void MS5611_UpdateTemp()
{
	// Arrays to hold the data to be written or read via SPI
	uint32_t ui32WriteBuffer[4] = {0,0,0,0};
	uint32_t ui32ReadBuffer[4] = {0,0,0,0};

	// Update Temperature value

	// MS5611 - Trigger ADC Conversion (Temperature with OSR 4096)
	ui32WriteBuffer[0] = MS5611_CONVERT_D2_OSR_4096;
	SPI_Write(&stMS5611_Handle, ui32WriteBuffer, 1);

	// Wait until MS5611 is ready (9ms @ 80 Mhz)
	DELAY_MS(18);

	// Clear FIFO
	SPI_ClearFIFO(&stMS5611_Handle, ui32ReadBuffer[0]);

	// MS5611 - Read ADC Conversion (Temperature) - CS Must be low and CLK toggle is needed for 32 Bits
	ui32WriteBuffer[0] = MS5611_READ_ADC;
	SPI_Write(&stMS5611_Handle, ui32WriteBuffer, 4);

	// Delay between SPI Operations
	SysCtlDelay(SPI_DELAY);

	// Receive 4 Byte response and store them in the same variable
	if ( false == SPI_ReadFIFO(&stMS5611_Handle, ui32ReadBuffer, 4) )
	{
#ifdef DEBUG_CB
		// Send error message
		UARTprintf("\nError: FIFO doesn't contain the amount of Bytes expected\n");
#endif
	}

	// Update Temperature value from the values read by SPI from the MS5611
	stMS5611_Values.Temperature = (ui32ReadBuffer[1] << 16) | (ui32ReadBuffer[2] << 8) | ui32ReadBuffer[3];			// Create 24 Bit Output (Ignore first Byte)
}
