/*
 * BasicTimer.c
 *
 *  Created on: 23/03/2023
 *      Author: if420_01
 */

#include "BasicTimer.h"

//Variable que guarda la referencia del periferico que se está utilizando

TIM_TypeDef		*ptrTimerUsed;

/*Funcion en la que cargamos la configuracion del Timer
 * Recordar que siempre se debe comenzar con activar la señal de reloj del periferico
 * que se está utilizando.
 * en este caso debemos ser cuidadosos al momento de utilizar la s interrupciones.
 * Los Timers están conectados directamente al elemento NVIC del cortex _ Mx
 * Debemos configurar y/o utilizar:
 * 		-TIMx_CR1 (control register 1)
 * 		-TIMx_SMCR (slave mode control register) -> mantener en 0 para modo Timer Basico
 * 		-TIMx_DIER (DMA and Interrupt enable register)
 * 		-TIMx_SR (status register)
 * 		-TIMx_CNT (counter)
 * 		-TIMx_PSVC (pre scaler)
 * 		-TIMx_ARR (AutoReload register)
 *
 *Como vamos a trabajar con interrupciones, antes de configurar una nueva, debemo descativar
 *el sistema global de interrupciones, activar la IRQ especifica y luego volver a encender
 *el sistema
 */

void BasicTimer_Config(BasicTimer_Handler_t *ptrBTimerHandler){
	//Guardamos una referencia al periferico que estamos utilizando
	ptrTimerUsed = ptrBTimerHandler->ptrTIMx;

	/*0. Desactivamos las interrupciones globales mientras configuramos el sistema.*/
	__disable_irq();

	/*1.Activar la señal de reloj del periferico requerido*/
	if(ptrBTimerHandler->ptrTIMx == TIM2){
		//Registro del RCC que nos activa la señal de reloj para el TIM2

	}
	else if(ptrBTimerHandler->ptrTIMx == TIM3){
		//Registro del RCC que nos activa la señal de reloj para el TIM2

	}
	else{
		__NOP();

	}

	/*2.Configuramos el pre-escaler
	 * Recordar que el pre-escaler nos indica la velocidad a la que se incrementa el counter,
	 * de forma que periodo_incremento * veces_incremento_counter = preiodo_update
	 * Modificar el valor del registro PSC en el TIM utilizado
	 */

	/*ESCRIBIR EL CODIGO*/

	/*3.Configuramos la direccion del counter (up/down)*/

	if(ptrBTimerHandler->TIMx_Config.TIMx_mode == BTIMER_MODE_UP){

		/*3a. Estamos en UP_Mode, el limite se carga en ARR y se comienza en 0 */
		//Configurar el registro que nos controla el modo up or down

		/*ESCRIBIR CODIGO*/

		/*3b. Configuramos el Autoreload. Este es el "limite" hasta donde el CNT va a contar */
		ptrBTimerHandler->ptrTIMx->ARR = ptrBTimerHandler->TIMx_Config.TIMx_period - 1;

		/*3c. Reiniciamos el registro counter*/

		/*ESCRIBIR CODIGO*/
	}



}
