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

void dump_stack(uint32_t *stack)
{
	volatile uint32_t r0;
	volatile uint32_t r1;
	volatile uint32_t r2;
	volatile uint32_t r3;
	volatile uint32_t r12;
	volatile uint32_t lr; /* link register */
	volatile uint32_t pc; /* program counter */
	volatile uint32_t psr;/* program status register */

	r0 = stack[0];
	r1 = stack[1];
	r2 = stack[2];
	r3 = stack[3];
	r12 = stack[4];
	lr = stack[5];
	pc = stack[6];
	psr = stack[7];

	crt("oops\n");
	crt("r0=%08x\n", (uint)r0);
	crt("r1=%08x\n", (uint)r1);
	crt("r2=%08x\n", (uint)r2);
	crt("r3=%08x\n", (uint)r3);
	crt("r12=%08x\n", (uint)r12);
	crt("lr=%08x\n", (uint)lr);
	crt("pc=%08x\n", (uint)pc);
	crt("psr=%08x\n", (uint)psr);

	while (1);
}

void attr_weak attr_naked isr_hf(void)
{
	__asm volatile (
		"tst lr, #4\n"
		"ite eq\n"
		"mrseq r0, msp\n"
		"mrsne r0, psp\n"
		"ldr r1, [r0, #24]\n"
		"ldr r2, addr_const\n"
		"bx r2\n"
		"addr_const: .word dump_stack\n"
	);
}

void attr_weak isr_none(void)
{
	crt("Panic: unhandled ISR\n");
	isr_hf();
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
