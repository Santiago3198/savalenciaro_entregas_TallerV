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

//Definiciones para comunicación I2C (LCD)
//#define LCD_ADDRESS		00000000;		//ID Device

//Definición de los handlers
//Handlers GPIO
GPIO_Handler_t handlerBlinky 				= {0};
GPIO_Handler_t handlerPinTX1				= {0};
GPIO_Handler_t handlerPinRX1				= {0};
GPIO_Handler_t handlerPinTX2				= {0};
GPIO_Handler_t handlerPinRX2				= {0};

//Handlers Timers
BasicTimer_Handler_t handlerTimerBlinky 	= {0};

//Handlers comunicación serial USART
USART_Handler_t handlerUsart1 				= {0};
USART_Handler_t usart2Comm					= {0};

//Handlers I2C (ACCEL)
GPIO_Handler_t handlerI2cSDA				= {0};
GPIO_Handler_t handlerI2cSCL 				= {0};
I2C_Handler_t handlerAccelerometer 			= {0};

//Handlers I2C (LCD)
GPIO_Handler_t handlerLcdSda				= {0};
GPIO_Handler_t handlerLcdScl				= {0};
I2C_Handler_t handlerLCD					= {0};

//Definición de variables
uint8_t rxData = 0;
uint8_t i2cBuffer = 0;
char bufferData[64] = "Accel MP-6050";

//Declaración de funciones
void configPLL(void);
void initSystem(void);

//Función principal del sistema
int main(void){

	initSystem();
	configPLL();

	//Imprimir un mensaje de inicio
	writeMsg(&usart2Comm, bufferData);

	/*Loop forever*/
	while(1){

		//Hacemos un eco con el valor que llega por el serial
		if(rxData != '\0'){
			writeChar(&usart2Comm, rxData);

			if(rxData == 'w'){
				sprintf(bufferData, "WHO_AM_I? (r)\n");
				writeMsg(&usart2Comm, bufferData);

				i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, WHO_AM_I);
				sprintf(bufferData, "dataRead = 0x%x \n", (unsigned int) i2cBuffer);
				writeMsg(&usart2Comm, bufferData);
				rxData = '\0';
			}
			else if(rxData == 'p'){
				sprintf(bufferData, "PWR_MGMT_1 state (r)\n");
				writeMsg(&usart2Comm, bufferData);

				i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, PWR_MGMT_1);
				sprintf(bufferData, "dataRead = 0x%x \n", (unsigned int) i2cBuffer);
				writeMsg(&usart2Comm, bufferData);
				rxData = '\0';
				}
				else if(rxData == 'r'){
				sprintf(bufferData, "PWR_MGMT_1 reset (w)\n");
				writeMsg(&usart2Comm, bufferData);

				i2c_writeSingleRegister(&handlerAccelerometer, PWR_MGMT_1, 0x00);
				rxData = '\0';
			}
			else if(rxData == 'x'){
				sprintf(bufferData, "Axis X data (r)\n");
				writeMsg(&usart2Comm, bufferData);

				uint8_t AccelX_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_L);
				uint8_t AccelX_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_H);
				int16_t AccelX = AccelX_high << 8 | AccelX_low;

				sprintf(bufferData, "AccelX = %d \n", (int) AccelX);
				writeMsg(&usart2Comm, bufferData);
				rxData = '\0';
			}
			else if(rxData == 'y'){
				sprintf(bufferData, "Axis Y data (r)\n");
				writeMsg(&usart2Comm, bufferData);

				uint8_t AccelY_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_L);
				uint8_t AccelY_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_H);
				int16_t AccelY = AccelY_high << 8 | AccelY_low;
				sprintf(bufferData, "AccelY = %d \n", (int) AccelY);
				writeMsg(&usart2Comm, bufferData);
				rxData = '\0';
			}
			else if(rxData == 'z'){
				sprintf(bufferData, "Axis Z data (r)\n");
				writeMsg(&usart2Comm, bufferData);

				uint8_t AccelZ_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_L);
				uint8_t AccelZ_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_H);
				int16_t AccelZ = AccelZ_high << 8 | AccelZ_low;
				sprintf(bufferData, "AccelZ = %d \n", (int) AccelZ);
				writeMsg(&usart2Comm, bufferData);
				rxData = '\0';
			}
			else{
				rxData = '\0';
			}
		}
	}
	return 0;
}

////Hacemos un eco con el valor que llega por el serial
//		if(rxData != '\0'){
//			writeChar(&handlerUsart1, rxData);
//
//			if(rxData == 'w'){
//				sprintf(bufferData, "WHO_AM_I? (r)\n");
//				writeMsg(&handlerUsart1, bufferData);
//
//				i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, WHO_AM_I);
//				sprintf(bufferData, "dataRead = 0x%x \n", (unsigned int) i2cBuffer);
//				writeMsg(&handlerUsart1, bufferData);
//				rxData = '\0';
//			}
//			else if(rxData == 'p'){
//				sprintf(bufferData, "PWR_MGMT_1 state (r)\n");
//				writeMsg(&handlerUsart1, bufferData);
//
//				i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, PWR_MGMT_1);
//				sprintf(bufferData, "dataRead = 0x%x \n", (unsigned int) i2cBuffer);
//				writeMsg(&handlerUsart1, bufferData);
//				rxData = '\0';
//				}
//				else if(rxData == 'r'){
//				sprintf(bufferData, "PWR_MGMT_1 reset (w)\n");
//				writeMsg(&handlerUsart1, bufferData);
//
//				i2c_writeSingleRegister(&handlerAccelerometer, PWR_MGMT_1, 0x00);
//				rxData = '\0';
//			}
//			else if(rxData == 'x'){
//				sprintf(bufferData, "Axis X data (r)\n");
//				writeMsg(&handlerUsart1, bufferData);
//
//				uint8_t AccelX_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_L);
//				uint8_t AccelX_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_H);
//				int16_t AccelX = AccelX_high << 8 | AccelX_low;
//
//				sprintf(bufferData, "AccelX = %d \n", (int) AccelX);
//				writeMsg(&handlerUsart1, bufferData);
//				rxData = '\0';
//			}
//			else if(rxData == 'y'){
//				sprintf(bufferData, "Axis Y data (r)\n");
//				writeMsg(&handlerUsart1, bufferData);
//
//				uint8_t AccelY_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_L);
//				uint8_t AccelY_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_H);
//				int16_t AccelY = AccelY_high << 8 | AccelY_low;
//				sprintf(bufferData, "AccelY = %d \n", (int) AccelY);
//				writeMsg(&handlerUsart1, bufferData);
//				rxData = '\0';
//			}
//			else if(rxData == 'z'){
//				sprintf(bufferData, "Axis Z data (r)\n");
//				writeMsg(&handlerUsart1, bufferData);
//
//				uint8_t AccelZ_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_L);
//				uint8_t AccelZ_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_H);
//				int16_t AccelZ = AccelZ_high << 8 | AccelZ_low;
//				sprintf(bufferData, "AccelZ = %d \n", (int) AccelZ);
//				writeMsg(&handlerUsart1, bufferData);
//				rxData = '\0';
//			}
//			else{
//				rxData = '\0';
//			}
//		}
//	}
//	return 0;

void initSystem(void){

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
	handlerTimerBlinky.TIMx_Config.TIMx_speed = BTIMER_SPEED_PLL_80_MHz_100us;  //BTIMER_SPEED_PLL_80_MHz_100us
	handlerTimerBlinky.TIMx_Config.TIMx_period = 2500;
	handlerTimerBlinky.TIMx_Config.TIMx_interruptEnable = 1;
	BasicTimer_Config(&handlerTimerBlinky);

	//Configuración de pines para USART1
	//TX Pin
	handlerPinTX1.pGPIOx									= GPIOA;
	handlerPinTX1.GPIO_PinConfig.GPIO_PinNumber				= PIN_15;
	handlerPinTX1.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerPinTX1.GPIO_PinConfig.GPIO_PinAltFunMode			= AF7;
	GPIO_Config(&handlerPinTX1);

	//RX Pin
	handlerPinRX1.pGPIOx									= GPIOB;
	handlerPinRX1.GPIO_PinConfig.GPIO_PinNumber				= PIN_7;
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
	handlerUsart1.USART_Config.USART_enableIntTX			= USART_TX_INTERRUP_DISABLE;
	handlerUsart1.USART_Config.USART_enableIntRX			= USART_RX_INTERRUP_ENABLE;
	USART_Config(&handlerUsart1);

	//Comunicación serial de prueba
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
	usart2Comm.USART_Config.USART_baudrate					= USART_BAUDRATE_115200;
	usart2Comm.USART_Config.USART_PLL_EN					= PLL_ENABLE;
	usart2Comm.USART_Config.USART_datasize					= USART_DATASIZE_8BIT;
	usart2Comm.USART_Config.USART_parity					= USART_PARITY_NONE;
	usart2Comm.USART_Config.USART_stopbits					= USART_STOPBIT_1;
	usart2Comm.USART_Config.USART_mode						= USART_MODE_RXTX;
	usart2Comm.USART_Config.USART_enableIntTX				= USART_TX_INTERRUP_DISABLE;
	usart2Comm.USART_Config.USART_enableIntRX				= USART_RX_INTERRUP_ENABLE;
	USART_Config(&usart2Comm);

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
//	handlerLCD.PLL_ON			= PLL_DISABLE;
//	handlerLCD.ptrI2Cx			= I2C1;
//	handlerLCD.slaveAddress		= LCD_ADDRESS;
//	handlerLCD.modeI2C			= I2C_MODE_FM;
//	i2c_Config(&handlerLCD);
}

void usart2Rx_Callback(void){
	//Leemos el valor del registro DR, donde se almacena el dato que llega.
	//Esto ademàs debe bajar la bandera de la interrupciòn
	rxData = getRxData();
}

void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerBlinky);
}


