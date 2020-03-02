/*
 * Author: Aurelio Colosimo, 2020
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

/* DFSDM driver */

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

static int cnt = 0;
static u32 tstart;
void isr_dfsdm_flt0(void)
{
	i32 datar;
	datar = ((i32)rd32(R_DFSDM1_FLTxRDATAR(0))) >> 8;
	cbuf_write(&cbuf, &datar, sizeof(datar));
	cnt++;
}

int dfsdm_dev_init(int fd)
{
	and32(R_DFSDM1_CHyCFGR1(5), ~0xf);
	or32(R_DFSDM1_CHyCFGR1(5), 0b0101);
	or32(R_DFSDM1_CHyCFGR1(5), BIT7); 	/* CH5 enable */

	/* CH5 Continuous mode on filter 0 */
	or32(R_DFSDM1_FLTxCR2(0), BIT1); /* REOCIE interrupt enable */

	and32(R_DFSDM1_FLTxFCR(0), ~(0x3ff << 16));
	or32(R_DFSDM1_FLTxFCR(0), 499 << 16);
	and32(R_DFSDM1_FLTxCR1(0), ~(0b111 << 24));
	or32(R_DFSDM1_FLTxCR1(0), BIT29 | BIT18 | (5 << 24));
	or32(R_DFSDM1_FLTxCR1(0), BIT0);
	or32(R_DFSDM1_CHyCFGR1(0), BIT31); /* DFSDM1 global enable */
	or32(R_DFSDM1_FLTxCR1(0), BIT17);
	cnt = 0;
	tstart = k_ticks();

	return 0;
}

static int dfsdm_dev_avail(int fd)
{
	int avail;
	int ret;

	and32(R_DFSDM1_FLTxCR2(0), ~BIT1); /* REOCIE interrupt disable */
	avail = cbuf_avail(&cbuf);
	if (avail >= sizeof(buf) - 4) {
		cbuf_init(&cbuf, buf, sizeof(buf));
		ret = -ERROVERRUN;
		goto done;
	}

	ret = avail;

done:
	or32(R_DFSDM1_FLTxCR2(0), BIT1); /* REOCIE interrupt enable */
	return ret;
}

static int dfsdm_dev_read(int fd, void *buf, size_t count)
{
	int ret;

	if (!buf || count < 0)
		return -ERRINVAL;

	if (count % sizeof(u32))
		wrn("dfsdm should be read on multiples of 4-byte words\n");

	and32(R_DFSDM1_FLTxCR2(0), ~BIT1); /* REOCIE interrupt disable */

	ret = cbuf_read(&cbuf, buf, count);

	or32(R_DFSDM1_FLTxCR2(0), BIT1); /* REOCIE interrupt enable */

	return ret;
}

static const k_drv_t attr_drvs dfsdm_dev_drv = {
	.maj = MAJ_SOC_DFSDM,
	.init = dfsdm_dev_init,
	.read = dfsdm_dev_read,
	.avail = dfsdm_dev_avail,
};
