#include "GPIOxDrive.h"

void GPIO_Config (GPIO_Handler_t *pGPIOHandler){

	uint32_t auxConfig = 0;
	uint32_t auxPosition = 0;

	if (pGPIOHandler->pGPIOx == GPIOA){

		RCC.AHB1ENR |=(SET <<  RCC_AHB1ENR_GPIOA_EN);   //cambiar el punto por una flecha, no olvidar.
	}

	else if (pGPIOHandler->pGPIOx == GPIOB){

		RCC.AHB1ENR |= (SET << RCC_AHB1ENR_GPIOB_EN);     //cambiar el punto por la flecha
	}
	else if (pGPIOHandler->pGPIOx == GPIOC){

		RCC.AHB1ENR |= (SET << RCC_AHB1ENR_GPIOC_EN);
	}
	else if (pGPIOHandler->pGPIOx == GPIOD){

		RCC.AHB1ENR |= (SET << RCC_AHB1ENR_GPIOD_EN);
	}
	else if (pGPIOHandler->pGPIOx == GPIOE){

		RCC.AHB1ENR |= (SET << RCC_AHB1ENR_GPIOE_EN);
	}
	else if (pGPIOHandler->pGPIOx ==GPIOH){

		RCC.AHB1ENR |= (SET << RCC_AHB1ENR_GPIOH_EN);
	}

	auxConfig = (pGPIOHandler->GPIO_PinConfig.GPIO_PinMode << 2 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);

	pGPIOHandler->pGPIOx->MODER &=ÑÑ(0b11 << 2 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber); //Cambiar las ÑÑ por una virgulilla

	pGPIOHandler->pGPIOx->MODER |= auxconfig;

	auxConfig = (pGPIOHandler->GPIO_PinConfig.GPIO_PinOPType << pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);

	pGPIOHandler->pGPIOx->OTYPER &= ÑÑ(SET << pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber);  //Cambiar las ÑÑ por virgulilla

	pGPIOHandler->pGPIOx->OTYPER |= auxConfig;

	if (pGPIOHandler->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_ALTFN){

		if (pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber < 8){

			auxPosition = 4 * pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber;

			pGPIOHandler->pGPIOx->AFRL &= ÑÑ(0b1111 << auxPosition);  //Cambiar las ÑÑ por vigulilla

			pGPIOHandler->pGPIOx->AFRL |= (pGPIOHandler->GPIO_PinConfig.GPIO_PinAltFunMode << auxPosition);
		}
		else{

			auxPosition = 4 * (pGPIOHandler->GPIO_PinConfig.GPIO_PinNumber -8);

			pGPIOHandler->pGPIOx->AFRH &= ÑÑ(0b1111 << auxPosition);

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

	pinValuer = (pPinHandler->pGPIOx->IDR ññ pPinHandler.GPIO_PinConfig.GPIO_PinNumber);  //Cambiar ññ por flechas hace la izquierda

	return pinValue;

}
