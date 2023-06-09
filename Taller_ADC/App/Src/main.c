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

USART_Handler_t handlerCommTerminal 	= {0};

ADC_Config_t channel 			= {0};
GPIO_Handler_t handlerPwmPin 	= {0};
PWM_Handler_t handlerPWM 		= {0};

#define CHANNELS   2
uint8_t rxData = 0;
uint8_t adcIsComplete		= 0;
uint8_t adcCounter = 0;
uint16_t dataADC[2] = {0};

char bufferData[128] = {0};

//Definición de los prototipos de funciones
void initSystem(void);
void configPLL(void);
void ADC_ConfigMultichannel(ADC_Config_t *adcConfig, uint8_t numeroDeCanales);

int main(void){

	configPLL();
	initSystem();
	writeMsg(&handlerCommTerminal, "\n~Iniciando Sistema~\n");

	/*Loop forever*/
	while(1){
		if(rxData != '\0'){

			if(rxData == 'a'){

				// Iniciamos una conversión ADC

				}

			// Limpiamos el dato de RX
			rxData = '\0';
		}
		if(adcIsComplete == 1){

			// Enviamos los datos por consola
			sprintf(bufferData,"%u\t%u\n",dataADC[0],dataADC[1]);
			writeMsg(&handlerCommTerminal, bufferData);

			// Bajamos la bandera del ADC
			adcIsComplete = 0;
		}
	}
}

void initSystem(void){

	//Configuraciòn del Blinky
	handlerBlinky.pGPIOx									= GPIOA;
	handlerBlinky.GPIO_PinConfig.GPIO_PinNumber				= PIN_5;
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

	//Configuración ADC
	channel.numberOfChannels		= CHANNELS;
	channel.multiChannel[0] 		= ADC_CHANNEL_1;
	channel.multiChannel[1] 		= ADC_CHANNEL_4;
	channel.multiSampling[0] 		= ADC_SAMPLING_PERIOD_84_CYCLES;
	channel.multiSampling[1] 		= ADC_SAMPLING_PERIOD_84_CYCLES;
	channel.dataAlignment 			= ADC_ALIGNMENT_RIGHT;
	channel.resolution 				= ADC_RESOLUTION_12_BIT;
	channel.eventType				= EXTERNAL_EVENT_ENABLE;
	channel.AdcEvent				= 8;
	ADC_ConfigMultichannel(&channel, CHANNELS);

	//Configuración del pin para el PWM
	handlerPwmPin.pGPIOx 								= GPIOA;
	handlerPwmPin.GPIO_PinConfig.GPIO_PinNumber 		= PIN_6;
	handlerPwmPin.GPIO_PinConfig.GPIO_PinMode   		= GPIO_MODE_ALTFN;
	handlerPwmPin.GPIO_PinConfig.GPIO_PinOPType 		= GPIO_OTYPE_PUSHPULL;
	handlerPwmPin.GPIO_PinConfig.GPIO_PinPuPdControl 	= GPIO_PUPDR_NOTHING;
	handlerPwmPin.GPIO_PinConfig.GPIO_PinSpeed  		= GPIO_OSPEEDR_FAST;
	handlerPwmPin.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF2;
	GPIO_Config(&handlerPwmPin);

	//Configuración de la señal PWM
	handlerPWM.ptrTIMx 				= TIM3;
	handlerPWM.config.channel 		= PWM_CHANNEL_1;
	handlerPWM.config.duttyCicle 	= 33;
	handlerPWM.config.periodo 		= 66;
	handlerPWM.config.prescaler 	= 100;
	pwm_Config(&handlerPWM);

	startPwmSignal(&handlerPWM);

	//Se carga la configuración de las interrupciones
	ADC_Channel_Interrupt(&channel);
}

void usart1Rx_Callback(void){
	//Leemos elvalor del registro DR, donde se almacena el dato que llega.
	rxData = getRxData();
}

void BasicTimer2_Callback(void){
	//Hacemos un Blinky para indicar que el equipo està funcionando correctamente
	GPIOxTooglePin(&handlerBlinky);
}

//void adcComplete_Callback(void){
//	dataADC[adcCounter] = getADC();
//	if(adcCounter < (CHANNELS-1)){
//		adcCounter++;
//	}
//	else{
//		adcIsComplete = 1;
//		adcCounter = 0;
//	}
//}
