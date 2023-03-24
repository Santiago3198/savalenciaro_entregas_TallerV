/*
 * BasicTimer.h
 *
 *  Created on: 23/03/2023
 *      Author: if420_01
 */

#ifndef INC_BASICTIMER_H_
#define INC_BASICTIMER_H_

#include "stm32f4xx.h"

#define BTIMER_MODE_UP				0
#define BTIMER_MODE_DOWN			1

#define BTIMER_SPEED_10us			160
#define BTIMER_SPEED_100us			1600
#define BTIMER_SPEED_16000us		16000

/*Estructura que contiene la configutacion minima necesaria para el manejo del timer*/

typedef struct
{
	uint8_t TIMx_mode;				//Up or Down
	uint32_t TIMx_speed;			//A qué velocidad se incrementa el timer
	uint32_t TIMx_period;			//Valor en ms del periodo del timer
	uint8_t	TIMx_interruptEnable;	//Activa o desactiva el modo interrupcion del timer

}BasicTimer_Config_t;

/*Handler para el timer*/

typedef struct
{
	TIM_Typedef		*ptrTIMx;
	BasicTimer_Config_t	TIMx_Config;

}BasicTimer_Handler_t;

void BasicTmer_Config(BasicTimer_Handler_t *ptrBTimerHandler);
void BasicTimerX_Callback(void); 		//Esta funcion debe ser sobreescrita en el main para que el sistema funcione



#endif /* BASICTIMER_H_ */
