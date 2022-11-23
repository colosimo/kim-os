/*
 * Author: Aurelio Colosimo, 2022
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

#define I2C_FREQ 50000
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
/* 	u32 ccr; */

	/* Enable HSI (8MHz internal oscillator) */
	or32(R_RCC_CR, BIT0);
	while (!(rd32(R_RCC_CR) & BIT1));

	*cpu_freq = *apb_freq = *ahb_freq = 16000000;

	and32(R_FLASH_OPTCR, ~0b1100);
	or32(R_FLASH_OPTCR, 0b1000);

	/* Flash latency */
	or32(R_FLASH_ACR, 0b111);

	or32(R_RCC_AHB1ENR, BIT7 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0); /* All GPIOs */
	or32(R_RCC_APB2ENR, BIT14 | BIT8 | BIT4); /* USART1, ADC1, SYSCFG */
	or32(R_RCC_APB1ENR, BIT21 | BIT17 | BIT1 | BIT0); /* USART2, PWR, I2C1, TIM3, TIM2 */

	/* USART2 on PA2/PA3 */
	gpio_func(IO(PORTA, 2), 7);
	gpio_func(IO(PORTA, 3), 7);
	gpio_mode(IO(PORTA, 2), PULL_NO);
	gpio_mode(IO(PORTA, 3), PULL_NO);

	wr32(R_USART2_BRR, ((*apb_freq / 16) << 4) / UART_BAUDRATE);
	or32(R_USART2_CR1, BIT13 | BIT5 | BIT3 | BIT2);
	or32(R_NVIC_ISER(1), BIT6); /* USART2 is irq 38 */

	uart_init();

#if 0
	/* Configure I2C1 pins: SDA on PB7, SCL on PB6 (AF4). */
	gpio_func(IO(PORTB, 6), 4);
	gpio_func(IO(PORTB, 7), 4);

	/* Perform a reset on I2C to clear BUSY bit due to a glitch, if any */
	or32(R_I2C1_CR1, BIT15);
	and32(R_I2C1_CR1, ~BIT15);

	wr32(R_I2C1_CR2, *apb_freq / MHZ);
	if (I2C_FREQ < 100 * KHZ)
		ccr = *apb_freq / I2C_FREQ / 2;
	else
		ccr = BIT15 | (*apb_freq / I2C_FREQ / 3);
	wr32(R_I2C1_CCR, ccr);
	wr32(R_I2C1_TRISE, *apb_freq / MHZ + 1);
	wr32(R_I2C1_FLT, 0b0111);
	or32(R_I2C1_CR1, BIT0);

	gpio_odrain(IO(PORTB, 6), 1);
	gpio_odrain(IO(PORTB, 7), 1);
	gpio_dir(IO(PORTB, 10), 1);
#endif
	or32(R_GPIOA_MODER, 0b11 << 0); /* PA0 analog mode */

}

declare_gpio_dev(0, IO(PORTA, 10), DIR_IN, PULL_DOWN, gpio1);
declare_gpio_dev(1, IO(PORTA, 11), DIR_IN, PULL_DOWN, gpio2);

declare_gpio_dev(2, IO(PORTC, 11), DIR_OUT, PULL_NO, ld1);
declare_gpio_dev(3, IO(PORTC, 12), DIR_OUT, PULL_NO, ld2);
declare_gpio_dev(4, IO(PORTA, 15), DIR_OUT, PULL_NO, ld3);
declare_gpio_dev(5, IO(PORTC, 10), DIR_OUT, PULL_NO, ld4);

declare_gpio_dev(6, IO(PORTC, 9), DIR_IN, PULL_NO, sw1);
declare_gpio_dev(7, IO(PORTC, 8), DIR_IN, PULL_NO, sw2);
declare_gpio_dev(8, IO(PORTC, 7), DIR_IN, PULL_NO, sw3);
declare_gpio_dev(9, IO(PORTC, 6), DIR_IN, PULL_NO, sw4);

declare_gpio_dev(10, IO(PORTA, 9), DIR_OUT, PULL_NO, pwm);
declare_gpio_dev(11, IO(PORTB, 0), DIR_OUT, PULL_NO, pwm_n);

#if 0
declare_dev(MAJ_SOC_I2C, MINOR_I2C1, NULL, i2c1);
#endif

const k_dev_t attr_devs uart2_dev = {
	.id = dev_id(MAJ_SOC_UART, MINOR_UART2),
	.name = "uart2",
};
