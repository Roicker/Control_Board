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
#define CS_MPU9250_PERIPH			SYSCTL_PERIPH_GPIOA						// Intentify on which Peripheral is the CS line of the MPU9250
#define CS_MPU9250_PORT				GPIO_PORTA_BASE							// Intentify on which Port is the CS line of the MPU9250
#define CS_MPU9250_PIN				GPIO_PIN_3								// Intentify on which Pin is the CS line of the MPU9250

//*****************************************************************************
//
//		Function Prototypes
//
//*****************************************************************************

void MPU9250_Init(void);

#endif /* MPU9250_H_ */
