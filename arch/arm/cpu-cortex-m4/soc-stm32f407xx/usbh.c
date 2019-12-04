/*
 * Author: Aurelio Colosimo, 2019
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

#include <basic.h>
#include <log.h>
#include <errcode.h>
#include <cbuf.h>
#include <reg.h>
#include <kim.h>
#include <kim-io-defs.h>
#include <kim-io.h>
#include <cpu.h>

#define OTG_FS_INTMSK (BIT29 | BIT24 | BIT2 | BIT1) /* DISCINT HPRTINT MMISM */

/* USB Full Speed Host driver */

struct usbh_priv_t {
	u32 intsts;
	int status;
} usbh_priv;

void isr_otgfs(void)
{
	volatile u32 st;
	st = rd32(R_OTG_FS_GINTSTS);
	usbh_priv.intsts = OTG_FS_INTMSK & st;
	wr32(R_OTG_FS_GINTSTS, ~0);
	or32(R_OTG_FS_HPRT, BIT1);

}

void usbh_start(struct task_t *t)
{
	/* Force host mode */
	or32(R_OTG_FS_GUSBCFG, BIT29);
	while (!(rd32(R_OTG_FS_GINTSTS) & BIT0));

	/* Init port and interrupts */
	or32(R_OTG_FS_GAHBCFG, BIT0); /* GINTMSK=1 */
	or32(R_OTG_FS_GINTMSK, OTG_FS_INTMSK);
	wr32(R_OTG_FS_HCFG, 0b01);
	or32(R_OTG_FS_HPRT, BIT12);
	wr32(R_OTG_FS_GINTSTS, ~0);
	or32(R_NVIC_ISER(2), BIT3); /* OTGFS is irq 67 */
}

void usbh_step(struct task_t *t)
{
	struct usbh_priv_t *p = (struct usbh_priv_t *)t->priv;
	int newst = p->status;

	switch (p->status) {
		case 0: /* Idle */
		if (p->intsts & BIT24)
			newst = 1;
		break;

		case 1: /* Connected */
		if (p->intsts & BIT29)
			newst = 0;
		break;
	}

	if (newst != p->status) {
		log("%s status %d -> %d\n", __func__, p->status, newst);
		p->status = newst;
	}

	p->intsts = 0;
}

static struct task_t attr_tasks task_usbh = {
	.start = usbh_start,
	.step = usbh_step,
	.intvl_ms = 10,
	.priv = &usbh_priv,
};

int usbfs_host_init(void)
{
	task_start(&task_usbh);
	return 0;
}
