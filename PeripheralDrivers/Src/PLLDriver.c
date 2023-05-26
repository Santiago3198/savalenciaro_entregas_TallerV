/*
 * PLLDriver.c
 *
 *  Created on: 23/05/2023
 *      Author: Sentry
 */

#include "PLLDriver.h"

//Variable que guarda el valor de la configuración del PLL
uint16_t freqPLL = 0;

void configPLL(void){

	//Se habilita el reloj interno HSI
	RCC->CR |= RCC_CR_HSION;

	//Aseguramos que el PLL està apagado
	RCC->CR &= ~RCC_CR_PLLON;

	//Seleccionamos el reloj que alimenta el PLL
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLSRC;

	//Configuramos los preescaler para que los buses funcionen con la nueva velocidad del dispositivo
	//APB1
	RCC->CFGR &= ~RCC_CFGR_PPRE1;
	RCC->CFGR |= RCC_CFGR_PPRE1_2;

	//APB2
	RCC->CFGR &= ~RCC_CFGR_PPRE2;

	//Configuración del bit PLLM
	RCC->PLLCFGR |= (10 << 0);

	//Configuración del bit PLLN
	RCC->PLLCFGR |= (100 << 6);

	//Configuración del bit PLLP
	RCC->PLLCFGR |= (2 << 16);

	//Configuración de la memoria flash
	FLASH->ACR &= ~(FLASH_ACR_LATENCY);

	//Se configura la latencia para 80 MHz
	FLASH->ACR |= FLASH_ACR_LATENCY_2WS;

	//Encender el PLL
	RCC->CR |= RCC_CR_PLLON;

	//Se le da tiempo al equipo para que cargue la nueva configuración de la frecuencia asignada
	while(!(RCC->CR & RCC_CR_PLLRDY)){
		__NOP();
	}

	//Seleccionamos el PLL como reloj del sistema
	RCC->CFGR &= ~RCC_CFGR_SW_0;
	RCC->CFGR |= RCC_CFGR_SW_1;

	//Activar el PLL para todo el dispositivo
	RCC->CFGR |= RCC_CFGR_SW_PLL;
}

uint16_t getConfigPLL(void){

	/* Variables donde se van a guardar en variables los valores que hay en los
	 * bits PLLN, PLLM y PLLP, esto para poder manipularlos y operarlos entre ellos
	 */

	uint16_t PLLN = 0;
	uint16_t PLLM = 0;
	uint8_t PLLP = 0;

	//Variables donde se guardan los valores de frecuencia
	uint8_t f_vco = 0;


	//PLLN
	//Se lleva el valor a la primera posición del registro y asignamos el valor del registro a la variable
	PLLN = (RCC->PLLCFGR >> RCC_PLLCFGR_PLLN_Pos);
	PLLN &= RCC_PLLCFGR_PLLN;

	//PLLM
	//Se lleva el valor a la primera posición del registro y asignamos el valor del registro a la variable
	PLLM = (RCC->PLLCFGR >> RCC_PLLCFGR_PLLM_Pos);
	PLLM &= RCC_PLLCFGR_PLLM;

	//PLLP
	//Se lleva el valor a la primera posición del registro y asignamos el valor del registro a la variable
	PLLP = (RCC->PLLCFGR >> RCC_PLLCFGR_PLLP_Pos);
	PLLP &= RCC_PLLCFGR_PLLP;

	/* Dependiendo de los valores que se le asignen a los bits PLLP
	 * entonces la variable solo puede tomar uno de los siguientes
	 * 4 valores
	 */

	if(PLLP == 0b00){
		PLLP = 2;
	}else if(PLLP == 0b01){
		PLLP = 4;
	}else if(PLLP == 0b10){
		PLLP = 6;
	}else if(PLLP == 0b11){
		PLLP = 8;
	}

	/* Se realizan las operaciones correspondientes y se guarda el valor de
	 * la frecuencia en la variable "config" para posteriormente ser mostrada
	 */

	f_vco = 16*(PLLN/PLLM);

	freqPLL = f_vco/PLLP;

	return freqPLL;
}
