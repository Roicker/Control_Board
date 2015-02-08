//*****************************************************************************
//
// IMU.c - Inertial Measurement Unit Driver
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

#include "drivers/IMU/IMU.h"

//*****************************************************************************
//
//		Global Variables
//
//*****************************************************************************

// Structures to hold sensor data
extern struct MS5611 stMS5611_Values;
extern struct MPU9250 stMPU9250_Values;

// Counters for calibration cycles
uint8_t IMU_u8AccelGyro_Counter = 0;
uint16_t IMU_u16Mag_Counter = 0;

//*****************************************************************************
//
//		Functions
//
//*****************************************************************************

//*****************************************************************************
//
//		Interrupt Handlers
//
//*****************************************************************************

//*****************************************************************************
//		IMU_IntHandler
//*****************************************************************************

void IMU_IntHandler()
{
	// Set Debug pin 1 to measure excecution time
	GPIOPinWrite(DEBUG_PORT, DEBUG_PIN_1, DEBUG_PIN_1);

	// Variable to store interrupt status
	uint32_t ui32Status;

	// Get interrupt status
	ui32Status = GPIOIntStatus(IMU_INTPIN_PORT, true);

	// Clear the asserted interrupts
	GPIOIntClear(IMU_INTPIN_PORT, ui32Status);

	// Read MPU-9250 FIFO
	MPU9250_Update9Axis();

	// Check if system should be calibrated
	if(!stMPU9250_Values.AccGyro_Calibrated)
	{
		// Call calibration function for Accel and Gyro
		IMU_CalibrateGyroAccel();
	}
	else if(!stMPU9250_Values.Mag_Calibrated)
	{
		// Call calibration function for Mag
		IMU_CalibrateMag();
	}


	// Set Debug pin 1 to measure excecution time
	GPIOPinWrite(DEBUG_PORT, DEBUG_PIN_1, 0);
}

//*****************************************************************************
//		IMU_Init
//*****************************************************************************

void IMU_Init()
{
	// Initialize SPI
	if( false == SPI_Init(IMU_SPI_MODULE) )
	{
#ifdef DEBUG_CB
		// Send error message
		UARTprintf("\nError: SPI Init failed!\n");
#endif
	}
	else
	{
		// Config INT pin for MPU-9250

		// Enable Peripheral
		SysCtlPeripheralEnable(IMU_INTPIN_PERIPH);

		// Setup GPIO
		GPIOPinTypeGPIOInput(IMU_INTPIN_PORT, IMU_INTPIN_PIN);
		GPIOPadConfigSet(IMU_INTPIN_PORT, IMU_INTPIN_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);

		// Enable Interrupts

		// Enable interrupts on high level
		GPIOIntTypeSet(IMU_INTPIN_PORT, (uint8_t)IMU_INTPIN_PIN , GPIO_RISING_EDGE);

		// Enable interrupts
		GPIOIntEnable(IMU_INTPIN_PORT, IMU_INTPIN_PIN);

		// Clear Interrupts
		GPIOIntClear(IMU_INTPIN_PORT, IMU_INTPIN_PIN);

		// Enable interrupts in the IMU Interrupt pin
		IntEnable(IMU_INTPIN_INT);

		// Set interrupt prio
		//IntPrioritySet(IMU_INTPIN_INT, 0x20);

		// End of INT pin for MPU-9250 config

		// Initialize MS5611
		MS5611_Init();

		// Initialize MPU9250
		MPU9250_Init();
	}
}

//*****************************************************************************
//		IMU_UpdateValues
//*****************************************************************************

void IMU_UpdateValues()
{
	//MPU9250_UpdateAccel();
	//MPU9250_UpdateGyro();
	//MPU9250_Update9Axis(); // Called in Interrupt Handler
	MS5611_UpdatePressure();
	MS5611_UpdateTemp();
#ifdef DEBUG_CB
	// Print IMU Variable Values

	// MPU9250 Values
	UARTprintf("\n\n MPU9250 \n ACCEL - X: %d  Y: %d  Z: %d", stMPU9250_Values.ACCEL.x, stMPU9250_Values.ACCEL.y, stMPU9250_Values.ACCEL.z);
	UARTprintf("\n GYRO  - X: %d  Y: %d  Z: %d", stMPU9250_Values.GYRO.x, stMPU9250_Values.GYRO.y, stMPU9250_Values.GYRO.z);
	UARTprintf("\n MAG   - X: %d  Y: %d  Z: %d", stMPU9250_Values.MAG.x, stMPU9250_Values.MAG.y, stMPU9250_Values.MAG.z);

	// MS5611 Values
	UARTprintf("\n MS5611 \n Pressure: %d  Temperature: %d\n", stMS5611_Values.Pressure, stMS5611_Values.Temperature);
#endif
}

//*****************************************************************************
//		IMU_CalibrateGyroAccel
//*****************************************************************************

void IMU_CalibrateGyroAccel()
{
	if(IMU_u8AccelGyro_Counter < IMU_ACC_GYRO_CAL_CYCLES)
	{
		// Create sum of all read Accel values
		stMPU9250_Values.ACCEL_BIAS.x += (uint32_t) stMPU9250_Values.ACCEL.x;
		stMPU9250_Values.ACCEL_BIAS.y += (uint32_t) stMPU9250_Values.ACCEL.y;
		stMPU9250_Values.ACCEL_BIAS.z += (uint32_t) stMPU9250_Values.ACCEL.z;

		// Create sum of all read Gyro values
		stMPU9250_Values.GYRO_BIAS.x += (uint32_t) stMPU9250_Values.GYRO.x;
		stMPU9250_Values.GYRO_BIAS.y += (uint32_t) stMPU9250_Values.GYRO.y;
		stMPU9250_Values.GYRO_BIAS.z += (uint32_t) stMPU9250_Values.GYRO.z;

		// Increase counter
		IMU_u8AccelGyro_Counter++;
	}
	else
	{
		// Reset calibration counter
		IMU_u8AccelGyro_Counter = 0;

		// Signal that the Accelerometer and Gyro have been calibrated
		stMPU9250_Values.AccGyro_Calibrated = true;

		// Normalize sums to get average bias
		stMPU9250_Values.ACCEL_BIAS.x /= (uint32_t) IMU_ACC_GYRO_CAL_CYCLES;
		stMPU9250_Values.ACCEL_BIAS.z /= (uint32_t) IMU_ACC_GYRO_CAL_CYCLES;
		stMPU9250_Values.ACCEL_BIAS.y /= (uint32_t) IMU_ACC_GYRO_CAL_CYCLES;

		// Normalize sums to get average bias
		stMPU9250_Values.GYRO_BIAS.x /= (uint32_t) IMU_ACC_GYRO_CAL_CYCLES;
		stMPU9250_Values.GYRO_BIAS.y /= (uint32_t) IMU_ACC_GYRO_CAL_CYCLES;
		stMPU9250_Values.GYRO_BIAS.z /= (uint32_t) IMU_ACC_GYRO_CAL_CYCLES;

		// Print message to signal that Gyro and Accel calibration is complete
		UARTprintf("\n\n Calibration of Accelerometer and Gyro is complete");
		UARTprintf("\n Accel Bias - X: %d  Y: %d  Z: %d", stMPU9250_Values.ACCEL_BIAS.x, stMPU9250_Values.ACCEL_BIAS.y, stMPU9250_Values.ACCEL_BIAS.z);
		UARTprintf("\n Gyro Bias - X: %d  Y: %d  Z: %d", stMPU9250_Values.GYRO_BIAS.x, stMPU9250_Values.GYRO_BIAS.y, stMPU9250_Values.GYRO_BIAS.z);
		UARTprintf("\n\nCommand: ");
	}
}
//*****************************************************************************
//		IMU_CalibrateMag
//*****************************************************************************

void IMU_CalibrateMag()
{
	if(IMU_u16Mag_Counter < IMU_MAG_CAL_CYCLES)
	{

	}
	else
	{
		// Reset calibration counter
		IMU_u16Mag_Counter = 0;

		// Signal that the Accelerometer and Gyro have been calibrated
		stMPU9250_Values.Mag_Calibrated = true;
	}
}
