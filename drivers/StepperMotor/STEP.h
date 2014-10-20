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
#define STEP_LEFT_MOVE_PIN						GPIO_PIN_0								// Intentify on which Pin is the step control of the Left Stepper Motor
#define STEP_LEFT_DIR_PIN						GPIO_PIN_1								// Intentify on which Pin is the direction control of the Left Stepper Motor

// Right Stepper Motor
#define STEP_RIGHT_PERIPH						SYSCTL_PERIPH_GPIOE						// Intentify on which Peripheral is the control of the Right Stepper Motor
#define STEP_RIGHT_PORT							GPIO_PORTE_BASE							// Intentify on which Port is the control of the Right Stepper Motor
#define STEP_RIGHT_MOVE_PIN						GPIO_PIN_4								// Intentify on which Pin is the step control of the Right Stepper Motor
#define STEP_RIGHT_DIR_PIN						GPIO_PIN_5								// Intentify on which Pin is the direction control of the Right Stepper Motor

#define STEP_DELAY								480										// Minimum delay for proper stepper motor operation (Microstep 1/8)

//*****************************************************************************
//
//		Structures
//
//*****************************************************************************

struct STEP_Motor
{
	const uint32_t ui32STEP_BaseAddress;
	const uint32_t ui32STEP_Port;
	const uint32_t ui32STEP_Move_Pin;
	const uint32_t ui32STEP_Dir_Pin;
	bool ui8STEP_Move_State : 1;
	bool ui8STEP_Dir_State : 1;
};

//*****************************************************************************
//
//		Function Prototypes
//
//*****************************************************************************

void STEP_Init(struct STEP_Motor* stMotorHandle);
void STEP_Move(struct STEP_Motor* stMotorHandle);
void STEP_ChangeDirection(struct STEP_Motor* stMotorHandle);

#endif /* STEP_H_ */
