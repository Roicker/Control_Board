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
#define STEP_LEFT_PERIPH						SYSCTL_PERIPH_GPIOB						// Intentify on which Peripheral is the control of the Left Stepper Motor
#define STEP_LEFT_PORT							GPIO_PORTB_BASE							// Intentify on which Port is the control of the Left Stepper Motor
#define STEP_LEFT_MOVE_PIN						GPIO_PIN_3								// Intentify on which Pin is the step control of the Left Stepper Motor
#define STEP_LEFT_DIR_PIN						GPIO_PIN_2								// Intentify on which Pin is the direction control of the Left Stepper Motor

// Right Stepper Motor
#define STEP_RIGHT_PERIPH						SYSCTL_PERIPH_GPIOC						// Intentify on which Peripheral is the control of the Right Stepper Motor
#define STEP_RIGHT_PORT							GPIO_PORTC_BASE							// Intentify on which Port is the control of the Right Stepper Motor
#define STEP_RIGHT_MOVE_PIN						GPIO_PIN_5								// Intentify on which Pin is the step control of the Right Stepper Motor
#define STEP_RIGHT_DIR_PIN						GPIO_PIN_4								// Intentify on which Pin is the direction control of the Right Stepper Motor

// Enable Driver
#define STEP_ENABLE_PERIPH						SYSCTL_PERIPH_GPIOE						// Intentify on which Peripheral is the enable pin of the steper motors
#define STEP_ENABLE_PORT						GPIO_PORTE_BASE							// Intentify on which Port is the enable pin of the steper motors
#define STEP_ENABLE_PIN							GPIO_PIN_0								// Intentify on which Pin is the enable pin of the steper motors

#define STEP_DELAY								500										// Minimum delay for proper stepper motor operation (Microstep 1/8)

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
	bool ui8STEP_MoveState : 1;
	bool ui8STEP_DirState : 1;
};

//*****************************************************************************
//
//		Function Prototypes
//
//*****************************************************************************

void STEP_Init(struct STEP_Motor* stMotorHandle);
void STEP_Move(struct STEP_Motor* stMotorHandle);
void STEP_ChangeDirection(struct STEP_Motor* stMotorHandle);
void STEP_Enable(void);
void STEP_Disable(void);

#endif /* STEP_H_ */
