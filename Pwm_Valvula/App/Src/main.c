/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Santiago Valencia Roldan 
 * @brief          : Main program body
 ******************************************************************************
*/
/*
 * Descomentar la siguiente linea y agregar al main si se requiere el uso
 * de la FPU.
 * SCB->CPACR |= (0xF << 20);
 * Activacion del coprocesador matematico
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "USARTxDriver.h"
#include "I2CxDriver.h"
#include "PWMDriver.h"
#include "PLLDriver.h"
#include "SysTickDriver.h"

//Definición de handlers
GPIO_Handler_t handlerBlinky 				= {0};		//Handler Blinky
GPIO_Handler_t handlerPinPwmChannel			= {0};		//Handler canal PWM
BasicTimer_Handler_t handlerTimerBlinky 	= {0};		//Handler Timer del blinky

//Declaración de funciones
void initSystem(void);

int main(void){

	initSystem();

	/*Loop forever*/
	while(1){

	}
	return 0;
}
void initSystem(void){

	//Configuración del blinky
	handlerBlinky.pGPIOx 											= GPIOC;
	handlerBlinky.GPIO_PinConfig.GPIO_PinNumber 					= PIN_0;
	handlerBlinky.GPIO_PinConfig.GPIO_PinMode 						= GPIO_MODE_OUT;
	handlerBlinky.GPIO_PinConfig.GPIO_PinOPType 					= GPIO_OTYPE_PUSHPULL;
	handlerBlinky.GPIO_PinConfig.GPIO_PinSpeed 						= GPIO_OSPEEDR_FAST;
	handlerBlinky.GPIO_PinConfig.GPIO_PinPuPdControl 				= GPIO_PUPDR_NOTHING;
	handlerBlinky.GPIO_PinConfig.GPIO_PinAltFunMode 				= AF0;
	GPIO_Config(&handlerBlinky);

	//Configuración del TIM2 (Blinky)
	handlerTimerBlinky.ptrTIMx 										= TIM2;
	handlerTimerBlinky.TIMx_Config.TIMx_mode 						= BTIMER_MODE_UP;
	handlerTimerBlinky.TIMx_Config.TIMx_speed 						= BTIMER_SPEED_100us;
	handlerTimerBlinky.TIMx_Config.TIMx_period 						= 2500;
	handlerTimerBlinky.TIMx_Config.TIMx_interruptEnable 			= 1;
	BasicTimer_Config(&handlerTimerBlinky);
}

void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerBlinky);
}
