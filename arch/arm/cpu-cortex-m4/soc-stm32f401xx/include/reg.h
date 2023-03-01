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

#define STACK_TOP ((void*)(0x20018000))

/* PWR registers */
#define R_PWR_CR           reg32(0x40007000)
#define R_PWR_CSR          reg32(0x40007004)

/* RCC registers */
#define R_RCC_CR           reg32(0x40023800)
#define R_RCC_PLLCFGR      reg32(0x40023804)
#define R_RCC_CFGR         reg32(0x40023808)
#define R_RCC_CIR          reg32(0x4002380c)
#define R_RCC_AHB1RSTR     reg32(0x40023810)
#define R_RCC_AHB2RSTR     reg32(0x40023814)
#define R_RCC_APB1RSTR     reg32(0x40023820)
#define R_RCC_APB2RSTR     reg32(0x40023824)
#define R_RCC_AHB1ENR      reg32(0x40023830)
#define R_RCC_AHB2ENR      reg32(0x40023834)
#define R_RCC_APB1ENR      reg32(0x40023840)
#define R_RCC_APB2ENR      reg32(0x40023844)
#define R_RCC_APB1LPENR    reg32(0x40023860)
#define R_RCC_APB2LPENR    reg32(0x40023864)
#define R_RCC_BDCR         reg32(0x40023870)
#define R_RCC_SSCGR        reg32(0x40023880)
#define R_RCC_PLLI2SCFGR   reg32(0x40023884)
#define R_RCC_DCKCFGR      reg32(0x40023884)

/* System Configuration Controller registers */
#define R_SYSCFG_MEMRMP    reg32(0x40013800)
#define R_SYSCFG_PMC       reg32(0x40013804)
#define R_SYSCFG_EXTICR1   reg32(0x40013808)
#define R_SYSCFG_EXTICR2   reg32(0x4001380c)
#define R_SYSCFG_EXTICR3   reg32(0x40013810)
#define R_SYSCFG_EXTICR4   reg32(0x40013814)
#define R_SYSCFG_CMPCR     reg32(0x40013820)

/* EXTI Registers */
#define R_EXTI_IMR         reg32(0x40013c00)
#define R_EXTI_EMR         reg32(0x40013c04)
#define R_EXTI_RTSR        reg32(0x40013c08)
#define R_EXTI_FTSR        reg32(0x40013c0c)
#define R_EXTI_SWIER       reg32(0x40013c10)
#define R_EXTI_PR          reg32(0x40013c14)
#define R_EXTI_IMR1        R_EXTI_IMR
#define R_EXTI_EMR1        R_EXTI_EMR
#define R_EXTI_RTSR1       R_EXTI_RTSR
#define R_EXTI_FTSR1       R_EXTI_FTSR
#define R_EXTI_SWIER1      R_EXTI_SWIER
#define R_EXTI_PR1         R_EXTI_PR

/* Flash interface registers */
#define R_FLASH_ACR        reg32(0x40023c00)
#define R_FLASH_KEYR       reg32(0x40023c04)
#define R_FLASH_OPTKEYR    reg32(0x40023c08)
#define R_FLASH_SR         reg32(0x40023c0c)
#define R_FLASH_CR         reg32(0x40023c10)
#define R_FLASH_OPTCR      reg32(0x40023c14)

/* USART1 registers */
#define R_USART1_SR        reg32(0x40011000)
#define R_USART1_DR        reg32(0x40011004)
#define R_USART1_BRR       reg32(0x40011008)
#define R_USART1_CR1       reg32(0x4001100c)
#define R_USART1_CR2       reg32(0x40011010)
#define R_USART1_CR3       reg32(0x40011014)
#define R_USART1_GTPR      reg32(0x40011018)

/* USART2 registers */
#define R_USART2_SR        reg32(0x40004400)
#define R_USART2_DR        reg32(0x40004404)
#define R_USART2_BRR       reg32(0x40004408)
#define R_USART2_CR1       reg32(0x4000440c)
#define R_USART2_CR2       reg32(0x40004410)
#define R_USART2_CR3       reg32(0x40004414)
#define R_USART2_GTPR      reg32(0x40004418)

/* USART6 registers */
#define R_USART6_SR        reg32(0x40011400)
#define R_USART6_DR        reg32(0x40011404)
#define R_USART6_BRR       reg32(0x40011408)
#define R_USART6_CR1       reg32(0x4001140c)
#define R_USART6_CR2       reg32(0x40011410)
#define R_USART6_CR3       reg32(0x40011414)
#define R_USART6_GTPR      reg32(0x40011418)

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

/* TIM2 registers */
#define R_TIM2_CR1         reg32(0x40000000)
#define R_TIM2_CR2         reg32(0x40000004)
#define R_TIM2_SMCR        reg32(0x40000008)
#define R_TIM2_DIER        reg32(0x4000000c)
#define R_TIM2_SR          reg32(0x40000010)
#define R_TIM2_EGR         reg32(0x40000014)
#define R_TIM2_CCMR1       reg32(0x40000018)
#define R_TIM2_CCMR2       reg32(0x4000001c)
#define R_TIM2_CCER        reg32(0x40000020)
#define R_TIM2_CNT         reg32(0x40000024)
#define R_TIM2_PSC         reg32(0x40000028)
#define R_TIM2_ARR         reg32(0x4000002c)
#define R_TIM2_CCR1        reg32(0x40000034)
#define R_TIM2_CCR2        reg32(0x40000038)
#define R_TIM2_CCR3        reg32(0x4000003c)
#define R_TIM2_CCR4        reg32(0x40000040)
#define R_TIM2_DCR         reg32(0x40000048)
#define R_TIM2_DMAR        reg32(0x4000004c)
#define R_TIM2_OR          reg32(0x40000050)

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

/* TIM4 registers */
#define R_TIM4_CR1         reg32(0x40000800)
#define R_TIM4_CR2         reg32(0x40000804)
#define R_TIM4_SMCR        reg32(0x40000808)
#define R_TIM4_DIER        reg32(0x4000080c)
#define R_TIM4_SR          reg32(0x40000810)
#define R_TIM4_EGR         reg32(0x40000814)
#define R_TIM4_CCMR1       reg32(0x40000818)
#define R_TIM4_CCMR2       reg32(0x4000081c)
#define R_TIM4_CCER        reg32(0x40000820)
#define R_TIM4_CNT         reg32(0x40000824)
#define R_TIM4_PSC         reg32(0x40000828)
#define R_TIM4_ARR         reg32(0x4000082c)
#define R_TIM4_CCR1        reg32(0x40000834)
#define R_TIM4_CCR2        reg32(0x40000838)
#define R_TIM4_CCR3        reg32(0x4000083c)
#define R_TIM4_CCR4        reg32(0x40000840)
#define R_TIM4_DCR         reg32(0x40000848)
#define R_TIM4_DMAR        reg32(0x4000084c)

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
#define R_I2C1_FLT         reg32(0x40005424)

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
#define R_I2C2_FLT         reg32(0x40005824)

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
#define R_I2C3_FLT         reg32(0x40005c24)

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
#define R_I2C_FLT(base)      reg32(base + 0x24)

/* RTC */
#define R_RTC_TR             reg32(0x40002800)
#define R_RTC_DR             reg32(0x40002804)
#define R_RTC_CR             reg32(0x40002808)
#define R_RTC_ISR            reg32(0x4000280c)
#define R_RTC_PRER           reg32(0x40002810)
#define R_RTC_WUTR           reg32(0x40002814)
#define R_RTC_CALIBR         reg32(0x40002818)
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

