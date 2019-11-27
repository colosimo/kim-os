
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

/* OTG_FS core global registers */
#define R_OTG_FS_GOTGCTL   reg32(0x50000000)
#define R_OTG_FS_GOTGINT   reg32(0x50000004)
#define R_OTG_FS_GAHBCFG   reg32(0x50000008)
#define R_OTG_FS_GUSBCFG   reg32(0x5000000C)
#define R_OTG_FS_GRSTCTL   reg32(0x50000010)
#define R_OTG_FS_GINTSTS   reg32(0x50000014)
#define R_OTG_FS_GINTMSK   reg32(0x50000018)
#define R_OTG_FS_GRXSTSR   reg32(0x5000001C)
#define R_OTG_FS_GRXSTSP   reg32(0x50000020)
#define R_OTG_FS_GRXFSIZ   reg32(0x50000024)
#define R_OTG_FS_HNPTXFSIZ reg32(0x50000028)
#define R_OTG_FS_DIEPTXF0  reg32(0x50000028)
#define R_OTG_FS_HNPTXSTS  reg32(0x5000002C)
#define R_OTG_FS_GCCFG     reg32(0x50000038)
#define R_OTG_FS_CID       reg32(0x5000003C)
#define R_OTG_FS_HPTXFSIZ  reg32(0x50000100)
#define R_OTG_FS_DIEPTXF1  reg32(0x50000104)
#define R_OTG_FS_DIEPTXF2  reg32(0x50000108)
#define R_OTG_FS_DIEPTXF3  reg32(0x5000010C)

/* OTG_FS host mode registers */
#define R_OTG_FS_HCFG      reg32(0x50000400)
#define R_OTG_FS_HFIR      reg32(0x50000404)
#define R_OTG_FS_HFNUM     reg32(0x50000408)
#define R_OTG_FS_HPTXSTS   reg32(0x50000410)
#define R_OTG_FS_HAINT     reg32(0x50000414)
#define R_OTG_FS_HAINTMSK  reg32(0x50000418)
#define R_OTG_FS_HPRT      reg32(0x50000440)
#define R_OTG_FS_HCCHAR(x)   reg32(0x50000500 + ((x) * 0x20))
#define R_OTG_FS_HCINT(x)    reg32(0x50000508 + ((x) * 0x20))
#define R_OTG_FS_HCINTMSK(x) reg32(0x5000050C + ((x) * 0x20))
#define R_OTG_FS_HCTSIZ(x)   reg32(0x50000510 + ((x) * 0x20))

/* OTG_FS device mode registers */
#define R_OTG_FS_DCFG      reg32(0x50000800)
#define R_OTG_FS_DCTL      reg32(0x50000804)
#define R_OTG_FS_DSTS      reg32(0x50000808)
#define R_OTG_FS_DIEPMSK   reg32(0x50000810)
#define R_OTG_FS_DOEPMSK   reg32(0x50000814)
#define R_OTG_FS_DAINT     reg32(0x50000818)
#define R_OTG_FS_DAINTMSK  reg32(0x5000081C)
#define R_OTG_FS_DVBUSDIS  reg32(0x50000828)
#define R_OTG_FS_DVBUSPULSE   reg32(0x5000082C)
#define R_OTG_FS_DIEPEMPMSK   reg32(0x50000834)
#define R_OTG_FS_DIEPCTL(x)   reg32(0x50000900 + (x) * 0x20)
#define R_OTG_FS_DIEPINT(x)   reg32(0x50000908 + (x) * 0x20)
#define R_OTG_FS_DIEPTSIZ0(x) reg32(0x50000910 + (x) * 0x20)
#define R_OTG_FS_DTXFSTS(x)   reg32(0x50000918 + (x) * 0x20)
#define R_OTG_FS_DOEPCTL(x)   reg32(0x50000B00 + (x) * 0x20)
#define R_OTG_FS_DOEPINT(x)   reg32(0x50000B08 + (x) * 0x20)
#define R_OTG_FS_DOEPTSIZ0(x) reg32(0x50000B10 + (x) * 0x20)

/* OTG_FS power and clock gating registers */
#define R_OTG_FS_PCGCCTL   reg32(0x50000E00)

/* DFIFO push/pop registers */
#define R_OTG_FS_DFIFO(x)  reg32(0x50001000 + (x) * 0x1000)

#endif /* _REG_H_ */
