/*
 * Author: Aurelio Colosimo, 2019, MIT License
 */

#ifndef _GPIO_H_
#define _GPIO_H_

#include <basic.h>
#include "reg.h"

#define PORTA 0
#define PORTB 1
#define PORTC 2
#define PORTD 3
#define PORTE 4
#define PORTF 5
#define PORTG 6
#define PORTH 7
#define PORTI 8

#define IO(port, pin) (((port) << 8) | (pin))
#define PORT(io) ((io) >> 8)
#define PIN(io) ((io) & 0xff)

#define IO_NULL IO(0xff, 0xff)

#define GPIOx_MODER(x) (R_GPIOA_MODER + (0x400 * PORT(x)) / 4)
#define GPIOx_OTYPER(x) (R_GPIOA_OTYPER + (0x400 * PORT(x)) / 4)
#define GPIOx_OSPEEDR(x) (R_GPIOA_OSPEEDR + (0x400 * PORT(x)) / 4)
#define GPIOx_IDR(x)   (R_GPIOA_IDR   + (0x400 * PORT(x)) / 4)
#define GPIOx_BSRR(x)  (R_GPIOA_BSRR  + (0x400 * PORT(x)) / 4)
#define GPIOx_AFRL(x)  (R_GPIOA_AFRL  + (0x400 * PORT(x)) / 4)
#define GPIOx_AFRH(x)  (R_GPIOA_AFRH  + (0x400 * PORT(x)) / 4)
#define GPIOx_AFR(x)   (PIN(x) < 8 ? GPIOx_AFRL(x) : GPIOx_AFRH(x))
#define GPIOx_PUPDR(x) (R_GPIOA_PUPDR + (0x400 * PORT(x)) / 4)

#define PULL_UP   0b01
#define PULL_NO   0b00
#define PULL_DOWN 0b10

#define DIR_OUT 1
#define DIR_IN  0

#define PINSEL(io) (PINSEL0 + 2 * PORT(io) + PIN(io) / 16)
#define PINMODE(io) (PINMODE0 + 2 * PORT(io) + PIN(io) / 16)

#define IOCTL_GPIO_PULLNO   IOCTL_USER(0)
#define IOCTL_GPIO_PULLUP   IOCTL_USER(1)
#define IOCTL_GPIO_PULLDOWN IOCTL_USER(2)

static inline void gpio_dir(u16 io, int out)
{
	volatile u32 *reg = GPIOx_MODER(io);
	and32(reg, ~(0b11 << (2 * PIN(io))));
	if (out) {
		or32(reg, (0b01 << (2 * PIN(io))));

		/* Very high speed */
		reg = GPIOx_OSPEEDR(io);
		and32(reg, ~(0b11 << (2 * PIN(io))));
		or32(reg, ~(0b11 << (2 * PIN(io))));
	}
}

static inline int gpio_rd(u16 io)
{
	return (rd32(GPIOx_IDR(io)) >> PIN(io)) & 0x1;
}

static inline void gpio_wr(u16 io, int v)
{
	wr32(GPIOx_BSRR(io), 1 << (PIN(io) + (v ? 0 : 16)));
}

static inline void gpio_func(u16 io, u8 f)
{
	volatile u32 *reg_afr;
	volatile u32 *reg_moder;
	u32 v;
	reg_afr = GPIOx_AFR(io);
	reg_moder = GPIOx_MODER(io);

	v = rd32(reg_moder);
	v &= ~(0b11 << (2 * PIN(io)));
	wr32(reg_moder, v);

	and32(reg_afr, ~(0b1111 << (4 * (PIN(io) % 8))));
	or32(reg_moder, (0b10 << (2 * PIN(io))));
	or32(reg_afr, ((u32)f & 0b1111) << (4 * (PIN(io) % 8)));
}

static inline void gpio_mode(u16 io, u8 f)
{
	volatile u32 *reg = GPIOx_PUPDR(io);
	and32(reg, ~(3 << (2 * (PIN(io) % 16))));
	or32(reg, ((u32)f) << (2 * (PIN(io) % 16)));
}

static inline void gpio_odrain(u16 io, int odrain)
{
	volatile u32 *reg = GPIOx_OTYPER(io);
	and32(reg, ~(1 << PIN(io)));
	if (odrain)
		or32(reg, (1 << PIN(io)));
}

/* I/O device/driver interface for kim-os: declare a device, having priv
 * field assigned to gpio_data_t, for each pin which will be used as generic
 * I/O pin (no AF handled here) */
void gpio_init(void);

struct gpio_data_t {
	u16 io; /* GPIO io(port, pin) */
	u8 dir; /* 1: out, 0: in */
	u8 pull_mode;
};

#define declare_gpio_dev(minor, _io, _dir, _pull_mode, _name) \
	static struct gpio_data_t attr_used gpio##minor = \
	{.io = _io, .dir = _dir, .pull_mode = _pull_mode}; \
	declare_dev(MAJ_SOC_GPIO, minor, &gpio##minor, _name);

#endif /* _GPIO_H_ */
