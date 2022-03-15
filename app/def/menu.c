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
#include <version.h>

#include "def.h"
#include "keys.h"
#include "lcd.h"
#include "rtc.h"
#include "pwm.h"
#include "rfrx.h"
#include "db.h"
#include "eeprom.h"

static int status = 0; /* Generic state machine variable */
static u32 ticks_exec;

#define MENU_VOICE_DEFAULT 0
#define STR_CONFIRM "CONFERMA?"
#define STR_EMPTY "ARCHIVIO VUOTO"
#define STR_PASSWORD "PASSWORD: [     ]"

#define DEF_TIMEOUT_MS (60 * 1000) /* 1 minute */

struct menu_voice_t {
	int id;
	const char text[2][20];
	void (*on_evt)(int evt);
	void (*refresh)(void);
	int id_next[4]; /* Next menu voice to be called on: UP, DOWN, ESC, ENTER */
	int enabled;
	u32 timeout_ms;
};

static struct menu_voice_t menu[];

static struct menu_voice_t *cur_menu = NULL;

static struct menu_voice_t *get_menu_voice(int id)
{
	int i;
	for (i = 0; menu[i].id >= 0; i++) {
		if (menu[i].id == id)
			return &menu[i];
	}
	return NULL;
}

/* Default browsing into menus */
static void on_evt_def(int key)
{
	struct menu_voice_t *new_menu;

	if (key == KEY_ESC && cur_menu->id_next[key] < 0) {
		show_home();
		cur_menu = NULL;
		goto done;
	}
	new_menu = cur_menu;
	do {
		new_menu = get_menu_voice(new_menu->id_next[key]);
		if (new_menu && new_menu->enabled) {
			status = 0;
			cur_menu = new_menu;
			lcd_write_line(cur_menu->text[0], 0, 1);
			lcd_write_line(cur_menu->text[1], 1, 1);
		}
	} while(new_menu && !new_menu->enabled);

done:
	keys_clear_evts(1 << key);
}

/* Date / Time setting */

static struct rtc_t r;
static u32 ticks_cancel;

static void update_screen_datetime()
{
	char buf[24];
	int cur_line, cur_pos, cur_show;

	if (status == 100) {
		lcd_write_line("ANNULLATO", 0, 1);
		lcd_write_line("", 1, 0);
		status = 103;
		return;
	} else if (status == 102) {
		lcd_write_line("DATA NON VALIDA", 0, 0);
		lcd_write_line("", 1, 0);
		status = 103;
		return;
	}

	k_sprintf(buf, "Data: %02d/%02d/%02d", r.day, r.month, r.year);
	lcd_write_line(buf, 0, 0);
	k_sprintf(buf, "Ore:  %02d:%02d", r.hour, r.min);
	lcd_write_line(buf, 1, 0);

	switch(status) {
		case 0:
		case 1: cur_line = 0; cur_pos = 7; cur_show = 1; break;
		case 2: cur_line = 0; cur_pos = 10; cur_show = 1; break;
		case 3: cur_line = 0; cur_pos = 13; cur_show = 1; break;
		case 4: cur_line = 1; cur_pos = 7; cur_show = 1; break;
		case 5: cur_line = 1; cur_pos = 10; cur_show = 1; break;
		case 6: cur_line = 1; cur_pos = 17; cur_show = 0; break;
		case 101: cur_show = 0;
		default: cur_line = cur_pos = cur_show = 0; break; /* Never happens */
	}
	lcd_cursor(cur_line, cur_pos, cur_show);

	if (status == 6)
		lcd_write_string("OK?");
}

static void refresh_datetime()
{
	if (status == 0) {
		rtc_get(&r);
		update_screen_datetime();
		status = 1;
	}
	else if (status >= 100 && k_elapsed(ticks_cancel) > MS_TO_TICKS(1000))
			on_evt_def(KEY_ESC);
}

static void on_evt_datetime(int key)
{
	switch (status) {
		case 1:
			if (key == KEY_UP)
				r.day = (r.day % 31) + 1;
			else if (key == KEY_DOWN) {
				if (r.day == 1)
					r.day = 31;
				else
					r.day--;
			}
			break;
		case 2:
			if (key == KEY_UP)
				r.month = (r.month % 12) + 1;
			else if (key == KEY_DOWN) {
				if (r.month == 1)
					r.month = 12;
				else
					r.month--;
			}
			break;
		case 3:
			if (key == KEY_UP)
				r.year = r.year % 100 + 1;
			else if (key == KEY_DOWN) {
				if (r.year == 0)
					r.year = 99;
				else
					r.year--;
			}
			break;

		case 4:
			if (key == KEY_UP)
				r.hour = (r.hour + 1) % 24;
			else if (key == KEY_DOWN) {
				if (r.hour == 0)
					r.hour = 23;
				else
					r.hour--;
			}
			break;

		case 5:
			if (key == KEY_UP)
				r.min = (r.min + 1) % 60;
			else if (key == KEY_DOWN) {
				if (r.min == 0)
					r.min = 59;
				else
					r.min--;
			}
			break;

		case 6:
			if (key == KEY_ENTER) {
				if (rtc_valid(&r)) {
					r.sec = 0;
					rtc_dump(&r);
					rtc_set(&r);
					status = 101;
				}
				else
					status = 102;
				ticks_cancel = k_ticks();
			}

		case 100:
			keys_clear_evts(1 << key);
			break;

		default:
			break;
	}

	if (key == KEY_ESC && status != 100) {
		status = 100;
		ticks_cancel = k_ticks();
	}

	if (status < 7 && key == KEY_ENTER)
		status++;
	update_screen_datetime();
	keys_clear_evts(1 << key);

}

/* PWM setting */

static int status_mode;
static int menu_mode = 0;
static struct pwm_cfg_t menu_p;

static void update_screen_pwm()
{
	char buf[24];
	int cur_line, cur_pos;

	k_sprintf(buf, "Freq: %03d  %cMode: %d", (uint)menu_p.freq,
	   menu_mode == status_mode ? '*' : ' ', menu_mode + 1);
	lcd_write_line(buf, 0, 0);
	k_sprintf(buf, "D.C.:  %02d", (uint)menu_p.duty);
	lcd_write_line(buf, 1, 0);

	switch(status) {
		case 0:
		case 1: cur_line = 0; cur_pos = 8; break;
		case 2: cur_line = 1; cur_pos = 8; break;
		default: cur_line = cur_pos = 0; break; /* Never happens */
	}
	lcd_cursor(cur_line, cur_pos, 1);
}

static void refresh_pwm(void)
{
	if (status == 0) {
		eeprom_read(EEPROM_PWM_STATUS_MODE_ADDR, &status_mode, 1);
		menu_mode = status_mode;
		eeprom_read(EEPROM_PWM_MODE0_ADDR + menu_mode * sizeof(menu_p),
		    (u8*)&menu_p, sizeof(menu_p));
		update_screen_pwm();
		status = 1;
	}
}

static void on_evt_pwm(int key)
{
	if (key == KEY_ESC) {
		on_evt_def(key);
		return;
	}

	switch (status) {
		case 1:
			if (key == KEY_UP && menu_p.freq < MAX_FREQ)
				menu_p.freq++;
			else if (key == KEY_DOWN && menu_p.freq > MIN_FREQ)
				menu_p.freq--;
			else if (key == KEY_ENTER)
				status = 2;
			break;

		case 2:
			if (key == KEY_UP && menu_p.duty < MAX_DUTY)
				menu_p.duty++;
			else if (key == KEY_DOWN && menu_p.duty > MIN_DUTY)
				menu_p.duty--;
			else if (key == KEY_ENTER)
				status = 1;

			break;

		default:
			break;
	}

	if (key == KEY_ENTER) {
		pwm_check(&menu_p.freq, &menu_p.duty);
		if (menu_mode == status_mode)
			pwm_set(menu_p.freq, menu_p.duty);
		log("Save mode %d: %d %d\n", menu_mode + 1, (uint)menu_p.freq, (uint)menu_p.duty);
		eeprom_write(EEPROM_PWM_MODE0_ADDR + menu_mode * sizeof(menu_p),
			(u8*)&menu_p, sizeof(menu_p));

		if (status == 1) {
			menu_mode = (menu_mode + 1) % 3;
			eeprom_read(EEPROM_PWM_MODE0_ADDR + menu_mode * sizeof(menu_p),
			    (u8*)&menu_p, sizeof(menu_p));
		}

	}
	update_screen_pwm();
	keys_clear_evts(1 << key);
}


/* Modality choice */

static int current_mode; /* Mode 0, 1, 2, Rolling (mode = 3), Logger (mode = 4) */
static int rolling_days;
static struct pwm_cfg_t mode_pwm_cfg;

static void update_screen_mode()
{
	char buf[24];
	int cur_line, cur_pos;

	if (status == 100) {
		status = 102;
		lcd_write_line("ESEGUITO", 1, 1);
	}
	else if (status == 101) {
		status = 102;
		lcd_write_line("ANNULLATO", 0, 1);
		lcd_write_line("", 1, 0);
		return;
	}
	if (status >= 100) {
		if (k_elapsed(ticks_exec) > MS_TO_TICKS(1000))
			on_evt_def(KEY_ESC);
		return;
	}

	if (current_mode <= 2)
		k_sprintf(buf, "Mode: %d (%dHz %d%%)", current_mode + 1,
		    (uint)mode_pwm_cfg.freq, (uint)mode_pwm_cfg.duty);
	else if (current_mode == 3)
		k_sprintf(buf, "Mode: Rolling");
	else if (current_mode == 4)
		k_sprintf(buf, "Mode: Logger");

	lcd_write_line(buf, 0, 0);

	if (current_mode == 3)
		k_sprintf(buf, "Numero giorni: %02d", (uint)rolling_days);
	else
		buf[0] = '\0';

	lcd_write_line(buf, 1, 0);

	switch(status) {
		case 0:
		case 1: cur_line = 0; cur_pos = 6; break;
		case 2: cur_line = 1; cur_pos = 15; break;
		default: cur_line = cur_pos = 0; break; /* Never happens */
	}
	lcd_cursor(cur_line, cur_pos, 1);
}

static void refresh_mode(void)
{
	if (status == 0) {
		eeprom_read(EEPROM_PWM_CURRENT_MODE_ADDR, &current_mode, 1);
		eeprom_read(EEPROM_PWM_MODE0_ADDR + current_mode * sizeof(mode_pwm_cfg),
		    (u8*)&mode_pwm_cfg, sizeof(mode_pwm_cfg));
		eeprom_read(EEPROM_PWM_ROL_DAYS_SETTING_ADDR, &rolling_days, sizeof(rolling_days));
		log("Load days %d\n", rolling_days);
		update_screen_mode();
		status = 1;
	}
	else if (status >= 100)
		update_screen_mode();
}

static void on_evt_mode(int key)
{
	if (status >= 100)
		return;

	if (key == KEY_ESC) {
		status = 101;
		ticks_exec = k_ticks();
		return;
	}

	switch (status) {
		case 1:
			if (key == KEY_UP && current_mode < 4)
				current_mode++;
			else if (key == KEY_DOWN && current_mode > 0)
				current_mode--;
			else if (key == KEY_ENTER) {
				if (current_mode == 3)
					status = 2;
				else {
					status = 100;
					ticks_exec = k_ticks();
				}
			}
			break;

		case 2:
			if (key == KEY_UP && rolling_days < 200)
				rolling_days++;
			else if (key == KEY_DOWN && rolling_days > 1)
				rolling_days--;
			else if (key == KEY_ENTER) {
				log("Save days %d\n", rolling_days);
				eeprom_write(EEPROM_PWM_ROL_DAYS_SETTING_ADDR, &rolling_days, sizeof(rolling_days));
				eeprom_write(EEPROM_PWM_ROL_DAYS_STATUS_ADDR, &rolling_days, sizeof(rolling_days));
				status = 100;
				ticks_exec = k_ticks();
			}
			break;

		default:
			break;
	}
	if (current_mode != 3 && current_mode != 4)
		eeprom_read(EEPROM_PWM_MODE0_ADDR + current_mode * sizeof(mode_pwm_cfg),
			(u8*)&mode_pwm_cfg, sizeof(mode_pwm_cfg));

	if (key == KEY_ENTER && (status == 2 || current_mode != 3)) {
		log("Save mode %d\n", current_mode);
		if (current_mode == 3)
			rolling_start(0);
		else
			rolling_stop();
		eeprom_write(EEPROM_PWM_CURRENT_MODE_ADDR, &current_mode, 1);
		if (current_mode != 3 && current_mode != 4) {
			pwm_set(mode_pwm_cfg.freq, mode_pwm_cfg.duty);
			eeprom_write(EEPROM_PWM_STATUS_MODE_ADDR, &current_mode, 1);
		}
		else if (current_mode == 4) {
			pwm_set(0, 0);
			ant_check_enable(0);
		}
	}

	update_screen_mode();
	keys_clear_evts(1 << key);
}

static void refresh_realtimesens(void)
{
	struct rfrx_frame_t *f;
	if (status == 0) {
		rfrx_clear_lastframe();
		status = 1;
	}

	f = rfrx_get_lastframe();
	if (f) {
		rfrx_frame_display(f);
		rfrx_clear_lastframe();
	}
}

static void refresh_reset(void)
{
	if (status == 100) {
		status = 102;
		lcd_write_line("ESEGUITO", 0, 1);
	}
	else if (status == 101) {
		status = 102;
		lcd_write_line("ANNULLATO", 0, 1);
	}

	else if (status >= 100 && k_elapsed(ticks_exec) > MS_TO_TICKS(1000))
		on_evt_def(KEY_ESC);

}

static void on_evt_reset_storici(int key)
{
	switch (status) {

		case 0:
			if (key == KEY_ENTER) {
				status = 100;
				db_alarm_reset();
				db_avvii_reset();
				db_data_reset(0);
				ticks_exec = k_ticks();
			}
			else if (key == KEY_ESC) {
				status = 101;
				ticks_exec = k_ticks();
			}
			break;

		case 100:

		default:
			break;
	}

	keys_clear_evts(1 << key);
}

static void on_evt_reset_contatore(int key)
{
	u32 tmp = 0;
	switch (status) {

		case 0:
			if (key == KEY_ENTER) {
				status = 100;
				eeprom_write(EEPROM_HOURS_ADDR, &tmp, sizeof(tmp));
				ticks_exec = k_ticks();
			}
			else if (key == KEY_ESC) {
				status = 101;
				ticks_exec = k_ticks();
			}
			break;

		case 100:

		default:
			break;
	}

	keys_clear_evts(1 << key);
}

static int do_refresh;
static int show_type; /* 0: avvii, 1: alarms */

static void refresh_show_avvii(void)
{
	int pos;
	struct alarm_t a;

	if (status == 0) {
		show_type = 0;
		pos = db_avvii_get(&a, -1);
		if (pos != DB_POS_INVALID)
			status = BIT30 | pos;
		else
			status = -1;
	}
	else if (!do_refresh)
		return;

	do_refresh = 0;

	if (status < 0) {
		lcd_write_line(STR_EMPTY, 0, 1);
		return;
	}
	pos = status & ~BIT30;
	pos = db_avvii_get(&a, pos);

	db_alarm_display(&a);
}

static void refresh_show_alarms(void)
{
	int pos;
	struct alarm_t a;

	if (status == 0) {
		show_type = 1;
		pos = db_alarm_get(&a, -1);
		if (pos != DB_POS_INVALID)
			status = BIT30 | pos;
		else
			status = -1;
	}
	else if (!do_refresh)
		return;

	do_refresh = 0;

	if (status < 0) {
		lcd_write_line(STR_EMPTY, 0, 1);
		return;
	}
	pos = status & ~BIT30;
	pos = db_alarm_get(&a, pos);

	db_alarm_display(&a);
}

static void on_evt_show(int key)
{
	int pos;
	struct alarm_t a;
	int max;

	if (status < 0) {
		on_evt_def(key);
		return;
	}

	if (show_type)
		max = ALRM_MAX_NUM;
	else
		max = AVVII_MAX_NUM;

	pos = status & ~BIT30;

	if (key == KEY_ESC) {
		on_evt_def(key);
		return;
	}
	else if (key == KEY_UP)
		pos = (pos + 1) % max;
	else if (key == KEY_DOWN) {
		if (pos == 0)
			pos = max - 1;
		else
			pos--;
	}

	keys_clear_evts(1 << key);

	if (show_type)
		db_alarm_get(&a, pos);
	else
		db_avvii_get(&a, pos);

	if (a.type != ALRM_TYPE_INVALID) {
		status = BIT30 | pos;
		do_refresh = 1;
	}
}

static int ent_pressed;
static u32 ent_pressed_ticks = 0;
static int show_data_page = 1;

static void refresh_show_data(void)
{
	int pos;
	struct data_t d;

	if (ent_pressed && !keys_get_stat(KEY_ENTER))
			ent_pressed = 0;

	if (ent_pressed && k_elapsed(ent_pressed_ticks) >= MS_TO_TICKS(5000)) {
		ent_pressed = 0;
		show_data_page = 2;
		do_refresh = 1;
	}

	if (status == 0) {
		pos = db_data_get(&d, -1);
		if (pos != DB_POS_INVALID)
			status = BIT30 | pos;
		else
			status = -1;
	}
	else if (!do_refresh)
		return;

	do_refresh = 0;

	if (status < 0) {
		lcd_write_line(STR_EMPTY, 0, 1);
		return;
	}
	pos = status & ~BIT30;
	pos = db_data_get(&d, pos);

	db_data_display(&d, show_data_page);
}

static void on_evt_show_data(int key)
{
	int pos;

	struct data_t d;

	if (status < 0) {
		on_evt_def(key);
		return;
	}

	pos = status & ~BIT30;

	if (key == KEY_ESC) {
		if (show_data_page == 1) {
			on_evt_def(key);
			return;
		}
		else {
			show_data_page = 1;
			do_refresh = 1;
		}
	}
	else if (key == KEY_UP)
		pos = (pos + 1) % DATA_MAX_NUM;
	else if (key == KEY_DOWN) {
		if (pos == 0)
			pos = DATA_MAX_NUM - 1;
		else
			pos--;
	}
	else if (key == KEY_ENTER) {
		ent_pressed_ticks = k_ticks();
		ent_pressed = 1;
	}

	keys_clear_evts(1 << key);

	pos = db_data_get(&d, pos);

	if (pos != DB_POS_INVALID) {
		status = BIT30 | pos;
		do_refresh = 1;
	}
}

static int pwd_secret[5] = {KEY_UP, KEY_ESC, KEY_DOWN, KEY_ESC, KEY_ENTER};
static int pwd_entered[5];

static void on_evt_pwd(int key)
{
	switch (status) {
		case 0:
			if (key == KEY_ESC) {
				lcd_write_line(STR_PASSWORD, 0, 0);
				lcd_write_line("", 1, 0);
				lcd_cursor(0, 11, 1);
				status = 1;
			}
			else
				on_evt_def(KEY_ESC);
			break;

		default:
			pwd_entered[status - 1] = key;
			lcd_write_string("*");
			lcd_cursor(0, 11 + status, 1);
			status++;
			if (status == 6) {
				if (!memcmp(pwd_entered, pwd_secret, sizeof(pwd_entered))) {
					log("Password ko\n");
					on_evt_def(KEY_ENTER);
				}
				else {
					log("Password ok\n");
					on_evt_def(KEY_ESC);
				}
			}

			break;
	}
	keys_clear_evts(1 << key);
}

static void refresh_pwd(void)
{
}

static struct menu_voice_t menu[] = {
	{0, {"MENU", "IMPOSTAZIONI"}, on_evt_def, NULL, {4, 1, -1, 24}, 1},
	{1, {"VISUALIZZA", "STORICO AVVII"}, on_evt_def, NULL, {0, 2, -1, 19}, 1},
	{2, {"VISUALIZZA", "SEGNALAZIONI"}, on_evt_def, NULL, {1, 3, -1, 20}, 1},
	{3, {"VISUALIZZA", "STORICO LETTURE"}, on_evt_def, NULL, {2, 4, -1, 22}, 1},
	{4, {"VISUALIZZA", "REALTIME SENSORI"}, on_evt_def, NULL, {3, 0, -1, 16}, 1},
	{5, {"IMPOSTAZIONI", "PARAMETRI F."}, on_evt_def, NULL, {13, 6, 0, 15}, 1},
	{6, {"IMPOSTAZIONI", "MODALITA'"}, on_evt_def, NULL, {5, 7, 0, 23}, 1},
	{7, {"IMPOSTAZIONI", "DATA E ORA"}, on_evt_def, NULL, {6, 8, 0, 14}, 1},
	{8, {"IMPOSTAZIONI", "RESET CONTATORE"}, on_evt_def, NULL, {7, 9, 0, 18}, 1},
	{9, {"IMPOSTAZIONI", "RESET STORICI"}, on_evt_def, NULL, {8, 10, 0, 17}, 1},
	{10, {"IMPOSTAZIONI", "BLUETOOTH"}, on_evt_def, NULL, {9, 11, 0, -1}, 0},
	{11, {"IMPOSTAZIONI", "COMUNICAZIONI RF"}, on_evt_def, NULL, {10, 12, 0, -1}, 0},
	{12, {"IMPOSTAZIONI", "VERSIONE FW"}, on_evt_def, NULL, {11, 13, 0, 21}, 1},
	{13, {"IMPOSTAZIONI", "TEST PWM"}, on_evt_def, NULL, {12, 5, 0, -1}, 0},
	{14, {"", ""}, on_evt_datetime, refresh_datetime, {-1, -1, 7, 7}, 1},
	{15, {"", ""}, on_evt_pwm, refresh_pwm, {-1, -1, 5, 5}, 1},
	{16, {"Attendere...", "Comunicazione"}, on_evt_def, refresh_realtimesens, {-1, -1, 4, 4}, 1,
	    .timeout_ms = (30 * 60 * 1000),},
	{17, {STR_CONFIRM, "RESET STORICI"}, on_evt_reset_storici, refresh_reset, {-1, -1, 9, 9}, 1},
	{18, {STR_CONFIRM, "RESET CONTATORE"}, on_evt_reset_contatore, refresh_reset, {-1, -1, 8, 8}, 1},
	{19, {"", ""}, on_evt_show, refresh_show_avvii, {-1, -1, 1, -1}, 1},
	{20, {"", ""}, on_evt_show, refresh_show_alarms, {-1, -1, 2, -1}, 1},
	{21, {"Git:  " GIT_VERSION, "Date: " COMPILE_DATE}, on_evt_def, NULL, {-1, -1, 12, -1}, 1},
	{22, {"", ""}, on_evt_show_data, refresh_show_data, {-1, -1, 3, -1}, 1},
	{23, {"", ""}, on_evt_mode, refresh_mode, {-1, -1, 6, 6}, 1},
	{24, {"MENU", "IMPOSTAZIONI"}, on_evt_pwd, refresh_pwd, {-1, -1, -1, 5}, 1},
	{-1}
};

void menu_start(struct task_t *t)
{
}

void menu_step(struct task_t *t)
{
	u32 k;
	int i;

	if (get_standby()) {
		cur_menu = NULL;
		return;
	}

	k = keys_get_evts();

	if (k && !cur_menu) {
		if (k & (1 << KEY_ESC))
			set_standby(1);
		else {
			cur_menu = get_menu_voice(MENU_VOICE_DEFAULT);
			if (!cur_menu)
				return;
			lcd_write_line(cur_menu->text[0], 0, 1);
			lcd_write_line(cur_menu->text[1], 1, 1);
		}
		keys_clear_evts(k);
		return;
	}

	if (!cur_menu)
		return;

	if (k && cur_menu->on_evt) {
		for (i = KEY_UP; i <= KEY_ENTER; i++) {
			if (k & (1 << i))
				cur_menu->on_evt(i);
		}
	}

	if (cur_menu->refresh)
		cur_menu->refresh();
}

struct task_t attr_tasks task_menu = {
	.start = menu_start,
	.step = menu_step,
	.intvl_ms = 5,
	.name = "menu",
};

u32 get_menu_timeout_ms(void)
{
	if (cur_menu && cur_menu->timeout_ms != 0)
		return cur_menu->timeout_ms;

	return DEF_TIMEOUT_MS; /* default timeout 1 min */
}
