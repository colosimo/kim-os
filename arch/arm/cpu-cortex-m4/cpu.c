/*
 * Author: Aurelio Colosimo, 2019
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

#include <kim.h>
#include <basic.h>
#include <linker.h>
#include <cpu.h>
#include <log.h>
#include <reg.h>

static uint32_t ticks = 0;

extern void init(void);

void isr_reset(void)
{
	unsigned char *src, *dest;

	/* Load data to ram */
	src = &__start_data_flash;
	dest = &__start_data_sram;
	while (dest != &__end_data_sram)
			*dest++ = *src++;

	/* Set bss section to 0 */
	dest = &__start_bss;
	while (dest != &__end_bss)
			*dest++ = 0;

	/* Skip to mach or board specific init */
	init();
}

static uint32_t ipsr(void)
{
  uint32_t res;
  __asm volatile ("mrs %0, ipsr" : "=r" (res));
  return res;
}

void attr_weak isr_hf(void)
{
	crt("Hard Fault\n");
	while(1);
}

void attr_weak isr_none(void)
{
	crt("Unhandled IPSR=%x ISPR=%x\n", (uint)ipsr(), (uint)rd32(R_NVIC_ISPR(0)));
	while(1);
}

void attr_weak isr_systick(void)
{
	ticks++;
}

static const void *attr_isrv_sys _isrv_sys[] = {
	/* Cortex-M3 system interrupts */
	STACK_TOP,	/* Stack top */
	isr_reset,	/* Reset */
	isr_none,	/* NMI */
	isr_hf,		/* Hard Fault */
	isr_none,	/* Memory management fault */
	isr_none,	/* Bus fault */
	isr_none,	/* Usage fault */
	0,			/* Reserved */
	0,			/* Reserved */
	0,			/* Reserved */
	0,			/* Reserved */
	isr_none,	/* SVC */
	0,			/* Reserved */
	0,			/* Reserved */
	isr_none,	/* PendSV */
	isr_systick,	/* SysTick */
};

u32 attr_weak k_ticks(void)
{
	return ticks;
}

void sleep(void)
{
	asm("wfi");
}
