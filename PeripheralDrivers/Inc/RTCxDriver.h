/*
 * RTCxDriver.h
 *
 *  Created on: 7/6/23
 *  Author: Sentry
 */

#ifndef RTCXDRIVER_H_
#define RTCXDRIVER_H_

#include <stm32f4xx.h>

//#define TIME_NOTATION_AM		0
//#define TIME_NOTATION_PM		1

#define  TIME_FORMAT_24_HOUR	0
#define  TIME_FORMAT_AM_PM		1

#define MONDAY					1
#define TUESDAY 				2
#define WEDNESDAY				3
#define THURSDAY				4
#define FRIDAY					5
#define SATURDAY				6
#define SUNDAY					7

#define JAN						1
#define FEB						2
#define MAR						3
#define APR						4
#define MAY						5
#define JUN						6
#define JUL						7
#define AUG						8
#define SEP						9
#define OCT						10
#define NOV						11
#define DEC						12

//Estructura de configuración para el RTC
typedef struct{
	uint16_t RTC_Hours; //Configuración de las horas
	uint16_t RTC_Minutes; //Configuración de los minutos
	uint16_t RTC_Seconds; //Configuración de los segundos
	uint16_t RTC_Year; //Configuración del año
	uint16_t RTC_WeekDay; //Configuración del día de la semana
	uint16_t RTC_TimeFormat; // Configuración del formato
	uint16_t RTC_TimeNotation; //Configuración de la notación
	uint16_t RTC_Month; //Configuración del mes
	uint16_t RTC_ValueDay; //Configuración valor día

}RTC_Config_t;

typedef struct{
	RTC_TypeDef 	*ptrRTC; //Dirección
	RTC_Config_t	RTC_Config; //Configuración

}RTC_Handler_t;

//Definición de funciones
void RTC_Config(RTC_Handler_t *ptrRtcHandler);		//Configuración del RTC
void *read_date(void); 								//Funcion para traer la fecha
void Mes(uint16_t Mes , char *Month_);				//Funcion config del mes
void semana (uint16_t dia, char *Weekday_); 		//Funcion config del día de la semana


#endif /* RTCXDRIVER_H_ */
