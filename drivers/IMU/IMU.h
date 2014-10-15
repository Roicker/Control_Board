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

#define IMU_SPI_MODULE				SPI_MODULE_0				// SPI (SSI) Port that will be used by the IMU - This should be modified toghether with IMU_SPI_BASE
#define IMU_SPI_BASE				SSI0_BASE					// SPI Module Base - This should be modified toghether with IMU_SPI_MODULE
#define IMU_SPI_MODE				SSI_FRF_MOTO_MODE_3			// Select SPI Mode - 0 or 3 for the MS5611 and MPU9250
#define IMU_SPI_BAUDRATE			1000000						// SPI Communication Baudrate for IMU

//*****************************************************************************
//
//		Function Prototypes
//
//*****************************************************************************

void IMU_Init(void);
void IMU_UpdateValues(void);

#endif /* IMU_H_ */
