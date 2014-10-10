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

		//SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_3, SSI_MODE_MASTER, 1000000, SPI_COMM_LENGTH); // Error in SysCtlClockGet on TIVAWARE 2.1.0
		// Config SSI0
		SSIConfigSetExpClk(SSI0_BASE, SYSCLK, SSI_FRF_MOTO_MODE_3, SSI_MODE_MASTER, 1000000, SPI_COMM_LENGTH);

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

