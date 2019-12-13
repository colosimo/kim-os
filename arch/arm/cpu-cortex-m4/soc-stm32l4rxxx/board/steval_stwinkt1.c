/*
 * Author: Aurelio Colosimo, 2019
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

/* STEVAL-STWIKT1 board */

#include <cpu.h>
#include <reg.h>
#include <linker.h>
#include <basic.h>
#include <kim.h>
#include <kim-io.h>
#include <kim-io-defs.h>
#include <log.h>
#include <gpio.h>
#include <uart.h>

#define DEFAULT_UART_BR 115200

void sleep(void)
{
	/* Redefining empty sleep (no wfi call) because in this board, linked
	 * to stlink-v3, it seems wfi interferes with correct communication
	 * over swd for debug and flash operations */
}

int putchar(int c)
{
	/* Write byte to tx register (DR) */
	wr32(R_USART3_TDR, c);
	/* Wait for data sent (TC bit in ISR register becomes 1) */
	while (!(rd32(R_USART3_ISR) & BIT6));
	return 0;
}


void board_init(u32 *cpu_freq, u32 *ahb_freq, u32 *apb_freq)
{
	/* Enable HSE (16MHz external oscillator) */
	or32(R_RCC_CR, BIT16);
	while (!(rd32(R_RCC_CR) & BIT17));

	/* PLL configuration: output < 120MHz; xtal is 16MHz
	 * HSE: 16MHz
	 * PLLM: 4,  VCO_IN:   4MHz
	 * PLLN=60, VCO_OUT: 240MHz
	 * PLLR=4 (0b00), PLLCLK: 60MHz */
	and32(R_RCC_PLLCFGR, ~0xff737ff3);
	or32(R_RCC_PLLCFGR, (30 << 8) | ((4 - 1) << 4) | 0b11);
	or32(R_RCC_PLLCFGR, BIT24);
	or32(R_RCC_CR, BIT24);
	while (!(rd32(R_RCC_CR) & BIT25));

	/* Flash latency */
	and32(R_FLASH_ACR, ~0b1111);
	or32(R_FLASH_ACR, 0b1111);
	while ((rd32(R_FLASH_ACR) & 0b1111) != 0b1111);


	/* Use PLL as system clock, with AHB prescaler set to 2 */
	wr32(R_RCC_CFGR, (0b1000 << 4));
	or32(R_RCC_CFGR, 0b11);
	while (((rd32(R_RCC_CFGR) >> 2) & 0b11) != 0b11);

	*cpu_freq = *apb_freq = *ahb_freq = 30000000;

	/* Enable desired peripherals */
	or32(R_RCC_APB1ENR1, BIT28 | BIT18 | BIT14); /* USART3, PWR and SPI2 */
	or32(R_RCC_AHB2ENR, 0x1ff); /* All GPIOs */

	or32(R_PWR_CR2, BIT9); /* Enable VDDIO_2, for PG[15..2] GPIOS */

	/* Configure USART3 on PD8 and PD9 at 115.2kbps */
	gpio_func(IO(PORTD, 8), 7);
	gpio_func(IO(PORTD, 9), 7);

	/* Set baudrate to DEFAULT_UART_BR and enable USART3 */
	wr32(R_USART3_BRR, *apb_freq / DEFAULT_UART_BR);
	or32(R_USART3_CR1, BIT5 | BIT3 | BIT2 | BIT0);
	or32(R_NVIC_ISER(1), BIT7); /* USART3 is irq 39 */

	/* Enable USART3 TX and RX on STMOD2 and STMOD3:
	 * PG0 must be held HIGH */
	gpio_dir(IO(PORTG, 0), 1);
	gpio_wr(IO(PORTG, 0), 1);

	dbg("%s done\n", __func__);
}

declare_dev(MAJ_SOC_UART, MINOR_UART3, NULL, uart3);
