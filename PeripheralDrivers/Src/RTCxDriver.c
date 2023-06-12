/*
 * RTCDriver.c
 *
 *  Created on: 8/6/2023
 *      Author: Sentry
 */
#include "RTCxDriver.h"
// Se crean unas variables globales que nos ayudaran a leer la fecha y la hora
uint8_t tiempo[7] = { 0 };
uint8_t fecha[7] = { 0 };

/*Configuramos el RTC*/
void ConfigRTC(RTC_Config_t *ptrHandlerRTC){
	// Para poder acceder a los registros que configuran el RTC, debemos
	// primero activar el PWR
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	//En los registros del PWR, activamos el acceso para configurar los registros del RCT
	PWR->CR |= PWR_CR_DBP;
	//Habilitamos el RTC
	RCC->BDCR |= RCC_BDCR_RTCEN;
	//Activamos la señal de reloj del LSE
	RCC->BDCR |= RCC_BDCR_LSEON;
	//Configuramos el LSE como la señal de reloj que utilizaremos en el LSE
	RCC->BDCR |= RCC_BDCR_RTCSEL_0;
	//Esperamos que el LSE se estabilice
	while (!(RCC->BDCR & RCC_BDCR_LSERDY)){
		__NOP();
	}
	/*Despues de realizar la activacion del RTC, los registros de este siguen bloqueados, por lo que hay que ingresar ciertos bits
	 * los cuales permiten realizar la configuracion de los registros necesarios.*/
	RTC->WPR |= 0xCA;
	RTC->WPR |= 0x53;
	//Inicializamos el RTC para poder programar fecha, hora y prescaler
	RTC->ISR |= RTC_ISR_INIT;
	//Esperamos a que termine la inicialización ( entre 1 y 2 ciclos de reloj)
	while (!(RTC->ISR & RTC_ISR_INITF)) {
		__NOP();
	}
	//Configuramos los registros de Preescaler
	// Dividimos la frecuencia real del RTC configurado con el LSE ( 32768 Hz), por un primer factor de prescaler
	// sumado 1, que se programa en los bits del 16 al 22 del RTC_PRER.
	//Programamos un valor del 127. Para que asi la frecuencia de salida sea de 256 Khz
	RTC->PRER |= 127 << RTC_PRER_PREDIV_A_Pos;
	// Ahora programamos un segundo prescaler, con el cual llegaremos a nuestra salida deseada que es de 1Khz,
	//en el prescaler anterior llegamos a una salida de 256Khz, por lo cual para este prescaler lo configuramos e
	// con 255(a este tambien se le suma 1 en el registro). Teniendo ya el prescaler configurado para tener una
	// frecuencia de 1Khz
	RTC->PRER |= 255 << RTC_PRER_PREDIV_S_Pos;
	//Configuramos el RTC para que tome valores directamente desde los contadores del calendario
	// y no los registros sombra
	RTC->CR |= RTC_CR_BYPSHAD;
	//Inicializamos los contadores del calendario en 0
	RTC->TR = 0;
	RTC->DR = 0;
	//Limpiamos el registro que se encarga de seleccionar el formato de hora que deseamos configurar
	RTC->CR &= ~RTC_CR_FMT;
	// Y se carga la configuracion del usuario para el formato de hora
	if (ptrHandlerRTC->FORMAT_NOTATION == MODE_24H){
		RTC->CR &= ~RTC_CR_FMT;
	}else if(ptrHandlerRTC->FORMAT_NOTATION == MODE_12H){
		RTC->CR &= ~RTC_CR_FMT;
		RTC->CR |= RTC_CR_FMT;
	}
	// Modificamos el registro RTC_TR que se encarga de la hora
	// Cargamos nuestar configuracion de si es 24 horas, o si es formato AM-PM cual es el valor de la hora a cargar
	if (ptrHandlerRTC->FORMAT_NOTATION == MODE_24H){
		RTC->TR &= ~RTC_TR_PM;
	}else if(ptrHandlerRTC->FORMAT_AM_PM == MODE_AM){
		RTC->TR &= ~RTC_TR_PM;
	}else if(ptrHandlerRTC->FORMAT_AM_PM == MODE_PM){
		RTC->TR &= ~RTC_TR_PM;
		RTC->TR |= RTC_TR_PM;
	}
	// Estos registros son configurados en formato BDC, es decir, para pasar de decimal a binario,
	// dividimos el numero en decimales y unidades, y estos valores se cargan en cada uno de los registros correspondientes

	//Limpiar el registro
	RTC->ISR = 0;
	//Inicializamos el RTC para poder programarlo
	RTC->ISR |= RTC_ISR_INIT;
	//Esperamos hasta que el RTC este listo para ser actualizado
	while(!(RTC->ISR & RTC_ISR_INITF)){
		__NOP();
	}
	//Configuración de la hora
	/*Para configurar la hora en el RTC se deben de modificar dos registros, los cuales son RTC->TR->HT y
	 * RTC->TR->HU, los cuales estan en formato BCD*/
	//Configuramos el HT
	RTC->TR |= (ptrHandlerRTC->RTC_HOURS / 10) << RTC_TR_HT_Pos;
	//Configuramos el HU
	RTC->TR |= (ptrHandlerRTC->RTC_HOURS % 10) << RTC_TSTR_HU_Pos;
	//Configuramos los minutos
	/*Al igual que las horas hay que modificar dos registros en el RTC->TR, los cuales son el MNT y MNU,
	 * los cuales también estan en el formato BCD*/
	//Configuramos el MNT
	RTC->TR |= (ptrHandlerRTC->RTC_MINUTS / 10) << RTC_TR_MNT_Pos;
	//Configuramos el MNU
	RTC->TR |= (ptrHandlerRTC->RTC_MINUTS % 10) << RTC_TR_MNU_Pos;
	//Configuracion de los segundos
	//Similar a los otros registros se realiza el mismo proceso
	RTC->TR |= ptrHandlerRTC->RTC_SEC / 10 << RTC_TR_ST_Pos;
	RTC->TR |= ptrHandlerRTC->RTC_SEC % 10 << RTC_TR_SU_Pos;

	// Configuracion de la fecha
	//Configuracion de los dias
	RTC->DR |= ptrHandlerRTC->RTC_DAYS / 10 << RTC_DR_DT_Pos;
	RTC->DR |= ptrHandlerRTC->RTC_DAYS  % 10 << RTC_DR_DU_Pos;

	// Se configura el mes
	RTC->DR |= ptrHandlerRTC->RTC_MONTH  % 10 << RTC_DR_MU_Pos;
	RTC->DR |= ptrHandlerRTC->RTC_MONTH  / 10 << RTC_DR_MT_Pos;

	// Se configura los años, donde hay que restarles 2000, para que asì solo configurar las decenas y las unidades
	RTC->DR |= ((ptrHandlerRTC->RTC_YEARS  - 2000) % 10) << RTC_DR_YU_Pos;
	RTC->DR |= (ptrHandlerRTC->RTC_YEARS  - 2000) / 10 << RTC_DR_YT_Pos;
	// Activamos la señal de reloj del RTC
	RCC->BDCR |= RCC_BDCR_RTCEN;
	// Desactivamos el modo de inicializacion, donde se podian cofigurar los registros de fecha y hora
	RTC->ISR &= ~RTC_ISR_INIT;
	//Desactivamos el acceso a los registros del RTC
	PWR->CR &= ~ PWR_CR_DBP;
	// Y se escribe una clave erronea para así poder de nuevo proteger estos registros
	RTC->WPR = (0xFF);
}

/*Funcion para obtener la hora de los registros del RTC*/
uint8_t* read_time(void) {

	// Se crean variables dentro de la funcion para así poder guardar los datos obtenidos dentro del arreglo
	uint8_t RTC_HoraUnits = 0;
	uint8_t RTC_HoraTens = 0;
	uint8_t RTC_MinutosUnits = 0;
	uint8_t RTC_MinutosTens = 0;
	uint8_t RTC_SegundosUnits = 0;
	uint8_t RTC_SegundosTens = 0;
	uint8_t RTC_AmPm = 0;

	// Revisamos el registro RTC_TR donde se configura la hora, y con ayuda de mascaras cogemos cada valor
	// de cada uno de los registros que necesitamos
	RTC_HoraUnits = ((RTC->TR & RTC_TR_HU_Msk) >> RTC_TR_HU_Pos);
	RTC_HoraTens = ((RTC->TR & RTC_TR_HT_Msk) >> RTC_TR_HT_Pos);
	RTC_MinutosUnits = ((RTC->TR & RTC_TR_MNU_Msk) >> RTC_TR_MNU_Pos);
	RTC_MinutosTens = ((RTC->TR & RTC_TR_MNT_Msk) >> RTC_TR_MNT_Pos);
	RTC_SegundosUnits = ((RTC->TR & RTC_TR_SU_Msk) >> RTC_TR_SU_Pos);
	RTC_SegundosTens = ((RTC->TR & RTC_TR_ST_Msk) >> RTC_TR_ST_Pos);
	RTC_AmPm = ((RTC->TR) >> RTC_TR_PM_Pos);

	// Se guardan en orden de unidades y luego decenas
	tiempo[0] = RTC_SegundosUnits;
	tiempo[1] = RTC_SegundosTens;
	tiempo[2] = RTC_MinutosUnits;
	tiempo[3] = RTC_MinutosTens;
	tiempo[4] = RTC_HoraUnits;
	tiempo[5] = RTC_HoraTens;
	tiempo[6] = RTC_AmPm;

	// Se devuelve la matriz con el tiempo
	return tiempo;
}

/*Funcion para obtener la fecha de los registros del RTC*/
uint8_t* read_date(void) {

	uint8_t RTC_dayUnits = 0;
	uint8_t RTC_dayTens = 0;
	uint8_t RTC_month = 0;
	uint8_t RTC_yearUnits = 0;
	uint8_t RTC_yearTens = 0;

	RTC_dayTens = ((RTC->DR & RTC_DR_DT_Msk) >> RTC_DR_DT_Pos);
	RTC_dayUnits = ((RTC->DR & RTC_DR_DU_Msk) >> RTC_DR_DU_Pos);
	RTC_yearUnits = ((RTC->DR & RTC_DR_YU_Msk) >> RTC_DR_YU_Pos);
	RTC_yearTens = ((RTC->DR & RTC_DR_YT_Msk) >> RTC_DR_YT_Pos);
	// Se organiza el mes para entregarlo en un solo arreglo
	RTC_month = (((RTC->DR & RTC_DR_MT_Msk) >> RTC_DR_MT_Pos) * 10) + (((RTC->DR & RTC_DR_MU_Msk) >> RTC_DR_MU_Pos) % 10);

	fecha[0] = RTC_dayUnits;
	fecha[1] = RTC_dayTens;
	fecha[2] = RTC_month;
	fecha[3] = RTC_yearUnits;
	fecha[4] = RTC_yearTens;


	return fecha;
}

