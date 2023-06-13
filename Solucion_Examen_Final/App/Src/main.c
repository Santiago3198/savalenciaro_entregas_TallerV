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
ADC_Config_t channel 						= {0};		//Handler de la configuración de los canales ADC

//Configuración PWM
PWM_Handler_t handlerPWM 					= {0};		//Handler de la configuracióń del PWM para el ADC

//Configuraicón RTC
RTC_Config_t handlerRTC						= {0};		//Handler de la configuración del RTC

//Definición de variables
uint8_t rxData = 0;
char bufferDataRTC[64] = {0};

//Variables relacionadas con la comunicación I2C del Accel
uint8_t i2cBuffer = 0;
uint16_t indx = 0;
uint8_t saveDataAccFlag = 0;
char bufferData[64] = {0};

//Variables relacionadas con el ADC
uint8_t adcIsComplete	= 0;
uint8_t adcCounter 		= 0;
uint16_t dataADC[2] 	= {0};
char bufferDataAdc[128] = {0};

unsigned int speed = 66;
uint16_t duttyPwm = 33;
float dataCh1[256] = {0};
float dataCh2[256] = {0};
uint16_t dataCounter = 0;
uint8_t flag = 0;

//Variables relacionadas con el uso de los comandos en terminal
bool stringComplete;
char cmd[256] = {0};
char userMsg[256] = {0};
char bufferReception[256] = {0};
uint16_t counterReception = 0;
unsigned int firstParameter = 0;
unsigned int secondParameter = 0;
unsigned int thirdParameter = 0;

//Definición de arreglos para guardar los muestreos del acelerómetro
float32_t arrayZ[1024] = {0};
uint8_t flagAcc = 0;
float converFact = (1.9/32767.0)*(9.8);			//Factor de conversión para los datos del accel

//Variables para la FFT
#define ACCEL_DATA_SIZE		1024
float32_t accelSignal[ACCEL_DATA_SIZE];
float32_t transformedSignal[ACCEL_DATA_SIZE];
float32_t readyFFT[ACCEL_DATA_SIZE];
float32_t maxVal = 0;
float32_t frequencyFFT = 0;
uint16_t indxFFT = 0;
char bufferFFT[128] = {0};

uint32_t ifftFlag = 0;
arm_rfft_fast_instance_f32 config_Rfft_fast_f32;
arm_cfft_radix4_instance_f32 configRadix4_f32;
arm_status statusInitFFT = ARM_MATH_ARGUMENT_ERROR;

//Definición número de canales para usar el ADC
#define CHANNELS   2

//Definiciones para comunicación I2C (ACCEL)
#define ACCEL_ADDRESS 	0b1101001;		//ID Device
#define ACCEL_ZOUT_H	63				//0x3F
#define ACCEL_ZOUT_L	64				//0x40

#define PWR_MGMT_1		107
#define WHO_AM_I		117

//Definición de funciones
void initSystem(void);
void configPLL(void);
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

	startPwmSignal(&handlerPWM);

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
		if(flagAcc == 1){

			writeMsg(&handlerUsart1, "\nDatos guardados correctamente \n");
			flagAcc = 0;
		}
	}
	return 0;
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
	sscanf(ptrBufferReception, "%s %s %u %u %u", cmd, userMsg, &firstParameter, &secondParameter, &thirdParameter);

	//El primer comando imprime una lista  con los otros comandos que tiene el equipo
	if(strcmp(cmd, "help") == 0){

		writeMsg(&handlerUsart1, "\n Help Menu CMDs: \n");
		writeMsg(&handlerUsart1, "\n Por favor ingrese un comando seguido de los parámetros a modificar separados por espacios \n");
		writeMsg(&handlerUsart1, "\n 1) help   -->   Menú de ayuda \n");
		writeMsg(&handlerUsart1, "\n 2) usermsg - Ingrese un mensaje  -->  Ingrese el mensaje que desea enviar \n");
		writeMsg(&handlerUsart1, "\n 3) signal - Seleccione la señal  -->  Seleccione HSI, LSE o PLL \n");
		writeMsg(&handlerUsart1, "   Señor usuario, recuerde que el PLL está a 100 MHz por lo que se configura un prescaler de 4 (default) para su visualización \n");
		writeMsg(&handlerUsart1, "   El prescaler puede modificarlo posteriormente a conveniencia \n");
		writeMsg(&handlerUsart1, "\n 4) prescaler - Valor del prescaler  -->  Ingrese el valor del prescaler deseado \n");
		writeMsg(&handlerUsart1, "   El número ingresado representa el valor por el cual será dividida la señal \n");
		writeMsg(&handlerUsart1, "		1 --> div1 \n");
		writeMsg(&handlerUsart1, "		2 --> div2 \n");
		writeMsg(&handlerUsart1, "		3 --> div3 \n");
		writeMsg(&handlerUsart1, "		4 --> div4 \n");
		writeMsg(&handlerUsart1, "		5 --> div5 \n");
		writeMsg(&handlerUsart1, "\n 5) frequency - Ingrese un caracter - Ingrese el valor de la velocidad \n");
		writeMsg(&handlerUsart1, "    Ingrese un valor entre 1 y 200 para modificar la velocidad del muestreo \n" );
		writeMsg(&handlerUsart1, "\n 6) dataAdc  -->  Show the ADC data \n");
		writeMsg(&handlerUsart1, "    Con este comando muestra dos arreglos de 256 datos c/u tomados con el ADC \n");
		writeMsg(&handlerUsart1, "\n 7) device  -->  WHO I AM? \n");
		writeMsg(&handlerUsart1, "\n 8) stateAcc  -->  \n");
		writeMsg(&handlerUsart1, "    Señor usuario, para usar los comandos del Accel primero debe resetear su configuración \n");
		writeMsg(&handlerUsart1, "\n 9) resetAcc  -->  Resetea la configuración del Accel \n");
		writeMsg(&handlerUsart1, "\n 10) capture  -->  Captura los datos tomados en el Accel y los guarda en un arreglo para cada eje \n ");
		writeMsg(&handlerUsart1, "\n 11) sampleZ  -->  Comando que imprime una muestra de 50 datos tomados por el Accel \n");
		writeMsg(&handlerUsart1, "\n 12) dataFFT  -->  Muestra la transformada rápida de Fourier de los datos tomados del Accel \n ");
		writeMsg(&handlerUsart1, "\n 13) hour - Formato de hora - Horas - Minutos  -->  Comando para configurar la hora \n");
		writeMsg(&handlerUsart1, "\n 14) date - Ingrese un caracter - Día - Mes - Año  -->  Comando para configurar la fecha \n");
		writeMsg(&handlerUsart1, "\n 15) showHour  -->  Con este comando podrá ver la hora ingresada con el comando número 13 \n");
		writeMsg(&handlerUsart1, "\n 16) showDate  -->  Con este comando podrá ver la fecha ingresada con el comando número 14 \n");
	}

	//El comando usermsg sirve para entender como funciona la recepcion de strings enviados desde consola
	else if(strcmp(cmd, "usermsg") == 0){

		//Si el usuario desea enviar un mensaje
		writeMsg(&handlerUsart1, "\nCOMD: usermsg \n");
		writeMsg(&handlerUsart1, userMsg);
		writeMsg(&handlerUsart1, "\n");
	}

	//Conmando para cambiar la señal que sale por el MCO1
	else if(strcmp(cmd, "signal") == 0){

		//Si se desea mostrar el reloj HSI
		if(strcmp(userMsg, "HSI") == 0){

			writeMsg(&handlerUsart1, "\nHSI selected \n");

			//Seleccionar el HSI
			RCC->CFGR &= ~RCC_CFGR_MCO1_0;
			RCC->CFGR &= ~RCC_CFGR_MCO1_1;
		}

		//Si se desea mostrar el reloj LSE
		else if(strcmp(userMsg, "LSE") == 0){

			writeMsg(&handlerUsart1, "\nLSE selected \n");

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

		//Si se desea mostrar el reloj PLL
		else if(strcmp(userMsg, "PLL") == 0){

			writeMsg(&handlerUsart1, "\nPLL selected \n");

			//Prescaler divide por 4
			RCC->CFGR &= ~RCC_CFGR_MCO1PRE;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_1;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_2;

			//Seleccionamos el PLL
			RCC->CFGR &= ~RCC_CFGR_MCO1;
			RCC->CFGR |= RCC_CFGR_MCO1;
		}
		else{

			//Escribir mensaje de error
			writeMsg(&handlerUsart1, "\nError!: Invalid source selected \n");
		}
	}
	else if(strcmp(cmd, "prescaler") == 0){

		//Se carga el valor del prescaler en el registro correspondiente
		if(strcmp(userMsg, "1") == 0){

			writeMsg(&handlerUsart1, "\nPrescaler value is: 1 \n");

			//No se divide
			RCC->CFGR &= ~RCC_CFGR_MCO1PRE;
		}
		else if(strcmp(userMsg, "2") == 0){

			writeMsg(&handlerUsart1, "\nPrescaler value is: 2 \n");

			//Prescaler divide por 2
			RCC->CFGR &= ~RCC_CFGR_MCO1PRE;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_2;
		}
		else if(strcmp(userMsg, "3") == 0){

			writeMsg(&handlerUsart1, "\nPrescaler value is: 3 \n");

			//Prescaler divide por 3
			RCC->CFGR &= ~RCC_CFGR_MCO1PRE;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_2;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_0;
		}
		else if(strcmp(userMsg, "4") == 0){

			writeMsg(&handlerUsart1, "\nPrescaler value is: 4 \n");

			//Prescaler divide por 4
			RCC->CFGR &= ~RCC_CFGR_MCO1PRE;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_1;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_2;
		}
		else if(strcmp(userMsg, "5") == 0){

			writeMsg(&handlerUsart1, "\nPrescaler value is: 5 \n");

			//Prescaler divide por 5
			RCC->CFGR &= ~RCC_CFGR_MCO1PRE;
			RCC->CFGR |= RCC_CFGR_MCO1PRE;
		}
		else{
			//Se envía un mensaje de error
			writeMsg(&handlerUsart1, "\nError!: Invalid prescaler value \n");
		}
	}
	else if(strcmp(cmd, "frequency") == 0){

		if((firstParameter > 1) && (firstParameter < 200)){

			//Se cargan los valores iniciales de freq y dutty
			setFrequency(&handlerPWM);
			setDuttyCycle(&handlerPWM);

			//Se actualizan los valores que se desean modificar con el comando
			speed = firstParameter;
			duttyPwm = (speed/2);

			//Se llaman las funciones para actualizar el dutty y la frecuencia
			updateFrequency(&handlerPWM, speed);
			updateDuttyCycle(&handlerPWM, duttyPwm);

			writeMsg(&handlerUsart1, "\nFrecuencia de muestreo configurada correctamente \n");
		}
		else{

			writeMsg(&handlerUsart1, "\nError!: Invalid value to sample \n");
		}
	}
	else if(strcmp(cmd, "dataAdc") == 0){

		startPwmSignal(&handlerPWM);

		writeMsg(&handlerUsart1, "\nTomando datos \n");
		flag = 1;

		while(!flag){
		__NOP();
		}
			for(uint16_t i = 0; i < 256; i++){

				sprintf(bufferDataAdc, "CH1 = %.2f ; CH2 = %.2f; \n", dataCh1[i]*3.3f/4095.f, dataCh2[i]*3.3f/4095.f);
				writeMsg(&handlerUsart1, bufferDataAdc);
			}
		rxData = '\0';
	}
	//Guarda los datos del accel en un arreglo para cada eje
	else if(strcmp(cmd, "capture") == 0){

		//Mensaje para el usuario
		writeMsg(&handlerUsart1, "\nGuardando datos... \n");

		//Levantamos la bandera para indicar que se deben almacenar los datos
		saveDataAccFlag = 1;
	}
	//Presenta los datos de las frecuencias de los datos del Accel
	else if(strcmp(cmd, "dataFFT") == 0){

		//Inicialización de las funciones
		statusInitFFT = arm_rfft_fast_init_f32(&config_Rfft_fast_f32, 1024);

		if(statusInitFFT == ARM_MATH_SUCCESS){

			sprintf(bufferFFT, "\nInicialización completada! \n");
			writeMsg(&handlerUsart1, bufferFFT);
		}

		sprintf(bufferFFT, "\nGuardando datos... \n");
		writeMsg(&handlerUsart1, bufferFFT);

		saveAccZ();

		sprintf(bufferFFT, "\nIniciando FFT... \n");
		writeMsg(&handlerUsart1, bufferFFT);

		//Si la inicizalización está completa se procede a poner el arreglo dentro de la función FFT
		if(statusInitFFT == ARM_MATH_SUCCESS){

			arm_rfft_fast_f32(&config_Rfft_fast_f32, arrayZ, transformedSignal, ifftFlag);
			arm_abs_f32(transformedSignal, accelSignal, 1024);

			//Se transforma cada uno de los datos del arreglo
			for(int i = 0; i < 1024; i++){
				if(i%2){
					readyFFT[i] = accelSignal[i];
				}
			}
		}

		//Cuando todos los datos están listos, se reinicia el contador para volver a empezar
		maxVal = readyFFT[0];
		for(int j = 0; j < 1024; j++){
			if(maxVal < readyFFT[j]){
				maxVal = readyFFT[j];
				indxFFT = j;
			}
		}
		sprintf(bufferFFT, "\nÍndice de la frecuencia fundamental: %u \n", indxFFT);
		writeMsg(&handlerUsart1, bufferFFT);

		frequencyFFT = (indxFFT*200/(1024));

		sprintf(bufferFFT, "Frecuencia = %#.4f Hz \n", frequencyFFT);
		writeMsg(&handlerUsart1, bufferFFT);
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
	else if(strcmp(cmd, "sampleZ") == 0){

		//Se realiza una pequeña toma de datos para verificar
		for(uint16_t j = 0; j < 10; j++){

			sprintf(bufferData, "%d | %.2f \n", j+1, (float)arrayZ[j]*converFact);
			writeMsgTX(&handlerUsart1, bufferData);
		}
	}
	else if(strcmp(cmd, "hour") == 0){

		//Se cargan los parámetros ingresados por el usuario
		if(handlerRTC.format == MODE_24H){

			if((firstParameter >= 0) && (firstParameter < 24)){
				handlerRTC.hours = firstParameter;
			}
			else{
				writeMsg(&handlerUsart1, "\nError!: Invalid input \n");
			}
			if((secondParameter >= 0) && (secondParameter < 60)){
				handlerRTC.minuts = secondParameter;
			}
			else{
				writeMsg(&handlerUsart1, "\nError!: Invalid input \n");
			}
		}
		else if(handlerRTC.format == MODE_12H){

			if((firstParameter > 0) && (firstParameter <= 12)){
				handlerRTC.hours = firstParameter;
			}
			else{
				writeMsg(&handlerUsart1, "\nError!: Invalid input \n");
			}
			if((secondParameter >= 0) && (secondParameter < 60)){
				handlerRTC.minuts = secondParameter;
			}
			else{
				//Mensaje de error
				writeMsg(&handlerUsart1, "\nError!: Invalid input \n");
			}
		}

		//Configuración del formato de las horas
		if(strcmp(userMsg, "24H") == 0){
			handlerRTC.format = MODE_24H;
		}
		else if(strcmp(userMsg, "AM") == 0){
			handlerRTC.format = MODE_12H;
			handlerRTC.AM_PM = MODE_AM;
		}
		else if(strcmp(userMsg, "PM") == 0){
			handlerRTC.format = MODE_12H;
			handlerRTC.AM_PM = MODE_PM;
		}
		else{
			//Mensaje de error
			writeMsg(&handlerUsart1, "\nError!: Invalid input \n");
		}
		RTC_Config(&handlerRTC);
	}
	else if(strcmp(cmd, "date") == 0){

		if((firstParameter >= 1) && (firstParameter <=30)){
			handlerRTC.days = firstParameter;
		}
		else{
			//Mensaje de error
			writeMsg(&handlerUsart1, "\nError!: Invalid input \n");
		}
		if((secondParameter >= 1) && (secondParameter <=12)){
			handlerRTC.month = secondParameter;
		}
		else{
			//Mensaje de error
			writeMsg(&handlerUsart1, "\nError!: Invalid input \n");
		}

		handlerRTC.year = thirdParameter;
		//Se carga la configuración
		RTC_Config(&handlerRTC);
	}
	else if(strcmp(cmd, "showHour") == 0){

		uint8_t secUni = 0;
		uint8_t secDec = 0;
		uint8_t minUni = 0;
		uint8_t minDec = 0;
		uint8_t horaUni = 0;
		uint8_t horaDec = 0;
		uint8_t AM_PM = 0;

		secUni = read_time()[0];
		secDec = read_time()[1];
		minUni = read_time()[2];
		minDec = read_time()[3];
		horaUni = read_time()[4];
		horaDec = read_time()[5];
		AM_PM = read_time()[6];

		if(AM_PM == 0){
			sprintf(bufferDataRTC, "\nHora: %u%u:%u%u:%u%u AM \n", horaDec, horaUni, minDec, minUni, secDec, secUni);
			writeMsg(&handlerUsart1, bufferDataRTC);
		}
		else if(AM_PM == 1){
			sprintf(bufferDataRTC, "\nHora: %u%u:%u%u:%u%u PM \n", horaDec, horaUni, minDec, minUni, secDec, secUni);
			writeMsg(&handlerUsart1, bufferDataRTC);
		}
		else{
			sprintf(bufferDataRTC, "\nHora: %u%u:%u%u:%u%u \n", horaDec, horaUni, minDec, minUni, secDec, secUni);
			writeMsg(&handlerUsart1, bufferDataRTC);
		}
	}
	else if(strcmp(cmd, "showDate") == 0){

		uint8_t daysUni = 0;
		uint8_t daysDec = 0;
		uint8_t _month = 0;
		uint8_t yearUni = 0;
		uint8_t yearDec = 0;

		daysUni = read_date()[0];
		daysDec = read_date()[1];
		_month = read_date()[2];
		yearUni = read_date()[3];
		yearDec = read_date()[4];

		unsigned int yearValue = 2000 + (yearDec*10) + yearUni;

		sprintf(bufferDataRTC, "\nFecha: %u%u-%u-%u \n", daysDec, daysUni, _month, yearValue);
		writeMsg(&handlerUsart1, bufferDataRTC);
	}
	else{
		//Se imprime el mensaje "Wrong CMD" si la escritura no corresponde a los CMD implementados
		writeMsg(&handlerUsart1, "\nWrong CMD \n");
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
	handlerTimerBlinky.TIMx_Config.TIMx_speed 				= 1000;
	handlerTimerBlinky.TIMx_Config.TIMx_period 				= 25000;
	handlerTimerBlinky.TIMx_Config.TIMx_interruptEnable 	= 1;
	BasicTimer_Config(&handlerTimerBlinky);

	//Configuración del TIM3 (Muestreo acelerómetro)
	handlerTimerSamp.ptrTIMx 								= TIM3;
	handlerTimerSamp.TIMx_Config.TIMx_mode 					= BTIMER_MODE_UP;
	handlerTimerSamp.TIMx_Config.TIMx_speed 				= BTIMER_SPEED_PLL_100_MHz_100us;
	handlerTimerSamp.TIMx_Config.TIMx_period 				= 50;
	handlerTimerSamp.TIMx_Config.TIMx_interruptEnable 		= 1;
	BasicTimer_Config(&handlerTimerSamp);

	//Configuración del MCO1
	handlerMCO1.pGPIOx										= GPIOA;
	handlerMCO1.GPIO_PinConfig.GPIO_PinNumber				= PIN_8;
	handlerMCO1.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_ALTFN;
	handlerMCO1.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
	handlerMCO1.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	handlerMCO1.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEEDR_FAST;
	handlerMCO1.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;
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
	handlerAccelerometer.PLL_ON								= PLL_ENABLE;
	handlerAccelerometer.ptrI2Cx							= I2C1;
	handlerAccelerometer.slaveAddress						= ACCEL_ADDRESS;
	handlerAccelerometer.modeI2C							= I2C_MODE_FM;
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
	channel.numberOfChannels								= CHANNELS;
	channel.multiChannel[0] 								= ADC_CHANNEL_1;
	channel.multiChannel[1] 								= ADC_CHANNEL_4;
	channel.multiSampling[0] 								= ADC_SAMPLING_PERIOD_84_CYCLES;
	channel.multiSampling[1] 								= ADC_SAMPLING_PERIOD_84_CYCLES;
	channel.dataAlignment 									= ADC_ALIGNMENT_RIGHT;
	channel.resolution 										= ADC_RESOLUTION_12_BIT;
	channel.eventType										= EXTERNAL_EVENT_ENABLE;
	channel.AdcEvent										= 11;
	ADC_ConfigMultichannel(&channel, CHANNELS);

	//Configuración del pin para el PWM
	handlerPwmPin.pGPIOx 									= GPIOA;
	handlerPwmPin.GPIO_PinConfig.GPIO_PinNumber 			= PIN_0;
	handlerPwmPin.GPIO_PinConfig.GPIO_PinMode   			= GPIO_MODE_ALTFN;
	handlerPwmPin.GPIO_PinConfig.GPIO_PinOPType 			= GPIO_OTYPE_PUSHPULL;
	handlerPwmPin.GPIO_PinConfig.GPIO_PinPuPdControl 		= GPIO_PUPDR_NOTHING;
	handlerPwmPin.GPIO_PinConfig.GPIO_PinSpeed  			= GPIO_OSPEEDR_FAST;
	handlerPwmPin.GPIO_PinConfig.GPIO_PinAltFunMode 		= AF2;
	GPIO_Config(&handlerPwmPin);

	//Configuración de la señal PWM
	handlerPWM.ptrTIMx 										= TIM5;
	handlerPWM.config.channel 								= PWM_CHANNEL_1;
	handlerPWM.config.duttyCicle 							= duttyPwm;
	handlerPWM.config.periodo 								= speed;			//2 ms -- 66
	handlerPWM.config.prescaler 							= 100;				//Escala de 1us -- Escala de tiempo
	pwm_Config(&handlerPWM);

	startPwmSignal(&handlerPWM);

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

void BasicTimer3_Callback(void){

	if (saveDataAccFlag == 0){
		return;
	}

	saveAccZ();

	indx++;

	if(indx >= 1024){
		indx = 0;
		saveDataAccFlag = 0;
		flagAcc = 1;
	}
}

void adcComplete_Callback(void){

	if(flag){
		//Se guarda el primer dato del primer canal en el arreglo 1
		if(adcCounter == 0){
			dataCh1[dataCounter] = (float) getADC();
		}
		//Se guarda el primer dato del segundo canal en el arreglo 2
		else{
			dataCh2[dataCounter] = (float) getADC();
			dataCounter++;
		}
		adcCounter++;

		//Cuando el contador llega a los 256 datos, detiene el proceso de ambos canales
		if(dataCounter > 256){
			dataCounter = 0;
			flag=0;
		}
		if(adcCounter == 2){
			adcCounter = 0;
		}
	}
}