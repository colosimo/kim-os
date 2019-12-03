/*
 * Author: Aurelio Colosimo, 2019
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

#include <basic.h>
#include <kim.h>
#include <kim-io.h>
#include <linker.h>
#include <errcode.h>

/* I/O Model */
static const k_drv_t *drv_find(uint8_t major)
{
	const k_drv_t *drv = drvs(0);
	for (;drv != &__stop_drvs; drv++) {
		if (drv->maj == major)
			return drv;
	}
	return NULL;
}

int k_fd(u8 maj, u8 min)
{
	k_dev_t *dev = devs(0);
	int ret;

	for (ret = 0; dev != &__stop_devs; dev++, ret++) {
		if (dev->id == dev_id(maj, min)) {
			if (!dev->drv) { /* Look for driver */
				dev->drv = drv_find(maj);
				return dev->drv ? ret : -ERRINVAL;
			}
			return ret;
		}
	}
	return -ERRINVAL;
}

int k_fd_byname(const char *name)
{
	k_dev_t *dev = devs(0);
	int ret;

	for (ret = 0; dev != &__stop_devs; dev++, ret++) {
		if (!strcmp(dev->name, name)) {
			if (!dev->drv) { /* Look for driver */
				dev->drv = drv_find(dev_major(dev->id));
				return dev->drv ? ret : -ERRINVAL;
			}
			return ret;
		}
	}
	return -ERRINVAL;
}

int k_avail(int fd)
{
	int (*ptr)(int fd);
	ptr = devs(fd)->drv->avail;
	if (!ptr)
			return -ERRINVAL;
	return ptr(fd);
}

int k_read(int fd, void *buf, size_t count)
{
	int (*ptr)(int, void *, size_t);
	ptr = devs(fd)->drv->read;
	if (!ptr)
			return -ERRINVAL;
	return ptr(fd, buf, count);
}

int k_write(int fd, const void *buf, size_t count)
{
	int (*ptr)(int, const void *, size_t);
	ptr = devs(fd)->drv->write;
	if (!ptr)
			return -ERRINVAL;
	return ptr(fd, buf, count);
}

int k_ioctl(int fd, int cmd, void *buf, size_t count)
{
	int (*ptr)(int, int, void *, size_t);
	ptr = devs(fd)->drv->ioctl;
	if (!ptr)
			return -ERRINVAL;
	return ptr(fd, cmd, buf, count);
}
