/*
 * Author: Aurelio Colosimo, 2020
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

#ifndef _REG_H_
#define _REG_H_

#include <basic.h>

#ifdef SOC_VARIANT_stm32f469ai
#define STACK_TOP ((void*)(0x20000000 + 320 * _K))
#else
#error Unhandled SOC_VARIANT: $(SOC_VARIANT)
#endif

/* PWR registers */
#define R_PWR_CR           reg32(0x40007000)
#define R_PWR_CSR          reg32(0x40007004)

/* RCC registers */
#define R_RCC_CR           reg32(0x40023800)
#define R_RCC_PLLCFGR      reg32(0x40023804)
#define R_RCC_CFGR         reg32(0x40023808)
#define R_RCC_CIR          reg32(0x4002380C)
#define R_RCC_AHB1RSTR     reg32(0x40023810)
#define R_RCC_AHB2RSTR     reg32(0x40023814)
#define R_RCC_AHB3RSTR     reg32(0x40023818)
#define R_RCC_APB1RSTR     reg32(0x40023820)
#define R_RCC_APB2RSTR     reg32(0x40023824)
#define R_RCC_AHB1ENR      reg32(0x40023830)
#define R_RCC_AHB2ENR      reg32(0x40023834)
#define R_RCC_AHB3ENR      reg32(0x40023838)
#define R_RCC_APB1ENR      reg32(0x40023840)
#define R_RCC_APB2ENR      reg32(0x40023844)

/* EXTI registers */
#define R_EXTI_IMR         reg32(0x40013c00)
#define R_EXTI_EMR         reg32(0x40013c04)
#define R_EXTI_RTSR        reg32(0x40013c08)
#define R_EXTI_FTSR        reg32(0x40013c0c)
#define R_EXTI_SWIER       reg32(0x40013c10)
#define R_EXTI_PR          reg32(0x40013c14)

/* SYSCFG registers */
#define R_SYSCFG_MEMRMP    reg32(0x40013800)
#define R_SYSCFG_PMC       reg32(0x40013804)
#define R_SYSCFG_EXTICR1   reg32(0x40013808)
#define R_SYSCFG_EXTICR2   reg32(0x4001380c)
#define R_SYSCFG_EXTICR3   reg32(0x40013810)
#define R_SYSCFG_EXTICR4   reg32(0x40013814)
#define R_SYSCFG_CMPCR     reg32(0x40013820)

/* Flash interface registers */
#define R_FLASH_ACR        reg32(0x40023c00)
#define R_FLASH_KEYR       reg32(0x40023c04)
#define R_FLASH_OPTKEYR    reg32(0x40023c08)
#define R_FLASH_SR         reg32(0x40023c0c)
#define R_FLASH_CR         reg32(0x40023c10)
#define R_FLASH_OPTCR      reg32(0x40023c14)
#define R_FLASH_OPTCR1     reg32(0x40023c18)

/* USART1 registers */
#define R_USART1_SR        reg32(0x40011000)
#define R_USART1_DR        reg32(0x40011004)
#define R_USART1_BRR       reg32(0x40011008)
#define R_USART1_CR1       reg32(0x4001100C)
#define R_USART1_CR2       reg32(0x40011010)
#define R_USART1_CR3       reg32(0x40011014)
#define R_USART1_GTPR      reg32(0x40011018)

/* USART2 registers */
#define R_USART2_SR        reg32(0x40004400)
#define R_USART2_DR        reg32(0x40004404)
#define R_USART2_BRR       reg32(0x40004408)
#define R_USART2_CR1       reg32(0x4000440C)
#define R_USART2_CR2       reg32(0x40004410)
#define R_USART2_CR3       reg32(0x40004414)
#define R_USART2_GTPR      reg32(0x40004418)

/* USART3 registers */
#define R_USART3_SR        reg32(0x40004800)
#define R_USART3_DR        reg32(0x40004804)
#define R_USART3_BRR       reg32(0x40004808)
#define R_USART3_CR1       reg32(0x4000480C)
#define R_USART3_CR2       reg32(0x40004810)
#define R_USART3_CR3       reg32(0x40004814)
#define R_USART3_GTPR      reg32(0x40004818)

/* UART4 registers */
#define R_UART4_SR        reg32(0x40004c00)
#define R_UART4_DR        reg32(0x40004c04)
#define R_UART4_BRR       reg32(0x40004c08)
#define R_UART4_CR1       reg32(0x40004c0C)
#define R_UART4_CR2       reg32(0x40004c10)
#define R_UART4_CR3       reg32(0x40004c14)
#define R_UART4_GTPR      reg32(0x40004c18)

/* UART5 registers */
#define R_UART5_SR        reg32(0x40005000)
#define R_UART5_DR        reg32(0x40005004)
#define R_UART5_BRR       reg32(0x40005008)
#define R_UART5_CR1       reg32(0x4000500C)
#define R_UART5_CR2       reg32(0x40005010)
#define R_UART5_CR3       reg32(0x40005014)
#define R_UART5_GTPR      reg32(0x40005018)

/* GPIOA registers */
#define R_GPIOA_MODER      reg32(0x40020000)
#define R_GPIOA_OTYPER     reg32(0x40020004)
#define R_GPIOA_OSPEEDR    reg32(0x40020008)
#define R_GPIOA_PUPDR      reg32(0x4002000c)
#define R_GPIOA_IDR        reg32(0x40020010)
#define R_GPIOA_ODR        reg32(0x40020014)
#define R_GPIOA_BSRR       reg32(0x40020018)
#define R_GPIOA_LCKR       reg32(0x4002001c)
#define R_GPIOA_AFRL       reg32(0x40020020)
#define R_GPIOA_AFRH       reg32(0x40020024)

/* GPIOB registers */
#define R_GPIOB_MODER      reg32(0x40020400)
#define R_GPIOB_OTYPER     reg32(0x40020404)
#define R_GPIOB_OSPEEDR    reg32(0x40020408)
#define R_GPIOB_PUPDR      reg32(0x4002040c)
#define R_GPIOB_IDR        reg32(0x40020410)
#define R_GPIOB_ODR        reg32(0x40020414)
#define R_GPIOB_BSRR       reg32(0x40020418)
#define R_GPIOB_LCKR       reg32(0x4002041c)
#define R_GPIOB_AFRL       reg32(0x40020420)
#define R_GPIOB_AFRH       reg32(0x40020424)

/* GPIOC registers */
#define R_GPIOC_MODER      reg32(0x40020800)
#define R_GPIOC_OTYPER     reg32(0x40020804)
#define R_GPIOC_OSPEEDR    reg32(0x40020808)
#define R_GPIOC_PUPDR      reg32(0x4002080c)
#define R_GPIOC_IDR        reg32(0x40020810)
#define R_GPIOC_ODR        reg32(0x40020814)
#define R_GPIOC_BSRR       reg32(0x40020818)
#define R_GPIOC_LCKR       reg32(0x4002081c)
#define R_GPIOC_AFRL       reg32(0x40020820)
#define R_GPIOC_AFRH       reg32(0x40020824)

/* GPIOD registers */
#define R_GPIOD_MODER      reg32(0x40020c00)
#define R_GPIOD_OTYPER     reg32(0x40020c04)
#define R_GPIOD_OSPEEDR    reg32(0x40020c08)
#define R_GPIOD_PUPDR      reg32(0x40020c0c)
#define R_GPIOD_IDR        reg32(0x40020c10)
#define R_GPIOD_ODR        reg32(0x40020c14)
#define R_GPIOD_BSRR       reg32(0x40020c18)
#define R_GPIOD_LCKR       reg32(0x40020c1c)
#define R_GPIOD_AFRL       reg32(0x40020c20)
#define R_GPIOD_AFRH       reg32(0x40020c24)

/* GPIOE registers */
#define R_GPIOE_MODER      reg32(0x40021000)
#define R_GPIOE_OTYPER     reg32(0x40021004)
#define R_GPIOE_OSPEEDR    reg32(0x40021008)
#define R_GPIOE_PUPDR      reg32(0x4002100c)
#define R_GPIOE_IDR        reg32(0x40021010)
#define R_GPIOE_ODR        reg32(0x40021014)
#define R_GPIOE_BSRR       reg32(0x40021018)
#define R_GPIOE_LCKR       reg32(0x4002101c)
#define R_GPIOE_AFRL       reg32(0x40021020)
#define R_GPIOE_AFRH       reg32(0x40021024)

/* GPIOF registers */
#define R_GPIOF_MODER      reg32(0x40021400)
#define R_GPIOF_OTYPER     reg32(0x40021404)
#define R_GPIOF_OSPEEDR    reg32(0x40021408)
#define R_GPIOF_PUPDR      reg32(0x4002140c)
#define R_GPIOF_IDR        reg32(0x40021410)
#define R_GPIOF_ODR        reg32(0x40021414)
#define R_GPIOF_BSRR       reg32(0x40021418)
#define R_GPIOF_LCKR       reg32(0x4002141c)
#define R_GPIOF_AFRL       reg32(0x40021420)
#define R_GPIOF_AFRH       reg32(0x40021424)

/* GPIOG registers */
#define R_GPIOG_MODER      reg32(0x40021800)
#define R_GPIOG_OTYPER     reg32(0x40021804)
#define R_GPIOG_OSPEEDR    reg32(0x40021808)
#define R_GPIOG_PUPDR      reg32(0x4002180c)
#define R_GPIOG_IDR        reg32(0x40021810)
#define R_GPIOG_ODR        reg32(0x40021814)
#define R_GPIOG_BSRR       reg32(0x40021818)
#define R_GPIOG_LCKR       reg32(0x4002181c)
#define R_GPIOG_AFRL       reg32(0x40021820)
#define R_GPIOG_AFRH       reg32(0x40021824)

/* GPIOH registers */
#define R_GPIOH_MODER      reg32(0x40021c00)
#define R_GPIOH_OTYPER     reg32(0x40021c04)
#define R_GPIOH_OSPEEDR    reg32(0x40021c08)
#define R_GPIOH_PUPDR      reg32(0x40021c0c)
#define R_GPIOH_IDR        reg32(0x40021c10)
#define R_GPIOH_ODR        reg32(0x40021c14)
#define R_GPIOH_BSRR       reg32(0x40021c18)
#define R_GPIOH_LCKR       reg32(0x40021c1c)
#define R_GPIOH_AFRL       reg32(0x40021c20)
#define R_GPIOH_AFRH       reg32(0x40021c24)

/* GPIOI registers */
#define R_GPIOI_MODER      reg32(0x40022000)
#define R_GPIOI_OTYPER     reg32(0x40022004)
#define R_GPIOI_OSPEEDR    reg32(0x40022008)
#define R_GPIOI_PUPDR      reg32(0x4002200c)
#define R_GPIOI_IDR        reg32(0x40022010)
#define R_GPIOI_ODR        reg32(0x40022014)
#define R_GPIOI_BSRR       reg32(0x40022018)
#define R_GPIOI_LCKR       reg32(0x4002201c)
#define R_GPIOI_AFRL       reg32(0x40022020)
#define R_GPIOI_AFRH       reg32(0x40022024)

/* GPIOJ registers */
#define R_GPIOJ_MODER      reg32(0x40022400)
#define R_GPIOJ_OTYPER     reg32(0x40022404)
#define R_GPIOJ_OSPEEDR    reg32(0x40022408)
#define R_GPIOJ_PUPDR      reg32(0x4002240c)
#define R_GPIOJ_IDR        reg32(0x40022410)
#define R_GPIOJ_ODR        reg32(0x40022414)
#define R_GPIOJ_BSRR       reg32(0x40022418)
#define R_GPIOJ_LCKR       reg32(0x4002241c)
#define R_GPIOJ_AFRL       reg32(0x40022420)
#define R_GPIOJ_AFRH       reg32(0x40022424)
#define R_GPIOJ_BRR        reg32(0x40022428)

/* GPIOK registers */
#define R_GPIOK_MODER      reg32(0x40022800)
#define R_GPIOK_OTYPER     reg32(0x40022804)
#define R_GPIOK_OSPEEDR    reg32(0x40022808)
#define R_GPIOK_PUPDR      reg32(0x4002280c)
#define R_GPIOK_IDR        reg32(0x40022810)
#define R_GPIOK_ODR        reg32(0x40022814)
#define R_GPIOK_BSRR       reg32(0x40022818)
#define R_GPIOK_LCKR       reg32(0x4002281c)
#define R_GPIOK_AFRL       reg32(0x40022820)
#define R_GPIOK_AFRH       reg32(0x40022824)
#define R_GPIOK_BRR        reg32(0x40022828)

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

/* I2C3 registers */
#define R_I2C3_CR1         reg32(0x40005c00)
#define R_I2C3_CR2         reg32(0x40005c04)
#define R_I2C3_OAR1        reg32(0x40005c08)
#define R_I2C3_OAR2        reg32(0x40005c0c)
#define R_I2C3_DR          reg32(0x40005c10)
#define R_I2C3_SR1         reg32(0x40005c14)
#define R_I2C3_SR2         reg32(0x40005c18)
#define R_I2C3_CCR         reg32(0x40005c1c)
#define R_I2C3_TRISE       reg32(0x40005c20)
#define R_I2C3_FLTR        reg32(0x40005c24)

/* Generic I2C register defines */
#define I2C1 0x40005400  /* base for I2C1 */
#define I2C2 0x40005800  /* base for I2C2 */
#define I2C3 0x40005c00  /* base for I2C3 */

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

/* Device electronic signature */
#define R_UUID_0             reg32(0x1fff7a10)
#define R_UUID_1             reg32(0x1fff7a14)
#define R_UUID_2             reg32(0x1fff7a18)

#endif /* _REG_H_ */
