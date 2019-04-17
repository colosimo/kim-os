/*
 * Author: Aurelio Colosimo, 2019
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

#ifndef _UART_H_
#define _UART_H_

#include <intdefs.h>

int uart_init(void);

int uart_avail(int uart_id);

int uart_read(int uart_id, void *buf, size_t count);

int uart_write(int uart_id, void *buf, size_t count);

#endif
