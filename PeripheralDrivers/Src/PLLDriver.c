/*
 * PLLDriver.c
 *
 *  Created on: 27.5.2023
 *    Author: Sentry
 */

#include "stm32f4xx.h"
#include "PLLDriver.h"

void configPLL (void){

		//APB1 = max Freq 50MHz
		//Se configura el prescaler para que divida por 2
		RCC->CFGR &= ~RCC_CFGR_PPRE1;
		RCC->CFGR |= RCC_CFGR_PPRE1_2;

		//APB2 = max Freq 100MHz
		//No es necesario configurar prescaler en el APB2
		RCC->CFGR &= ~RCC_CFGR_PPRE2_0;
		RCC->CFGR &= ~RCC_CFGR_PPRE2_1;
		RCC->CFGR &= ~RCC_CFGR_PPRE2_2;

		//Activar el HSI como alimentación del PLL
		RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLSRC;
		RCC->PLLCFGR |= (RCC_PLLCFGR_PLLSRC_HSI);

		//Apagar el PLL
		RCC->CR &= ~ RCC_CR_PLLON;

		//Limpiar los bit M del registro PLLCFGR
		RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM_0;
		RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM_1;
		RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM_2;
		RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM_3;
		RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM_4;
		RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM_5;

		//PLLM = 10
		RCC->PLLCFGR |= (0b1010 << RCC_PLLCFGR_PLLM_Pos);

		//Limpiar los bit N del registro PLLCFGR
		RCC->PLLCFGR &= ~ RCC_PLLCFGR_PLLN_0;
		RCC->PLLCFGR &= ~ RCC_PLLCFGR_PLLN_1;
		RCC->PLLCFGR &= ~ RCC_PLLCFGR_PLLN_2;
		RCC->PLLCFGR &= ~ RCC_PLLCFGR_PLLN_3;
		RCC->PLLCFGR &= ~ RCC_PLLCFGR_PLLN_4;
		RCC->PLLCFGR &= ~ RCC_PLLCFGR_PLLN_5;
		RCC->PLLCFGR &= ~ RCC_PLLCFGR_PLLN_6;
		RCC->PLLCFGR &= ~ RCC_PLLCFGR_PLLN_7;
		RCC->PLLCFGR &= ~ RCC_PLLCFGR_PLLN_8;

		//PLLN = 100		//001100100
		RCC->PLLCFGR |= (0b001100100 << RCC_PLLCFGR_PLLN_Pos);

		//PLLP = 2
		RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLP);

		//Limpiamos el registro de la FLASH
		FLASH->ACR &= ~(FLASH_ACR_LATENCY);

		//Configurar 2 WS para el reloj
		FLASH->ACR |= FLASH_ACR_LATENCY_2WS;


		//Habilitar el PLL
		RCC->CR |= RCC_CR_PLLON;
		while (!(RCC->CR & (RCC_CR_PLLRDY)));

		//PLL como reloj del sistema
		RCC->CFGR &= ~RCC_CFGR_SW_0;
		RCC->CFGR |= RCC_CFGR_SW_1;
}

uint16_t getConfigPLL(void){
	//Variable auxiliar para guardar la configuracion del equipo
	uint16_t auxVariable = 0;
	//Variables para guardar los valores de PLLM, PLLN y PLLP
	uint8_t auxPLLM = 0;
	uint16_t auxPLLN = 0;
	uint8_t auxPLLP = 0;
	//Variable para guardar la frecuencia de entrada del PLL
	uint8_t auxFreqIn = 0;

	//le damos el valor a la frecuencia de entrada del PLL
	auxFreqIn = 16;

	//Guardo los valores del PLLM, PLLN y PLLP en sus respectivas varaibles
	//PLLM
	auxPLLM = (RCC->PLLCFGR >> RCC_PLLCFGR_PLLM_Pos);
	//Aplicamos la mascara para obtener solo el valor del registro PLLM
	auxPLLM &= (RCC_PLLCFGR_PLLM);

	//PLLN
	auxPLLN = (RCC->PLLCFGR >> RCC_PLLCFGR_PLLN_Pos);
//	//Aplicamos la mascara para obtener solo el valor del registro PLLN
//	auxPLLN &= (RCC_PLLCFGR_PLLN);

	//PLLP
	auxPLLP = (RCC->PLLCFGR >> RCC_PLLCFGR_PLLP_Pos);
	//Aplicamos la mascara para obtener solo el valor del registro PLLP
	auxPLLP &= (RCC_PLLCFGR_PLLP);
	//Hacemos un switch case para guardar el valor real del registro PLLP
	switch(auxPLLP){
	case 0:{
		auxPLLP = 2;
		break;
	}
	case 1:{
		auxPLLP = 4;
		break;
	}
	case 2:{
		auxPLLP = 6;
		break;
	}
	case 3:{
		auxPLLP = 8;
		break;
	}
	}

	//Calculando para el VCO clock
	auxVariable = auxFreqIn * (auxPLLN / (uint16_t) auxPLLM);
	//Calculando PLL general clock output
	auxVariable = auxVariable / auxPLLP;

	return auxVariable;
}
