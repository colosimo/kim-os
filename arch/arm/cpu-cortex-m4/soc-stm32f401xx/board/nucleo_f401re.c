/*
 * Author: Aurelio Colosimo, 2023
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

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
#include <i2c.h>

#define I2C_FREQ 400000
#define UART_BAUDRATE 115200

int putchar(int c)
{
	/* Write byte to tx register (DR) */
	wr32(R_USART2_DR, c);
	/* Wait for data sent (ISR bit7 cleared) */
	while (!(rd32(R_USART2_SR) & BIT7));
	return 0;
}

void board_init(u32 *cpu_freq, u32 *ahb_freq, u32 *apb_freq)
{
	/* Enable HSI (8MHz internal oscillator) */
	or32(R_RCC_CR, BIT0);
	while (!(rd32(R_RCC_CR) & BIT1));

	*cpu_freq = *apb_freq = *ahb_freq = 16000000;

	and32(R_FLASH_OPTCR, ~0b1100);
	or32(R_FLASH_OPTCR, 0b1000);

	/* Flash latency */
	or32(R_FLASH_ACR, 0b111);

	or32(R_RCC_AHB1ENR, BIT7 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0); /* All GPIOs */
	or32(R_RCC_APB2ENR, BIT14 | BIT4); /* SYSCFG, USART1 */
	or32(R_RCC_APB1ENR, BIT28 | BIT17 ); /* PWR, USART2 */

	/* USART2 on PA2/PA3 */
	gpio_func(IO(PORTA, 2), 7);
	gpio_func(IO(PORTA, 3), 7);
	gpio_mode(IO(PORTA, 2), PULL_NO);
	gpio_mode(IO(PORTA, 3), PULL_UP);

	wr32(R_USART2_BRR, ((*apb_freq / 16) << 4) / UART_BAUDRATE);
	or32(R_USART2_CR1, BIT13 | BIT5 | BIT3 | BIT2);
	or32(R_NVIC_ISER(1), BIT6); /* USART2 is irq 38 */

	uart_init();
}

declare_dev(MAJ_SOC_I2C, MINOR_I2C1, NULL, i2c1);
declare_dev(MAJ_SOC_I2C, MINOR_I2C3, NULL, i2c3);

const k_dev_t attr_devs uart2_dev = {
	.id = dev_id(MAJ_SOC_UART, MINOR_UART2),
	.name = "uart2",
};
