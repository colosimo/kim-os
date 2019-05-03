/*
 * Author: Aurelio Colosimo, 2017
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

#include <basic.h>
#include <errcode.h>
#include <nvm.h>
#include <log.h>
#include <reg.h>
#include <kim.h>

static void wait_no_bsy(void)
{
	/* Wait until BSY bit is de-asserted */
	while(rd32(R_FLASH_SR) & BIT0);
}

static int unlock_flash_cr(void)
{
	wait_no_bsy();
	if (rd32(R_FLASH_CR) & BIT7) {
		/* unlock the FLASH_CR register */
		wr32(R_FLASH_KEYR, 0x45670123);
		wr32(R_FLASH_KEYR, 0xcdef89ab);
	}
	return 0;
}

int nvm_erase_pages(unsigned s_start, unsigned s_end)
{
	unsigned i;
	int ret = 0;

	if (s_end < s_start)
		return -ERRINVAL;

	unlock_flash_cr();

	/* Disable PG (Programming) and enable PER (Page Erase) */
	and32(R_FLASH_CR, ~BIT0);
	or32(R_FLASH_CR, BIT1);

	for (i = s_start; i <= s_end; i++) {
		wait_no_bsy();
		wr32(R_FLASH_AR, 0x08000000 + i * _K);
		or32(R_FLASH_CR, BIT6);
		wait_no_bsy();
		if (!(rd32(R_FLASH_SR) & BIT5)) {
			ret = -ERRIO;
			err("%s failed on page 0x%x\n", __func__, i);
			break;
		}
	}

	/* Disable PER (Page Erase) */
	and32(R_FLASH_CR, ~BIT1);
	/* Lock the CR register until next prepare */
	or32(R_FLASH_CR, BIT7);
	return ret;
}

int nvm_copy_to_flash(void *ptr, const void *data, size_t cnt)
{
	u16 *src, *dst;
	int i;
	unsigned p = (unsigned)ptr;

	/* FIXME RAM length depending on the chip */
	if (data < (void*)0x20000000 || data >= (void*)0x20002000)
		return -ERRINVAL;

	/* FIXME flash length depending on chip */
	if (p < 0x08000000 || p >= 0x08080000)
		return -ERRINVAL;

	unlock_flash_cr();

	/* Clear PER and set PG bit in FLASH_CR register */
	and32(R_FLASH_CR, ~BIT1);
	or32(R_FLASH_CR, BIT0);

	src = (u16*)data;
	dst = (u16*)ptr;
	for (i = 0; i < cnt / 2; i++) {
		*dst++ = *src++;
		wait_no_bsy();
	}

	/* Lock the CR register until next prepare */
	and32(R_FLASH_CR, ~BIT0);
	or32(R_FLASH_CR, BIT7);

	return 0;
}

unsigned nvm_get_page(const void *addr)
{
	/* FIXME flash length depending on chip */
	if (addr < (void*)0x08000000 || (addr > (void*)0x08080000))
		return -ERRINVAL;

	return ((unsigned)addr - 0x08000000) / _K;
}
