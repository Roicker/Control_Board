/*
 * CTRL.h
 *
 *  Created on: 07/08/2016
 *      Author: Ray
 */

#ifndef CTRL_H_
#define CTRL_H_

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

// These defines are needed for loop timing and PID controller timing
#define SIXTY_SECONDS (1200)
#define TEN_SECONDS    (200)
#define FIVE_SECONDS   (100)
#define ONE_SECOND      (20)

// Period time of TTimer in msec.
#define T_50MSEC        (50)
#define T_10MSEC		(10)
#define PI              (3.141592654)
#define NODF            (1)

// PID controller upper & lower limit [%]
#define GMA_HLIM		(1000.0)
#define GMA_LLIM		(-1000.0)

//*****************************************************************************
//
//		Structures
//
//*****************************************************************************

struct CTRL_PID_Data
{
   float kc;			// Controller gain
   float ti;			// Time-constant for I action from Dialog Box
   float td;			// Time-constant for D action from Dialog Box
   float ts;			// Sample time [sec.]
   float setpoint;		// Process Value Setpoint
   float yk;			// The output variable y[k]
   float k_lpf;			// Time constant [sec.] for LPF filter
   float k0;			// k0 value for PID controller
   float k1;			// k1 value for PID controller
   float k2;			// k2 value for PID controller
   float k3;			// k3 value for PID controller
   float e;				// Error
   float e1;			// Error [k-1]
   float e2;			// Error [k-2]
   float xk_1;			// PV[k-1] = Thlt[k-1]
   float xk_2;			// PV[k-2] = Thlt[k-1]
   uint16_t ts_ticks;	// Ticks for timer
   uint8_t  pid_model;	// PID Controller type [0..3]
   float pp;			// debug
   float pi;			// debug
   float pd;			// debug
};

struct CTRL_Simple_PID_Data
{
   float Kp;			// Proportional Controller gain
   float Ki;			// Integral Controller gain
   float Kd;			// Derivative Controller gain
   float ts;			// Sample time [sec.]
   float setpoint;		// Process Value Setpoint
   float oldsetpoint;	// Old Process Value Setpoint
   float yk;			// The output variable y[k]
   float e;				// Error
   float e1;			// Error [k-1]
   float e2;			// Error [k-2]
   float xk_1;			// PV[k-1] = Thlt[k-1]
   float xk_2;			// PV[k-2] = Thlt[k-1]
};


//*****************************************************************************
//
//		Function Prototypes
//
//*****************************************************************************

void CTRL_ControlLoop(void);
void CTRL_PID(float xk, struct CTRL_PID_Data *PID_Data, bool vrg);
void CTRL_InitPID(struct CTRL_PID_Data *PID_Data);
void CTRL_Init(void);
void CTRL_SimplePD(float xk, struct CTRL_Simple_PID_Data *PID_Data, bool vrg);

#endif /* CTRL_H_ */
