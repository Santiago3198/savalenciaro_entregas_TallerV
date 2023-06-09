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
#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "USARTxDriver.h"
#include "I2CxDriver.h"
#include "PWMDriver.h"
#include "PLLDriver.h"
#include "SysTickDriver.h"

//Definiciòn de variables
GPIO_Handler_t handlerBlinky = {0};
GPIO_Handler_t handlerPinTX = {0};
GPIO_Handler_t handlerPinRX = {0};

BasicTimer_Handler_t handlerTimerBlinky = {0};

USART_Handler_t handlerCommTerminal = {0};

uint8_t rxData = 0;
char bufferData[128] = {0};


//Definición de funciones
void initSystem(void);

int main(void){

	/*Loop forever*/
	while(1){

	}
	return 0;
}

void initSystem(void){

	//Configuración del Blinky
	handlerBlinky.pGPIOx = GPIOA;
	handlerBlinky.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerBlinky.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
	handlerBlinky.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerBlinky.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEEDR_FAST;
	handlerBlinky.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerBlinky);

	//Configuración del TIM2 (Blinky)
	handlerTimerBlinky.ptrTIMx = TIM2;
	handlerTimerBlinky.TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
	handlerTimerBlinky.TIMx_Config.TIMx_speed = 1000;
	handlerTimerBlinky.TIMx_Config.TIMx_period = 25000;
	handlerTimerBlinky.TIMx_Config.TIMx_interruptEnable = 1;
	BasicTimer_Config(&handlerTimerBlinky);

	//Configuración de los pines para USART
	handlerPinTX.pGPIOx										= GPIOA;
	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber				= PIN_9;
	handlerPinTX.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerPinTX.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
	handlerPinTX.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	handlerPinTX.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEEDR_FAST;
	handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode			= AF7;
	GPIO_Config(&handlerPinTX);

	handlerPinRX.pGPIOx										= GPIOA;
	handlerPinRX.GPIO_PinConfig.GPIO_PinNumber				= PIN_10;
	handlerPinRX.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerPinRX.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
	handlerPinRX.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	handlerPinRX.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEEDR_FAST;
	handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode			= AF7;
	GPIO_Config(&handlerPinRX);

	//Configuración del puerto serial
	handlerCommTerminal.ptrUSARTx	 						= USART1;
	handlerCommTerminal.USART_Config.USART_baudrate			= USART_BAUDRATE_115200;
	handlerCommTerminal.USART_Config.USART_datasize			= USART_DATASIZE_8BIT;
	handlerCommTerminal.USART_Config.USART_parity			= USART_PARITY_NONE;
	handlerCommTerminal.USART_Config.USART_stopbits			= USART_STOPBIT_1;
	handlerCommTerminal.USART_Config.USART_mode				= USART_MODE_RXTX;
	handlerCommTerminal.USART_Config.USART_enableIntTX		= USART_TX_INTERRUP_DISABLE;
	handlerCommTerminal.USART_Config.USART_enableIntRX		= USART_RX_INTERRUP_ENABLE;
	USART_Config(&handlerCommTerminal);
}

void usart1Rx_Callback(void){
	//Leemos elvalor del registro DR, donde se almacena el dato que llega.
	//Esto ademàs debe bajar la bandera de la interrupciòn
	rxData = getRxData();
}

void BasicTimer2_Callback(void){
	//Hacemos un Blinky para indicar que el equipo està funcionando correctamente
	GPIOxTooglePin(&handlerBlinky);
}


