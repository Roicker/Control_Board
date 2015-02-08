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
uint8_t	u8DIST_EdgeCounter = 0;

uint32_t u32DIST_StartTimer = 0;
uint32_t u32DIST_EndTimer = 0;
uint32_t u32DIST_DistDS1Int = 0;
uint32_t u32DIST_DistDS1Frac = 0;

uint64_t u64DIST_PulseCount = 1000000;				// This variable must be initialized to avoid an error on the first run

float fDIST_PulseTime = 0;
float fDIST_PulseDistance = 0;
float fDIST_PulseDistanceAdjusted = 0;
float fDIST_DistanceDS1 = 0;

bool bDIST_Done = true;

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
		GPIOPinTypeGPIOOutput(pstSA[i].ui32DIST_SensorPort, pstSA[i].ui32DIST_SensorTrigger);

		// Init Ultrasonic Distance Sensor - Echo
		GPIOPinTypeGPIOInput(pstSA[i].ui32DIST_SensorPort, pstSA[i].ui32DIST_SensorEcho);

		GPIOPadConfigSet(pstSA[i].ui32DIST_SensorPort, pstSA[i].ui32DIST_SensorEcho, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
	}

	// Enable Interrupts

	for(i = 0; i < DIST_NUMBER_OF_SENSORS; i++)
	{
		// Enable interrupts on Rising and Falling edges
		GPIOIntTypeSet(pstSA[i].ui32DIST_SensorPort, (uint8_t)(pstSA[i].ui32DIST_SensorEcho) , GPIO_BOTH_EDGES);

		// Enable Echo interrupts only
		GPIOIntEnable(pstSA[i].ui32DIST_SensorPort, pstSA[i].ui32DIST_SensorEcho);

		// Clear Interrupts
		GPIOIntClear(pstSA[i].ui32DIST_SensorPort, pstSA[i].ui32DIST_SensorEcho);

		// Enable interrupts in the Echo pin
		IntEnable(pstSA[i].ui32DIST_SensorInterrupt);

		// Set interrupt prio
		//IntPrioritySet(pstSA[i].ui32DIST_SensorInterrupt, 0x60);
	}
}

//*****************************************************************************
//	DIST_Measure
//*****************************************************************************

void DIST_Measure(struct DIST_Sensor* stSensorHandle)
{
	if(stSensorHandle == NULL)
	{
#ifdef DEBUG_CB
		// Send error message
		UARTprintf("\nError: Distance Sensor doesn't exist. Check pointer sent to DIST_Measure.\n");
#endif
	}
	else
	{
		// Send Trigger Pulse
		GPIOPinWrite(stSensorHandle->ui32DIST_SensorPort, stSensorHandle->ui32DIST_SensorTrigger, stSensorHandle->ui32DIST_SensorTrigger);

		// Enable Timer 1
		TimerEnable(TIMER1_BASE, TIMER_A);

		// Store Distance Sensor currently in use
		u8DIST_ActiveSensor = stSensorHandle->u8DIST_SensorID;
	}
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
	if(bDIST_Done)
	{
		// Calculate Distance
		fDIST_DistanceDS1 = DIST_CalculateDistance();

		// Separate Distance in 2 integers
		u32DIST_DistDS1Int = (uint32_t)( floor(fDIST_DistanceDS1) );
		u32DIST_DistDS1Frac =  (uint32_t)( (fDIST_DistanceDS1 - u32DIST_DistDS1Int) * 1000000 );

#ifdef DEBUG_CB
		// Print Distance Values
		UARTprintf("\n\n DS1: %d.%d \n", u32DIST_DistDS1Int, u32DIST_DistDS1Frac);
#endif

		// Measure Distance with DS4
		DIST_Measure(&pstSA[3]);

		// Set flag to signal that a measurement is in progress
		bDIST_Done = false;
	}
}

//*****************************************************************************
//	DIST_CalculateDistance
//*****************************************************************************

float DIST_CalculateDistance(void)
{
	// Calculate pulse duration
	fDIST_PulseTime = (float)u64DIST_PulseCount * CYCLE_TIME;

	// Calculate distance from the pulse duration
	fDIST_PulseDistance = fDIST_PulseTime * DIST_SPEED_OF_SOUND_OVER_2;

	// Adjust the data
	//fDIST_PulseDistanceAdjusted = DIST_S1_M*fDIST_PulseDistance + DIST_S1_B;
	fDIST_PulseDistanceAdjusted = fDIST_PulseDistance;

	// Return Result
	return fDIST_PulseDistanceAdjusted;
}

//*****************************************************************************
//	DIST_IntHandler
//*****************************************************************************

void DIST_IntHandler(void)
{
	// Disable Echo interrupt flag
	GPIOIntClear(pstSA[u8DIST_ActiveSensor].ui32DIST_SensorPort, pstSA[u8DIST_ActiveSensor].ui32DIST_SensorEcho);

	if(1 == u8DIST_EdgeCounter)
	{
		// Get timer value
		u32DIST_EndTimer = TimerValueGet(TIMER0_BASE, TIMER_A);
		//u32DIST_EndTimer = HWREG(TIMER0_BASE + TIMER_O_TAR) : HWREG(ui32Base + TIMER_O_TBR);

		// Calculate time
		if(u32DIST_EndTimer > u32DIST_StartTimer)
		{
			u64DIST_PulseCount = DIST_TIMER_SIZE_32 - ( u32DIST_EndTimer - u32DIST_StartTimer) + 4;
		}
		else
		{
			u64DIST_PulseCount = u32DIST_StartTimer - u32DIST_EndTimer + 4;
		}

		// Set flag to signal that measurement is complete
		bDIST_Done = true;

		// Indicate that this is the falling edge
		u8DIST_EdgeCounter = 0;
	}
	else
	{
		// Get Timer value
		u32DIST_StartTimer = TimerValueGet(TIMER0_BASE, TIMER_A);
		//u32DIST_StartTimer = HWREG(TIMER0_BASE + TIMER_O_TAR) : HWREG(ui32Base + TIMER_O_TBR);

		// Signal that this is the rising edge
		u8DIST_EdgeCounter++;
	}
}

