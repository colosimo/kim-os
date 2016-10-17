/*
 * Author: Aurelio Colosimo, 2016
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

#ifndef _CPU_H_
#define _CPU_H_

/* System Control Block registers */
#define R_SCB_CPUID   reg32(0xE000ED00)
#define R_SCB_ICSR    reg32(0xE000ED04)
#define R_SCB_AIRCR   reg32(0xE000ED0C)
#define R_SCB_SCR     reg32(0xE000ED10)
#define R_SCB_CCR     reg32(0xE000ED14)
#define R_SCB_SHPR2   reg32(0xE000ED1C)
#define R_SCB_SHPR3   reg32(0xE000ED20)

/* SysTick registers */
#define R_SYST_CSR    reg32(0xE000E010)
#define R_SYST_RVR    reg32(0xE000E014)
#define R_SYST_CVR    reg32(0xE000E018)
#define R_SYST_CALIB  reg32(0xE000E01C)

/* NVIC registers */
#define R_NVIC_ISER   reg32(0xE000E100)
#define R_NVIC_ICER   reg32(0xE000E180)
#define R_NVIC_ISPR   reg32(0xE000E200)
#define R_NVIC_IPR(x) reg32(0xE000E400 + 4 * (x))

/* disable interrupts */
static inline void cpsid(void)
{
  __asm volatile ("cpsid i");
}

/* enable interrupts */
static inline void cpsie(void)
{
  __asm volatile ("cpsie i");
}

#endif /* _CPU_H_ */
