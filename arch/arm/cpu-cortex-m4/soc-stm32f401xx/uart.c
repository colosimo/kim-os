/*
 * Author: Aurelio Colosimo, 2019
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

extern int putchar(int c);

#define UART_BUF_SIZE 96

static struct cbuf_t uart_cbuf[5];

static u8 buf1[UART_BUF_SIZE];
static u8 buf2[UART_BUF_SIZE];
static u8 buf5[UART_BUF_SIZE];

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

void isr_usart6(void)
{
	u8 c;
	while (rd32(R_USART6_SR) & (BIT5 | BIT3)) {
		c = rd32(R_USART6_DR);
		cbuf_write(&uart_cbuf[5], &c, 1);
	}
}

int uart_init(void)
{
	cbuf_init(&uart_cbuf[0], buf1, sizeof(buf1));
	cbuf_init(&uart_cbuf[1], buf2, sizeof(buf2));
	cbuf_init(&uart_cbuf[5], buf5, sizeof(buf5));
	return 0;
}

int uart_avail(int fd)
{
	int uart_id = dev_minor(devs(fd)->id);

	if (uart_id > array_size(uart_cbuf))
		return -ERRINVAL;
	return cbuf_avail(&uart_cbuf[uart_id]);
}

int uart_read(int fd, void *buf, size_t count)
{
	int uart_id = dev_minor(devs(fd)->id);

	if (uart_id > array_size(uart_cbuf))
		return -ERRINVAL;

	if (!buf || count < 0)
		return -ERRINVAL;

	return cbuf_read(&uart_cbuf[uart_id], buf, count);
}

int uart_write(int fd, const void *buf, size_t count)
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

		case 5:
			dr = R_USART6_DR;
			sr = R_USART6_SR;
			break;
		default:
			return -ERRINVAL;
	}

	for (i = 0; i < count; i++) {
		wr32(dr, ((u8*)buf)[i]);
		while (!(rd32(sr) & BIT7));
	}

	while (!(rd32(sr) & BIT6));

	return count;
}

const k_drv_t attr_drvs uart_drv = {
	.maj = MAJ_SOC_UART,
	.name = "stm32-uart",
	.read = uart_read,
	.write = uart_write,
	.avail = uart_avail,
};
