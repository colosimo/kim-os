/*
 * Author: Aurelio Colosimo, 2021
 * Copyright: Elo System srl
 */

#include <stdint.h>
#include <log.h>
#include <kim.h>
#include <linker.h>
#include <kim-io.h>
#include <kim-io-defs.h>
#include <errcode.h>

#include "lcd.h"


void def_start(struct task_t *t)
{
	log("%s\n", __func__);
	lcd_init();
	//lcd_write(0x80, 0);
	lcd_write_string(__func__, 0);
	//lcd_write_string("UNO", 1);
}

void def_step(struct task_t *t)
{
	//log("%s\n", __func__);
	//lcd_write_string(__func__, 1);
}

struct task_t attr_tasks task_def = {
	.start = def_start,
	.step = def_step,
	.intvl_ms = 1000,
	.name = "def",
};
