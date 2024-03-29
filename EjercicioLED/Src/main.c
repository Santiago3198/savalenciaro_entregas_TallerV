/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
#include <stm32f4xx.h>
#include <stdint.h>
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"

GPIO_Handler_t handlerLED = {0};
GPIO_Handler_t handlerButton = {0};

//Variable que guarda el estado del USER_Button
uint8_t userButton = {0};

int main(void){


	//Configuración pin LED
	handlerLED.pGPIOx = GPIOA;
	handlerLED.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
	handlerLED.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerLED.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEEDR_FAST;
	handlerLED.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerLED.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerLED.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	GPIO_Config(&handlerLED);

	//Configuracion pun USER_Button
	handlerButton.pGPIOx = GPIOC;
	handlerButton.GPIO_PinConfig.GPIO_PinNumber = PIN_13;
	handlerButton.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	handlerButton.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEEDR_FAST;
	handlerButton.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerButton.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_PULLUP;
	handlerButton.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	GPIO_Config(&handlerButton);

    while (1){

        userButton = GPIO_ReadPin(&handlerButton);
        if(userButton == RESET){
        	GPIO_WritePin(&handlerLED, RESET);
        }else{
        	GPIO_WritePin(&handlerLED, SET);
        }

    	}
    return 0;

}

