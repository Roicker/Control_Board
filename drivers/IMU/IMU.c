//*****************************************************************************
//
// IMU.c - Inertial Measurement Unit Driver
//
// Copyright (c) 2014 Raymundo Magaña Gomez - http://raymundomagana.webs.com/
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

// Counters for calibration cycles
uint8_t IMU_u8AccelGyro_Counter = 0;
uint16_t IMU_u16Mag_Counter = 0;

// Variables for Madgwick's Algoritm
volatile float beta = IMU_BETA_DEF;								// 2 * Proportional gain (Kp)
volatile float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;		// Quaternion of sensor frame relative to auxiliary frame
volatile float fex = 0, fey = 0, fez = 0;						// Float representation of euler angles
volatile int16_t ex = 0, ey = 0, ez = 0;						// Integer representation of euler angles

// Select invSqrt implementation
int instability_fix = 1;

//*****************************************************************************
//
//		Extern Variables
//
//*****************************************************************************

// Structures to hold sensor data
extern struct MS5611 stMS5611_Values;
extern struct MPU9250 stMPU9250_Values[2];
extern struct MPU9250_BIAS stMPU9250_Bias;

// Calibration flags
extern bool bAccGyro_Calibrated;
extern bool bMag_Calibrated;

// Variable to store which stMPU9250_Values is currently head
extern uint8_t ui8MPU9250_RBHead;

// Variable to store which stMPU9250_Values is currently tail
extern uint8_t ui8MPU9250_RBTail;

// Variable to store how many stMPU9250_Values entries are filled
extern uint8_t ui8MPU9250_RBCount;

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
#ifdef DEBUG_CB
	// Set Debug pin 1 to measure excecution time
	GPIOPinWrite(DEBUG_PORT, DEBUG_PIN_1, DEBUG_PIN_1);
#endif

	// Variable to store interrupt status
	uint32_t ui32Status;

	// Get interrupt status
	ui32Status = GPIOIntStatus(IMU_INTPIN_PORT, true);

	// Clear the asserted interrupts
	GPIOIntClear(IMU_INTPIN_PORT, ui32Status);

	// Read MPU-9250 FIFO
	MPU9250_Update9Axis();

#ifdef DEBUG_CB
	// Clear Debug pin 1 to measure excecution time
	GPIOPinWrite(DEBUG_PORT, DEBUG_PIN_1, 0);
#endif
}

//*****************************************************************************
//
//		Functions
//
//*****************************************************************************

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
	// MPU9250_Update9Axis is called in an Interrupt context
	MS5611_UpdatePressure();
	MS5611_UpdateTemp();

#ifdef DEBUG_CB
	// Print IMU Variable Values

	// MPU9250 Values
	UARTprintf("\n\n MPU9250 \n ACCEL - X: %d  Y: %d  Z: %d", stMPU9250_Values[ui8MPU9250_RBHead].ACCEL_RAW.x, stMPU9250_Values[ui8MPU9250_RBHead].ACCEL_RAW.y, stMPU9250_Values[ui8MPU9250_RBHead].ACCEL_RAW.z);
	UARTprintf("\n GYRO  - X: %d  Y: %d  Z: %d", stMPU9250_Values[ui8MPU9250_RBHead].GYRO_RAW.x, stMPU9250_Values[ui8MPU9250_RBHead].GYRO_RAW.y, stMPU9250_Values[ui8MPU9250_RBHead].GYRO_RAW.z);
	UARTprintf("\n MAG   - X: %d  Y: %d  Z: %d", stMPU9250_Values[ui8MPU9250_RBHead].MAG_RAW.x, stMPU9250_Values[ui8MPU9250_RBHead].MAG_RAW.y, stMPU9250_Values[ui8MPU9250_RBHead].MAG_RAW.z);

	// MS5611 Values
	UARTprintf("\n\n MS5611 \n Pressure: %d  Temperature: %d\n", stMS5611_Values.Pressure, stMS5611_Values.Temperature);
#endif
}

//*****************************************************************************
//		IMU_ProcessValues
//*****************************************************************************

void IMU_ProcessValues()
{
	// Convert raw data to real values

	// Accelerometer
	stMPU9250_Values[ui8MPU9250_RBTail].ACCEL.x = (float) (stMPU9250_Values[ui8MPU9250_RBTail].ACCEL_RAW.y * MPU9250_ACC_SCALE);
	stMPU9250_Values[ui8MPU9250_RBTail].ACCEL.y = - ((float) (stMPU9250_Values[ui8MPU9250_RBTail].ACCEL_RAW.x * MPU9250_ACC_SCALE));
	stMPU9250_Values[ui8MPU9250_RBTail].ACCEL.z = (float) (stMPU9250_Values[ui8MPU9250_RBTail].ACCEL_RAW.z * MPU9250_ACC_SCALE);

	// Gyroscope
	stMPU9250_Values[ui8MPU9250_RBTail].GYRO.x = (float) (stMPU9250_Values[ui8MPU9250_RBTail].GYRO_RAW.y * MPU9250_GYRO_SCALE);
	stMPU9250_Values[ui8MPU9250_RBTail].GYRO.y = - ((float) (stMPU9250_Values[ui8MPU9250_RBTail].GYRO_RAW.x * MPU9250_GYRO_SCALE));
	stMPU9250_Values[ui8MPU9250_RBTail].GYRO.z = (float) (stMPU9250_Values[ui8MPU9250_RBTail].GYRO_RAW.z * MPU9250_GYRO_SCALE);

	// Magnetometer // Invert Z for correct orientation
	stMPU9250_Values[ui8MPU9250_RBTail].MAG.x = (float) (stMPU9250_Values[ui8MPU9250_RBTail].MAG_RAW.x * MPU9250_MAG_SCALE);
	stMPU9250_Values[ui8MPU9250_RBTail].MAG.y = - ((float) (stMPU9250_Values[ui8MPU9250_RBTail].MAG_RAW.y * MPU9250_MAG_SCALE));
	stMPU9250_Values[ui8MPU9250_RBTail].MAG.z = - ((float) (stMPU9250_Values[ui8MPU9250_RBTail].MAG_RAW.z * MPU9250_MAG_SCALE));

	// Compensate Magnetometer values with the sensitivity data
	stMPU9250_Values[ui8MPU9250_RBTail].MAG.x *= (float) (stMPU9250_Bias.MAG_SENS.x);
	stMPU9250_Values[ui8MPU9250_RBTail].MAG.y *= (float) (stMPU9250_Bias.MAG_SENS.y);
	stMPU9250_Values[ui8MPU9250_RBTail].MAG.z *= (float) (stMPU9250_Bias.MAG_SENS.z);

	// Update Quaternion values
	IMU_MadgwickAHRSupdate(stMPU9250_Values[ui8MPU9250_RBTail].GYRO.x, stMPU9250_Values[ui8MPU9250_RBTail].GYRO.y, stMPU9250_Values[ui8MPU9250_RBTail].GYRO.z, stMPU9250_Values[ui8MPU9250_RBTail].ACCEL.x, stMPU9250_Values[ui8MPU9250_RBTail].ACCEL.y, stMPU9250_Values[ui8MPU9250_RBTail].ACCEL.z, stMPU9250_Values[ui8MPU9250_RBTail].MAG.x, stMPU9250_Values[ui8MPU9250_RBTail].MAG.y, stMPU9250_Values[ui8MPU9250_RBTail].MAG.z);

	// Calculate Euler angles
	IMU_ComputeAngles();

	// Convert to Degrees
	fex *= 57.295645;
	fey *= 57.295645;
	fez *= 57.295645;

	// Convert to integers to send
	ex = lrintf(fex);
	ey = lrintf(fey);
	ez = lrintf(fez);

/*	// Check if system should be calibrated
	if(!bAccGyro_Calibrated)
	{
		// Call calibration function for Accel and Gyro
		IMU_CalibrateGyroAccel();
	}
	else
	{
		// Correct bias

		// Accelerometer
		stMPU9250_Values[ui8MPU9250_RBTail].ACCEL.x = stMPU9250_Values[ui8MPU9250_RBTail].ACCEL.x - stMPU9250_Bias.ACCEL_BIAS.x;
		stMPU9250_Values[ui8MPU9250_RBTail].ACCEL.y = stMPU9250_Values[ui8MPU9250_RBTail].ACCEL.y - stMPU9250_Bias.ACCEL_BIAS.y;
		stMPU9250_Values[ui8MPU9250_RBTail].ACCEL.z = stMPU9250_Values[ui8MPU9250_RBTail].ACCEL.z - stMPU9250_Bias.ACCEL_BIAS.z;

		// Gyroscope
		stMPU9250_Values[ui8MPU9250_RBTail].GYRO.x = stMPU9250_Values[ui8MPU9250_RBTail].GYRO.x - stMPU9250_Bias.GYRO_BIAS.x;
		stMPU9250_Values[ui8MPU9250_RBTail].GYRO.y = stMPU9250_Values[ui8MPU9250_RBTail].GYRO.y - stMPU9250_Bias.GYRO_BIAS.z;
		stMPU9250_Values[ui8MPU9250_RBTail].GYRO.z = stMPU9250_Values[ui8MPU9250_RBTail].GYRO.z - stMPU9250_Bias.GYRO_BIAS.z;
	}

	if(!bMag_Calibrated)
	{
		// Call calibration function for Mag
		IMU_CalibrateMag();
	}
	else
	{
		// What to do here?
	}*/
}

//*****************************************************************************
//		IMU_CalibrateGyroAccel
//*****************************************************************************

void IMU_CalibrateGyroAccel()
{
	if(IMU_u8AccelGyro_Counter < IMU_ACC_GYRO_CAL_CYCLES)
	{
		// Create sum of all read Accel values
		stMPU9250_Bias.ACCEL_BIAS.x += (float) stMPU9250_Values[ui8MPU9250_RBTail].ACCEL.x;
		stMPU9250_Bias.ACCEL_BIAS.y += (float) stMPU9250_Values[ui8MPU9250_RBTail].ACCEL.y;
		stMPU9250_Bias.ACCEL_BIAS.z += (float) stMPU9250_Values[ui8MPU9250_RBTail].ACCEL.z;

		// Create sum of all read Gyro values
		stMPU9250_Bias.GYRO_BIAS.x += (float) stMPU9250_Values[ui8MPU9250_RBTail].GYRO.x;
		stMPU9250_Bias.GYRO_BIAS.y += (float) stMPU9250_Values[ui8MPU9250_RBTail].GYRO.y;
		stMPU9250_Bias.GYRO_BIAS.z += (float) stMPU9250_Values[ui8MPU9250_RBTail].GYRO.z;

		// Increase counter
		IMU_u8AccelGyro_Counter++;
	}
	else
	{
		// Reset calibration counter
		IMU_u8AccelGyro_Counter = 0;

		// Signal that the Accelerometer and Gyro have been calibrated
		bAccGyro_Calibrated = true;

		// Normalize sums to get average bias
		stMPU9250_Bias.ACCEL_BIAS.x /= (float) IMU_ACC_GYRO_CAL_CYCLES;
		stMPU9250_Bias.ACCEL_BIAS.z /= (float) IMU_ACC_GYRO_CAL_CYCLES;
		stMPU9250_Bias.ACCEL_BIAS.y /= (float) IMU_ACC_GYRO_CAL_CYCLES;

		// Normalize sums to get average bias
		stMPU9250_Bias.GYRO_BIAS.x /= (float) IMU_ACC_GYRO_CAL_CYCLES;
		stMPU9250_Bias.GYRO_BIAS.y /= (float) IMU_ACC_GYRO_CAL_CYCLES;
		stMPU9250_Bias.GYRO_BIAS.z /= (float) IMU_ACC_GYRO_CAL_CYCLES;

#ifdef DEBUG_CB
		// Print message to signal that Gyro and Accel calibration is complete
		UARTprintf("\n\n Calibration of Accelerometer and Gyro is complete");
		UARTprintf("\n\nCommand: ");
#endif
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
		bMag_Calibrated = true;
	}
}

//*****************************************************************************
//		IMU_MadgwickAHRSupdate
// 		AHRS algorithm update
//*****************************************************************************

void IMU_MadgwickAHRSupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz)
{
	// Internal variables for Madgwick's algorithm
	float recipNorm;
	float s0, s1, s2, s3;
	float qDot1, qDot2, qDot3, qDot4;
	float hx, hy;
	float _2q0mx, _2q0my, _2q0mz, _2q1mx, _2bx, _2bz, _4bx, _4bz, _2q0, _2q1, _2q2, _2q3, _2q0q2, _2q2q3, q0q0, q0q1, q0q2, q0q3, q1q1, q1q2, q1q3, q2q2, q2q3, q3q3;

	// Use IMU algorithm if magnetometer measurement invalid (avoids NaN in magnetometer normalisation)
	if((mx == 0.0f) && (my == 0.0f) && (mz == 0.0f))
	{
		IMU_MadgwickAHRSupdateIMU(gx, gy, gz, ax, ay, az);
		return;
	}

	// Rate of change of quaternion from gyroscope
	qDot1 = 0.5f * (-q1 * gx - q2 * gy - q3 * gz);
	qDot2 = 0.5f * (q0 * gx + q2 * gz - q3 * gy);
	qDot3 = 0.5f * (q0 * gy - q1 * gz + q3 * gx);
	qDot4 = 0.5f * (q0 * gz + q1 * gy - q2 * gx);

	// Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
	if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f)))
	{
		// Normalise accelerometer measurement
		recipNorm = IMU_invSqrt(ax * ax + ay * ay + az * az);
		ax *= recipNorm;
		ay *= recipNorm;
		az *= recipNorm;

		// Normalise magnetometer measurement
		recipNorm = IMU_invSqrt(mx * mx + my * my + mz * mz);
		mx *= recipNorm;
		my *= recipNorm;
		mz *= recipNorm;

		// Auxiliary variables to avoid repeated arithmetic
		_2q0mx = 2.0f * q0 * mx;
		_2q0my = 2.0f * q0 * my;
		_2q0mz = 2.0f * q0 * mz;
		_2q1mx = 2.0f * q1 * mx;
		_2q0 = 2.0f * q0;
		_2q1 = 2.0f * q1;
		_2q2 = 2.0f * q2;
		_2q3 = 2.0f * q3;
		_2q0q2 = 2.0f * q0 * q2;
		_2q2q3 = 2.0f * q2 * q3;
		q0q0 = q0 * q0;
		q0q1 = q0 * q1;
		q0q2 = q0 * q2;
		q0q3 = q0 * q3;
		q1q1 = q1 * q1;
		q1q2 = q1 * q2;
		q1q3 = q1 * q3;
		q2q2 = q2 * q2;
		q2q3 = q2 * q3;
		q3q3 = q3 * q3;

		// Reference direction of Earth's magnetic field
		hx = mx * q0q0 - _2q0my * q3 + _2q0mz * q2 + mx * q1q1 + _2q1 * my * q2 + _2q1 * mz * q3 - mx * q2q2 - mx * q3q3;
		hy = _2q0mx * q3 + my * q0q0 - _2q0mz * q1 + _2q1mx * q2 - my * q1q1 + my * q2q2 + _2q2 * mz * q3 - my * q3q3;
		_2bx = sqrt(hx * hx + hy * hy);
		_2bz = -_2q0mx * q2 + _2q0my * q1 + mz * q0q0 + _2q1mx * q3 - mz * q1q1 + _2q2 * my * q3 - mz * q2q2 + mz * q3q3;
		_4bx = 2.0f * _2bx;
		_4bz = 2.0f * _2bz;

		// Gradient decent algorithm corrective step
		s0 = -_2q2 * (2.0f * q1q3 - _2q0q2 - ax) + _2q1 * (2.0f * q0q1 + _2q2q3 - ay) - _2bz * q2 * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (-_2bx * q3 + _2bz * q1) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + _2bx * q2 * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
		s1 = _2q3 * (2.0f * q1q3 - _2q0q2 - ax) + _2q0 * (2.0f * q0q1 + _2q2q3 - ay) - 4.0f * q1 * (1 - 2.0f * q1q1 - 2.0f * q2q2 - az) + _2bz * q3 * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (_2bx * q2 + _2bz * q0) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + (_2bx * q3 - _4bz * q1) * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
		s2 = -_2q0 * (2.0f * q1q3 - _2q0q2 - ax) + _2q3 * (2.0f * q0q1 + _2q2q3 - ay) - 4.0f * q2 * (1 - 2.0f * q1q1 - 2.0f * q2q2 - az) + (-_4bx * q2 - _2bz * q0) * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (_2bx * q1 + _2bz * q3) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + (_2bx * q0 - _4bz * q2) * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
		s3 = _2q1 * (2.0f * q1q3 - _2q0q2 - ax) + _2q2 * (2.0f * q0q1 + _2q2q3 - ay) + (-_4bx * q3 + _2bz * q1) * (_2bx * (0.5f - q2q2 - q3q3) + _2bz * (q1q3 - q0q2) - mx) + (-_2bx * q0 + _2bz * q2) * (_2bx * (q1q2 - q0q3) + _2bz * (q0q1 + q2q3) - my) + _2bx * q1 * (_2bx * (q0q2 + q1q3) + _2bz * (0.5f - q1q1 - q2q2) - mz);
		recipNorm = IMU_invSqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3); // normalise step magnitude
		s0 *= recipNorm;
		s1 *= recipNorm;
		s2 *= recipNorm;
		s3 *= recipNorm;

		// Apply feedback step
		qDot1 -= beta * s0;
		qDot2 -= beta * s1;
		qDot3 -= beta * s2;
		qDot4 -= beta * s3;
	}

	// Integrate rate of change of quaternion to yield quaternion
	q0 += qDot1 * (1.0f / IMU_SAMPLE_FREQ);
	q1 += qDot2 * (1.0f / IMU_SAMPLE_FREQ);
	q2 += qDot3 * (1.0f / IMU_SAMPLE_FREQ);
	q3 += qDot4 * (1.0f / IMU_SAMPLE_FREQ);

	// Normalise quaternion
	recipNorm = IMU_invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
	q0 *= recipNorm;
	q1 *= recipNorm;
	q2 *= recipNorm;
	q3 *= recipNorm;
}


//*****************************************************************************
//		IMU_MadgwickAHRSupdateIMU
// 		IMU algorithm update
//*****************************************************************************

void IMU_MadgwickAHRSupdateIMU(float gx, float gy, float gz, float ax, float ay, float az)
{
	// Internal variables for Madgwick's algorithm
	float recipNorm;
	float s0, s1, s2, s3;
	float qDot1, qDot2, qDot3, qDot4;
	float _2q0, _2q1, _2q2, _2q3, _4q0, _4q1, _4q2 ,_8q1, _8q2, q0q0, q1q1, q2q2, q3q3;

	// Rate of change of quaternion from gyroscope
	qDot1 = 0.5f * (-q1 * gx - q2 * gy - q3 * gz);
	qDot2 = 0.5f * (q0 * gx + q2 * gz - q3 * gy);
	qDot3 = 0.5f * (q0 * gy - q1 * gz + q3 * gx);
	qDot4 = 0.5f * (q0 * gz + q1 * gy - q2 * gx);

	// Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
	if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f)))
	{
		// Normalise accelerometer measurement
		recipNorm = IMU_invSqrt(ax * ax + ay * ay + az * az);
		ax *= recipNorm;
		ay *= recipNorm;
		az *= recipNorm;

		// Auxiliary variables to avoid repeated arithmetic
		_2q0 = 2.0f * q0;
		_2q1 = 2.0f * q1;
		_2q2 = 2.0f * q2;
		_2q3 = 2.0f * q3;
		_4q0 = 4.0f * q0;
		_4q1 = 4.0f * q1;
		_4q2 = 4.0f * q2;
		_8q1 = 8.0f * q1;
		_8q2 = 8.0f * q2;
		q0q0 = q0 * q0;
		q1q1 = q1 * q1;
		q2q2 = q2 * q2;
		q3q3 = q3 * q3;

		// Gradient decent algorithm corrective step
		s0 = _4q0 * q2q2 + _2q2 * ax + _4q0 * q1q1 - _2q1 * ay;
		s1 = _4q1 * q3q3 - _2q3 * ax + 4.0f * q0q0 * q1 - _2q0 * ay - _4q1 + _8q1 * q1q1 + _8q1 * q2q2 + _4q1 * az;
		s2 = 4.0f * q0q0 * q2 + _2q0 * ax + _4q2 * q3q3 - _2q3 * ay - _4q2 + _8q2 * q1q1 + _8q2 * q2q2 + _4q2 * az;
		s3 = 4.0f * q1q1 * q3 - _2q1 * ax + 4.0f * q2q2 * q3 - _2q2 * ay;
		recipNorm = IMU_invSqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3); // normalise step magnitude
		s0 *= recipNorm;
		s1 *= recipNorm;
		s2 *= recipNorm;
		s3 *= recipNorm;

		// Apply feedback step
		qDot1 -= beta * s0;
		qDot2 -= beta * s1;
		qDot3 -= beta * s2;
		qDot4 -= beta * s3;
	}

	// Integrate rate of change of quaternion to yield quaternion
	q0 += qDot1 * (1.0f / IMU_SAMPLE_FREQ);
	q1 += qDot2 * (1.0f / IMU_SAMPLE_FREQ);
	q2 += qDot3 * (1.0f / IMU_SAMPLE_FREQ);
	q3 += qDot4 * (1.0f / IMU_SAMPLE_FREQ);

	// Normalise quaternion
	recipNorm = IMU_invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
	q0 *= recipNorm;
	q1 *= recipNorm;
	q2 *= recipNorm;
	q3 *= recipNorm;
}

//*****************************************************************************
//		IMU_ComputeAngles
// 		Calculate Euler angles from quaternions
//*****************************************************************************

void IMU_ComputeAngles()
{
	fex = atan2f(q0*q1 + q2*q3, 0.5f - q1*q1 - q2*q2);
	fey = asinf(-2.0f * (q1*q3 - q0*q2));
	fez = atan2f(q1*q2 + q0*q3, 0.5f - q2*q2 - q3*q3);
}

//*****************************************************************************
//		IMU_invSqrt
// 		Fast inverse square-root
// 		See: http://en.wikipedia.org/wiki/Fast_inverse_square_root
//*****************************************************************************

float IMU_invSqrt(float x)
{
	if (instability_fix == 0)
	{
		/* original code */
		float halfx = 0.5f * x;
		float y = x;
		long i = *(long*)&y;
		i = 0x5f3759df - (i>>1);
		y = *(float*)&i;
		y = y * (1.5f - (halfx * y * y));
		return y;
	}
	else if (instability_fix == 1)
	{
		/* close-to-optimal  method with low cost from http://pizer.wordpress.com/2008/10/12/fast-inverse-square-root */
		unsigned int i = 0x5F1F1412 - (*(unsigned int*)&x >> 1);
		float tmp = *(float*)&i;
		return tmp * (1.69000231f - 0.714158168f * x * tmp * tmp);
	}
	else
	{
		/* optimal but expensive method: */
		return 1.0f / sqrtf(x);
	}
}

