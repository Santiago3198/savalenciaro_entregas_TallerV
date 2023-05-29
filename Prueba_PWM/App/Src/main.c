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

//Definicion de los handlers necesarios
GPIO_Handler_t handlerBlinkyPin 			= {0};
GPIO_Handler_t handlerUserButton 			= {0};
BasicTimer_Handler_t handlerBlinkyTimer 	= {0};
EXTI_Config_t handlerUserButtonExti 		= {0};

//Elementos para la comunicacion serial

GPIO_Handler_t handlerPinTX2	= {0};
GPIO_Handler_t handlerPinRX2 	= {0};

USART_Handler_t usart2Comm		= {0};

uint8_t sendMsg = 0;
uint8_t usart2DataReceived = 0;

//Elementos para el PWM
GPIO_Handler_t handlerPinPwmChannel = {0};
PWM_Handler_t handlerSignalPWM		= {0};

uint16_t duttyValue = 1500;

char bufferMsg[64] = {0};

void initSystem(void);

int main(void){

	//Inicializar los elementos del sistema
	initSystem();

	/*Loop forever*/
	while(1){

		//Verificando el PWM
		if(usart2DataReceived != '\0'){
			if(usart2DataReceived == 'd'){

				//Down
				duttyValue -= 10;
				updateDuttyCycle(&handlerSignalPWM, duttyValue);
			}

			//Para probar el seno
			if(usart2DataReceived == 'u'){

				//Up
				duttyValue += 10;

				//Lanzamos un nuevo ciclo de adquisicones
				updateDuttyCycle(&handlerSignalPWM, duttyValue);
			}

			//Imprimimos el mensaje
			sprintf(bufferMsg, "dutty = %u \n", (unsigned int) duttyValue);
			writeMsg(&usart2Comm, bufferMsg);

			//Cambiamos el estado del elemento que controla la entrada
			usart2DataReceived = '\0';
		}

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

	usart2Comm.ptrUSARTx	 								= USART2;
	usart2Comm.USART_Config.USART_baudrate					= USART_BAUDRATE_9600;
	usart2Comm.USART_Config.USART_datasize					= USART_DATASIZE_8BIT;
	usart2Comm.USART_Config.USART_parity					= USART_PARITY_NONE;
	usart2Comm.USART_Config.USART_stopbits					= USART_STOPBIT_1;
	usart2Comm.USART_Config.USART_mode						= USART_MODE_RXTX;
	usart2Comm.USART_Config.USART_enableIntTX				= USART_TX_INTERRUP_DISABLE;
	usart2Comm.USART_Config.USART_enableIntRX				= USART_RX_INTERRUP_ENABLE;
	USART_Config(&usart2Comm);

	//Configuraciòn del pin para la señal PWM
	handlerPinPwmChannel.pGPIOx									= GPIOC;
	handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinNumber			= PIN_7;
	handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEEDR_FAST;
	handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinAltFunMode		= AF2;
	GPIO_Config(&handlerPinPwmChannel);

	//Configuraciòn del timer para que genere la señal PWM
	handlerSignalPWM.ptrTIMx					= TIM3;
	handlerSignalPWM.config.channel				= PWM_CHANNEL_2;
	handlerSignalPWM.config.duttyCicle			= duttyValue;
	handlerSignalPWM.config.periodo				= 20000;
	handlerSignalPWM.config.prescaler			= 16;
	pwm_Config(&handlerSignalPWM);

	//Activamos la señal
	enableOutput(&handlerSignalPWM);
	startPwmSignal(&handlerSignalPWM);
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


