//*****************************************************************************
//
// DIST.c - Ultrasonic Distance Sensor Driver for SRF05 and HCSR04
//
// Copyright (c) 2014 Raymundo Magaña Gomez - http://rmg.mx
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

#include "drivers/Distance/DIST.h"

//*****************************************************************************
//
//		Global Variables
//
//*****************************************************************************

struct DIST_Sensor pstSA[DIST_NUMBER_OF_SENSORS] = { {DIST_S1_PERIPH, DIST_S1_PORT, DIST_S1_TRIGGER, DIST_S1_ECHO, DIST_S1_INT, 0}, \
													 {DIST_S2_PERIPH, DIST_S2_PORT, DIST_S2_TRIGGER, DIST_S2_ECHO, DIST_S2_INT, 1}, \
													 {DIST_S3_PERIPH, DIST_S3_PORT, DIST_S3_TRIGGER, DIST_S3_ECHO, DIST_S3_INT, 2}, \
													 {DIST_S4_PERIPH, DIST_S4_PORT, DIST_S4_TRIGGER, DIST_S4_ECHO, DIST_S4_INT, 3}};

uint8_t u8DIST_ActiveSensor = 0;

//*****************************************************************************
//
//		Functions
//
//*****************************************************************************

//*****************************************************************************
//	DIST_Init
//*****************************************************************************

void DIST_Init(void)
{
	// Variable for counting in for loop
	uint8_t	i;

	// Enable Peripherals
	for(i = 0; i < DIST_NUMBER_OF_SENSORS; i++)
	{
		SysCtlPeripheralEnable(pstSA[i].ui32DIST_BaseAddress);
	}

	// Init Pins

	for(i = 0; i < DIST_NUMBER_OF_SENSORS; i++)
	{
		// Init Ultrasonic Distance Sensor - Trigger
		GPIOPinTypeGPIOOutput(pstSA[i].ui32DIST_SensorPort , pstSA[i].ui32DIST_SensorTrigger);

		// Init Ultrasonic Distance Sensor - Echo
		GPIOPinTypeGPIOInput(pstSA[i].ui32DIST_SensorPort , pstSA[i].ui32DIST_SensorEcho);
	}

	// Enable Interrupts

	for(i = 0; i < DIST_NUMBER_OF_SENSORS; i++)
	{
		// Enable interrupts in the Echo pin
		IntEnable(pstSA[i].ui32DIST_SensorInterrupt);

		// Enable Echo interrupts only
		GPIOIntEnable(pstSA[i].ui32DIST_SensorInterrupt, pstSA[i].ui32DIST_SensorEcho);
	}
}

//*****************************************************************************
//	DIST_Measure
//*****************************************************************************

void DIST_Measure(struct DIST_Sensor* stSensorHandle)
{
	// Send Trigger Pulse
	GPIOPinWrite(stSensorHandle->ui32DIST_SensorPort, stSensorHandle->ui32DIST_SensorTrigger, stSensorHandle->ui32DIST_SensorTrigger);

	// Enable Timer 1
	TimerEnable(TIMER1_BASE, TIMER_A);

	// Store Distance Sensor currently in use
	u8DIST_ActiveSensor = stSensorHandle->u8DIST_SensorID;
}

//*****************************************************************************
//	DIST_ResetTrigger
//*****************************************************************************

void DIST_ResetTrigger(void)
{
	// Reset Trigger Pin
	GPIOPinWrite(pstSA[u8DIST_ActiveSensor].ui32DIST_SensorPort, pstSA[u8DIST_ActiveSensor].ui32DIST_SensorTrigger, 0);
}

//*****************************************************************************
//	DIST_UpdateDistance
//*****************************************************************************

void DIST_UpdateDistance(void)
{
	// Measure Distance with DS1
	DIST_Measure(&pstSA[0]);
}
