/*
 * Author: Aurelio Colosimo, 2019
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

/* STM32F103 / Nucleo-F103RB board */

#include <reg.h>
#include <linker.h>
#include <basic.h>
#include <kim.h>
#include <log.h>

int putchar(int c)
{
	/* FIXME implement putchar */
	return 0;
}

void attr_weak board_init(unsigned *cpu_freq)
{
	/* FIXME Init clock and assign cpu_freq */
	/* FIXME Init uart */

	log("%s done\n", __func__);
}
