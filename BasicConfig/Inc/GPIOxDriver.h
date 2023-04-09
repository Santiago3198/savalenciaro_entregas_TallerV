/*
 * GPIOxDrive.h
 *
 *  Created on: 9/03/2023
 *      Author: if420_01
 */


/*Definiciones relacionadas con el debido manejo y control del periferico GPIOx (Funciones del controlador)*/


#ifndef GPIOXDRIVER_H_
#define GPIOXDRIVER_H_
#include "stm32fxxhal.h"

typedef struct
{
	uint8_t GPIO_PinNumber;				//PIN CON EL QUE DESEAMOS TRABAJAR
	uint8_t GPIO_PinMode;				//MODO DE LA CONFIGURACION: ENTRADA, ETC
	uint8_t GPIO_PinSpeed;				//VEOLOCIDAD DE RESPUESTA DEL PIN
	uint8_t GPIO_PinPuPdControl;		//SELECCION DE SALIDA PULL-UP, PULL-DOWN O LIBRE
	uint8_t GPIO_PinOPType;				//SELECCIONA SALIDA PUPD O OPEN/DRAIN
	uint8_t GPIO_PinAltFunMode;			//SELECCIONA CUAL ES LA FUNCION ALTERNATIVA QUE SE ESTÁ CONFIGURANDO

}GPIO_PinConfig_t;

/*La siguiente estructura contiene dos elementos:
 * 1.Direccion del puerto que se esta utilizando
 * 2.Configuracion especifica del pin que se esta utilizando
 */

typedef struct
{
	GPIOx_RegDef_t		*pGPIOx;				//Direccion del puerto al que el pin corresponde
	GPIO_PinConfig_t	GPIO_PinConfig;			//Configuracion del pin

}GPIO_Handler_t;

/*Definiciones de las cabeceras de las funciones del GPIOxDriver*/

void GPIO_Config (GPIO_Handler_t *pGPIOHandler);
void GPIO_WritePin(GPIO_Handler_t *pPinHandler, uint8_t newState);
uint32_t GPIO_ReadPin(GPIO_Handler_t *pPinHandler);



#endif /* GPIOXDRIVER_H_ */
