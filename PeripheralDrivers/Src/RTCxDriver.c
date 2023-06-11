/*
 * RTCxDriver.c
 *
 *  Created on: 7/6/23
 *  Author: Sentry
 */


#include "RTCxDriver.h"
#include <stdio.h>

void RTC_Config(RTC_Handler_t *ptrRtcHandler) {

	// Habilitar el PWR Clock
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;

	//Accediendo al RTC
	PWR->CR  |= PWR_CR_DBP;

	//Habilitar la fuente del reloj
	RCC->BDCR |= RCC_BDCR_RTCEN;

	// Set the LSE clock on
	RCC->BDCR |= RCC_BDCR_LSEON;

	// Select the LSE crystal as clock
	RCC->BDCR |= RCC_BDCR_RTCSEL_0;

	//Tiempo de espera para que el reloj se estabilice
	while(!(RCC->BDCR & RCC_BDCR_LSERDY)){
		__NOP() ;
	}

	//Bits de protección contra escritura (se deshabilitan)
	RTC->WPR = (0xCA);
	RTC->WPR = (0x53);

	//Modo inicialización
	RTC->ISR |= RTC_ISR_INIT;

	while (!(RTC->ISR & RTC_ISR_INITF)){
		__NOP() ;
	}

	/* La frecuencia del LSE es de 32 kHz, por lo que el prescaler asíncrono
	 * debe configurarse en 128 y el prescaler síncrono en 256 para que la
	 * frecuencia interna del reloj sea de aproximadamente 1Hz
	 */

	RTC->PRER |= RTC_PRER_PREDIV_A;
	RTC->PRER |= 0xFF << 0 ;

	//Se carga la configuración inicial para el RTC
    RTC->CR |= RTC_CR_BYPSHAD;

    //Se limpian los registros
	RTC->TR = 0;
	RTC->DR = 0;

	//Config del formato
	RTC->CR |= ((ptrRtcHandler->RTC_Config.RTC_TimeFormat) << RTC_CR_FMT_Pos);

	//Config del año
	RTC->DR |= ((ptrRtcHandler->RTC_Config.RTC_Year) / 10) << RTC_DR_YT_Pos;
	RTC->DR |= ((ptrRtcHandler->RTC_Config.RTC_Year) % 10) << RTC_DR_YU_Pos;

	//Config del mes
	RTC->DR |= ((ptrRtcHandler->RTC_Config.RTC_Month) / 10) << RTC_DR_MT_Pos;
	RTC->DR |= ((ptrRtcHandler->RTC_Config.RTC_Month) % 10) << RTC_DR_MU_Pos;

	//Config de la semana
	RTC->DR |= ((ptrRtcHandler->RTC_Config.RTC_WeekDay)) << RTC_DR_WDU_Pos;

	//Config del día
	RTC->DR |= ((ptrRtcHandler->RTC_Config.RTC_ValueDay) / 10) << RTC_DR_DT_Pos;
	RTC->DR |= ((ptrRtcHandler->RTC_Config.RTC_ValueDay) % 10) << RTC_DR_DU_Pos;

	//Config de la hora
	RTC->TR |= ((ptrRtcHandler->RTC_Config.RTC_Hours) / 10) << RTC_TR_HT_Pos;
	RTC->TR |= ((ptrRtcHandler->RTC_Config.RTC_Hours) % 10) << RTC_TR_HU_Pos;

	RTC->TR |= ((ptrRtcHandler->RTC_Config.RTC_Minutes) / 10) << RTC_TR_MNT_Pos;
	RTC->TR |= ((ptrRtcHandler->RTC_Config.RTC_Minutes) % 10) << RTC_TR_MNU_Pos;

	RTC->TR |= ((ptrRtcHandler->RTC_Config.RTC_Seconds) / 10) << RTC_TR_ST_Pos;
	RTC->TR |= ((ptrRtcHandler->RTC_Config.RTC_Seconds) % 10) << RTC_TR_SU_Pos;

	//Config del formato de la hora
	RTC->TR |= ((ptrRtcHandler->RTC_Config.RTC_TimeNotation) << RTC_TR_PM_Pos);

	/* Exiting initialization mode by clearing init bit */

		RCC->BDCR |= RCC_BDCR_RTCEN;
		RTC->ISR &= ~RTC_ISR_INIT;
		PWR->CR &= ~ PWR_CR_DBP;

	    RTC->CR &= ~RTC_CR_BYPSHAD;
		RTC->WPR = (0xFF); // Key Lock write protection
}


//Definición de variables

//Almacena los valores de la función read_date
uint16_t calendario [7] = {0};

//Conversion de formato BCD
uint16_t RTC_BcdToByte(uint16_t BCD_Value){

    uint16_t Decimal_Value = ((BCD_Value/16*10) + (BCD_Value%16));
    return Decimal_Value;
}

// Funcion puntero que permite almacenar los valores del tiempo en el Main
void *read_date(void){

	 uint16_t RTC_Hours     = 0;
	 uint16_t RTC_Minutes 	= 0;
	 uint16_t RTC_Seconds 	= 0;

	 uint16_t RTC_year 		= 0;
	 uint16_t RTC_Month 	= 0;
	 uint16_t RTC_Day 		= 0;
	 uint16_t RTC_Weekday 	= 0;

	 uint32_t RTC_Time 		= 0;
	 RTC_Time = RTC->TR;

	 uint32_t RTC_Date 		= 0;
	 RTC_Date = RTC->DR;

	 RTC_Hours	 = RTC_BcdToByte(((RTC_Time & 0x3F0000) >> 16));
	 RTC_Minutes = RTC_BcdToByte(((RTC_Time & 0x007F00) >> 8));
	 RTC_Seconds = RTC_BcdToByte((RTC_Time  & 0x7F));

	 RTC_Weekday = RTC_BcdToByte(((RTC_Date & 0xE000)   >> 13));
	 RTC_year    = RTC_BcdToByte(((RTC_Date & 0xFF0000) >> 16));
	 RTC_Month   = RTC_BcdToByte(((RTC_Date & 0x1F00)   >> 8));
	 RTC_Day     = RTC_BcdToByte((RTC_Date  & 0x3F));

	calendario[0] = RTC_Seconds;
	calendario[1] = RTC_Minutes;
	calendario[2] = RTC_Hours;
	calendario[3] = RTC_Weekday;
	calendario[4] = RTC_Day;
	calendario[5] = RTC_Month;
	calendario[6] = RTC_year;

	return calendario;
}


// Funcion de configuración del mes
void config_Month(uint16_t Mes , char *Month_){

	switch(Mes){
			case JAN:
				sprintf(Month_,"Jan");
				break;

			case FEB:
				sprintf(Month_,"Feb");
				break;

			case APR:
				sprintf(Month_,"Apr");
				break;

			case MAR:
				sprintf(Month_,"Mar");
				break;

			case MAY:
				sprintf(Month_,"May");
				break;

			case JUN:
				sprintf(Month_,"Jun");
				break;

			case JUL:
				sprintf(Month_,"Jul");
				break;

			case AUG:
				sprintf(Month_,"Aug");
				break;

			case SEP:
				sprintf(Month_,"Sep");
				break;

			case OCT:
				sprintf(Month_,"Oct");
				break;

			case NOV:
				sprintf(Month_,"Nov");
				break;

			case DEC:
				sprintf(Month_,"Dec");
				break;

			default:
				sprintf(Month_,"Error!: Invalid input \n");
				break;
			}

}

// Funcion que convierte el valor numerico del dia en un string con el nombre del dia de la semana
void config_Week(uint16_t dia, char *Weekday_){

	switch(dia){
		case MONDAY:
			sprintf(Weekday_,"Monday");
			break;

		case TUESDAY:
			sprintf(Weekday_,"Tuesday");
			break;

		case WEDNESDAY:
			sprintf(Weekday_,"Wednesday");
			break;

		case THURSDAY:
			sprintf(Weekday_,"Thursday");
			break;

		case FRIDAY:
			sprintf(Weekday_,"Friday");
			break;

		case SATURDAY:
			sprintf(Weekday_,"Saturday");
			break;

		case SUNDAY:
			sprintf(Weekday_,"Sunday");
			break;

		default:
			sprintf(Weekday_,"Error!: Invalid input \n");
			break;
		}
}
