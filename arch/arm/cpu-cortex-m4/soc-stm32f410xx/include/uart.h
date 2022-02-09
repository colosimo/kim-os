/*
 * Author: Aurelio Colosimo, 2019, MIT License
 */

#ifndef _UART_H_
#define _UART_H_

#define MINOR_UART1 0
#define MINOR_UART2 1
#define MINOR_UART6 5

int uart_init(void);

struct uart_data_t {
        u16 rs485we_io; /* io(port, pin) of GPIO used as write enable */
        u16 rs485re_io; /* io(port, pin) of GPIO used as read enable */
};

#define declare_uart_dev(minor, _rs485we_io, _rs485re_io, _name) \
	static struct uart_data_t attr_used uart##minor = { \
		.rs485we_io = _rs485we_io, \
		.rs485re_io = _rs485re_io, \
	}; \
	declare_dev(MAJ_SOC_UART, minor, &uart##minor, _name);

#endif /* _UART_H_ */
