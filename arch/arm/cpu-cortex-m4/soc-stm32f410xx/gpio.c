/*
 * Author: Aurelio Colosimo, 2020
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

/* GPIO driver */

#include <kim-io.h>
#include <kim-io-defs.h>
#include <reg.h>
#include <gpio.h>
#include <linker.h>
#include <errcode.h>
#include <gpio.h>
#include <log.h>

#include <kim.h>

#define gpio_priv(fd) ((struct gpio_data_t*)(devs(fd)->priv))

static int gpio_dev_init(int fd)
{
	struct gpio_data_t *p;
	p = gpio_priv(fd);
	gpio_dir(p->io, p->dir);
	gpio_mode(p->io, p->pull_mode);
	return 0;
}

static int gpio_dev_write(int fd, const void *buf, size_t len)
{
	gpio_wr(gpio_priv(fd)->io, *((uint8_t*)buf));
	return 0;
}

static int gpio_dev_read(int fd, void *buf, size_t count)
{
	uint8_t *val;

	if (!buf || !count)
		return -ERRINVAL;
	val = (uint8_t*)buf;
	*val = gpio_rd(gpio_priv(fd)->io);
	return 1;
}

static int gpio_dev_ioctl(int fd, int cmd, void *buf, size_t count)
{
	struct gpio_cfg_t *cfg;
	struct gpio_data_t *p;
	p = gpio_priv(fd);

	if (cmd == IOCTL_DEV_CFG && count >= sizeof(*cfg)) {
		cfg = (struct gpio_cfg_t *)buf;
		gpio_dir(p->io, cfg->dir);
		gpio_mode(p->io, cfg->pull_mode);
		return 0;
	}

	return -ERRINVAL;
}

static const k_drv_t attr_drvs gpio_dev_drv = {
	.maj = MAJ_SOC_GPIO,
	.init = gpio_dev_init,
	.read = gpio_dev_read,
	.write = gpio_dev_write,
	.ioctl = gpio_dev_ioctl,
};
