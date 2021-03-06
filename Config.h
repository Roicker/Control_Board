/*
 * Config.h
 *
 *  Created on: 22/09/2014
 *      Author: Ray
 */

#ifndef CONFIG_H_
#define CONFIG_H_

//*****************************************************************************
//
//		Library Header Files
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <math.h>
#include "inc/hw_memmap.h"
#include "inc/hw_ssi.h"
#include "inc/hw_ints.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_timer.h"
#include "driverlib/pin_map.h"
#include "driverlib/fpu.h"

//*****************************************************************************
//
//		Local Header Files
//
//*****************************************************************************

// Drivers
#include "drivers/IMU/IMU.h"
#include "drivers/MPU9250/MPU9250.h"
#include "drivers/MS5611/MS5611.h"
#include "drivers/SPI/SPI.h"
#include "drivers/StepperMotor/STEP.h"
#include "drivers/Distance/DIST.h"
#include "drivers/Timer/TIMER.h"
#include "drivers/Control/CTRL.h"
#include "drivers/Communication/COMM.h"

// Driverlib
#include "driverlib/MPU9250/hw_mpu9250.h"
#include "driverlib/ROM/rom.h"
#include "driverlib/GPIO/gpio.h"
#include "driverlib/SSI/ssi.h"
#include "driverlib/CPU/cpu.h"
#include "driverlib/UART/uart.h"
#include "driverlib/SysCtl/sysctl.h"
#include "driverlib/Interrupt/interrupt.h"
#include "driverlib/timer/timer.h"

// Utils
#include "utils/uartstdio.h"

// Main header
#include "main.h"

//*****************************************************************************
//
//		Definitions
//
//*****************************************************************************

#define SYSCLK						80000000								// System Frequency
#define INTOSCLK					16000000								// Internal Oscillator Frequency
#define MS_CYCLE_COUNT				13320									// Amount of Cycles in a milisecond
#define DELAY_MS(x)					SysCtlDelay(MS_CYCLE_COUNT*x)
#define COMM_RX_BUFFER_SIZE			50										// Amount of characters be stored in RX Buffer
#define CARRIAGE_RETURN				0x0D									// ASCII Value of Enter Key
#define CYCLE_TIME					(float)0.0000000125						// Duration of 1 Clock cycle

#define READ_FLAG   				0x80

#define MASK_LSB_ON					0x01

#define DEBUG_PERIPH				SYSCTL_PERIPH_GPIOA						// Peripheral Debug pins
#define DEBUG_PORT					GPIO_PORTA_BASE							// Port for Debug pins
#define DEBUG_PIN_1					GPIO_PIN_6								// Debug pin 1
#define DEBUG_PIN_2					GPIO_PIN_7								// Debug pin 2

#define _bswap_constant_uint16(x)	x = ((((x) >> 8) & 0xff) | (((x) & 0xff) << 8))	// Byte swap macro

#endif /* CONFIG_H_ */
