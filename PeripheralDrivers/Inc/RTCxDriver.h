/*
 * RTCDriver.h
 *
 *  Created on: 8/6/2023
 *      Author: Sentry
 */

#ifndef RTCDRIVER_H_
#define RTCDRIVER_H_

#include "stm32f4xx.h"

#define CLOCK_LSE 	0b01
#define CLOCK_LSI 	0b10
#define CLOCK_HSE 	0b11
#define CLOCK_NONE 	0b00

#define MODE_24H	0
#define MODE_12H	1

#define MODE_AM  	0
#define MODE_PM 	1

//Definicion del RTC
typedef struct
{
	uint8_t		format;  //format;	 	//Cual va a ser el formato en el que se va a presentar la hora formato de 24 horas o AM/PM
	uint8_t 	AM_PM;   //AM_PM;			//Si se eligio el formato de 12 horas, en cual se esta en AM o PM
	uint8_t		hours;   //RTC_HOURS; 				//Las horas que se van a ingresar
	uint8_t		minuts;  //RTC_MINUTS;				//Los minutos que se desean ingresar
	uint8_t		seconds; //RTC_SEC;				//Los segundos que se deasean ingresar
	uint8_t 	days;    //days;
	uint8_t 	month;   //RTC_MONTH;
	uint8_t 	year;    //year;
}RTC_Config_t;

//Cargar la configuración del RTC
void RTC_Config(RTC_Config_t *ptrHandlerRTC);

//Funcion para leer la hora
uint8_t* read_time(void);

//Funcion para leer la fecha
uint8_t* read_date(void);

#endif
