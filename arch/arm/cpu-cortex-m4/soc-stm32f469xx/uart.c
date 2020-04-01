/*
 * Author: Aurelio Colosimo, 2020
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

#include <basic.h>
#include <log.h>
#include <errcode.h>
#include <cbuf.h>
#include <reg.h>
#include <kim.h>
#include <kim-io-defs.h>
#include <kim-io.h>
#include <cpu.h>
#include <uart.h>
#include <gpio.h>

#define UART_BUF_SIZE 96

#define uart_priv(fd) ((struct uart_data_t*)(devs(fd)->priv))

/* FIXME: ugly, better move uart_cbuf to priv field of uart devices
 * (define uart_data_t in uart.h) */
static struct cbuf_t uart_cbuf[5];
static u8 buf[5][UART_BUF_SIZE];

void isr_usart1(void)
{
	u8 c;
	while (rd32(R_USART1_SR) & (BIT5 | BIT3)) {
		c = rd32(R_USART1_DR);
		cbuf_write(&uart_cbuf[0], &c, 1);
	}
}

void isr_usart2(void)
{
	u8 c;
	while (rd32(R_USART2_SR) & (BIT5 | BIT3)) {
		c = rd32(R_USART2_DR);
		cbuf_write(&uart_cbuf[1], &c, 1);
	}
}

void isr_usart3(void)
{
	u8 c;
	while (rd32(R_USART3_SR) & (BIT5 | BIT3)) {
		c = rd32(R_USART3_DR);
		cbuf_write(&uart_cbuf[2], &c, 1);
	}
}

void isr_uart4(void)
{
	u8 c;
	while (rd32(R_UART4_SR) & (BIT5 | BIT3)) {
		c = rd32(R_UART4_DR);
		cbuf_write(&uart_cbuf[3], &c, 1);
	}
}

void isr_uart5(void)
{
	u8 c;
	while (rd32(R_UART5_SR) & (BIT5 | BIT3)) {
		c = rd32(R_UART5_DR);
		cbuf_write(&uart_cbuf[4], &c, 1);
	}
}

int uart_init(int fd)
{
	u8 min = dev_minor(devs(fd)->id);
	cbuf_init(&uart_cbuf[min], buf[min], sizeof(buf[min]));
	return 0;
}

static int uart_avail(int fd)
{
	int uart_id = dev_minor(devs(fd)->id);

	if (uart_id > array_size(uart_cbuf))
		return -ERRINVAL;
	return cbuf_avail(&uart_cbuf[uart_id]);
}

static int uart_read(int fd, void *buf, size_t count)
{
	int uart_id = dev_minor(devs(fd)->id);

	if (uart_id > array_size(uart_cbuf))
		return -ERRINVAL;

	if (!buf || count < 0)
		return -ERRINVAL;

	return cbuf_read(&uart_cbuf[uart_id], buf, count);
}

static int uart_write(int fd, const void *buf, size_t count)
{
	int i;
	volatile u32 *dr;
	volatile u32 *sr;
	int uart_id = dev_minor(devs(fd)->id);

	if (!buf || count < 0)
		return -ERRINVAL;

	switch (uart_id) {
		case 0:
			dr = R_USART1_DR;
			sr = R_USART1_SR;
			break;
		case 1:
			dr = R_USART2_DR;
			sr = R_USART2_SR;
			break;
		case 2:
			dr = R_USART3_DR;
			sr = R_USART3_SR;
			break;
		case 3:
			dr = R_UART4_DR;
			sr = R_UART4_SR;
			break;
		case 4:
			dr = R_UART5_DR;
			sr = R_UART5_SR;
			break;
		default:
			return -ERRINVAL;
	}

	if (uart_priv(fd)->rs485de_io != IO_NULL)
		gpio_wr(uart_priv(fd)->rs485de_io, 1); /* manually handle DE (set to 1) */

	for (i = 0; i < count; i++) {
		wr32(dr, ((u8*)buf)[i]);
		while (!(rd32(sr) & BIT7));
	}

	while (!(rd32(sr) & BIT6));

	if (uart_priv(fd)->rs485de_io != IO_NULL)
		gpio_wr(uart_priv(fd)->rs485de_io, 0); /* manually handle DE (set to 0) */

	return count;
}

static const k_drv_t attr_drvs uart_drv = {
	.maj = MAJ_SOC_UART,
	.name = "stm32-uart",
	.init = uart_init,
	.read = uart_read,
	.write = uart_write,
	.avail = uart_avail,
};
