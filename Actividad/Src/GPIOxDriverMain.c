/*
 * GPIOxDriverMain.c
 *
 *  Created on: Mar 12, 2023
 *      Author: Santiago Valencia
 */

#include <stdint.h>

#include "stm32fxxhal.h"
#include "GPIOxDriver.h"

int main(void){

	GPIO_Handler_t handlerUserLedPin = {0};

	handlerUserLedPin.pGPIOx = GPIOA;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinNumber				= PIN_5;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_MEDIUM;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;

	GPIO_Config(&handlerUserLedPin);

	GPIO_WritePin(&handlerUserLedPin, SET);

	while(1){
		NOP();
	}

}

