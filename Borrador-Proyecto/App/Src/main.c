/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Santiago Valencia Roldan 
 * @brief          : Programa principal proyecto final
 ******************************************************************************
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
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
#include "RTCxDriver.h"

#include "arm_math.h"

//Definición de handlers GPIO
GPIO_Handler_t handlerBlinky 				= {0};		//Handler Blinky
GPIO_Handler_t handlerPinTX1				= {0};		//Handler Transmisión USART
GPIO_Handler_t handlerPinRX1				= {0};		//Handler Recepción USART
GPIO_Handler_t handlerValvePin				= {0};		//Handler Válvula

//Definición de handlers TIM
BasicTimer_Handler_t handlerTimerBlinky 	= {0};		//Handler Timer del blinky

//Handlers comunicación serial USART
USART_Handler_t handlerUsart1 				= {0};		//Handler USART1

//Handlers I2C
GPIO_Handler_t handlerI2cSDA				= {0};		//Handler SDA I2C acelerómetro
GPIO_Handler_t handlerI2cSCL 				= {0};		//Handler SCL I2C acelerómetro
I2C_Handler_t handlerAccelerometer 			= {0};		//Handler de la configuración I2C acelerómetro
I2C_Handler_t handlerBarometer				= {0};		//Handler de la configuración I2C barómetro

//Definición de variables
uint8_t rxData = 0;

//Variables relacionadas con la comunicación I2C del Accel
uint8_t i2cBuffer = 0;
uint16_t indx = 0;
uint8_t saveDataAccFlag = 0;
char bufferData[64] = "Module GY-88";

//Variables relacionadas con el uso de los comandos en terminal
bool stringComplete;
uint16_t counterReception = 0;
char cmd[256] = {0};
char userMsg[256] = {0};
char bufferReception[256] = {0};
unsigned int firstParameter = 0;
unsigned int secondParameter = 0;
unsigned int thirdParameter = 0;

float converFact = (2/32767.0)*(9.8);			//Factor de conversión para los datos del accel

long Press = 0;
long Temp = 0;
long Altitude = 0;
long p0 = 1013.25;
long expo = 1/5.255;

//Deficiniones para Barómetro
//Definiciones de constantes de calibración
short AC1 = 0;
short AC2 = 0;
short AC3 = 0;
unsigned short AC4 = 0;
unsigned short AC5 = 0;
unsigned short AC6 = 0;
short B1 = 0;
short B2 = 0;
short MB = 0;
short MC = 0;
short MD = 0;

long UT = 0;
long UP = 0;
short oss = 3; 		//Oversampling Settings

long X1 = 0;
long X2 = 0;
long X3 = 0;
long B3 = 0;
unsigned long B4 = 0;
long B5 = 0;
long B6 = 0;
unsigned long B7 = 0;
long T = 0;
long P = 0;

long p = 0;

//Definiciones para comunicación I2C (ACCEL)
#define ACCEL_ADDRESS 	0b1101001;		//ID Device
#define ACCEL_XOUT_H	59 				//0x3B
#define ACCEL_XOUT_L 	60 				//0x3C
#define ACCEL_YOUT_H	61				//0x3D
#define ACCEL_YOUT_L	62 				//0x3E
#define ACCEL_ZOUT_H	63				//0x3F
#define ACCEL_ZOUT_L	64				//0x40

//Definiciones para comunicación I2C (GYRO)
#define GYRO_XOUT_H		67				//0x43
#define GYRO_XOUT_L		68				//0x44
#define GYRO_YOUT_H		69				//0x45
#define GYRO_YOUT_L		70				//0x46
#define GYRO_ZOUT_H		71				//0x47
#define GYRO_ZOUT_L		72				//0x48

#define PWR_MGMT_1		107				//0x40
#define WHO_AM_I		117				//0x68

//Definiciones para comunicación I2C (BAR)
#define BAR_ADDRESS		0b1110111;		//ID Device

//Definición de funciones
void initSystem(void);
void BMP085(void);
void parseCommands(char *ptrBufferReception);

int main(void){

	//Sintonización del HSI
	RCC->CR &= ~RCC_CR_HSITRIM;
	RCC->CR |= (13 << RCC_CR_HSITRIM_Pos);

	//Activación del coprocesador matemático
	SCB->CPACR |= (0xF << 20);

	initSystem();

	writeMsg(&handlerUsart1, "\n~Iniciando Sistema~\n");
	GPIO_WritePin(&handlerValvePin, RESET);

	/*Loop forever*/
	while(1){

		//Creamos una cadena de caracteres con los datos que llegan por el puerto serial
		//El caracter '@' nos indica que es el final de la cadena

		if (rxData != '\0'){
			bufferReception[counterReception] = rxData;
			counterReception++;

			//Se define el siguiente caracter para indicar que el string está completo
			if(rxData == '@'){

				stringComplete = true;

				//Agrego esta línea para crear el string con null al final
				bufferReception[counterReception-1] = '\0';

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

void parseCommands(char *ptrBufferReception){

	/* Esta funcion de C lee la cadena de caracteres a la que apunta el "ptr"
	 * y almacena en tres elementos diferentes: un string llamado "cmd" y dos números
	 * integer llamados "firstParameter" y "secondParameter".
	 * De esta forma, podemos introducir información al micro desde el puerto serial
	 */
	sscanf(ptrBufferReception, "%s %s %u %u %u", cmd, userMsg, &firstParameter, &secondParameter, &thirdParameter);

	//El primer comando imprime una lista  con los otros comandos que tiene el equipo
	if(strcmp(cmd, "help") == 0){

		writeMsg(&handlerUsart1, "\n Help Menu CMDs: \n");
		writeMsg(&handlerUsart1, "\n Por favor ingrese un comando seguido de los parametros a modificar separados por espacios \n");
		writeMsg(&handlerUsart1, "\n 1) help   -->   Menu de ayuda \n");
		writeMsg(&handlerUsart1, "\n 2) device  -->  WHO I AM? \n");
		writeMsg(&handlerUsart1, "\n 3) stateAcc  -->  Carga el estado del Accel \n");
		writeMsg(&handlerUsart1, "\n 4) resetAcc  -->  Resetea la configuracion del Accel \n");
		writeMsg(&handlerUsart1, "\n 5) calibrateAcc  -->  Calibración del Accel-Gyro \n");
		writeMsg(&handlerUsart1, "\n 6) showAcc  -->  Presenta los datos actuales capturados por el acelerómetro \n");
		writeMsg(&handlerUsart1, "\n 7) showBar  -->  Presenta los datos actuales capturados por el barómetro \n");
		writeMsg(&handlerUsart1, "\n 8) valve  -->  Alto o bajo para cerrar o abrir la valvula de combustible \n");
	}
	else if(strcmp(cmd, "device") == 0){

		//Se lee el dispositivo para realizar la comunicación
		sprintf(bufferData, "\nWHO_AM_I? (r)\n");
		writeMsgTX(&handlerUsart1, bufferData);

		i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, WHO_AM_I);
		sprintf(bufferData, "\ndataRead = 0x%x \n", (unsigned int) i2cBuffer);
		writeMsgTX(&handlerUsart1, bufferData);
		rxData = '\0';
	}
	else if(strcmp(cmd, "stateAcc") == 0){

		sprintf(bufferData, "\nPWR_MGMT_1 state (r)\n");
		writeMsgTX(&handlerUsart1, bufferData);

		i2cBuffer = i2c_readSingleRegister(&handlerAccelerometer, PWR_MGMT_1);
		sprintf(bufferData, "\ndataRead = 0x%x \n", (unsigned int) i2cBuffer);
		writeMsgTX(&handlerUsart1, bufferData);
		rxData = '\0';
	}
	else if(strcmp(cmd, "resetAcc") == 0){

		//Se reinicia la configuración del accel
		sprintf(bufferData, "\nPWR_MGMT_1 reset (w)\n");
		writeMsgTX(&handlerUsart1, bufferData);

		i2c_writeSingleRegister(&handlerAccelerometer, PWR_MGMT_1, 0x00);
		rxData = '\0';
	}
	else if(strcmp(cmd, "calibrateAcc") == 0){

		//Se escriben los registros de calibración para el Accel
		//Se reinician las rutas de las señales del Accel y el Gyro
		i2c_writeSingleRegister(&handlerAccelerometer, 0x68, 0b11 << 1);

		//Se selecciona el modo de ultra alta resolución para el Accel
		i2c_writeSingleRegister(&handlerAccelerometer, 0x1C, 0b00 << 4);

		//Se selecciona el modo de ultra alta resolución para el Gyro
		i2c_writeSingleRegister(&handlerAccelerometer, 0x1B, 0b00 << 4);

		//Desactivamos el sensor de temperatura integrado en el MPU6050
		i2c_writeSingleRegister(&handlerAccelerometer, 0x6B, 0b01 << 3);

		writeMsg(&handlerUsart1, "\nAccel calibrado correctamente \n");
	}
	else if(strcmp(cmd, "valve") == 0){

		//Se cambia el estado el estado del pin
		GPIOxTooglePin(&handlerValvePin);

		if(GPIO_ReadPin(&handlerValvePin) == 1){
			writeMsg(&handlerUsart1, "\nVálvula abierta \n");
		}
		else{
			writeMsg(&handlerUsart1, "\nVálvula cerrada \n");
		}
	}
	else if(strcmp(cmd, "showAcc") == 0){

		uint8_t AccelX_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_L);
		uint8_t AccelX_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_XOUT_H);
		int16_t AccelX = AccelX_high << 8 | AccelX_low;

		uint8_t AccelY_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_L);
		uint8_t AccelY_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_YOUT_H);
		int16_t AccelY = AccelY_high << 8 | AccelY_low;

		uint8_t AccelZ_low = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_L);
		uint8_t AccelZ_high = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_ZOUT_H);
		int16_t AccelZ = AccelZ_high << 8 | AccelZ_low;

		uint8_t GyrX_low = i2c_readSingleRegister(&handlerAccelerometer, GYRO_XOUT_L);
		uint8_t GyrX_high = i2c_readSingleRegister(&handlerAccelerometer, GYRO_XOUT_H);
		int16_t GyrX = GyrX_high << 8 | GyrX_low;

		uint8_t GyrY_low = i2c_readSingleRegister(&handlerAccelerometer, GYRO_YOUT_L);
		uint8_t GyrY_high = i2c_readSingleRegister(&handlerAccelerometer, GYRO_YOUT_H);
		int16_t GyrY = GyrY_high << 8 | GyrY_low;

		uint8_t GyrZ_low = i2c_readSingleRegister(&handlerAccelerometer, GYRO_ZOUT_L);
		uint8_t GyrZ_high = i2c_readSingleRegister(&handlerAccelerometer, GYRO_ZOUT_H);
		int16_t GyrZ = GyrZ_high << 8 | GyrZ_low;

		sprintf(bufferData, "\nLa aceleración en X es: %.2f m/s² \n", (float)AccelX*converFact);
		writeMsg(&handlerUsart1, bufferData);

		sprintf(bufferData, "\nLa aceleración en Y es: %.2f m/s² \n", (float)AccelY*converFact);
		writeMsg(&handlerUsart1, bufferData);

		sprintf(bufferData, "\nLa aceleración en Z es: %.2f m/s² \n", (float)AccelZ*converFact);
		writeMsg(&handlerUsart1, bufferData);

		sprintf(bufferData, "\nEl ángulo en X es: %.2f \n", (float)GyrX);
		writeMsg(&handlerUsart1, bufferData);

		sprintf(bufferData, "\nEl ángulo en Y es: %.2f \n", (float)GyrY);
		writeMsg(&handlerUsart1, bufferData);

		sprintf(bufferData, "\nEl ángulo en Z es: %.2f \n", (float)GyrZ);
		writeMsg(&handlerUsart1, bufferData);
		rxData = '\0';
	}
	else if(strcmp(cmd, "showBar") == 0){

		BMP085();

		Altitude = 44330 * (1-((p/p0)^expo));

		sprintf(bufferData, "\nLa presión es: %.2f \n", (float)Press);
		writeMsg(&handlerUsart1, bufferData);

		sprintf(bufferData, "\nLa temperatura es: %.2f \n", (float)Temp);
		writeMsg(&handlerUsart1, bufferData);

		sprintf(bufferData, "\nLa altura es: %.2f \n", (float)Altitude);
		writeMsg(&handlerUsart1, bufferData);
		rxData = '\0';
	}
}

void initSystem(void){

	//Configuración del Blinky
	handlerBlinky.pGPIOx 									= GPIOH;
	handlerBlinky.GPIO_PinConfig.GPIO_PinNumber 			= PIN_1;
	handlerBlinky.GPIO_PinConfig.GPIO_PinMode 				= GPIO_MODE_OUT;
	handlerBlinky.GPIO_PinConfig.GPIO_PinOPType 			= GPIO_OTYPE_PUSHPULL;
	handlerBlinky.GPIO_PinConfig.GPIO_PinSpeed 				= GPIO_OSPEEDR_FAST;
	handlerBlinky.GPIO_PinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerBlinky);

	//Configuración del TIM2 (Blinky)
	handlerTimerBlinky.ptrTIMx 								= TIM2;
	handlerTimerBlinky.TIMx_Config.TIMx_mode 				= BTIMER_MODE_UP;
	handlerTimerBlinky.TIMx_Config.TIMx_speed 				= BTIMER_SPEED_100us;
	handlerTimerBlinky.TIMx_Config.TIMx_period 				= 2500;
	handlerTimerBlinky.TIMx_Config.TIMx_interruptEnable 	= 1;
	BasicTimer_Config(&handlerTimerBlinky);

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
	handlerAccelerometer.PLL_ON								= PLL_DISABLE;
	handlerAccelerometer.ptrI2Cx							= I2C1;
	handlerAccelerometer.slaveAddress						= ACCEL_ADDRESS;
	handlerAccelerometer.modeI2C							= I2C_MODE_FM;
	i2c_Config(&handlerAccelerometer);

	//Configuraciòn I2C (BAR)
	handlerBarometer.PLL_ON									= PLL_DISABLE;
	handlerBarometer.ptrI2Cx								= I2C1;
	handlerBarometer.slaveAddress							= BAR_ADDRESS;
	handlerBarometer.modeI2C								= I2C_MODE_FM;
	i2c_Config(&handlerBarometer);

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
	handlerUsart1.USART_Config.USART_baudrate				= USART_BAUDRATE_9600;
	handlerUsart1.USART_Config.USART_PLL_EN					= PLL_DISABLE;
	handlerUsart1.USART_Config.USART_datasize				= USART_DATASIZE_8BIT;
	handlerUsart1.USART_Config.USART_parity					= USART_PARITY_NONE;
	handlerUsart1.USART_Config.USART_stopbits				= USART_STOPBIT_1;
	handlerUsart1.USART_Config.USART_mode					= USART_MODE_RXTX;
	handlerUsart1.USART_Config.USART_enableIntTX			= USART_TX_INTERRUP_DISABLE;
	handlerUsart1.USART_Config.USART_enableIntRX			= USART_RX_INTERRUP_ENABLE;
	USART_Config(&handlerUsart1);

	//Configuración Pin de salida
	handlerValvePin.pGPIOx 									= GPIOA;
	handlerValvePin.GPIO_PinConfig.GPIO_PinNumber 			= PIN_0;
	handlerValvePin.GPIO_PinConfig.GPIO_PinMode 			= GPIO_MODE_OUT;
	handlerValvePin.GPIO_PinConfig.GPIO_PinOPType 			= GPIO_OTYPE_PUSHPULL;
	handlerValvePin.GPIO_PinConfig.GPIO_PinSpeed 			= GPIO_OSPEEDR_FAST;
	handlerValvePin.GPIO_PinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_NOTHING;
	handlerValvePin.GPIO_PinConfig.GPIO_PinAltFunMode 		= AF0;
	GPIO_Config(&handlerValvePin);
}

void BMP085(void){

	//Se configura el SysTick en 16 MHz
	config_SysTick_ms(0);

	//Guardar en variables el valor que hay en cada registro para los datos de calibración
	AC1 = i2c_readSingleRegister(&handlerBarometer, 0xAA);
	AC2 = i2c_readSingleRegister(&handlerBarometer, 0xAC);
	AC3 = i2c_readSingleRegister(&handlerBarometer, 0xAE);
	AC4 = i2c_readSingleRegister(&handlerBarometer, 0xB0);
	AC5 = i2c_readSingleRegister(&handlerBarometer, 0xB2);
	AC6 = i2c_readSingleRegister(&handlerBarometer, 0xB4);
	B1 = i2c_readSingleRegister(&handlerBarometer, 0xB6);
	B2 = i2c_readSingleRegister(&handlerBarometer, 0xB8);
	MB = i2c_readSingleRegister(&handlerBarometer, 0xBA);
	MC = i2c_readSingleRegister(&handlerBarometer, 0xBC);
	MD = i2c_readSingleRegister(&handlerBarometer, 0xBE);

	/* Leer el valor de la temperatura no compensado
	 */

	i2c_writeSingleRegister(&handlerBarometer, 0xF4, 0x2E);

	//Tiempo de espera para reescribir el registro
	delay_ms(5);

	//Se guarda el valor en la variable de la temperatura
	uint8_t Temp_low = i2c_readSingleRegister(&handlerBarometer, 0xF7);
	uint8_t Temp_high = i2c_readSingleRegister(&handlerBarometer, 0xF6);
	UT = (Temp_high << 8) + Temp_low;

	/* Leer el valor de la presión no compensado
	 */

	//Se configura par que tome las medidas en ultra alta resolución
	i2c_writeSingleRegister(&handlerBarometer, 0xF4, 0x34+(oss<<6));

	//Tiempo de espera para reescribir el registro
	delay_ms(5);

	//Se guarda el valor en la variable de la temperatura
	uint8_t Press_low = i2c_readSingleRegister(&handlerBarometer, 0xF7);
	uint8_t Press_high = i2c_readSingleRegister(&handlerBarometer, 0xF6);
	uint8_t Press_Xlow = i2c_readSingleRegister(&handlerBarometer, 0xF8);
	UP = ((Press_high<<16) + (Press_low<<8) + Press_Xlow) >> (8-oss);

	//Calcular la temperatura real con los datos de calibración
	X1 = (UT-AC6)*AC5/(2^15);
	X2 = MC*(2^11)/(X1 + MD);
	B5 = X1 + X2;
	T = (B5 + 8)/(2^4);

	//Guardamos el valor de la temperatura para posteriormente leerlo
	Temp = T;

	//Calcular la presión real con los datos de calibración
	B6 = B5 - 4000;
	X1 = (B2 * (B6 * B6/(2^12))) / (2^11);
	X2 = (AC2 * B6) / (2^11);
	X3 = X1 + X2;
	B3 = (((AC1*4+X3) << oss) + 2)/4;
	X1 = (AC3 * B6) / (2^13);
	X2 = (B1 * (B6 * B6 / (2^12)))/(2^16);
	X3 = ((X1+X2)+2) / (4) ;
	B4 = AC4 * (unsigned long)(X3 + 32768) / (2^15);
	B7 = ((unsigned long)UP - B3) * (50000 >> oss);

	if(B7 < 0x80000000){
		p = (B7/2) * B4;
	}
	else{
		p = (B7/B4) * 2;
	}

	X1 = (p/(2^8)) * (p/(2^8));
	X1 = (X1 * 3038)/(2^16);
	X2 = (-7357 * p)/(2^16);
	P = p + (X1 + X2 + 3791)/(2^4);

	Press = P;
}

void usart1Rx_Callback(void){
	//Leemos el valor del registro DR, donde se almacena el dato que llega.
	rxData = getRxData();
}

void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerBlinky);
}
