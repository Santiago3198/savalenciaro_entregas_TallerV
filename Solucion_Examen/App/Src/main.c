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

//Definición de handlers GPIO
GPIO_Handler_t handlerBlinky 				= {0};		//Handler Blinky
GPIO_Handler_t handlerPinTX1				= {0};		//Handler Transmisión USART
GPIO_Handler_t handlerPinRX1				= {0};		//Handler Recepción USART

//Definición de handlers TIM
BasicTimer_Handler_t handlerTimerBlinky 	= {0};		//Handler Timer del blinky
BasicTimer_Handler_t handlerTimerSamp		= {0};		//Handler Timer del muestreo

//Handlers comunicación serial USART
USART_Handler_t handlerUsart1 				= {0};		//Handler USART1

//Handlers I2C (ACCEL)
GPIO_Handler_t handlerI2cSDA				= {0};		//Handler SDA I2C acelerómetro
GPIO_Handler_t handlerI2cSCL 				= {0};		//Handler SCL I2C acelerómetro
I2C_Handler_t handlerAccelerometer 			= {0};		//Handler de la configuración I2C acelerómetro

//Configuración de canales ADC
ADC_Config_t channel_1				= {0};
ADC_Config_t channel_2				= {0};

//Definición de variables
uint8_t rxData = 0;
uint8_t i2cBuffer = 0;
uint16_t indx = 0;
uint8_t saveDataAccFlag = 0;
char bufferData[64] = "Accel MP-6050";

//Definición de arreglos para guardar los muestreos del acelerómetro
uint16_t arrayX[2000] = {0};
uint16_t arrayY[2000] = {0};
uint16_t arrayZ[2000] = {0};
float converFact = (2.0/32767.0)*(9.8);			//Factor de conversión para los datos del accel

//Definiciones para comunicación I2C (ACCEL)
#define ACCEL_ADDRESS 	0b1101001;		//ID Device
#define ACCEL_XOUT_H	59 				//0x3B
#define ACCEL_XOUT_L 	60 				//0x3C
#define ACCEL_YOUT_H	61				//0x3D
#define ACCEL_YOUT_L	62 				//0x3E
#define ACCEL_ZOUT_H	63				//0x3F
#define ACCEL_ZOUT_L	64				//0x40

#define PWR_MGMT_1		107
#define WHO_AM_I		117

//Definición de funciones
void initSystem(void);
void configPLL(void);
void saveAccX(void);
void saveAccY(void);
void saveAccZ(void);

int main(void){

	//Activación del coprocesador matemático
	SCB->CPACR |= (0xF << 20);

	//Sintonización para el HSI


	configPLL();
	initSystem();

	/*Loop forever*/
	while(1){

		//Creamos una cadena de caracteres con los datos que llegan por el puerto serial
		//El caracter '@' nos indica que es el final de la cadena
		if (rxData != '\0'){
			bufferReception[counterReception] = rxData;
			counterReception++;

			//if the incoming character is a newline, set a flag
			//so the main loop can do something about it:
			if(rxData == '@'){
				stringComplete = true;

				//Agrego esta línea para crear el string con null al final
				bufferReception[counterReception] = '\0';

				counterReception = 0;
			}

			//Para que no vuelva a entrar, Solo cambia debido a la interrupción
			rxData = '\0';
		}

		//Hacemos un análisis de la cadena de datos obtenida
		if(stringComplete){
			parseCommands(bufferReception);
			stringComplete = false;
		}
	}
	return 0;
}

void saveAccX(void) {

	uint8_t AccelX_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_L);
	uint8_t AccelX_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_H);
	int16_t AccelX = AccelX_high << 8 | AccelX_low;

	arrayX[indx] = AccelX;
}

void saveAccY(void) {

	uint8_t AccelY_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_L);
	uint8_t AccelY_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_H);
	int16_t AccelY = AccelY_high << 8 | AccelY_low;

	arrayY[indx] = AccelY;
}

void saveAccZ(void) {

	uint8_t AccelZ_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_L);
	uint8_t AccelZ_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_H);
	int16_t AccelZ = AccelZ_high << 8 | AccelZ_low;

	arrayZ[indx] = AccelZ;
}

void parseCommands(char *ptrBufferReception){

	/* Esta funcion de C lee la cadena de caracteres a la que apunta el "ptr"
	 * y almacena en tres elementos diferentes: un string llamado "cmd" y dos números
	 * integer llamados "firstParameter" y "secondParameter".
	 * De esta forma, podemos introducir información al micro desde el puerto serial
	 */
	sscanf(ptrBufferReception, "%s %u %u %s", cmd, &firstParameter, &secondParameter, userMsg);

	//El primer comando imprime una lista  con los otros comandos que tiene el equipo
	if(strcmp(cms, "help") == 0){

		writeMsg(&handlerUsart1, "Help Menu CMDs: \n");
		writeMsg(&handlerUsart1, "1) help   --  Print this menu \n");
		writeMsg(&handlerUsart1, "2) dummy #A #B   --  dummy cmd, #A and #B are uint32_t \n");
		writeMsg(&handlerUsart1, "3) usermsg # # msg  --  msg is a string coming from outside \n");
	}

	//El comando dummy sirve para entender como funciona la recepción de números enviados por consola
	else if(strcmp(cmd, "dummy") == 0){

		writeMsg(&handlerUsart1, "CMD: dummy \n");

		//Cambiando el formato para presentar el numero por el puerto serial
		sprintf(bufferData, "number A = %u \n", firstParameter);
		writeMsg(&handlerUsart1, bufferData);

		//Cambiando el formato para presentar el numero por el puerto serial
		sprintf(bufferData, "number B = %u \n", secondParameter);
		writeMsg(&handlerUsart1, bufferData);
	}

	//El comando usermsg sirve para entender como funciona la recepcion de strings enviados desde consola
	else if(strcmp(cmd, "usermsg") == 0){
		writeMsg(&handlerUsart1, "COMD: usermsg \n");
		writeMsg(&handlerUsart1, usermsg);
		writeMsg(&handlerUsart1, "\n");
	}
	else{
		//Se imprime el mensaje "Wrong CMD" si la escritura no corresponde a los CMD implementados
		writeMsg(&handlerUsart1, "Wrong CMD");
	}
}

void initSystem(void){

	//Configuración del Blinky
	handlerBlinky.pGPIOx = GPIOH;
	handlerBlinky.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerBlinky.GPIO_PinConfig.GPIO_PinNumber = PIN_1;
	handlerBlinky.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerBlinky.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEEDR_FAST;
	handlerBlinky.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerBlinky.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	GPIO_Config(&handlerBlinky);

	//Configuración del TIM2 (Blinky)
	handlerTimerBlinky.ptrTIMx = TIM2;
	handlerTimerBlinky.TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
	handlerTimerBlinky.TIMx_Config.TIMx_speed = BTIMER_SPEED_100us;
	handlerTimerBlinky.TIMx_Config.TIMx_period = 250;
	handlerTimerBlinky.TIMx_Config.TIMx_interruptEnable = 1;
	BasicTimer_Config(&handlerTimerBlinky);

	//Configuración del TIM5 (Muestreo acelerómetro)
	handlerTimerSamp.ptrTIMx = TIM5;
	handlerTimerSamp.TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
	handlerTimerSamp.TIMx_Config.TIMx_speed = BTIMER_SPEED_100us;
	handlerTimerSamp.TIMx_Config.TIMx_period = 10;
	handlerTimerSamp.TIMx_Config.TIMx_interruptEnable = 1;
	BasicTimer_Config(&handlerTimerSamp);

	//Configuración comunicación I2C (ACCEL)
	//SDA
	handlerI2cSDA.pGPIOx									= GPIOB;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinNumber				= PIN_9;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_OPENDRAIN;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEEDR_FAST;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinAltFunMode			= AF4;
	GPIO_Config(&handlerI2cSDA);

	//SCL
	handlerI2cSCL.pGPIOx									= GPIOB;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinNumber				= PIN_8;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_OPENDRAIN;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEEDR_FAST;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinAltFunMode			= AF4;
	GPIO_Config(&handlerI2cSCL);

	//Configuraciòn I2C (ACCEL)
	handlerAccelerometer.PLL_ON			= PLL_ENABLE;
	handlerAccelerometer.ptrI2Cx		= I2C1;
	handlerAccelerometer.slaveAddress	= ACCEL_ADDRESS;
	handlerAccelerometer.modeI2C		= I2C_MODE_FM;
	i2c_Config(&handlerAccelerometer);

	//Configuración de pines para USART1
	//TX Pin
	handlerPinTX1.pGPIOx									= GPIOA;
	handlerPinTX1.GPIO_PinConfig.GPIO_PinNumber				= PIN_9;
	handlerPinTX1.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerPinTX1.GPIO_PinConfig.GPIO_PinAltFunMode			= AF7;
	GPIO_Config(&handlerPinTX1);

	//RX Pin
	handlerPinRX1.pGPIOx									= GPIOA;
	handlerPinRX1.GPIO_PinConfig.GPIO_PinNumber				= PIN_10;
	handlerPinRX1.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerPinRX1.GPIO_PinConfig.GPIO_PinAltFunMode			= AF7;
	GPIO_Config(&handlerPinRX1);

	//Configuración de la comunicación serial
	handlerUsart1.ptrUSARTx	 								= USART1;
	handlerUsart1.USART_Config.USART_baudrate				= USART_BAUDRATE_115200;
	handlerUsart1.USART_Config.USART_PLL_EN					= PLL_ENABLE;
	handlerUsart1.USART_Config.USART_datasize				= USART_DATASIZE_8BIT;
	handlerUsart1.USART_Config.USART_parity					= USART_PARITY_NONE;
	handlerUsart1.USART_Config.USART_stopbits				= USART_STOPBIT_1;
	handlerUsart1.USART_Config.USART_mode					= USART_MODE_RXTX;
	handlerUsart1.USART_Config.USART_enableIntTX			= USART_TX_INTERRUP_ENABLE;
	handlerUsart1.USART_Config.USART_enableIntRX			= USART_RX_INTERRUP_ENABLE;
	USART_Config(&handlerUsart1);

	// Configuración del ADC
	channel_1.channel 			= ADC_CHANNEL_0;
	channel_1.dataAlignment 	= ADC_ALIGNMENT_RIGHT;
	channel_1.samplingPeriod 	= ADC_SAMPLING_PERIOD_84_CYCLES;
	channel_1.resolution		= ADC_RESOLUTION_12_BIT;

	channel_2.channel 			= ADC_CHANNEL_1;
	channel_2.dataAlignment 	= ADC_ALIGNMENT_RIGHT;
	channel_2.samplingPeriod 	= ADC_SAMPLING_PERIOD_84_CYCLES;
	channel_2.resolution		= ADC_RESOLUTION_12_BIT;
}

void usart1Rx_Callback(void){
	//Leemos el valor del registro DR, donde se almacena el dato que llega.
	//Esto ademàs debe bajar la bandera de la interrupciòn
	rxData = getRxData();
}

void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerBlinky);
}

void BasicTimer5_Callback(void){
	if (saveDataAccFlag == 0){
		return;
	}

	saveAccX();
	saveAccY();
	saveAccZ();

	indx++;

	if(indx >= 2000){
		indx = 0;
		saveDataAccFlag = 0;
	}
}
