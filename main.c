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
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
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
//		Extern Variables
//
//*****************************************************************************

uint8_t ui8UART_RX_String[UART_RX_BUFFER_SIZE];
uint8_t ui8UART_RX_Pointer_End = 0;
uint8_t ui8UART_RX_Pointer_Start = 0;
bool bUART_StringAvailable = false;
bool bUART_RSM_Enabled = false;
bool bUART_LSM_Enabled = false;

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

void UARTIntHandler(void)
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

	// Init RGB LED
	GPIO_Init();

	// Init UART
	UART_Init();

	// Init IMU
	IMU_Init();

	// Init Stepper Motors
	// Left
	STEP_Init(&stLeft_Handle);
	// Right
	STEP_Init(&stRight_Handle);

	return ret;
}

void UART_Init(void)
{
	// Enable Port A
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	// Enable UART 0
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

	// Configure A.0 and A.1 as RX and TX of UART 0
	ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
	ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
	ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Use the Internal Oscillator as the UART clock source.
	ROM_UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

	// Init the UART utilities (UART0, Baudrate 115,200 and UART CLK = Internal Oscillator)
	UARTStdioConfig(0, 115200, INTOSCLK);

	// Enable processor interrupts
	IntMasterEnable();

	// Enable the UART interrupt
	IntEnable(INT_UART0);

	// Enable RX and TX interrupts only
	UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
}

void GPIO_Init(void)
{
	// RGB LED Configuration

	// Enable Port F
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

	// Set Pins F.1, F.2 and F.3 as Outputs on the APB Bus (Normal Performance)
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
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

//*****************************************************************************
//
//		Main Function
//
//*****************************************************************************

int main(void)
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
	UARTprintf("\nSelect desired command:\n 1 - Enable/Disable Right Stepper\n 2 - Enable/Disable Left Stepper\n 3 - Reverse Right Stepper\n 4 - Reverse Left Stepper\n 5 - Read RAW IMU Values\n\nCommand: ");

	while(1)
	{
		// If characters are available, call function to process the data
		if(bUART_StringAvailable)
		{
			// UART Function that processes the received command
			UART_Comm();
		}

		// Cyclic operations

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

	// Unreachable return
	return 0;
}
