//*****************************************************************************
//
// SPI.c - SPI Driver
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
//		Header files
//
//*****************************************************************************

#include "drivers/SPI/SPI.h"

//*****************************************************************************
//
//		Functions
//
//*****************************************************************************

bool SPI_Init(uint8_t ui8SPI_Module_Sel)
{
	bool ret = false; 															// Init return bool var

	switch(ui8SPI_Module_Sel)
	{
	case SPI_MODULE_0:
		// Enable SSI0
		SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);

		// Enable Port A for SPI
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

		// Enable Peripheral for MS5611 if it has not been activated
		if(CS_MS5611_PERIPH != SYSCTL_PERIPH_GPIOA)
		{
			SysCtlPeripheralEnable(CS_MS5611_PERIPH);
		}

		// Enable Peripheral for MPU9250 if it has not been activated
		if( (CS_MPU9250_PERIPH != SYSCTL_PERIPH_GPIOA) && (CS_MPU9250_PERIPH != CS_MS5611_PERIPH) )
		{
			SysCtlPeripheralEnable(CS_MPU9250_PERIPH);
		}

		// Init CS Line MS5611
		GPIOPinTypeGPIOOutput(CS_MS5611_PORT, CS_MS5611_PIN);

		// Init CS Line MPU9250
		GPIOPinTypeGPIOOutput(CS_MPU9250_PORT, CS_MPU9250_PIN);

		// Config Interrupt pin for MPU9250?

		// Config SSI0 Pins
		GPIOPinConfigure(GPIO_PA2_SSI0CLK);
		GPIOPinConfigure(GPIO_PA4_SSI0RX);
		GPIOPinConfigure(GPIO_PA5_SSI0TX);
		GPIOPinTypeSSI(GPIO_PORTA_BASE,GPIO_PIN_5|GPIO_PIN_4|GPIO_PIN_2);

		//SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), IMU_SPI_MODE, SSI_MODE_MASTER, IMU_SPI_BAUDRATE, SPI_COMM_LENGTH); // Error in SysCtlClockGet on TIVAWARE 2.1.0
		// Config SSI0
		SSIConfigSetExpClk(SSI0_BASE, SYSCLK, IMU_SPI_MODE, SSI_MODE_MASTER, IMU_SPI_BAUDRATE, SPI_COMM_LENGTH);

		// Enable SSI0
		SSIEnable(SSI0_BASE);

		// Indicate that the initialization was performed correctly
		ret = true;
		break;
	case SPI_MODULE_1:
		// If needed, other SPI Modules (1, 2 or 3) can be configured. This proyect uses the Module 0 for the IMU.
	case SPI_MODULE_2:
		// If needed, other SPI Modules (1, 2 or 3) can be configured. This proyect uses the Module 0 for the IMU.
	case SPI_MODULE_3:
		// If needed, other SPI Modules (1, 2 or 3) can be configured. This proyect uses the Module 0 for the IMU.
	default:
#ifdef DEBUG_CB
		// Send error message
		UARTprintf("\nError: Selected SPI Module is not available\n");
#endif
		break;																	// Use ret to notify that the selected module doesn't exist
	}

	return ret;
}

bool SPI_Write(struct SPI_Slave stHandle, uint32_t* ui32WriteBuffer, uint8_t ui8Length)
{
	// Init counter for loop
	uint8_t i = 0;

	// Init return bool var
	bool ret = false;

	// Check the input parameters - Is the test (0 == stHandle.ui32SPI_BaseAddress) necessary?
	if( (NULL == ui32WriteBuffer) || (0 == stHandle.ui32SPI_BaseAddress) )
	{
#ifdef DEBUG_CB
		// Send error message
		UARTprintf("\nError: Input parameters for SPI_Write are invalid\n");
#endif
	}
	else
	{
		// Pull CS Line Low
		GPIOPinWrite(stHandle.ui32SPI_CS_Port , stHandle.ui32SPI_CS_Pin, 0);

		// Loop to send characters (value of ui8Length)
		for(i = 0; i < ui8Length; i++)
		{
			// Write SPI_COMM_LENGTH Bits (Length configured in SPI_Init)
			SSIDataPut(stHandle.ui32SPI_BaseAddress, ui32WriteBuffer[i]);
		}

		// Wait for SPI Write to complete (Should this be replaced with Interrupt?? or removed? is SSIDataPut blocking?)
		while(SSIBusy(stHandle.ui32SPI_BaseAddress))
		{
		}

		// Pull CS Line High
		GPIOPinWrite(stHandle.ui32SPI_CS_Port , stHandle.ui32SPI_CS_Pin, stHandle.ui32SPI_CS_Pin);

		// Delay for the amount of cycles defined in SPI_DELAY - Commented out because each sensor has different timing requirements
		//SysCtlDelay(SPI_DELAY);

		ret = true;
	}

	// Return if function call was successful
	return ret;
}

