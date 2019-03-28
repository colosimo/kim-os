/*
 * Author: Aurelio Colosimo, 2019
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

#ifndef _REG_H_
#define _REG_H_

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
#define R_RCC_CSR      reg32(0x40021024)
#define R_RCC_AHBRSTR  reg32(0x40021028)
#define R_RCC_CFGR2    reg32(0x4002102C)
#define R_RCC_CFGR3    reg32(0x40021030)
#define R_RCC_CR2      reg32(0x40021034)

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
#define R_USART1_CR1    reg32(0x40013800)
#define R_USART1_CR2    reg32(0x40013804)
#define R_USART1_CR3    reg32(0x40013808)
#define R_USART1_BRR    reg32(0x4001380C)
#define R_USART1_RTOR   reg32(0x40013814)
#define R_USART1_RQR    reg32(0x40013818)
#define R_USART1_ISR    reg32(0x4001381C)
#define R_USART1_ICR    reg32(0x40013820)
#define R_USART1_RDR    reg32(0x40013824)
#define R_USART1_TDR    reg32(0x40013828)

/* USART2 registers */
#define R_USART2_CR1    reg32(0x40004400)
#define R_USART2_CR2    reg32(0x40004404)
#define R_USART2_CR3    reg32(0x40004408)
#define R_USART2_BRR    reg32(0x4000440C)
#define R_USART2_RTOR   reg32(0x40004414)
#define R_USART2_RQR    reg32(0x40004418)
#define R_USART2_ISR    reg32(0x4000441C)
#define R_USART2_ICR    reg32(0x40004420)
#define R_USART2_RDR    reg32(0x40004424)
#define R_USART2_TDR    reg32(0x40004428)

/* GPIO registers */
#define R_GPIOA_MODER   reg32(0x48000000)
#define R_GPIOA_OTYPER  reg32(0x48000004)
#define R_GPIOA_OSPEEDR reg32(0x48000008)
#define R_GPIOA_PUPDR   reg32(0x4800000c)
#define R_GPIOA_IDR     reg32(0x48000010)
#define R_GPIOA_ODR     reg32(0x48000014)
#define R_GPIOA_BSRR    reg32(0x48000018)
#define R_GPIOA_LCKR    reg32(0x4800001c)
#define R_GPIOA_AFRL    reg32(0x48000020)
#define R_GPIOA_AFRH    reg32(0x48000024)

#endif /* _REG_H_ */
