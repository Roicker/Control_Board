/*
 * main.h
 *
 *  Created on: 03/10/2014
 *      Author: Ray
 */

#ifndef MAIN_H_
#define MAIN_H_

//*****************************************************************************
//
//		Local Header Files
//
//*****************************************************************************

// System Configuration
#include "Config.h"

//*****************************************************************************
//
//		Function Protoypes
//
//*****************************************************************************

void main(void);
bool SYS_Init(void);
void LED_Init(void);
void DEBUG_Init(void);
uint8_t * UTILS_Memcpy(uint8_t * pDestination, const uint32_t * cpSource, uint32_t u32Length);
void UTILS_Memset(uint8_t * pDestination, uint8_t u8Value, uint32_t u32Length);

#endif /* MAIN_H_ */
