/*
 * Author: Aurelio Colosimo, 2019
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

#ifndef _KIM_IO_DEFS_H_
#define _KIM_IO_DEFS_H_

#include <linker.h>

/* Device/driver Common Major defines */
#define MAJ_SOC_GPIO    0
#define MAJ_SOC_UART    1
#define MAJ_SOC_SPI     2
#define MAJ_SOC_I2C     3
#define MAJ_SOC_ADC     4
#define MAJ_SOC_PWM     5
#define MAJ_SOC_DFSDM   6
#define MAJ_SOC_NVMPAGE 7

#define IOCTL_DEV_INIT   0
#define IOCTL_DEV_DEINIT 1
#define IOCTL_DEV_RST    2
#define IOCTL_DEV_CFG    3
#define IOCTL_DEV_GET    4
#define IOCTL_DEV_SET    5
#define IOCTL_DEV_XFER   6

struct attr_packed spi_dev_xfer_t {
	u8 addr;
	u8 *buf;
	int bs;
	int count;
	int dir;
	int fd_cs;
};

struct i2c_xfer_t {
	u8 addr;
	u8 *buf;
	size_t len;
	u8 dir;
	u8 nostop;
};

#endif /* _KIM_IO_DEFS_H_ */
