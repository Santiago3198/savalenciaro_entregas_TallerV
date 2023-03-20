/*
 * GPIOxDrive.h
 *
 *  Created on: 9/03/2023
 *      Author: if420_01
 */

#ifndef GPIOXDRIVER_H_
#define GPIOXDRIVER_H_

#include <stdint.h>

#define GPIO_MODE_IN		0
#define GPIO_MODE_OUT		1
#define GPIO_MODE_ALTFN		2
#define GPIO_MODE_ANALOG	3

#define GPIO_OTYPE_PUSHPULL		0
#define GPIO_OTYPE_OPENDRAIN	1

#define GPIO_OSPEEDR_LOW		0
#define GPIO_OSPEEDR_MEDIUM		1
#define GPIO_OSPEEDR_FAST		2
#define GPIO_OSPEEDR_HIGH		3

#define GPIO_PUPDR_NOTHING		0
#define GPIO_PUPDR_PULLUP		1
#define GPIO_PUPDR_PULLDOWN		2
#define GPIO_PUPDR_RESERVED		3

#define PIN_0		0
#define PIN_1		1
#define PIN_2		2
#define PIN_3		3
#define PIN_4		4
#define PIN_5		5
#define PIN_6		6
#define PIN_7		7
#define PIN_8		8
#define PIN_9		9
#define PIN_10		10
#define PIN_11		11
#define PIN_12		12
#define PIN_13		13
#define PIN_14		14
#define PIN_15		15

#define AF0		0b0000
#define AF1		0b0001
#define AF2		0b0010
#define AF3		0b0011
#define AF4		0b0100
#define AF5		0b0101
#define AF6		0b0110
#define AF7		0b0111
#define AF8		0b1000
#define AF9		0b1001
#define AF10	0b1010
#define AF11	0b1011
#define AF12	0b1100
#define AF13	0b1101
#define AF14	0b1110
#define AF15	0b1111


typedef struct
{
	uint8_t GPIO_PinNumber;			//PIN CON EL QUE DESEAMOS TRABAJAR
	uint8_t GPIO_PinMode;			//MODO DE LA CONFIGURACION: ENTRADA, ETC
	uint8_t GPIO_PinSpeed;			// VEOLOCIDAD DE RESPUESTA DEL PIN
	uint8_t GPIO_PinPuPdControl;	//SELECCION DE SALIDA PULL-UP, PULL-DOWN O LIBRE
	uint8_t GPIO_PinOPType;			//SELECCIONA SALIDA PUPD O OPEN/DRAIN
	uint8_t GPIO_PinAltFunMode;		//SELECCIONA CUAL ES LA FUNCION ALTERNATIVA QUE SE ESTÁ CONFIGURANDO

}GPIO_PinConfig_t;

typedef struct
{
	GPIO_TypeDef_t		*pGPIOx;
	GPIO_PinConfig_t	GPIO_PinConfig;

}GPIO_Handler_t;

void GPIO_Config (GPIO_Handler_t *pGPIOHandler);
void GPIO_WritePin(GPIO_Handler_t *pPinHandler, uint8_t newState);
uint32_t GPIO_ReadPin(GPIO_Handler_t *pPinHandler);



#endif /* GPIOXDRIVER_H_ */
