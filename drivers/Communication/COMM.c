//*****************************************************************************
//
// COMM.c - Control functions
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

#include "drivers/Communication/COMM.h"

//*****************************************************************************
//
//		Global Variables
//
//*****************************************************************************

// Variables to create circular buffer
uint8_t ui8COMM_RX_String[COMM_RX_BUFFER_SIZE];
uint8_t ui8COMM_RX_Pointer_End = 0;
uint8_t ui8COMM_RX_Pointer_Start = 0;

// Support variables
bool bCOMM_StringAvailable = false;
bool bCOMM_RSM_Enabled = false;
bool bCOMM_LSM_Enabled = false;
char cCOMMDataTX = 0;
char cCOMMDataRX = 0;

//*****************************************************************************
//
//		Extern Variables
//
//*****************************************************************************

// External variable that contains the Robot Inclination Angle
extern volatile float fex;

// Controller structures
extern struct CTRL_Simple_PID_Data CTRL_RobotAngle;
extern struct CTRL_PID_Data CTRL_RobotSpeed;

// Integer representation of euler angles
extern volatile int16_t ex, ey, ez;

//*****************************************************************************
//
//		Interrupt Handlers
//
//*****************************************************************************

//*****************************************************************************
//		COMM_IntHandler
//*****************************************************************************
void COMM_IntHandler()
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
		ui8COMM_RX_String[ui8COMM_RX_Pointer_End] = UARTCharGetNonBlocking(UART0_BASE);

#ifdef DEBUG_CB_CONSOLE
		// Echo character
		UARTCharPutNonBlocking(UART0_BASE, ui8COMM_RX_String[ui8COMM_RX_Pointer_End]);

		// Check if an "Enter" was pressed
		if( CARRIAGE_RETURN == ui8COMM_RX_String[ui8COMM_RX_Pointer_End] )
		{
			// Signal that a string is available
			bCOMM_StringAvailable = true;
		}
#else
		// Notify that a data has been received
		bCOMM_StringAvailable = true;
#endif

		// Increase pointer
		ui8COMM_RX_Pointer_End++;

		// Restore pointer if it's at the end of the circular buffer
		if(ui8COMM_RX_Pointer_End == COMM_RX_BUFFER_SIZE)
		{
			ui8COMM_RX_Pointer_End = 0;
		}
	}
}

//*****************************************************************************
//
//		Functions
//
//*****************************************************************************

//*****************************************************************************
//		COMM_Init
//*****************************************************************************
void COMM_Init()
{
	// Enable Port A
	//ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	// Enable Port B
	//ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

	// Enable Port E
	//ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

	// Enable UART 0
	//ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

	// Enable UART 1
	//ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);

	// Enable UART 5
	//ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART5);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART5);

	// Configure B.0 and B.1 as RX and TX of UART 1
//	ROM_GPIOPinConfigure(GPIO_PB0_U1RX);
//	ROM_GPIOPinConfigure(GPIO_PB1_U1TX);
//	ROM_GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	GPIOPinConfigure(GPIO_PB0_U1RX);
	GPIOPinConfigure(GPIO_PB1_U1TX);
	GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Use the Internal Oscillator as the UART clock source.
	//ROM_UARTClockSourceSet(UART1_BASE, UART_CLOCK_PIOSC);
	UARTClockSourceSet(UART1_BASE, UART_CLOCK_PIOSC);

	// Init the UART utilities (UART1, Baudrate 115,200 and UART CLK = Internal Oscillator)
	UARTStdioConfig(1, 115200, INTOSCLK);

	// Configure E.4 and E.5 as RX and TX of UART 5
	//ROM_GPIOPinConfigure(GPIO_PE4_U5RX);
	//ROM_GPIOPinConfigure(GPIO_PE5_U5TX);
	//ROM_GPIOPinTypeUART(GPIO_PORTE_BASE, GPIO_PIN_4 | GPIO_PIN_5);
	GPIOPinConfigure(GPIO_PE4_U5RX);
	GPIOPinConfigure(GPIO_PE5_U5TX);
	GPIOPinTypeUART(GPIO_PORTE_BASE, GPIO_PIN_4 | GPIO_PIN_5);

    // Use the Internal Oscillator as the UART clock source.
	//ROM_UARTClockSourceSet(UART5_BASE, UART_CLOCK_PIOSC);
	UARTClockSourceSet(UART5_BASE, UART_CLOCK_PIOSC);

	// Init the UART utilities (UART5, Baudrate 115,200 and UART CLK = Internal Oscillator)
	UARTStdioConfig(5, 115200, INTOSCLK);

	// IMPORTANT! UART 0 Must be the last one to be configured in order for UARTPrintf to be directed to UART 0

	// Configure A.0 and A.1 as RX and TX of UART 0
	//ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
	//ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
	//ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	// Use the Internal Oscillator as the UART clock source.
	//ROM_UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
	UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

	// Init the UART utilities (UART0, Baudrate 115,200 and UART CLK = Internal Oscillator)
	UARTStdioConfig(0, 115200, INTOSCLK);

	// Enable the UART interrupt
	IntEnable(INT_UART0);

	// Set interrupt prio
	//IntPrioritySet(IMU_INTPIN_INT, 0x80);

	// Enable RX and TX interrupts only
	UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
}

//*****************************************************************************
//		COMM_CommProcess
//*****************************************************************************
void COMM_CommProcess()
{
	// Local Variables
	uint8_t firstint, firstdec = 0;

	// Read command and execute desired operations
	switch(ui8COMM_RX_String[ui8COMM_RX_Pointer_Start])
	{
		// Send New Data - "." Received
	case 0x2E:
		// Send the current angles
		UARTprintf("%d, %d, %d\n",ex,ey,ez);
		// Prepare for next command
		ui8COMM_RX_Pointer_Start = ui8COMM_RX_Pointer_End;
		break;
		// Send Control Data - "p" Received
	case 0x70:
		// Send data
		COMM_SendFloat(fex);
		UARTprintf(", ");
		COMM_SendFloat(CTRL_RobotAngle.yk);
		UARTprintf("\n");

		// Prepare for next command
		ui8COMM_RX_Pointer_Start = ui8COMM_RX_Pointer_End;
		break;
		// Set Kp parameter - 'S' Received
	case 0x53:
		// Increase ui8COMM_RX_Pointer_Start to get next char
		COMM_GetNextChar(&ui8COMM_RX_Pointer_Start);

		// Store the received data
		firstint = ui8COMM_RX_String[ui8COMM_RX_Pointer_Start];

		// Increase ui8COMM_RX_Pointer_Start to get next char
		COMM_GetNextChar(&ui8COMM_RX_Pointer_Start);

		// Store the received data
		firstdec = ui8COMM_RX_String[ui8COMM_RX_Pointer_Start];

		// Store data in Kp
		CTRL_RobotAngle.Kp = 0;
		CTRL_RobotAngle.Kp = firstint + firstdec/100.0;

		// Prepare for next command
		ui8COMM_RX_Pointer_Start = ui8COMM_RX_Pointer_End;
		break;
		// Set Ki parameter - 'T' Received
	case 0x54:
		// Increase ui8COMM_RX_Pointer_Start to get next char
		COMM_GetNextChar(&ui8COMM_RX_Pointer_Start);

		// Store the received data
		firstint = ui8COMM_RX_String[ui8COMM_RX_Pointer_Start];

		// Increase ui8COMM_RX_Pointer_Start to get next char
		COMM_GetNextChar(&ui8COMM_RX_Pointer_Start);

		// Store the received data
		firstdec = ui8COMM_RX_String[ui8COMM_RX_Pointer_Start];

		// Store data in Ki
		CTRL_RobotAngle.Ki = 0;
		CTRL_RobotAngle.Ki = firstint + firstdec/100.0;

		// Prepare for next command
		ui8COMM_RX_Pointer_Start = ui8COMM_RX_Pointer_End;
		break;
		// Set Kd parameter - 'U' Received
	case 0x55:
		// Increase ui8COMM_RX_Pointer_Start to get next char
		COMM_GetNextChar(&ui8COMM_RX_Pointer_Start);

		// Store the received data
		firstint = ui8COMM_RX_String[ui8COMM_RX_Pointer_Start];

		// Increase ui8COMM_RX_Pointer_Start to get next char
		COMM_GetNextChar(&ui8COMM_RX_Pointer_Start);

		// Store the received data
		firstdec = ui8COMM_RX_String[ui8COMM_RX_Pointer_Start];

		// Store data in Kd
		CTRL_RobotAngle.Kd = 0;
		CTRL_RobotAngle.Kd = firstint + firstdec/100.0;

		// Prepare for next command
		ui8COMM_RX_Pointer_Start = ui8COMM_RX_Pointer_End;
		break;
		// 'z' Received
	case 0x7A:
		// Do nothing
		// Prepare for next command
		ui8COMM_RX_Pointer_Start = ui8COMM_RX_Pointer_End;
		break;
#ifdef DEBUG_CB_CONSOLE
		// Command 1
	case 0x31:
		// Enable Right Stepper Motor
		bCOMM_RSM_Enabled = !bCOMM_RSM_Enabled;
		// New line
		UARTprintf("\nCommand: ");
		// Prepare for next command
		ui8COMM_RX_Pointer_Start = ui8COMM_RX_Pointer_End;
		break;
		// Command 2
	case 0x32:
		// Enable Left Stepper Motor
		bCOMM_LSM_Enabled = !bCOMM_LSM_Enabled;
		// New line
		UARTprintf("\nCommand: ");
		// Prepare for next command
		ui8COMM_RX_Pointer_Start = ui8COMM_RX_Pointer_End;
		break;
		// Command 3
	case 0x33:
		// Change direction of Right Stepper Motor
		STEP_ChangeDirection(&stRight_Handle);
		// New line
		UARTprintf("\nCommand: ");
		// Prepare for next command
		ui8COMM_RX_Pointer_Start = ui8COMM_RX_Pointer_End;
		break;
		// Command 4
	case 0x34:
		// Change direction of Left Stepper Motor
		STEP_ChangeDirection(&stLeft_Handle);
		// New line
		UARTprintf("\nCommand: ");
		// Prepare for next command
		ui8COMM_RX_Pointer_Start = ui8COMM_RX_Pointer_End;
		break;
		// Command 5
	case 0x35:
		// Read RAW IMU Values and send them to the console
		IMU_UpdateValues();
		// New line
		UARTprintf("\nCommand: ");
		// Prepare for next command
		ui8COMM_RX_Pointer_Start = ui8COMM_RX_Pointer_End;
		break;
		// Command 6
	case 0x36:
		// Enable Stepper Motors
		STEP_Enable();
		// New line
		UARTprintf("\nCommand: ");
		// Prepare for next command
		ui8COMM_RX_Pointer_Start = ui8COMM_RX_Pointer_End;
		break;
		// Command 7
	case 0x37:
		// Disable Stepper Motors
		STEP_Disable();
		// New line
		UARTprintf("\nCommand: ");
		// Prepare for next command
		ui8COMM_RX_Pointer_Start = ui8COMM_RX_Pointer_End;
		break;
		// Command 8
	case 0x38:
		// Measure distance
		DIST_UpdateDistance();
		// New line
		UARTprintf("\nCommand: ");
		// Prepare for next command
		ui8COMM_RX_Pointer_Start = ui8COMM_RX_Pointer_End;
		break;
		// Command 9
	case 0x39:
		// Calibrate Accel and Gyro
		bAccGyro_Calibrated = false;
		// New line
		UARTprintf("\nCommand: ");
		// Prepare for next command
		ui8COMM_RX_Pointer_Start = ui8COMM_RX_Pointer_End;
		break;
		// Command A
	case 0x41:
		// Calibrate Mag
		bMag_Calibrated = false;
		// New line
		UARTprintf("\nCommand: ");
		// Prepare for next command
		ui8COMM_RX_Pointer_Start = ui8COMM_RX_Pointer_End;
		break;
		// Command CR
	case CARRIAGE_RETURN:
		// New line
		UARTprintf("\nCommand: ");
		// Prepare for next command
		ui8COMM_RX_Pointer_Start = ui8COMM_RX_Pointer_End;
		break;
#endif
	default:
#ifdef DEBUG_CB_CONSOLE
		UARTprintf("\nError: Command "" not recognized! Please try again\nCommand: ");
#endif
		// Prepare for next command
		ui8COMM_RX_Pointer_Start = ui8COMM_RX_Pointer_End;
		break;
	}

	bCOMM_StringAvailable = false;
}

//*****************************************************************************
//		COMM_GetNextChar
//*****************************************************************************
void COMM_GetNextChar(uint8_t *ui8BufferCounter)
{
	// Restore circular array counter if it's at the end of the circular buffer
	if(*ui8BufferCounter == COMM_RX_BUFFER_SIZE)
	{
		*ui8BufferCounter = 0;
	//		CTRL_RobotAngle
	}
	else
	{
		// Increase circular array counter
		*ui8BufferCounter = *ui8BufferCounter + 1;
	}
}

//*****************************************************************************
//		COMM_SendFloat
//*****************************************************************************
void COMM_SendFloat(float fNumber)
{
	// Local variables
	int16_t di = 0;
	uint32_t dr = 0;

	// Store integer and decimal numbers
	di = (int16_t)(fNumber);
	dr = (uint32_t)( fabsf((fNumber - di) * IMU_DECIMALS_FACTOR ));

	// Check if the number is -0.xxxxx
	if( (fNumber < 0) && (0 == di) )
	{
		// Send the value with a negative sign
		UARTprintf("-%d.%04u",di,dr);
	}
	else // number is positive
	{
		// Send the value
		UARTprintf("%d.%04u",di,dr);
	}
}
