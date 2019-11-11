
/*
 * Author: Aurelio Colosimo, 2019
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

#ifndef _REG_H_
#define _REG_H_

#include <basic.h>

#ifdef SOC_VARIANT_stm32f407vg
#define STACK_TOP ((void*)(0x20020000))
#else
#error Unhandled SOC_VARIANT: $(SOC_VARIANT)
#endif

/* RCC registers */
#define R_RCC_CR       reg32(0x40023800)
#define R_RCC_PLLCFGR  reg32(0x40023804)
#define R_RCC_CFGR     reg32(0x40023808)
#define R_RCC_CIR      reg32(0x4002380C)
#define R_RCC_AHB1RSTR reg32(0x40023810)
#define R_RCC_AHB2RSTR reg32(0x40023814)
#define R_RCC_AHB3RSTR reg32(0x40023818)
#define R_RCC_APB1RSTR reg32(0x40023820)
#define R_RCC_APB2RSTR reg32(0x40023824)
#define R_RCC_AHB1ENR  reg32(0x40023830)
#define R_RCC_AHB2ENR  reg32(0x40023834)
#define R_RCC_AHB3ENR  reg32(0x40023838)
#define R_RCC_APB1ENR  reg32(0x40023840)
#define R_RCC_APB2ENR  reg32(0x40023844)
#define R_RCC_AHB1LPENR reg32(0x40023850)
#define R_RCC_AHB2LPENR reg32(0x40023854)
#define R_RCC_AHB3LPENR reg32(0x40023858)
#define R_RCC_APB1LPENR reg32(0x40023860)
#define R_RCC_APB2LPENR reg32(0x40023864)
#define R_RCC_BDCR     reg32(0x40023870)
#define R_RCC_CSR      reg32(0x40023874)
#define R_RCC_SSCGR    reg32(0x40023880)
#define R_RCC_PLLI2SCFGR reg32(0x40023884)

/* Flash interface registers */
#define R_FLASH_ACR     reg32(0x40023C00)
#define R_FLASH_KEYR    reg32(0x40023C04)
#define R_FLASH_OPTKEYR reg32(0x40023C08)
#define R_FLASH_SR      reg32(0x40023C0c)
#define R_FLASH_CR      reg32(0x40023C10)
#define R_FLASH_OPTCR   reg32(0x40023C14)

/* USART1 registers */
#define R_USART1_SR     reg32(0x40011000)
#define R_USART1_DR     reg32(0x40011004)
#define R_USART1_BRR    reg32(0x40011008)
#define R_USART1_CR1    reg32(0x4001100C)
#define R_USART1_CR2    reg32(0x40011010)
#define R_USART1_CR3    reg32(0x40011014)
#define R_USART1_GTPR   reg32(0x40011018)

/* USART2 registers */
#define R_USART2_SR     reg32(0x40004400)
#define R_USART2_DR     reg32(0x40004404)
#define R_USART2_BRR    reg32(0x40004408)
#define R_USART2_CR1    reg32(0x4000440C)
#define R_USART2_CR2    reg32(0x40004410)
#define R_USART2_CR3    reg32(0x40004414)
#define R_USART2_GTPR   reg32(0x40004418)

/* USART3 registers */
#define R_USART3_SR     reg32(0x40004800)
#define R_USART3_DR     reg32(0x40004804)
#define R_USART3_BRR    reg32(0x40004808)
#define R_USART3_CR1    reg32(0x4000480C)
#define R_USART3_CR2    reg32(0x40004810)
#define R_USART3_CR3    reg32(0x40004814)
#define R_USART3_GTPR   reg32(0x40004818)

/* GPIO registers */
#define R_GPIOA_MODER   reg32(0x40020000)
#define R_GPIOA_OTYPER  reg32(0x40020004)
#define R_GPIOA_OSPEEDR reg32(0x40020008)
#define R_GPIOA_PUPDR   reg32(0x4002000C)
#define R_GPIOA_IDR     reg32(0x40020010)
#define R_GPIOA_ODR     reg32(0x40020014)
#define R_GPIOA_BSRR    reg32(0x40020018)
#define R_GPIOA_LCKR    reg32(0x4002001c)
#define R_GPIOA_AFRL    reg32(0x40020020)
#define R_GPIOA_AFRH    reg32(0x40020024)

#endif /* _REG_H_ */
