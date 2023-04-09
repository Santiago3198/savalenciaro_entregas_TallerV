/*
 * ConfigEXTI_main.c
 *
 *  Created on: 23/03/2023
 *      Author: if420_01
 */

#include <stdint.h>
#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"


//Definicion de los elementos del programa

GPIO_Handler_t handler_Led2 = {0}; //PA5
GPIO_Handler_t handler_UserButton;
BasicTimer_Handler_t handlerBlinkytimer = {0};  //TIM2





int main(void){



	while(1){

	}

	return 0;

//Prototipos de las funciones del main
void init_Hardware(void){

	//Inicializando el led2

	handler_Led2.pGPIOx =GPIOA;
	handler_Led2.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
	handler_Led2.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handler_Led2.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handler_Led2.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handler_Led2.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	//Cargar la configuracion en los registros MCU

	GPIO_Config(&handler_Led2);

	GPIO_WritePin(&handler_Led2, SET);

	//Configuramos el TIM2 para hacer un BLinky
	handlerBlinkyTimer.ptrTIMx = TIM2;
	handlerBlinkyTimer.TIMx_Config.TIMx_mode = BTIMER_MODE_UP; //Cuenta de forma ascendente
	handlerBlinkyTimer.TIMx_Config.TIMx_speed = BTIMER_SPEED_1ms; //Se incrementa cada 1 ms
	handlerBlinkyTimer.TIMx_Config.TIMx_period = 250;  //Genera una interrupcion cada 250 ms
	handlerBlinkyTimer.TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUPT_ENABLE;


	//FALTA TERMINAR TIMERS

	//2a. Activar la señal de reloj para el sysconfig

	RCC->APB2ENR |= RCC_APB2_SYSCFGEN;

	//2b. Configurar el multiplexor 13

	SYSCFG->EXTICR[3] &= ~(0xF << SYSCFG_EXTICR4_EXTI13_Pos); //Limpiamos la posicion del mux13
	SYSCFG->EXTICR[3] |= SYCFG_EXTICR4_EXTI13_PC;  //Asignamos el puerto C

	//3 configurar el EXTI

	EXTI->FTSR = 0;  //Desactivamos todas las posibles detecciones de flancos de bajada
	EXTI->RTSR = 0; //LLevamos a unvalor conocido el registro
	EXTI->RTSR |= EXTI_RTSR_TR13; //Activando la deteccion del flanco
	EXTI->IMR = 0;
	EXTI->IMR |= EXTI_IMR_IM13;  //Activamos la interrupcion EXTI_13

	//4a. Deshabilitar las interrupciones globales

	__disable_irq();

	//4b. Incluir la interrupcion en el NVIC

	NVIC_EnableIRQ(EXTI15_10_IRQn);

}
//Verificar la interrupcion
//Bajar la bandera
//Llamar al Callback

void EXTI15_10_IRQHandler(void){

	//Verificar la interrupcion
	if((EXTI->PR & EXTI_PR_PR13) != 0){
		//Efectivamente es la interrupcion 13 la que se activo
		EXTI->PR |= EXTI_PR_PR13;  //Bajando la bandera de la interrupcion
		callback_exti13();

	}
}

}
