/*
 * Author: Aurelio Colosimo, 2016
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

#ifndef _KIM_H_
#define _KIM_H_

#include <intdefs.h>

/* Main Function */
void k_main(void);

/* System ticks */
u32 k_ticks(void);

/* Device handling */
int k_getfd(int devid);
int k_read(int fd, void *buf, size_t count);
int k_write(int fd, const void *buf, size_t count);
int k_ioctl(int d, int request, ...);

struct __attribute__((packed)) k_drv_t {
	int (*read)(int fd, void *buf, size_t count);
	int (*write)(int fd, const void *buf, size_t count);
	int (*ioctl)(int fd, int cmd, const void *buf, size_t count);
	uint8_t maj;
	uint8_t unused[3];
};

struct __attribute__((packed)) k_dev_t {
	uint16_t id;
	const struct k_drv_t *drv;
	void *priv;
	uint8_t unused[2];
};

#endif /* _KIM_H_ */
