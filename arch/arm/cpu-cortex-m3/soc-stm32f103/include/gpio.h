/*
 * Author: Aurelio Colosimo, 2019, MIT License
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

#define PORTA 0
#define PORTB 1
#define PORTC 2
#define PORTD 3
#define PORTF 5

#define IO(port, pin) (((port) << 8) | (pin))
#define PORT(io) ((io) >> 8)
#define PIN(io) ((io) & 0xff)

#define PULL_UP   0b01
#define PULL_NO   0b00
#define PULL_DOWN 0b10

static inline void gpio_dir(u16 io, int out)
{
	/* FIXME TODO */
}

static inline int gpio_rd(u16 io)
{
	/* FIXME TODO */
	return 0;
}

static inline void gpio_wr(u16 io, int v)
{
	/* FIXME TODO */
}

static inline void gpio_func(u16 io, u8 f)
{
	/* FIXME TODO */
}

static inline void gpio_mode(u16 io, u8 f)
{
	/* FIXME TODO */
}

static inline void gpio_odrain(u16 io, int odrain)
{
	/* FIXME TODO */
}
