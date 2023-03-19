/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Santiago Valencia Roldan
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include <stdint.h>
#include <stm32fxxhal.h>
int main(void)
{
	//Configuracion inicial del MCU
	//RCC->AHB1ENR = 0x001
	RCC-> AHB1ENR	&= ~(1 << 0);	//Borrar la posisión 0 del registro AHB1ENR
	RCC-> AHB1ENR	|= 1 << 0;		//Activando la señal de relloj del GPIOA

	//Configurar MODER
	GPIOA->MODER &= ~(0b11 << 10);		//Limpiando la posicion correspondiente al pin 5
	GPIOA->MODER |= (0b01 << 10);		//Configurar el pin C5 como salida general
	GPIOA->OTYPER &= ~(0b1 << 5);		//pin C5 configurado como salida Push-Pull
	GPIOA->OSPEEDR &= ~(0b11 << 10);	//Limpiando las posiciones correspondientes al pin C5
	GPIOA->OSPEEDR |= (0b01 << 10);		//Configurando la velocidad como Fast
	GPIOA->ODR &= ~(0b1 << 5);			//Limpiando posicion 5 - LED Apagado
	GPIOA->ODR |= (0b1 << 5);			//Escribiendo 1 en posicion 5 - LED Encendido
    /* Loop forever */
	while(1){

	}

	return 0;
}
