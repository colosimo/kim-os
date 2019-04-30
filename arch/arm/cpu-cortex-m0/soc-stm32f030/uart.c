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
#include <uart.h>
#include <reg.h>
#include <kim.h>
#include <cpu.h>

#define UART_BUF_SIZE 512

static struct cbuf_t uart_cbuf[2];

static u8 buf1[UART_BUF_SIZE];
static u8 buf2[UART_BUF_SIZE];

void isr_uart1(void)
{
	u8 c;
	c = rd32(R_USART1_RDR);
	cbuf_write(&uart_cbuf[0], &c, 1);
}

void isr_uart2(void)
{
	u8 c;
	c = rd32(R_USART2_RDR);
	cbuf_write(&uart_cbuf[1], &c, 1);
}

int uart_init(void)
{
	cbuf_init(&uart_cbuf[0], buf1, sizeof(buf1));
	cbuf_init(&uart_cbuf[1], buf2, sizeof(buf2));
	or32(R_NVIC_ISER, BIT28);
	or32(R_NVIC_ISER, BIT27);

	return 0;
}

int uart_avail(int uart_id)
{
	if (uart_id > array_size(uart_cbuf))
		return -ERRINVAL;
	return cbuf_avail(&uart_cbuf[uart_id]);
}

int uart_read(int uart_id, void *buf, size_t count)
{
	if (uart_id > array_size(uart_cbuf))
		return -ERRINVAL;

	return cbuf_read(&uart_cbuf[uart_id], buf, count);
}

int uart_write(int uart_id, void *buf, size_t count)
{
	int i;
	volatile u32 *tdr;
	volatile u32 *isr;
	tdr = uart_id ? R_USART2_TDR : R_USART1_TDR;
	isr = uart_id ? R_USART2_ISR : R_USART1_ISR;
	for (i = 0; i < count; i++) {
		wr32(tdr, ((u8*)buf)[i]);
		while (!(rd32(isr) & BIT7));
	}
	return count;
}
