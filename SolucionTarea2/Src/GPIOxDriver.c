#include "GPIOxDriver.h"

void GPIO_Config (GPIO_Handler_t *pGPIOHandler){

	uint32_t auxConfig = 0;
	uint32_t auxPosition = 0;

	if (pGPIOHandler->pGPIOx == GPIOA){

		RCC->AHB1ENR |=(SET <<  RCC_AHB1ENR_GPIOA_EN);
	}

	else if (pGPIOHandler->pGPIOx == GPIOB){

		RCC->AHB1ENR |= (SET << RCC_AHB1ENR_GPIOB_EN);
	}
	else if (pGPIOHandler->pGPIOx == GPIOC){

		RCC->AHB1ENR |= (SET << RCC_AHB1ENR_GPIOC_EN);
	}
	else if (pGPIOHandler->pGPIOx == GPIOD){

		RCC->AHB1ENR |= (SET << RCC_AHB1ENR_GPIOD_EN);
	}
	else if (pGPIOHandler->pGPIOx == GPIOE){

		RCC->AHB1ENR |= (SET << RCC_AHB1ENR_GPIOE_EN);
	}
	else if (pGPIOHandler->pGPIOx ==GPIOH){

		RCC->AHB1ENR |= (SET << RCC_AHB1ENR_GPIOH_EN);
	}

	auxConfig = (pGPIOHandler->GPIO_PinConfig.GPIO_PinMode << 2 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);

	pGPIOHandler->pGPIOx->MODER &= ~(0b11 << 2 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);

	pGPIOHandler->pGPIOx->MODER |= auxConfig;

	auxConfig = (pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType << pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);

	pGPIOHandler->pGPIOx->OTYPER &= ~(SET << pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);

	pGPIOHandler->pGPIOx->OTYPER |= auxConfig;

	if (pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_ALTFN){

		if (pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber < 8){

			auxPosition = 4 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber;

			pGPIOHandler->pGPIOx->AFRL &= ~(0b1111 << auxPosition);

			pGPIOHandler->pGPIOx->AFRL |= (pGPIOHandler->GPIO_PinConfig.GPIO_PinAltFunMode << auxPosition);
		}
		else{

			auxPosition = 4 * (pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber -8);

			pGPIOHandler->pGPIOx->AFRH &= ~(0b1111 << auxPosition);

			pGPIOHandler->pGPIOx->AFRH |= (pGPIOHandler->GPIO_PinConfig.GPIO_PinAltFunMode << auxPosition);

		}
	}
}

void GPIO_WritePin (GPIO_Handler_t *pPinHandler, uint8_t newState){

	if (newState == SET){

		pPinHandler->pGPIOx->BSRR |= (SET << pPinHandler->GPIO_PinConfig.GPIO_PinNumber);
	}
	else{

		pPinHandler->pGPIOx->BSRR |= (SET << (pPinHandler->GPIO_PinConfig.GPIO_PinNumber + 16));
	}
}

uint32_t GPIO_ReadPin(GPIO_Handler_t *pPinHandler){
	uint32_t pinValue = 0;

	// pinValue = (pPinHandler->pGPIOx->IDR << pPinHandler->GPIO_PinConfig.GPIO_PinNumber);		Línea a Corregir
	pinValue = (pPinHandler->pGPIOx->IDR >> pPinHandler->GPIO_PinConfig.GPIO_PinNumber) & SET;	//Línea Corregida

	return pinValue;

}

/* Primer punto.
	a. La variable pinValue está retornando el valor que hay en el registro IDR y le está realizando un Shift Register hacia la izquierda una cantidad de 
	   posiciones igual a GPIO_PinNumber del GPIO_PinConfig. 
	b. La solución debería consistir en que la variable pinValue tome el valor que corresponde a lo que hay en el registro IDR del pin que se quiera leer, es decir,
	   pinValue es igual a 0 si el pin que se está usando está apagado e igual a 1 si el pin está encendido. 
/*
