/*
 * Author: Aurelio Colosimo, 2021
 * Copyright: Elo System srl
 */

#define ALRM_TYPE_INVALID  0xff
#define ALRM_TYPE_ANT      0x01
#define ALRM_TYPE_BATTERY  0x02

attr_packed struct alarm_t
{
	u8 type;
	u8 sens;
	u8 year;
	u8 month;
	u8 day;
	u8 hour;
	u8 min;
	u8 dummy;
};

#define ALRM_MAX_NUM ((EEPROM_ALARMS_END_ADDR - EEPROM_ALARMS_START_ADDR) / sizeof(struct alarm_t))

void db_init(void);

void db_alarm_add(int type, int sens);

void db_alarm_get(struct alarm_t *a, int pos);

void db_alarm_dump(struct alarm_t *a);

void db_alarm_reset(void);

void db_reset_storici(void);
