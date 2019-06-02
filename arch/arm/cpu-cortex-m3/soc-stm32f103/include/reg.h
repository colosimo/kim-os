/*
 * Author: Aurelio Colosimo, 2019
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

#ifndef _REG_H_
#define _REG_H_

#define SOC_STM32F103

/* RCC registers */
#define R_RCC_CR       reg32(0x40021000)
#define R_RCC_CFGR     reg32(0x40021004)
#define R_RCC_CIR      reg32(0x40021008)
#define R_RCC_APB2RSTR reg32(0x4002100C)
#define R_RCC_APB1RSTR reg32(0x40021010)
#define R_RCC_AHBENR   reg32(0x40021014)
#define R_RCC_APB2ENR  reg32(0x40021018)
#define R_RCC_APB1ENR  reg32(0x4002101C)
#define R_RCC_BDCR     reg32(0x40021020)

/* Flash interface registers */
#define R_FLASH_ACR     reg32(0x40022000)
#define R_FLASH_KEYR    reg32(0x40022004)
#define R_FLASH_OPTKEYR reg32(0x40022008)
#define R_FLASH_SR      reg32(0x4002200c)
#define R_FLASH_CR      reg32(0x40022010)
#define R_FLASH_AR      reg32(0x40022014)
#define R_FLASH_OBR     reg32(0x4002201C)
#define R_FLASH_WRPR    reg32(0x40022020)

/* USART1 registers */
#define R_USART1_SR     reg32(0x40013800)
#define R_USART1_DR     reg32(0x40013804)
#define R_USART1_BRR    reg32(0x40013808)
#define R_USART1_CR1    reg32(0x4001380C)
#define R_USART1_CR2    reg32(0x40013810)
#define R_USART1_CR3    reg32(0x40013814)
#define R_USART1_GTPR   reg32(0x40013818)

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
#define R_GPIOA_CRL     reg32(0x40010800)
#define R_GPIOA_CRH     reg32(0x40010804)
#define R_GPIOA_IDR     reg32(0x40010808)
#define R_GPIOA_ODR     reg32(0x4001080c)
#define R_GPIOA_BSRR    reg32(0x40010810)
#define R_GPIOA_BRR     reg32(0x40010814)
#define R_GPIOA_LCKR    reg32(0x40010818)

#endif /* _REG_H_ */
