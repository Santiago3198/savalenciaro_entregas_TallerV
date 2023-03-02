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
//CONTINUAR AQUÍ

#define ENABLE		1
#define DISABLE		0
#define SET			ENABLE
#define CLEAR		DISABLE
#define RESET		DISABLE


#endif /* STM31FXXHAL_H_ */
