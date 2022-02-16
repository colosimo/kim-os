
/*
 * Author: Aurelio Colosimo, 2022
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

#ifndef _REG_H_
#define _REG_H_

#include <basic.h>

#define STACK_TOP ((void*)(0x20004000))

/* SPI1 registers */
#define R_SPI1_CR1         reg32(0x40013000)
#define R_SPI1_CR2         reg32(0x40013004)
#define R_SPI1_SR          reg32(0x40013008)
#define R_SPI1_DR          reg8 (0x4001300c)
#define R_SPI1_CRCPR       reg32(0x40013010)
#define R_SPI1_RXCRCR      reg32(0x40013014)
#define R_SPI1_TXCRCR      reg32(0x40013018)
#define R_SPI1_I2SCFGR     reg32(0x4001301c)
#define R_SPI1_I2SPR       reg32(0x40013020)

/* SPI2 registers */
#define R_SPI2_CR1         reg32(0x40003800)
#define R_SPI2_CR2         reg32(0x40003804)
#define R_SPI2_SR          reg32(0x40003808)
#define R_SPI2_DR          reg8 (0x4000380c)
#define R_SPI2_CRCPR       reg32(0x40003810)
#define R_SPI2_RXCRCR      reg32(0x40003814)
#define R_SPI2_TXCRCR      reg32(0x40003818)
#define R_SPI2_I2SCFGR     reg32(0x4000381c)
#define R_SPI2_I2SPR       reg32(0x40003820)

/* SPI3 registers */
#define R_SPI3_CR1         reg32(0x40003c00)
#define R_SPI3_CR2         reg32(0x40003c04)
#define R_SPI3_SR          reg32(0x40003c08)
#define R_SPI3_DR          reg8 (0x40003c0c)
#define R_SPI3_CRCPR       reg32(0x40003c10)
#define R_SPI3_RXCRCR      reg32(0x40003c14)
#define R_SPI3_TXCRCR      reg32(0x40003c18)
#define R_SPI3_I2SCFGR     reg32(0x40003c1c)
#define R_SPI3_I2SPR       reg32(0x40003c20)

/* RCC registers */
#define R_RCC_CR       reg32(0x40023800)
#define R_RCC_PLLCFGR  reg32(0x40023804)
#define R_RCC_CFGR     reg32(0x40023808)
#define R_RCC_CIR      reg32(0x4002380C)
#define R_RCC_AHB1RSTR reg32(0x40023810)
#define R_RCC_APB1RSTR reg32(0x40023820)
#define R_RCC_APB2RSTR reg32(0x40023824)
#define R_RCC_AHB1ENR  reg32(0x40023830)
#define R_RCC_APB1ENR  reg32(0x40023840)
#define R_RCC_APB2ENR  reg32(0x40023844)
#define R_RCC_AHB1LPENR reg32(0x40023850)
#define R_RCC_APB1LPENR reg32(0x40023860)
#define R_RCC_APB2LPENR reg32(0x40023864)
#define R_RCC_BDCR     reg32(0x40023870)
#define R_RCC_CSR      reg32(0x40023874)
#define R_RCC_SSCGR    reg32(0x40023880)
#define R_RCC_DCKCFGR  reg32(0x4002388c)
#define R_RCC_DCKCFGR2 reg32(0x40023894)

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

/* USART6 registers */
#define R_USART6_SR     reg32(0x40011400)
#define R_USART6_DR     reg32(0x40011404)
#define R_USART6_BRR    reg32(0x40011408)
#define R_USART6_CR1    reg32(0x4001140C)
#define R_USART6_CR2    reg32(0x40011410)
#define R_USART6_CR3    reg32(0x40011414)
#define R_USART6_GTPR   reg32(0x40011418)

/* GPIOA registers */
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

/* GPIOB registers */
#define R_GPIOB_MODER   reg32(0x40020400)
#define R_GPIOB_OTYPER  reg32(0x40020404)
#define R_GPIOB_OSPEEDR reg32(0x40020408)
#define R_GPIOB_PUPDR   reg32(0x4002040C)
#define R_GPIOB_IDR     reg32(0x40020410)
#define R_GPIOB_ODR     reg32(0x40020414)
#define R_GPIOB_BSRR    reg32(0x40020418)
#define R_GPIOB_LCKR    reg32(0x4002041c)
#define R_GPIOB_AFRL    reg32(0x40020420)
#define R_GPIOB_AFRH    reg32(0x40020424)

/* GPIOC registers */
#define R_GPIOC_MODER   reg32(0x40020800)
#define R_GPIOC_OTYPER  reg32(0x40020804)
#define R_GPIOC_OSPEEDR reg32(0x40020808)
#define R_GPIOC_PUPDR   reg32(0x4002080C)
#define R_GPIOC_IDR     reg32(0x40020810)
#define R_GPIOC_ODR     reg32(0x40020814)
#define R_GPIOC_BSRR    reg32(0x40020818)
#define R_GPIOC_LCKR    reg32(0x4002081c)
#define R_GPIOC_AFRL    reg32(0x40020820)
#define R_GPIOC_AFRH    reg32(0x40020824)

/* GPIOH registers */
#define R_GPIOH_MODER   reg32(0x40021c00)
#define R_GPIOH_OTYPER  reg32(0x40021c04)
#define R_GPIOH_OSPEEDR reg32(0x40021c08)
#define R_GPIOH_PUPDR   reg32(0x40021c0C)
#define R_GPIOH_IDR     reg32(0x40021c10)
#define R_GPIOH_ODR     reg32(0x40021c14)
#define R_GPIOH_BSRR    reg32(0x40021c18)
#define R_GPIOH_LCKR    reg32(0x40021c1c)
#define R_GPIOH_AFRL    reg32(0x40021c20)
#define R_GPIOH_AFRH    reg32(0x40021c24)

/* I2C1 registers */
#define R_I2C1_CR1         reg32(0x40005400)
#define R_I2C1_CR2         reg32(0x40005404)
#define R_I2C1_OAR1        reg32(0x40005408)
#define R_I2C1_OAR2        reg32(0x4000540c)
#define R_I2C1_DR          reg32(0x40005410)
#define R_I2C1_SR1         reg32(0x40005414)
#define R_I2C1_SR2         reg32(0x40005418)
#define R_I2C1_CCR         reg32(0x4000541c)
#define R_I2C1_TRISE       reg32(0x40005420)
#define R_I2C1_FLTR        reg32(0x40005424)

/* I2C2 registers */
#define R_I2C2_CR1         reg32(0x40005800)
#define R_I2C2_CR2         reg32(0x40005804)
#define R_I2C2_OAR1        reg32(0x40005808)
#define R_I2C2_OAR2        reg32(0x4000580c)
#define R_I2C2_DR          reg32(0x40005810)
#define R_I2C2_SR1         reg32(0x40005814)
#define R_I2C2_SR2         reg32(0x40005818)
#define R_I2C2_CCR         reg32(0x4000581c)
#define R_I2C2_TRISE       reg32(0x40005820)
#define R_I2C2_FLTR        reg32(0x40005824)

/* I2C4 registers */
#define R_I2C4_CR1         reg32(0x40006000)
#define R_I2C4_CR2         reg32(0x40006004)
#define R_I2C4_OAR1        reg32(0x40006008)
#define R_I2C4_OAR2        reg32(0x4000600c)
#define R_I2C4_DR          reg32(0x40006010)
#define R_I2C4_SR1         reg32(0x40006014)
#define R_I2C4_SR2         reg32(0x40006018)
#define R_I2C4_CCR         reg32(0x4000601c)
#define R_I2C4_TRISE       reg32(0x40006020)
#define R_I2C4_FLTR        reg32(0x40006024)

/* Generic I2C register defines */
#define I2C1 0x40005400  /* base for I2C1 */
#define I2C2 0x40005800  /* base for I2C2 */
#define I2C4 0x40006000  /* base for I2C4 */

#define R_I2C_CR1(base)      reg32(base)
#define R_I2C_CR2(base)      reg32(base + 0x04)
#define R_I2C_OAR1(base)     reg32(base + 0x08)
#define R_I2C_OAR2(base)     reg32(base + 0x0c)
#define R_I2C_DR(base)       reg32(base + 0x10)
#define R_I2C_SR1(base)      reg32(base + 0x14)
#define R_I2C_SR2(base)      reg32(base + 0x18)
#define R_I2C_CCR(base)      reg32(base + 0x1c)
#define R_I2C_TRISE(base)    reg32(base + 0x20)
#define R_I2C_FLTR(base)     reg32(base + 0x24)

/* TIM1 registers */
#define R_TIM1_CR1      reg16(0x40010000)
#define R_TIM1_CR2      reg16(0x40010004)
#define R_TIM1_SMCR     reg16(0x40010008)
#define R_TIM1_DIER     reg16(0x4001000c)
#define R_TIM1_SR       reg16(0x40010010)
#define R_TIM1_EGR      reg16(0x40010014)
#define R_TIM1_CCMR1    reg16(0x40010018)
#define R_TIM1_CCMR2    reg16(0x4001001c)
#define R_TIM1_CCER     reg16(0x40010020)
#define R_TIM1_CNT      reg16(0x40010024)
#define R_TIM1_PSC      reg16(0x40010028)
#define R_TIM1_ARR      reg16(0x4001002c)
#define R_TIM1_RCR      reg16(0x40010030)
#define R_TIM1_CCR1     reg16(0x40010034)
#define R_TIM1_CCR2     reg16(0x40010038)
#define R_TIM1_CCR3     reg16(0x4001003c)
#define R_TIM1_CCR4     reg16(0x40010040)
#define R_TIM1_BDTR     reg16(0x40010044)
#define R_TIM1_DCR      reg16(0x40010048)
#define R_TIM1_DMAR     reg16(0x4001004c)

/* TIM1 registers */
#define R_TIM1_CR1      reg16(0x40010000)
#define R_TIM1_CR2      reg16(0x40010004)
#define R_TIM1_SMCR     reg16(0x40010008)
#define R_TIM1_DIER     reg16(0x4001000c)
#define R_TIM1_SR       reg16(0x40010010)
#define R_TIM1_EGR      reg16(0x40010014)
#define R_TIM1_CCMR1    reg16(0x40010018)
#define R_TIM1_CCMR2    reg16(0x4001001c)
#define R_TIM1_CCER     reg16(0x40010020)
#define R_TIM1_CNT      reg16(0x40010024)
#define R_TIM1_PSC      reg16(0x40010028)
#define R_TIM1_ARR      reg16(0x4001002c)
#define R_TIM1_RCR      reg16(0x40010030)
#define R_TIM1_CCR1     reg16(0x40010034)
#define R_TIM1_CCR2     reg16(0x40010038)
#define R_TIM1_CCR3     reg16(0x4001003c)
#define R_TIM1_CCR4     reg16(0x40010040)
#define R_TIM1_BDTR     reg16(0x40010044)
#define R_TIM1_DCR      reg16(0x40010048)
#define R_TIM1_DMAR     reg16(0x4001004c)

/* TIM5 registers */
#define R_TIM5_CR1      reg16(0x40000c00)
#define R_TIM5_CR2      reg16(0x40000c04)
#define R_TIM5_SMCR     reg16(0x40000c08)
#define R_TIM5_DIER     reg16(0x40000c0c)
#define R_TIM5_SR       reg16(0x40000c10)
#define R_TIM5_EGR      reg16(0x40000c14)
#define R_TIM5_CCMR1    reg16(0x40000c18)
#define R_TIM5_CCMR2    reg16(0x40000c1c)
#define R_TIM5_CCER     reg16(0x40000c20)
#define R_TIM5_CNT      reg16(0x40000c24)
#define R_TIM5_PSC      reg16(0x40000c28)
#define R_TIM5_ARR      reg16(0x40000c2c)
#define R_TIM5_CCR1     reg16(0x40000c34)
#define R_TIM5_CCR2     reg16(0x40000c38)
#define R_TIM5_CCR3     reg16(0x40000c3c)
#define R_TIM5_CCR4     reg16(0x40000c40)
#define R_TIM5_DCR      reg16(0x40000c48)
#define R_TIM5_DMAR     reg16(0x40000c4c)
#define R_TIM5_OR       reg16(0x40000c50)

/* DAC registers */
#define R_DAC_CR        reg32(0x40007400)
#define R_DAC_SWTRIGR   reg32(0x40007404)
#define R_DAC_DHR12R1   reg32(0x40007408)
#define R_DAC_DHR12L1   reg32(0x4000740c)
#define R_DAC_DHR8R1    reg32(0x40007410)
#define R_DAC_DOR1      reg32(0x4000742c)
#define R_DAC_SR        reg32(0x40007434)

/* ADC1 registers */
#define R_ADC1_SR       reg32(0x40012000)
#define R_ADC1_CR1      reg32(0x40012004)
#define R_ADC1_CR2      reg32(0x40012008)
#define R_ADC1_SMPR1    reg32(0x4001200c)
#define R_ADC1_SMPR2    reg32(0x40012010)
#define R_ADC1_JOFR1    reg32(0x40012014)
#define R_ADC1_JOFR2    reg32(0x40012018)
#define R_ADC1_JOFR3    reg32(0x4001201c)
#define R_ADC1_JOFR4    reg32(0x40012020)
#define R_ADC1_HTR      reg32(0x40012024)
#define R_ADC1_LTR      reg32(0x40012028)
#define R_ADC1_SQR1     reg32(0x4001202c)
#define R_ADC1_SQR2     reg32(0x40012030)
#define R_ADC1_SQR3     reg32(0x40012034)
#define R_ADC1_JSQR     reg32(0x40012038)
#define R_ADC1_JDR1     reg32(0x4001203c)
#define R_ADC1_JDR2     reg32(0x40012040)
#define R_ADC1_JDR3     reg32(0x40012044)
#define R_ADC1_JDR4     reg32(0x40012048)
#define R_ADC1_DR       reg32(0x4001204c)

#endif /* _REG_H_ */
