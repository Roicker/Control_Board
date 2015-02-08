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
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. WE SHALL NOT, UNDER ANY
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

//*****************************************************************************
//	SPI_Init
//*****************************************************************************

bool SPI_Init(uint8_t ui8SPI_Module_Sel)
{
	bool ret = false; 															// Init return bool var

	switch(ui8SPI_Module_Sel)
	{
	case SPI_MODULE_0:
		// Enable SSI0
		SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);

		// Enable Peripheral (Port A) for SPI
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
		// Enable SSI1
		SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI1);

		// Enable Port D for SPI
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

		// Enable Peripheral for MS5611 if it has not been activated
		if(CS_MS5611_PERIPH != SYSCTL_PERIPH_GPIOD)
		{
			SysCtlPeripheralEnable(CS_MS5611_PERIPH);
		}

		// Enable Peripheral for MPU9250 if it has not been activated
		if( (CS_MPU9250_PERIPH != SYSCTL_PERIPH_GPIOD) && (CS_MPU9250_PERIPH != CS_MS5611_PERIPH) )
		{
			SysCtlPeripheralEnable(CS_MPU9250_PERIPH);
		}

		// Init CS Line MS5611
		GPIOPinTypeGPIOOutput(CS_MS5611_PORT, CS_MS5611_PIN);

		// Init CS Line MPU9250
		GPIOPinTypeGPIOOutput(CS_MPU9250_PORT, CS_MPU9250_PIN);

		// Config Interrupt pin for MPU9250?

		// Config SSI1 Pins
		GPIOPinConfigure(GPIO_PD0_SSI1CLK);
		GPIOPinConfigure(GPIO_PD2_SSI1RX);
		GPIOPinConfigure(GPIO_PD3_SSI1TX);
		GPIOPinTypeSSI(GPIO_PORTD_BASE,GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_0);

		//SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), IMU_SPI_MODE, SSI_MODE_MASTER, IMU_SPI_BAUDRATE, SPI_COMM_LENGTH); // Error in SysCtlClockGet on TIVAWARE 2.1.0
		// Config SSI1
		SSIConfigSetExpClk(SSI1_BASE, SYSCLK, IMU_SPI_MODE, SSI_MODE_MASTER, IMU_SPI_BAUDRATE, SPI_COMM_LENGTH);

		// Enable SSI1
		SSIEnable(SSI1_BASE);

		// Indicate that the initialization was performed correctly
		ret = true;
		break;
	case SPI_MODULE_2:
		// If needed, other SPI Modules (2 or 3) can be configured. This proyect uses the Module 1 (previously 0) for the IMU.
	case SPI_MODULE_3:
		// If needed, other SPI Modules (2 or 3) can be configured. This proyect uses the Module 1 (previously 0) for the IMU.
	default:
#ifdef DEBUG_CB
		// Send error message
		UARTprintf("\nError: Selected SPI Module is not available\n");
#endif
		break;																	// Use ret to notify that the selected module doesn't exist
	}

	return ret;
}

//*****************************************************************************
//	SPI_Write
//*****************************************************************************

bool SPI_Write(struct SPI_Slave* stDeviceHandle, uint32_t* ui32WriteBuffer, uint16_t ui16Length)
{
	// Init counter for loop
	uint16_t i = 0;

	// Init return bool var
	bool ret = false;

	// Check the input parameters - Is the test (0 == stDeviceHandle.ui32SPI_BaseAddress) necessary?
	if( (NULL == ui32WriteBuffer) || (0 == stDeviceHandle->ui32SPI_BaseAddress) )
	{
#ifdef DEBUG_CB
		// Send error message
		UARTprintf("\nError: Input parameters for SPI_Write are invalid\n");
#endif
	}
	else
	{
		// Pull CS Line Low
		GPIOPinWrite(stDeviceHandle->ui32SPI_CSPort, stDeviceHandle->ui32SPI_CSPin, 0);

		// Loop to send characters (value of ui8Length)
		for(i = 0; i < ui16Length; i++)
		{
			// Write SPI_COMM_LENGTH Bits (Length configured in SPI_Init)
			SSIDataPut(stDeviceHandle->ui32SPI_BaseAddress, ui32WriteBuffer[i]);
		}

		// Wait for SPI Write to complete (Should this be replaced with Interrupt?? or removed? is SSIDataPut blocking?)
		while(SSIBusy(stDeviceHandle->ui32SPI_BaseAddress))
		{
		}

		// Pull CS Line High
		GPIOPinWrite(stDeviceHandle->ui32SPI_CSPort, stDeviceHandle->ui32SPI_CSPin, stDeviceHandle->ui32SPI_CSPin);

		ret = true;
	}

	// Return value to indicate if function call was successful
	return ret;
}

//*****************************************************************************
//	SPI_ClearFIFO
//*****************************************************************************

void SPI_ClearFIFO(struct SPI_Slave* stDeviceHandle, uint32_t ui32ReadVar)
{
	while(SSIDataGetNonBlocking(stDeviceHandle->ui32SPI_BaseAddress, &ui32ReadVar))
	{
	}
}

//*****************************************************************************
//	SPI_ReadFIFO
//*****************************************************************************

bool SPI_ReadFIFO(struct SPI_Slave* stDeviceHandle, uint32_t* ui32ReadBuffer, uint16_t ui16Length)
{
	// Init counter for loop
	uint16_t i = 0;

	// Init return bool var
	bool ret = true;

	// Read the amount of bytes requested from the FIFO
	for(i = 0; i < ui16Length; i++)
	{
		// Get data from FIFO - if FIFO is empty, the function returns 0
		if(0 == SSIDataGetNonBlocking(stDeviceHandle->ui32SPI_BaseAddress, &ui32ReadBuffer[i]) )
		{
			// FIFO is empty
			ret = false;
		}
	}

	// Return variable to signal if function was successful
	return ret;
}

