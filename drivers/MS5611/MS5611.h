/*
 * MS5611.h
 *
 *  Created on: 09/10/2014
 *      Author: Ray
 */

#ifndef MS5611_H_
#define MS5611_H_

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

// CS Line Config for MS5611
#define CS_MS5611_PERIPH			SYSCTL_PERIPH_GPIOB						// Intentify on which Peripheral is the CS line of the MS5611
#define CS_MS5611_PORT				GPIO_PORTB_BASE							// Intentify on which Port is the CS line of the MS5611
#define CS_MS5611_PIN				GPIO_PIN_6								// Intentify on which Pin is the CS line of the MS5611

//*****************************************************************************
//
//		Function Prototypes
//
//*****************************************************************************

void MS5611_Init(void);
void MS5611_UpdatePressure(void);
void MS5611_UpdateTemp(void);

#endif /* MS5611_H_ */
