/*
 * TIMER.c
 *
 *  Created on: 29/10/2014
 *      Author: Ray
 */

//*****************************************************************************
//
//		Header files
//
//*****************************************************************************

#include "drivers/Timer/TIMER.h"

//*****************************************************************************
//
//		Global variables
//
//*****************************************************************************

// Variable for 10 mSec Container
bool C_10mS = false;

// Variable for 100 uSec Container
bool C_100uS = false;

//*****************************************************************************
//
//		Extern Variables
//
//*****************************************************************************

extern struct STEP_Motor stRight_Handle;
extern struct STEP_Motor stLeft_Handle;

//*****************************************************************************
//
//		Functions
//
//*****************************************************************************

//*****************************************************************************
//	TIMER_IntHandler1
//*****************************************************************************

void TIMER_IntHandler1(void)
{
	// Clear the timer interrupt
	TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

	// Lower the trigger pin
	DIST_ResetTrigger();
}

//*****************************************************************************
//	TIMER_IntHandler2
//*****************************************************************************

void TIMER_IntHandler2(void)
{
	// Clear the timer interrupt
	TimerIntClear(TIMER2_BASE, TIMER_TIMA_TIMEOUT);

	// Reset initial load of Timer 2
	TimerLoadSet(TIMER2_BASE, TIMER_A, TIMER_IMU_CYCLE);

	// Set container flag
	C_10mS = true;
}

//*****************************************************************************
//	TIMER_IntHandler3
//*****************************************************************************

void TIMER_IntHandler3(void)
{
	// Clear the timer interrupt
	TimerIntClear(TIMER3_BASE, TIMER_TIMA_TIMEOUT);

	// Reset initial load of Timer 3
	TimerLoadSet(TIMER3_BASE, TIMER_A, TIMER_100US_CONTAINER);

	// Set container flag
	C_100uS = true;
}

//*****************************************************************************
//	TIMER_IntHandler4
//*****************************************************************************

void TIMER_IntHandler4(void)
{
	// Clear the timer interrupt
	TimerIntClear(TIMER4_BASE, TIMER_TIMA_TIMEOUT);

	// Load the counter for the Right Stepper timing
	TimerLoadSet(TIMER4_BASE, TIMER_A, stRight_Handle.ui32STEP_Delay);

	// Send step pulse for Right Stepper Motor
	STEP_Move(&stRight_Handle);
}

//*****************************************************************************
//	TIMER_IntHandler5
//*****************************************************************************

void TIMER_IntHandler5(void)
{
	// Clear the timer interrupt
	TimerIntClear(TIMER5_BASE, TIMER_TIMA_TIMEOUT);

	// Load the counter for the Left Stepper timing
	TimerLoadSet(TIMER5_BASE, TIMER_A, stLeft_Handle.ui32STEP_Delay);

	// Send step pulse for Left Stepper Motor
	STEP_Move(&stLeft_Handle);
}

//*****************************************************************************
//
//		Functions
//
//*****************************************************************************

//*****************************************************************************
//	TIMER_Init
//*****************************************************************************

void TIMER_Init(void)
{
	///// Timer 0 ///// For DIST Module

	// Enable Timer 0 Peripheral
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

	// Configure Timer 0
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

	// Prepare initial load of Timer 0
	TimerLoadSet(TIMER0_BASE, TIMER_A, 0xFFFFFFFF);

	// Select the Clock source for Timer 0
	TimerClockSourceSet(TIMER0_BASE, TIMER_CLOCK_SYSTEM);

	// Enable Timer 0
	TimerEnable(TIMER0_BASE, TIMER_A);

	///// Timer 1 ///// For DIST Module

	// Enable Timer 1 Peripheral
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);

	// Configure Timer 1
	TimerConfigure(TIMER1_BASE, TIMER_CFG_ONE_SHOT);

	// Prepare initial load of Timer 1
	TimerLoadSet(TIMER1_BASE, TIMER_A, (TIMER_1_uSEC * 10) - TIMER_1_TRIGGER_OVERHEAD );

	// Select the Clock source for Timer 1
	TimerClockSourceSet(TIMER1_BASE, TIMER_CLOCK_SYSTEM);

	// Enable interrupts for Timer 1
	IntEnable(INT_TIMER1A);

	// Set interrupt prio
	//IntPrioritySet(INT_TIMER1A, 0x40);

	// Enable timeout interrupts for Timer 1
	TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

	///// Timer 2 ///// 10 mSec Container

	// Enable Timer 2 Peripheral
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);

	// Configure Timer 2
	TimerConfigure(TIMER2_BASE, TIMER_CFG_PERIODIC);

	// Prepare initial load of Timer 2
	TimerLoadSet(TIMER2_BASE, TIMER_A, TIMER_IMU_CYCLE);

	// Select the Clock source for Timer 2
	TimerClockSourceSet(TIMER2_BASE, TIMER_CLOCK_SYSTEM);

	// Enable Timer 2
	TimerEnable(TIMER2_BASE, TIMER_A);

	// Enable interrupts for Timer 2
	IntEnable(INT_TIMER2A);

	// Enable timeout interrupts for Timer 2
	TimerIntEnable(TIMER2_BASE, TIMER_TIMA_TIMEOUT);

	///// Timer 3 ///// 100uSec Container

	// Enable Timer 3 Peripheral
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER3);

	// Configure Timer 3
	TimerConfigure(TIMER3_BASE, TIMER_CFG_PERIODIC);

	// Prepare initial load of Timer 3
	TimerLoadSet(TIMER3_BASE, TIMER_A, TIMER_100US_CONTAINER);

	// Select the Clock source for Timer 3
	TimerClockSourceSet(TIMER3_BASE, TIMER_CLOCK_SYSTEM);

	// Enable Timer 3
	TimerEnable(TIMER3_BASE, TIMER_A);

	// Enable interrupts for Timer 3
	IntEnable(INT_TIMER3A);

	// Enable timeout interrupts for Timer 3
	TimerIntEnable(TIMER3_BASE, TIMER_TIMA_TIMEOUT);

	///// Timer 4 ///// Right Stepper Motor

	// Enable Timer 4 Peripheral
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER4);

	// Configure Timer 4
	TimerConfigure(TIMER4_BASE, TIMER_CFG_PERIODIC);

	// Prepare initial load of Timer 4
	TimerLoadSet(TIMER4_BASE, TIMER_A, TIMER_1_mSEC);

	// Select the Clock source for Timer 4
	TimerClockSourceSet(TIMER4_BASE, TIMER_CLOCK_SYSTEM);

	// Enable Timer 4
	TimerEnable(TIMER4_BASE, TIMER_A);

	// Enable interrupts for Timer 4
	IntEnable(INT_TIMER4A);

	// Enable timeout interrupts for Timer 4
	TimerIntEnable(TIMER4_BASE, TIMER_TIMA_TIMEOUT);

	///// Timer 5 ///// Left Stepper Motor

	// Enable Timer 5 Peripheral
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER5);

	// Configure Timer 5
	TimerConfigure(TIMER5_BASE, TIMER_CFG_PERIODIC);

	// Prepare initial load of Timer 5
	TimerLoadSet(TIMER5_BASE, TIMER_A, TIMER_1_mSEC);

	// Select the Clock source for Timer 5
	TimerClockSourceSet(TIMER5_BASE, TIMER_CLOCK_SYSTEM);

	// Enable Timer 5
	TimerEnable(TIMER5_BASE, TIMER_A);

	// Enable interrupts for Timer 5
	IntEnable(INT_TIMER5A);

	// Enable timeout interrupts for Timer 5
	TimerIntEnable(TIMER5_BASE, TIMER_TIMA_TIMEOUT);
}
