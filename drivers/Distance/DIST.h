/*
 * DIST.h
 *
 *  Created on: 29/10/2014
 *      Author: Ray
 */

#ifndef DIST_H_
#define DIST_H_

//*****************************************************************************
//
//		Local Header Files
//
//*****************************************************************************

#include "Config.h"

//*****************************************************************************
//
//		Definitions
//
//*****************************************************************************

#define DIST_S1_PERIPH							SYSCTL_PERIPH_GPIOD						// Intentify on which Peripheral is the control of the Ultrasonic Distance Sensor 1
#define DIST_S1_PORT							GPIO_PORTD_BASE							// Intentify on which Port is the control of the Ultrasonic Distance Sensor 1
#define DIST_S1_TRIGGER							GPIO_PIN_6								// Intentify on which Pin is the control of the Ultrasonic Distance Sensor 1
#define DIST_S1_ECHO							GPIO_PIN_7								// Intentify on which Pin is the control of the Ultrasonic Distance Sensor 1
#define DIST_S1_INT								INT_GPIOD								// Intentify on which Peripheral is the interrupt of the Ultrasonic Distance Sensor 1
#define DIST_S1_M								1.0238									// m Parameter for linear adjustment (y = m*x + b)
#define DIST_S1_B								0.0024									// b Parameter for linear adjustment (y = m*x + b)

#define DIST_S2_PERIPH							SYSCTL_PERIPH_GPIOB						// Intentify on which Peripheral is the control of the Ultrasonic Distance Sensor 2
#define DIST_S2_PORT							GPIO_PORTB_BASE							// Intentify on which Port is the control of the Ultrasonic Distance Sensor 2
#define DIST_S2_TRIGGER							GPIO_PIN_7								// Intentify on which Pin is the direction control of the Ultrasonic Distance Sensor 2
#define DIST_S2_ECHO							GPIO_PIN_6								// Intentify on which Pin is the step control of the Ultrasonic Distance Sensor 2
#define DIST_S2_INT								INT_GPIOB								// Intentify on which Peripheral is the interrupt of the Ultrasonic Distance Sensor 2

#define DIST_S3_PERIPH							SYSCTL_PERIPH_GPIOA						// Intentify on which Peripheral is the control of the Ultrasonic Distance Sensor 3
#define DIST_S3_PORT							GPIO_PORTA_BASE							// Intentify on which Port is the control of the Ultrasonic Distance Sensor 3
#define DIST_S3_TRIGGER							GPIO_PIN_3								// Intentify on which Pin is the direction control of the Ultrasonic Distance Sensor 3
#define DIST_S3_ECHO							GPIO_PIN_2								// Intentify on which Pin is the step control of the Ultrasonic Distance Sensor 3
#define DIST_S3_INT								INT_GPIOA								// Intentify on which Peripheral is the interrupt of the Ultrasonic Distance Sensor 3

#define DIST_S4_PERIPH							SYSCTL_PERIPH_GPIOC						// Intentify on which Peripheral is the control of the Ultrasonic Distance Sensor 4
#define DIST_S4_PORT							GPIO_PORTC_BASE							// Intentify on which Port is the control of the Ultrasonic Distance Sensor 4
#define DIST_S4_TRIGGER							GPIO_PIN_6								// Intentify on which Pin is the direction control of the Ultrasonic Distance Sensor 4
#define DIST_S4_ECHO							GPIO_PIN_7								// Intentify on which Pin is the step control of the Ultrasonic Distance Sensor 4
#define DIST_S4_INT								INT_GPIOC								// Intentify on which Peripheral is the interrupt of the Ultrasonic Distance Sensor 4

#define DIST_NUMBER_OF_SENSORS					4										// Number of Ultrasonic Sensors Connected

#define DIST_TIMER_SIZE_32						4294967295								// Amount of counts in a 32 Bit Timer

#define DIST_SPEED_OF_SOUND_OVER_2				171										// The result of the speed of sound / 2 = 343 / 2 = 171 (rounded down)

//*****************************************************************************
//
//		Structures
//
//*****************************************************************************

struct DIST_Sensor
{
	const uint32_t ui32DIST_BaseAddress;
	const uint32_t ui32DIST_SensorPort;
	const uint32_t ui32DIST_SensorTrigger;
	const uint32_t ui32DIST_SensorEcho;
	const uint32_t ui32DIST_SensorInterrupt;
	const uint8_t  u8DIST_SensorID;
};

//*****************************************************************************
//
//		Function Prototypes
//
//*****************************************************************************

void DIST_Init(void);
void DIST_Measure(struct DIST_Sensor* stSensorHandle);
void DIST_ResetTrigger(void);
void DIST_UpdateDistance(void);
void DIST_IntHandler(void);
float DIST_CalculateDistance(void);

#endif /* DIST_H_ */
