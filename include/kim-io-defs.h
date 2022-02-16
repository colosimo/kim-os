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
#define MAJ_SOC_DAC     8
#define MAJ_SOC_FMPI2C  9

#define IOCTL_DEV_INIT   0
#define IOCTL_DEV_DEINIT 1
#define IOCTL_DEV_RST    2
#define IOCTL_DEV_CFG    3
#define IOCTL_DEV_GET    4
#define IOCTL_DEV_SET    5
#define IOCTL_DEV_XFER   6

#define IOCTL_USER(x)    (32 + x)

#define PULL_UP   0b01
#define PULL_NO   0b00
#define PULL_DOWN 0b10

#define DIR_OUT 1
#define DIR_IN  0

struct attr_packed spi_dev_xfer_t {
	u8 addr;
	u8 *buf;
	int bs;
	int count;
	int dir;
	int fd_cs;
};

#define I2C_DIR_OUT 1
#define I2C_DIR_IN  0

struct i2c_xfer_t {
	u8 addr; /* 7bit address */
	u8 *buf;
	size_t len;
	u8 dir;
	u8 nostop;
};

struct gpio_cfg_t {
	u8 dir;
	u8 pull_mode;
};

#endif /* _KIM_IO_DEFS_H_ */
