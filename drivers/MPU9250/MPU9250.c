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

struct SPI_Slave stMPU9250_Handle = {IMU_SPI_BASE, CS_MPU9250_PORT, CS_MPU9250_PIN};

//*****************************************************************************
//
//		Functions
//
//*****************************************************************************

void MPU9250_Init()
{
	// Arrays to hold the data to be written or read via SPI
	uint32_t ui32WriteBuffer[2] = {0,0};
	uint32_t ui32ReadBuffer[2] = {0,0};

	// Reset MPU9250
	ui32WriteBuffer[0] = MPU9250_O_PWR_MGMT_1;
	ui32WriteBuffer[1] = MPU9250_PWR_MGMT_1_H_RESET;
	SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

	// Delay 12.5 ms
	DELAY_MS(25);

	// Change the clock source to best available source. PLL if ready (on the same register)
	ui32WriteBuffer[1] = MPU9250_PWR_MGMT_1_CLKSEL_AUTO_1;
	SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

	// Clear FIFO
	SPI_ClearFIFO(&stMPU9250_Handle, ui32ReadBuffer[0]);

	// Clear variable to start cycle
	ui32ReadBuffer[1] = 0x00;

	// If clock source is = to what was written to the MPU9250_O_PWR_MGMT_1 register, the device has woken up properly
	while(0x01 != ui32ReadBuffer[1])
	{
		// Make sure the device has woken up
		ui32WriteBuffer[0] = MPU9250_O_PWR_MGMT_1 | READ_FLAG;
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Read Data from RX FIFO
		if ( false == SPI_ReadFIFO(&stMPU9250_Handle, ui32ReadBuffer, 2) )
		{
	#ifdef DEBUG_CB
			// Send error message
			UARTprintf("\nError: FIFO doesn't contain the amount of Bytes expected\n");
	#endif
		}
	}

	// Send command to read the Device ID of the MPU9250
	ui32WriteBuffer[0] = MPU9250_O_WHO_AM_I | READ_FLAG;
	ui32WriteBuffer[1] = 0x00;										// Is this necessary?
	SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

	// Read Data from RX FIFO
	if ( false == SPI_ReadFIFO(&stMPU9250_Handle, ui32ReadBuffer, 2) )
	{
#ifdef DEBUG_CB
		// Send error message
		UARTprintf("\nError: FIFO doesn't contain the amount of Bytes expected\n");
#endif
	}

	// Check that the device ID is = 0x71. If the ID is not 0x71 the device is not working properly
	if(MPU9250_WHO_AM_I_MPU9250 == ui32ReadBuffer[1])
	{
		// Disable I2C Slave (Master for other sensors is independent)
		ui32WriteBuffer[0] = MPU9250_O_USER_CTRL;
		ui32WriteBuffer[1] = MPU9250_USER_CTRL_I2C_IF_DIS;
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Enable ACC and GYRO (all axis)
		ui32WriteBuffer[0] = MPU9250_O_PWR_MGMT_2;
		ui32WriteBuffer[1] = MPU9250_PWR_MGMT_2_ENABLE_ALL;
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// DLPF Enabled - Gyro Bandwidth 250 Hz / Temp Bandwidth 4000 Hz
		ui32WriteBuffer[0] = MPU9250_O_CONFIG;
		ui32WriteBuffer[1] = MPU9250_CONFIG_DLPF_CFG_250_4000;
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Config GYRO +-2000dps (Full Range)
		ui32WriteBuffer[0] = MPU9250_O_GYRO_CONFIG;
		ui32WriteBuffer[1] = MPU9250_GYRO_CONFIG_FS_SEL_2000;
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Config ACC +-4G
		ui32WriteBuffer[0] = MPU9250_O_ACCEL_CONFIG;
		ui32WriteBuffer[1] = MPU9250_ACCEL_CONFIG_AFS_SEL_4G;
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Config ACC DLPF (184Hz Output)
		ui32WriteBuffer[0] = MPU9250_O_ACCEL_CONFIG_2;
		ui32WriteBuffer[1] = MPU9250_ACCEL_CONFIG_2_ACCEL_DLPF_184;
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Config Interrupt pin - INT pin held high until a read operation is performed
		ui32WriteBuffer[0] = MPU9250_O_INT_PIN_CFG;
		ui32WriteBuffer[1] = MPU9250_INT_PIN_CFG_LATCH_INT_EN | MPU9250_INT_PIN_CFG_INT_ANYRD_2CLEAR;
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Config Interrupt pin - INT is triggered when new Data is Ready
		ui32WriteBuffer[0] = MPU9250_O_INT_ENABLE;
		ui32WriteBuffer[1] = MPU9250_INT_ENABLE_RAW_RDY_EN;
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// AK8963 Initialization //

/*		// Enable I2C Master to control AK8963
		ui32WriteBuffer[0] = MPU9250_O_USER_CTRL;
		ui32WriteBuffer[1] = MPU9250_USER_CTRL_I2C_MST_EN;
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// I2C bus to AK8963 speed set at 400Khz
		ui32WriteBuffer[0] = MPU9250_O_I2C_MST_CTRL;
		ui32WriteBuffer[1] = MPU9250_I2C_MST_CTRL_I2C_MST_CLK_400;
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Set the I2C slave addres of AK8963 and configure for write
		ui32WriteBuffer[0] = MPU9250_O_I2C_SLV0_ADDR;
		ui32WriteBuffer[1] = AK8963_ADDRESS;
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// I2C slave 0 register address from where to begin data transfer
		ui32WriteBuffer[0] = MPU9250_O_I2C_SLV0_REG;
		ui32WriteBuffer[1] = 0x0B;
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Reset AK8963
		ui32WriteBuffer[0] = MPU9250_O_I2C_SLV0_DO;
		ui32WriteBuffer[1] = 0x01;
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		//
		ui32WriteBuffer[0] = MPU9250_O_I2C_SLV0_CTRL;
		ui32WriteBuffer[1] = 0x81;
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// I2C slave 0 register address from where to begin data transfer
		ui32WriteBuffer[0] = MPU9250_O_I2C_SLV0_REG;
		ui32WriteBuffer[1] = 0x0A;
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Register value to continuous measurement in 16bit
		ui32WriteBuffer[0] = MPU9250_O_I2C_SLV0_DO;
		ui32WriteBuffer[1] = 0x12;
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		//
		ui32WriteBuffer[0] = MPU9250_O_I2C_SLV0_CTRL;
		ui32WriteBuffer[1] = 0x81;
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);*/
	}
	else
	{
#ifdef DEBUG_CB
		// Return error! The Device ID is not 0x71!
		UARTprintf("\nError: The Device ID of sensor MPU9250 is not 0x71\n");
#endif
	}
}
