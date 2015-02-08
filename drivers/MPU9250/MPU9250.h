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
#define MPU9250_EXP_FIFO_COUNT		21
#define	MPU9250_RX_FIFO_BYTES		22

//*****************************************************************************
//
//		Structures
//
//*****************************************************************************

struct Accelerometer
{
	int16_t x;
	int16_t y;
	int16_t z;
};

struct Gyroscope
{
	int16_t x;
	int16_t y;
	int16_t z;
};

struct Magnetometer
{
	int16_t y;
	int16_t x;
	int16_t z;
	uint8_t ST2;
};

struct Accel_Bias
{
	int32_t x;
	int32_t y;
	int32_t z;
};

struct Gyro_Bias
{
	int32_t x;
	int32_t y;
	int32_t z;
};

struct Mag_Bias
{
	int32_t y;
	int32_t x;
	int32_t z;
};

struct MPU9250
{
	struct Accelerometer ACCEL;
	int16_t TEMP;
	struct Gyroscope GYRO;
	struct Magnetometer MAG;
	struct Accel_Bias ACCEL_BIAS;
	struct Gyro_Bias GYRO_BIAS;
	struct Mag_Bias MAG_BIAS;
	bool AccGyro_Calibrated;
	bool Mag_Calibrated;
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
uint32_t MPU9250_ReadFIFOCount(void);

#endif /* MPU9250_H_ */
