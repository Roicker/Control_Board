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
//		Function Prototypes
//
//*****************************************************************************

void IMU_Init(void);

//*****************************************************************************
//
//		Definitions
//
//*****************************************************************************

#define IMU_SPI_MODULE				SPI_MODULE_0				// SPI (SSI) Port that will be used by the IMU

#endif /* IMU_H_ */
