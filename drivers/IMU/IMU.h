/*
 * IMU.h
 *
 *  Created on: 09/10/2014
 *      Author: Ray
 */

#ifndef IMU_H_
#define IMU_H_

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

// Pi should be taken from Math.h => Check this
#define M_PI            			3.14159265358979323846

#define IMU_SPI_MODULE				SPI_MODULE_1				// SPI (SSI) Port that will be used by the IMU - This should be modified toghether with IMU_SPI_BASE
#define IMU_SPI_BASE				SSI1_BASE					// SPI Module Base - This should be modified toghether with IMU_SPI_MODULE
#define IMU_SPI_MODE				SSI_FRF_MOTO_MODE_3			// Select SPI Mode - 0 or 3 for the MS5611 and MPU9250
#define IMU_SPI_BAUDRATE			1000000						// SPI Communication Baudrate for IMU

#define IMU_INTPIN_PERIPH			SYSCTL_PERIPH_GPIOE			// Intentify on which Peripheral is the control of the IMU Interrupt pin
#define IMU_INTPIN_PORT				GPIO_PORTE_BASE				// Intentify on which Port is the control of the IMU Interrupt pin
#define IMU_INTPIN_PIN				GPIO_PIN_2					// Intentify on which Pin is the the IMU Interrupt
#define IMU_INTPIN_INT				INT_GPIOE					// Intentify on which Peripheral is the interrupt of the IMU Interrupt pin

#define IMU_ACC_GYRO_CAL_CYCLES		50							// Amount of samples for Accelerometer and Gyro calibration
#define IMU_MAG_CAL_CYCLES			2000						// Amount of samples for Magnetometer calibration

#define IMU_SAMPLE_FREQ				100.0f						// Sample frequency in Hz
#define IMU_BETA_DEF				0.1f						// 2 * Proportional gain

//*****************************************************************************
//
//		Function Prototypes
//
//*****************************************************************************

void IMU_Init(void);
void IMU_UpdateValues(void);
void IMU_IntHandler(void);
void IMU_ProcessValues(void);
void IMU_CalibrateGyroAccel(void);
void IMU_CalibrateMag(void);
void IMU_MadgwickAHRSupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz);
void IMU_MadgwickAHRSupdateIMU(float gx, float gy, float gz, float ax, float ay, float az);
float IMU_invSqrt(float x);
void IMU_ComputeAngles(void);

#endif /* IMU_H_ */
