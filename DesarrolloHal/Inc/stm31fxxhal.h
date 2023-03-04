/*
 * stm31fxxhal.h
 *
 *  Created on: Mar 2, 2023
 *      Author: if420_01
 */

#ifndef STM31FXXHAL_H_
#define STM31FXXHAL_H_

#include <stdint.h>
#include <stddef.h>

#define HSI_CLOCK_SPEED  16000000
#define HSE_CLOCK_SPEED  4000000

#define NOP() asm("NOP")
#define __weak	__attribute__((weak))

#define FLASH_BASE_ADDR		0x08000000U
#define SRAM_BASE_ADDR		0x20000000U

#define APB1_BASE_ADDR		0x40000000U
#define APB2_BASE_ADDR		0x40010000U
#define AHB1_BASE_ADDR		0x40020000U
#define AHB2_BASE_ADDR		0x50000000U

#define USB_OTG_FS_BASE_ADDR		(AHB2_BASE_ADDR + 0x0000U)

#define RCC_BASE_ADDR		(AHB1_BASE_ADDR + 0x3800U)
#define GPIOH_BASE_ADDR		(AHB1_BASE_ADDR + 0x1C00U)
#define GPIOE_BASE_ADDR		(AHB1_BASE_ADDR + 0x1000U)
#define GPIOD_BASE_ADDR		(AHB1_BASE_ADDR + 0x0C00U)
#define GPIOC_BASE_ADDR		(AHB1_BASE_ADDR + 0x0800U)
#define GPIOB_BASE_ADDR		(AHB1_BASE_ADDR + 0x0400U)
#define GPIOA_BASE_ADDR		(AHB1_BASE_ADDR + 0x0000U)


#define ENABLE			1
#define DISABLE			0
#define SET				ENABLE
#define CLEAR			DISABLE
#define RESET			DISABLE
#define GPIO_PIN_SET	SET
#define GPIO_PIN_RESET	RESET
#define FLAG_SET		SET	
#define FLAG_RESET		RESET
#define I2C_WIRE		0
#define I2C_READ		1

typedef struct
{
	volatile uint32_t CR;
	volatile uint32_t PLLCFGR;
	volatile uint32_t CFGR;
	volatile uint32_t CIR;
	volatile uint32_t AHB1RSTR;
	volatile uint32_t AHB2RSTR;
	volatile uint32_t reserved0;
	volatile uint32_t reserved1;
	volatile uint32_t APB1RSTR;
	volatile uint32_t APB2RSTR;
	volatile uint32_t reserved2;
	volatile uint32_t reserved3;
	volatile uint32_t AHB1ENR;
	volatile uint32_t AHB2ENR;
	volatile uint32_t reserved4;
	volatile uint32_t reserved5;
	volatile uint32_t APB1ENR;
	volatile uint32_t APB2ENR;
	volatile uint32_t reserved6;
	volatile uint32_t reserved7;
	volatile uint32_t AHB1LPENR;
	volatile uint32_t AHB2LPENR;
	volatile uint32_t reserved8;
	volatile uint32_t reserved9;
	volatile uint32_t APB1LPENR;
	volatile uint32_t APB2LPENR;
	volatile uint32_t reserved10;
	volatile uint32_t reserved11;
	volatile uint32_t BDCR;
	volatile uint32_t CSR;
	volatile uint32_t reserved12;
	volatile uint32_t reserved13;
	volatile uint32_t SSCGR;
	volatile uint32_t PLLI2SCFGR;
	volatile uint32_t reserved14;
	volatile uint32_t DCKCFGR;
} RCC RedDef t;        //¿Por que no cambia de color a azul?.
//Continuar aqui!!!
#endif /* STM31FXXHAL_H_ */
