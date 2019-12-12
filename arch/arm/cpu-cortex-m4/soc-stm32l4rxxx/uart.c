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

#define UART_BUF_SIZE 96

static struct cbuf_t uart_cbuf[3];

/* FIXME: ugly, better move buffers to priv field of uart devices
 * (define uart_data_t in uart.h) */
static u8 buf1[UART_BUF_SIZE];
static u8 buf2[UART_BUF_SIZE];
static u8 buf3[UART_BUF_SIZE];

void isr_usart1(void)
{
	u8 c;
	while (rd32(R_USART1_ISR) & (BIT5 | BIT3)) {
		c = rd32(R_USART1_RDR);
		cbuf_write(&uart_cbuf[0], &c, 1);
		or32(R_USART1_ICR, BIT3);
	}
}

void isr_usart2(void)
{
	u8 c;
	while (rd32(R_USART2_ISR) & (BIT5 | BIT3)) {
		c = rd32(R_USART2_RDR);
		cbuf_write(&uart_cbuf[1], &c, 1);
		or32(R_USART2_ICR, BIT3);
	}
}

void isr_usart3(void)
{
	u8 c;
	while (rd32(R_USART3_ISR) & (BIT5 | BIT3)) {
		c = rd32(R_USART3_RDR);
		cbuf_write(&uart_cbuf[2], &c, 1);
		or32(R_USART3_ICR, BIT3);
	}
}

int uart_init(void)
{
	cbuf_init(&uart_cbuf[0], buf1, sizeof(buf1));
	cbuf_init(&uart_cbuf[1], buf2, sizeof(buf2));
	cbuf_init(&uart_cbuf[2], buf3, sizeof(buf3));
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
	volatile u32 *tdr;
	volatile u32 *isr;
	int uart_id = dev_minor(devs(fd)->id);

	if (!buf || count < 0)
		return -ERRINVAL;

	switch (uart_id) {
		case 0:
			tdr = R_USART1_TDR;
			isr = R_USART1_ISR;
			break;

		case 1:
			tdr = R_USART2_TDR;
			isr = R_USART2_ISR;
			break;

		case 2:
			tdr = R_USART3_TDR;
			isr = R_USART3_ISR;
			break;
		default:
			return -ERRINVAL;
	}

	for (i = 0; i < count; i++) {
		wr32(tdr, ((u8*)buf)[i]);
		while (!(rd32(isr) & BIT7));
	}

	while (!(rd32(isr) & BIT6));

	return count;
}

static const k_drv_t attr_drvs uart_drv = {
	.maj = MAJ_SOC_UART,
	.name = "stm32-uart",
	.read = uart_read,
	.write = uart_write,
	.avail = uart_avail,
};
