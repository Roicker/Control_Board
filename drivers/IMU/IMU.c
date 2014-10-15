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

#include "drivers/IMU/IMU.h"

//*****************************************************************************
//
//		Global Variables
//
//*****************************************************************************

extern struct MS5611 stMS5611_Values;
extern struct MPU9250 stMPU9250_Values;

//*****************************************************************************
//
//		Functions
//
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

	// Initialize MPU9250
	MPU9250_Init();

	// Initialize MS5611
	MS5611_Init();
}

void IMU_UpdateValues()
{
	MPU9250_UpdateAccel();
	MPU9250_UpdateGyro();
	MS5611_UpdatePressure();
	MS5611_UpdateTemp();
#ifdef DEBUG_CB
	// Print IMU Variable Values

	// MPU9250 Values
	UARTprintf("\n MPU9250 \n ACCEL - X: %d  Y: %d  Z: %d", stMPU9250_Values.ACCEL.x, stMPU9250_Values.ACCEL.y, stMPU9250_Values.ACCEL.z);
	UARTprintf("\n GYRO  - X: %d  Y: %d  Z: %d", stMPU9250_Values.GYRO.x, stMPU9250_Values.GYRO.y, stMPU9250_Values.GYRO.z);
	UARTprintf("\n MAG   - X: %d  Y: %d  Z: %d", stMPU9250_Values.MAG.x, stMPU9250_Values.MAG.y, stMPU9250_Values.MAG.z);

	// MS5611 Values
	UARTprintf("\n MS5611 \n Pressure: %d  Temperature: %d\n", stMS5611_Values.Pressure, stMS5611_Values.Temperature);
#endif
}
