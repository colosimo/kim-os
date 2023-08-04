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
#include "ant.h"
#include "rfrx.h"
#include "db.h"
#include "eeprom.h"
#include "bluetooth.h"
#include "deadline.h"
#include "osm.h"

static int status = 0; /* Generic state machine variable */
static u32 ticks_exec;

#define MENU_VOICE_DEFAULT 10
#define MENU_VOICE_PWD 14
#define MENU_VOICE_ACTIVE_ALARMS 39
#define STR_CONFIRM "CONFERMA?"
#define STR_EMPTY "ARCHIVIO VUOTO"
#define STR_PASSWORD "PASSWORD: [     ]"
#define GENERAL_UNLOCK_CODE "51111"

#define DEF_TIMEOUT_MS (60 * 1000) /* 1 minute */

struct menu_voice_t {
	int id;
	const char text[2][21];
	void (*on_evt)(int evt);
	void (*refresh)(void);
	int id_next[4]; /* Next menu voice to be called on: UP, DOWN, ESC, ENTER */
	int enabled;
	void (*on_enter)(int evt);
};

static struct menu_voice_t menu[];

static struct menu_voice_t *cur_menu = NULL;

extern int deadline_lock;
extern int deadline_idx;
extern u32 hours;

static struct menu_voice_t *get_menu_voice(int id);

static int active_alarms;

void reset_active_alarms(void)
{
	active_alarms = get_alarm_bitfield();
}

void show_home(void)
{
	char buf[24];
	u16 mA1, mA2;
	u8 en_def_out, en_osm;
	status = 0;

	if (active_alarms) {
		cur_menu = get_menu_voice(MENU_VOICE_ACTIVE_ALARMS);
		return;
	}

	osm_measure(OSM_CH1, NULL, &mA1, NULL);
	osm_measure(OSM_CH2, NULL, &mA2, NULL);
	eeprom_read(EEPROM_HOURS_ADDR, (u8*)&hours, sizeof(hours));

	eeprom_read(EEPROM_ENABLE_DEF_OUT, &en_def_out, 1);
	eeprom_read(EEPROM_ENABLE_OSM, &en_osm, 1);

	k_sprintf(buf, "Elo srl Centr: %d%c", en_osm, en_def_out ? 'D' : ' ');
	lcd_write_line(buf, 0, 0);
	k_sprintf(buf, "%04d GG %03d mA   %c",
	    (uint) hours / 24, (uint)(mA1 + mA2), dl_isactive() ? 'T' : ' ');
	lcd_write_line(buf, 1, 0);
}

static struct menu_voice_t *get_menu_voice(int id)
{
	int i;

	if (id < 0)
		return NULL;

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

	if (new_menu->id_next[key] < 0)
		goto done;

	do {
		new_menu = get_menu_voice(new_menu->id_next[key]);
		if (new_menu && new_menu->enabled) {
			status = 0;
			cur_menu = new_menu;
			lcd_write_line(cur_menu->text[0], 0, 0);
			lcd_write_line(cur_menu->text[1], 1, 0);
			if (cur_menu->on_enter)
				cur_menu->on_enter(key);
		}
	} while(new_menu && !new_menu->enabled);

done:
	keys_clear_evts(1 << key);
}

/* Show main info */

static void refresh_info()
{
	char buf[24];
	u32 bluetooth_id;
	u32 temp;
	struct rtc_t r;
	rtc_get(&r);
	eeprom_read(EEPROM_BLUETOOTH_ID, &bluetooth_id, 4);
	osm_measure(OSM_CH1, NULL, NULL, &temp);
	k_sprintf(buf, "S/N: %05d", (uint)bluetooth_id);
	lcd_write_line(buf, 0, 0);

	k_sprintf(buf, "V:%s %02d/%02d/%02d T:%02d",
		HUMAN_VERSION, r.day, r.month, r.year, (uint)temp);
	lcd_write_line(buf, 1, 0);
	status = 1;
}

/* Show measures */

static void refresh_measures()
{
	char buf[24];
	u32 v[OSM_CH2 + 1];
	u16 a[OSM_CH2 + 1];
	u32 t[OSM_CH2 + 1];
	int i;

	if (status == 0) {
		for (i = OSM_CH1; i <= OSM_CH2; i++)
			osm_measure(i, &v[i], &a[i], &t[i]);

		k_sprintf(buf, "V1: %02d.%d V2: %02d.%d",
		    (uint)v[OSM_CH1] / 1000, (uint)(v[OSM_CH1] % 1000) / 100,
		    (uint)v[OSM_CH2] / 1000, (uint)(v[OSM_CH2] % 1000) / 100);
		lcd_write_line(buf, 0, 0);

		k_sprintf(buf, "mA1: %03d mA2: %03d", (uint)a[OSM_CH1], (uint)a[OSM_CH2]);
		lcd_write_line(buf, 1, 0);

		status = 1;
	}
}

/* Show def rolling and mode */

static void refresh_def_rolling()
{
	u8 mode, st;
	char buf[24];

	if (status == 0) {
		eeprom_read(EEPROM_ANT_CURRENT_MODE_ADDR, &mode, 1);
		if (mode != 4) {
			eeprom_read(EEPROM_ANT_STATUS_MODE_ADDR, &st, 1);
			k_sprintf(buf, "DEF %c%c", mode == 3 ? 'R' : ' ', '0' + st + 1);
		}
		else
			k_sprintf(buf, "DEF OFF");
		lcd_write_line(buf, 0, 0);
	}
}


/* Date / Time setting */

static struct rtc_t r;
static u32 ticks_cancel;
char cont[6];

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

	k_sprintf(buf, "D %02d/%02d/%02d H %02d:%02d",
	    r.day, r.month, r.year, r.hour, r.min);
	lcd_write_line(buf, 0, 0);
	k_sprintf(buf, "Cont %s", cont);
	lcd_write_line(buf, 1, 0);

	switch(status) {
		case 0:
		case 1: cur_line = 0; cur_pos = 2; cur_show = 1; break;
		case 2: cur_line = 0; cur_pos = 5; cur_show = 1; break;
		case 3: cur_line = 0; cur_pos = 8; cur_show = 1; break;
		case 4: cur_line = 0; cur_pos = 13; cur_show = 1; break;
		case 5: cur_line = 0; cur_pos = 16; cur_show = 1; break;
		case 6:
		case 7:
		case 8:
		case 9:
			cur_line = 1; cur_pos = 5 + status - 6; cur_show = 1; break;
		case 10: cur_line = 1; cur_pos = 17; cur_show = 0; break;
		case 99:
		case 101:
		default: cur_line = cur_pos = cur_show = 0; break; /* Never happens */
	}
	lcd_cursor(cur_line, cur_pos, cur_show);

	if (status == 10)
		lcd_write_string("OK?");
}

static void refresh_datetime()
{
	if (status == 0) {
		rtc_get(&r);
		eeprom_read(EEPROM_HOURS_ADDR, (u8*)&hours, sizeof(hours));
		k_sprintf(cont, "%04d", (uint)(hours / 24));
		status = 99; /* Idle */
		update_screen_datetime();
	}
	else if (status >= 100 && k_elapsed(ticks_cancel) > MS_TO_TICKS(1000)) {
		status = 99;
		update_screen_datetime();
	}
}

static void on_evt_datetime(int key)
{
	u8 digit;
	switch (status) {
		case 99:
			if (key == KEY_ENTER) {
				status = 1;
				update_screen_datetime();
				keys_clear_evts(1 << key);
			}
			else
				on_evt_def(key);
			return;
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
		case 7:
		case 8:
		case 9:
			digit = cont[status - 6];
			if (key == KEY_UP)
				digit++;
			else if (key == KEY_DOWN)
				digit--;

			if (digit < '0')
				digit = '9';
			else if (digit > '9')
				digit = '0';
			cont[status - 6] = digit;
			break;

		case 10:
			if (key == KEY_ENTER) {
				if (rtc_valid(&r)) {
					r.sec = 0;
					rtc_dump(&r);
					rtc_set(&r);
					hours = atoi(cont) * 24;
					eeprom_write(EEPROM_HOURS_ADDR, (u8*)&hours, sizeof(hours));
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

	if (status < 10 && key == KEY_ENTER)
		status++;

	update_screen_datetime();
	keys_clear_evts(1 << key);

}

/* DEF PWM ANT setting */

static int status_mode;
static int menu_mode = 0;
static struct ant_cfg_t menu_p;

static void update_screen_ant()
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

static void refresh_ant(void)
{
	if (status == 0) {
		eeprom_read(EEPROM_ANT_STATUS_MODE_ADDR, &status_mode, 1);
		menu_mode = status_mode;
		eeprom_read(EEPROM_ANT_MODE0_ADDR + menu_mode * sizeof(menu_p),
		    (u8*)&menu_p, sizeof(menu_p));
		update_screen_ant();
		status = 1;
	}
}

static void on_evt_ant(int key)
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
		ant_check(&menu_p.freq, &menu_p.duty);
		if (menu_mode == status_mode)
			ant_set(menu_p.freq, menu_p.duty);
		log("Save mode %d: %d %d\n", menu_mode + 1, (uint)menu_p.freq, (uint)menu_p.duty);
		eeprom_write(EEPROM_ANT_MODE0_ADDR + menu_mode * sizeof(menu_p),
			(u8*)&menu_p, sizeof(menu_p));

		if (status == 1) {
			menu_mode = (menu_mode + 1) % 3;
			eeprom_read(EEPROM_ANT_MODE0_ADDR + menu_mode * sizeof(menu_p),
			    (u8*)&menu_p, sizeof(menu_p));
		}

	}
	update_screen_ant();
	keys_clear_evts(1 << key);
}

/* Modality choice */

static int current_mode; /* Mode 0, 1, 2, Rolling (mode = 3), Logger (mode = 4) */
static int rolling_days;
static struct ant_cfg_t mode_ant_cfg;

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
		    (uint)mode_ant_cfg.freq, (uint)mode_ant_cfg.duty);
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
		eeprom_read(EEPROM_ANT_CURRENT_MODE_ADDR, &current_mode, 1);
		eeprom_read(EEPROM_ANT_MODE0_ADDR + current_mode * sizeof(mode_ant_cfg),
		    (u8*)&mode_ant_cfg, sizeof(mode_ant_cfg));
		eeprom_read(EEPROM_ANT_ROL_DAYS_SETTING_ADDR, &rolling_days, sizeof(rolling_days));
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
				eeprom_write(EEPROM_ANT_ROL_DAYS_SETTING_ADDR, &rolling_days, sizeof(rolling_days));
				eeprom_write(EEPROM_ANT_ROL_DAYS_STATUS_ADDR, &rolling_days, sizeof(rolling_days));
				status = 100;
				ticks_exec = k_ticks();
			}
			break;

		default:
			break;
	}
	if (current_mode != 3 && current_mode != 4)
		eeprom_read(EEPROM_ANT_MODE0_ADDR + current_mode * sizeof(mode_ant_cfg),
			(u8*)&mode_ant_cfg, sizeof(mode_ant_cfg));

	if (key == KEY_ENTER && (status == 2 || current_mode != 3)) {
		log("Save mode %d\n", current_mode);
		if (current_mode == 3)
			rolling_start(0);
		else
			rolling_stop();
		eeprom_write(EEPROM_ANT_CURRENT_MODE_ADDR, &current_mode, 1);
		if (current_mode != 3 && current_mode != 4) {
			ant_set(mode_ant_cfg.freq, mode_ant_cfg.duty);
			eeprom_write(EEPROM_ANT_STATUS_MODE_ADDR, &current_mode, 1);
		}
		else if (current_mode == 4) {
			ant_set(MIN_FREQ, 0);
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

#if 0

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
				db_ant_reset(1);
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
#endif

static int do_refresh;
static int show_type; /* 0: avvii, 1: alarms */

static void refresh_show_avvii(void)
{
	int pos;
	struct avvii_t a;

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

	db_avvii_display(&a, pos);
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

	db_alarm_display(&a, pos);
}

static void on_evt_show(int key)
{
	int pos;
	struct alarm_t a;
	struct avvii_t av;
	int max;
	int ret;

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
		ret = db_alarm_get(&a, pos);
	else
		ret = db_avvii_get(&av, pos);

	if (ret != DB_POS_INVALID) {
		status = BIT30 | pos;
		do_refresh = 1;
	}
}

static int ent_pressed;
static u32 ent_pressed_ticks = 0;
static int show_data_page = 1;

static void refresh_show_data_ant(void)
{
	int pos;
	struct data_ant_t d;

	if (ent_pressed && !keys_get_stat(KEY_ENTER))
			ent_pressed = 0;

	if (ent_pressed && k_elapsed(ent_pressed_ticks) >= MS_TO_TICKS(5000)) {
		ent_pressed = 0;
		show_data_page = 2;
		do_refresh = 1;
	}

	if (status == 0) {
		pos = db_ant_get(&d, -1);
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
	pos = db_ant_get(&d, pos);

	db_ant_display(&d, show_data_page);
}

static void on_evt_show_data_ant(int key)
{
	int pos;

	struct data_ant_t d;

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
		pos = (pos + 1) % ANT_MAX_NUM;
	else if (key == KEY_DOWN) {
		if (pos == 0)
			pos = ANT_MAX_NUM - 1;
		else
			pos--;
	}
	else if (key == KEY_ENTER) {
		ent_pressed_ticks = k_ticks();
		ent_pressed = 1;
	}

	keys_clear_evts(1 << key);

	pos = db_ant_get(&d, pos);

	if (pos != DB_POS_INVALID) {
		status = BIT30 | pos;
		do_refresh = 1;
	}
}

static void refresh_show_data_osm(void)
{
	int pos;
	struct data_osm_t d;

	if (ent_pressed && !keys_get_stat(KEY_ENTER))
			ent_pressed = 0;

	if (status == 0) {
		pos = db_osm_get(&d, -1);
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
	pos = db_osm_get(&d, pos);

	db_osm_display(&d, pos);
}

static void on_evt_show_data_osm(int key)
{
	int pos;

	struct data_osm_t d;

	if (status < 0) {
		on_evt_def(key);
		return;
	}

	pos = status & ~BIT30;

	if (key == KEY_ESC)
		on_evt_def(key);
	else if (key == KEY_UP)
		pos = (pos + 1) % OSM_MAX_NUM;
	else if (key == KEY_DOWN) {
		if (pos == 0)
			pos = OSM_MAX_NUM - 1;
		else
			pos--;
	}
	else if (key == KEY_ENTER) {
		ent_pressed_ticks = k_ticks();
		ent_pressed = 1;
	}

	keys_clear_evts(1 << key);

	pos = db_osm_get(&d, pos);

	if (pos != DB_POS_INVALID) {
		status = BIT30 | pos;
		do_refresh = 1;
	}
}


#if 0
static int pwd_secret[5] = {KEY_UP, KEY_ESC, KEY_DOWN, KEY_ESC, KEY_ENTER};
static int pwd_entered[5];

static void attr_unused on_evt_pwd(int key)
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
					dbg("Password ok\n");
					on_evt_def(KEY_ENTER);
				}
				else {
					dbg("Password ko\n");
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
#endif

static void on_evt_avg_en(int key)
{
	u8 en_daily_avg;

	if (key == KEY_ESC)
		on_evt_def(key);

	else if (key == KEY_UP || key == KEY_DOWN || key == KEY_ENTER) {
		eeprom_read(EEPROM_ENABLE_DAILY_AVG, &en_daily_avg, 1);
		en_daily_avg = !en_daily_avg;
		eeprom_write(EEPROM_ENABLE_DAILY_AVG, &en_daily_avg, 1);
		status = 0; /* Force refresh */
	}

	keys_clear_evts(1 << key);
}

static void refresh_avg_en(void)
{
	char buf[24];
	u8 en_daily_avg;
	if (status == 0) {
		eeprom_read(EEPROM_ENABLE_DAILY_AVG, &en_daily_avg, 1);
		k_sprintf(buf, "MEDIA GIORN.: %s", en_daily_avg ? "SI" : "NO");
		lcd_write_line(buf, 0, 0);
		status = 1;
	}

}

#if 0

static void on_evt_data_dump(int key)
{
	u32 id;
	struct rtc_t r;
	if (key == KEY_ENTER) {
		ant_check_enable(0);
		eeprom_read(EEPROM_BLUETOOTH_ID, &id, 4);
		id &= 0xffff;
		lcd_write_line("INVIO DATI...", 1, 1);
		k_delay(100);
		kprint("\r\n\r\n===== ELO %05d DATA BEGIN ", (uint)id);
		rtc_get(&r);
		rtc_dump_kprint(&r);
		kprint(" =====");
		db_avvii_dump_all();
		db_ant_dump_all();
		db_alarm_dump_all();
		lcd_write_line("INVIO DATI FINITO", 1, 1);
		on_evt_def(KEY_ESC);
		kprint("\r\n===== ELO %05d DATA END =====\r\n", (uint)id);
		ant_check_enable(1);
	}
	else
		on_evt_def(key);
	keys_clear_evts(1 << key);
}

static void refresh_info_readonly()
{
	char buf[24];
	u32 bluetooth_id;
	struct rtc_t r;
	if (status == 0) {
		eeprom_read(EEPROM_BLUETOOTH_ID, &bluetooth_id, 4);
		rtc_get(&r);
		k_sprintf(buf, "%05d  %s %s", (u16) bluetooth_id, HUMAN_VERSION, GIT_VERSION);
		lcd_write_line(buf, 0, 0);

		k_sprintf(buf, "%02d/%02d/%02d %02d:%02d", r.day, r.month, r.year,
		    r.hour, r.min);
		lcd_write_line(buf, 1, 0);


		status = 1;
	}
}
#endif
/* Deadlines settings ("Funzionamento Temporizzato")*/
static int dl_idx = 0;
static struct deadline_t dl;

static void update_screen_deadline()
{
	char buf[24];
	int cur_line, cur_pos, cur_show;
	struct rtc_t *rtc;

	rtc = &dl.rtc;

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

	k_sprintf(buf, "%d %s  Cod: %s", dl_idx + 1, dl.enable ? "ON " : "OFF", dl.code);
	lcd_write_line(buf, 0, 0);
	k_sprintf(buf, "%02d/%02d/%02d %02d:%02d", rtc->day, rtc->month, rtc->year, rtc->hour, rtc->min);
	lcd_write_line(buf, 1, 0);

	switch(status) {
		case 0:
		case 1: cur_line = 0; cur_pos = 0; cur_show = 1; break;
		case 2: cur_line = 0; cur_pos = 2; cur_show = 1; break;
		case 3: cur_line = 0; cur_pos = 12; cur_show = 1; break;
		case 4: cur_line = 0; cur_pos = 13; cur_show = 1; break;
		case 5: cur_line = 0; cur_pos = 14; cur_show = 1; break;
		case 6: cur_line = 0; cur_pos = 15; cur_show = 1; break;
		case 7: cur_line = 0; cur_pos = 16; cur_show = 1; break;
		case 11: cur_line = 1; cur_pos = 0; cur_show = 1; break;
		case 12: cur_line = 1; cur_pos = 3; cur_show = 1; break;
		case 13: cur_line = 1; cur_pos = 6; cur_show = 1; break;
		case 14: cur_line = 1; cur_pos = 9; cur_show = 1; break;
		case 15: cur_line = 1; cur_pos = 12; cur_show = 1; break;
		case 16: cur_line = 1; cur_pos = 17; cur_show = 0; break;
		case 101: cur_show = 0;
		default: cur_line = cur_pos = cur_show = 0; break; /* Never happens */
	}
	lcd_cursor(cur_line, cur_pos, cur_show);

	if (status == 16)
		lcd_write_string("OK?");
}

static void refresh_deadline()
{
	if (status == 0) {
		dl_idx = 0;
		dl_get(0, &dl);
		update_screen_deadline();
		status = 1;
	}
	else if (status >= 100 && k_elapsed(ticks_cancel) > MS_TO_TICKS(1000))
			on_evt_def(KEY_ESC);
}

static void on_evt_deadline(int key)
{
	struct rtc_t *rtc;
	u8 digit;
	rtc = &dl.rtc;

	switch (status) {
		case 1:
			if (key == KEY_UP)
				dl_idx = (dl_idx + 1) % 3;
			else if (key == KEY_DOWN) {
				if (dl_idx == 0)
					dl_idx = 2;
				else
					dl_idx--;
			}
			dl_get(dl_idx, &dl);
			break;
		case 2:
			if (key == KEY_UP || key == KEY_DOWN)
				dl.enable = !dl.enable;
			break;

		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
			digit = dl.code[status - 3] - '0';
			if (key == KEY_UP)
				digit = (digit + 1) % 10;
			else if (key == KEY_DOWN) {
				if (digit == 0)
					digit = 9;
				else
					digit--;
			}
			dl.code[status - 3] = digit + '0';
			break;

		case 11:
			if (key == KEY_UP)
				rtc->day = (rtc->day % 31) + 1;
			else if (key == KEY_DOWN) {
				if (rtc->day == 1)
					rtc->day = 31;
				else
					rtc->day--;
			}
			break;
		case 12:
			if (key == KEY_UP)
				rtc->month = (rtc->month % 12) + 1;
			else if (key == KEY_DOWN) {
				if (rtc->month == 1)
					rtc->month = 12;
				else
					rtc->month--;
			}
			break;
		case 13:
			if (key == KEY_UP)
				rtc->year = rtc->year % 100 + 1;
			else if (key == KEY_DOWN) {
				if (rtc->year == 0)
					rtc->year = 99;
				else
					rtc->year--;
			}
			break;

		case 14:
			if (key == KEY_UP)
				rtc->hour = (rtc->hour + 1) % 24;
			else if (key == KEY_DOWN) {
				if (rtc->hour == 0)
					rtc->hour = 23;
				else
					rtc->hour--;
			}
			break;

		case 15:
			if (key == KEY_UP)
				rtc->min = (rtc->min + 1) % 60;
			else if (key == KEY_DOWN) {
				if (rtc->min == 0)
					rtc->min = 59;
				else
					rtc->min--;
			}
			break;

		case 16:
			if (key == KEY_ENTER) {
				if (rtc_valid(&r)) {
					rtc->sec = 0;
					dl_set(dl_idx, &dl);
					rtc_dump(rtc);
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

	if (key == KEY_ENTER) {
		if (status == 7)
			status = 11;
		else if (status < 7 || (status >= 11 && status <= 16 ))
			status++;
	}
	update_screen_deadline();
	keys_clear_evts(1 << key);
}

/* Deadlines Code (password to unlock) */
static u8 dl_code[6];
static int dl_to_unlock = -1;
static struct deadline_t dl;

static void update_screen_dl_code()
{
	char buf[24];
	if (dl_to_unlock < 0)
		return;

	dl_get(dl_to_unlock, &dl);
	k_sprintf(buf, "Timeout %02d/%02d/%02d", dl.rtc.day, dl.rtc.month, dl.rtc.year);
	lcd_write_line(buf, 0, 0);

	k_sprintf(buf, "Cod. Sblocco %s", dl_code);
	lcd_write_line(buf, 1, 0);

	lcd_cursor(1, status + 12, 1);
}

static void refresh_dl_code()
{
	if (status == 0) {
		if (dl_to_unlock < 0) {
			on_evt_def(KEY_DOWN);
			return;
		}

		status = 1;
		strcpy((char*)dl_code, "00000");
		update_screen_dl_code();
	}
}

static void on_evt_dl_code(int key)
{
	u8 digit;
	int ret;

	if (key == KEY_ESC) {
		on_evt_def(key);
		return;
	}

	switch (status) {
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
			digit = dl_code[status - 1] - '0';
			if (key == KEY_UP)
				digit = (digit + 1) % 10;
			else if (key == KEY_DOWN) {
				if (digit == 0)
					digit = 9;
				else
					digit--;
			}
			dl_code[status - 1] = digit + '0';
			break;


		case 6:
			keys_clear_evts(1 << key);
			break;

		default:
			break;
	}
	if (key == KEY_ENTER) {
		if (status == 5) {
			if (dl_to_unlock >= 0)
				ret = dl_unlock(dl_to_unlock, dl_code);
			else {
				if (strcmp((char*)dl_code, GENERAL_UNLOCK_CODE) == 0) {
					ret = 1;
					dl_disable_all();
				}
				else
					ret = 0;
			}

			if (ret)
				lcd_write_line("PASSWORD OK", 0, 1);
			else
				lcd_write_line("PASSWORD ERRATA", 0, 1);
			k_delay_us(1000000);
			on_evt_def(KEY_ENTER);
			keys_clear_evts(1 << key);
			return;
		}
		else
			status++;
	}
	update_screen_dl_code();
	keys_clear_evts(1 << key);
}

/* PWM OSM Setting Begin */

static int osm_ch = OSM_CH1;
static struct osm_cfg_t menu_osm;
static int osm_cursor_pos;

static void update_osm(void)
{
	char buf[24];
	u32 v, temp;
	u16 a;

	osm_measure(osm_ch, &v, &a, &temp);

	k_sprintf(buf, "C%d F:%03d T:%03d D:%03d", osm_ch + 1, (uint)menu_osm.freq,
		(uint)menu_osm.volt_perc, (uint)menu_osm.duty);
	lcd_write_line(buf, 0, 0);

	k_sprintf(buf, "T:%d.%dV C=%03dmA", (uint)v / 1000, (uint)(v % 1000) / 100, (uint)a);
	lcd_write_line(buf, 1, 0);

	if (osm_cursor_pos > 0)
		lcd_cursor(0, osm_cursor_pos, 1);
	else
		lcd_cursor(0, 0, 0);
}

static void refresh_osm(void)
{
	if (status == 0) {
		eeprom_read(EEPROM_OSM_CH1_CFG + 0x10 * osm_ch, &menu_osm, sizeof(menu_osm));
		status = 1;
	}
	update_osm();
}

static void enter_osm_double_ch_page(int key)
{
	if (key == KEY_ENTER || key == KEY_DOWN)
		osm_ch = OSM_CH1;
	else
		osm_ch = OSM_CH2;
}

static void on_evt_osm(int key)
{
	if (key == KEY_ESC) {
		osm_cursor_pos = 0;

		if (status == 0 || status == 1) {
			on_evt_def(key);
			return;
		}
		else {
			status = 0;
			goto refresh;
		}
	}

	if (status > 1) {
		switch (status) {
		case 2:
			if (key == KEY_UP) {
				if (menu_osm.freq <= 200)
					menu_osm.freq++;
				osm_set_cfg(osm_ch, &menu_osm);
				eeprom_write(EEPROM_OSM_CH1_CFG + 0x10 * osm_ch, &menu_osm, sizeof(menu_osm));
			}
			else if (key == KEY_DOWN) {
				if (menu_osm.freq > 0)
					menu_osm.freq--;
				osm_set_cfg(osm_ch, &menu_osm);
				eeprom_write(EEPROM_OSM_CH1_CFG + 0x10 * osm_ch, &menu_osm, sizeof(menu_osm));
			}
			if (key == KEY_ENTER) {
				osm_cursor_pos = 11;
				status++;
			}
			break;

		case 3:
			if (key == KEY_UP) {
				if (menu_osm.volt_perc < 100)
					menu_osm.volt_perc++;
				osm_set_cfg(osm_ch, &menu_osm);
				eeprom_write(EEPROM_OSM_CH1_CFG + 0x10 * osm_ch, &menu_osm, sizeof(menu_osm));
			}
			else if (key == KEY_DOWN) {
				if (menu_osm.volt_perc > 0)
					menu_osm.volt_perc--;
				osm_set_cfg(osm_ch, &menu_osm);
				eeprom_write(EEPROM_OSM_CH1_CFG + 0x10 * osm_ch, &menu_osm, sizeof(menu_osm));
			}
			if (key == KEY_ENTER) {
				osm_cursor_pos = 17;
				status++;
			}
			break;

		case 4:
			if (key == KEY_UP) {
				if (menu_osm.duty < 100)
					menu_osm.duty++;
				osm_set_cfg(osm_ch, &menu_osm);
				eeprom_write(EEPROM_OSM_CH1_CFG + 0x10 * osm_ch, &menu_osm, sizeof(menu_osm));
			}
			else if (key == KEY_DOWN) {
				if (menu_osm.duty > 0)
					menu_osm.duty--;
				osm_set_cfg(osm_ch, &menu_osm);
				eeprom_write(EEPROM_OSM_CH1_CFG + 0x10 * osm_ch, &menu_osm, sizeof(menu_osm));
			}

			if (key == KEY_ENTER) {
				osm_cursor_pos = 0;
				status = 0;
				break;
			}
		}
	}
	else {
		if (osm_ch == OSM_CH1 && key == KEY_DOWN) {
			osm_ch = OSM_CH2;
			goto refresh;
		}
		else if (osm_ch == OSM_CH2 && key == KEY_UP) {
			osm_ch = OSM_CH1;
			goto refresh;
		}
		else if (key == KEY_ENTER) {
			osm_cursor_pos = 5;
			status = 2;
			keys_clear_evts(1 << key);
		}
		else {
			osm_ch = OSM_CH1;
			osm_cursor_pos = 0;
			on_evt_def(key);
		}

		return;
	}
refresh:
	eeprom_read(EEPROM_OSM_CH1_CFG + 0x10 * osm_ch, &menu_osm, sizeof(menu_osm));
	refresh_osm();
	keys_clear_evts(1 << key);

}

/* PWM OSM Setting End */
static u8 alrm_pol;
static void update_alrm_pol(void)
{
	const char *buf[] = {"Clos", "Open", "Off"};

	if (alrm_pol > 2)
		alrm_pol = 2;
	lcd_write_line(buf[alrm_pol], 1, 0);
	if (status == 2)
		lcd_cursor(1, osm_cursor_pos, 1);
	else
		lcd_cursor(0, 0, 0);
}

static void refresh_alrm_pol(void)
{
	if (status == 0) {
		eeprom_read(EEPROM_ALRM_OUT_POL, &alrm_pol, 1);
		update_alrm_pol();
		status = 1;
	}
}

static void on_evt_alrm_pol(int key)
{
	if (status == 1) {
		if (key == KEY_ENTER) {
			status = 2;
			update_alrm_pol();
			keys_clear_evts(1 << key);
		}
		else
			on_evt_def(key);
		return;
	}
	else if (status == 2) {
		if (key == KEY_ESC) {
			eeprom_read(EEPROM_ALRM_OUT_POL, &alrm_pol, 1);
			status = 1;
		}
		else if (key == KEY_ENTER) {
			eeprom_write(EEPROM_ALRM_OUT_POL, &alrm_pol, 1);
			status = 1;
		}
		else if (key == KEY_UP)
			alrm_pol = (alrm_pol + 1) % 3;
		else if (key == KEY_DOWN) {
			if (alrm_pol == 0)
				alrm_pol = 2;
			else
				alrm_pol--;
		}
		update_alrm_pol();
	}
	keys_clear_evts(1 << key);
}


/* F. Mode Setting Begin */

static int fmode_cursor_pos = 0;
static char fmode_sn[6];
static u8 fmode_en_def_out, fmode_en_osm, fmode_temp_max;

static void update_fmode(void)
{
	char buf[24];

	k_sprintf(buf, "F. Mode %d%c", fmode_en_osm, fmode_en_def_out ? 'D' : ' ');
	lcd_write_line(buf, 0, 0);

	k_sprintf(buf, "S/N %s T:%02d", fmode_sn, fmode_temp_max);
	lcd_write_line(buf, 1, 0);

	if (fmode_cursor_pos > 0)
		lcd_cursor(fmode_cursor_pos / 20, fmode_cursor_pos % 20, 1);
}

static void refresh_fmode(void)
{
	u32 bluetooth_id;

	if (status == 0) {
		eeprom_read(EEPROM_ENABLE_DEF_OUT, &fmode_en_def_out, 1);
		eeprom_read(EEPROM_ENABLE_OSM, &fmode_en_osm, 1);
		eeprom_read(EEPROM_T_MAX, &fmode_temp_max, 1);

		eeprom_read(EEPROM_BLUETOOTH_ID, &bluetooth_id, 4);
		k_sprintf(fmode_sn, "%05d", (uint)bluetooth_id);
		update_fmode();
		status = 1;
		fmode_cursor_pos = 0;
	}
}

static void on_evt_fmode(int key)
{
	u8 digit;
	u32 tmp;
	if (status == 0 || key == KEY_ESC) {
		fmode_cursor_pos = 0;
		on_evt_def(key);
		return;
	}

	switch (status) {
		case 1:
			if (key == KEY_ENTER) {
				fmode_cursor_pos = 8;
				update_fmode();
				status = 2;
			}
			else
				on_evt_def(key);
			break;

		case 2:
			if (key == KEY_ENTER) {
				eeprom_write(EEPROM_ENABLE_OSM, &fmode_en_osm, 1);
				osm_restart();
				fmode_cursor_pos = 9;
				status = 3;
			}
			else if (key == KEY_UP)
				fmode_en_osm = (fmode_en_osm + 1) % 3;
			else if (key == KEY_DOWN) {
				if (fmode_en_osm == 0)
					fmode_en_osm = 2;
				else
					fmode_en_osm--;
			}
			update_fmode();
			break;

		case 3:
			if (key == KEY_ENTER) {
				eeprom_write(EEPROM_ENABLE_DEF_OUT, &fmode_en_def_out, 1);
				def_restart();
				fmode_cursor_pos = 24;
				status = 4;
			}
			else if (key == KEY_UP || key == KEY_DOWN) {
				fmode_en_def_out = !fmode_en_def_out;
			}
			update_fmode();
			break;

		case 4:
			if (key == KEY_ENTER) {
				if (fmode_cursor_pos < 28) {
					fmode_cursor_pos++;
					update_fmode();
				}
				else {
					tmp = atoi(fmode_sn);
					eeprom_write(EEPROM_BLUETOOTH_ID, &tmp, sizeof(tmp));
					status = 5;
					fmode_cursor_pos = 32;
					update_fmode();
				}
			}
			else {
				digit = fmode_sn[fmode_cursor_pos - 24];
				if (key == KEY_UP) {
					if (digit < '9')
						digit++;
					else
						digit = '0';
				}
				else if (key == KEY_DOWN) {
					if (digit > '0')
						digit--;
					else
						digit = '9';
				}
				fmode_sn[fmode_cursor_pos - 24] = digit;
				update_fmode();
			}
			break;

		case 5:
			if (key == KEY_ENTER) {
				eeprom_write(EEPROM_T_MAX, &fmode_temp_max, 1);
				status = 0;
				fmode_cursor_pos = 0;
			}
			else if (key == KEY_UP) {
				if (fmode_temp_max < 70)
					fmode_temp_max++;
			}
			else if (key == KEY_DOWN) {
				if (fmode_temp_max > 30)
					fmode_temp_max--;
			}

			update_fmode();
			break;

		default:
			on_evt_def(key);
	}
	keys_clear_evts(1 << key);
}

/* F. Mode Setting End */

/* EPT Setting Begin */
static int ept_cursor_pos;
static u16 ept_en;
static u16 ept_pause, ept_inv_10xsec;

static void update_ept(void)
{
	char buf[24];

	k_sprintf(buf, "EPT:%c P(sec):%d", ept_en ? 'S' : 'N', ept_pause);
	lcd_write_line(buf, 0, 0);

	k_sprintf(buf, "INV(sec):%d.%d", ept_inv_10xsec / 10, ept_inv_10xsec % 10);
	lcd_write_line(buf, 1, 0);

	if (ept_cursor_pos > 0)
		lcd_cursor(ept_cursor_pos / 20, ept_cursor_pos % 20, 1);
}

static void refresh_ept(void)
{
	if (status == 0) {
		eeprom_read(EEPROM_EPT_EN, &ept_en, 1);
		eeprom_read(EEPROM_EPT_PAUSE, &ept_pause, 2);
		eeprom_read(EEPROM_EPT_INV, &ept_inv_10xsec, 2);

		update_ept();
		status = 1;
		ept_cursor_pos = 0;
	}
}

static void on_evt_ept(int key)
{
	if (status == 0 || key == KEY_ESC) {
		on_evt_def(key);
		ept_cursor_pos = 0;
		return;
	}

	switch (status) {
		case 1:
			if (key == KEY_ENTER) {
				ept_cursor_pos = 4;
				update_ept();
				status = 2;
			}
			else
				on_evt_def(key);
			break;

		case 2:
			if (key == KEY_ENTER) {
				eeprom_write(EEPROM_EPT_EN, &ept_en, 1);
				ept_cursor_pos = 14;
				status = 3;
			}
			else if (key == KEY_UP || key == KEY_DOWN)
				ept_en = !ept_en;
			update_ept();
			break;

		case 3:
			if (key == KEY_ENTER) {
				eeprom_write(EEPROM_EPT_PAUSE, &ept_pause, 2);
				ept_cursor_pos = 29;
				status = 4;
			}
			else if (key == KEY_UP) {
				if (ept_pause < 59)
					ept_pause++;
			}
			else if (key == KEY_DOWN) {
				if (ept_pause > 0)
					ept_pause--;
			}

			update_ept();
			break;

		case 4:
			if (key == KEY_ENTER) {
				eeprom_write(EEPROM_EPT_INV, &ept_inv_10xsec, 2);
				status = 5;
				ept_cursor_pos = 31;
			}

			if (key == KEY_UP) {
				if (ept_inv_10xsec <= 89)
					ept_inv_10xsec += 10;
				else
					ept_inv_10xsec = 99;
			}
			else if (key == KEY_DOWN) {
				if (ept_inv_10xsec >= 10)
					ept_inv_10xsec -= 10;
				else
					ept_inv_10xsec = 0;
			}
			update_ept();
			break;

		case 5:
			if (key == KEY_ENTER) {
				eeprom_write(EEPROM_EPT_INV, &ept_inv_10xsec, 2);
				status = 0;
				ept_cursor_pos = 0;
			}

			if (key == KEY_UP) {
				if (ept_inv_10xsec < 99)
					ept_inv_10xsec++;
			}
			else if (key == KEY_DOWN) {
				if (ept_inv_10xsec > 0)
					ept_inv_10xsec--;
			}
			update_ept();
			break;

		default:
			on_evt_def(key);
	}
	keys_clear_evts(1 << key);
}

/* EPT Setting End */

/* Current check Begin */
static struct osm_cur_check_t menu_check;

static void update_cur_check()
{
	char buf[24];

	k_sprintf(buf, "C%d Contr. Corr.:%c", osm_ch + 1, (uint)menu_check.enable ? 'S': 'N');
	lcd_write_line(buf, 0, 0);

	k_sprintf(buf, "%%:%02d T=%02d", menu_check.max_perc, menu_check.intvl);
	lcd_write_line(buf, 1, 0);

	if (osm_cursor_pos > 0)
		lcd_cursor(osm_cursor_pos / 20, osm_cursor_pos % 20, 1);
	else
		lcd_cursor(0, 0, 0);
}

static void refresh_cur_check(void)
{
	if (status == 0) {
		osm_get_cur_check(osm_ch, &menu_check);
		status = 1;
		update_cur_check();
	}
}

static void on_evt_cur_check(int key)
{
	if (key == KEY_ESC) {
		osm_cursor_pos = 0;

		if (status == 0 || status == 1) {
			osm_ch = OSM_CH1;
			on_evt_def(key);
			return;
		}
		else {
			status = 0;
			goto refresh;
		}
	}

	if (status > 1) {
		switch (status) {
		case 2:
			if (key == KEY_UP || key == KEY_DOWN) {
				menu_check.enable = !menu_check.enable;
				osm_set_cur_check(osm_ch, &menu_check);
			}
			else if (key == KEY_ENTER) {
				osm_cursor_pos = 23;
				status++;
			}
			break;

		case 3:
			if (key == KEY_UP) {
				if (menu_check.max_perc < 50)
					menu_check.max_perc++;
				osm_set_cur_check(osm_ch, &menu_check);
			}
			else if (key == KEY_DOWN) {
				if (menu_check.max_perc > 1)
					menu_check.max_perc--;
				osm_set_cur_check(osm_ch, &menu_check);
			}
			if (key == KEY_ENTER) {
				osm_cursor_pos = 28;
				status++;
			}
			break;

		case 4:
			if (key == KEY_UP) {
				if (menu_check.intvl < 10)
					menu_check.intvl++;
				osm_set_cur_check(osm_ch, &menu_check);
			}
			else if (key == KEY_DOWN) {
				if (menu_check.intvl > 0)
					menu_check.intvl--;
				osm_set_cur_check(osm_ch, &menu_check);
			}

			if (key == KEY_ENTER) {
				osm_cursor_pos = 0;
				status = 0;
			}
			break;
		}
	}
	else {
		if (osm_ch == OSM_CH1 && key == KEY_DOWN) {
			status = 0;
			osm_ch = OSM_CH2;
			goto refresh;
		}
		else if (osm_ch == OSM_CH2 && key == KEY_UP) {
			status = 0;
			osm_ch = OSM_CH1;
			goto refresh;
		}
		else if (key == KEY_ENTER) {
			osm_cursor_pos = 16;
			status = 2;
			update_cur_check();
			keys_clear_evts(1 << key);
		}
		else {
			osm_ch = OSM_CH1;
			on_evt_def(key);
		}

		return;
	}

	update_cur_check();
	keys_clear_evts(1 << key);

	return;

refresh:
	refresh_cur_check();
	keys_clear_evts(1 << key);
}

/* Current check End */

/* Short Circuit Begin */
static u16 max_cur;

static void update_short_circuit()
{
	char buf[24];

	k_sprintf(buf, "C%d Corto Circuito", osm_ch + 1);
	lcd_write_line(buf, 0, 0);

	k_sprintf(buf, "mA: %04d", max_cur);
	lcd_write_line(buf, 1, 0);

	if (osm_cursor_pos > 0)
		lcd_cursor(osm_cursor_pos / 20, osm_cursor_pos % 20, 1);
	else
		lcd_cursor(0, 0, 0);
}

static void refresh_short_circuit(void)
{
	if (status == 0) {
		osm_get_max(osm_ch, &max_cur);
		status = 1;
		update_short_circuit();
	}
}

static void on_evt_short_circuit(int key)
{
	if (key == KEY_ESC) {
		osm_cursor_pos = 0;

		if (status == 0 || status == 1) {
			osm_ch = OSM_CH1;
			on_evt_def(key);
			return;
		}
		else {
			status = 0;
			goto refresh;
		}
	}

	if (status > 1) {
		switch (status) {

		case 2:
			if (key == KEY_UP) {
				if (max_cur < 1990)
					max_cur += 10;
				else
					max_cur = 2000;
				osm_set_max(osm_ch, max_cur);
			}
			else if (key == KEY_DOWN) {
				if (max_cur > 10)
					max_cur -= 10;
				else
					max_cur = 0;
				osm_set_max(osm_ch, max_cur);
			}
			if (key == KEY_ENTER) {
				osm_cursor_pos = 0;
				status = 0;
			}
			break;
		}
	}
	else {
		if (osm_ch == OSM_CH1 && key == KEY_DOWN) {
			status = 0;
			osm_ch = OSM_CH2;
			goto refresh;
		}
		else if (osm_ch == OSM_CH2 && key == KEY_UP) {
			status = 0;
			osm_ch = OSM_CH1;
			goto refresh;
		}
		else if (key == KEY_ENTER) {
			osm_cursor_pos = 27;
			status = 2;
			update_short_circuit();
			keys_clear_evts(1 << key);
		}
		else {
			osm_ch = OSM_CH1;
			on_evt_def(key);
		}

		return;
	}

	update_short_circuit();
	keys_clear_evts(1 << key);

	return;

refresh:
	refresh_short_circuit();
	keys_clear_evts(1 << key);
}

/* Short Circuit End */

/* Active Alarms Begin */

static int active_alarms;
int alrm_showing_type = ALRM_TYPE_INVALID;

static void update_active_alarms(void)
{
	if (active_alarms & ALRM_BITFIELD_ANT) {
		alrm_showing_type = ALRM_TYPE_ANT;
		active_alarms &= ~ALRM_BITFIELD_ANT;
	}
	else if (active_alarms & ALRM_BITFIELD_BATTERY(0) ||
        active_alarms & ALRM_BITFIELD_BATTERY(1) ||
        active_alarms & ALRM_BITFIELD_BATTERY(2) ||
        active_alarms & ALRM_BITFIELD_BATTERY(3) ) {

		/* FIXME: handle batteries separately */
		alrm_showing_type = ALRM_TYPE_BATTERY;
		active_alarms &= ~ALRM_BITFIELD_BATTERY(0);
		active_alarms &= ~ALRM_BITFIELD_BATTERY(1);
		active_alarms &= ~ALRM_BITFIELD_BATTERY(2);
		active_alarms &= ~ALRM_BITFIELD_BATTERY(3);
	}
	else if (active_alarms & ALRM_BITFIELD_OVERTEMP) {
		alrm_showing_type = ALRM_TYPE_OVERTEMP;
		active_alarms &= ~ALRM_BITFIELD_OVERTEMP;
	}
	else if (active_alarms & ALRM_BITFIELD_PEAK(OSM_CH1)) {
		alrm_showing_type = ALRM_TYPE_PEAK(OSM_CH1);
		active_alarms &= ~ALRM_BITFIELD_PEAK(OSM_CH1);
	}
	else if (active_alarms & ALRM_BITFIELD_PEAK(OSM_CH2)) {
		alrm_showing_type = ALRM_TYPE_PEAK(OSM_CH2);
		active_alarms &= ~ALRM_BITFIELD_PEAK(OSM_CH2);
	}
	else if (active_alarms & ALRM_BITFIELD_SHORT(OSM_CH1)) {
		alrm_showing_type = ALRM_TYPE_SHORT(OSM_CH1);
		active_alarms &= ~ALRM_BITFIELD_SHORT(OSM_CH1);
	}
	else if (active_alarms & ALRM_BITFIELD_SHORT(OSM_CH2)) {
		alrm_showing_type = ALRM_TYPE_SHORT(OSM_CH2);
		active_alarms &= ~ALRM_BITFIELD_SHORT(OSM_CH2);
	}
	else
		alrm_showing_type = ALRM_TYPE_INVALID;

	if (alrm_showing_type != ALRM_TYPE_INVALID) {
		lcd_write_line("Allarme", 0, 0);
		lcd_write_line(get_alarm_str_by_type(alrm_showing_type), 1, 0);
	}
	else
		on_evt_def(KEY_ESC);
}

static void refresh_active_alarms(void)
{
	if (status == 0) {
		update_active_alarms();
		status = 1;
	}
}

static void on_evt_active_alarms(int key)
{
	if (key == KEY_ESC) {
		active_alarms = 0;
		on_evt_def(key);
		return;
	}

	if (key == KEY_ENTER) {
		if (!keys_is_long_evt(KEY_ENTER))
			return;

		if (alrm_showing_type == ALRM_TYPE_SHORT(OSM_CH1) ||
		    alrm_showing_type == ALRM_TYPE_SHORT(OSM_CH2)) {
			clr_alarm(ALRM_BITFIELD_SHORT(alrm_showing_type - ALRM_TYPE_SHORT(OSM_CH1)));
			osm_restart();
		}
		else if (alrm_showing_type == ALRM_TYPE_PEAK(OSM_CH1) ||
		    alrm_showing_type == ALRM_TYPE_PEAK(OSM_CH2)) {
			clr_alarm(ALRM_BITFIELD_PEAK(alrm_showing_type - ALRM_TYPE_PEAK(OSM_CH1)));
			osm_restart();
		}

		update_active_alarms();
	}

	keys_clear_evts(1 << key);
}

/* Active Alarms End */

static struct menu_voice_t menu[] = {
	{10, {"VERSIONE", ""}, on_evt_def, NULL, {30, 20, -1, 11}, 1},
	{11, {"", ""}, on_evt_def, refresh_info, {14, 12, 10, -1}, 1},
	{12, {"", ""}, on_evt_def, refresh_measures, {11, 13, 10, -1}, 1},
	{13, {"", ""}, on_evt_def, refresh_def_rolling, {12, 14, 10, -1}, 1},
	{14, {"", ""}, on_evt_dl_code, refresh_dl_code, {13, 11, 10, -1}, 1},
	{20, {"LOG", ""}, on_evt_def, NULL, {10, 30, -1, 21}, 1},
	{21, {"LOG", "ALLARMI"}, on_evt_def, NULL, {23, 22, 20, 210}, 1},
	{210, {"", ""}, on_evt_show, refresh_show_alarms, {-1, -1, 21, -1}, 1},
	{22, {"LOG", "AVVII"}, on_evt_def, NULL, {21, 23, 20, 220}, 1},
	{220, {"", ""}, on_evt_show, refresh_show_avvii, {-1, -1, 22, -1}, 1},
	{23, {"LOG", "LETTURE PWM"}, on_evt_def, NULL, {22, 24, 20, 231}, 1},
	{24, {"LOG", "LETTURE DEF"}, on_evt_def, NULL, {23, 22, 20, 232}, 1},
	{231, {"", ""}, on_evt_show_data_osm, refresh_show_data_osm, {232, 232, 23, -1}, 1},
	{232, {"", ""}, on_evt_show_data_ant, refresh_show_data_ant, {231, 231, 23, -1}, 1},
	{30, {"IMPOSTAZIONI", ""}, on_evt_def, NULL, {20, 10, -1, 31}, 1},
	{31, {"", ""}, on_evt_fmode, refresh_fmode, {38, 32, 30, -1}, 1},
	{32, {"PARAMETRI PWM", ""}, on_evt_def, NULL, {31, 33, 30, 321}, 1},
	{321, {"", ""}, on_evt_osm, refresh_osm, {325, 322, 32, -1}, 1, enter_osm_double_ch_page},
	{322, {"", ""}, on_evt_ept, refresh_ept, {321, 323, 32, -1}, 1},
	{323, {"START RIT:A", "IL:GG/MM/AA h:OO"}, on_evt_def, NULL, {322, 324, 32, -1}, 1},
	{324, {"", ""}, on_evt_cur_check, refresh_cur_check, {323, 325, 32, -1}, 1, enter_osm_double_ch_page},
	{325, {"", ""}, on_evt_short_circuit, refresh_short_circuit, {324, 321, 32, -1}, 1, enter_osm_double_ch_page},
	{33, {"PARAMETRI DEF", ""}, on_evt_def, NULL, {32, 34, 30, 331}, 1},
	{331, {"PARAMETRI DEF", "PARAMETRI F."}, on_evt_def, NULL, {334, 332, 33, 3310}, 1},
	{3310, {"", ""}, on_evt_ant, refresh_ant, {-1, -1, 331, 331}, 1},
	{332, {"PARAMETRI DEF", "MODALITA'"}, on_evt_def, NULL, {331, 333, 33, 3320}, 1},
	{3320, {"", ""}, on_evt_mode, refresh_mode, {-1, -1, 332, 332}, 1},
	{333, {"PARAMETRI DEF", "ABIL. MEDIA GIORN."}, on_evt_def, NULL, {332, 334, 0, 3330}, 1},
	{3330, {"", ""}, on_evt_avg_en, refresh_avg_en, {-1, -1, 333, 333}, 1},
	{334, {"VISUALIZZA", "REALTIME SENSORI"}, on_evt_def, NULL, {333, 331, -1, 3340}, 1},
	{3340, {"Attendere...", "Comunicazione"}, on_evt_def, refresh_realtimesens, {-1, -1, 334, 334}, 1},
	{34, {"", ""}, on_evt_datetime, refresh_datetime, {33, 35, 30, -1}, 1},
	{35, {"Funz. a tempo", ""}, on_evt_def, NULL, {34, 36, 30, 350}, 1},
	{350, {"", ""}, on_evt_deadline, refresh_deadline, {-1, -1, 35, 35}, 1},
	{36, {"Comunicazioni", ""}, on_evt_def, NULL, {35, 37, 30, -1}, 0},
	{37, {"Rele errore", ""}, on_evt_alrm_pol, refresh_alrm_pol, {36, 38, 30, -1}, 1},
	{38, {"Backup/Restore", ""}, on_evt_def, NULL, {37, 31, 30, -1}, 0},
	{39, {"Active Alarms", ""}, on_evt_active_alarms, refresh_active_alarms, {-1, -1, MENU_VOICE_DEFAULT, -1}, 0},
	{-1}
};

static int menu_refresh_cnt = 0;
void menu_start(struct task_t *t)
{
}

void menu_step(struct task_t *t)
{
	u32 k;
	int i;
	struct menu_voice_t *pwd;

	if (get_standby()) {
		cur_menu = NULL;
		return;
	}

	k = keys_get_evts();

	if (k && !cur_menu) {
		if (k & (1 << KEY_ESC))
			set_standby(1);
		else {
			pwd = get_menu_voice(MENU_VOICE_PWD);
			if (pwd) {
				dl_to_unlock = get_deadline_idx();
				if (dl_to_unlock >= 0)
					pwd->enabled = 1;
				else
					pwd->enabled = 0;
			}

			cur_menu = get_menu_voice(MENU_VOICE_DEFAULT);
			if (!cur_menu)
				return;

			lcd_write_line(cur_menu->text[0], 0, 0);
			lcd_write_line(cur_menu->text[1], 1, 0);
		}
		keys_clear_evts(k);
		return;
	}

	menu_refresh_cnt++;

	if (!cur_menu) {
		if (menu_refresh_cnt % 200 == 0)
			show_home();
		return;
	}

	if (k && cur_menu->on_evt) {
		for (i = KEY_UP; i <= KEY_ENTER; i++) {
			if (k & (1 << i))
				cur_menu->on_evt(i);
		}
		if (cur_menu->refresh)
			cur_menu->refresh();
	}
	else
		if (cur_menu->refresh && menu_refresh_cnt % 200 == 0)
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
	return DEF_TIMEOUT_MS; /* default timeout 1 min */
}
