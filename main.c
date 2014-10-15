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

	while(1)
	{
		IMU_UpdateValues();
		DELAY_MS(1000);
	}

	// Unreachable return
	return 0;
}
