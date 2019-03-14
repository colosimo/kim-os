/*
 * Author: Aurelio Colosimo, 2016
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

#include <kim.h>
#include <cpu.h>
#include <reg.h>
#include <log.h>

#define SYSTICKS_FREQ 100

extern void board_init();

extern void isr_none(void);
extern void isr_uart(void) {}

static const void *attr_isrv_irq _isrv[] = {

	/* LPC111x peripheral IRQs */
	isr_none,	/* PIO0_0 */
	isr_none,	/* PIO0_1 */
	isr_none,	/* PIO0_2 */
	isr_none,	/* PIO0_3 */
	isr_none,	/* PIO0_4 */
	isr_none,	/* PIO0_5 */
	isr_none,	/* PIO0_6 */
	isr_none,	/* PIO0_7 */
	isr_none,	/* PIO0_8 */
	isr_none,	/* PIO0_9 */
	isr_none,	/* PIO0_10 */
	isr_none,	/* PIO0_11 */
	isr_none,	/* PIO1_0 */
	isr_none,	/* CCAN */
	isr_none,	/* SPI1 */
	isr_none,	/* I2C */
	isr_none,	/* CT16B0 */
	isr_none,	/* CT16B1 */
	isr_none,	/* CT32B0 */
	isr_none,	/* CT32B1 */
	isr_none,	/* SPI0 */
	isr_uart,	/* UART */
	0,			/* Reserved */
	0,			/* Reserved */
	isr_none,	/* ADC */
	isr_none,	/* WDT */
	isr_none,	/* BOD */
	0,			/* Reserved */
	isr_none,	/* PIO_3 */
	isr_none,	/* PIO_2 */
	isr_none,	/* PIO_1 */
	isr_none,	/* PIO_0 */
};

int putchar(int c)
{
	if (c == '\n')
		putchar('\r');
	wr32(R_U0THR, c);
	while (!(rd32(R_U0LSR) & BIT5));
	return 0;
}

u32 attr_weak k_ticks_freq(void) /* FIXME dummy */
{
	return SYSTICKS_FREQ;
}

void attr_used init(void)
{
	unsigned cpu_freq;

	/* Init board */
	board_init(&cpu_freq);

	/* Init system ticks */
	wr32(R_SYST_RVR, cpu_freq / SYSTICKS_FREQ);
	wr32(R_SYST_CVR, 0);
	wr32(R_SYST_CSR, BIT0 | BIT1 | BIT2);

	log("Init done, launching k_main\n");

	/* Skip to main */
	k_main();
}
