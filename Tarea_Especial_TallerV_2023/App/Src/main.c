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
#include "PWMDriver.h"
#include "SysTickDriver.h"
#include "I2CxDriver.h"
#include "PLLDriver.h"

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

//Definición de los handlers
//Handlers GPIO
GPIO_Handler_t handlerBlinky 				= {0};		//Handler Blinky
GPIO_Handler_t handlerPinTX1				= {0};		//Handler Transmisión USART
GPIO_Handler_t handlerPinRX1				= {0};		//Handler Recepción USART
GPIO_Handler_t handlerPinPwmChannel1		= {0};		//Handler canal 1 PWM
GPIO_Handler_t handlerPinPwmChannel2		= {0};		//Handler canal 2 PWM
GPIO_Handler_t handlerPinPwmChannel3		= {0};		//Handler canal 3 PWM

//Handlers Timers
BasicTimer_Handler_t handlerTimerBlinky 	= {0};		//Handler Timer del blinky
BasicTimer_Handler_t handlerTimerSamp		= {0};		//Handler Timer del muestreo

//Handlers comunicación serial USART
USART_Handler_t handlerUsart1 				= {0};		//Handler USART1

//Handlers PWM
PWM_Handler_t handlerSignalPWM1 			= {0};		//PWM1
PWM_Handler_t handlerSignalPWM2 			= {0};		//PWM2
PWM_Handler_t handlerSignalPWM3			 	= {0};		//PWM3

//Handlers I2C (ACCEL)
GPIO_Handler_t handlerI2cSDA				= {0};		//Handler SDA I2C acelerómetro
GPIO_Handler_t handlerI2cSCL 				= {0};		//Handler SCL I2C acelerómetro
I2C_Handler_t handlerAccelerometer 			= {0};		//Handler de la configuración I2C acelerómetro

//Handlers I2C (LCD)
GPIO_Handler_t handlerLcdSda				= {0};		//Handler SDA I2C LCD
GPIO_Handler_t handlerLcdScl				= {0};		//Handler SCL I2C LCD
I2C_Handler_t handlerLCD					= {0};		//Handler de la configuración I2C LCD

//Definición de variables
uint8_t rxData = 0;
uint8_t i2cBuffer = 0;
uint16_t indx = 0;										//Índice para llenar los array del Accel
char bufferData[64] = "Accel MP-6050";
uint8_t saveDataAccFlag = 0;						    //Bandera para guadar datos del Acc en los array

//Definición de arreglos para guardar los muestreos del acelerómetro
uint16_t arrayX[2000] = {0};
uint16_t arrayY[2000] = {0};
uint16_t arrayZ[2000] = {0};
float converFact = (2.0/32767.0)*(9.8);			//Factor de conversión para los datos del accel

//Declaración de funciones
void configPLL(void);
void initSystem(void);
void ReadAccX(void);
void ReadAccY(void);
void ReadAccZ(void);

//Función principal del sistema
int main(void){

	//Activación del coprocesador matemático
	SCB->CPACR |= (0xF << 20);

	configPLL();
	initSystem();

	//Imprimir un mensaje de inicio
	writeMsgTX(&handlerUsart1, bufferData);

	/*Loop forever*/
	while(1){

		//Hacemos un eco con el valor que llega por el serial
		if(rxData != '\0'){
			writeCharTX(&handlerUsart1, rxData);

			if(rxData == 'w'){
				sprintf(bufferData, "WHO_AM_I? (r)\n");
				writeMsgTX(&handlerUsart1, bufferData);

				i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, WHO_AM_I);
				sprintf(bufferData, "dataRead = 0x%x \n", (unsigned int) i2cBuffer);
				writeMsgTX(&handlerUsart1, bufferData);
				rxData = '\0';
			}
			else if(rxData == 'p'){
				sprintf(bufferData, "PWR_MGMT_1 state (r)\n");
				writeMsgTX(&handlerUsart1, bufferData);

				i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, PWR_MGMT_1);
				sprintf(bufferData, "dataRead = 0x%x \n", (unsigned int) i2cBuffer);
				writeMsgTX(&handlerUsart1, bufferData);
				rxData = '\0';
				}
				else if(rxData == 'r'){
				sprintf(bufferData, "PWR_MGMT_1 reset (w)\n");
				writeMsgTX(&handlerUsart1, bufferData);

				i2c_writeSingleRegister(&handlerAccelerometer, PWR_MGMT_1, 0x00);
				rxData = '\0';
			}
			else if(rxData == 'x'){
				sprintf(bufferData, "Axis X data (r)\n");
				writeMsgTX(&handlerUsart1, bufferData);

				uint8_t AccelX_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_L);
				uint8_t AccelX_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_H);
				int16_t AccelX = AccelX_high << 8 | AccelX_low;

				sprintf(bufferData, "AccelX = %d \n", (int) AccelX);
				writeMsgTX(&handlerUsart1, bufferData);
				rxData = '\0';
			}
			else if(rxData == 'y'){
				sprintf(bufferData, "Axis Y data (r)\n");
				writeMsgTX(&handlerUsart1, bufferData);

				uint8_t AccelY_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_L);
				uint8_t AccelY_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_H);
				int16_t AccelY = AccelY_high << 8 | AccelY_low;
				sprintf(bufferData, "AccelY = %d \n", (int) AccelY);
				writeMsgTX(&handlerUsart1, bufferData);
				rxData = '\0';
			}
			else if(rxData == 'z'){
				sprintf(bufferData, "Axis Z data (r)\n");
				writeMsgTX(&handlerUsart1, bufferData);

				uint8_t AccelZ_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_L);
				uint8_t AccelZ_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_H);
				int16_t AccelZ = AccelZ_high << 8 | AccelZ_low;
				sprintf(bufferData, "AccelZ = %d \n", (int) AccelZ);
				writeMsgTX(&handlerUsart1, bufferData);
				rxData = '\0';
			}
			else if(rxData == 's'){

				sprintf(bufferData, "Sample axis X - Y - Z \n");
				writeMsgTX(&handlerUsart1, bufferData);

				sprintf(bufferData, "Wait for Data... \n");
				writeMsgTX(&handlerUsart1, bufferData);

				saveDataAccFlag = 1;

				while(saveDataAccFlag){
					__NOP();
				}

				sprintf(bufferData, "X ; Y ; Z \n");
				writeMsgTX(&handlerUsart1, bufferData);

				for(uint16_t i = 0; i < 2000; i++)
				{
					sprintf(bufferData, "%d | %.2f; %.2f; %.2f; \n", i+1, (float)arrayX[i]*converFact, (float)arrayY[i]*converFact, (float)arrayZ[i]*converFact);
					writeMsgTX(&handlerUsart1, bufferData);
				}
				rxData = '\0';

			}
			else{
				rxData = '\0';
			}
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

void initSystem(void){

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

	//Configuración del Pin Blinky
	handlerBlinky.pGPIOx = GPIOC;
	handlerBlinky.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerBlinky.GPIO_PinConfig.GPIO_PinNumber = PIN_0;
	handlerBlinky.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerBlinky.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEEDR_FAST;
	handlerBlinky.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerBlinky.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	GPIO_Config(&handlerBlinky);

	//Configuración del TIM2 (Blinky)
	handlerTimerBlinky.ptrTIMx = TIM2;
	handlerTimerBlinky.TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
	handlerTimerBlinky.TIMx_Config.TIMx_speed = BTIMER_SPEED_PLL_80_MHz_100us;
	handlerTimerBlinky.TIMx_Config.TIMx_period = 2500;
	handlerTimerBlinky.TIMx_Config.TIMx_interruptEnable = 1;
	BasicTimer_Config(&handlerTimerBlinky);

	//Configuración del TIM5 (Muestreo acelerómetro)
	handlerTimerSamp.ptrTIMx = TIM5;
	handlerTimerSamp.TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
	handlerTimerSamp.TIMx_Config.TIMx_speed = BTIMER_SPEED_PLL_80_MHz_100us;
	handlerTimerSamp.TIMx_Config.TIMx_period = 10;
	handlerTimerSamp.TIMx_Config.TIMx_interruptEnable = 1;
	BasicTimer_Config(&handlerTimerSamp);

//	//Configuración para el PWM 1
//	handlerSignalPWM1.ptrTIMx					= TIM3;
//	handlerSignalPWM1.config.channel			= PWM_CHANNEL_2;
//	handlerSignalPWM1.config.duttyCicle			= 95;
//	handlerSignalPWM1.config.periodo			= 100;
//	handlerSignalPWM1.config.prescaler			= BTIMER_SPEED_PLL_80_MHz_100us;
//	pwm_Config(&handlerSignalPWM1);
//
//	//Configuración del pin para la señal PWM1
//	handlerPinPwmChannel1.pGPIOx								= GPIOA;
//	handlerPinPwmChannel1.GPIO_PinConfig.GPIO_PinNumber			= PIN_7;
//	handlerPinPwmChannel1.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
//	handlerPinPwmChannel1.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
//	handlerPinPwmChannel1.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEEDR_FAST;
//	handlerPinPwmChannel1.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
//	handlerPinPwmChannel1.GPIO_PinConfig.GPIO_PinAltFunMode		= AF2;
//	GPIO_Config(&handlerPinPwmChannel1);
//
//	//Configuración para el PWM 2
//	handlerSignalPWM2.ptrTIMx					= TIM3;
//	handlerSignalPWM2.config.channel			= PWM_CHANNEL_2;
//	handlerSignalPWM2.config.duttyCicle			= 20;
//	handlerSignalPWM2.config.periodo			= 100;
//	handlerSignalPWM2.config.prescaler			= BTIMER_SPEED_PLL_80_MHz_100us;
//	pwm_Config(&handlerSignalPWM2);
//
//	//Configuración del pin para la señal PWM2
//	handlerPinPwmChannel2.pGPIOx								= GPIOA;
//	handlerPinPwmChannel2.GPIO_PinConfig.GPIO_PinNumber			= PIN_7;
//	handlerPinPwmChannel2.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
//	handlerPinPwmChannel2.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
//	handlerPinPwmChannel2.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEEDR_FAST;
//	handlerPinPwmChannel2.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
//	handlerPinPwmChannel2.GPIO_PinConfig.GPIO_PinAltFunMode		= AF2;
//	GPIO_Config(&handlerPinPwmChannel2);
//
//	//Configuración para el PWM 3
//	handlerSignalPWM3.ptrTIMx					= TIM3;
//	handlerSignalPWM3.config.channel			= PWM_CHANNEL_3;
//	handlerSignalPWM3.config.duttyCicle			= 50;
//	handlerSignalPWM3.config.periodo			= 100;
//	handlerSignalPWM3.config.prescaler			= BTIMER_SPEED_PLL_80_MHz_100us;
//	pwm_Config(&handlerSignalPWM3);
//
//	//Configuración del pin para la señal PWM3
//	handlerPinPwmChannel3.pGPIOx								= GPIOB;
//	handlerPinPwmChannel3.GPIO_PinConfig.GPIO_PinNumber			= PIN_0;
//	handlerPinPwmChannel3.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
//	handlerPinPwmChannel3.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
//	handlerPinPwmChannel3.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEEDR_FAST;
//	handlerPinPwmChannel3.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
//	handlerPinPwmChannel3.GPIO_PinConfig.GPIO_PinAltFunMode		= AF2;
//	GPIO_Config(&handlerPinPwmChannel3);

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

//	//Configuración comunicaión I2C (LCD)
//	//SDA
//	handlerLcdSda.pGPIOx									= GPIOB;
//	handlerLcdSda.GPIO_PinConfig.GPIO_PinNumber				= PIN_9;
//	handlerLcdSda.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
//	handlerLcdSda.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_OPENDRAIN;
//	handlerLcdSda.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
//	handlerLcdSda.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEEDR_FAST;
//	handlerLcdSda.GPIO_PinConfig.GPIO_PinAltFunMode			= AF4;
//	GPIO_Config(&handlerLcdSda);
//
//	//SCL
//	handlerLcdScl.pGPIOx									= GPIOB;
//	handlerLcdScl.GPIO_PinConfig.GPIO_PinNumber				= PIN_8;
//	handlerLcdScl.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
//	handlerLcdScl.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_OPENDRAIN;
//	handlerLcdScl.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
//	handlerLcdScl.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEEDR_FAST;
//	handlerLcdScl.GPIO_PinConfig.GPIO_PinAltFunMode			= AF4;
//	GPIO_Config(&handlerLcdScl);
//
//	//Configuración I2C (LCD)
//	handlerLCD.PLL_ON			= PLL_ENABLE;
//	handlerLCD.ptrI2Cx			= I2C3;
//	handlerLCD.slaveAddress		= 0;
//	handlerLCD.modeI2C			= I2C_MODE_FM;
//	i2c_Config(&handlerLCD);
//
//	//LA LCD NO FUNCIONOÓ ALV XD
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


