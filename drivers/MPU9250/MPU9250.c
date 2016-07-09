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
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. WE SHALL NOT, UNDER ANY
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

// Structures to hold sensor data
struct SPI_Slave stMPU9250_Handle = {IMU_SPI_BASE, CS_MPU9250_PORT, CS_MPU9250_PIN};
struct MPU9250 stMPU9250_Values[2] =	{
											{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
											{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
										};

// Variable to store which stMPU9250_Values is currently buffer
uint8_t MPU9250_BufferVar = 0;

// Calibration flags
bool AccGyro_Calibrated = true;
bool Mag_Calibrated = true;

// Variables to store Mag calibration read from AK8963
uint16_t AK8963_X_CAL, AK8963_Y_CAL, AK8963_Z_CAL;

// Init buffer
uint32_t ui32ClearBuffer[513];

//*****************************************************************************
//
//		Functions
//
//*****************************************************************************

//*****************************************************************************
//		MPU9250_Init
//*****************************************************************************

void MPU9250_Init()
{
	// Arrays to hold the data to be written or read via SPI
	uint32_t ui32WriteBuffer[4] = {0,0,0,0};
	uint32_t ui32ReadBuffer[4] = {0,0,0,0};

	// Set ui32ClearBuffer
	//memset(ui32ClearBuffer, 0xAFAF, 513);

	// MPU9250 Initialization //

	// Reset MPU9250
	ui32WriteBuffer[0] = MPU9250_O_PWR_MGMT_1;
	ui32WriteBuffer[1] = MPU9250_PWR_MGMT_1_H_RESET;
	SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

	// Delay 12.5 ms
	DELAY_MS(25);

	// Change the clock source to best available source. PLL if ready (on the same register)
	ui32WriteBuffer[1] = MPU9250_PWR_MGMT_1_CLKSEL_AUTO_1;
	SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

	// Delay between SPI Operations
	SysCtlDelay(SPI_DELAY);

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

		// Delay between SPI Operations
		SysCtlDelay(SPI_DELAY);
	}

	// Send command to read the Device ID of the MPU9250
	ui32WriteBuffer[0] = MPU9250_O_WHO_AM_I | READ_FLAG;
	// Sending 0x00 in second Byte seems to be unnecessary, but better safe than sorry
	ui32WriteBuffer[1] = 0x00;
	SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

	// Delay between SPI Operations
	SysCtlDelay(SPI_DELAY);

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
		// Reset FIFO
		ui32WriteBuffer[0] = MPU9250_O_USER_CTRL;
		ui32WriteBuffer[1] = MPU9250_USER_CTRL_FIFO_RESET;
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Delay between SPI Operations
		SysCtlDelay(SPI_DELAY);

//		// Disable I2C Slave
//		ui32WriteBuffer[0] = MPU9250_O_USER_CTRL;
//		ui32WriteBuffer[1] = MPU9250_USER_CTRL_I2C_IF_DIS;
//		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Delay between SPI Operations
		SysCtlDelay(SPI_DELAY);

		// Enable ACC and GYRO (all axis)
		ui32WriteBuffer[0] = MPU9250_O_PWR_MGMT_2;
		ui32WriteBuffer[1] = MPU9250_PWR_MGMT_2_ENABLE_ALL;
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Delay between SPI Operations
		SysCtlDelay(SPI_DELAY);

		// DLPF Enabled - Gyro Bandwidth 184 Hz / Temp Bandwidth 188 Hz - ODR 1 kHz
		ui32WriteBuffer[0] = MPU9250_O_CONFIG;
		ui32WriteBuffer[1] = MPU9250_CONFIG_DLPF_CFG_184_188;
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Delay between SPI Operations
		SysCtlDelay(SPI_DELAY);

		// Config GYRO +-2000dps (Full Range)
		ui32WriteBuffer[0] = MPU9250_O_GYRO_CONFIG;
		ui32WriteBuffer[1] = MPU9250_GYRO_CONFIG_FS_SEL_2000;
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Delay between SPI Operations
		SysCtlDelay(SPI_DELAY);

		// Config ACC +-4G
		ui32WriteBuffer[0] = MPU9250_O_ACCEL_CONFIG;
		ui32WriteBuffer[1] = MPU9250_ACCEL_CONFIG_AFS_SEL_4G;
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Delay between SPI Operations
		SysCtlDelay(SPI_DELAY);

		// Config ACC DLPF (184Hz Output)
		ui32WriteBuffer[0] = MPU9250_O_ACCEL_CONFIG_2;
		ui32WriteBuffer[1] = MPU9250_ACCEL_CONFIG_2_ACCEL_DLPF_184;
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Delay between SPI Operations
		SysCtlDelay(SPI_DELAY);

		/////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////
		// AK8963 Initialization ////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////

		// Disable I2C Slave, Enable I2C Master
		ui32WriteBuffer[0] = MPU9250_O_USER_CTRL;
		ui32WriteBuffer[1] = MPU9250_USER_CTRL_I2C_MST_EN | MPU9250_USER_CTRL_I2C_IF_DIS;
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Delay between SPI Operations
		SysCtlDelay(SPI_DELAY);

		// I2C bus to AK8963 speed set at 400Khz, multi master enable and enable data ready interrupt until external sensor data is loaded
		ui32WriteBuffer[0] = MPU9250_O_I2C_MST_CTRL;
		ui32WriteBuffer[1] = MPU9250_I2C_MST_CTRL_MULT_MST_EN | MPU9250_I2C_MST_CTRL_I2C_MST_CLK_400 | MPU9250_I2C_MST_CTRL_WAIT_FOR_ES;
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Delay between SPI Operations
		SysCtlDelay(SPI_DELAY);

		///////////////////////////////////////////////////////////////////////////

		// Reset the AK8963 ///////////////////////////////////////////////////////

		// Set the I2C slave addres of AK8963 and configure for write
		ui32WriteBuffer[0] = MPU9250_O_I2C_SLV0_ADDR;
		ui32WriteBuffer[1] = AK8963_ADDRESS;
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Delay between SPI Operations
		SysCtlDelay(SPI_DELAY);

		// Write to I2C slave 0 the address from where to begin data transfer write
		ui32WriteBuffer[0] = MPU9250_O_I2C_SLV0_REG;
		ui32WriteBuffer[1] = AK8963_CNTL2;
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Delay between SPI Operations
		SysCtlDelay(SPI_DELAY);

		// Write the Data to be transferred to AK8963
		ui32WriteBuffer[0] = MPU9250_O_I2C_SLV0_DO;
		ui32WriteBuffer[1] = AK8963_CNTL2_RESET; // Reset AK8963
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Delay between SPI Operations
		SysCtlDelay(SPI_DELAY);

		// Enable I2C Transmission - Send 1 Byte
		ui32WriteBuffer[0] = MPU9250_O_I2C_SLV0_CTRL;
		ui32WriteBuffer[1] = MPU9250_I2C_SLV0_CTRL_EN_T1; // Highest bit enables the I2C and the lowest 4 bits set the transfer length
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Delay between after reset of AK8963
		SysCtlDelay(I2C_DELAY);
		SysCtlDelay(I2C_DELAY);

		/////////////////////////////////////////////////////////////////////////////

		// Set AK8963 in FUSE Mode //////////////////////////////////////////////////

		// Write to I2C slave 0 the address from where to begin data transfer write
		ui32WriteBuffer[0] = MPU9250_O_I2C_SLV0_REG;
		ui32WriteBuffer[1] = AK8963_CNTL1;
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Delay between SPI Operations
		SysCtlDelay(SPI_DELAY);

		// Write the Data to be transferred to AK8963
		ui32WriteBuffer[0] = MPU9250_O_I2C_SLV0_DO;
		ui32WriteBuffer[1] = AK8963_CNTL1_FUSE_ROM; // Set AK8963 in FUSE mode
		// ui32WriteBuffer[1] = AK8963_CNTL1_16_BIT | AK8963_CNTL1_FUSE_ROM; // Set AK8963 in FUSE mode
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Delay between SPI Operations
		SysCtlDelay(SPI_DELAY);

		// Enable I2C Transmission - Send 1 Byte
		ui32WriteBuffer[0] = MPU9250_O_I2C_SLV0_CTRL;
		ui32WriteBuffer[1] = MPU9250_I2C_SLV0_CTRL_EN_T1; // Highest bit enables the I2C and the lowest 4 bits set the transfer length
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Delay after setting AK8963 in FUSE Mode
		SysCtlDelay(I2C_DELAY);
		SysCtlDelay(I2C_DELAY);
		SysCtlDelay(I2C_DELAY);
		SysCtlDelay(I2C_DELAY);

		/////////////////////////////////////////////////////////////////////////////

		// Read the AK8963 ID ///////////////////////////////////////////////////////

		// Set the I2C slave addres of AK8963 and configure for read
		ui32WriteBuffer[0] = MPU9250_O_I2C_SLV0_ADDR;
		ui32WriteBuffer[1] = AK8963_ADDRESS | 0x80;
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Delay between SPI Operations
		SysCtlDelay(SPI_DELAY);

		// Write to I2C slave 0 the address from where to begin data transfer (read or write)
		ui32WriteBuffer[0] = MPU9250_O_I2C_SLV0_REG;
		ui32WriteBuffer[1] = AK8963_WIA;	// Read AK8963 ID
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Delay between SPI Operations
		SysCtlDelay(SPI_DELAY);

		// Enable I2C Transmission - Receive 1 Byte
		ui32WriteBuffer[0] = MPU9250_O_I2C_SLV0_CTRL;
		ui32WriteBuffer[1] = MPU9250_I2C_SLV0_CTRL_EN_T1; // Highest bit enables the I2C and the lowest 4 bits set the transfer length
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Delay between SPI Operations
		SysCtlDelay(I2C_DELAY);

		// Clear FIFO
		SPI_ClearFIFO(&stMPU9250_Handle, ui32ReadBuffer[0]);

		// Send command to read the Device ID of the AK8963
		ui32WriteBuffer[0] = MPU9250_O_EXT_SENS_DATA_00 | READ_FLAG;
		// Sending 0x00 in second Byte seems to be unnecessary, but better safe than sorry
		ui32WriteBuffer[1] = 0x00;
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Delay between SPI Operations
		SysCtlDelay(SPI_DELAY);

		// Read Data from RX FIFO
		if ( false == SPI_ReadFIFO(&stMPU9250_Handle, ui32ReadBuffer, 2) )
		{
#ifdef DEBUG_CB
			// Send error message
			UARTprintf("\nError: FIFO doesn't contain the amount of Bytes expected\n");
#endif
		}

		// Check that the device ID of the AK8963 is = 0x48. If the ID is not 0x48 the device is not working properly
		if(AK8963_DEVICE_ID != ui32ReadBuffer[1])
		{
#ifdef DEBUG_CB
			// Send error message
			UARTprintf("\nError: AK8963 ID is not 0x48. ID read is: %x\n", ui32ReadBuffer[1]);
#endif
		}

		/////////////////////////////////////////////////////////////////////////////

		// Read the AK8963 Sensitivity adjustment values ////////////////////////////

		// Set the I2C slave addres of AK8963 and configure for read
		ui32WriteBuffer[0] = MPU9250_O_I2C_SLV0_ADDR;
		ui32WriteBuffer[1] = AK8963_ADDRESS | 0x80;
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Delay between SPI Operations
		SysCtlDelay(SPI_DELAY);

		// Write to I2C slave 0 the address from where to begin data transfer (read or write)
		ui32WriteBuffer[0] = MPU9250_O_I2C_SLV0_REG;
		ui32WriteBuffer[1] = AK8963_ASAX;	// Register containing the first sensitivity value
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Delay between SPI Operations
		SysCtlDelay(SPI_DELAY);

		// Enable I2C Transmission - Receive 3 Bytes
		ui32WriteBuffer[0] = MPU9250_O_I2C_SLV0_CTRL;
		ui32WriteBuffer[1] = MPU9250_I2C_SLV0_CTRL_EN_T3; // Highest bit enables the I2C and the lowest 4 bits set the transfer length
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Delay between SPI Operations
		SysCtlDelay(I2C_DELAY);

		// Clear FIFO
		SPI_ClearFIFO(&stMPU9250_Handle, ui32ReadBuffer[0]);

		// Send command to read the sensitivity data from AK8963
		ui32WriteBuffer[0] = MPU9250_O_EXT_SENS_DATA_00 | READ_FLAG;
		// Sending 0x00 in second Byte seems to be unnecessary, but better safe than sorry
		ui32WriteBuffer[1] = 0x00;
		// Sending 0x00 in third Byte seems to be unnecessary, but better safe than sorry
		ui32WriteBuffer[2] = 0x00;
		// Sending 0x00 in fourth Byte seems to be unnecessary, but better safe than sorry
		ui32WriteBuffer[3] = 0x00;
		// Send read command
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 4);

		// Delay between SPI Operations
		SysCtlDelay(SPI_DELAY);

		// Read Data from RX FIFO
		if ( false == SPI_ReadFIFO(&stMPU9250_Handle, ui32ReadBuffer, 4) )
		{
	#ifdef DEBUG_CB
			// Send error message
			UARTprintf("\nError: FIFO doesn't contain the amount of Bytes expected\n");
	#endif
		}

		// Print device sensitivity
#ifdef DEBUG_CB
		// Send error message
		UARTprintf("\n\n ASAX: %d  ASAY: %d  ASAZ: %d\n", ui32ReadBuffer[1], ui32ReadBuffer[2], ui32ReadBuffer[3]);
#endif

		/////////////////////////////////////////////////////////////////////////////

		// Power Down AK8963 ////////////////////////////////////////////////////////

		// Set the I2C slave addres of AK8963 and configure for write
		ui32WriteBuffer[0] = MPU9250_O_I2C_SLV0_ADDR;
		ui32WriteBuffer[1] = AK8963_ADDRESS;
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Delay between SPI Operations
		SysCtlDelay(SPI_DELAY);

		// Write to I2C slave 0 the address from where to begin data transfer write
		ui32WriteBuffer[0] = MPU9250_O_I2C_SLV0_REG;
		ui32WriteBuffer[1] = AK8963_CNTL1;
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Delay between SPI Operations
		SysCtlDelay(SPI_DELAY);

		// Write the Data to be transferred to AK8963
		ui32WriteBuffer[0] = MPU9250_O_I2C_SLV0_DO;
		ui32WriteBuffer[1] = AK8963_CNTL1_POWER_DOWN; // Power Down AK8963
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Delay between SPI Operations
		SysCtlDelay(SPI_DELAY);

		// Enable I2C Transmission - Send 1 Byte
		ui32WriteBuffer[0] = MPU9250_O_I2C_SLV0_CTRL;
		ui32WriteBuffer[1] = MPU9250_I2C_SLV0_CTRL_EN_T1; // Highest bit enables the I2C and the lowest 4 bits set the transfer length
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Delay between SPI Operations
		SysCtlDelay(I2C_DELAY);

		/////////////////////////////////////////////////////////////////////////////

		// Setup the AK8963 for continuous operation ////////////////////////////////

		// Write to I2C slave 0 the address from where to begin data transfer (read or write)
		ui32WriteBuffer[0] = MPU9250_O_I2C_SLV0_REG;
		ui32WriteBuffer[1] = AK8963_CNTL1;
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Delay between SPI Operations
		SysCtlDelay(SPI_DELAY);

		// Write the Data to be transferred to AK8963
		ui32WriteBuffer[0] = MPU9250_O_I2C_SLV0_DO;
		ui32WriteBuffer[1] = AK8963_CNTL1_16_BIT | AK8963_CNTL1_M2_CONT_100HZ; // Enable continuous measurement (100Hz) in 16bit output
		//ui32WriteBuffer[1] = AK8963_CNTL1_M1_CONT_8HZ; // Enable continuous measurement (8Hz) in 14bit output
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Delay between SPI Operations
		SysCtlDelay(SPI_DELAY);

		// Enable I2C Transmission - Send 1 Byte
		ui32WriteBuffer[0] = MPU9250_O_I2C_SLV0_CTRL;
		ui32WriteBuffer[1] = MPU9250_I2C_SLV0_CTRL_EN_T1; // Highest bit enables the I2C and the lowest 4 bits set the transfer length
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);
		// Delay between SPI Operations
		SysCtlDelay(I2C_DELAY);

		/////////////////////////////////////////////////////////////////////////////

		// Setup SLV0 to read the AK8963 values at the desired rate /////////////////

		// Set the I2C slave addres of AK8963 and configure for read
		ui32WriteBuffer[0] = MPU9250_O_I2C_SLV0_ADDR;
		ui32WriteBuffer[1] = AK8963_ADDRESS | 0x80;
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Delay between SPI Operations
		SysCtlDelay(SPI_DELAY);

		// Write to I2C slave 0 the address from where to begin data transfer (read or write)
		ui32WriteBuffer[0] = MPU9250_O_I2C_SLV0_REG;
		ui32WriteBuffer[1] = AK8963_ST1;	// First register in the sensor output bank
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Delay between SPI Operations
		SysCtlDelay(SPI_DELAY);

		// Enable I2C Transmission - Receive 8 Bytes - ST2 must be read to indicate to AK8963 that a complete dataset has been read and that data protection can be disabled
		ui32WriteBuffer[0] = MPU9250_O_I2C_SLV0_CTRL;
		ui32WriteBuffer[1] = MPU9250_I2C_SLV0_CTRL_EN_T8; // Highest bit enables the I2C and the lowest 4 bits set the transfer length
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Delay between SPI Operations
		SysCtlDelay(I2C_DELAY);

		// Delay between SPI Operations
		SysCtlDelay(I2C_DELAY*20);

		/////////////////////////////////////////////////////////////////////////////

		// Interrupts Setup /////////////////////////////////////////////////////////

		// Config Interrupt pin - INT pin held high until a read operation is performed. Additionally INT pin is configured active high and push-pull
		ui32WriteBuffer[0] = MPU9250_O_INT_PIN_CFG;
		ui32WriteBuffer[1] = MPU9250_INT_PIN_CFG_LATCH_INT_EN | MPU9250_INT_PIN_CFG_INT_ANYRD_2CLEAR;
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Delay between SPI Operations
		SysCtlDelay(SPI_DELAY);

		// Config Interrupt pin - INT is triggered when new Data is Ready
		ui32WriteBuffer[0] = MPU9250_O_INT_ENABLE;
		ui32WriteBuffer[1] = MPU9250_INT_ENABLE_RAW_RDY_EN;
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Delay between SPI Operations
		SysCtlDelay(SPI_DELAY);

		/////////////////////////////////////////////////////////////////////////////

		// FIFO Setup ///////////////////////////////////////////////////////////////

		// Set sample rate divider for FIFO Sample Rate -> 100 Hz when Gyro and Accel are updated at 1 kHz
		ui32WriteBuffer[0] = MPU9250_O_SMPLRT_DIV;
		ui32WriteBuffer[1] = 0x09;
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Delay between SPI Operations
		SysCtlDelay(SPI_DELAY);

		// Copy external sensor data to FIFO once all data has been updated
		ui32WriteBuffer[0] = MPU9250_O_I2C_MST_DELAY_CTRL;
		ui32WriteBuffer[1] = MPU9250_I2C_MST_DELAY_CTRL_DELAY_ES_SHADOW;
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Delay between SPI Operations
		SysCtlDelay(SPI_DELAY);

		// Setup the Data to be copied to FIFO
		ui32WriteBuffer[0] = MPU9250_O_FIFO_EN;
		ui32WriteBuffer[1] = MPU9250_FIFO_EN_TEMP_PUT | MPU9250_FIFO_EN_GYRO_XOUT | MPU9250_FIFO_EN_GYRO_YOUT | MPU9250_FIFO_EN_GYRO_ZOUT | MPU9250_FIFO_EN_ACCEL | MPU9250_FIFO_EN_SLV0;
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Delay between SPI Operations
		SysCtlDelay(SPI_DELAY);

		// Enable FIFO and Maintain I2C Master -> On and I2C Slave -> Off
		ui32WriteBuffer[0] = MPU9250_O_USER_CTRL;
		ui32WriteBuffer[1] = MPU9250_USER_CTRL_I2C_MST_EN | MPU9250_USER_CTRL_FIFO_EN; //| MPU9250_USER_CTRL_I2C_IF_DIS;
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Delay between SPI Operations
		SysCtlDelay(SPI_DELAY);

		/////////////////////////////////////////////////////////////////////////////
	}
	else
	{
#ifdef DEBUG_CB
		// Return error! The Device ID is not 0x71!
		UARTprintf("\nError: The Device ID of sensor MPU9250 is not 0x71\n");
#endif
	}
}

//*****************************************************************************
//		MPU9250_Update9Axis
//*****************************************************************************

void MPU9250_Update9Axis()
{
	// Arrays to hold the data to be written or read via SPI
	uint32_t ui32WriteBuffer[22] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	uint32_t ui32ReadBuffer[22] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	// Variable to store FIFO Counter
	uint32_t ui32FifoCounter = 0;

	// Variable to store Element Counter
	uint8_t ui8ElementCounter = 0;

	// Pointer for memcpy operation
	uint8_t * pu8DestinationPointer = NULL;

	// Word counter
	// uint8_t ui8WordCounter = 0;

	// Swap buffer identifier
	if(0 == MPU9250_BufferVar)
	{
		MPU9250_BufferVar = 1;
	}
	else
	{
		MPU9250_BufferVar = 0;
	}

	// Read the FIFO Counter
	ui32FifoCounter = MPU9250_ReadFIFOCount();

	// Check if the amount of Bytes was the expected amount
	if(MPU9250_EXP_FIFO_COUNT ==  ui32FifoCounter)
	{
		//
		// Read MPU9-9250 FIFO Values
		//

		// Send command to read 7 bytes of sensor data from MPU-9250 - Bytes 1 to 7
		ui32WriteBuffer[0] = MPU9250_O_FIFO_R_W | READ_FLAG;
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, SPI_FIFO_LENGTH);

		// Read Data from RX FIFO
		if ( false == SPI_ReadFIFO(&stMPU9250_Handle, ui32ReadBuffer, SPI_FIFO_LENGTH) )
		{
#ifdef DEBUG_CB
			// Send error message
			UARTprintf("\nError: FIFO doesn't contain the amount of Bytes expected\n");
#endif
		}
		else
		{
			// Copy data to internal buffer
			pu8DestinationPointer = UTILS_Memcpy( (uint8_t *)&stMPU9250_Values[MPU9250_BufferVar].ACCEL, &ui32ReadBuffer[1], SPI_FIFO_DATA_BYTES);

			// Increase counter that indicates amount of bytes read
			ui8ElementCounter = ui8ElementCounter + SPI_FIFO_DATA_BYTES;
		}

		// Delay between SPI Operations
		SysCtlDelay(SPI_DELAY);

		// Send command to read 7 bytes of sensor data from MPU-9250 - Bytes 8 to 14
		ui32WriteBuffer[0] = MPU9250_O_FIFO_R_W | READ_FLAG;
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, SPI_FIFO_LENGTH);

		// Read Data from RX FIFO
		if ( false == SPI_ReadFIFO(&stMPU9250_Handle, ui32ReadBuffer, SPI_FIFO_LENGTH) )
		{
#ifdef DEBUG_CB
			// Send error message
			UARTprintf("\nError: FIFO doesn't contain the amount of Bytes expected\n");
#endif
		}
		else
		{
			// Copy data to internal buffer
			pu8DestinationPointer = UTILS_Memcpy(pu8DestinationPointer, &ui32ReadBuffer[1], SPI_FIFO_DATA_BYTES);

			// Increase counter that indicates amount of bytes read
			ui8ElementCounter = ui8ElementCounter + SPI_FIFO_DATA_BYTES;
		}

		// Delay between SPI Operations
		SysCtlDelay(SPI_DELAY);

		// Send command to read 7 bytes of sensor data from MPU-9250 - Bytes 15 to 21
		ui32WriteBuffer[0] = MPU9250_O_FIFO_R_W | READ_FLAG;
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, SPI_FIFO_LENGTH);

		// Read Data from RX FIFO
		if ( false == SPI_ReadFIFO(&stMPU9250_Handle, ui32ReadBuffer, SPI_FIFO_LENGTH) )
		{
#ifdef DEBUG_CB
			// Send error message
			UARTprintf("\nError: FIFO doesn't contain the amount of Bytes expected\n");
#endif
		}
		else
		{
			// Copy data to internal buffer
			pu8DestinationPointer = UTILS_Memcpy(pu8DestinationPointer, &ui32ReadBuffer[1], SPI_FIFO_DATA_BYTES);

			// Increase counter that indicates amount of bytes read
			ui8ElementCounter = ui8ElementCounter + SPI_FIFO_DATA_BYTES;
		}

		// Delay between SPI Operations
		SysCtlDelay(SPI_DELAY);

		// Send command to read 1 byte of sensor data from MPU-9250 - Byte 22
		ui32WriteBuffer[0] = MPU9250_O_FIFO_R_W | READ_FLAG;
		SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

		// Read Data from RX FIFO
		if ( false == SPI_ReadFIFO(&stMPU9250_Handle, ui32ReadBuffer, 2) )
		{
#ifdef DEBUG_CB
			// Send error message
			UARTprintf("\nError: FIFO doesn't contain the amount of Bytes expected\n");
#endif
		}
		else
		{
			// Copy data to internal buffer
			pu8DestinationPointer = UTILS_Memcpy(pu8DestinationPointer, &ui32ReadBuffer[1], 1);

			// Increase counter that indicates amount of bytes read
			ui8ElementCounter = ui8ElementCounter + SPI_FIFO_DATA_BYTES;
		}

		// Delay between SPI Operations
		SysCtlDelay(SPI_DELAY);

		// Postprocessing of the data

		// Swap bytes of the received values
		_bswap_constant_uint16(stMPU9250_Values[MPU9250_BufferVar].ACCEL.x);
		_bswap_constant_uint16(stMPU9250_Values[MPU9250_BufferVar].ACCEL.y);
		_bswap_constant_uint16(stMPU9250_Values[MPU9250_BufferVar].ACCEL.z);
		_bswap_constant_uint16(stMPU9250_Values[MPU9250_BufferVar].TEMP_MPU);
		_bswap_constant_uint16(stMPU9250_Values[MPU9250_BufferVar].GYRO.x);
		_bswap_constant_uint16(stMPU9250_Values[MPU9250_BufferVar].GYRO.y);
		_bswap_constant_uint16(stMPU9250_Values[MPU9250_BufferVar].GYRO.z);
		_bswap_constant_uint16(stMPU9250_Values[MPU9250_BufferVar].MAG.x);
		_bswap_constant_uint16(stMPU9250_Values[MPU9250_BufferVar].MAG.y);
		_bswap_constant_uint16(stMPU9250_Values[MPU9250_BufferVar].MAG.z);
		_bswap_constant_uint16(stMPU9250_Values[MPU9250_BufferVar].TEMP_AK);

	}
	else if (0 ==  ui32FifoCounter)
	{
		// Do nothing
#ifdef DEBUG_CB
		UARTprintf("\nE: FIFO Empty\n");
#endif
	}
	else
	{
#ifdef DEBUG_CB
			// Send error message
			UARTprintf("\nE: %d\n", ui32FifoCounter);
#endif

			while(ui32FifoCounter > 10)
			{
				// Send command to read 22 bytes of sensor data from MPU-9250
				ui32WriteBuffer[0] = MPU9250_O_FIFO_R_W | READ_FLAG;
				SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 12);

				// Reduce FIFO counter
				ui32FifoCounter -= 11;
			}

			// Send command to read the remaining bytes of sensor data from MPU-9250
			ui32WriteBuffer[0] = MPU9250_O_FIFO_R_W | READ_FLAG;
			SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, ui32FifoCounter + 1);

			// Clear FIFO
			SPI_ClearFIFO(&stMPU9250_Handle, ui32ReadBuffer[0]);
	}
}

//*****************************************************************************
//		MPU9250_ReadFIFOCount
//*****************************************************************************

uint32_t MPU9250_ReadFIFOCount()
{
	// Arrays to hold the data to be written or read via SPI
	uint32_t ui32WriteBuffer[2] = {0,0};
	uint32_t ui32ReadBuffer[2] = {0,0};

	// Variable to store FIFO Counter
	uint32_t ui32FifoCounter = 0;

	// Clear FIFO
	SPI_ClearFIFO(&stMPU9250_Handle, ui32ReadBuffer[0]);

	//
	// Read MPU9-9250 FIFO Counter
	//

	// Send command to read FIFO Count Registers (MSB - Start with FIFO_COUNTH to latch the counter)
	ui32WriteBuffer[0] = MPU9250_O_FIFO_COUNTH | READ_FLAG;
	SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

	// Read Data from RX FIFO
	if ( false == SPI_ReadFIFO(&stMPU9250_Handle, ui32ReadBuffer, 2) )
	{
#ifdef DEBUG_CB
		// Send error message
		UARTprintf("\nError: FIFO doesn't contain the amount of Bytes expected\n");
#endif
	}
	else
	{
		ui32FifoCounter = ui32ReadBuffer[1] << 8;
	}

	// Delay between SPI Operations
	SysCtlDelay(SPI_DELAY);

	// Send command to read FIFO Count Register (LSB)
	ui32WriteBuffer[0] = MPU9250_O_FIFO_COUNTL | READ_FLAG;
	SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

	// Read Data from RX FIFO
	if ( false == SPI_ReadFIFO(&stMPU9250_Handle, ui32ReadBuffer, 2) )
	{
#ifdef DEBUG_CB
		// Send error message
		UARTprintf("\nError: FIFO doesn't contain the amount of Bytes expected\n");
#endif
	}
	else
	{
		ui32FifoCounter |= ui32ReadBuffer[1];
	}

	// Delay between SPI Operations
	SysCtlDelay(SPI_DELAY);

	// Return the FIFO Counter value
	return ui32FifoCounter;
}

/*
// *****************************************************************************
//		MPU9250_UpdateAccel
// *****************************************************************************

void MPU9250_UpdateAccel()
{
	// Arrays to hold the data to be written or read via SPI
	uint32_t ui32WriteBuffer[2] = {0,0};
	uint32_t ui32ReadBuffer[2] = {0,0};

	// Clear FIFO
	SPI_ClearFIFO(&stMPU9250_Handle, ui32ReadBuffer[0]);

	//
	// Update ACCEL X
	//

	// Send command to read ACCEL X MSB
	ui32WriteBuffer[0] = MPU9250_O_ACCEL_XOUT_H | READ_FLAG;
	SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

	// Read Data from RX FIFO
	if ( false == SPI_ReadFIFO(&stMPU9250_Handle, ui32ReadBuffer, 2) )
	{
#ifdef DEBUG_CB
		// Send error message
		UARTprintf("\nError: FIFO doesn't contain the amount of Bytes expected\n");
#endif
	}
	else
	{
		stMPU9250_Values[MPU9250_BufferVar].ACCEL.x = ui32ReadBuffer[1] << 8;
	}

	// Delay between SPI Operations
	SysCtlDelay(SPI_DELAY);

	// Send command to read ACCEL X LSB
	ui32WriteBuffer[0] = MPU9250_O_ACCEL_XOUT_L | READ_FLAG;
	SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

	// Read Data from RX FIFO
	if ( false == SPI_ReadFIFO(&stMPU9250_Handle, ui32ReadBuffer, 2) )
	{
#ifdef DEBUG_CB
		// Send error message
		UARTprintf("\nError: FIFO doesn't contain the amount of Bytes expected\n");
#endif
	}
	else
	{
		stMPU9250_Values[MPU9250_BufferVar].ACCEL.x |= ui32ReadBuffer[1];
	}

	// Delay between SPI Operations
	SysCtlDelay(SPI_DELAY);

	//
	// Update ACCEL Y
	//

	// Send command to read ACCEL Y MSB
	ui32WriteBuffer[0] = MPU9250_O_ACCEL_YOUT_H | READ_FLAG;
	SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

	// Read Data from RX FIFO
	if ( false == SPI_ReadFIFO(&stMPU9250_Handle, ui32ReadBuffer, 2) )
	{
#ifdef DEBUG_CB
		// Send error message
		UARTprintf("\nError: FIFO doesn't contain the amount of Bytes expected\n");
#endif
	}
	else
	{
		stMPU9250_Values[MPU9250_BufferVar].ACCEL.y = ui32ReadBuffer[1] << 8;
	}

	// Delay between SPI Operations
	SysCtlDelay(SPI_DELAY);

	// Send command to read ACCEL Y LSB
	ui32WriteBuffer[0] = MPU9250_O_ACCEL_YOUT_L | READ_FLAG;
	SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

	// Read Data from RX FIFO
	if ( false == SPI_ReadFIFO(&stMPU9250_Handle, ui32ReadBuffer, 2) )
	{
#ifdef DEBUG_CB
		// Send error message
		UARTprintf("\nError: FIFO doesn't contain the amount of Bytes expected\n");
#endif
	}
	else
	{
		stMPU9250_Values[MPU9250_BufferVar].ACCEL.y |= ui32ReadBuffer[1];
	}

	// Delay between SPI Operations
	SysCtlDelay(SPI_DELAY);

	//
	// Update ACCEL Z
	//

	// Send command to read ACCEL Z MSB
	ui32WriteBuffer[0] = MPU9250_O_ACCEL_ZOUT_H | READ_FLAG;
	SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

	// Read Data from RX FIFO
	if ( false == SPI_ReadFIFO(&stMPU9250_Handle, ui32ReadBuffer, 2) )
	{
#ifdef DEBUG_CB
		// Send error message
		UARTprintf("\nError: FIFO doesn't contain the amount of Bytes expected\n");
#endif
	}
	else
	{
		stMPU9250_Values[MPU9250_BufferVar].ACCEL.z = ui32ReadBuffer[1] << 8;
	}

	// Delay between SPI Operations
	SysCtlDelay(SPI_DELAY);

	// Send command to read ACCEL Z LSB
	ui32WriteBuffer[0] = MPU9250_O_ACCEL_ZOUT_L | READ_FLAG;
	SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

	// Read Data from RX FIFO
	if ( false == SPI_ReadFIFO(&stMPU9250_Handle, ui32ReadBuffer, 2) )
	{
#ifdef DEBUG_CB
		// Send error message
		UARTprintf("\nError: FIFO doesn't contain the amount of Bytes expected\n");
#endif
	}
	else
	{
		stMPU9250_Values[MPU9250_BufferVar].ACCEL.z |= ui32ReadBuffer[1];
	}

	// Delay between SPI Operations
	SysCtlDelay(SPI_DELAY);
}

// *****************************************************************************
//		MPU9250_UpdateGyro
// *****************************************************************************

void MPU9250_UpdateGyro()
{
	// Arrays to hold the data to be written or read via SPI
	uint32_t ui32WriteBuffer[2] = {0,0};
	uint32_t ui32ReadBuffer[2] = {0,0};

	// Clear FIFO
	SPI_ClearFIFO(&stMPU9250_Handle, ui32ReadBuffer[0]);

	//
	// Update GYRO X
	//

	// Send command to read GYRO X MSB
	ui32WriteBuffer[0] = MPU9250_O_GYRO_XOUT_H | READ_FLAG;
	SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

	// Read Data from RX FIFO
	if ( false == SPI_ReadFIFO(&stMPU9250_Handle, ui32ReadBuffer, 2) )
	{
#ifdef DEBUG_CB
		// Send error message
		UARTprintf("\nError: FIFO doesn't contain the amount of Bytes expected\n");
#endif
	}
	else
	{
		stMPU9250_Values[MPU9250_BufferVar].GYRO.x = ui32ReadBuffer[1] << 8;
	}

	// Delay between SPI Operations
	SysCtlDelay(SPI_DELAY);

	// Send command to read GYRO X LSB
	ui32WriteBuffer[0] = MPU9250_O_GYRO_XOUT_L | READ_FLAG;
	SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

	// Read Data from RX FIFO
	if ( false == SPI_ReadFIFO(&stMPU9250_Handle, ui32ReadBuffer, 2) )
	{
#ifdef DEBUG_CB
		// Send error message
		UARTprintf("\nError: FIFO doesn't contain the amount of Bytes expected\n");
#endif
	}
	else
	{
		stMPU9250_Values[MPU9250_BufferVar].GYRO.x |= ui32ReadBuffer[1];
	}

	// Delay between SPI Operations
	SysCtlDelay(SPI_DELAY);

	//
	// Update GYRO Y
	//

	// Send command to read GYRO Y MSB
	ui32WriteBuffer[0] = MPU9250_O_GYRO_YOUT_H | READ_FLAG;
	SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

	// Read Data from RX FIFO
	if ( false == SPI_ReadFIFO(&stMPU9250_Handle, ui32ReadBuffer, 2) )
	{
#ifdef DEBUG_CB
		// Send error message
		UARTprintf("\nError: FIFO doesn't contain the amount of Bytes expected\n");
#endif
	}
	else
	{
		stMPU9250_Values[MPU9250_BufferVar].GYRO.y = ui32ReadBuffer[1] << 8;
	}

	// Delay between SPI Operations
	SysCtlDelay(SPI_DELAY);

	// Send command to read GYRO Y LSB
	ui32WriteBuffer[0] = MPU9250_O_GYRO_YOUT_L | READ_FLAG;
	SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

	// Read Data from RX FIFO
	if ( false == SPI_ReadFIFO(&stMPU9250_Handle, ui32ReadBuffer, 2) )
	{
#ifdef DEBUG_CB
		// Send error message
		UARTprintf("\nError: FIFO doesn't contain the amount of Bytes expected\n");
#endif
	}
	else
	{
		stMPU9250_Values[MPU9250_BufferVar].GYRO.y |= ui32ReadBuffer[1];
	}

	// Delay between SPI Operations
	SysCtlDelay(SPI_DELAY);


	//
	// Update GYRO Z
	//

	// Send command to read GYRO Z MSB
	ui32WriteBuffer[0] = MPU9250_O_GYRO_ZOUT_H | READ_FLAG;
	SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

	// Read Data from RX FIFO
	if ( false == SPI_ReadFIFO(&stMPU9250_Handle, ui32ReadBuffer, 2) )
	{
#ifdef DEBUG_CB
		// Send error message
		UARTprintf("\nError: FIFO doesn't contain the amount of Bytes expected\n");
#endif
	}
	else
	{
		stMPU9250_Values[MPU9250_BufferVar].GYRO.z = ui32ReadBuffer[1] << 8;
	}

	// Delay between SPI Operations
	SysCtlDelay(SPI_DELAY);

	// Send command to read GYRO Z LSB
	ui32WriteBuffer[0] = MPU9250_O_GYRO_ZOUT_L | READ_FLAG;
	SPI_Write(&stMPU9250_Handle, ui32WriteBuffer, 2);

	// Read Data from RX FIFO
	if ( false == SPI_ReadFIFO(&stMPU9250_Handle, ui32ReadBuffer, 2) )
	{
#ifdef DEBUG_CB
		// Send error message
		UARTprintf("\nError: FIFO doesn't contain the amount of Bytes expected\n");
#endif
	}
	else
	{
		stMPU9250_Values[MPU9250_BufferVar].GYRO.z |= ui32ReadBuffer[1];
	}

	// Delay between SPI Operations
	SysCtlDelay(SPI_DELAY);
}*/

