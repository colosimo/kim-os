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

static struct cbuf_t uart_cbuf[3];
static u32 cpu_freq;

static u8 buf1[UART_BUF_SIZE];
static u8 buf2[UART_BUF_SIZE];
static u8 buf3[UART_BUF_SIZE];

void isr_uart1(void)
{
	u8 c;
	while (rd32(R_USART1_SR) & (BIT5 | BIT3)) {
		c = rd32(R_USART1_DR);
		cbuf_write(&uart_cbuf[0], &c, 1);
	}
}

void isr_uart2(void)
{
	u8 c;
	while (rd32(R_USART2_SR) & (BIT5 | BIT3)) {
		c = rd32(R_USART2_DR);
		cbuf_write(&uart_cbuf[1], &c, 1);
	}
}

void isr_uart3(void)
{
	u8 c;
	while (rd32(R_USART3_SR) & (BIT5 | BIT3)) {
		c = rd32(R_USART3_DR);
		cbuf_write(&uart_cbuf[2], &c, 1);
	}
}

int uart_init(u32 _cpu_freq)
{
	cpu_freq = _cpu_freq;
	cbuf_init(&uart_cbuf[0], buf1, sizeof(buf1));
	cbuf_init(&uart_cbuf[1], buf2, sizeof(buf2));
	cbuf_init(&uart_cbuf[2], buf3, sizeof(buf3));

	or32(R_NVIC_ISER(1), BIT5); /* USART1 is irq 37 */
	or32(R_NVIC_ISER(1), BIT6); /* USART1 is irq 38 */
	or32(R_NVIC_ISER(1), BIT7); /* USART1 is irq 39 */

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

	if (!buf || count < 0)
		return -ERRINVAL;

	return cbuf_read(&uart_cbuf[uart_id], buf, count);
}

int uart_write(int uart_id, void *buf, size_t count)
{
	int i;
	volatile u32 *dr = NULL;
	volatile u32 *sr = NULL;

	if (uart_id > array_size(uart_cbuf))
		return -ERRINVAL;

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
	}

	if (!dr || !sr)
		return 0;

	for (i = 0; i < count; i++) {
		wr32(dr, ((u8*)buf)[i]);
		while (!(rd32(sr) & BIT7));
	}
	while (!(rd32(sr) & BIT6));
	return count;
}

int uart_set_baudrate(int uart_id, u32 baudrate)
{
	volatile u32 *brr;

	if (uart_id > array_size(uart_cbuf))
		return -ERRINVAL;

	if (!baudrate || baudrate > 3000000) /* Max 3Mbit/s */
		return -ERRINVAL;

	brr = uart_id ? R_USART2_BRR : R_USART1_BRR;
	wr32(brr, (cpu_freq / baudrate) << 4); /* FIXME set fraction too */
	return 0;
}
