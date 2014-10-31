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
//	TIMER_Init
//*****************************************************************************

void TIMER_Init(void)
{
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

	// Enable timeout interrupts for Timer 1
	TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
}
