/*
 * GPIOxDrive.h
 *
 *  Created on: 9/03/2023
 *      Author: if420_01
 */

#ifndef GPIOXDRIVER_H_
#define GPIOXDRIVER_H_
#include "stm32fxxhal.h"

typedef struct
{
	uint8_t GPIO_PinNumber;		//PIN CON EL QUE DESEAMOS TRABAJAR
	uint8_t GPIO_PinMode;		//MODO DE LA CONFIGURACION: ENTRADA, ETC
	uint8_t GPIO_PinSpeed;		// VEOLOCIDAD DE RESPUESTA DEL PIN
	uint8_t GPIO_PinPuPdControl;	//SELECCION DE SALIDA PULL-UP, PULL-DOWN O LIBRE
	uint8_t GPIO_PinOPType;		//SELECCIONA SALIDA PUPD O OPEN/DRAIN
	uint8_t GPIO_PinAltFunMode;	//SELECCIONA CUAL ES LA FUNCION ALTERNATIVA QUE SE ESTÁ CONFIGURANDO

}GPIO_PinConfig_t;

typedef struct
{
	GPIOx_RegDef_t		*pGPIOx;
	GPIO_PinConfig_t	GPIO_PinConfig;

}GPIO_Handler_t;

void GPIO_Config (GPIO_Handler_t *pGPIOHandler);
void GPIO_WritePin(GPIO_Handler_t *pPinHandler, uint8_t newState);
uint32_t GPIO_ReadPin(GPIO_Handler_t *pPinHandler);

//Segundo punto Tarea 2. 

void GPIOxTooglePin (GPIO_Handler_t *pGPIOHandler);



#endif /* GPIOXDRIVER_H_ */
