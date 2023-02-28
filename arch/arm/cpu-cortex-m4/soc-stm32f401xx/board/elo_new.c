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

#define I2C_FREQ 200000
#define UART_BAUDRATE 115200

int putchar(int c)
{
	/* Write byte to tx register (DR) */
	wr32(R_USART1_DR, c);
	/* Wait for data sent (ISR bit7 cleared) */
	while (!(rd32(R_USART1_SR) & BIT7));
	return 0;
}

void board_init(u32 *cpu_freq, u32 *ahb_freq, u32 *apb_freq)
{
	u32 ccr;
	uint date;
	uint time;
	//u32 pllcfgr;
	const char *days[7] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

	/* Enable HSI (8MHz internal oscillator) */
	or32(R_RCC_CR, BIT0);
	while (!(rd32(R_RCC_CR) & BIT1));

	*cpu_freq = *apb_freq = *ahb_freq = 16000000;

	and32(R_FLASH_OPTCR, ~0b1100);
	or32(R_FLASH_OPTCR, 0b1000);

	/* Flash latency */
	or32(R_FLASH_ACR, 0b111);

	or32(R_RCC_AHB1ENR, BIT7 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0); /* All GPIOs */
	or32(R_RCC_APB2ENR, BIT14 | BIT4); /* USART1, SYSCFG */
	or32(R_RCC_APB1ENR, BIT28 | BIT21 | BIT17 | BIT1 | BIT0); /* USART2, PWR, I2C1, TIM3, TIM2 */

	gpio_func(IO(PORTA, 9), 7);
	gpio_func(IO(PORTA, 10), 7);
	gpio_mode(IO(PORTA, 9), PULL_NO);
	gpio_mode(IO(PORTA, 10), PULL_NO);

	wr32(R_USART1_BRR, ((*apb_freq / 16) << 4) / UART_BAUDRATE);
	or32(R_USART1_CR1, BIT13 | BIT5 | BIT3 | BIT2);
	or32(R_NVIC_ISER(1), BIT5); /* USART1 is irq 37 */
#if 0
	/* USART2 on PA2/PA3 */
	gpio_func(IO(PORTA, 2), 7);
	gpio_func(IO(PORTA, 3), 7);
	gpio_mode(IO(PORTA, 2), PULL_NO);
	gpio_mode(IO(PORTA, 3), PULL_UP);

	wr32(R_USART2_BRR, ((*apb_freq / 16) << 4) / UART_BAUDRATE);
	or32(R_USART2_CR1, BIT13 | BIT5 | BIT3 | BIT2);
	or32(R_NVIC_ISER(1), BIT6); /* USART2 is irq 38 */
#endif

	uart_init();


	/* Configure I2C1 pins: SDA on PB9, SCL on PB8 (AF4). */
	gpio_func(IO(PORTB, 6), 4);
	//gpio_mode(IO(PORTB, 6), PULL_UP);

	gpio_func(IO(PORTB, 7), 4);
	//gpio_mode(IO(PORTB, 7), PULL_UP);

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

	/* RTC initialization */
	or32(R_PWR_CR, BIT8);
	wr32(R_SYSCFG_EXTICR4, 0b001 << 8); /* PB14 on EXTI */

	if (((rd32(R_RCC_BDCR) >> 8) & 0b11) != 0b01)
		wr32(R_RCC_BDCR, BIT16);

	wr32(R_RCC_BDCR, BIT0);
	while (!(rd32(R_RCC_BDCR) & 0b10));
	or32(R_RCC_BDCR, BIT15 | BIT8);

	date = rd32(R_RTC_DR);
	time = rd32(R_RTC_TR);
	log("%s done, date is %s %d%d-%d%d-%d%d %d%d:%d%d:%d%d\n", __func__,
		days[((date >> 13) & 0x7) - 1],
	    (date >> 20) & 0xf, (date >> 16) & 0xf, (date >> 12) & 0x1, (date >> 8) & 0xf,
		(date >> 4) & 0x3, date & 0xf,
	    (time >> 20) & 0x3, (time >> 16) & 0xf, (time >> 12) & 0x7, (time >> 8) & 0xf,
	    (time >> 4) & 0x7, time & 0xf);

	gpio_dir(IO(PORTC, 3), DIR_OUT);
	gpio_wr(IO(PORTC, 3), 1);

	gpio_dir(IO(PORTA, 5), DIR_OUT);
	gpio_wr(IO(PORTA, 5), 1);

	log("OK!\n");
}


declare_gpio_dev(0, IO(PORTC, 13), DIR_OUT, PULL_NO, user_led_1);
declare_gpio_dev(1, IO(PORTA, 5), DIR_OUT, PULL_NO, user_led_2);
declare_gpio_dev(2, IO(PORTC, 12), DIR_IN, PULL_NO, ant_check);
declare_gpio_dev(3, IO(PORTD, 2), DIR_OUT, PULL_NO, alarm_out);
#if 0
declare_gpio_dev(4, IO(PORTB, 1), DIR_IN, PULL_DOWN, sw_up);
declare_gpio_dev(5, IO(PORTB, 2), DIR_IN, PULL_DOWN, sw_down);
declare_gpio_dev(6, IO(PORTB, 10), DIR_IN, PULL_DOWN, sw_esc);
declare_gpio_dev(7, IO(PORTC, 6), DIR_IN, PULL_DOWN, sw_enter);
#endif

declare_gpio_dev(8, IO(PORTA, 6), DIR_OUT, PULL_NO, lcd_backlight);
declare_gpio_dev(9, IO(PORTB, 9), DIR_IN, PULL_NO, rf_rx);

declare_gpio_dev(10, IO(PORTB, 10), DIR_OUT, PULL_NO, db0);
declare_gpio_dev(11, IO(PORTC, 8), DIR_OUT, PULL_NO, db1);
declare_gpio_dev(12, IO(PORTC, 7), DIR_OUT, PULL_NO, db2);
declare_gpio_dev(13, IO(PORTC, 6), DIR_OUT, PULL_NO, db3);
declare_gpio_dev(14, IO(PORTB, 15), DIR_OUT, PULL_NO, db4);
declare_gpio_dev(15, IO(PORTB, 14), DIR_OUT, PULL_NO, db5);
declare_gpio_dev(16, IO(PORTB, 13), DIR_OUT, PULL_NO, db6);
declare_gpio_dev(17, IO(PORTB, 12), DIR_OUT, PULL_NO, db7);
declare_gpio_dev(18, IO(PORTC, 5), DIR_OUT, PULL_NO, e);
declare_gpio_dev(19, IO(PORTA, 7), DIR_OUT, PULL_NO, rs);
declare_gpio_dev(20, IO(PORTC, 4), DIR_OUT, PULL_NO, rw);

#if 0
declare_gpio_dev(21, IO(PORTA, 4), DIR_OUT, PULL_NO, at_cmd);
declare_gpio_dev(22, IO(PORTC, 3), DIR_OUT, PULL_NO, bt_reset);
#endif

declare_dev(MAJ_SOC_I2C, MINOR_I2C1, NULL, i2c1);

const k_dev_t attr_devs uart1_dev = {
	.id = dev_id(MAJ_SOC_UART, MINOR_UART1),
	.name = "uart1",
};
#if 0
const k_dev_t attr_devs uart2_dev = {
	.id = dev_id(MAJ_SOC_UART, MINOR_UART2),
	.name = "uart2",
};
#endif
