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
GPIO_Handler_t handlerUserButton 			= {0};
BasicTimer_Handler_t handlerBlinkyTimer 	= {0};
EXTI_Config_t handlerUserButtonExti 		= {0};

//Elementos para la comunicacion serial
GPIO_Handler_t handlerPinTX	= {0};
GPIO_Handler_t handlerPinRX = {0};
USART_Handler_t usart2Comm	= {0};
uint8_t sendMsg = 0;

//Definir las cabeceras de las funciones del main
void initSystem(void);

/*
 * Funcion principal del programa
 * Esta funcion es el corazon del programa
 */

int main(void){

	//Inicializamos todos los elementos del sistema
	initSystem();

	/*Loop forever*/
	while(1){
		if(sendMsg == 4){
			writeMsg (&usart2Comm, "Malparido protocolo de USART funciona cuando le da la perra gana");
			sendMsg = 0;
		}
	}
	return 0;
}

/*Funcion encargada de la inicializacion de los elementos del sistema*/

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

	/* El pin USER_BUTTON es una entrada simple que entregara la interrupcion EXTI1
	 * Observar que el pin seleccionado es el PIN_13, por lo cual el callback
	 * que se debe configurar es el callback_extInt13()
	 */
	handlerUserButton.pGPIOx								= GPIOC;
	handlerUserButton.GPIO_PinConfig.GPIO_PinNumber			= PIN_13;
	handlerUserButton.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;
	handlerUserButton.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerUserButton);

	handlerUserButtonExti.pGPIOHandler	= &handlerUserButton; //Esto es un puntero a la estructura
	handlerUserButtonExti.edgeType		= EXTERNAL_INTERRUPT_RISING_EDGE;
	extInt_Config(&handlerUserButtonExti);

	//Configuracion de la comunicacion serial

	handlerPinTX.pGPIOx										= GPIOA;
	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber				= PIN_2;
	handlerPinTX.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode			= AF7;
	GPIO_Config(&handlerPinTX);

	handlerPinRX.pGPIOx										= GPIOA;
	handlerPinRX.GPIO_PinConfig.GPIO_PinNumber				= PIN_3;
	handlerPinRX.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode			= AF7;
	GPIO_Config(&handlerPinRX);

	usart2Comm.ptrUSARTx	 								= USART2;
	usart2Comm.USART_Config.USART_baudrate					= USART_BAUDRATE_115200;
	usart2Comm.USART_Config.USART_datasize					= USART_DATASIZE_8BIT;
	usart2Comm.USART_Config.USART_parity					= USART_PARITY_NONE;
	usart2Comm.USART_Config.USART_stopbits					= USART_STOPBIT_1;
	usart2Comm.USART_Config.USART_mode						= USART_MODE_RXTX;
	USART_Config(&usart2Comm);
}

/*
 * Callback del TIM2 - Hacemos un Blinky
 */
void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerBlinkyPin);
	sendMsg++;
}

/*Callback del userButton -> hacer algo...*/
void callback_extInt13(void){
	__NOP();
}

