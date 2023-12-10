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
#include "ant.h"
#include "osm.h"

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

void db_avvii_add_start()
{
	u32 pos;
	u32 addr;
	struct avvii_t a;
	struct rtc_t r;

	/* Increase pos, so to "close" previous record, belonging to
	 * previous boot */
	eeprom_read(EEPROM_AVVII_CUR_POS, &pos, sizeof(pos));
	pos = (pos + 1) % AVVII_MAX_NUM;
	eeprom_write(EEPROM_AVVII_CUR_POS, &pos, sizeof(pos));

	/* Create a new record for the current boot */
	rtc_get(&r);
	a.start_year = a.stop_year = r.year;
	a.start_month = a.stop_month = r.month;
	a.start_day = a.stop_day = r.day;
	a.days = 0;

	addr = EEPROM_AVVII_START_ADDR + pos * sizeof(struct avvii_t);
	eeprom_write(addr, &a, sizeof(a));
}

void db_avvii_refresh_stop(u16 days)
{
	u32 pos;
	u32 addr;
	struct avvii_t a;
	struct rtc_t r;

	/* To be called periodically, update the stop date and the days count */
	rtc_get(&r);

	eeprom_read(EEPROM_AVVII_CUR_POS, &pos, sizeof(pos));
	addr = EEPROM_AVVII_START_ADDR + pos * sizeof(struct avvii_t);
	eeprom_read(addr, &a, sizeof(a));

	a.stop_year = r.year;
	a.stop_month = r.month;
	a.stop_day = r.day;
	a.days = days;
	eeprom_write(addr, &a, sizeof(a));
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

void db_alarm_dump(struct alarm_t *a)
{
	char buf[24];
	if (a->type > ALRM_TYPE_BATTERY)
		return;
	k_sprintf(buf, "%s\n", get_alarm_str_by_type(a->type));
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
		        a.type, get_alarm_str_by_type(a.type), s, a.day, a.month, a.year, a.hour, a.min);
		}
		p = (p + 1) % ALRM_MAX_NUM;
		if (p == p_init)
			break;
	}
}

void db_avvii_dump_all(void)
{
	int p, p_init;
	struct avvii_t a;

	eeprom_read(EEPROM_AVVII_CUR_POS, &p_init, sizeof(p_init));
	p = p_init;

	kprint("\r\n\rSTART,STOP,DAYS\r\n");
	while (p != DB_POS_INVALID) {
		p = db_avvii_get(&a, p);
		if (a.start_year != ALRM_TYPE_INVALID) {
			kprint("%02d/%02d/%02d,%02d/%02d/%02d,%d\r\n",
		        a.start_day, a.start_month, a.start_year,
			    a.stop_day, a.stop_month, a.stop_year, a.days);
		}
		p = (p + 1) % AVVII_MAX_NUM;
		if (p == p_init)
			break;
	}
}

void db_avvii_display(struct avvii_t *a, u16 pos)
{
	char buf[24];

	k_sprintf(buf, "Avvii %03d GG:%03d", pos, a->days);

	lcd_write_line(buf, 0, 0);

	k_sprintf(buf, "A:%02d%02d%02d S:%02d%02d%02d",
		a->start_day, a->start_month, a->start_year,
		a->stop_day, a->stop_month, a->stop_year);

	lcd_write_line(buf, 1, 0);
}

void db_alarm_display(struct alarm_t *a, u16 pos)
{
	char buf[24];
	if (a->type >= ALRM_TYPE_LAST)
		return;

	if (a->type == ALRM_TYPE_BATTERY)
		k_sprintf(buf, "%03d %s S:%d", pos, get_alarm_str_by_type(a->type), a->sens + 1);
	else
		k_sprintf(buf, "%03d %s", pos, get_alarm_str_by_type(a->type));

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

int db_avvii_get(struct avvii_t *a, int pos)
{
	u32 addr;
	u32 p;

	if (pos != DB_POS_INVALID && pos >= AVVII_MAX_NUM) {
		memset(a, 0xff, sizeof(*a));
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

	addr = EEPROM_AVVII_START_ADDR + p * sizeof(struct avvii_t);
	eeprom_read(addr, a, sizeof(*a));

	if (a->start_year != 0xff)
		return p;

	return DB_POS_INVALID;
}

static struct data_ant_t data_ant_status[4][24];
static int data_ant_pos[4];

void db_ant_init(void)
{
	memset(data_ant_status, 0, sizeof(data_ant_status));
	memset(data_ant_pos, 0, sizeof(data_ant_pos));
}

static void db_ant_fill_all(struct data_ant_t *d)
{
	struct rtc_t r;
	u32 freq, duty;

	rtc_get(&r);
	d->day = r.day;
	d->month = r.month;
	d->year = r.year;
	d->hour = r.hour;
	d->min = r.min;

	ant_get(&freq, &duty);
	d->freq = (u16)(freq & 0x3ff);
	d->duty = (u16)(duty & 0x3f);
}

static void db_ant_save_record(struct data_ant_t *d)
{
	u32 pos;
	u32 addr;

	eeprom_read(EEPROM_ANT_CUR_POS, &pos, sizeof(pos));

	addr = EEPROM_ANT_START_ADDR + pos * sizeof(struct data_ant_t);
	eeprom_write(addr, d, sizeof(*d));

	pos = (pos + 1) % ANT_MAX_NUM;
	eeprom_write(EEPROM_ANT_CUR_POS, &pos, sizeof(pos));

	/* Invalidate current record */
	memset(d, 0xff, sizeof(*d));
	addr = EEPROM_ANT_START_ADDR + pos * sizeof(struct data_ant_t);
	eeprom_write(addr, d, sizeof(*d));
}


void db_ant_add(struct data_ant_t *d)
{
	int s;
	int cnt;
	u8 en_daily_avg;

	s = d->sens;

	if (s > 3) {
		err("Invalid data sens %d\n", d->sens);
		return;
	}

	cnt = data_ant_pos[s];
	memcpy(&data_ant_status[s][cnt], d, sizeof(*d));

	data_ant_pos[s] = (data_ant_pos[s] + 1) % 24;

	eeprom_read(EEPROM_ENABLE_DAILY_AVG, &en_daily_avg, 1);
	en_daily_avg &= BIT0;
	if (!en_daily_avg) {
		db_ant_fill_all(d);
		db_ant_save_record(d);
	}
}

void db_ant_save_to_eeprom(void)
{
	int s, j;
	struct data_ant_t d;
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
			if (!data_ant_status[s][j].vbat && !data_ant_status[s][j].vread)
				continue;
			cnt++;
			vread += data_ant_status[s][j].vread;
			temp += data_ant_status[s][j].temp;
			hum += data_ant_status[s][j].hum;
			vbat = min(vbat, data_ant_status[s][j].vbat);
		}

		if (!cnt)
			continue;

		d.sens = s;
		d.temp = temp / cnt;
		d.vread = vread / cnt;
		d.hum = hum / cnt;
		d.vbat = vbat;

		if (en_daily_avg) {
			db_ant_fill_all(&d);
			db_ant_save_record(&d);
		}

		if (vbat < BATTERY_THRES)
			db_alarm_add(ALRM_TYPE_BATTERY, s);

	}
	db_ant_init();

}

int db_ant_get(struct data_ant_t *d, int pos)
{
	u32 addr;
	u32 p;

	if (pos != DB_POS_INVALID && pos >= ANT_MAX_NUM)
		return DB_POS_INVALID;

	p = pos;
	if (p == DB_POS_INVALID) {
		eeprom_read(EEPROM_ANT_CUR_POS, &p, sizeof(p));
		if (p == 0)
			p = ANT_MAX_NUM - 1;
		else
			p--;
	}

	addr = EEPROM_ANT_START_ADDR + p * sizeof(struct data_ant_t);
	eeprom_read(addr, d, sizeof(*d));

	if (d->sens > 3 || d->month > 12 || d->day > 31)
		return DB_POS_INVALID;

	return p;
}

void db_ant_display(struct data_ant_t *d, int npage, int pos)
{
	char buf[24];

	if (npage == 1) {

		k_sprintf(buf, "S%d %04d %02d%02d%02d H:%s%d", d->sens + 1, pos,
		    d->day, d->month, d->year,
			d->hum < 100 ? " " : "", d->hum);
		lcd_write_line(buf, 0, 0);

		k_sprintf(buf, "T%c%d.%d B:%s%d.%d mV%c%d",
			d->temp < 0 ? '-': ':', (uint)abs(d->temp / 10), (uint)abs(d->temp % 10),
			d->vbat >= 100 ? "" : " ", (uint)d->vbat / 10, d->vbat % 10,
			(d->vread < 0) ? '-' : ':', (uint)abs(d->vread));
		lcd_write_line(buf, 1, 0);
	}
	else {
		k_sprintf(buf, "Freq: %03d  D.C.: %02d", d->freq, d->duty);
		lcd_write_line(buf, 0, 0);

		k_sprintf(buf, "Time: %02d:%02d", d->hour, d->min);
		lcd_write_line(buf, 1, 0);
	}
}

void db_ant_dump_all()
{
	int p, p_init;
	struct data_ant_t d;
	eeprom_read(EEPROM_ANT_CUR_POS, &p_init, sizeof(p_init));
	p = p_init;

	kprint("\r\n\r\nSENS,DATE,TIME,TEMP,VOLT,HUM,VBAT,FREQ,DUTY\r\n");
	while (p != DB_POS_INVALID) {
		db_ant_get(&d, p);
		if (d.sens != 0xff) {
			kprint("%d,%02d/%02d/%02d,%02d:%02d,%d,%d,%d,%d,%d,%d\r\n",
		        d.sens + 1, d.day, d.month, d.year, d.hour, d.min,
				d.temp,d.vread, d.hum, d.vbat, d.freq, d.duty);
		}
		p = (p + 1) % ANT_MAX_NUM;
		if (p == p_init)
			break;
	}
}

static struct data_osm_t data_osm_status[24];
static int data_osm_pos;

void db_osm_init(void)
{
	memset(data_osm_status, 0, sizeof(data_osm_status));
	data_osm_pos = 0;
}

void db_osm_add(struct data_osm_t *d)
{
	struct rtc_t r;
	rtc_get(&r);
	d->year = r.year;
	d->month = r.month;
	d->day = r.day;
	memcpy(&data_osm_status[data_osm_pos], d, sizeof(*d));
	data_osm_pos = (data_osm_pos + 1) % 24;
}

int db_osm_get(struct data_osm_t *d, int pos)
{
	u32 addr;
	u32 p;

	if (pos != DB_POS_INVALID && pos >= OSM_MAX_NUM)
		return DB_POS_INVALID;

	p = pos;
	if (p == DB_POS_INVALID) {
		eeprom_read(EEPROM_OSM_CUR_POS, &p, sizeof(p));
		if (p == 0)
			p = OSM_MAX_NUM - 1;
		else
			p--;
	}

	addr = EEPROM_OSM_START_ADDR + p * sizeof(struct data_osm_t);
	eeprom_read(addr, d, sizeof(*d));

	if (d->month > 12 || d->day > 31)
		return DB_POS_INVALID;

	return p;
}

void db_osm_display(struct data_osm_t *d, int pos)
{
	char buf[24];

	k_sprintf(buf, "%04d %02d%02d%02d", pos, d->day, d->month, d->year);
	lcd_write_line(buf, 0, 0);

	k_sprintf(buf, "mA1: %03d mA2:%03d", d->mA1, d->mA2);
	lcd_write_line(buf, 1, 0);
}

void db_osm_save_to_eeprom(void)
{
	int j;
	struct data_osm_t d;
	struct rtc_t r;
	int mA1, mA2;
	int cnt;
	u32 pos;
	u32 addr;

	cnt = 0;
	mA1 = mA2 = 0;

	for (j = 0; j < 24; j++) {
		if (!data_osm_status[j].day)
			continue;
		cnt++;
		mA1 += data_osm_status[j].mA1;
		mA2 += data_osm_status[j].mA2;
	}

	if (!cnt)
		return;

	rtc_get(&r);
	d.day = r.day;
	d.month = r.month;
	d.year = r.year;
	d.mA1 = mA1 / cnt;
	d.mA2 = mA2 / cnt;

	eeprom_read(EEPROM_OSM_CUR_POS, &pos, sizeof(pos));

	addr = EEPROM_OSM_START_ADDR + pos * sizeof(struct data_osm_t);
	eeprom_write(addr, &d, sizeof(d));

	/* Increase pos and invalidate next record */

	pos = (pos + 1) % OSM_MAX_NUM;
	eeprom_write(EEPROM_OSM_CUR_POS, &pos, sizeof(pos));
	memset(&d, 0xff, sizeof(d));
	addr = EEPROM_OSM_START_ADDR + pos * sizeof(struct data_osm_t);
	eeprom_write(addr, &d, sizeof(d));

	db_osm_init();
}

void db_osm_dump_all()
{
	/* TODO */
}

void db_data_reset()
{
	u32 tmp;
	u8 page[64];

	tmp = 0;
	eeprom_write(EEPROM_ANT_CUR_POS, &tmp, sizeof(tmp));
	eeprom_write(EEPROM_OSM_CUR_POS, &tmp, sizeof(tmp));

	memset(page, 0xff, sizeof(page));
	for (tmp = EEPROM_ANT_START_ADDR; tmp < EEPROM_ANT_END_ADDR; tmp += 64)
		eeprom_write(tmp, page, sizeof(page));
	for (tmp = EEPROM_OSM_START_ADDR; tmp < EEPROM_OSM_END_ADDR; tmp += 64)
		eeprom_write(tmp, page, sizeof(page));

}
