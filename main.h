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
void UART_Init(void);
void LED_Init(void);
void UART_IntHandler(void);

#endif /* MAIN_H_ */
