/*
 * PWMDriver.c
 *
 *  Created on: 21/05/2023
 *      Author: Sentry
 */

#include "PWMDriver.h"

void pwm_Config(PWM_Handler_t *ptrPwmHandler){

	/*1. Activar la señal de reloj del periférico requerido*/
	if(ptrPwmHandler->ptrTIMx == TIM2){
		//Agregar codigo
	}
	else if(ptrPwmHandler->ptrTIMx == TIM3){
		//Agregar código
	}
	else if(ptrPwmHandler->ptrTIMx == TIM4){
		//Agregar código
	}
	else if(ptrPwmHandler->ptrTIMx == TIM5){
		//Agregar código
	}
	else{
		__NOP();
	}

	/*1. Cargamos la frecuencia deseada */
	setFrecuency(ptrPwmHandler);

	/*2. Cargamos el valor del Dutty-Cycle */
	setDuttyCycle(ptrPwmHandler);

	/*2a. Estamos en UP mode, el límite se carga al ARR y se comienza en 0 */
	//Agregar código

	/*3. Configuramos los bits CCxS del registro TIMy_CCMR1,
	 *de forma que sea modo salida.
	 *(Para cada canal hay un conjunto CCxS)
	 *
	 *4. Además, en el mismo "case" podemos configurar el modo del
	 *PWM, su polaridad...
	 *
	 *5. Y adempas activamos el preload bit, para que cada vez que exista un
	 *update-event el valor cargado en el CCRx será recargado en el registro
	 *"shadow" del PWM
	 */

	switch(ptrPwmHandler->config.channel){
	case PWM_CHANNEL_1:{
		//Seleccionamos como salida el canal
		//Agregar código

		//Configuramos el canal como PWM
		//Agregue código

		//Activamos la funcionalidad de pre-load
		//Agregue código

		break;
	}
	case PWM_CHANNEL_2:{
		//Seleccionamos como salida el canal
		//Agregar código

		//Configuramos el canal como PWM
		//Agregar código

		//Activamos la funcionalidad de pre-load
		//Agregar código
	}
	//Agregar los otros dos casos
	default:{
		break;
	}

	/*6. Activamos la salida seleccionada */
	enableOutput(ptrPwmHandler);

	}//fin del switch-case
}

/* Funcion para desactivar el Timer y detener todo el módulo PWM */
void stopPwmSignal(PWM_Handler_t *ptrPwmHandler){
	//Agregar código
}

/* Función encargada de activar cada uno de los canales con los que cuenta el TimerX */
void enableOutput(PWM_Handler_t *ptrPwmHandler){
	switch (ptrPwmHandler->config.channel){
	case PWM_CHANNEL_1:{
		//Activamos la salida del canal 1
		//Agregar código
		break;
	}

	//Agregar código para los otros tres casos

	default:{
		break;
	}
	}
}

/*
 * La frecuencia s definida por el conjunto formado por el preescaler
 * (PSC) y el valor límite al que llega el Timer (ARR), con estos dos se establece
 * la frecuencia
 */
void setFrecuency(PWM_Handler_t *ptrPwmHandler){

	/* Cargamos el valor del prescaler, nos define la velocidad (ns)
	 * a la cual se incrementa el Timer
	 */
	//Agregar código

	/* Cargamos el valor del ARR, el cual es el límite
	 * de incrementos del timer antes de hacer un update y reload
	 */
	//Agregar código
}

/*Función para actualizar la frecuencia, funciona de la mano con setFrecuency */
void updateFrequency(PWM_Handler_t *ptrPwmHandler){

	//Seleccionamos el canal para configurar su Dutty
	switch(ptrPwmHandler->config.channel){
	case PWM_CHANNEL_1:{
		ptrPwmHandler->ptrTIMx->CCR1 = ptrPwmHandler->config.duttyCicle;

		break;
	}

	//Agregar código con los otros casos

	default:{
		break;
	}

	}//Fin del switch-case
}

/* Función para actualizar el Dutty, funciona de la mano con setDuttyCycle */
void updateDuttyCucle(PWM_Handler_t *ptrPwmHandler, uint16_t newDutty){

	//Actualizamos el registro que manipula el Dutty
	//Agregar código

	//Llamamos a la función que cambia el Dutty y cargamos el nuevo valor
	//Agregar código
}
