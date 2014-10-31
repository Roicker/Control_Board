//*****************************************************************************
//
// main.c - Main program entry point and initialization
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

uint8_t ui8UART_RX_String[UART_RX_BUFFER_SIZE];
uint8_t ui8UART_RX_Pointer_End = 0;
uint8_t ui8UART_RX_Pointer_Start = 0;
bool bUART_StringAvailable = false;
bool bUART_RSM_Enabled = false;
bool bUART_LSM_Enabled = false;
char cUARTDataTX = 0;
char cUARTDataRX = 0;

//*****************************************************************************
//
//		Extern Variables
//
//*****************************************************************************

extern struct STEP_Motor stLeft_Handle;
extern struct STEP_Motor stRight_Handle;

//*****************************************************************************
//
//		Interrupt Handlers
//
//*****************************************************************************

void UART_IntHandler(void)
{
	// Variable to store interrupt status
	uint32_t ui32Status;

	// Get interrupt status
	ui32Status = UARTIntStatus(UART0_BASE, true);

	// Clear the asserted interrupts
	UARTIntClear(UART0_BASE, ui32Status);

	// Loop while there are chars in the RX FIFO
	while(UARTCharsAvail(UART0_BASE))
	{
		// Store Char in Buffer
		ui8UART_RX_String[ui8UART_RX_Pointer_End] = UARTCharGetNonBlocking(UART0_BASE);

		// Echo character
		UARTCharPutNonBlocking(UART0_BASE, ui8UART_RX_String[ui8UART_RX_Pointer_End]);

		// Check if an "Enter" was pressed
		if( CARRIAGE_RETURN == ui8UART_RX_String[ui8UART_RX_Pointer_End] )
		{
			// Signal that a string is available
			bUART_StringAvailable = true;
		}

		// Increase pointer
		ui8UART_RX_Pointer_End++;

		// Restore pointer if it's at the end of the circular buffer
		if(ui8UART_RX_Pointer_End == UART_RX_BUFFER_SIZE)
		{
			ui8UART_RX_Pointer_End = 0;
		}
	}
}

//*****************************************************************************
//
//		Init Functions
//
//*****************************************************************************

bool SYS_Init(void)
{
	// Create ret variable and initialize it
	bool ret = true;

	// Run at 80Mhz
	ROM_SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

	// Init UART - Must be the first initialization in order for Error messages to be sent if necessary
	UART_Init();

	// Init Stepper Motors
	// Left
	STEP_Init(&stLeft_Handle);
	// Right
	STEP_Init(&stRight_Handle);

#ifdef DEBUG_CB
	// Disable Stepper Motor Driver
	STEP_Disable();
#endif

	// Init RGB LED
	LED_Init();

	// Init IMU
	IMU_Init();

	// Init Ultrasonic Distance Sensors
	DIST_Init();

	// Init Timers
	TIMER_Init();

	// Enable processor interrupts
	IntMasterEnable();

	return ret;
}

void UART_Init(void)
{
	// Enable Port A
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	// Enable Port B
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

	// Enable Port E
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

	// Enable UART 0
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

	// Enable UART 1
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);

	// Enable UART 5
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART5);

	// Configure A.0 and A.1 as RX and TX of UART 0
	ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
	ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
	ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Use the Internal Oscillator as the UART clock source.
	ROM_UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

	// Init the UART utilities (UART0, Baudrate 115,200 and UART CLK = Internal Oscillator)
	UARTStdioConfig(0, 115200, INTOSCLK);

	// Configure B.0 and B.1 as RX and TX of UART 1
	ROM_GPIOPinConfigure(GPIO_PB0_U1RX);
	ROM_GPIOPinConfigure(GPIO_PB1_U1TX);
	ROM_GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Use the Internal Oscillator as the UART clock source.
	ROM_UARTClockSourceSet(UART1_BASE, UART_CLOCK_PIOSC);

	// Init the UART utilities (UART1, Baudrate 115,200 and UART CLK = Internal Oscillator)
	UARTStdioConfig(1, 115200, INTOSCLK);

	// Configure E.4 and E.5 as RX and TX of UART 5
	ROM_GPIOPinConfigure(GPIO_PE4_U5RX);
	ROM_GPIOPinConfigure(GPIO_PE5_U5TX);
	ROM_GPIOPinTypeUART(GPIO_PORTE_BASE, GPIO_PIN_4 | GPIO_PIN_5);

    // Use the Internal Oscillator as the UART clock source.
	ROM_UARTClockSourceSet(UART5_BASE, UART_CLOCK_PIOSC);

	// Init the UART utilities (UART5, Baudrate 115,200 and UART CLK = Internal Oscillator)
	UARTStdioConfig(5, 115200, INTOSCLK);

	// Enable the UART interrupt
	IntEnable(INT_UART0);

	// Enable RX and TX interrupts only
	UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
}

void LED_Init(void)
{
	// RGB LED Configuration

	// Enable Port F
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

	// Set Pins F.1, F.2 and F.3 as Outputs on the APB Bus (Normal Performance)
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
}

//*****************************************************************************
//
//		Main Function
//
//*****************************************************************************

void main(void)
{
	if ( false == SYS_Init() )
	{
#ifdef DEBUG_CB
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
#ifdef DEBUG_CB
		// Send success message
		UARTprintf("\nInit Successful!\n");
#endif
	}

	// Init Console
	UARTprintf("\nSelect desired command:\n 1 - Enable/Disable Right Stepper\n 2 - Enable/Disable Left Stepper\n 3 - Reverse Right Stepper\n 4 - Reverse Left Stepper\n 5 - Read RAW IMU Values\n 6 - Enable SM Driver\n 7 - Disable SM Driver\n\nCommand: ");

	while(1)
	{
		// If characters are available, call function to process the data
		if(bUART_StringAvailable)
		{
			// UART Function that processes the received command
			UART_Comm();
		}

/*		cUARTDataTX = 0;

		UARTCharPutNonBlocking(UART1_BASE, cUARTDataTX);

		cUARTDataRX = UARTCharGetNonBlocking(UART5_BASE);

		if( 0 == cUARTDataRX )
		{
			// Blink LED - Blue
			ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
		}

		DELAY_MS(1000);

		cUARTDataTX = 1;

		UARTCharPutNonBlocking(UART5_BASE, cUARTDataTX);

		cUARTDataRX = UARTCharGetNonBlocking(UART1_BASE);

		if( 1 == cUARTDataRX )
		{
			// Blink LED - Green?
			ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
		}

		DELAY_MS(1000);*/

		// Cyclic operations

		// Measure distance
		DIST_UpdateDistance();

		DELAY_MS(1000);

		// Right Stepper Motor
		if(bUART_RSM_Enabled)
		{
			// Move 1 Step
			STEP_Move(&stRight_Handle);
		}

		// Left Stepper Motor
		if(bUART_LSM_Enabled)
		{
			// Move 1 Step
			STEP_Move(&stLeft_Handle);
		}

		// Delay
		SysCtlDelay(STEP_DELAY);
	}
}

//*****************************************************************************
//
//		UART Communication
//
//*****************************************************************************

void UART_Comm()
{
	// Read command and execute desired operations
	switch(ui8UART_RX_String[ui8UART_RX_Pointer_Start])
	{
	// Command 1
	case 0x31:
		// Enable Right Stepper Motor
		bUART_RSM_Enabled = !bUART_RSM_Enabled;
		// New line
		UARTprintf("\nCommand: ");
		// Prepare for next command
		ui8UART_RX_Pointer_Start = ui8UART_RX_Pointer_End;
		break;
		// Command 2
	case 0x32:
		// Enable Left Stepper Motor
		bUART_LSM_Enabled = !bUART_LSM_Enabled;
		// New line
		UARTprintf("\nCommand: ");
		// Prepare for next command
		ui8UART_RX_Pointer_Start = ui8UART_RX_Pointer_End;
		break;
		// Command 3
	case 0x33:
		// Change direction of Right Stepper Motor
		STEP_ChangeDirection(&stRight_Handle);
		// New line
		UARTprintf("\nCommand: ");
		// Prepare for next command
		ui8UART_RX_Pointer_Start = ui8UART_RX_Pointer_End;
		break;
		// Command 4
	case 0x34:
		// Change direction of Left Stepper Motor
		STEP_ChangeDirection(&stLeft_Handle);
		// New line
		UARTprintf("\nCommand: ");
		// Prepare for next command
		ui8UART_RX_Pointer_Start = ui8UART_RX_Pointer_End;
		break;
		// Command 5
	case 0x35:
		// Read RAW IMU Values and send them to the console
		IMU_UpdateValues();
		// New line
		UARTprintf("\nCommand: ");
		// Prepare for next command
		ui8UART_RX_Pointer_Start = ui8UART_RX_Pointer_End;
		break;
		// Command 6
	case 0x36:
		// Enable Stepper Motors
		STEP_Enable();
		// New line
		UARTprintf("\nCommand: ");
		// Prepare for next command
		ui8UART_RX_Pointer_Start = ui8UART_RX_Pointer_End;
		break;
		// Command 7
	case 0x37:
		// Disable Stepper Motors
		STEP_Disable();
		// New line
		UARTprintf("\nCommand: ");
		// Prepare for next command
		ui8UART_RX_Pointer_Start = ui8UART_RX_Pointer_End;
		break;
		// Command CR
	case CARRIAGE_RETURN:
		// New line
		UARTprintf("\nCommand: ");
		// Prepare for next command
		ui8UART_RX_Pointer_Start = ui8UART_RX_Pointer_End;
		break;
	default:
		UARTprintf("\nError: Command "" not recognized! Please try again\nCommand: ");
		// Prepare for next command
		ui8UART_RX_Pointer_Start = ui8UART_RX_Pointer_End;
		break;
	}

	bUART_StringAvailable = false;
}
