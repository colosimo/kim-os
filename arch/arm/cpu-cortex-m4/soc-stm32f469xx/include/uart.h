/*
 * Author: Aurelio Colosimo, 2020, MIT License
 */

#ifndef _UART_H_
#define _UART_H_

#define MINOR_UART1 0
#define MINOR_UART2 1
#define MINOR_UART3 2
#define MINOR_UART4 3
#define MINOR_UART5 4
#define MINOR_UART6 5
#define MINOR_UART7 6
#define MINOR_UART8 7

struct uart_data_t {
	u16 rs485de_io; /* io(port, pin) of GPIO used as chip select */
};

#define declare_uart_dev(minor, _rs485de_io, _name) \
	static struct uart_data_t attr_used uart##minor = {.rs485de_io = _rs485de_io}; \
	declare_dev(MAJ_SOC_UART, minor, &uart##minor, _name);

#endif /* _UART_H_ */
