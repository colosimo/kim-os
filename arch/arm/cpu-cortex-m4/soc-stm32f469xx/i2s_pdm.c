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

#define BYTES_PER_SAMPLE (1024 / 8) /* 10 bits are 1024 PDM samples, i.e.
    1024/8 SPI sampled bytes */

static u8 buf[BYTES_PER_SAMPLE * 128]; /* record up to 128 samples (~100ms @1.25kHz) */
static struct cbuf_t cbuf = {
	.size = sizeof(buf),
	.buf = buf,
};

void isr_spi2(void) /* FIXME I2S hardcoded on SPI2 */
{
	u32 sr;
	u16 smp;
	sr = rd32(R_SPI2_SR);
	smp = rd16(R_SPI2_DR16);

	if (sr & BIT0) {
		and32(R_SPI2_SR, ~BIT0);
		cbuf_write(&cbuf, &smp, sizeof(smp));
	}
}

int i2s_pdm_dev_init(int fd)
{
	u32 i2s_div, i2s_odd;
	/* Target is 40kHz @16bit.
	 * When counting 1's and 0's, you'll have a set of 1024 samples (10bit)
	 * at the frequency: 40kHz/32 = 1.25kHz.
	 * NOTE: I2SPLL frequency is 128MHz, set in zsensor.c file */
	wr32(R_SPI2_CR2, BIT6); /* RXNEIE */
	wr32(R_SPI2_I2SCFGR, BIT11 | (0b11 << 8) | BIT3 | (0b00 << 1) | (0 << 0));
	i2s_div = 50;
	i2s_odd = 0;
	/* I2S_FREQ=128MHz; Fs = 128MHz/[(16*2)*((2*i2s_div)+i2s_odd)] = 40KHz */
	wr32(R_SPI2_I2SPR, (i2s_odd << 8) | i2s_div);

	or32(R_SPI2_I2SCFGR, BIT10); /* I2S Enable */
	return 0;
}

static int i2s_pdm_dev_avail(int fd)
{
	int avail;
	int ret;

	and32(R_SPI2_CR2, ~BIT6); /* RXNEIE interrupt disable */
	avail = cbuf_avail(&cbuf);
	if (avail >= sizeof(buf) - 4) {
		cbuf_clear(&cbuf);
		ret = -ERROVERRUN;
		goto done;
	}

	ret = avail / BYTES_PER_SAMPLE;

done:
	or32(R_SPI2_CR2, BIT6); /* RXNEIE interrupt enable */
	return ret;
}

static int i2s_pdm_dev_read(int fd, void *buf, size_t count)
{
	/* Look-up Table, declaring how many 1's are present in a nibble. */
	u16 lut[] = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4};
	int ret;
	u32 smp32;
	i32 d;
	int avail;
	int i, j, k;
	i32 *b;

	if (!buf || count < 0)
		return -ERRINVAL;

	and32(R_SPI2_CR2, ~BIT6); /* RXNEIE interrupt disable */
	avail = cbuf_avail(&cbuf);
	or32(R_SPI2_CR2, BIT6); /* RXNEIE interrupt enable */

	if (count % sizeof(u32))
		wrn("i2s_pdm should be read on multiples of 4-byte words\n");

	b = (i32*)buf;
	ret = 0;

	for (i = 0; i < min(avail / BYTES_PER_SAMPLE, count / 4); i++) {
		d = 0;

		for (j = 0; j < BYTES_PER_SAMPLE / 4; j++) {
			and32(R_SPI2_CR2, ~BIT6); /* RXNEIE interrupt disable */
			cbuf_read(&cbuf, &smp32, 4);
			or32(R_SPI2_CR2, BIT6); /* RXNEIE interrupt enable */

			/* Use the Look-Up Table above, in order to count how many
			 * 1's are in the 32-bit smp32 variable, by adding 4bit per cycle */
			for (k = 0; k < 8; k++) {
				d += lut[smp32 & 0xf];
				smp32 >>= 4;
			}
		}
		*b = d;
		b++;
		ret += 4;
	}
	return ret;
}

static const k_drv_t attr_drvs i2s_pdm_dev_drv = {
	.maj = MAJ_SOC_DFSDM,
	.init = i2s_pdm_dev_init,
	.read = i2s_pdm_dev_read,
	.avail = i2s_pdm_dev_avail,
};
