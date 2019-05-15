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
#define CPU_FREQ 72000000

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
	/* Enable HSE (8MHz external oscillator) */
	or32(R_RCC_CR, BIT16);
	while (!(rd32(R_RCC_CR) & BIT17));

	/* Configure PLL */
	and32(R_RCC_CFGR, ~((0b1111 << 18) | BIT16 | BIT17));
	or32(R_RCC_CFGR, (((CPU_FREQ / XTAL_FREQ) - 2) << 18) | BIT16);
	or32(R_RCC_CR, BIT24);
	while (!(rd32(R_RCC_CR) & BIT25));

	and32(R_FLASH_ACR, ~(0b111));

	/* Configure flash latency */
	while(rd32(R_FLASH_SR) & BIT0);
	if (CPU_FREQ > 24000000)
		or32(R_FLASH_ACR, 0b001);
	else if (CPU_FREQ > 48000000)
		or32(R_FLASH_ACR, 0b010);

	/* Use PLL as system clock */
	or32(R_RCC_CFGR, 0b10);
	while (((rd32(R_RCC_CFGR) >> 2) & 0x3) != 0b10);

	*cpu_freq = CPU_FREQ;

	or32(R_RCC_APB1ENR, BIT17 | BIT18 | BIT19 | BIT20); /* USART2/3/4/5 */
	or32(R_RCC_APB2ENR, BIT14 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7 | BIT8); 	/* USART1 and all GPIOs*/

	/* This initializes USART2 on PA2/PA3 for debug purpose
	   NOTE: need to solder 0 Ohm on SB62/63 pads and unsolder on SB13/14 */
	gpio_func(IO(PORTA, 2), 1);
	gpio_func(IO(PORTA, 3), 1);
	gpio_mode(IO(PORTA, 2), PULL_NO);
	gpio_mode(IO(PORTA, 3), PULL_UP);

	/* This initializes USART1 on PA9/PA10 for data */
	gpio_func(IO(PORTA, 9), 1);
	gpio_func(IO(PORTA, 10), 1);
	gpio_mode(IO(PORTA, 9), PULL_NO);
	gpio_mode(IO(PORTA, 10), PULL_UP);

	/* Init USART1 and USART2 at 115kbps:
	 * fPCLK=72MHz, br=115.2KBps, BRR=39.0625, see table 192 pag. 799 */
	wr32(R_USART2_BRR, (39 << 4) | 1);
	or32(R_USART2_CR1, BIT13 | BIT5 | BIT3 | BIT2);

	wr32(R_USART1_BRR, (39 << 4) | 1);
	or32(R_USART1_CR1, BIT13 | BIT5 | BIT3 | BIT2);

	dbg("%s done\n", __func__);
}
