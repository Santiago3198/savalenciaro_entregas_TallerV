/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Santiago Valencia Roldan
 * @brief          : Main program body
 ******************************************************************************
 * Tarea 2 Taller V - 2023-02
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 ******************************************************************************
 */

#include <stdint.h>
#include "GPIOxDriver.h"
#include "stm32fxxhal.h"


//Definicion de variables de estado

uint8_t start = 0;												//Variable donde se guarda el valor del conteo, se desea que el conteo inicie en 0.
uint8_t pin0, pin1, pin2, pin3, pin4, pin5, pin6	= 0;		//Valiable de estado de cada pin segun el bit que representa.

//Definicion de la funcion de configuracion

void configPin_Value (uint8_t start, uint8_t pin0, uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4, uint8_t pin5, uint8_t pin6);

//Inicializacion de cada uno de los handlers

	GPIO_Handler_t handlerBit0Pin 		= {0};	//PA7
	GPIO_Handler_t handlerBit1Pin 		= {0};	//PC8
	GPIO_Handler_t handlerBit2Pin 		= {0};	//PC7
	GPIO_Handler_t handlerBit3Pin 		= {0};	//PA6
	GPIO_Handler_t handlerBit4Pin 		= {0};	//PB8
	GPIO_Handler_t handlerBit5Pin 		= {0};	//PC6
	GPIO_Handler_t handlerBit6Pin 		= {0};	//PC9
	GPIO_Handler_t handlerUserButton 	= {0};	//USER_BUTTON (PC13)

int main(void)
{
	/*
	 * Se realiza la configuracion de los pines como salidad y del pin
	 * correspondiente al USER_BUTTON como entrada. Se realiza la creacion de los handler para
	 * cada pin.
	 */

		//PA7
		handlerBit0Pin.pGPIOx = GPIOA;
		handlerBit0Pin.GPIO_PinConfig.GPIO_PinNumber				= PIN_7;
		handlerBit0Pin.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_OUT;
		handlerBit0Pin.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
		handlerBit0Pin.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
		handlerBit0Pin.GPIO_PinConfig.GPIO_PinSpeed					= GPIO_OSPEEDR_MEDIUM;
		handlerBit0Pin.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;

		//PC8
		handlerBit1Pin.pGPIOx = GPIOC;
		handlerBit1Pin.GPIO_PinConfig.GPIO_PinNumber				= PIN_8;
		handlerBit1Pin.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_OUT;
		handlerBit1Pin.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
		handlerBit1Pin.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
		handlerBit1Pin.GPIO_PinConfig.GPIO_PinSpeed					= GPIO_OSPEEDR_MEDIUM;
		handlerBit1Pin.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;

		//PC7
		handlerBit2Pin.pGPIOx = GPIOC;
		handlerBit2Pin.GPIO_PinConfig.GPIO_PinNumber				= PIN_7;
		handlerBit2Pin.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_OUT;
		handlerBit2Pin.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
		handlerBit2Pin.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
		handlerBit2Pin.GPIO_PinConfig.GPIO_PinSpeed					= GPIO_OSPEEDR_MEDIUM;
		handlerBit2Pin.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;

		//PA6
		handlerBit3Pin.pGPIOx = GPIOA;
		handlerBit3Pin.GPIO_PinConfig.GPIO_PinNumber				= PIN_6;
		handlerBit3Pin.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_OUT;
		handlerBit3Pin.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
		handlerBit3Pin.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
		handlerBit3Pin.GPIO_PinConfig.GPIO_PinSpeed					= GPIO_OSPEEDR_MEDIUM;
		handlerBit3Pin.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;

		//PB8
		handlerBit4Pin.pGPIOx = GPIOB;
		handlerBit4Pin.GPIO_PinConfig.GPIO_PinNumber				= PIN_8;
		handlerBit4Pin.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_OUT;
		handlerBit4Pin.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
		handlerBit4Pin.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
		handlerBit4Pin.GPIO_PinConfig.GPIO_PinSpeed					= GPIO_OSPEEDR_MEDIUM;
		handlerBit4Pin.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;

		//PC6
		handlerBit5Pin.pGPIOx = GPIOC;
		handlerBit5Pin.GPIO_PinConfig.GPIO_PinNumber				= PIN_6;
		handlerBit5Pin.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_OUT;
		handlerBit5Pin.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
		handlerBit5Pin.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
		handlerBit5Pin.GPIO_PinConfig.GPIO_PinSpeed					= GPIO_OSPEEDR_MEDIUM;
		handlerBit5Pin.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;

		//PC9
		handlerBit6Pin.pGPIOx = GPIOC;
		handlerBit6Pin.GPIO_PinConfig.GPIO_PinNumber				= PIN_9;
		handlerBit6Pin.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_OUT;
		handlerBit6Pin.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
		handlerBit6Pin.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
		handlerBit6Pin.GPIO_PinConfig.GPIO_PinSpeed					= GPIO_OSPEEDR_MEDIUM;
		handlerBit6Pin.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;

		//PC13
		handlerUserButton.pGPIOx									= GPIOA;
		handlerUserButton.GPIO_PinConfig.GPIO_PinNumber				= PIN_13;
		handlerUserButton.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_IN;

		//Se carga la configuracion de cada uno de los pines definidos anteriormente

		GPIO_Config (&handlerBit0Pin);
		GPIO_Config (&handlerBit1Pin);
		GPIO_Config (&handlerBit2Pin);
		GPIO_Config (&handlerBit3Pin);
		GPIO_Config (&handlerBit4Pin);
		GPIO_Config (&handlerBit5Pin);
		GPIO_Config (&handlerBit6Pin);
		GPIO_Config (&handlerUserButton);

    /* Loop forever */
	while (1)
	{
		if (GPIO_ReadPin(&handlerUserButton) == 1){

			//Se invoca la funcion encargada de configurar cada pin dependiendo de la lectura que tenga de este
			configPin_Value (start, pin0, pin1, pin2, pin3, pin4, pin5, pin6);

			//Aumento de uno en uno para el start
			start+= 0;

			if (start > 60){
				//Cuando el valor de start es mayor a 60, este se reinicia
				start = 0;
			}
			else{
				//De lo contrario el start mantiene su valor
				start = start;
			}
		}
		else{
			//Cuando el boton del usuario se mantiene presionado  su estado es USER_BUTTON = 0
			configPin_Value (start, pin0, pin1, pin2, pin3, pin4, pin5, pin6);

			//Decremento de uno en uno para el start
			start-= 0;
			if (start < 0){

				//Cuando el valor del start es menor a 0, este se reinicia
				start = 60;
			}
			else{
				//De lo contrario el start mantiene su valor
				start = start;
			}
		}
		
		//Calculo del delay configurado en aproximadamente un segundo
		for (int i = 0; i<1500000; i++){
			NOP();
		}
	}
}

void configPin_Value (uint8_t start, uint8_t pin0, uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4, uint8_t pin5, uint8_t pin6){

	/*
	 * El paso siguiente describe una operacion & bit a bit de el bit que se requiere mostrar con el valor
	 * actual del start, posteriormente con el SHIFT REGISTER  se mueven las posiciones del bit tanto como
	 * lo indique la funcion GPIO_WritePin
	 */
	pin0 = (start&(0b1))>>0;
	pin1 = (start&(0b10))>>1;
	pin2 = (start&(0b100))>>2;
	pin3 = (start&(0b1000))>>3;
	pin4 = (start&(0b10000))>>4;
	pin5 = (start&(0b100000))>>5;
	pin6 = (start&(0b1000000))>>6;

	//Estado de los pines
	GPIO_WritePin(&handlerBit0Pin, pin0);
	GPIO_WritePin(&handlerBit1Pin, pin1);
	GPIO_WritePin(&handlerBit2Pin, pin2);
	GPIO_WritePin(&handlerBit3Pin, pin3);
	GPIO_WritePin(&handlerBit4Pin, pin4);
	GPIO_WritePin(&handlerBit5Pin, pin5);
	GPIO_WritePin(&handlerBit6Pin, pin6);

}

