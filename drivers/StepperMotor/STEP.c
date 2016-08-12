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

struct STEP_Motor stLeft_Handle = {STEP_LEFT_PERIPH, STEP_LEFT_PORT, STEP_LEFT_MOVE_PIN, STEP_LEFT_DIR_PIN, (STEPPER_DELAY * TIMER_1_uSEC), false, false, false};
struct STEP_Motor stRight_Handle = {STEP_RIGHT_PERIPH, STEP_RIGHT_PORT, STEP_RIGHT_MOVE_PIN, STEP_RIGHT_DIR_PIN, (STEPPER_DELAY * TIMER_1_uSEC), false, false, false};

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
	// Check if the stepper movement is enabled
	if(stMotorHandle->bSTEP_MovementEnabled)
	{
		// Check state of Step Pin
		if(stMotorHandle->bSTEP_MoveState)
		{
			// Move one step
			GPIOPinWrite(stMotorHandle->ui32STEP_Port, stMotorHandle->ui32STEP_MovePin, 0);
			// Toggle state
			stMotorHandle->bSTEP_MoveState = false;
		}
		else
		{
			// Move one step
			GPIOPinWrite(stMotorHandle->ui32STEP_Port, stMotorHandle->ui32STEP_MovePin, stMotorHandle->ui32STEP_MovePin);
			// Toggle state
			stMotorHandle->bSTEP_MoveState = true;
		}
	}
}

//*****************************************************************************
//	STEP_ChangeDirection
//*****************************************************************************

void STEP_ChangeDirection(struct STEP_Motor* stMotorHandle)
{
	if(stMotorHandle->bSTEP_DirState)
	{
		// Change direction
		GPIOPinWrite(stMotorHandle->ui32STEP_Port, stMotorHandle->ui32STEP_DirPin, 0);
		// Toggle direction flag
		stMotorHandle->bSTEP_DirState = false;
	}
	else
	{
		// Change direction
		GPIOPinWrite(stMotorHandle->ui32STEP_Port, stMotorHandle->ui32STEP_DirPin, stMotorHandle->ui32STEP_DirPin);
		// Toggle direction flag
		stMotorHandle->bSTEP_DirState = true;
	}
}

//*****************************************************************************
//	STEP_SetDirection
//  bDirection = 1 sets direction forwards
//  bDirection = 0 sets direction backwards
//*****************************************************************************

void STEP_SetDirection(struct STEP_Motor* stMotorHandle, bool bDirection)
{
	if(bDirection == true)
	{
		// Set direction
		GPIOPinWrite(stMotorHandle->ui32STEP_Port, stMotorHandle->ui32STEP_DirPin, stMotorHandle->ui32STEP_DirPin);
	}
	else
	{
		// Set direction
		GPIOPinWrite(stMotorHandle->ui32STEP_Port, stMotorHandle->ui32STEP_DirPin, 0);
	}
	// Set direction flag
	stMotorHandle->bSTEP_DirState = bDirection;
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

//*****************************************************************************
//	STEP_AdaptSpeedToRange
//*****************************************************************************
int16_t STEP_AdaptSpeedToRange(float fSpeed)
{
	// Local variable to store return value
	int16_t ReturnSpeed = 0;

	// Calculate the Speed in the adapted Range
	ReturnSpeed = M_CONSTANT * fSpeed;

	// Return Speed in the required range
	return ReturnSpeed;
}

//*****************************************************************************
//	STEP_AdaptSpeedToPositiveRange
//*****************************************************************************
int16_t STEP_AdaptSpeedToPositiveRange(float fSpeed)
{
	// Local variable to store return value
	int16_t ReturnSpeed = 0;

	// Check if Speed is in positive range
	if(fSpeed > 50.0)
	{
		// Calculate the Speed in the adapted Range
		ReturnSpeed = M_CONSTANT * fSpeed + B_CONSTANT;
	}
	else if(fSpeed < 50.0) // Check if speed is in negative range
	{
		// Invert the input to convert the speed to negative
		fSpeed = 50.0 - fSpeed;

		// Calculate the Speed in the adapted Range
		ReturnSpeed = - (M_CONSTANT * fSpeed);
	}
	else // Speed is equal to 50
	{
		// If speed is equal to 50 then it is in the middle of the range
		ReturnSpeed = 0;
	}

	// Return Speed in the required range
	return ReturnSpeed;
}

//*****************************************************************************
//	STEP_SetSpeed
//  Set motor speed
//  Possible values of i16Speed are from MIN_STEPPER_SPEED to MAX_STEPPER_SPEED
//*****************************************************************************

void STEP_SetSpeed(struct STEP_Motor* stMotorHandle, int16_t i16Speed)
{
	// Check that speed is within range
	if(i16Speed > MAX_STEPPER_SPEED)
	{
		// Speed too high
		i16Speed = MAX_STEPPER_SPEED;
	}
	else if(i16Speed < MIN_STEPPER_SPEED)
	{
		// Speed too low
		i16Speed = MIN_STEPPER_SPEED;
	}

	// Check if speed is positive
	if(i16Speed > 0)
	{
		// Change Stepper Motor step delay according to current speed
		stMotorHandle->ui32STEP_Delay = (STEPPER_DELAY - i16Speed) * TIMER_1_uSEC;

		// Set direction backwards
		STEP_SetDirection(stMotorHandle, false);

		// Enable movement
		stMotorHandle->bSTEP_MovementEnabled = true;
	}
	else if(i16Speed < 0) // Check if speed is negative
	{
		// Change Stepper Motor step delay according to current speed
		stMotorHandle->ui32STEP_Delay = (STEPPER_DELAY + i16Speed) * TIMER_1_uSEC;

		// Set direction forward
		STEP_SetDirection(stMotorHandle, true);

		// Enable movement
		stMotorHandle->bSTEP_MovementEnabled = true;
	}
	else // If speed is 0 increase delay to reduce system workload
	{
		// Change Stepper Motor step delay to reduce system workload
		stMotorHandle->ui32STEP_Delay = (STEPPER_DELAY) * TIMER_1_uSEC;

		// Disable movement
		stMotorHandle->bSTEP_MovementEnabled = false;
	}
}
