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
#include "I2CDriver.h"
#include "PWMDriver.h"
#include "PLLDriver.h"
#include "SysTickDriver.h"

//Definición de los handlers
//Handlers GPIO
GPIO_Handler_t handlerBlinky 				= {0};
GPIO_Handler_t handlerPinTX1				= {0};
GPIO_Handler_t handlerPinRX1				= {0};

//Handlers Timers
BasicTimer_Handler_t handlerTimerBlinky 	= {0};

//Handlers comunicación serial
USART_Handler_t handlerUsart1 				= {0};

//Definir las cabeceras de las funciones del main
void initSystem(void);

//Función principal del sistema
int main(void){

	//Configuración del Blinky
	handlerBlinky.pGPIOx = GPIOA;
	handlerBlinky.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerBlinky.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
	handlerBlinky.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerBlinky.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEEDR_FAST;
	handlerBlinky.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerBlinky.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	GPIO_Config(&handlerBlinky);

	//Configuración del TIM2 (Blinky)
	handlerTimerBlinky.ptrTIMx = TIM2;
	handlerTimerBlinky.TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
	handlerTimerBlinky.TIMx_Config.TIMx_speed = BTIMER_SPEED_100us;		//Corregir velocidad según la nueva frecuencia
	handlerTimerBlinky.TIMx_Config.TIMx_period = 2500;					//Corregir  periodo según la nueva frecuencia
	handlerTimerBlinky.TIMx_Config.TIMx_interruptEnable = 1;
	BasicTimer_Config(&handlerTimerBlinky);

	//Configuración de pines para USART1
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

	//Configuración de la comunicación serial
	handlerUsart1.ptrUSARTx	 								= USART1;
	handlerUsart1.USART_Config.USART_baudrate				= USART_BAUDRATE_115200;
	handlerUsart1.USART_Config.USART_datasize				= USART_DATASIZE_8BIT;
	handlerUsart1.USART_Config.USART_parity					= USART_PARITY_NONE;
	handlerUsart1.USART_Config.USART_stopbits				= USART_STOPBIT_1;
	handlerUsart1.USART_Config.USART_mode					= USART_MODE_RXTX;
	handlerUsart1.USART_Config.USART_enableIntTX			= USART_TX_INTERRUP_ENABLE;
	handlerUsart1.USART_Config.USART_enableIntRX			= USART_RX_INTERRUP_ENABLE;
	USART_Config(&handlerUsart1);


	/*Loop forever*/
	while(1){

	}
	return 0;
}


