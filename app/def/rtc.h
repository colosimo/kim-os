/*
 * Author: Aurelio Colosimo, 2021
 * Copyright: Elo System srl
 */

struct rtc_t {
	u16 year;
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
