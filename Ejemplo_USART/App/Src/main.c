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
#include <math.h>
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
GPIO_Handler_t handlerPinTX1	= {0};
GPIO_Handler_t handlerPinRX1	= {0};
GPIO_Handler_t handlerPinTX2	= {0};
GPIO_Handler_t handlerPinRX2 	= {0};
GPIO_Handler_t handlerPinTX6	= {0};
GPIO_Handler_t handlerPinRX6	= {0};
USART_Handler_t usart1Comm		= {0};
USART_Handler_t usart2Comm		= {0};
USART_Handler_t usart6Comm		= {0};
uint8_t sendMsg = 0;
uint8_t usart2DataReceived = 0;

char dataMsg[64] = {0};

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
		if(sendMsg > 4){

			sprintf(dataMsg, "El valor de Pi es = %f \n", M_PI);
			writeMsg(&usart2Comm, dataMsg);

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
	handlerPinTX1.pGPIOx									= GPIOA;
	handlerPinTX1.GPIO_PinConfig.GPIO_PinNumber				= PIN_9;
	handlerPinTX1.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerPinTX1.GPIO_PinConfig.GPIO_PinAltFunMode			= AF7;
	GPIO_Config(&handlerPinTX1);

	handlerPinRX1.pGPIOx									= GPIOA;
	handlerPinRX1.GPIO_PinConfig.GPIO_PinNumber				= PIN_10;
	handlerPinRX1.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerPinRX1.GPIO_PinConfig.GPIO_PinAltFunMode			= AF7;
	GPIO_Config(&handlerPinRX1);

	handlerPinTX2.pGPIOx									= GPIOA;
	handlerPinTX2.GPIO_PinConfig.GPIO_PinNumber				= PIN_2;
	handlerPinTX2.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerPinTX2.GPIO_PinConfig.GPIO_PinAltFunMode			= AF7;
	GPIO_Config(&handlerPinTX2);

	handlerPinRX2.pGPIOx									= GPIOA;
	handlerPinRX2.GPIO_PinConfig.GPIO_PinNumber				= PIN_3;
	handlerPinRX2.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerPinRX2.GPIO_PinConfig.GPIO_PinAltFunMode			= AF7;
	GPIO_Config(&handlerPinRX2);

	handlerPinTX6.pGPIOx									= GPIOA;
	handlerPinTX6.GPIO_PinConfig.GPIO_PinNumber				= PIN_11;
	handlerPinTX6.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerPinTX6.GPIO_PinConfig.GPIO_PinAltFunMode			= AF8;
	GPIO_Config(&handlerPinTX6);

	handlerPinRX6.pGPIOx									= GPIOA;
	handlerPinRX6.GPIO_PinConfig.GPIO_PinNumber				= PIN_12;
	handlerPinRX6.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerPinRX6.GPIO_PinConfig.GPIO_PinAltFunMode			= AF8;
	GPIO_Config(&handlerPinRX6);

	usart1Comm.ptrUSARTx	 								= USART1;
	usart1Comm.USART_Config.USART_baudrate					= USART_BAUDRATE_9600;
	usart1Comm.USART_Config.USART_datasize					= USART_DATASIZE_8BIT;
	usart1Comm.USART_Config.USART_parity					= USART_PARITY_NONE;
	usart1Comm.USART_Config.USART_stopbits					= USART_STOPBIT_1;
	usart1Comm.USART_Config.USART_mode						= USART_MODE_RXTX;
	usart1Comm.USART_Config.USART_enableIntTX				= USART_TX_INTERRUP_DISABLE;
	usart1Comm.USART_Config.USART_enableIntRX				= USART_RX_INTERRUP_ENABLE;
	USART_Config(&usart1Comm);

	usart2Comm.ptrUSARTx	 								= USART2;
	usart2Comm.USART_Config.USART_baudrate					= USART_BAUDRATE_9600;
	usart2Comm.USART_Config.USART_datasize					= USART_DATASIZE_8BIT;
	usart2Comm.USART_Config.USART_parity					= USART_PARITY_NONE;
	usart2Comm.USART_Config.USART_stopbits					= USART_STOPBIT_1;
	usart2Comm.USART_Config.USART_mode						= USART_MODE_RXTX;
	usart2Comm.USART_Config.USART_enableIntTX				= USART_TX_INTERRUP_DISABLE;
	usart2Comm.USART_Config.USART_enableIntRX				= USART_RX_INTERRUP_ENABLE;
	USART_Config(&usart2Comm);

	usart6Comm.ptrUSARTx	 								= USART6;
	usart6Comm.USART_Config.USART_baudrate					= USART_BAUDRATE_9600;
	usart6Comm.USART_Config.USART_datasize					= USART_DATASIZE_8BIT;
	usart6Comm.USART_Config.USART_parity					= USART_PARITY_NONE;
	usart6Comm.USART_Config.USART_stopbits					= USART_STOPBIT_1;
	usart6Comm.USART_Config.USART_mode						= USART_MODE_RXTX;
	usart6Comm.USART_Config.USART_enableIntTX				= USART_TX_INTERRUP_DISABLE;
	usart6Comm.USART_Config.USART_enableIntRX				= USART_RX_INTERRUP_ENABLE;
	USART_Config(&usart6Comm);
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

/*
 * Esta funcion se ejecuta cada vez que un caracter es recibido
 * por el puerto USART2
 */

void usart2Rx_Callback(void){
	usart2DataReceived = getRxData();
}
