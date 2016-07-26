/*
 * TIMER.h
 *
 *  Created on: 29/10/2014
 *      Author: Ray
 */

#ifndef TIMER_H_
#define TIMER_H_

//*****************************************************************************
//
//		Local Header Files
//
//*****************************************************************************

// System Configuration
#include "Config.h"

//*****************************************************************************
//
//		Definitions
//
//*****************************************************************************

#define TIMER_1_uSEC							79										// Number of timer counts needed to reach 1 usec
#define TIMER_1_mSEC							79986									// Number of timer counts needed to reach 1 msec
#define TIMER_IMU_CYCLE							(TIMER_1_mSEC * 10)						// IMU Processing cycle
#define TIMER_1_TRIGGER_OVERHEAD				120										// Amount of cycles required for code processing of the trigger pin

//*****************************************************************************
//
//		Function Protoypes
//
//*****************************************************************************

void TIMER_Init(void);
void TIMER_IntHandler1(void);
void TIMER_IntHandler2(void);

#endif /* TIMER_H_ */
