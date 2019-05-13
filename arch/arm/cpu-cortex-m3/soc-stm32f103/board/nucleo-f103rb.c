/*
 * Author: Aurelio Colosimo, 2019
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

/* STM32F103 / Nucleo-F103RB board */

#include <cpu.h>
#include <reg.h>
#include <linker.h>
#include <basic.h>
#include <kim.h>
#include <log.h>
#include <gpio.h>

#define XTAL_FREQ 8000000
#define CPU_FREQ 48000000

int putchar(int c)
{
	/* Write byte to tx register (TD) */
	wr32(R_USART1_DR, c);
	/* Wait for data sent (DR becomes empty) */
	while (!(rd32(R_USART1_SR) & BIT7));
	return 0;
}

void board_init(u32 *cpu_freq)
{
	/* FIXME TODO */
	dbg("%s done\n", __func__);
	/* FIXME ACR must consider also 48MHz < freq < 72MHz */
}
