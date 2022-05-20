/*
 * Author: Aurelio Colosimo, 2021
 * Copyright: Elo System srl
 */

#ifndef _RTC_H_
#define _RTC_H_

#include <kim.h>

struct attr_packed rtc_t {
	u8 year;
	u8 month;
	u8 day;
	u8 hour;
	u8 min;
	u8 sec;
	u8 wdu;
};

void rtc_set(const struct rtc_t *r);

void rtc_get(struct rtc_t *r);

void rtc_dump(const struct rtc_t *r);

void rtc_dump_kprint(const struct rtc_t *r);

int rtc_valid(const struct rtc_t *r);

/* 1: r1 < r2; 0: r1 == r2; -1: r1 > r2 */
int rtc_compare(const struct rtc_t *r1, const struct rtc_t *r2);

#endif
