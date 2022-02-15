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

static int pwm_dev_write(int fd, const void *buf, size_t len)
{
	u32 arr;
	u8 duty;
	log("%s\n", __func__);
	if (len == 1)
		duty = (u8)*((u8*)buf);
	else
		return -ERRINVAL;

	if (duty > 100)
		return -ERRINVAL;

	arr = rd32(R_TIM5_ARR);

	and32(R_TIM5_CR1, ~BIT0);
	wr32(R_TIM5_CCR1, (((arr + 1) * (100 - duty)) / 100));

	or32(R_TIM5_CR1, BIT0);
	return len;
}

static const k_drv_t attr_drvs pwm_dev_drv = {
	.maj = MAJ_SOC_PWM,
	.write = pwm_dev_write,
};
