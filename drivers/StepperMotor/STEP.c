//*****************************************************************************
//
// STEP.c - Stepper Motor Driver for Pololu DRV8825 Driver Carrier
//
// Copyright (c) 2014 Raymundo Magaña Gomez - http://raymundomagana.webs.com/
//
// Software License Agreement
//
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. WE SHALL NOT, UNDER ANY
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

	// Enable Port for Motor Control "Enable"
	SysCtlPeripheralEnable(STEP_ENABLE_PERIPH);

	// Disable NMI for PD7 if it will be used
	if( (stMotorHandle->ui32STEP_BaseAddress == SYSCTL_PERIPH_GPIOD) && ( (stMotorHandle->ui32STEP_DirPin == GPIO_PIN_7) || (stMotorHandle->ui32STEP_MovePin == GPIO_PIN_7) ) )
	{
		HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
		HWREG(GPIO_PORTD_BASE + GPIO_O_CR) |= 0x80;
		HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = 0;
	}

	// Init Stepper Motor Pins - Direction
	GPIOPinTypeGPIOOutput(stMotorHandle->ui32STEP_Port, stMotorHandle->ui32STEP_DirPin);

	// Init Stepper Motor Pins - Movement (toggle for step)
	GPIOPinTypeGPIOOutput(stMotorHandle->ui32STEP_Port, stMotorHandle->ui32STEP_MovePin);

	// Init Stepper Motor Pins - Enable
	GPIOPinTypeGPIOOutput(STEP_ENABLE_PORT, STEP_ENABLE_PIN);
}

//*****************************************************************************
//	STEP_Move
//*****************************************************************************

void STEP_Move(struct STEP_Motor* stMotorHandle)
{
	if(stMotorHandle->ui8STEP_MoveState)
	{
		// Move one step
		GPIOPinWrite(stMotorHandle->ui32STEP_Port, stMotorHandle->ui32STEP_MovePin, 0);
		// Toggle state
		stMotorHandle->ui8STEP_MoveState = false;
	}
	else
	{
		// Move one step
		GPIOPinWrite(stMotorHandle->ui32STEP_Port, stMotorHandle->ui32STEP_MovePin, stMotorHandle->ui32STEP_MovePin);
		// Toggle state
		stMotorHandle->ui8STEP_MoveState = true;
	}
}

//*****************************************************************************
//	STEP_ChangeDirection
//*****************************************************************************

void STEP_ChangeDirection(struct STEP_Motor* stMotorHandle)
{
	if(stMotorHandle->ui8STEP_DirState)
	{
		// Change direction
		GPIOPinWrite(stMotorHandle->ui32STEP_Port, stMotorHandle->ui32STEP_DirPin, 0);
		// Toggle state
		stMotorHandle->ui8STEP_DirState = false;
	}
	else
	{
		// Change direction
		GPIOPinWrite(stMotorHandle->ui32STEP_Port, stMotorHandle->ui32STEP_DirPin, stMotorHandle->ui32STEP_DirPin);
		// Toggle state
		stMotorHandle->ui8STEP_DirState = true;
	}
}

//*****************************************************************************
//	STEP_SetDirection
//*****************************************************************************

void STEP_SetDirection(struct STEP_Motor* stMotorHandle, bool bDirection)
{
	// Change direction
	GPIOPinWrite(stMotorHandle->ui32STEP_Port, stMotorHandle->ui32STEP_DirPin, bDirection);
	// Toggle state
	stMotorHandle->ui8STEP_DirState = !bDirection;
}

//*****************************************************************************
//	STEP_Enable
//*****************************************************************************

void STEP_Enable()
{
	// Enable Stepper Motor Driver Modules
	GPIOPinWrite(STEP_ENABLE_PORT, STEP_ENABLE_PIN, 0);
}

//*****************************************************************************
//	STEP_Disable
//*****************************************************************************

void STEP_Disable()
{
	// Disable Stepper Motor Driver Modules
	GPIOPinWrite(STEP_ENABLE_PORT, STEP_ENABLE_PIN, STEP_ENABLE_PIN);
}
