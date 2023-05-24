/*
 * PLLDriver.h
 *
 *  Created on: 23/05/2023
 *      Author: Sentry
 */

#ifndef PLLDRIVER_H_
#define PLLDRIVER_H_

#include "stm32f4xx.h"

//Función para configurar el PLL a 80MHz
void configPLL(void);

//Función que entrega la configuración actual del PLL
uint16_t getConfigPLL(void);

#endif /* PLLDRIVER_H_ */
