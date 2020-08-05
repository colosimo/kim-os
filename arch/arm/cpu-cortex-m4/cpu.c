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
#include <reg.h>
#include <log.h>

static const void *attr_isrv_sys _isrv_sys[] = {
	/* Cortex-M4 system interrupts */
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

void attr_naked isr_hf(void)
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

void isr_none(void)
{
	u32 icsr = rd32(R_SCB_ICSR);
	crt("Unhandled ICSR %08x, (IRQ %d)\n", (uint)icsr,
	    ((int)icsr & 0x1ff) - 16);
	isr_hf();
	while(1);
}
