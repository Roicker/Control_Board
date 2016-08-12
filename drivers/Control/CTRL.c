//*****************************************************************************
//
// CTRL.c - Control functions
//
// Copyright (c) 2016 Raymundo Magaña Gomez - http://raymundomagana.webs.com/
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

#include "drivers/Control/CTRL.h"

//*****************************************************************************
//
//		Global Variables
//
//*****************************************************************************

int16_t CTRL_RightSteering = 0;
int16_t CTRL_LeftSteering = 0;

//*****************************************************************************
//		Structures for PID Controllers
//           The following parameters can be changed:
//           Kc: The controller gain
//           Ti: Time-constant for the Integral Gain
//           Td: Time-constant for the Derivative Gain
//           Ts: The sample period [seconds]
//*****************************************************************************

//struct CTRL_PID_Data CTRL_RobotAngle = {0.1, 0, 0, 0.01, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
struct CTRL_Simple_PID_Data CTRL_RobotAngle = {100, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0};
struct CTRL_PID_Data CTRL_RobotSpeed = {0, 0, 0, 0.01, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

//*****************************************************************************
//
//		Extern Variables
//
//*****************************************************************************

// External variable that contains the Robot Inclination Angle
extern volatile float fex;

// Stepper Motor Handles
extern struct STEP_Motor stRight_Handle;
extern struct STEP_Motor stLeft_Handle;

//*****************************************************************************
//
//		Functions
//
//*****************************************************************************


//*****************************************************************************
//	CTRL_ControlLoop
//*****************************************************************************

void CTRL_ControlLoop()
{
	// Update Robot Angle PID
	//CTRL_PID(fex, &CTRL_RobotAngle, true);
	CTRL_SimplePD(fex, &CTRL_RobotAngle, true);

	// Update wheel speed after converting to appropriate range and adding steering values
	STEP_SetSpeed(&stRight_Handle, ( STEP_AdaptSpeedToRange(CTRL_RobotAngle.yk) + CTRL_RightSteering ) );
	STEP_SetSpeed(&stLeft_Handle, ( STEP_AdaptSpeedToRange(CTRL_RobotAngle.yk) + CTRL_LeftSteering ) );

	// Update Robot Speed PID
	//CTRL_PID(####, &CTRL_Output, &CTRL_RobotSpeed, true);
}

//*****************************************************************************
//	CTRL_Init
//*****************************************************************************

void CTRL_Init()
{
	// Init Robot Angle Controller
	//CTRL_InitPID(&CTRL_RobotAngle);

	// Init Robot Speed Controller
	CTRL_InitPID(&CTRL_RobotSpeed);
}

//*****************************************************************************
//  CTRL_InitPID
//
//  Purpose  : This function initialises the Type A PID controller
//             It also utilizes filtering of the D-action
//  Variables: PID_Data: pointer to struct containing all PID parameters
//
//                   Kc.Ts
//             k0 =  -----   (for I-term)
//                    Ti
//
//                       Td
//             k1 = Kc . --  (for D-term)
//                       Ts
//
//  Returns  : No values are returned
//*****************************************************************************
void CTRL_InitPID(struct CTRL_PID_Data *PID_Data)
{
	// Controller tick
	PID_Data->ts_ticks = (int)((PID_Data->ts * 1000.0) / T_10MSEC);

	if (PID_Data->ts_ticks > SIXTY_SECONDS)
	{
		PID_Data->ts_ticks = SIXTY_SECONDS;
	}

	// Calculate k0 if ti is not 0
	if (PID_Data->ti == 0.0)
	{
		PID_Data->k0 = 0.0;
	}
	else
	{
		PID_Data->k0 = PID_Data->kc * PID_Data->ts / PID_Data->ti;
	}

	// Calculate k1
	PID_Data->k1 = PID_Data->kc * PID_Data->td / PID_Data->ts;
}


//*****************************************************************************
//	CTRL_PID
//  Purpose  : This function implements the Takahashi Type C PID
//             controller: the P and D term are no longer dependent
//             on the set-point, only on PV (which is Thlt).
//             The D term is NOT low-pass filtered.
//             This function should be called once every TS seconds.
//  Variables:
//        xk : The input variable x[k] (= measured variable)
// *PID_Data : Pointer to struct containing PID parameters
//        vrg: Release signal: 1 = Start control, 0 = disable PID controller
//  Returns  : No values are returned
//*****************************************************************************
void CTRL_PID(float xk, struct CTRL_PID_Data *PID_Data, bool vrg)
{
	if (vrg)
	{
		//--------------------------------------------------------------------------------
		// Takahashi Type C PID controller (NO filtering of D-action):
		//
		//                                    Kc.Ts        Kc.Td
		// y[k] = y[k-1] + Kc.(x[k-1]-x[k]) + -----.e[k] + -----.(2.x[k-1]-x[k]-x[k-2])
		//                                      Ti           Ts
		//
		//--------------------------------------------------------------------------------
		PID_Data->pp = PID_Data->kc * (PID_Data->xk_1 - xk);              				// Kc.(x[k-1]-x[k])
		PID_Data->pi = PID_Data->k0 * (PID_Data->setpoint - xk);              			// (Kc.Ts/Ti).e[k]
		PID_Data->pd = PID_Data->k1 * (2.0 * PID_Data->xk_1 - xk - PID_Data->xk_2); 	// (Kc.Td/Ts).(2.x[k-1]-x[k]-x[k-2])
		PID_Data->yk  += PID_Data->pp + PID_Data->pi + PID_Data->pd;     				// Add y[k-1] + P, I & D actions to y[k]
	}
	else { PID_Data->yk = PID_Data->pp = PID_Data->pi = PID_Data->pd = 0.0; }

	PID_Data->xk_2  = PID_Data->xk_1;		// x[k-2] = x[k-1]
	PID_Data->xk_1  = xk;					// x[k-1] = x[k]

	// Limit y[k] to GMA_HLIM and GMA_LLIM
	if (PID_Data->yk > GMA_HLIM)
	{
		PID_Data->yk = GMA_HLIM;
	}
	else if (PID_Data->yk < GMA_LLIM)
	{
		PID_Data->yk = GMA_LLIM;
	} // else
}

//*****************************************************************************
//	CTRL_SimplePD
//  Purpose  : This function implements a simple digital PD Controller
//  Variables:
//        xk : The input variable x[k] (= measured variable)
// *PID_Data : Pointer to struct containing PID parameters
//        vrg: Release signal: 1 = Start control, 0 = disable PID controller
//  Returns  : No values are returned
//*****************************************************************************
void CTRL_SimplePD(float xk, struct CTRL_Simple_PID_Data *PID_Data, bool vrg)
{
	// Local Variable
	//float xk_filtered = 0;

	if (vrg)
	{
		// Filter x[k]
		//xk_filtered = (xk + PID_Data->xk_1 + PID_Data->xk_2) / 3;
		// Calculate error
		PID_Data->e = PID_Data->setpoint - xk;
		//PID_Data->e = PID_Data->setpoint - xk_filtered;
		// Calculate yk
		PID_Data->yk += (PID_Data->Kp * PID_Data->e) + (PID_Data->Kd * (PID_Data->setpoint - PID_Data->oldsetpoint) - PID_Data->Kd * (xk - PID_Data->e2)) / PID_Data->ts;
	}
	else
	{
		PID_Data->yk = 0.0;
	}

	// Store old values
	PID_Data->e2 = PID_Data->e1;
	PID_Data->e1 = PID_Data->e;

	PID_Data->oldsetpoint = PID_Data->setpoint;

	PID_Data->xk_2  = PID_Data->xk_1;		// x[k-2] = x[k-1]
	PID_Data->xk_1  = xk;					// x[k-1] = x[k]

	// Limit y[k] to GMA_HLIM and GMA_LLIM
	if (PID_Data->yk > GMA_HLIM)
	{
		PID_Data->yk = GMA_HLIM;
	}
	else if (PID_Data->yk < GMA_LLIM)
	{
		PID_Data->yk = GMA_LLIM;
	} // else
}
