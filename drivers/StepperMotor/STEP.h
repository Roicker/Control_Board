/*
 * STEP.h
 *
 *  Created on: 19/10/2014
 *      Author: Ray
 */

#ifndef STEP_H_
#define STEP_H_

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

// Left Stepper Motor
#define STEP_LEFT_PERIPH						SYSCTL_PERIPH_GPIOB							// Intentify on which Peripheral is the control of the Left Stepper Motor
#define STEP_LEFT_PORT							GPIO_PORTB_BASE								// Intentify on which Port is the control of the Left Stepper Motor
#define STEP_LEFT_MOVE_PIN						GPIO_PIN_3									// Intentify on which Pin is the step control of the Left Stepper Motor
#define STEP_LEFT_DIR_PIN						GPIO_PIN_2									// Intentify on which Pin is the direction control of the Left Stepper Motor

// Right Stepper Motor
#define STEP_RIGHT_PERIPH						SYSCTL_PERIPH_GPIOC							// Intentify on which Peripheral is the control of the Right Stepper Motor
#define STEP_RIGHT_PORT							GPIO_PORTC_BASE								// Intentify on which Port is the control of the Right Stepper Motor
#define STEP_RIGHT_MOVE_PIN						GPIO_PIN_5									// Intentify on which Pin is the step control of the Right Stepper Motor
#define STEP_RIGHT_DIR_PIN						GPIO_PIN_4									// Intentify on which Pin is the direction control of the Right Stepper Motor

// Enable Driver
#define STEP_ENABLE_PERIPH						SYSCTL_PERIPH_GPIOE							// Intentify on which Peripheral is the enable pin of the steper motors
#define STEP_ENABLE_PORT						GPIO_PORTE_BASE								// Intentify on which Port is the enable pin of the steper motors
#define STEP_ENABLE_PIN							GPIO_PIN_0									// Intentify on which Pin is the enable pin of the steper motors

#define STEP_DELAY								3500										// Minimum delay for proper stepper motor operation (Microstep 1/8)

// Stepper delay parameters
#define BASE_STEPPER_DELAY						300											// 300 uSec min delay - Could be lower, more tests needed
#define STEPPER_DELAY							(MAX_STEPPER_SPEED + BASE_STEPPER_DELAY)	// Value used to calculate delay necessary in timers

// Stepper speed parameters
#define MIN_STEPPER_SPEED						-9500										// This negative speed represents 9,500 uSec + 300 uSec (min delay) = 9,800 uSec or 9.8 mSec (Smaller than Control loop cycle time)
#define MAX_STEPPER_SPEED						9500										// This speed represents 9,500 uSec + 300 uSec (min delay) = 9,800 uSec or 9.8 mSec (Smaller than Control loop cycle time)

// Straight line Speed mapping
#define B_CONSTANT								0											// Offset or "b" on the equation y = m*x + b when the Controller output is -100 to 100
#define M_CONSTANT								(MAX_STEPPER_SPEED / 1000)					// Slope or "m" on the equation y = m*x + b when the Controller output is -100 to 100
#define B_CONSTANT_POSITIVE						MIN_STEPPER_SPEED							// Offset or "b" on the equation y = m*x + b when the Controller output is 0 to 100
#define M_CONSTANT_POSITIVE						(MAX_STEPPER_SPEED / 50)					// Slope or "m" on the equation y = m*x + b when the Controller output is 0 to 100

//*****************************************************************************
//
//		Structures
//
//*****************************************************************************

struct STEP_Motor
{
	const uint32_t ui32STEP_BaseAddress;
	const uint32_t ui32STEP_Port;
	const uint32_t ui32STEP_MovePin;
	const uint32_t ui32STEP_DirPin;
	uint32_t ui32STEP_Delay;
	bool bSTEP_MoveState : 1;
	bool bSTEP_DirState : 1;
	bool bSTEP_MovementEnabled : 1;
};

//*****************************************************************************
//
//		Function Prototypes
//
//*****************************************************************************

void STEP_Init(struct STEP_Motor* stMotorHandle);
void STEP_Move(struct STEP_Motor* stMotorHandle);
void STEP_ChangeDirection(struct STEP_Motor* stMotorHandle);
void STEP_SetDirection(struct STEP_Motor* stMotorHandle, bool bDirection);
void STEP_SetSpeed(struct STEP_Motor* stMotorHandle, int16_t i16Speed);
void STEP_Enable(void);
void STEP_Disable(void);
int16_t STEP_AdaptSpeedToRange(float pfSpeed);
int16_t STEP_AdaptSpeedToPositiveRange(float fSpeed);

#endif /* STEP_H_ */
