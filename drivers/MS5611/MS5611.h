/*
 * MS5611.h
 *
 *  Created on: 09/10/2014
 *      Author: Ray
 */

#ifndef MS5611_H_
#define MS5611_H_

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

// CS Line Config for MS5611
#define CS_MS5611_PERIPH					SYSCTL_PERIPH_GPIOB						// Intentify on which Peripheral is the CS line of the MS5611
#define CS_MS5611_PORT						GPIO_PORTB_BASE							// Intentify on which Port is the CS line of the MS5611
#define CS_MS5611_PIN						GPIO_PIN_6								// Intentify on which Pin is the CS line of the MS5611

// MS5611 Registers
#define MS5611_RESET						0x1E									// MS5611 Reset

#define MS5611_CONVERT_D1_OSR_256			0x40									// Convert D1 with OSR = 256
#define MS5611_CONVERT_D1_OSR_512			0x42									// Convert D1 with OSR = 256
#define MS5611_CONVERT_D1_OSR_1024			0x44									// Convert D1 with OSR = 256
#define MS5611_CONVERT_D1_OSR_2048			0x46									// Convert D1 with OSR = 256
#define MS5611_CONVERT_D1_OSR_4096			0x48									// Convert D1 with OSR = 256

#define MS5611_CONVERT_D2_OSR_256			0x50									// Convert D1 with OSR = 256
#define MS5611_CONVERT_D2_OSR_512			0x52									// Convert D1 with OSR = 256
#define MS5611_CONVERT_D2_OSR_1024			0x54									// Convert D1 with OSR = 256
#define MS5611_CONVERT_D2_OSR_2048			0x56									// Convert D1 with OSR = 256
#define MS5611_CONVERT_D2_OSR_4096			0x58									// Convert D1 with OSR = 256

#define MS5611_READ_ADC						0x00									// Read the current data on the ADC Output

#define MS5611_PROM_READ_BASE				0xA0									// Base Address to read PROM (with calibration data)
#define MS5611_PRESSURE_SENS				0x01									// Calibration coefficient 1
#define MS5611_PRESSURE_OFFSET				0x02									// Calibration coefficient 2
#define MS5611_TEMP_CS						0x03									// Calibration coefficient 3
#define MS5611_TEMP_CO						0x04									// Calibration coefficient 4
#define MS5611_TEMP_REFERENCE				0x05									// Calibration coefficient 5
#define MS5611_TEMP_SENS					0x06									// Calibration coefficient 6
#define MS5611_CRC							0x07									// Calibration coefficient CRC

#define MS5611_NUMBER_CALIBRATION_PARAM		7										// Number of calibration Parameters

//*****************************************************************************
//
//		Structures
//
//*****************************************************************************

struct MS5611
{
	uint32_t Pressure;
	uint32_t Temperature;
	uint16_t Pressure_Sens;
	uint16_t Pressure_Offset;
	uint16_t Temp_CS;
	uint16_t Temp_CO;
	uint16_t Temp_Reference;
	uint16_t Temp_Sens;
	uint16_t CRC;
};

//*****************************************************************************
//
//		Function Prototypes
//
//*****************************************************************************

void MS5611_Init(void);
void MS5611_UpdatePressure(void);
void MS5611_UpdateTemp(void);

#endif /* MS5611_H_ */
