//*****************************************************************************
//
// STEP.c - Stepper Motor Driver for Pololu DRV8825 Driver Carrier
//
// Copyright (c) 2014 Raymundo Magaña Gomez - http://rmg.mx
//
// Software License Agreement
//
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
//
//*****************************************************************************

//*****************************************************************************
//
//		Header files
//
//*****************************************************************************

#include "drivers/StepperMotor/STEP.h"

//*****************************************************************************
//
//		Global Variables
//
//*****************************************************************************

struct STEP_Motor stLeft_Handle = {STEP_LEFT_PERIPH, STEP_LEFT_PORT, STEP_LEFT_MOVE_PIN, STEP_LEFT_DIR_PIN, 0, 0};
struct STEP_Motor stRight_Handle = {STEP_RIGHT_PERIPH, STEP_RIGHT_PORT, STEP_RIGHT_MOVE_PIN, STEP_RIGHT_DIR_PIN, 0, 0};

//*****************************************************************************
//
//		Functions
//
//*****************************************************************************

//*****************************************************************************
//	STEP_Init
//*****************************************************************************

void STEP_Init(struct STEP_Motor* stMotorHandle)
{
	// Enable Port for Motor Control
	SysCtlPeripheralEnable(stMotorHandle->ui32STEP_BaseAddress);

	// Init Stepper Motor Pins - Direction
	GPIOPinTypeGPIOOutput(stMotorHandle->ui32STEP_Port, stMotorHandle->ui32STEP_Dir_Pin);

	// Init Stepper Motor Pins - Movement (toggle for step)
	GPIOPinTypeGPIOOutput(stMotorHandle->ui32STEP_Port, stMotorHandle->ui32STEP_Move_Pin);
}

//*****************************************************************************
//	STEP_Move
//*****************************************************************************

void STEP_Move(struct STEP_Motor* stMotorHandle)
{
	if(stMotorHandle->ui8STEP_Move_State)
	{
		// Move one step
		GPIOPinWrite(stMotorHandle->ui32STEP_Port, stMotorHandle->ui32STEP_Move_Pin, 0);
		// Toggle state
		stMotorHandle->ui8STEP_Move_State = false;
	}
	else
	{
		// Move one step
		GPIOPinWrite(stMotorHandle->ui32STEP_Port, stMotorHandle->ui32STEP_Move_Pin, stMotorHandle->ui32STEP_Move_Pin);
		// Toggle state
		stMotorHandle->ui8STEP_Move_State = true;
	}
}

//*****************************************************************************
//	STEP_ChangeDirection
//*****************************************************************************

void STEP_ChangeDirection(struct STEP_Motor* stMotorHandle)
{
	if(stMotorHandle->ui8STEP_Dir_State)
	{
		// Change direction
		GPIOPinWrite(stMotorHandle->ui32STEP_Port, stMotorHandle->ui32STEP_Dir_Pin, 0);
		// Toggle state
		stMotorHandle->ui8STEP_Dir_State = false;
	}
	else
	{
		// Change direction
		GPIOPinWrite(stMotorHandle->ui32STEP_Port, stMotorHandle->ui32STEP_Dir_Pin, stMotorHandle->ui32STEP_Dir_Pin);
		// Toggle state
		stMotorHandle->ui8STEP_Dir_State = true;
	}
}

//*****************************************************************************
//	STEP_SetDirection
//*****************************************************************************

void STEP_SetDirection(struct STEP_Motor* stMotorHandle, bool bDirection)
{
	// Change direction
	GPIOPinWrite(stMotorHandle->ui32STEP_Port, stMotorHandle->ui32STEP_Dir_Pin, bDirection);
	// Toggle state
	stMotorHandle->ui8STEP_Dir_State = !bDirection;
}
