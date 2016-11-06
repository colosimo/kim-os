/*
 * Author: Aurelio Colosimo, 2016
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

/* LPCXpresso LPC1115 Rev A board (OM13035) */

#include <reg.h>
#include <linker.h>
#include <basic.h>

void attr_weak board_init()
{
	/* Init UART on TXD and RXD pins */
	or32(R_SYSAHBCLKCTRL, BIT16);
	and32(R_IOCON_PIO1_6, ~(BIT0 | BIT1 | BIT2));
	or32(R_IOCON_PIO1_6, 0x1);
	and32(R_IOCON_PIO1_7, ~(BIT0 | BIT1 | BIT2));
	or32(R_IOCON_PIO1_7, 0x1);

	or32(R_SYSAHBCLKCTRL, BIT12);
	or32(R_UARTCLKDIV, 1);
	or32(R_U0LCR, BIT7);
	wr32(R_U0FDR, (2 << 4) + 1);
	wr32(R_U0DLL, 52);
	wr32(R_U0DLM, 0);
	and32(R_U0LCR, ~BIT7);
	or32(R_U0LCR, BIT1 | BIT0);
}
