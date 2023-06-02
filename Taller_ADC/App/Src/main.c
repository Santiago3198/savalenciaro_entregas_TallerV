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
#include "AdcDriver.h"

//Definiciòn de variables
GPIO_Handler_t handlerBlinky = {0};
GPIO_Handler_t handlerPinTX = {0};
GPIO_Handler_t handlerPinRX = {0};

BasicTimer_Handler_t handlerTimerBlinky = {0};
BasicTimer_Handler_t handlerTimer5		= {0};

USART_Handler_t handlerCommTerminal = {0};

ADC_Config_t channel_0 = {0};

uint8_t rxData = 0;
uint8_t adcIsComplete = 0;
uint8_t dataADC = 0;

char bufferData[128] = {0};

//Definiciòn de los prototipos de funciones
void initSystem(void);

int main(void){

	initSystem();
	writeMsg(&handlerCommTerminal, "\n~Iniciando Sistema~\n");

	/*Loop forever*/
	while(1){

//
//		if(rxData == '\0'){
//
//			if(rxData != 's'){
//				//Activamos una conversión simple
//				startSingleADC();
//			}
//			//Limpiamos el dato de RX
//			rxData = '\0';
//		}
		if(adcIsComplete == 1){

			//Enviamos los datos por consola
			sprintf(bufferData, "data: %u\n", dataADC);
			writeMsg(&handlerCommTerminal, bufferData);

			//Bajamos la bandera del ADC
			adcIsComplete = 0;
		}
	}
}

void initSystem(void){

	//Configuraciòn del Blinky
	handlerBlinky.pGPIOx									= GPIOC;
	handlerBlinky.GPIO_PinConfig.GPIO_PinNumber				= PIN_0;
	handlerBlinky.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	handlerBlinky.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
	handlerBlinky.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEEDR_FAST;
	handlerBlinky.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerBlinky);

	//Configuraciòn del TIM 2
	handlerTimerBlinky.ptrTIMx 								= TIM2;
	handlerTimerBlinky.TIMx_Config.TIMx_mode				= BTIMER_MODE_UP;
	handlerTimerBlinky.TIMx_Config.TIMx_speed				= BTIMER_SPEED_100us;
	handlerTimerBlinky.TIMx_Config.TIMx_period				= 2500;
	handlerTimerBlinky.TIMx_Config.TIMx_interruptEnable		= 1;
	BasicTimer_Config(&handlerTimerBlinky);

	//Configurción TIM 5
	handlerTimer5.ptrTIMx 									= TIM5;
	handlerTimer5.TIMx_Config.TIMx_mode						= BTIMER_MODE_UP;
	handlerTimer5.TIMx_Config.TIMx_speed					= BTIMER_SPEED_10us;
	handlerTimer5.TIMx_Config.TIMx_period					= 500;
	handlerTimer5.TIMx_Config.TIMx_interruptEnable			= 1;
	BasicTimer_Config(&handlerTimer5);

	//Configuraciòn de los pines para USART
	handlerPinTX.pGPIOx										= GPIOA;
	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber				= PIN_2;
	handlerPinTX.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerPinTX.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
	handlerPinTX.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	handlerPinTX.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEEDR_FAST;
	handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode			= AF7;
	GPIO_Config(&handlerPinTX);

	handlerPinRX.pGPIOx										= GPIOA;
	handlerPinRX.GPIO_PinConfig.GPIO_PinNumber				= PIN_3;
	handlerPinRX.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerPinRX.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
	handlerPinRX.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	handlerPinRX.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEEDR_FAST;
	handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode			= AF7;
	GPIO_Config(&handlerPinRX);

	//Configuraciòn del puerto serial
	handlerCommTerminal.ptrUSARTx	 						= USART2;
	handlerCommTerminal.USART_Config.USART_baudrate			= USART_BAUDRATE_115200;
	handlerCommTerminal.USART_Config.USART_datasize			= USART_DATASIZE_8BIT;
	handlerCommTerminal.USART_Config.USART_parity			= USART_PARITY_NONE;
	handlerCommTerminal.USART_Config.USART_stopbits			= USART_STOPBIT_1;
	handlerCommTerminal.USART_Config.USART_mode				= USART_MODE_RXTX;
	handlerCommTerminal.USART_Config.USART_enableIntTX		= USART_TX_INTERRUP_DISABLE;
	handlerCommTerminal.USART_Config.USART_enableIntRX		= USART_RX_INTERRUP_ENABLE;
	USART_Config(&handlerCommTerminal);

	//Configuración ADC
	channel_0.channel = ADC_CHANNEL_0;
	channel_0.dataAlignment = ADC_ALIGNMENT_RIGHT;
	channel_0.samplingPeriod = ADC_SAMPLING_PERIOD_84_CYCLES;
	channel_0.resolution = ADC_RESOLUTION_12_BIT;
	adc_Config(&channel_0);
}

void usart2Rx_Callback(void){
	//Leemos elvalor del registro DR, donde se almacena el dato que llega.
	//Esto ademàs debe bajar la bandera de la interrupciòn
	rxData = getRxData();
}

void BasicTimer2_Callback(void){
	//Hacemos un Blinky para indicar que el equipo està funcionando correctamente
	GPIOxTooglePin(&handlerBlinky);
}

void BasicTimer5_Callback(void){
	startSingleADC();
}

void adcComplete_Callback(void){

	dataADC = getADC();
	adcIsComplete = SET;
}
