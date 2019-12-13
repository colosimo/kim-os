/*
 * Author: Aurelio Colosimo, 2019
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

#ifndef _KIM_IO_H_
#define _KIM_IO_H_

#include <intdefs.h>
#include <basic.h>

/* dev id <-> (major, minor) conversions */
#define dev_id(maj, min) ((uint16_t)((uint16_t)(maj) << 8 | (min)))
#define dev_major(did) ((uint8_t)(did >> 8))
#define dev_minor(did) ((uint8_t)(did & 0xff))

#define declare_dev(maj, min, _priv, _name) \
	const k_dev_t attr_devs dev_##_name = { \
	.id = dev_id(maj, min), \
	.priv = _priv, \
	.name = str(_name), \
};

/* Device handling */
int k_fd(u8 maj, u8 min);
int k_fd_byname(const char *name);
int k_read(int fd, void *buf, size_t count);
int k_write(int fd, const void *buf, size_t count);
int k_avail(int fd);
int k_ioctl(int d, int cmd, void *buf, size_t count);

struct __attribute__((packed)) k_drv_t {
	int (*read)(int fd, void *buf, size_t count);
	int (*write)(int fd, const void *buf, size_t count);
	int (*avail)(int fd);
	int (*ioctl)(int fd, int cmd, void *buf, size_t count);
	const char *name;
	uint8_t maj;
	uint8_t unused[3];
};
typedef struct k_drv_t k_drv_t;

struct __attribute__((packed)) k_dev_t {
	uint16_t id;
	const struct k_drv_t *drv;
	void *priv;
	const char *name;
	u16 class_id;
};
typedef struct k_dev_t k_dev_t;

int dev_enum(int (*dev_cb)(k_dev_t *, int), int);
int dev_dump(k_dev_t *dev, int);

#endif /* _KIM_IO_H_ */
