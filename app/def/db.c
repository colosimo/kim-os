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
#include "pwm.h"

static void fill_alarm(struct alarm_t *a, int type)
{
	struct rtc_t r;
	if (type != ALRM_TYPE_STOP)
		rtc_get(&r);
	else {
		eeprom_read(EEPROM_LAST_SEEN_ON_RTC, &r, sizeof(r));
		if (!rtc_valid(&r))
			return;
	}

	a->type = type;
	a->year = r.year;
	a->month = r.month;
	a->day = r.day;
	a->hour = r.hour;
	a->min = r.min;
	if (type == ALRM_TYPE_STOP) {
		r.month = 0xff; /* Invalidate last seen on */
		eeprom_write(EEPROM_LAST_SEEN_ON_RTC, &r, sizeof(r));
	}
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

void db_start_stop_add(u8 type)
{
	u32 pos;
	u32 addr;
	struct alarm_t a;

	if (type != ALRM_TYPE_START && type != ALRM_TYPE_STOP) {
		err("%s invalid type %d\n", __func__, type);
		return;
	}

	fill_alarm(&a, type);

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

static const char *alarm_str[ALRM_TYPE_LAST] =
    {"Err. Antenna", "", "Start", "Stop", "Batteria Bassa", "Err. Tempo On", "Err. Tempo Off"};

void db_alarm_dump(struct alarm_t *a)
{
	char buf[24];
	if (a->type > ALRM_TYPE_BATTERY)
		return;
	k_sprintf(buf, "%s\n", alarm_str[a->type]);
	log(buf);
	k_sprintf(buf, "SEN:%d %02d/%02d/%02d %02d:%02d\n",
	    a->sens, a->day, a->month, a->year, a->hour, a->min);
	log(buf);
}

void db_alarm_dump_all()
{
	int p, p_init;
	struct alarm_t a;
	u8 s;

	eeprom_read(EEPROM_ALARMS_CUR_POS, &p_init, sizeof(p_init));
	p = p_init;

	kprint("\r\n\r\nALRM_ID,ALRM_STR,SENS,DATE,TIME\r\n");
	while (p != DB_POS_INVALID) {
		db_alarm_get(&a, p);
		if (a.type != ALRM_TYPE_INVALID) {
			if (a.type == ALRM_TYPE_ANT)
				s = 0;
			else
				s = a.sens + 1;
			kprint("%d,%s,%d,%02d/%02d/%02d,%02d:%02d\r\n",
		        a.type, alarm_str[a.type], s, a.day, a.month, a.year, a.hour, a.min);
		}
		p = (p + 1) % ALRM_MAX_NUM;
		if (p == p_init)
			break;
	}
}

void db_avvii_dump_all(void)
{
	int p, p_init;
	struct alarm_t a;

	eeprom_read(EEPROM_AVVII_CUR_POS, &p_init, sizeof(p_init));
	p = p_init;

	kprint("\r\n\r\nEVT_ID,EVT_STR,DATE,TIME\r\n");
	while (p != DB_POS_INVALID) {
		p = db_avvii_get(&a, p);
		if (a.type != ALRM_TYPE_INVALID) {
			kprint("%d,%s,%02d/%02d/%02d,%02d:%02d\r\n",
		        a.type, alarm_str[a.type], a.day, a.month, a.year, a.hour, a.min);
		}
		p = (p + 1) % AVVII_MAX_NUM;
		if (p == p_init)
			break;
	}
}

void db_alarm_display(struct alarm_t *a)
{
	char buf[24];
	if (a->type >= ALRM_TYPE_LAST)
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

static struct data_t data_status[4][24];
static int data_pos[4];

void db_data_init(void)
{
	memset(data_status, 0, sizeof(data_status));
	memset(data_pos, 0, sizeof(data_pos));
}

static void db_data_fill_all(struct data_t *d)
{
	struct rtc_t r;
	u32 freq, duty;

	rtc_get(&r);
	d->day = r.day;
	d->month = r.month;
	d->year = r.year;
	d->hour = r.hour;
	d->min = r.min;

	pwm_get(&freq, &duty);
	d->freq = (u16)(freq & 0x3ff);
	d->duty = (u16)(duty & 0x3f);
}

static void db_data_save_record(struct data_t *d)
{
	u32 pos;
	u32 addr;

	eeprom_read(EEPROM_DATA_CUR_POS, &pos, sizeof(pos));

	addr = EEPROM_DATA_START_ADDR + pos * sizeof(struct data_t);
	eeprom_write(addr, d, sizeof(*d));

	pos = (pos + 1) % DATA_MAX_NUM;
	eeprom_write(EEPROM_DATA_CUR_POS, &pos, sizeof(pos));

	/* Invalidate current record */
	memset(d, 0xff, sizeof(*d));
	addr = EEPROM_DATA_START_ADDR + pos * sizeof(struct data_t);
	eeprom_write(addr, d, sizeof(*d));
}


void db_data_add(struct data_t *d)
{
	int s;
	int cnt;
	u8 en_daily_avg;

	s = d->sens;

	if (s > 3) {
		err("Invalid data sens %d\n", d->sens);
		return;
	}

	cnt = data_pos[s];
	memcpy(&data_status[s][cnt], d, sizeof(*d));

	data_pos[s] = (data_pos[s] + 1) % 24;

	eeprom_read(EEPROM_ENABLE_DAILY_AVG, &en_daily_avg, 1);
	en_daily_avg &= BIT0;
	if (!en_daily_avg) {
		db_data_fill_all(d);
		db_data_save_record(d);
	}
}

void db_data_save_to_eeprom(void)
{
	int s, j;
	struct data_t d;
	int vread, temp, hum, vbat;
	int cnt;
	u8 en_daily_avg;

	eeprom_read(EEPROM_ENABLE_DAILY_AVG, &en_daily_avg, 1);
	en_daily_avg &= BIT0;

	for (s = 0; s < 4; s++) {

		cnt = 0;
		vread = temp = hum = 0;
		vbat = 255;

		for (j = 0; j < 24; j++) {
			if (!data_status[s][j].vbat && !data_status[s][j].vread)
				continue;
			cnt++;
			vread += data_status[s][j].vread;
			temp += data_status[s][j].temp;
			hum += data_status[s][j].hum;
			vbat = min(vbat, data_status[s][j].vbat);
		}

		if (!cnt)
			continue;

		d.sens = s;
		d.temp = temp / cnt;
		d.vread = vread / cnt;
		d.hum = hum / cnt;
		d.vbat = vbat;

		if (en_daily_avg) {
			db_data_fill_all(&d);
			db_data_save_record(&d);
		}

		if (vbat < BATTERY_THRES)
			db_alarm_add(ALRM_TYPE_BATTERY, s);

	}
	db_data_init();

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

void db_data_display(struct data_t *d, int npage)
{
	char buf[24];

	if (npage == 1) {

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
	else {
		k_sprintf(buf, "Freq: %03d  D.C.: %02d", d->freq, d->duty);
		lcd_write_line(buf, 0, 0);

		k_sprintf(buf, "Time: %02d:%02d", d->hour, d->min);
		lcd_write_line(buf, 1, 0);
	}
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

void db_data_dump_all()
{
	int p, p_init;
	struct data_t d;
	eeprom_read(EEPROM_DATA_CUR_POS, &p_init, sizeof(p_init));
	p = p_init;

	kprint("\r\n\r\nSENS,DATE,TIME,TEMP,VOLT,HUM,VBAT,FREQ,DUTY\r\n");
	while (p != DB_POS_INVALID) {
		db_data_get(&d, p);
		if (d.sens != 0xff) {
			kprint("%d,%02d/%02d/%02d,%02d:%02d,%d,%d,%d,%d,%d,%d\r\n",
		        d.sens + 1, d.day, d.month, d.year, d.hour, d.min,
				d.temp,d.vread, d.hum, d.vbat, d.freq, d.duty);
		}
		p = (p + 1) % DATA_MAX_NUM;
		if (p == p_init)
			break;
	}
}
