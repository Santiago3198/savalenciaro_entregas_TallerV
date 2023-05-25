/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Santiago Valencia Roldan 
 * @brief          : Main program body
 ******************************************************************************
*/
/*
 * Descomentar la siguiente linea y agregar al main si se requiere el uso
 * de la FPU.
 * SCB->CPACR |= (0xF << 20);
 * Activacion del coprocesador matematico
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"
#include "USARTxDriver.h"
#include "I2CDriver.h"
#include "PWMDriver.h"
#include "PLLDriver.h"
#include "SysTickDriver.h"

int main(void){

	/*Loop forever*/
	while(1){

	}
	return 0;
}


