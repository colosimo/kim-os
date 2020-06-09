/*
 * Author: Aurelio Colosimo, 2020
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

/* NVM Page driver */

#include <kim-io.h>
#include <kim-io-defs.h>
#include <reg.h>
#include <gpio.h>
#include <linker.h>
#include <errcode.h>
#include <log.h>
#include <cpu.h>

#include <kim.h>

#define NVM_BASE 0x08000000

static int nvm_init_once = 1;

static void wait_no_bsy(void)
{
	/* Wait until BSY bit is de-asserted */
	while(rd32(R_FLASH_SR) & BIT16);
}

static int unlock_flash_cr(void)
{
	wait_no_bsy();
	if (rd32(R_FLASH_CR) & BIT31) {
		/* unlock the FLASH_CR register */
		wr32(R_FLASH_KEYR, 0x45670123);
		wr32(R_FLASH_KEYR, 0xcdef89ab);
	}
	return 0;
}

static int nvm_dev_init(int fd)
{
	if (nvm_init_once) {
		unlock_flash_cr();
		and32(R_FLASH_CR, ~(0b11 << 8));
		or32(R_FLASH_CR, BIT24 | 0b10 << 8); /* 32-bit parallelism */
		or32(R_FLASH_CR, BIT31);
		or32(R_FLASH_SR, 0x1f3);
		nvm_init_once = 0;
	}
	return 0;
}

void *nvm_page_addr(int x)
{
	if (x < 5)
		return (void*)(NVM_BASE + x * 16 * _K);
	else if (x == 5)
		return nvm_page_addr(4) + 64 * _K;
	else
		return nvm_page_addr(5) + (x - 5) * 128 * _K;
}

u32 nvm_page_size(int x)
{
	if (x < 4)
		return 16 * _K;
	else if (x < 5)
		return 64 * _K;
	else
		return 128 * _K;
}

static int nvm_dev_write(int fd, const void *buf, size_t len)
{
	int nvm_page = dev_minor(devs(fd)->id);
	u32 *dst, *src;
	int i;
	u32 sr;

	if (len % 4)
		wrn("%s len %d should be multiple of 4, "
		    "padding with bytes exceeding len\n", __func__, len);

	if ((int)buf % 4)
		err("%s buf misaligned (32-bit alignment needed), %p\n", __func__, buf);

	unlock_flash_cr();

	/* Erase begin */

	/* Disable PG (Programming) and enable SER (Sector Erase) */
	and32(R_FLASH_CR, ~BIT0);
	or32(R_FLASH_CR, BIT1);

	wait_no_bsy();
	/* Set SNB in FLASH_CR register */
	and32(R_FLASH_CR, ~(0x1f << 3));
	or32(R_FLASH_CR, ((nvm_page & 0x1f) << 3));
	cpsid(); /* Erase (below) will run while interrupts are disabled */
	or32(R_FLASH_CR, BIT16);

	wait_no_bsy();

	sr = rd32(R_FLASH_SR);
	if (!(sr & BIT0)) {
		err("Erase failed on page %d sr=%08x\n", nvm_page, (uint)sr);
		or32(R_FLASH_SR, 0x1f3);
		cpsie();
		return -ERRIO;
	}

	wr32(R_FLASH_SR, BIT0);

	/* Erase End */

	/* Write Begin */

	/* Clear SER and set PG bit in FLASH_CR register */
	and32(R_FLASH_CR, ~BIT1);
	or32(R_FLASH_CR, BIT0);

	src = (u32*)buf;
	dst = (u32*)nvm_page_addr(nvm_page);

	for (i = 0; i < (len + 3) / 4; i++) {
		*dst++ = *src++;
		wait_no_bsy();
	}

	/* Write End */

	/* Lock the CR register until next usage */
	and32(R_FLASH_CR, ~BIT0);
	or32(R_FLASH_CR, BIT31);

	cpsie();

	return (int)src - (int)buf;
}

static int nvm_dev_read(int fd, void *buf, size_t count)
{
	int nvm_page = dev_minor(devs(fd)->id);
	int ret = min(count, nvm_page_size(nvm_page));
	memcpy(buf, nvm_page_addr(nvm_page), ret);
	return ret;
}

static const k_drv_t attr_drvs nvm_dev_drv = {
	.maj = MAJ_SOC_NVMPAGE,
	.init = nvm_dev_init,
	.write = nvm_dev_write,
	.read = nvm_dev_read,
};
