/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Santiago Valencia Roldan 
 * @brief          : Main program body
 ******************************************************************************
 * Descomentar la siguiente linea y agregar al main si se requiere el uso
 * de la FPU.
 * SCB->CPACR |= (0xF << 20);
 * Activacion del coprocesador matematico
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
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
GPIO_Handler_t handlerMCO1					= {0};		//Handler para probar el HSI
GPIO_Handler_t handlerPwmPin 				= {0};		//Handler Pin PWM del ADC

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
ADC_Config_t channel 						= {0};

//Configuración PWM
PWM_Handler_t handlerPWM 					= {0};

//Definición de variables
uint8_t rxData = 0;

//Variables relacionadas con la comunicación I2C del Accel
uint8_t i2cBuffer = 0;
uint16_t indx = 0;
uint8_t saveDataAccFlag = 0;
char bufferData[64] = "Accel MP-6050";

//Variables relacionadas con el ADC
uint8_t adcIsComplete	= 0;
uint8_t adcCounter 		= 0;
uint16_t dataADC[2] 	= {0};
char bufferDataAdc[128] = {0};

unsigned int speed = 0;
uint16_t duttyPwm = 0;
unsigned int dataCh1[256] = {0};
unsigned int dataCh2[256] = {0};
uint16_t dataCounter = 0;

//Variables relacionadas con el uso de los comandos en terminal
bool stringComplete;
char cmd[256] = {0};
char userMsg[256] = {0};
char bufferReception[256] = {0};
uint16_t counterReception = 0;
unsigned int firstParameter = 0;
unsigned int secondParameter = 0;

//Definición de arreglos para guardar los muestreos del acelerómetro
uint16_t arrayX[2000] = {0};
uint16_t arrayY[2000] = {0};
uint16_t arrayZ[2000] = {0};
float converFact = (2.0/32767.0)*(9.8);			//Factor de conversión para los datos del accel

//Definición número de canales para usar el ADC
#define CHANNELS   2

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
void parseCommands(char *ptrBufferReception);
void ADC_ConfigMultichannel(ADC_Config_t *adcConfig, uint8_t numeroDeCanales);
void show_Data_Adc(void);

int main(void){

	//Sintonización del HSI
	RCC->CR &= ~RCC_CR_HSITRIM;
	RCC->CR |= (13 << RCC_CR_HSITRIM_Pos);

	//Activación del coprocesador matemático
	SCB->CPACR |= (0xF << 20);

	configPLL();
	initSystem();

	writeMsg(&handlerUsart1, "\n~Iniciando Sistema~\n");

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
				bufferReception[counterReception-1] = '\0';

				counterReception = 0;
			}
			//Para que no vuelva a entrar, Solo cambia debido a la interrupción
			rxData = '\0';
		}
//		if(adcIsComplete == true){
//
//			for(uint16_t i = 0; i < 256; i++){
//
//				sprintf(bufferData, "%d | %u; %u; \n", i+1, dataCh1[i], dataCh2[i]);
//				writeMsgTX(&handlerUsart1, bufferData);
//			}
//				//Bajamos la bandera del ADC
//				adcIsComplete = 0;
//			}

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
	sscanf(ptrBufferReception, "%s %s %u", cmd, userMsg, &firstParameter);

	//El primer comando imprime una lista  con los otros comandos que tiene el equipo
	if(strcmp(cmd, "help") == 0){

		writeMsg(&handlerUsart1, "Help Menu CMDs: \n");
		writeMsg(&handlerUsart1, "1) help   --   Print this menu \n");
		writeMsg(&handlerUsart1, "2) usermsg msg   --   msg is a string coming from outside \n");
		writeMsg(&handlerUsart1, "3) signal select_signal   --   Select HSI, LSE or PLL \n");
		writeMsg(&handlerUsart1, "4) prescaler # select_prescaler   ---  Select prescaler value \n");
		writeMsg(&handlerUsart1, "5) sampFreq # #   --   Select speed sampling \n");
		writeMsg(&handlerUsart1, "6) dataAdc   --   Show the ADC data \n");
	}

	//El comando usermsg sirve para entender como funciona la recepcion de strings enviados desde consola
	else if(strcmp(cmd, "usermsg") == 0){
		writeMsg(&handlerUsart1, "COMD: usermsg \n");
		writeMsg(&handlerUsart1, userMsg);
		writeMsg(&handlerUsart1, "\n");
	}
	else if(strcmp(cmd, "signal") == 0){
		if(strcmp(userMsg, "HSI") == 0){

			writeMsg(&handlerUsart1, "HSI selected \n");

			//Seleccionar el HSI
			RCC->CFGR &= ~RCC_CFGR_MCO1_0;
			RCC->CFGR &= ~RCC_CFGR_MCO1_1;
		}
		else if(strcmp(userMsg, "LSE") == 0){

			writeMsg(&handlerUsart1, "LSE selected \n");

			//Se habilita la señal para el APB1
			RCC->APB1ENR &= ~RCC_APB1ENR_PWREN;
			RCC->APB1ENR |= RCC_APB1ENR_PWREN;

			//Se quita la protección de los bits para modificar el LSE
			PWR->CR &= ~PWR_CR_DBP;
			PWR->CR |= PWR_CR_DBP;

			//Encendemos el LSE
			RCC->BDCR &= ~RCC_BDCR_LSEON;
			RCC->BDCR |= RCC_BDCR_LSEON;

			//Seleccionamos el LSE
			RCC->CFGR &= ~RCC_CFGR_MCO1;
			RCC->CFGR |= RCC_CFGR_MCO1_0;
		}
		else if(strcmp(userMsg, "PLL") == 0){

			writeMsg(&handlerUsart1, "PLL selected \n");

			//Seleccionamos el PLL
			RCC->CFGR &= ~RCC_CFGR_MCO1;
			RCC->CFGR |= RCC_CFGR_MCO1;
		}
		else{

			//Escribir mensaje de error
			writeMsg(&handlerUsart1, "Error!: Invalid source selected \n");
		}
	}
	else if(strcmp(cmd, "prescaler") == 0){

		//Se carga el valor del prescaler en el registro correspondiente
		if(strcmp(userMsg, "1") == 0){

			writeMsg(&handlerUsart1, "Prescaler value is: 1");

			//No se divide
			RCC->CFGR &= ~RCC_CFGR_MCO1PRE;
		}
		else if(strcmp(userMsg, "2") == 0){

			writeMsg(&handlerUsart1, "Prescaler value is: 2");

			//Prescaler divide por 2
			RCC->CFGR &= ~RCC_CFGR_MCO1PRE;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_2;;
		}
		else if(strcmp(userMsg, "3") == 0){

			writeMsg(&handlerUsart1, "Prescaler value is: 3");

			//Prescaler divide por 3
			RCC->CFGR &= ~RCC_CFGR_MCO1PRE;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_2;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_0;
		}
		else if(strcmp(userMsg, "4") == 0){

			writeMsg(&handlerUsart1, "Prescaler value is: 4");

			//Prescaler divide por 4
			RCC->CFGR &= ~RCC_CFGR_MCO1PRE;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_1;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_2;
		}
		else if(strcmp(userMsg, "5") == 0){

			writeMsg(&handlerUsart1, "Prescaler value is: 5");

			//Prescaler divide por 5
			RCC->CFGR &= ~RCC_CFGR_MCO1PRE;
			RCC->CFGR |= RCC_CFGR_MCO1PRE;
		}
		else{
			//Se envía un mensaje de error
			writeMsg(&handlerUsart1, "Error!: Invalid prescaler value");
		}
	}
	else if(strcmp(cmd, "sampFreq") == 0){

		//Se actualizan los valores que se desean modificar con el comando
		speed = firstParameter;
		duttyPwm = (speed/2);

		//Se llaman las funciones para actualizar el dutty y la frecuencia
		updateFrequency(&handlerPWM, speed);
		updateDuttyCycle(&handlerPWM, duttyPwm);
	}
	else if(strcmp(cmd, "dataAdc") == 0){

		startPwmSignal(&handlerPWM);

		sprintf(bufferData, "# ; CH1 ; CH2 \n");
		writeMsgTX(&handlerUsart1, bufferData);

		if(adcIsComplete == true){

			for(uint16_t i = 0; i < 256; i++){

				sprintf(bufferData, "%d | %u; %u; \n", i+1, dataCh1[i], dataCh2[i]);
				writeMsgTX(&handlerUsart1, bufferData);
			}
				//Bajamos la bandera del ADC
				adcIsComplete = 0;
			}

		rxData = '\0';
	}
	else{
		//Se imprime el mensaje "Wrong CMD" si la escritura no corresponde a los CMD implementados
		writeMsg(&handlerUsart1, "Wrong CMD");
	}
}

void initSystem(void){

	//Configuración del Blinky
	handlerBlinky.pGPIOx = GPIOA;
	handlerBlinky.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
	handlerBlinky.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerBlinky.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerBlinky.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEEDR_FAST;
	handlerBlinky.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerBlinky);

	//Configuración del TIM2 (Blinky)
	handlerTimerBlinky.ptrTIMx 								= TIM2;
	handlerTimerBlinky.TIMx_Config.TIMx_mode 				= BTIMER_MODE_UP;
	handlerTimerBlinky.TIMx_Config.TIMx_speed 				= 1000;
	handlerTimerBlinky.TIMx_Config.TIMx_period 				= 25000;
	handlerTimerBlinky.TIMx_Config.TIMx_interruptEnable 	= 1;
	BasicTimer_Config(&handlerTimerBlinky);

	//Configuración del TIM5 (Muestreo acelerómetro)
	handlerTimerSamp.ptrTIMx 								= TIM5;
	handlerTimerSamp.TIMx_Config.TIMx_mode 					= BTIMER_MODE_UP;
	handlerTimerSamp.TIMx_Config.TIMx_speed 				= BTIMER_SPEED_100us;
	handlerTimerSamp.TIMx_Config.TIMx_period 				= 10;
	handlerTimerSamp.TIMx_Config.TIMx_interruptEnable 		= 1;
	BasicTimer_Config(&handlerTimerSamp);

	//Configuración del MCO1
	handlerMCO1.pGPIOx									= GPIOA;
	handlerMCO1.GPIO_PinConfig.GPIO_PinNumber			= PIN_8;
	handlerMCO1.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerMCO1.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerMCO1.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerMCO1.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEEDR_FAST;
	handlerMCO1.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;
	GPIO_Config(&handlerMCO1);

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
	handlerPWM.config.duttyCicle 	= 500;
	handlerPWM.config.periodo 		= 1000;			//2 ms -- 66
	handlerPWM.config.prescaler 	= 25000;			//Escala de 1us -- Escala de tiempo
	pwm_Config(&handlerPWM);

	//startPwmSignal(&handlerPWM);

	//Se carga la configuración de las interrupciones
	ADC_Channel_Interrupt(&channel);
}

void usart1Rx_Callback(void){
	//Leemos el valor del registro DR, donde se almacena el dato que llega.
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

void adcComplete_Callback(void){

	//Se guarda el primer dato del primer canal en el arreglo 1
	if(adcCounter == 0){
		dataCh1[dataCounter] = getADC();
	}
	//Se guarda el primer dato del segundo canal en el arreglo 2
	else{
		dataCh2[dataCounter] = getADC();
		dataCounter++;
	}
	adcCounter++;

	//Cuando el contador llega a los 256 datos, detiene el proceso de ambos canales
	if(dataCounter > 256){
		dataCounter = 0;
		stopPwmSignal(&handlerPWM);
		adcIsComplete = true;
	}
	if(adcCounter == 2){
		adcCounter = 0;
	}
}
