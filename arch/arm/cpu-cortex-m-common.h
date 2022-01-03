/*
 * Author: Aurelio Colosimo, 2019
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

#ifndef _CPU_CORTEX_M_COMMON_H_
#define _CPU_CORTEX_M_COMMON_H_

/* System Control Block registers */
#define R_SCB_CPUID   reg32(0xE000ED00)
#define R_SCB_ICSR    reg32(0xE000ED04)
#define R_SCB_ACTLR   reg32(0xE000ED08)
#define R_SCB_VTOR    reg32(0xE000ED08)
#define R_SCB_AIRCR   reg32(0xE000ED0C)
#define R_SCB_SCR     reg32(0xE000ED10)
#define R_SCB_CCR     reg32(0xE000ED14)
#define R_SCB_SHPR1   reg32(0xE000ED18)
#define R_SCB_SHPR2   reg32(0xE000ED1C)
#define R_SCB_SHPR3   reg32(0xE000ED20)
#define R_SCB_SHCRS   reg32(0xE000ED24)
#define R_SCB_CFSR    reg32(0xE000ED28)
#define R_SCB_MMSR    reg8(0xE000ED28)
#define R_SCB_BFSR    reg8(0xE000ED29)
#define R_SCB_UFSR    reg32(0xE000ED2A)
#define R_SCB_HFSR    reg32(0xE000ED2C)
#define R_SCB_MMFAR   reg32(0xE000ED34)
#define R_SCB_BFAR    reg32(0xE000ED38)
#define R_SCB_AFSR    reg32(0xE000ED3C)

/* SysTick registers */
#define R_SYST_CSR    reg32(0xE000E010)
#define R_SYST_RVR    reg32(0xE000E014)
#define R_SYST_CVR    reg32(0xE000E018)
#define R_SYST_CALIB  reg32(0xE000E01C)

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

#define attr_isrv_sys attr_sect("isrv_sys")
#define attr_isrv_irq attr_sect("isrv_irq")

static inline void cpu_reset()
{
	wr32(R_SCB_AIRCR, (0x05fa << 16) | BIT2);
}

/* ISR related common functions */

void attr_weak isr_hf(void);

void attr_weak isr_none(void);

void isr_reset(void);

void sleep(void);

void attr_weak isr_systick(void);

u32 attr_weak k_ticks(void);

#endif /* _CPU_CORTEX_M_COMMON_H_ */
