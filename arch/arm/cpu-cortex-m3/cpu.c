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
#include <cpu-cortex-m-common.h>

/* FIXME: Stack top depends on soc, so it should be moved externally */
#define STACK_TOP ((void*)(0x20005000))

static const void *attr_isrv_sys _isrv_sys[] = {
	/* Cortex-M3 system interrupts */
	STACK_TOP,	/* Stack top */
	isr_reset,	/* Reset */
	isr_none,	/* NMI */
	isr_none,	/* Hard Fault */
	isr_none,	/* Memory management fault */
	isr_none,	/* Bus fault */
	0,			/* Usage fault */
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
