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

void db_init(void)
{
}

void db_alarm_add(int type, int sens)
{
	u32 pos;
	u32 addr;
	struct alarm_t a;
	struct rtc_t r;

	log("add alarm %d\n", type);
	rtc_get(&r);
	a.type = type;
	a.sens = sens;
	a.year = r.year;
	a.month = r.month;
	a.day = r.day;
	a.hour = r.hour;
	a.min = r.min;

	eeprom_read(EEPROM_ALARMS_CUR_POS, &pos, sizeof(pos));

	addr = EEPROM_ALARMS_START_ADDR + pos * sizeof(struct alarm_t);
	eeprom_write(addr, &a, sizeof(a));

	pos = (pos + 1) % ALRM_MAX_NUM;
	eeprom_write(EEPROM_ALARMS_CUR_POS, &pos, sizeof(pos));
}

void db_alarm_get(struct alarm_t *a, int pos)
{
	u32 addr;
	if (pos >= ALRM_MAX_NUM) {
		a->type = ALRM_TYPE_INVALID;
		return;
	}
	addr = EEPROM_ALARMS_START_ADDR + pos * sizeof(struct alarm_t);
	eeprom_read(addr, a, sizeof(*a));
}

void db_alarm_dump(struct alarm_t *a)
{
	char buf[24];
	const char *str[3] = {"Invalid", "Err. Antenna", "Batteria Bassa"};
	k_sprintf(buf, "%s\n", str[a->type & 0b11]);
	log(buf);
	k_sprintf(buf, "SEN:%d %02d/%02d/%02d %02d:%02d\n",
	    a->sens, a->day, a->month, a->year, a->hour, a->min);
	log(buf);
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
