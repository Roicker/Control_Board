/*
 * MPU9250.h
 *
 *  Created on: 09/10/2014
 *      Author: Ray
 */

#ifndef MPU9250_H_
#define MPU9250_H_

//*****************************************************************************
//
//		Local Header Files
//
//*****************************************************************************

#include "Config.h"

//*****************************************************************************
//
//		Definitions
//
//*****************************************************************************

// CS Line Config for MPU9250
#define CS_MPU9250_PERIPH			SYSCTL_PERIPH_GPIOE						// Intentify on which Peripheral is the CS line of the MPU9250
#define CS_MPU9250_PORT				GPIO_PORTE_BASE							// Intentify on which Port is the CS line of the MPU9250
#define CS_MPU9250_PIN				GPIO_PIN_3								// Intentify on which Pin is the CS line of the MPU9250

// Number of values to be read from MPU-9250 FIFO
#define MPU9250_EXP_FIFO_COUNT		22
#define	MPU9250_RX_FIFO_BYTES		22

// Size of stMPU9250_Values Ring Buffer
#define MPU9250_RB_SIZE				10

// Accelerometer scale
#define MPU9250_ACC_SCALE			MPU9250_ACC_SCALE_4G

// Possible Accelerometer scale values
#define MPU9250_ACC_SCALE_2G		1.0/16384.0
#define MPU9250_ACC_SCALE_4G		1.0/8192.0
#define MPU9250_ACC_SCALE_8G		1.0/4096.0
#define MPU9250_ACC_SCALE_16G		1.0/2048.0

// Gyroscope Scale
#define MPU9250_GYRO_SCALE			MPU9250_GYRO_SCALE_500_RAD

// Possible Gyroscope scale values
#define MPU9250_GYRO_SCALE_250_DEG		1 / 131.0
#define MPU9250_GYRO_SCALE_500_DEG		1 / 62.5
#define MPU9250_GYRO_SCALE_1000_DEG		1 / 32.8
#define MPU9250_GYRO_SCALE_2000_DEG		1 / 16.4
#define MPU9250_GYRO_SCALE_250_RAD		M_PI / (131.0 * 180.0)
#define MPU9250_GYRO_SCALE_500_RAD		M_PI / (62.5 * 180.0)
#define MPU9250_GYRO_SCALE_1000_RAD		M_PI / (32.8 * 180.0)
#define MPU9250_GYRO_SCALE_2000_RAD		M_PI / (16.4 * 180.0)

// Magnetometer Scale
#define MPU9250_MAG_SCALE			MPU9250_MAG_SCALE_16

// Possible Magnetometer scale values
#define MPU9250_MAG_SCALE_14		0.6f
#define MPU9250_MAG_SCALE_16		0.15f

// Calibration Values //

// Sensitivity values read from Magnetometer
#define MPU9250_MAG_SENS_X			(177.0 - 128.0) / 256.0 + 1.0f
#define MPU9250_MAG_SENS_Y			(178.0 - 128.0) / 256.0 + 1.0f
#define MPU9250_MAG_SENS_Z			(166.0 - 128.0) / 256.0 + 1.0f

#define ACCEL_X_OFFSET ((ACCEL_X_MIN + ACCEL_X_MAX) / 2.0f)		// Accelerometer offset values
#define ACCEL_Y_OFFSET ((ACCEL_Y_MIN + ACCEL_Y_MAX) / 2.0f)
#define ACCEL_Z_OFFSET ((ACCEL_Z_MIN + ACCEL_Z_MAX) / 2.0f)

#define MAG_X_OFFSET ((MAG_X_MIN + MAG_X_MAX) / 2.0f)			// Magnetometer offset values
#define MAG_Y_OFFSET ((MAG_Y_MIN + MAG_Y_MAX) / 2.0f)
#define MAG_Z_OFFSET ((MAG_Z_MIN + MAG_Z_MAX) / 2.0f)

// "Accel x,y,z (min/max) = -1.0068/1.0212  -1.0078/1.0249  -0.9982/1.0361"
#define ACCEL_X_MIN ((float) -1.0068)
#define ACCEL_X_MAX ((float) 1.0212)
#define ACCEL_Y_MIN ((float) -1.0078)
#define ACCEL_Y_MAX ((float) 1.0249)
#define ACCEL_Z_MIN ((float) -0.9982)
#define ACCEL_Z_MAX ((float) 1.0361)

// "Mag x,y,z (min/max) = 54.2138/87.7458  -5.0474/51.4555  -58.144/33.7739"
#define MAG_X_MIN ((float) 54.2138)
#define MAG_X_MAX ((float) 87.7458)
#define MAG_Y_MIN ((float) -5.0474)
#define MAG_Y_MAX ((float) 51.4555)
#define MAG_Z_MIN ((float) -58.144)
#define MAG_Z_MAX ((float) 33.7739)

//"Gyro x,y,z (current/average) = -40.00/-42.05  98.00/96.20  -18.00/-18.36"
#define GYRO_X_OFFSET ((float) 0.01283)
#define GYRO_Y_OFFSET ((float) 0.01763)
#define GYRO_Z_OFFSET ((float) 0.0036)

//*****************************************************************************
//
//		Variables
//
//*****************************************************************************

//*****************************************************************************
//
//		Structures
//
//*****************************************************************************

struct Accelerometer_Raw
{
	int16_t x;
	int16_t y;
	int16_t z;
};

struct Accelerometer
{
	float x;
	float y;
	float z;
};

struct Gyroscope_Raw
{
	int16_t x;
	int16_t y;
	int16_t z;
};

struct Gyroscope
{
	float x;
	float y;
	float z;
};

struct Magnetometer_Raw
{
	int16_t x;
	int16_t y;
	int16_t z;
	uint8_t ST2;
};

struct Magnetometer
{
	float x;
	float y;
	float z;
};

struct Accel_Offset
{
	float x;
	float y;
	float z;
};

struct Gyro_Bias
{
	float x;
	float y;
	float z;
};

struct Mag_Raw_Bias
{
	float x;
	float y;
	float z;
};

struct Mag_Sensitivity
{
	float x;
	float y;
	float z;
};

struct Mag_Offset
{
	float x;
	float y;
	float z;
};

struct MPU9250
{
	struct Accelerometer_Raw ACCEL_RAW;
	int16_t TEMP_MPU;
	struct Gyroscope_Raw GYRO_RAW;
	struct Magnetometer_Raw MAG_RAW;
	int16_t TEMP_AK;
	struct Accelerometer ACCEL;
	struct Gyroscope GYRO;
	struct Magnetometer MAG;
};

struct MPU9250_CAL
{
	struct Accel_Offset ACCEL_OFFSET;
	struct Gyro_Bias GYRO_BIAS;
	struct Mag_Raw_Bias MAG_BIAS;			// Store ASA values read from Magnetometer (Values are not used on RT because they are static)
	struct Mag_Sensitivity MAG_SENS;		// Sensitivity values calculated from ASA values are stored here
	struct Mag_Offset MAG_OFFSET;
};

//*****************************************************************************
//
//		Function Prototypes
//
//*****************************************************************************

void MPU9250_Init(void);
void MPU9250_UpdateAccel(void);
void MPU9250_UpdateGyro(void);
void MPU9250_Update9Axis(void);
void MPU9250_RBAddElement(void);
bool MPU9250_RBRemoveElement(void);
uint32_t MPU9250_ReadFIFOCount(void);

#endif /* MPU9250_H_ */
