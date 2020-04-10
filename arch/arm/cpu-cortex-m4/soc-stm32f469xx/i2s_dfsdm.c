/*
 * Author: Aurelio Colosimo, 2020
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

/* I2S with embedded software Digital Filter for Sigma-Delta Modulator driver
 * This diver converts a PDM (Pulse Density modulator) signal into a PCM
 * one, by counting 0's and 1's read on I2S/SPI input
 * NOTE: the driver is hardcoded on SPI2, since it's the one used in
 * Zeppelin board */

#include <kim-io.h>
#include <kim-io-defs.h>
#include <reg.h>
#include <linker.h>
#include <errcode.h>
#include <log.h>
#include <kim.h>
#include <cbuf.h>

static u8 buf[512 * 4];
static struct cbuf_t cbuf = {
	.size = sizeof(buf),
	.buf = buf,
};

/* Look-up Table, declaring how many 1's are present in a nibble.
 * See isr_spi2 below */
static u16 lut[] = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4};
static int smpcnt = 0;
static u32 d = 0;

void isr_spi2(void) /* FIXME I2S hardcoded on SPI2 */
{
	volatile u16 smp = rd16(R_SPI2_DR16);
	int i;

	/* This function uses the Look-Up Table above, in order to count how many
	 * 1's are in the 16-bit word, by adding 4bit per cycle */
	for (i = 0; i < 4; i++) {
		d += lut[smp & 0xf];
		smp >>= 4;
	}

	smpcnt += 16;

	if (smpcnt == 1024) {
		cbuf_write(&cbuf, &d, sizeof(d));
		d = 0;
		smpcnt = 0;
	}
}

int i2s_dfsdm_dev_init(int fd)
{
	u32 i2s_div, i2s_odd;
	/* Target is 160kHz @16bit.
	 * When counting 1's and 0's, you'll have a set of 1024 samples (10bit)
	 * at the frequency: 160kHz/32 = 5kHz.
	 * NOTE: I2SPLL frequency is 128MHz, set in zeppelin.c file */
	wr32(R_SPI2_CR2, BIT6); /* RXNEIE */
	wr32(R_SPI2_I2SCFGR, BIT11 | (0b11 << 8) | BIT3 | (0b00 << 1) | (0 << 0));
	i2s_div = 12;
	i2s_odd = 1;
	/* I2S_FREQ=128MHz; Fs = 128MHz/[(16*2)*((2*i2s_div)+i2s_odd)] = 160KHz */
	wr32(R_SPI2_I2SPR, (i2s_odd << 8) | i2s_div);

	or32(R_SPI2_I2SCFGR, BIT10); /* I2S Enable */
	return 0;
}

static int i2s_dfsdm_dev_avail(int fd)
{
	int avail;
	int ret;

	and32(R_SPI2_CR2, ~BIT6); /* RXNEIE interrupt disable */
	avail = cbuf_avail(&cbuf);
	if (avail >= sizeof(buf) - 4) {
		cbuf_init(&cbuf, buf, sizeof(buf));
		ret = -ERROVERRUN;
		goto done;
	}

	ret = avail;

done:
	or32(R_SPI2_CR2, BIT6); /* RXNEIE interrupt enable */
	return ret;
}

static int i2s_dfsdm_dev_read(int fd, void *buf, size_t count)
{
	int ret;
	u32 d;

	if (!buf || count < 0)
		return -ERRINVAL;

	if (count % sizeof(u32))
		wrn("i2s_dfsdm should be read on multiples of 4-byte words\n");

	and32(R_SPI2_CR2, ~BIT6); /* RXNEIE interrupt disable */

	ret = cbuf_read(&cbuf, &d, 4);
	*((u32*)buf) = d;

	or32(R_SPI2_CR2, BIT6); /* RXNEIE interrupt enable */

	return ret;
}

static const k_drv_t attr_drvs i2s_dfsdm_dev_drv = {
	.maj = MAJ_SOC_DFSDM,
	.init = i2s_dfsdm_dev_init,
	.read = i2s_dfsdm_dev_read,
	.avail = i2s_dfsdm_dev_avail,
};
