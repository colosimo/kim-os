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

extern void init(void);

void attr_weak isr_hf(void)
{
	crt("Hard Fault\n");
	while(1);
}

void attr_weak isr_none(void)
{
	crt("Unhandled ISR\n");
	while (1);
}

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

void attr_weak sleep(void)
{
	asm("wfi");
}

/* System ticks */

static uint32_t ticks = 0;

void attr_weak isr_systick(void)
{
	ticks++;
}

u32 attr_weak k_ticks(void)
{
	return ticks;
}
