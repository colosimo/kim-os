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

#include "def.h"
#include "keys.h"
#include "lcd.h"
#include "rtc.h"

static int status = 0; /* Generic state machine variable */

#define MENU_VOICE_DEFAULT 0

struct menu_voice_t {
	int id;
	const char text[2][20];
	void (*on_evt)(int evt);
	void (*refresh)(void);
	int id_next[4]; /* Next menu voice to be called on: UP, DOWN, ESC, ENTER */
};

static struct menu_voice_t menu[];

static struct menu_voice_t *cur_menu = NULL;

struct menu_voice_t *get_menu_voice(int id)
{
	int i;
	for (i = 0; menu[i].id >= 0; i++) {
		if (menu[i].id == id)
			return &menu[i];
	}
	return NULL;
}

/* Default browsing into menus */
void on_evt_def(int key)
{
	struct menu_voice_t *new_menu;

	if (key == KEY_ESC && cur_menu->id_next[key] < 0) {
		show_home();
		cur_menu = NULL;
		goto done;
	}
	new_menu = get_menu_voice(cur_menu->id_next[key]);
	if (new_menu) {
		status = 0;
		cur_menu = new_menu;
		lcd_write_line(cur_menu->text[0], 0, 1);
		lcd_write_line(cur_menu->text[1], 1, 1);
	}

done:
	keys_clear_evts(1 << key);
}

/* Date / Time setting */

static struct rtc_t r;
static u32 ticks_cancel;

void update_screen()
{
	char buf[24];
	int cur_line, cur_pos, cur_show;

	if (status == 100) {
		lcd_write_line("ANNULLATO", 0, 0);
		lcd_write_line("", 1, 0);
		return;
	} else if (status == 102) {
		lcd_write_line("DATA NON VALIDA", 0, 0);
		lcd_write_line("", 1, 0);
		return;
	}

	k_sprintf(buf, "DATA: %02d/%02d/%02d", r.day, r.month, r.year);
	lcd_write_line(buf, 0, 0);
	k_sprintf(buf, "ORE:  %02d:%02d", r.hour, r.min);
	lcd_write_line(buf, 1, 0);

	switch(status) {
		case 0:
		case 1: cur_line = 0; cur_pos = 6; cur_show = 1; break;
		case 2: cur_line = 0; cur_pos = 9; cur_show = 1; break;
		case 3: cur_line = 0; cur_pos = 12; cur_show = 1; break;
		case 4: cur_line = 1; cur_pos = 6; cur_show = 1; break;
		case 5: cur_line = 1; cur_pos = 9; cur_show = 1; break;
		case 6: cur_line = 1; cur_pos = 17; cur_show = 0; break;
		case 101: cur_show = 0;
		default: cur_line = cur_pos = cur_show = 0; break; /* Never happens */
	}
	lcd_cursor(cur_line, cur_pos, cur_show);

	if (status == 6)
		lcd_write_string("OK?");
}

void refresh_datetime()
{
	if (status == 0) {
		rtc_get(&r);
		update_screen();
		status = 1;
	}
	else if (status >= 100 && k_elapsed(ticks_cancel) > MS_TO_TICKS(1000))
			on_evt_def(KEY_ESC);
}

void on_evt_datetime(int key)
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
	update_screen();
	keys_clear_evts(1 << key);

}

static struct menu_voice_t menu[] = {
	{0, {"MENU", "IMPOSTAZIONI"}, on_evt_def, NULL, {4, 1, -1, 5}},
	{1, {"VISUALIZZA", "STORICO AVVII"}, on_evt_def, NULL, {0, 2, -1, -1}},
	{2, {"VISUALIZZA", "SEGNALAZIONI"}, on_evt_def, NULL, {1, 3, -1, -1}},
	{3, {"VISUALIZZA", "STORICO LETTURE"}, on_evt_def, NULL, {2, 4, -1, -1}},
	{4, {"VISUALIZZA", "REALTIME SENSORI"}, on_evt_def, NULL, {3, 0, -1, -1}},
	{5, {"IMPOSTAZIONI", "PARAMETRI F."}, on_evt_def, NULL, {13, 6, 0, -1}},
	{6, {"IMPOSTAZIONI", "MODALITA'"}, on_evt_def, NULL, {5, 7, 0, -1}},
	{7, {"IMPOSTAZIONI", "DATA E ORA"}, on_evt_def, NULL, {6, 8, 0, 14}},
	{8, {"IMPOSTAZIONI", "RESET CONTATORE"}, on_evt_def, NULL, {7, 9, 0, -1}},
	{9, {"IMPOSTAZIONI", "RESET STORICI"}, on_evt_def, NULL, {8, 10, 0, -1}},
	{10, {"IMPOSTAZIONI", "BLUETOOTH"}, on_evt_def, NULL, {9, 11, 0, -1}},
	{11, {"IMPOSTAZIONI", "COMUNICAZIONI RF"}, on_evt_def, NULL, {10, 12, 0, -1}},
	{12, {"IMPOSTAZIONI", "VERSIONE FW"}, on_evt_def, NULL, {11, 13, 0, -1}},
	{13, {"IMPOSTAZIONI", "TEST PWM"}, on_evt_def, NULL, {12, 5, 0, -1}},
	{14, {"", ""}, on_evt_datetime, refresh_datetime, {-1, -1, 7, 7}},
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
