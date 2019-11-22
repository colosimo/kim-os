
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

#ifdef SOC_VARIANT_stm32f4r9zi
#define STACK_TOP ((void*)(0x200a0000))
#else
#error Unhandled SOC_VARIANT: $(SOC_VARIANT)
#endif

/* RCC registers */
#define R_RCC_CR           reg32(0x40021000)
#define R_RCC_ICSCR        reg32(0x40021004)
#define R_RCC_CFGR         reg32(0x40021008)
#define R_RCC_PLLCFGR      reg32(0x4002100c)
#define R_RCC_PLLSAI1CFGR  reg32(0x40021010)
#define R_RCC_PLLSAI2CFGR  reg32(0x40021014)
#define R_RCC_CIER         reg32(0x40021018)
#define R_RCC_CIFR         reg32(0x4002101c)
#define R_RCC_CICR         reg32(0x40021020)
#define R_RCC_AHB1RSTR     reg32(0x40021028)
#define R_RCC_AHB2RSTR     reg32(0x4002102c)
#define R_RCC_AHB3RSTR     reg32(0x40021030)
#define R_RCC_APB1RSTR1    reg32(0x40021038)
#define R_RCC_APB1RSTR2    reg32(0x4002103c)
#define R_RCC_APB2RSTR     reg32(0x40021040)
#define R_RCC_AHB1ENR      reg32(0x40021048)
#define R_RCC_AHB2ENR      reg32(0x4002104c)
#define R_RCC_AHB3ENR      reg32(0x40021050)
#define R_RCC_APB1ENR1     reg32(0x40021058)
#define R_RCC_APB1ENR2     reg32(0x4002105c)
#define R_RCC_APB2ENR      reg32(0x40021060)
#define R_RCC_AHB1SMENR    reg32(0x40021068)
#define R_RCC_AHB2SMENR    reg32(0x4002106c)
#define R_RCC_AHB3SMENR    reg32(0x40021070)
#define R_RCC_APB1SMENR1   reg32(0x40021078)
#define R_RCC_APB1SMENR2   reg32(0x4002107c)
#define R_RCC_APB2SMENR    reg32(0x40021080)
#define R_RCC_CCIPR        reg32(0x40021088)
#define R_RCC_BDCR         reg32(0x40021090)
#define R_RCC_CSR          reg32(0x40021094)
#define R_RCC_CRRCR        reg32(0x40021098)
#define R_RCC_CCIPR2       reg32(0x4002109c)

/* Flash interface registers */
#define R_FLASH_ACR        reg32(0x40022000)
#define R_FLASH_PDKEYR     reg32(0x40022004)
#define R_FLASH_KEYR       reg32(0x40022008)
#define R_FLASH_OPTKEYR    reg32(0x4002200c)
#define R_FLASH_SR         reg32(0x40022010)
#define R_FLASH_CR         reg32(0x40022014)
#define R_FLASH_ECCR       reg32(0x40022018)
#define R_FLASH_OPTR       reg32(0x40022020)
#define R_FLASH_PCROP1SR   reg32(0x40022024)
#define R_FLASH_PCROP1ER   reg32(0x40022028)
#define R_FLASH_WRP1AR     reg32(0x4002202c)
#define R_FLASH_WRP2AR     reg32(0x40022030)
#define R_FLASH_PCROP2SR   reg32(0x40022044)
#define R_FLASH_PCROP2ER   reg32(0x40022048)
#define R_FLASH_WRP1BR     reg32(0x4002204c)
#define R_FLASH_WRP2BR     reg32(0x40022050)
#define R_FLASH_CFGR       reg32(0x40022130)

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
#define R_USART1_PRESC     reg32(0x4001382c)

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
#define R_USART2_PRESC     reg32(0x4000442c)

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
#define R_USART3_PRESC     reg32(0x4000482c)

/* GPIOA registers */
#define R_GPIOA_MODER      reg32(0x48000000)
#define R_GPIOA_OTYPER     reg32(0x48000004)
#define R_GPIOA_OSPEEDR    reg32(0x48000008)
#define R_GPIOA_PUPDR      reg32(0x4800000c)
#define R_GPIOA_IDR        reg32(0x48000010)
#define R_GPIOA_ODR        reg32(0x48000014)
#define R_GPIOA_BSRR       reg32(0x48000018)
#define R_GPIOA_LCKR       reg32(0x4800001c)
#define R_GPIOA_AFRL       reg32(0x48000000)
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
#define R_GPIOB_AFRL       reg32(0x48000400)
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
#define R_GPIOC_AFRL       reg32(0x48000800)
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
#define R_GPIOD_AFRL       reg32(0x48000c00)
#define R_GPIOD_AFRH       reg32(0x48000c24)
#define R_GPIOD_BRR        reg32(0x48000c28)

/* GPIOE registers */
#define R_GPIOE_MODER      reg32(0x48001000)
#define R_GPIOE_OTYPER     reg32(0x48001004)
#define R_GPIOE_OSPEEDR    reg32(0x48001008)
#define R_GPIOE_PUPDR      reg32(0x4800100c)
#define R_GPIOE_IDR        reg32(0x48001010)
#define R_GPIOE_ODR        reg32(0x48001014)
#define R_GPIOE_BSRR       reg32(0x48001018)
#define R_GPIOE_LCKR       reg32(0x4800101c)
#define R_GPIOE_AFRL       reg32(0x48001000)
#define R_GPIOE_AFRH       reg32(0x48001024)
#define R_GPIOE_BRR        reg32(0x48001028)

/* GPIOF registers */
#define R_GPIOF_MODER      reg32(0x48001400)
#define R_GPIOF_OTYPER     reg32(0x48001404)
#define R_GPIOF_OSPEEDR    reg32(0x48001408)
#define R_GPIOF_PUPDR      reg32(0x4800140c)
#define R_GPIOF_IDR        reg32(0x48001410)
#define R_GPIOF_ODR        reg32(0x48001414)
#define R_GPIOF_BSRR       reg32(0x48001418)
#define R_GPIOF_LCKR       reg32(0x4800141c)
#define R_GPIOF_AFRL       reg32(0x48001400)
#define R_GPIOF_AFRH       reg32(0x48001424)
#define R_GPIOF_BRR        reg32(0x48001428)

/* GPIOG registers */
#define R_GPIOG_MODER      reg32(0x48001800)
#define R_GPIOG_OTYPER     reg32(0x48001804)
#define R_GPIOG_OSPEEDR    reg32(0x48001808)
#define R_GPIOG_PUPDR      reg32(0x4800180c)
#define R_GPIOG_IDR        reg32(0x48001810)
#define R_GPIOG_ODR        reg32(0x48001814)
#define R_GPIOG_BSRR       reg32(0x48001818)
#define R_GPIOG_LCKR       reg32(0x4800181c)
#define R_GPIOG_AFRL       reg32(0x48001800)
#define R_GPIOG_AFRH       reg32(0x48001824)
#define R_GPIOG_BRR        reg32(0x48001828)

/* GPIOH registers */
#define R_GPIOH_MODER      reg32(0x48001c00)
#define R_GPIOH_OTYPER     reg32(0x48001c04)
#define R_GPIOH_OSPEEDR    reg32(0x48001c08)
#define R_GPIOH_PUPDR      reg32(0x48001c0c)
#define R_GPIOH_IDR        reg32(0x48001c10)
#define R_GPIOH_ODR        reg32(0x48001c14)
#define R_GPIOH_BSRR       reg32(0x48001c18)
#define R_GPIOH_LCKR       reg32(0x48001c1c)
#define R_GPIOH_AFRL       reg32(0x48001c00)
#define R_GPIOH_AFRH       reg32(0x48001c24)
#define R_GPIOH_BRR        reg32(0x48001c28)

/* GPIOI registers */
#define R_GPIOI_MODER      reg32(0x48002000)
#define R_GPIOI_OTYPER     reg32(0x48002004)
#define R_GPIOI_OSPEEDR    reg32(0x48002008)
#define R_GPIOI_PUPDR      reg32(0x4800200c)
#define R_GPIOI_IDR        reg32(0x48002010)
#define R_GPIOI_ODR        reg32(0x48002014)
#define R_GPIOI_BSRR       reg32(0x48002018)
#define R_GPIOI_LCKR       reg32(0x4800201c)
#define R_GPIOI_AFRL       reg32(0x48002000)
#define R_GPIOI_AFRH       reg32(0x48002024)
#define R_GPIOI_BRR        reg32(0x48002028)

#endif /* _REG_H_ */
