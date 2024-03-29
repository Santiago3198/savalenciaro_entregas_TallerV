/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
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


#include <stm32f4xx.h>
#include <stdint.h>
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"

void displayWrite(uint8_t number);
void switchDigit(void);
void wormPath(uint8_t number);

GPIO_Handler_t handlerBlinky 				= {0};		//PIN A6
GPIO_Handler_t handlerUserButton 			= {0};		//PIN C13
GPIO_Handler_t handlerAuxButton 			= {0};		//PIN C4
GPIO_Handler_t handlerEncoder 				= {0};		//PIN B3
BasicTimer_Handler_t handlerTimerBlinky 	= {0};
BasicTimer_Handler_t handlerTimerDisplay 	= {0};
EXTI_Config_t extiConfigEncoder 			= {0};
EXTI_Config_t extiConfigAuxButton 			= {0};

int8_t contador 	= 0;
uint8_t modeWorm 	= 0;
int8_t wormCounter 	= 0;

//Declaracion de los pines del display

GPIO_Handler_t 	ledA 			= {0};   		//LED A        	PA1
GPIO_Handler_t 	ledB 	 		= {0};   		//LED B 		PA4
GPIO_Handler_t 	ledC 	 		= {0};   		//LED C 		PB13
GPIO_Handler_t 	ledD 			= {0};   		//LED D 		PB1
GPIO_Handler_t 	ledE 			= {0};   		//LED E			PB15
GPIO_Handler_t 	ledF 			= {0};   		//LED F			PB0
GPIO_Handler_t 	ledG 	 		= {0};   		//LED G			PB2
GPIO_Handler_t 	DispUni 		= {0};   		//ANODO 1	   	PC1
GPIO_Handler_t 	DispDec 		= {0};  		//ANODO 2      	PA0

int main(void){

	//Configuración inicial del blinky
	handlerBlinky.pGPIOx = GPIOA;
	handlerBlinky.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerBlinky.GPIO_PinConfig.GPIO_PinNumber = PIN_6;
	handlerBlinky.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerBlinky.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEEDR_FAST;
	handlerBlinky.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerBlinky.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	GPIO_Config(&handlerBlinky);

	//Configuracion inicial del TIM2
	handlerTimerBlinky.ptrTIMx = TIM2;
	handlerTimerBlinky.TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
	handlerTimerBlinky.TIMx_Config.TIMx_speed = BTIMER_SPEED_100us;
	handlerTimerBlinky.TIMx_Config.TIMx_period = 2500;
	handlerTimerBlinky.TIMx_Config.TIMx_interruptEnable = 1;
	BasicTimer_Config(&handlerTimerBlinky);

	//Configuracion ledA
	ledA.pGPIOx = GPIOA;
	ledA.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	ledA.GPIO_PinConfig.GPIO_PinNumber = PIN_1;
	ledA.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	ledA.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEEDR_FAST;
	ledA.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	ledA.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	GPIO_Config(&ledA);

	//Configuracion ledB
	ledB.pGPIOx = GPIOA;
	ledB.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	ledB.GPIO_PinConfig.GPIO_PinNumber = PIN_4;
	ledB.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	ledB.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEEDR_FAST;
	ledB.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	ledB.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	GPIO_Config(&ledB);

	//Configuracion ledC
	ledC.pGPIOx = GPIOB;
	ledC.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	ledC.GPIO_PinConfig.GPIO_PinNumber = PIN_13;
	ledC.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	ledC.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEEDR_FAST;
	ledC.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	ledC.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	GPIO_Config(&ledC);

	//Configuracion ledD
	ledD.pGPIOx = GPIOB;
	ledD.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	ledD.GPIO_PinConfig.GPIO_PinNumber = PIN_1;
	ledD.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	ledD.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEEDR_FAST;
	ledD.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	ledD.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	GPIO_Config(&ledD);

	//Configuracion ledE
	ledE.pGPIOx = GPIOB;
	ledE.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	ledE.GPIO_PinConfig.GPIO_PinNumber = PIN_15;
	ledE.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	ledE.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEEDR_FAST;
	ledE.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	ledE.GPIO_PinConfig.GPIO_PinAltFunMode = AF13;
	GPIO_Config(&ledE);

	//Configuracion ledF
	ledF.pGPIOx = GPIOB;
	ledF.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	ledF.GPIO_PinConfig.GPIO_PinNumber = PIN_0;
	ledF.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	ledF.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEEDR_FAST;
	ledF.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	ledF.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	GPIO_Config(&ledF);

	//Configuracion ledG
	ledG.pGPIOx = GPIOB;
	ledG.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	ledG.GPIO_PinConfig.GPIO_PinNumber = PIN_2;
	ledG.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	ledG.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEEDR_FAST;
	ledG.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	ledG.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	GPIO_Config(&ledG);

	//Configuración inicial del DispUni
	DispUni.pGPIOx = GPIOC;
	DispUni.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	DispUni.GPIO_PinConfig.GPIO_PinNumber = PIN_1;
	DispUni.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	DispUni.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEEDR_FAST;
	DispUni.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	DispUni.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	GPIO_Config(&DispUni);

	//Configuración inicial del DispDec
	DispDec.pGPIOx = GPIOA;
	DispDec.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	DispDec.GPIO_PinConfig.GPIO_PinNumber = PIN_0;
	DispDec.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	DispDec.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEEDR_FAST;
	DispDec.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	DispDec.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	GPIO_Config(&DispDec);

	//Configuracion del TIM4
	/*
	 * Timer encargado de hacer switch entre los transistores conectados a cada digito del 7 segmentos
	 */
	handlerTimerDisplay.ptrTIMx = TIM4;
	handlerTimerDisplay.TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
	handlerTimerDisplay.TIMx_Config.TIMx_speed = BTIMER_SPEED_100us;
	handlerTimerDisplay.TIMx_Config.TIMx_period = 100;
	handlerTimerDisplay.TIMx_Config.TIMx_interruptEnable = 1;
	BasicTimer_Config(&handlerTimerDisplay);

	//Configuracion Encoder
	/*
	 * Esta configurado de manera tal que usa el flanco de subida del data como la referencia
	 * para cambiar al siguiente valor
	 */
	handlerEncoder.pGPIOx = GPIOB;
	handlerEncoder.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	handlerEncoder.GPIO_PinConfig.GPIO_PinNumber = PIN_3;
	handlerEncoder.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerEncoder.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEEDR_FAST;
	handlerEncoder.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerEncoder.GPIO_PinConfig.GPIO_PinAltFunMode = AF2;

	extiConfigEncoder.pGPIOHandler= &handlerEncoder;
	extiConfigEncoder.edgeType = EXTERNAL_INTERRUPT_RISING_EDGE;
	extInt_Config(&extiConfigEncoder);

	//Configuracion Clk
	/*
	 *El clock me indica hacia que direccion esta girando el encoder, utiliza el valor del data y respecto a ese valor
	 *se sabe si es horario u antihorario
	 */
	handlerUserButton.pGPIOx = GPIOC;
	handlerUserButton.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	handlerUserButton.GPIO_PinConfig.GPIO_PinNumber = PIN_0;
	handlerUserButton.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerUserButton.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEEDR_FAST;
	handlerUserButton.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerUserButton.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	GPIO_Config(&handlerUserButton);

	//Configuracion AuxButton
	/*
	 * Boton usado para cambiar al modo gusanito
	*/
	handlerAuxButton.pGPIOx = GPIOC;
	handlerAuxButton.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	handlerAuxButton.GPIO_PinConfig.GPIO_PinNumber = PIN_4;
	handlerAuxButton.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerAuxButton.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEEDR_FAST;
	handlerAuxButton.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerAuxButton.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;

	extiConfigAuxButton.pGPIOHandler= &handlerAuxButton;
	extiConfigAuxButton.edgeType = EXTERNAL_INTERRUPT_RISING_EDGE;
	extInt_Config(&extiConfigAuxButton);


	GPIO_WritePin(&DispUni, SET);
	GPIO_WritePin(&DispDec, RESET);

	/* Como el 7 segmentos es anodo común, para apagar un Led se debe poner en SET y viceversa
	 * Se apagan todos los Leds
	 */
	GPIO_WritePin(&ledA, SET);
	GPIO_WritePin(&ledB, SET);
	GPIO_WritePin(&ledC, SET);
	GPIO_WritePin(&ledD, SET);
	GPIO_WritePin(&ledE, SET);
	GPIO_WritePin(&ledF, SET);
	GPIO_WritePin(&ledG, SET);

	contador = 0;

	//Ciclo del programa

    while (1){

    	}
    return 0;
    }

/*
 *El BasicTimer2_Callback se encanga de que el blinky permanezca encendido mientras el programa
 *se esté ejecutando
 */
void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerBlinky);
}

/*
 * EL BasicTimer4_Callback es el que está "pendiente" de que cuando se genere la interrupción
 * que corresponde al AuxButton
 */
void BasicTimer4_Callback(void){
	if(modeWorm == RESET){

		//Cada vez que interrumpe TIM4 se cambia el estado de los dos pines para hacer el switch
		GPIOxTooglePin(&DispUni);
		GPIOxTooglePin(&DispDec);
		switchDigit();
	}else{
		wormPath(wormCounter);
	}
}

/*
 * Si el modo gusanito está en RESET entonces el programa ejecuta la parte del
 * código que corresponda a la función de subir y bajar el contador del 0 al 99
 * de forma ascendente o descendente
 */
void callback_extInt3(void){
	uint32_t button = GPIO_ReadPin(&handlerUserButton);
	if (modeWorm == RESET){
		if(button == RESET){
			contador--;
		}else{
			contador++;
		}
		if (contador > 99){
			contador = 0;
		}
		if(contador < 0){
			contador = 99;
		}
	}else{
		if(button == RESET){
			wormCounter--;
		}else{
			wormCounter++;
		}
		if (wormCounter > 12){
			wormCounter = 0;
		}
		if(wormCounter < 0){
			wormCounter = 12;
		}
	}
}

/*
 * Interrupción que activa el modo gusanito
 */
void callback_extInt4(void){
	modeWorm = !modeWorm;
}

/*
 * Función que indica qué debe mostrar cada uno de los displays de las unidades y las decenas
 */
void switchDigit(void){
	uint32_t left = GPIO_ReadPin(&DispUni);
	if(left == SET){
		displayWrite(contador/10);
	}
	uint32_t right = GPIO_ReadPin(&DispDec);
	if(right == SET){
		displayWrite(contador%10);
	}
}

/*
 * Función que determina cuales Leds deben estar encendidos dependiendo del número que se
 * encuentre guardado en ese momento
 */
void displayWrite(uint8_t number){

	switch (number){
		case 0:{
			GPIO_WritePin(&ledA, RESET);
			GPIO_WritePin(&ledB, RESET);
			GPIO_WritePin(&ledC, RESET);
			GPIO_WritePin(&ledD, RESET);
			GPIO_WritePin(&ledE, RESET);
			GPIO_WritePin(&ledF, RESET);
			GPIO_WritePin(&ledG, SET);
			break;
		}
		case 1:{
			GPIO_WritePin(&ledA, SET);
			GPIO_WritePin(&ledB, RESET);
			GPIO_WritePin(&ledC, RESET);
			GPIO_WritePin(&ledD, SET);
			GPIO_WritePin(&ledE, SET);
			GPIO_WritePin(&ledF, SET);
			GPIO_WritePin(&ledG, SET);
			break;
		}
		case 2:{
			GPIO_WritePin(&ledA, RESET);
			GPIO_WritePin(&ledB, RESET);
			GPIO_WritePin(&ledC, SET);
			GPIO_WritePin(&ledD, RESET);
			GPIO_WritePin(&ledE, RESET);
			GPIO_WritePin(&ledF, SET);
			GPIO_WritePin(&ledG, RESET);
			break;
		}

		case 3:{
			GPIO_WritePin(&ledA, RESET);
			GPIO_WritePin(&ledB, RESET);
			GPIO_WritePin(&ledC, RESET);
			GPIO_WritePin(&ledD, RESET);
			GPIO_WritePin(&ledE, SET);
			GPIO_WritePin(&ledF, SET);
			GPIO_WritePin(&ledG, RESET);
			break;
		}
		case 4:{
			GPIO_WritePin(&ledA, SET);
			GPIO_WritePin(&ledB, RESET);
			GPIO_WritePin(&ledC, RESET);
			GPIO_WritePin(&ledD, SET);
			GPIO_WritePin(&ledE, SET);
			GPIO_WritePin(&ledF, RESET);
			GPIO_WritePin(&ledG, RESET);

			break;
		}
		case 5:{
			GPIO_WritePin(&ledA, RESET);
			GPIO_WritePin(&ledB, SET);
			GPIO_WritePin(&ledC, RESET);
			GPIO_WritePin(&ledD, RESET);
			GPIO_WritePin(&ledE, SET);
			GPIO_WritePin(&ledF, RESET);
			GPIO_WritePin(&ledG, RESET);
			break;
		}
		case 6:{
			GPIO_WritePin(&ledA, RESET);
			GPIO_WritePin(&ledB, SET);
			GPIO_WritePin(&ledC, RESET);
			GPIO_WritePin(&ledD, RESET);
			GPIO_WritePin(&ledE, RESET);
			GPIO_WritePin(&ledF, RESET);
			GPIO_WritePin(&ledG, RESET);
			break;
		}

		case 7:{
			GPIO_WritePin(&ledA, RESET);
			GPIO_WritePin(&ledB, RESET);
			GPIO_WritePin(&ledC, RESET);
			GPIO_WritePin(&ledD, SET);
			GPIO_WritePin(&ledE, SET);
			GPIO_WritePin(&ledF, SET);
			GPIO_WritePin(&ledG, SET);
			break;
		}

		case 8:{
			GPIO_WritePin(&ledA, RESET);
			GPIO_WritePin(&ledB, RESET);
			GPIO_WritePin(&ledC, RESET);
			GPIO_WritePin(&ledD, RESET);
			GPIO_WritePin(&ledE, RESET);
			GPIO_WritePin(&ledF, RESET);
			GPIO_WritePin(&ledG, RESET);
			break;
		}
		case 9:{
			GPIO_WritePin(&ledA, RESET);
			GPIO_WritePin(&ledB, RESET);
			GPIO_WritePin(&ledC, RESET);
			GPIO_WritePin(&ledD, RESET);
			GPIO_WritePin(&ledE, SET);
			GPIO_WritePin(&ledF, RESET);
			GPIO_WritePin(&ledG, RESET);
			break;
		}
		default:{
			GPIO_WritePin(&ledA, SET);
			GPIO_WritePin(&ledB, SET);
			GPIO_WritePin(&ledC, SET);
			GPIO_WritePin(&ledD, SET);
			GPIO_WritePin(&ledE, SET);
			GPIO_WritePin(&ledF, SET);
			GPIO_WritePin(&ledG, SET);
			break;

		}


	}

}

/*
 * Función que determina cuales Leds deben estar encendidos dependiendo de la posición
 * en la que se encuentre el gusanito en este momento.
 */
void wormPath(uint8_t number){

	switch (number){
		case 0:{
			GPIO_WritePin(&DispDec, SET);
			GPIO_WritePin(&DispUni, RESET);
			GPIO_WritePin(&ledA, RESET);
			GPIO_WritePin(&ledB, SET);
			GPIO_WritePin(&ledC, SET);
			GPIO_WritePin(&ledD, SET);
			GPIO_WritePin(&ledE, SET);
			GPIO_WritePin(&ledF, SET);
			GPIO_WritePin(&ledG, SET);
			break;
		}
		case 1:{
			GPIO_WritePin(&DispDec, RESET);
			GPIO_WritePin(&DispUni, SET);
			GPIO_WritePin(&ledA, RESET);
			GPIO_WritePin(&ledB, SET);
			GPIO_WritePin(&ledC, SET);
			GPIO_WritePin(&ledD, SET);
			GPIO_WritePin(&ledE, SET);
			GPIO_WritePin(&ledF, SET);
			GPIO_WritePin(&ledG, SET);
			break;
		}
		case 2:{
			GPIO_WritePin(&DispDec, RESET);
			GPIO_WritePin(&DispUni, SET);
			GPIO_WritePin(&ledA, SET);
			GPIO_WritePin(&ledB, SET);
			GPIO_WritePin(&ledC, SET);
			GPIO_WritePin(&ledD, SET);
			GPIO_WritePin(&ledE, SET);
			GPIO_WritePin(&ledF, RESET);
			GPIO_WritePin(&ledG, SET);
			break;
		}

		case 3:{
			GPIO_WritePin(&DispDec, RESET);
			GPIO_WritePin(&DispUni, SET);
			GPIO_WritePin(&ledA, SET);
			GPIO_WritePin(&ledB, SET);
			GPIO_WritePin(&ledC, SET);
			GPIO_WritePin(&ledD, SET);
			GPIO_WritePin(&ledE, RESET);
			GPIO_WritePin(&ledF, SET);
			GPIO_WritePin(&ledG, SET);
			break;
		}
		case 4:{
			GPIO_WritePin(&DispDec, RESET);
			GPIO_WritePin(&DispUni, SET);
			GPIO_WritePin(&ledA, SET);
			GPIO_WritePin(&ledB, SET);
			GPIO_WritePin(&ledC, SET);
			GPIO_WritePin(&ledD, RESET);
			GPIO_WritePin(&ledE, SET);
			GPIO_WritePin(&ledF, SET);
			GPIO_WritePin(&ledG, SET);

			break;
		}
		case 5:{
			GPIO_WritePin(&DispDec, SET);
			GPIO_WritePin(&DispUni, RESET);
			GPIO_WritePin(&ledA, SET);
			GPIO_WritePin(&ledB, SET);
			GPIO_WritePin(&ledC, SET);
			GPIO_WritePin(&ledD, SET);
			GPIO_WritePin(&ledE, RESET);
			GPIO_WritePin(&ledF, SET);
			GPIO_WritePin(&ledG, SET);
			break;
		}
		case 6:{
			GPIO_WritePin(&DispDec, SET);
			GPIO_WritePin(&DispUni, RESET);
			GPIO_WritePin(&ledA, SET);
			GPIO_WritePin(&ledB, SET);
			GPIO_WritePin(&ledC, SET);
			GPIO_WritePin(&ledD, SET);
			GPIO_WritePin(&ledE, SET);
			GPIO_WritePin(&ledF, RESET);
			GPIO_WritePin(&ledG, SET);
			break;
		}

		case 7:{
			GPIO_WritePin(&DispDec, RESET);
			GPIO_WritePin(&DispUni, SET);
			GPIO_WritePin(&ledA, SET);
			GPIO_WritePin(&ledB, SET);
			GPIO_WritePin(&ledC, RESET);
			GPIO_WritePin(&ledD, SET);
			GPIO_WritePin(&ledE, SET);
			GPIO_WritePin(&ledF, SET);
			GPIO_WritePin(&ledG, SET);
			break;
		}

		case 8:{
			GPIO_WritePin(&DispDec, RESET);
			GPIO_WritePin(&DispUni, SET);
			GPIO_WritePin(&ledA, SET);
			GPIO_WritePin(&ledB, RESET);
			GPIO_WritePin(&ledC, SET);
			GPIO_WritePin(&ledD, SET);
			GPIO_WritePin(&ledE, SET);
			GPIO_WritePin(&ledF, SET);
			GPIO_WritePin(&ledG, SET);
			break;
		}
		case 9:{
			GPIO_WritePin(&DispDec, SET);
			GPIO_WritePin(&DispUni, RESET);
			GPIO_WritePin(&ledA, SET);
			GPIO_WritePin(&ledB, RESET);
			GPIO_WritePin(&ledC, SET);
			GPIO_WritePin(&ledD, SET);
			GPIO_WritePin(&ledE, SET);
			GPIO_WritePin(&ledF, SET);
			GPIO_WritePin(&ledG, SET);
			break;
		}
		case 10:{
			GPIO_WritePin(&DispDec, SET);
			GPIO_WritePin(&DispUni, RESET);
			GPIO_WritePin(&ledA, SET);
			GPIO_WritePin(&ledB, SET);
			GPIO_WritePin(&ledC, SET);
			GPIO_WritePin(&ledD, RESET);
			GPIO_WritePin(&ledE, SET);
			GPIO_WritePin(&ledF, SET);
			GPIO_WritePin(&ledG, SET);
			break;
		}
		case 11:{
			GPIO_WritePin(&DispDec, SET);
			GPIO_WritePin(&DispUni, RESET);
			GPIO_WritePin(&ledA, SET);
			GPIO_WritePin(&ledB, SET);
			GPIO_WritePin(&ledC, RESET);
			GPIO_WritePin(&ledD, SET);
			GPIO_WritePin(&ledE, SET);
			GPIO_WritePin(&ledF, SET);
			GPIO_WritePin(&ledG, SET);
			break;
		}
		case 12:{
			GPIO_WritePin(&DispDec, SET);
			GPIO_WritePin(&DispUni, RESET);
			GPIO_WritePin(&ledA, SET);
			GPIO_WritePin(&ledB, RESET);
			GPIO_WritePin(&ledC, SET);
			GPIO_WritePin(&ledD, SET);
			GPIO_WritePin(&ledE, SET);
			GPIO_WritePin(&ledF, SET);
			GPIO_WritePin(&ledG, SET);
			break;
		}
		default:{

			GPIO_WritePin(&ledA, SET);
			GPIO_WritePin(&ledB, SET);
			GPIO_WritePin(&ledC, SET);
			GPIO_WritePin(&ledD, SET);
			GPIO_WritePin(&ledE, SET);
			GPIO_WritePin(&ledF, SET);
			GPIO_WritePin(&ledG, SET);
			break;

		}


	}

}
