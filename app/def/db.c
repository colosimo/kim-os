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
#include "db.h"
#include "eeprom.h"
#include "rtc.h"
#include "lcd.h"

void db_init(void)
{
}

static void fill_alarm(struct alarm_t *a, int type)
{
	struct rtc_t r;
	rtc_get(&r);
	a->type = type;
	a->year = r.year;
	a->month = r.month;
	a->day = r.day;
	a->hour = r.hour;
	a->min = r.min;
}

void db_alarm_add(int type, int sens)
{
	u32 pos;
	u32 addr;
	struct alarm_t a;

	fill_alarm(&a, type);
	a.sens = sens;

	eeprom_read(EEPROM_ALARMS_CUR_POS, &pos, sizeof(pos));

	addr = EEPROM_ALARMS_START_ADDR + pos * sizeof(struct alarm_t);
	eeprom_write(addr, &a, sizeof(a));

	pos = (pos + 1) % ALRM_MAX_NUM;
	eeprom_write(EEPROM_ALARMS_CUR_POS, &pos, sizeof(pos));
}

void db_start_add(void)
{
	u32 pos;
	u32 addr;
	struct alarm_t a;

	fill_alarm(&a, ALRM_TYPE_START);

	eeprom_read(EEPROM_AVVII_CUR_POS, &pos, sizeof(pos));

	addr = EEPROM_AVVII_START_ADDR + pos * sizeof(struct alarm_t);
	eeprom_write(addr, &a, sizeof(a));

	pos = (pos + 1) % AVVII_MAX_NUM;
	eeprom_write(EEPROM_AVVII_CUR_POS, &pos, sizeof(pos));
}

int db_alarm_get(struct alarm_t *a, int pos)
{
	u32 addr;
	u32 p;

	if (pos != DB_POS_INVALID && pos >= ALRM_MAX_NUM) {
		a->type = ALRM_TYPE_INVALID;
		return DB_POS_INVALID;
	}

	p = pos;
	if (p == DB_POS_INVALID) {
		eeprom_read(EEPROM_ALARMS_CUR_POS, &p, sizeof(p));
		if (p == 0)
			p = ALRM_MAX_NUM - 1;
		else
			p--;
	}

	addr = EEPROM_ALARMS_START_ADDR + p * sizeof(struct alarm_t);
	eeprom_read(addr, a, sizeof(*a));

	if (a->type != ALRM_TYPE_INVALID)
		return p;

	return DB_POS_INVALID;
}

static const char *alarm_str[ALRM_TYPE_STOP + 1] =
    {"Err. Antenna", "Batteria Bassa", "Start", "Stop"};

void db_alarm_dump(struct alarm_t *a)
{
	char buf[24];
	if (a->type > ALRM_TYPE_STOP)
		return;
	k_sprintf(buf, "%s\n", alarm_str[a->type]);
	log(buf);
	k_sprintf(buf, "SEN:%d %02d/%02d/%02d %02d:%02d\n",
	    a->sens, a->day, a->month, a->year, a->hour, a->min);
	log(buf);
}

void db_alarm_display(struct alarm_t *a)
{
	char buf[24];
	if (a->type > ALRM_TYPE_STOP)
		return;

	if (a->type == ALRM_TYPE_BATTERY)
		k_sprintf(buf, "%s S:%d", alarm_str[a->type], a->sens + 1);
	else
		k_sprintf(buf, "%s", alarm_str[a->type]);

	lcd_write_line(buf, 0, 0);

	k_sprintf(buf, "%02d/%02d/%02d %02d:%02d",
	    a->day, a->month, a->year, a->hour, a->min);

	lcd_write_line(buf, 1, 0);
}

void db_alarm_reset(void)
{
	u32 tmp;
	u8 page[64];

	tmp = 0;
	eeprom_write(EEPROM_ALARMS_CUR_POS, &tmp, sizeof(tmp));

	memset(page, 0xff, sizeof(page));
	for (tmp = EEPROM_ALARMS_START_ADDR; tmp < EEPROM_ALARMS_END_ADDR; tmp += 64)
		eeprom_write(tmp, page, sizeof(page));
}

void db_avvii_reset(void)
{
	u32 tmp;
	u8 page[64];

	tmp = 0;
	eeprom_write(EEPROM_AVVII_CUR_POS, &tmp, sizeof(tmp));

	memset(page, 0xff, sizeof(page));
	for (tmp = EEPROM_AVVII_START_ADDR; tmp < EEPROM_AVVII_END_ADDR; tmp += 64)
		eeprom_write(tmp, page, sizeof(page));
}

int db_avvii_get(struct alarm_t *a, int pos)
{
	u32 addr;
	u32 p;

	if (pos != DB_POS_INVALID && pos >= AVVII_MAX_NUM) {
		a->type = ALRM_TYPE_INVALID;
		return DB_POS_INVALID;
	}

	p = pos;
	if (p == DB_POS_INVALID) {
		eeprom_read(EEPROM_AVVII_CUR_POS, &p, sizeof(p));
		if (p == 0)
			p = AVVII_MAX_NUM - 1;
		else
			p--;
	}

	addr = EEPROM_AVVII_START_ADDR + p * sizeof(struct alarm_t);
	eeprom_read(addr, a, sizeof(*a));

	if (a->type != ALRM_TYPE_INVALID)
		return p;

	return DB_POS_INVALID;
}


struct data_t data_status[4][24];
int data_cnt[4] = {0, 0, 0, 0};

void db_data_add(struct data_t *d)
{
	int s;
	int cnt;

	s = d->sens;

	if (s > 3) {
		err("Invalid data sens %d\n", d->sens);
		return;
	}

	cnt = data_cnt[s];
	memcpy(&data_status[s][cnt], d, sizeof(*d));

	data_cnt[s] = (data_cnt[s] + 1) % 24;
}

void db_data_save_to_eeprom(void)
{
	int s, j;
	struct data_t d;
	int vread, temp, hum, vbat;
	u32 pos;
	u32 addr;
	int cnt;
	struct rtc_t r;

	for (s = 0; s < 4; s++) {

		cnt = data_cnt[s];
		if (!cnt)
			continue;

		vread = temp = hum = vbat = 0;

		for (j = 0; j < cnt; j++) {
			vread += data_status[s][j].vread;
			temp += data_status[s][j].temp;
			hum += data_status[s][j].hum;
			vbat += data_status[s][j].vbat;
		}

		d.sens = s;
		d.temp = temp / cnt;
		d.vread = vread / cnt;
		d.hum = hum / cnt;
		d.vbat = vbat / cnt;

		rtc_get(&r);
		d.day = r.day;
		d.month = r.month;
		d.year = r.year;

		eeprom_read(EEPROM_DATA_CUR_POS, &pos, sizeof(pos));

		addr = EEPROM_DATA_START_ADDR + pos * sizeof(struct data_t);
		eeprom_write(addr, &d, sizeof(d));

		pos = (pos + 1) % DATA_MAX_NUM;
		eeprom_write(EEPROM_DATA_CUR_POS, &pos, sizeof(pos));

		/* Invalidate current record */
		memset(&d, 0xff, sizeof(d));
		addr = EEPROM_DATA_START_ADDR + pos * sizeof(struct data_t);
		eeprom_write(addr, &d, sizeof(d));

		data_cnt[s] = 0;
	}
}

int db_data_get(struct data_t *d, int pos)
{
	u32 addr;
	u32 p;

	if (pos != DB_POS_INVALID && pos >= DATA_MAX_NUM)
		return DB_POS_INVALID;

	p = pos;
	if (p == DB_POS_INVALID) {
		eeprom_read(EEPROM_DATA_CUR_POS, &p, sizeof(p));
		if (p == 0)
			p = DATA_MAX_NUM - 1;
		else
			p--;
	}

	addr = EEPROM_DATA_START_ADDR + p * sizeof(struct data_t);
	eeprom_read(addr, d, sizeof(*d));

	if (d->sens > 3 || d->month > 12 || d->day > 31)
		return DB_POS_INVALID;

	return p;
}

void db_data_display(struct data_t *d)
{
	char buf[24];

	k_sprintf(buf, "S:%d %02d/%02d/%02d mV:%c%d", d->sens + 1, d->day, d->month, d->year,
	    (d->vread < 0) ? '-' : ' ',
	    (uint)abs(d->vread));
	lcd_write_line(buf, 0, 0);

	k_sprintf(buf, "H:%s%d T:%c%d.%d B:%s%d.%d",
	    d->hum < 100 ? " " : "", d->hum,
	    d->temp < 0 ? '-': ' ', (uint)abs(d->temp / 10), (uint)abs(d->temp % 10),
	    d->vbat >= 100 ? "" : " ", (uint)d->vbat / 10, d->vbat % 10);
	lcd_write_line(buf, 1, 0);

}

void db_data_reset(int erase_all)
{
	u32 tmp;
	u8 page[64];

	tmp = 0;
	eeprom_write(EEPROM_DATA_CUR_POS, &tmp, sizeof(tmp));

	if (erase_all) {
		memset(page, 0xff, sizeof(page));
		for (tmp = EEPROM_DATA_START_ADDR; tmp < EEPROM_DATA_END_ADDR; tmp += 64)
			eeprom_write(tmp, page, sizeof(page));
	}
}
