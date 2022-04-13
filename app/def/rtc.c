/*
 * Author: Aurelio Colosimo, 2021
 * Copyright: Elo System srl
 */
#include <kim.h>
#include <reg.h>
#include <gpio.h>
#include <log.h>
#include <reg.h>

#include "rtc.h"

void rtc_set(const struct rtc_t *r)
{
	/* Sample code showing how to set an arbitrary date, e.g. Tue 2021-06-08 13:39:00 */
	u32 tr, dr;

	wr32(R_RTC_WPR, 0xca);
	wr32(R_RTC_WPR, 0x53);
	or32(R_RTC_ISR, BIT7);
	while (!(rd32(R_RTC_ISR) & BIT6));

	/* Time register setting */
	tr = ((r->hour / 10) << 20) | ((r->hour % 10) << 16) |
	     ((r->min / 10) << 12) | ((r->min % 10) << 8) |
	     ((r->sec / 10) << 4) | (r->sec % 10);
	wr32(R_RTC_TR, tr);

	/* Date register setting */
	dr = (((r->year % 100) / 10) << 20) | ((r->year % 10) << 16) |
	     ((r->month / 10) << 12) | ((r->month % 10) << 8) |
	     ((r->day / 10) << 4) | (r->day % 10) | (r->wdu & 0b111) << 13;
	wr32(R_RTC_DR, dr);

	and32(R_RTC_ISR, ~BIT7);
	wr32(R_RTC_WPR, 0xff);
}

void rtc_get(struct rtc_t *r)
{
	u32 dr, tr;

	tr = rd32(R_RTC_TR);
	dr = rd32(R_RTC_DR);

	r->year = 10 * ((dr >> 20) & 0xf) + ((dr >> 16) & 0xf);
	r->month = 10 * ((dr >> 12) & 0x1) + ((dr >> 8) & 0xf);
	r->day = 10 * ((dr >> 4) & 0x3) + (dr & 0xf);
	r->wdu = (dr >> 13) & 0x7;

	r->hour = 10 * ((tr >> 20) & 0x3) + ((tr >> 16) & 0xf);
	r->min = 10 * ((tr >> 12) & 0x7) + ((tr >> 8) & 0xf);
	r->sec = 10 * ((tr >> 4) & 0x7) + (tr & 0xf);
}

int rtc_valid(const struct rtc_t *r)
{
	int days_in_month[23] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	if (r->year % 4 == 0)
		days_in_month[1] = 29;

	return r->month < 12 && r->day <= days_in_month[r->month - 1];
}

static void _rtc_dump(const struct rtc_t *r, int _log)
{
	const char *days[7] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
	const char *fmt = "%s %d-%02d-%02d %02d:%02d:%02d";

	if (_log) {
		log(fmt, days[r->wdu - 1], r->year, r->month, r->day, r->hour, r->min, r->sec);
		kprint("\n");
	}
	else
		kprint(fmt, days[r->wdu - 1], r->year, r->month, r->day, r->hour, r->min, r->sec);
}

void rtc_dump(const struct rtc_t *r)
{
	_rtc_dump(r, 1);
}

void rtc_dump_kprint(const struct rtc_t *r)
{
	_rtc_dump(r, 0);
}

static int rtc_cmd_cb(int argc, char *argv[], int fdout)
{
	struct rtc_t r;

	if (argc < 8) {
		rtc_get(&r);
		rtc_dump(&r);
		return 0;
	}

	r.year = atoi(argv[1]);
	r.month = atoi(argv[2]);
	r.day = atoi(argv[3]);
	r.hour = atoi(argv[4]);
	r.min = atoi(argv[5]);
	r.sec = atoi(argv[6]);
	r.wdu = atoi(argv[7]);

	rtc_set(&r);

	return 0;
}

const struct cli_cmd_t attr_cli cli_rtc = {
	.narg = 0,
	.cmd = rtc_cmd_cb,
	.name = "rtc",
	.descr = "rtc [yy] [mo] [dd] [hh] [min] [sec] [wd]",
};
