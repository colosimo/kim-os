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
		cur_menu = new_menu;
		lcd_write_string(cur_menu->text[0], 0, 1);
		lcd_write_string(cur_menu->text[1], 1, 1);
	}

done:
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
	{7, {"IMPOSTAZIONI", "DATA E ORA"}, on_evt_def, NULL, {6, 8, 0, -1}},
	{8, {"IMPOSTAZIONI", "RESET CONTATORE"}, on_evt_def, NULL, {7, 9, 0, -1}},
	{9, {"IMPOSTAZIONI", "RESET STORICI"}, on_evt_def, NULL, {8, 10, 0, -1}},
	{10, {"IMPOSTAZIONI", "BLUETOOTH"}, on_evt_def, NULL, {9, 11, 0, -1}},
	{11, {"IMPOSTAZIONI", "COMUNICAZIONI RF"}, on_evt_def, NULL, {10, 12, 0, -1}},
	{12, {"IMPOSTAZIONI", "VERSIONE FW"}, on_evt_def, NULL, {11, 13, 0, -1}},
	{13, {"IMPOSTAZIONI", "TEST PWM"}, on_evt_def, NULL, {12, 5, 0, -1}},
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
			lcd_write_string(cur_menu->text[0], 0, 1);
			lcd_write_string(cur_menu->text[1], 1, 1);
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
