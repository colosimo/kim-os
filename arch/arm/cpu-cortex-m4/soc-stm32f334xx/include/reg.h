/*
 * Author: Aurelio Colosimo, 2021
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

#ifndef _REG_H_
#define _REG_H_

#include <basic.h>

#define STACK_TOP ((void*)(0x20003000))

/* PWR registers */
#define R_PWR_CR           reg32(0x40007000)
#define R_PWR_CSR          reg32(0x40007004)

/* RCC registers */
#define R_RCC_CR           reg32(0x40021000)
#define R_RCC_CFGR         reg32(0x40021004)
#define R_RCC_CIR          reg32(0x40021008)
#define R_RCC_APB2RSTR     reg32(0x4002100c)
#define R_RCC_APB1RSTR     reg32(0x40021010)
#define R_RCC_AHBENR       reg32(0x40021014)
#define R_RCC_APB2ENR      reg32(0x40021018)
#define R_RCC_APB1ENR      reg32(0x4002101c)
#define R_RCC_BDCR         reg32(0x40021020)
#define R_RCC_CSR          reg32(0x40021024)
#define R_RCC_AHBRSTR      reg32(0x40021028)
#define R_RCC_CFGR2        reg32(0x4002102c)
#define R_RCC_CFGR3        reg32(0x40021030)

/* Flash interface registers */
#define R_FLASH_ACR        reg32(0x40022000)
#define R_FLASH_KEYR       reg32(0x40022004)
#define R_FLASH_OPTKEYR    reg32(0x40022008)
#define R_FLASH_SR         reg32(0x4002200c)
#define R_FLASH_CR         reg32(0x40022010)
#define R_FLASH_AR         reg32(0x40022014)
#define R_FLASH_OBR        reg32(0x4002201c)
#define R_FLASH_WRPR       reg32(0x40022020)

/* USART1 registers */
#define R_USART1_CR1       reg32(0x40013800)
#define R_USART1_CR2       reg32(0x40013804)
#define R_USART1_CR3       reg32(0x40013808)
#define R_USART1_BRR       reg32(0x4001380c)
#define R_USART1_GTPR      reg32(0x40013810)
#define R_USART1_RTOR      reg32(0x40013814)
#define R_USART1_RQR       reg32(0x40013818)
#define R_USART1_ISR       reg32(0x4001381c)
#define R_USART1_ICR       reg32(0x40013820)
#define R_USART1_RDR       reg32(0x40013824)
#define R_USART1_TDR       reg32(0x40013828)

/* USART2 registers */
#define R_USART2_CR1       reg32(0x40004400)
#define R_USART2_CR2       reg32(0x40004404)
#define R_USART2_CR3       reg32(0x40004408)
#define R_USART2_BRR       reg32(0x4000440c)
#define R_USART2_GTPR      reg32(0x40004410)
#define R_USART2_RTOR      reg32(0x40004414)
#define R_USART2_RQR       reg32(0x40004418)
#define R_USART2_ISR       reg32(0x4000441c)
#define R_USART2_ICR       reg32(0x40004420)
#define R_USART2_RDR       reg32(0x40004424)
#define R_USART2_TDR       reg32(0x40004428)

/* USART3 registers */
#define R_USART3_CR1       reg32(0x40004800)
#define R_USART3_CR2       reg32(0x40004804)
#define R_USART3_CR3       reg32(0x40004808)
#define R_USART3_BRR       reg32(0x4000480c)
#define R_USART3_GTPR      reg32(0x40004810)
#define R_USART3_RTOR      reg32(0x40004814)
#define R_USART3_RQR       reg32(0x40004818)
#define R_USART3_ISR       reg32(0x4000481c)
#define R_USART3_ICR       reg32(0x40004820)
#define R_USART3_RDR       reg32(0x40004824)
#define R_USART3_TDR       reg32(0x40004828)

/* GPIOA registers */
#define R_GPIOA_MODER      reg32(0x48000000)
#define R_GPIOA_OTYPER     reg32(0x48000004)
#define R_GPIOA_OSPEEDR    reg32(0x48000008)
#define R_GPIOA_PUPDR      reg32(0x4800000c)
#define R_GPIOA_IDR        reg32(0x48000010)
#define R_GPIOA_ODR        reg32(0x48000014)
#define R_GPIOA_BSRR       reg32(0x48000018)
#define R_GPIOA_LCKR       reg32(0x4800001c)
#define R_GPIOA_AFRL       reg32(0x48000020)
#define R_GPIOA_AFRH       reg32(0x48000024)
#define R_GPIOA_BRR        reg32(0x48000028)

/* GPIOB registers */
#define R_GPIOB_MODER      reg32(0x48000400)
#define R_GPIOB_OTYPER     reg32(0x48000404)
#define R_GPIOB_OSPEEDR    reg32(0x48000408)
#define R_GPIOB_PUPDR      reg32(0x4800040c)
#define R_GPIOB_IDR        reg32(0x48000410)
#define R_GPIOB_ODR        reg32(0x48000414)
#define R_GPIOB_BSRR       reg32(0x48000418)
#define R_GPIOB_LCKR       reg32(0x4800041c)
#define R_GPIOB_AFRL       reg32(0x48000420)
#define R_GPIOB_AFRH       reg32(0x48000424)
#define R_GPIOB_BRR        reg32(0x48000428)

/* GPIOC registers */
#define R_GPIOC_MODER      reg32(0x48000800)
#define R_GPIOC_OTYPER     reg32(0x48000804)
#define R_GPIOC_OSPEEDR    reg32(0x48000808)
#define R_GPIOC_PUPDR      reg32(0x4800080c)
#define R_GPIOC_IDR        reg32(0x48000810)
#define R_GPIOC_ODR        reg32(0x48000814)
#define R_GPIOC_BSRR       reg32(0x48000818)
#define R_GPIOC_LCKR       reg32(0x4800081c)
#define R_GPIOC_AFRL       reg32(0x48000820)
#define R_GPIOC_AFRH       reg32(0x48000824)
#define R_GPIOC_BRR        reg32(0x48000828)

/* GPIOD registers */
#define R_GPIOD_MODER      reg32(0x48000c00)
#define R_GPIOD_OTYPER     reg32(0x48000c04)
#define R_GPIOD_OSPEEDR    reg32(0x48000c08)
#define R_GPIOD_PUPDR      reg32(0x48000c0c)
#define R_GPIOD_IDR        reg32(0x48000c10)
#define R_GPIOD_ODR        reg32(0x48000c14)
#define R_GPIOD_BSRR       reg32(0x48000c18)
#define R_GPIOD_LCKR       reg32(0x48000c1c)
#define R_GPIOD_AFRL       reg32(0x48000c20)
#define R_GPIOD_AFRH       reg32(0x48000c24)
#define R_GPIOD_BRR        reg32(0x48000c28)

/* GPIOF registers */
#define R_GPIOF_MODER      reg32(0x48001400)
#define R_GPIOF_OTYPER     reg32(0x48001404)
#define R_GPIOF_OSPEEDR    reg32(0x48001408)
#define R_GPIOF_PUPDR      reg32(0x4800140c)
#define R_GPIOF_IDR        reg32(0x48001410)
#define R_GPIOF_ODR        reg32(0x48001414)
#define R_GPIOF_BSRR       reg32(0x48001418)
#define R_GPIOF_LCKR       reg32(0x4800141c)
#define R_GPIOF_AFRL       reg32(0x48001420)
#define R_GPIOF_AFRH       reg32(0x48001424)
#define R_GPIOF_BRR        reg32(0x48001428)

/* TIM3 registers */
#define R_TIM3_CR1         reg32(0x40000400)
#define R_TIM3_CR2         reg32(0x40000404)
#define R_TIM3_SMCR        reg32(0x40000408)
#define R_TIM3_DIER        reg32(0x4000040c)
#define R_TIM3_SR          reg32(0x40000410)
#define R_TIM3_EGR         reg32(0x40000414)
#define R_TIM3_CCMR1       reg32(0x40000418)
#define R_TIM3_CCMR2       reg32(0x4000041c)
#define R_TIM3_CCER        reg32(0x40000420)
#define R_TIM3_CNT         reg32(0x40000424)
#define R_TIM3_PSC         reg32(0x40000428)
#define R_TIM3_ARR         reg32(0x4000042c)
#define R_TIM3_CCR1        reg32(0x40000434)
#define R_TIM3_CCR2        reg32(0x40000438)
#define R_TIM3_CCR3        reg32(0x4000043c)
#define R_TIM3_CCR4        reg32(0x40000440)
#define R_TIM3_DCR         reg32(0x40000448)
#define R_TIM3_DMAR        reg32(0x4000044c)

/* SPI1 registers */
#define R_SPI1_CR1         reg32(0x40013000)
#define R_SPI1_CR2         reg32(0x40013004)
#define R_SPI1_SR          reg32(0x40013008)
#define R_SPI1_DR          reg8 (0x4001300c)
#define R_SPI1_CRCPR       reg32(0x40013010)
#define R_SPI1_RXCRCR      reg32(0x40013014)
#define R_SPI1_TXCRCR      reg32(0x40013018)

/* I2C1 registers */
#define R_I2C1_CR1         reg32(0x40005400)
#define R_I2C1_CR2         reg32(0x40005404)
#define R_I2C1_OAR1        reg32(0x40005408)
#define R_I2C1_OAR2        reg32(0x4000540c)
#define R_I2C1_TIMINGR     reg32(0x40005410)
#define R_I2C1_TIMEOUTR    reg32(0x40005414)
#define R_I2C1_ISR         reg32(0x40005418)
#define R_I2C1_ICR         reg32(0x4000541c)
#define R_I2C1_PECR        reg32(0x40005420)
#define R_I2C1_RXDR        reg32(0x40005424)
#define R_I2C1_TXDR        reg32(0x40005428)

/* I2C2 registers */
#define R_I2C2_CR1         reg32(0x40005800)
#define R_I2C2_CR2         reg32(0x40005804)
#define R_I2C2_OAR1        reg32(0x40005808)
#define R_I2C2_OAR2        reg32(0x4000580c)
#define R_I2C2_TIMINGR     reg32(0x40005810)
#define R_I2C2_TIMEOUTR    reg32(0x40005814)
#define R_I2C2_ISR         reg32(0x40005818)
#define R_I2C2_ICR         reg32(0x4000581c)
#define R_I2C2_PECR        reg32(0x40005820)
#define R_I2C2_RXDR        reg32(0x40005824)
#define R_I2C2_TXDR        reg32(0x40005828)

/* Generic I2C register defines */
#define I2C1 0x40005400  /* base for I2C1 */

#define R_I2C_CR1(base)      reg32(base)
#define R_I2C_CR2(base)      reg32(base + 0x04)
#define R_I2C_OAR1(base)     reg32(base + 0x08)
#define R_I2C_OAR2(base)     reg32(base + 0x0c)
#define R_I2C_TIMINGR(base)  reg32(base + 0x10)
#define R_I2C_TIMEOUTR(base) reg32(base + 0x14)
#define R_I2C_ISR(base)      reg32(base + 0x18)
#define R_I2C_ICR(base)      reg32(base + 0x1c)
#define R_I2C_PECR(base)     reg32(base + 0x20)
#define R_I2C_RXDR(base)     reg32(base + 0x24)
#define R_I2C_TXDR(base)     reg32(base + 0x28)

/* RTC */
#define R_RTC_TR             reg32(0x40002800)
#define R_RTC_DR             reg32(0x40002804)
#define R_RTC_CR             reg32(0x40002808)
#define R_RTC_ISR            reg32(0x4000280c)
#define R_RTC_PRER           reg32(0x40002810)
#define R_RTC_WUTR           reg32(0x40002814)
#define R_RTC_ALRMAR         reg32(0x4000281c)
#define R_RTC_ALRMBR         reg32(0x40002820)
#define R_RTC_WPR            reg32(0x40002824)
#define R_RTC_SSR            reg32(0x40002828)
#define R_RTC_SHIFTR         reg32(0x4000282c)
#define R_RTC_TSTR           reg32(0x40002830)
#define R_RTC_TSDR           reg32(0x40002834)
#define R_RTC_TSSSR          reg32(0x40002838)
#define R_RTC_CALR           reg32(0x4000283c)
#define R_RTC_TAFCR          reg32(0x40002840)
#define R_RTC_ALRMASSR       reg32(0x40002844)
#define R_RTC_ALRMBSSR       reg32(0x40002848)
#define R_RTC_OR             reg32(0x4000284c)
#define R_RTC_BKPR(x)        reg32(0x40002850 + 0x4 * (x))

#endif /* _REG_H_ */

