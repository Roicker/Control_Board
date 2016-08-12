//*****************************************************************************
//
// main.c - Main program entry point and initialization
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

#define TARGET_IS_BLIZZARD_RB1

//*****************************************************************************
//
//		Header files
//
//*****************************************************************************

#include "main.h"

//*****************************************************************************
//
//		Global Variables
//
//*****************************************************************************

#ifdef DEBUG_CB_CONSOLE
// Variable to store status of debug pin
bool TIMER_bTimer2Toggle = false;
#endif

//*****************************************************************************
//
//		Extern Variables
//
//*****************************************************************************

// Structures to hold Stepper Motor Handles
extern struct STEP_Motor stLeft_Handle;
extern struct STEP_Motor stRight_Handle;

// Structures to hold sensor data
extern struct MPU9250 stMPU9250_Values[2];

// Calibration flags
extern bool bAccGyro_Calibrated;
extern bool bMag_Calibrated;

// Variable to store which stMPU9250_Values is currently head
extern uint8_t ui8MPU9250_RBHead;

// Variable to store which stMPU9250_Values is currently tail
extern uint8_t ui8MPU9250_RBTail;

// Variable to store how many stMPU9250_Values entries are filled
extern uint8_t ui8MPU9250_RBCount;

// Variable for 10 mSec Container
extern bool C_10mS;

// Variable for 100 uSec Container
extern bool C_100uS;

// Comm variables
extern bool bCOMM_StringAvailable;

//*****************************************************************************
//
//		Functions
//
//*****************************************************************************

//*****************************************************************************
//		Main Function
//*****************************************************************************

void main(void)
{
	if ( false == SYS_Init() )
	{
#ifdef DEBUG_CB_CONSOLE
		// Send error message
		UARTprintf("\nError: Init failed!\n");
#endif
		// Enter eternal while loop to signal error
		while(1)
		{
			// Error! Blink RED Led
			ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);
			// Delay for 250 ms
			DELAY_MS(500);
			// Error! Blink RED Led
			ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0);
			// Delay for 1500 ms
			DELAY_MS(3000);
		}
	}
	else
	{
#ifdef DEBUG_CB_CONSOLE
		// Send success message
		UARTprintf("\n\nInit Successful!\n");
#endif
	}
#ifdef DEBUG_CB_CONSOLE
	// Init Console
	UARTprintf("\nSelect desired command:\n 1 - Enable/Disable Right Stepper\n 2 - Enable/Disable Left Stepper\n 3 - Reverse Right Stepper\n 4 - Reverse Left Stepper");
	UARTprintf("\n 5 - Read RAW IMU Values\n 6 - Enable SM Driver\n 7 - Disable SM Driver\n 8 - Read Distance\n 9 - Calibrate Accel and Gyro\n A - Calibrate Mag\n\nCommand: ");
#endif

	// Main Loop
	while(1)
	{
#ifdef DEBUG_CB_CONSOLE
		// Call test routine
		TEST_main();
#endif
		// Normal board operation routines

		// 100 uSec Container
		if(true == C_100uS)
		{
			//////// 100 uSec Container ////////

			// Nothing to do here - delete it? if so, remove/disable Timer 3 implementation

			////////////////////////////////////

			// Reset container flag
			C_100uS = false;
		}

		// 10 mSec Container
		if(true == C_10mS)
		{

#ifdef DEBUG_CB_CONSOLE
			// Signal start of cycle
			GPIOPinWrite(DEBUG_PORT, DEBUG_PIN_2, DEBUG_PIN_2);
#endif

			//////// 10 mSec Container ////////

			// Update orientation
			IMU_Cycle();

			// Run Control Loop (where the magic happens)
			CTRL_ControlLoop();

			///////////////////////////////////

			// Reset container flag
			C_10mS = false;

#ifdef DEBUG_CB_CONSOLE
			// Signal end of cycle
			GPIOPinWrite(DEBUG_PORT, DEBUG_PIN_2, 0);
#endif

#ifdef DEBUG_CB_CONSOLE
			// Toggle Debug Pin 1 to test container cycle
			if(false == TIMER_bTimer2Toggle)
			{
				// Set Debug pin 1 to measure excecution time
				GPIOPinWrite(DEBUG_PORT, DEBUG_PIN_1, DEBUG_PIN_1);

				// Toggle variable
				TIMER_bTimer2Toggle = true;
			}
			else
			{
				// Clear Debug pin 1 to measure excecution time
				GPIOPinWrite(DEBUG_PORT, DEBUG_PIN_1, 0);

				// Toggle variable
				TIMER_bTimer2Toggle = false;
			}
#endif
		}

		//////// Background Tasks ////////

		// Process serial communication //

		// If characters are available, call function to process the data
		if(bCOMM_StringAvailable)
		{
			// UART Function that processes the received command
			COMM_CommProcess();
		}

		//////////////////////////////////
	}
}

//*****************************************************************************
//		SYS_Init
//*****************************************************************************

bool SYS_Init(void)
{
	// Create ret variable and initialize it
	bool ret = true;

	// Disable processor interrupts
	ROM_IntMasterDisable();

	// Enable FPU
	ROM_FPULazyStackingEnable();
	ROM_FPUEnable();

	// Run at 80Mhz
	ROM_SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

	// Init UART - Must be the first initialization in order for Error messages to be sent if necessary
	COMM_Init();

	//// Init Stepper Motors ////

	// Left
	STEP_Init(&stLeft_Handle);
	// Set direction forward
	STEP_SetDirection(&stLeft_Handle, true);
	// Set Initial Speed
	//STEP_SetSpeed(&stLeft_Handle, 9500);

	// Right
	STEP_Init(&stRight_Handle);
	// Set direction forward
	STEP_SetDirection(&stRight_Handle, true);
	// Set Initial Speed
	//STEP_SetSpeed(&stRight_Handle, 1);

	////////////////////////////

	// Init RGB LED
	LED_Init();

	// Init IMU
	IMU_Init();

	// Init Ultrasonic Distance Sensors
	DIST_Init();

	// Init Timers
	TIMER_Init();

	// Init Controllers
	CTRL_Init();

#ifdef DEBUG_CB_CONSOLE
	// Init debug pins
	DEBUG_Init();
#endif

	// Enable processor interrupts
	IntMasterEnable();

	return ret;
}

//*****************************************************************************
//		LED_Init
//*****************************************************************************

void LED_Init(void)
{
	// RGB LED Configuration

	// Enable Port F
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

	// Set Pins F.1, F.2 and F.3 as Outputs on the APB Bus (Normal Performance)
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
}

//*****************************************************************************
//		DEBUG_Init
//*****************************************************************************

void DEBUG_Init()
{
	// Enable Port for debug pins
	SysCtlPeripheralEnable(DEBUG_PERIPH);

	// Init debug pin 1
	GPIOPinTypeGPIOOutput(DEBUG_PORT, DEBUG_PIN_1);

	// Init debug pin 2
	GPIOPinTypeGPIOOutput(DEBUG_PORT, DEBUG_PIN_2);

	// Reset Debug pin 1
	GPIOPinWrite(DEBUG_PORT, DEBUG_PIN_1, 0);

	// Reset Debug pin 2
	GPIOPinWrite(DEBUG_PORT, DEBUG_PIN_2, 0);
}

//*****************************************************************************
//		UTILS_Memcpy
//*****************************************************************************

uint8_t * UTILS_Memcpy(uint8_t * pDestination, const uint32_t * cpSource, uint32_t u32Length)
{
	// Variable used in for loop
	uint32_t u32Counter = 0;

	// Copy memory from cpSource to pDestination
    for(u32Counter = 0; u32Counter < u32Length; u32Counter++)
    {
    	pDestination[u32Counter] = cpSource[u32Counter];
    }

    // Returns address of the first byte after the destination buffer
    return &pDestination[u32Counter];
}

//*****************************************************************************
//		UTILS_Memset
//*****************************************************************************

void UTILS_Memset(uint8_t * pDestination, uint8_t u8Value, uint32_t u32Length)
{
	// Variable used in for loop
	uint32_t u32Counter = 0;

	// Set memory on pDestination
    for(u32Counter = 0; u32Counter < u32Length; u32Counter++)
    {
    	pDestination[u32Counter] = u8Value;
    }
}

//*****************************************************************************
//		UTILS_Wordswap
//*****************************************************************************
//
//void UTILS_Wordswap(uint16_t * pu16Input, uint32_t u32Length)
//{
//	// Variable used in for loop
//	uint32_t u32Counter = 0;
//
//	// Temporary variable for swap
//	uint8_t u8MSB = 0;
//
//	// u8 Pointer for swap
//
//	// Swap Bytes in Word
//    for(u32Counter = 0; u32Counter < u32Length; u32Counter++)
//    {
//
//    	u8MSB = ()&pu16Input >> 8)
//    }
//}
