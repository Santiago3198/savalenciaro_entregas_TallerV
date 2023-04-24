/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Santiago Valencia Roldan 
 * @brief          : Main program body
 ******************************************************************************
*/

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "USARTxDriver.h"

//Definicion de los handlers necesarios
GPIO_Handler_t handlerBlinkyPin 			= {0};
BasicTimer_Handler_t handlerBlinkyTimer 	= {0};

//Elementos para la comunicacion serial
GPIO_Handler_t handlerPinTX	= {0};
USART_Handler_t handlerUSART2	= {0};

void initSystem(void);

int main(void){

	writeChar(&handlerUSART2, 5);

	/*Loop forever*/
	while(1){

	}
	return 0;
}

void initSystem(void){

	//Configurando el pin para el Led_Blinky
	handlerBlinkyPin.pGPIOx									= GPIOA;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinNumber			= PIN_5;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEEDR_FAST;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerBlinkyPin);

	//Configurando el Timer2 para que funcione con el Blinky
	handlerBlinkyTimer.ptrTIMx 								= TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode				= BTIMER_MODE_UP;
	handlerBlinkyTimer.TIMx_Config.TIMx_speed				= BTIMER_SPEED_100us;
	handlerBlinkyTimer.TIMx_Config.TIMx_period				= 2500;
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable		= 1;
	BasicTimer_Config(&handlerBlinkyTimer);

	handlerPinTX.pGPIOx										= GPIOA;
	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber				= PIN_2;
	handlerPinTX.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode			= AF7;
	GPIO_Config(&handlerPinTX);

	handlerUSART2.ptrUSARTx	 								= USART2;
	handlerUSART2.USART_Config.USART_baudrate					= USART_BAUDRATE_115200;
	handlerUSART2.USART_Config.USART_datasize					= USART_DATASIZE_8BIT;
	handlerUSART2.USART_Config.USART_parity					= USART_PARITY_NONE;
	handlerUSART2.USART_Config.USART_stopbits					= USART_STOPBIT_1;
	handlerUSART2.USART_Config.USART_mode						= USART_MODE_RXTX;
	USART_Config(&handlerUSART2);
}



