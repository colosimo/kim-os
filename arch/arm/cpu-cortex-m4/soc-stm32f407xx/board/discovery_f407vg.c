/*
 * Author: Aurelio Colosimo, 2019
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

/* STM32F407VG / STM32F407G-DISC1 board */

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
#include <usb.h>

#define UART_BAUDRATE 115200

/* NOTE on clock setting for discovery-f407vg board
 * Crystal is 8MHz. PLL is configured to provide a 168MHz clock. This clock
 * goes into AHB prescaler, set to 4, and then to APB (with no further
 * prescaler). Peripheral clock is thus 42MHz.
 * The PLL clock also provides the 48MHz USB */

int putchar(int c)
{
	/* Write byte to tx register (DR) */
	wr32(R_USART2_DR, c);
	/* Wait for data sent (DR becomes empty) */
	while (!(rd32(R_USART2_SR) & BIT7));
	return 0;
}

void board_init(u32 *cpu_freq, u32 *ahb_freq, u32 *apb_freq)
{
	/* Enable HSE (8MHz external oscillator) */
	or32(R_RCC_CR, BIT16);
	while (!(rd32(R_RCC_CR) & BIT17));

	/* PLLM=8 PLLN=336, PLLP=00 (2), PLLQ=7; f_PLL=168MHz, f_USB=48MHz */
	and32(R_RCC_PLLCFGR, ~0x0f037fff);
	or32(R_RCC_PLLCFGR, BIT22 | (7 << 24) | (336 << 6) | 8);
	or32(R_RCC_CR, BIT24);
	while (!(rd32(R_RCC_CR) & BIT25));

	/* Flash latency */
	or32(R_FLASH_ACR, 0b111);

	/* Use PLL as system clock, with AHB prescaler set to 4 */
	wr32(R_RCC_CFGR, (0x9 << 4) | 0x2);
	while (((rd32(R_RCC_CFGR) >> 2) & 0x3) != 0x2);

	*cpu_freq = 168000000;
	*apb_freq = *ahb_freq = 42000000;

	or32(R_RCC_AHB1ENR, BIT2 | BIT0); /* GPIOC, GPIOA */
	or32(R_RCC_AHB2ENR, BIT7); /* OTGFS */
	or32(R_RCC_APB1ENR, BIT17); /* USART2 */

	/* USART2 on PA2/PA3 */
	gpio_func(IO(PORTA, 2), 7);
	gpio_func(IO(PORTA, 3), 7);
	gpio_mode(IO(PORTA, 2), PULL_NO);
	gpio_mode(IO(PORTA, 3), PULL_NO);

	wr32(R_USART2_BRR, ((*apb_freq / 16) << 4) / UART_BAUDRATE);
	or32(R_USART2_CR1, BIT13 | BIT5 | BIT3 | BIT2);
	or32(R_NVIC_ISER(1), BIT6); /* USART2 is irq 38 */

	/* USB on PA11/PA12 */
	gpio_func(IO(PORTA, 11), 10);
	gpio_func(IO(PORTA, 12), 10);

	gpio_dir(IO(PORTC, 0), 1);
	gpio_wr(IO(PORTC, 0), 0);

	usbfs_host_init();

	dbg("%s done\n", __func__);
}

const k_dev_t attr_devs uart2_dev = {
	.id = dev_id(MAJ_SOC_UART, MINOR_UART2),
	.name = "uart2",
};
