/*
 * Author: Aurelio Colosimo, 2022
 * Copyright: Elo System srl
 */

#include <reg.h>
#include <log.h>
#include "deadline.h"
#include "eeprom.h"

static struct deadline_t deadlines[3];

int dl_valid(struct deadline_t *dl)
{
	int i;
	if (!rtc_valid(&dl->rtc)) {
		return 0;
	}
	if (dl->enable != 0 && dl->enable != 1)
		return 0;

	for (i = 0; i < 5; i++) {
		if (!isdigit(dl->code[i])) {
			return 0;
		}
	}
	return 1;
}

static u32 def_code[3] = {17931, 79595, 86248};

void dl_reset_all(void)
{
	int i;
	for (i = 0; i < 3; i++)
		dl_reset(&deadlines[i], i);
}

void dl_reset(struct deadline_t *dl, int idx)
{
	int i;
	u32 _code = def_code[idx];

	rtc_get(&dl->rtc);
	dl->rtc.sec = 0;
	dl->enable = 0;
	for (i = 0; i < 5; i++) {
		dl->code[4 - i] = '0' + _code % 10;
		_code /= 10;
	}
	dl->code[5] = '\0';
	eeprom_write(EEPROM_DEADLINE1_CFG + 0x10 * idx, &deadlines[idx], sizeof(struct deadline_t));
}

void dl_load_all(void)
{
	int i;
	for (i = 0; i < 3; i++) {
		eeprom_read(EEPROM_DEADLINE1_CFG + 0x10 * i, &deadlines[i], sizeof(struct deadline_t));
		if (!dl_valid(&deadlines[i]))
			dl_reset(&deadlines[i], i);
	}
}

void dl_get(int idx, struct deadline_t *dl)
{
	memcpy(dl, &deadlines[idx], sizeof(*dl));
	if (!dl->enable) {
		rtc_get(&dl->rtc);
		dl->rtc.month += 2 * (idx + 1);
		if (dl->rtc.month > 12) {
			dl->rtc.month = dl->rtc.month % 12;
			dl->rtc.year++;
		}
	}
}

void dl_set(int idx, struct deadline_t *dl)
{
	memcpy(&deadlines[idx], dl, sizeof(*dl));
	eeprom_write(EEPROM_DEADLINE1_CFG + 0x10 * idx, dl, sizeof(struct deadline_t));
}

void dl_dump(struct deadline_t *dl)
{
	log("enable %d, code: %s\n", dl->enable, dl->code);
	rtc_dump(&dl->rtc);
}

int dl_isactive(void)
{
	int i;
	for (i = 0; i < 3; i++) {
		if (deadlines[i].enable)
			return 1;
	}
	return 0;
}

int dl_iselapsed(void)
{
	struct rtc_t now;
	int i;

	rtc_get(&now);
	for (i = 0; i < 3; i++) {
		if (!deadlines[i].enable)
			continue;
		if (rtc_compare(&deadlines[i].rtc, &now) == 1)
			return i;
	}
	return -1;
}

int dl_firstelapsing(void)
{
	struct rtc_t *first;
	int first_found = 0;
	int first_id = -1;
	int i;

	for (i = 0; i < 3; i++) {
		if (!deadlines[i].enable)
			continue;
		if (!first_found) {
			first = &deadlines[i].rtc;
			first_id = i;
			first_found = 1;
			continue;
		}
		if (rtc_compare(&deadlines[i].rtc, first) == 1) {
			first = &deadlines[i].rtc;
			first_id = i;
			continue;
		}
	}
	return first_id;
}

int dl_unlock(int idx, u8 *code)
{
	if (idx < 0 || idx >= 3)
		return 0;
	if (!strcmp((char*)deadlines[idx].code, (char*)code)) {
		deadlines[idx].enable = 0;
		eeprom_write(EEPROM_DEADLINE1_CFG + 0x10 * idx, &deadlines[idx],
		    sizeof(struct deadline_t));
		return 1;
	}
	return 0;
}

void dl_disable_all(void)
{
	int i;
	for (i = 0; i < 3; i++) {
		deadlines[i].enable = 0;
		eeprom_write(EEPROM_DEADLINE1_CFG + 0x10 * i, &deadlines[i],
		    sizeof(struct deadline_t));
	}
}
