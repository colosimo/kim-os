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
#include "pwm.h"
#include "eeprom.h"
#include "keys.h"
#include "db.h"
#include "rtc.h"
#include "def.h"

/* DEF Main task */

#define STR_ELO_BANNER "ELO Srl 0536/844420"
#define STR_FUNZ "Funz: g:%03d h:%02d"
#define MS_IN_MIN (60 * 1000)
#define MS_IN_HOUR (60 * MS_IN_MIN)

const char zero = 0;
const char one = 1;

static int alrm = 0;
static int curday = -1;

void set_alarm(int _alrm)
{
	alrm |= _alrm;
}

void clr_alarm(int _alrm)
{
	alrm &= ~_alrm;
}

int get_alarm(int _alrm)
{
	return (alrm & _alrm) ? 1 : 0;
}

u32 last_time_inc;
u32 last_time_key;
u32 hours = 0;

static void def_step(struct task_t *t);

void show_home(void)
{
	char buf[24];
	int gg, hh;

	lcd_write_line(STR_ELO_BANNER, 0, 0);
	eeprom_read(EEPROM_HOURS_ADDR, (u8*)&hours, sizeof(hours));

	gg = hours / 24;
	hh = hours % 24;
	k_sprintf(buf, STR_FUNZ, gg, hh);
	lcd_write_line(buf, 1, 0);
}

void set_standby(int stdby)
{
	show_home();
	lcd_set_backlight(!stdby);
	if (stdby)
		last_time_key = 0;
}

int get_standby(void)
{
	return !lcd_get_backlight();
}

void rearm_standby(void)
{
	last_time_key = k_ticks();
}

static void def_start(struct task_t *t)
{
	lcd_init();
	eeprom_init();
	pwm_init();

	db_data_init();
	set_standby(0);
	last_time_key = last_time_inc = k_ticks();
	def_step(t);

	db_start_add();
}

static void def_step(struct task_t *t)
{
	struct rtc_t r;

	if (k_elapsed(last_time_inc) >= MS_TO_TICKS(MS_IN_HOUR)) {
		last_time_inc = k_ticks();
		hours++;
		eeprom_write(EEPROM_HOURS_ADDR, (u8*)&hours, sizeof(hours));
		/* FIXME Check rolling freq */
	}

	if (k_elapsed(last_time_key) >= MS_TO_TICKS(MS_IN_MIN) && !get_standby())
		set_standby(1);

	/* Save data at midnight */
	rtc_get(&r);
	if (r.hour == 23 && r.min == 59 && r.day != curday) {
		db_data_save_to_eeprom();
		curday = r.day;
	}
}

struct task_t attr_tasks task_def = {
	.start = def_start,
	.step = def_step,
	.intvl_ms = 1000,
	.name = "def",
};

/* Antenna check */

#define ANTENNA_CHECK_DELAY_MS 1000

static u32 t_last_on;
static u32 ant_error;
void ant_check_start(struct task_t *t)
{
	ant_error = 0;
	t_last_on = k_ticks();
	k_write(k_fd_byname("user_led_2"), &zero, 1);
}

void ant_check_step(struct task_t *t)
{
	u8 ant_check;
	k_read(k_fd_byname("ant_check"), &ant_check, 1);

	if (!ant_check) {
		t_last_on = k_ticks();
		if (get_alarm(ALRM_BITFIELD_ANT))
			clr_alarm(ALRM_BITFIELD_ANT);
	}

	if (!get_alarm(ALRM_BITFIELD_ANT) &&
	    k_elapsed(t_last_on) > MS_TO_TICKS(ANTENNA_CHECK_DELAY_MS)) {
		set_alarm(ALRM_BITFIELD_ANT);
		db_alarm_add(ALRM_TYPE_ANT, 0);
	}
}

struct task_t attr_tasks task_ant_check = {
	.start = ant_check_start,
	.step = ant_check_step,
	.intvl_ms = 1,
	.name = "ant_check",
};

/* PWM out manual command */

static int pwm_cmd_cb(int argc, char *argv[], int fdout)
{
	u32 freq;
	u32 duty;
	freq = atoi(argv[1]);
	duty = atoi(argv[2]);

	pwm_set(freq, duty);
	return 0;
}

const struct cli_cmd_t attr_cli cli_pwm = {
	.narg = 2,
	.cmd = pwm_cmd_cb,
	.name = "pwm",
	.descr = "pwm {freq} {duty}",
};
