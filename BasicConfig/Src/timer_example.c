/*
 * timer_example.c
 *
 *  Created on: 14/03/2023
 *      Author: if420_01
 */

#include <stdint.h>
#include <stm32fxxhal.h>

#include "GPIOxDriver.h"

GPIO_Handler_t handlerOnBoardLed = {};

int main (void) {
	/* Establecer las configuraciones del PIN_A5 como salida */
	handlerOnBoardLed.pGPIOx = GPIOA;
	handlerOnBoardLed.GPIO_PinConfig.GPIO_PinNumber	= PIN_5;
	handlerOnBoardLed.GPIO_PinConfig.GPIO_PinMode	= GPIO_MODE_OUT;
	handlerOnBoardLed.GPIO_PinConfig.GPIO_PinSpeed	= GPIO_OSPEED_FAST;
	handlerOnBoardLed.GPIO_PinConfig.GPIO_PinOPType	= GPIO_OTYPE_PUSHPULL;

	GPIO_Config(&handlerOnBoardLed);

	GPIO_WritePin(&handlerOnBoardLed, SET);

	while(1){

	}
}



