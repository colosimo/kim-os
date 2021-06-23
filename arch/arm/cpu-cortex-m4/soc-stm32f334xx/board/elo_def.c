/*
 * Author: Aurelio Colosimo, 2019
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

#define UART_BAUDRATE 115200

int putchar(int c)
{
	/* Write byte to tx register (DR) */
	wr32(R_USART1_TDR, c);
	/* Wait for data sent (ISR bit7 cleared) */
	while (!(rd32(R_USART1_ISR) & BIT7));
	return 0;
}

void board_init(u32 *cpu_freq, u32 *ahb_freq, u32 *apb_freq)
{
	uint date;
	uint time;
	const char *days[7] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
	/* Enable HSI (8MHz internal oscillator) */
	or32(R_RCC_CR, BIT16);
	while (!(rd32(R_RCC_CR) & BIT1));

	/* PLLMUL=0110b */
	wr32(R_RCC_CFGR, (0b1111 << 18));

	/* Enable PLL */
	or32(R_RCC_CR, BIT24);
	while (!(rd32(R_RCC_CR) & BIT25));

	/* Flash latency */
	or32(R_FLASH_ACR, 0b010);

	/* Use PLL as system clock */
	wr32(R_RCC_CFGR, 0b10);
	while (((rd32(R_RCC_CFGR) >> 2) & 0x3) != 0b10);

	*cpu_freq = *apb_freq = *ahb_freq = 64000000;

	or32(R_RCC_AHBENR, BIT22 | BIT20 | BIT19 | BIT18 | BIT17); /* All GPIOs */
	or32(R_RCC_APB2ENR, BIT14 | BIT0); /* USART1, SYSCFG */
	or32(R_RCC_APB1ENR, BIT28 | BIT21 | BIT1 | BIT0); /* PWR, I2C1, TIM3, TIM2 */

	/* USART1 on PC4/PC5 */
	gpio_func(IO(PORTC, 4), 7);
	gpio_func(IO(PORTC, 5), 7);
	gpio_mode(IO(PORTC, 4), PULL_NO);
	gpio_mode(IO(PORTC, 5), PULL_NO);
	wr32(R_USART1_BRR, *apb_freq / UART_BAUDRATE);
	or32(R_USART1_CR1, BIT5 | BIT3 | BIT2 | BIT0);
	or32(R_NVIC_ISER(1), BIT5); /* USART1 is irq 37 */
	uart_init(0);

	/* I2C1 on PB8/PB9 */

	gpio_func(IO(PORTB, 8), 4);
	gpio_func(IO(PORTB, 9), 4);
	gpio_odrain(IO(PORTB, 8), 1);
	gpio_odrain(IO(PORTB, 9), 1);

	/* Perform a reset on I2C to clear BUSY bit due to a glitch, if any */

	or32(R_I2C1_CR1, BIT15);
	and32(R_I2C1_CR1, ~BIT15);
	and32(R_I2C1_CR1, ~0b1);
	or32(R_RCC_CFGR3, BIT4);
	/* PRESC = 3, SCLL=0x13, SCLH=0xf, SDADEL=0x2, SCLDEL=0x4 */
	wr32(R_I2C1_TIMINGR, (3 << 28) | (4 << 20) | (2 << 16) | (0xf << 8) | 0x13);
	or32(R_I2C1_CR1, 0b1);

	/* RTC initialization */
	or32(R_PWR_CR, BIT8);

	wr32(R_SYSCFG_EXTICR4, 0b001 << 8); /* PB14 on EXTI */

//#define LSE_PRESENT /* In v5.1 only */
#ifdef LSE_PRESENT
	if (((rd32(R_RCC_BDCR) >> 8) & 0b11) != 0b01)
		wr32(R_RCC_BDCR, BIT16);

	wr32(R_RCC_BDCR, BIT0);
	while (!(rd32(R_RCC_BDCR) & 0b10));
	or32(R_RCC_BDCR, BIT15 | BIT8);
#else
	wr32(R_RCC_CSR, BIT0);
	while (!(rd32(R_RCC_CSR) & 0b10));
	or32(R_RCC_BDCR, BIT15 | BIT9);

	wr32(R_RTC_WPR, 0xca);
	wr32(R_RTC_WPR, 0x53);

	or32(R_RTC_ISR, BIT7);
	while (!(rd32(R_RTC_ISR) & BIT6));
	wr32(R_RTC_PRER, (127 << 16) | 311);
	and32(R_RTC_ISR, ~BIT7);

	wr32(R_RTC_WPR, 0xff);

#endif

	date = rd32(R_RTC_DR);
	time = rd32(R_RTC_TR);
	log("%s done, date is %s %d%d-%d%d-%d%d %d%d:%d%d:%d%d\n", __func__,
		days[((date >> 13) & 0x7) - 1],
	    (date >> 20) & 0xf, (date >> 16) & 0xf, (date >> 12) & 0x1, (date >> 8) & 0xf,
		(date >> 4) & 0x3, date & 0xf,
	    (time >> 20) & 0x3, (time >> 16) & 0xf, (time >> 12) & 0x7, (time >> 8) & 0xf,
	    (time >> 4) & 0x7, time & 0xf);
}

declare_gpio_dev(0, IO(PORTC, 1), DIR_OUT, PULL_NO, user_led_1);
declare_gpio_dev(1, IO(PORTC, 15), DIR_OUT, PULL_NO, user_led_2);
declare_gpio_dev(2, IO(PORTB, 13), DIR_IN, PULL_NO, ant_check);
declare_gpio_dev(3, IO(PORTB, 12), DIR_OUT, PULL_NO, alarm_out);
declare_gpio_dev(4, IO(PORTB, 1), DIR_IN, PULL_DOWN, sw_up);
declare_gpio_dev(5, IO(PORTB, 2), DIR_IN, PULL_DOWN, sw_down);
declare_gpio_dev(6, IO(PORTB, 10), DIR_IN, PULL_DOWN, sw_esc);
declare_gpio_dev(7, IO(PORTB, 11), DIR_IN, PULL_DOWN, sw_enter);
declare_gpio_dev(8, IO(PORTA, 11), DIR_OUT, PULL_NO, lcd_backlight);
declare_gpio_dev(9, IO(PORTB, 14), DIR_IN, PULL_NO, rf_rx);

declare_gpio_dev(10, IO(PORTB, 6), DIR_OUT, PULL_NO, db0);
declare_gpio_dev(11, IO(PORTB, 5), DIR_OUT, PULL_NO, db1);
declare_gpio_dev(12, IO(PORTB, 4), DIR_OUT, PULL_NO, db2);
declare_gpio_dev(13, IO(PORTB, 3), DIR_OUT, PULL_NO, db3);
declare_gpio_dev(14, IO(PORTD, 2), DIR_OUT, PULL_NO, db4);
declare_gpio_dev(15, IO(PORTC, 12), DIR_OUT, PULL_NO, db5);
declare_gpio_dev(16, IO(PORTC, 11), DIR_OUT, PULL_NO, db6);
declare_gpio_dev(17, IO(PORTC, 10), DIR_OUT, PULL_NO, db7);
declare_gpio_dev(18, IO(PORTC, 13), DIR_OUT, PULL_NO, e);
declare_gpio_dev(19, IO(PORTC, 14), DIR_OUT, PULL_NO, rs);
declare_gpio_dev(20, IO(PORTB, 7), DIR_OUT, PULL_NO, rw);

declare_dev(MAJ_SOC_I2C, MINOR_I2C1, NULL, i2c1);

const k_dev_t attr_devs uart1_dev = {
	.id = dev_id(MAJ_SOC_UART, MINOR_UART1),
	.name = "uart1",
};
