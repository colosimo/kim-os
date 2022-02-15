/*
 * Author: Aurelio Colosimo, 2019
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

/* SPI driver */

#include <kim-io.h>
#include <kim-io-defs.h>
#include <reg.h>
#include <linker.h>
#include <errcode.h>
#include <log.h>
#include <kim.h>

static int dac_dev_write(int fd, const void *buf, size_t len)
{
	if (len == 1)
		wr32(R_DAC_DHR8R1, *((u8*)buf));
	else if (len == 2)
		wr32(R_DAC_DHR12R1, *((u16*)buf));
	else if (len == 4)
		wr32(R_DAC_DHR12R1, (u16)*((u32*)buf));
	else
		return -ERRINVAL;

	return len;
}

static const k_drv_t attr_drvs dac_dev_drv = {
	.maj = MAJ_SOC_DAC,
	.write = dac_dev_write,
};
