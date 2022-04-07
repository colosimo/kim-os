/*
 * Author: Aurelio Colosimo, 2021
 * Copyright: Elo System srl
 */

#include "eeprom.h"

#define DB_POS_INVALID 0xffffffff

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

attr_packed struct data_t
{
	u8 sens;
	u8 year;
	u8 month;
	u8 day;
	i16 temp;
	i16 vread;
	u8 hum;
	u8 vbat;
	u16 freq: 10;
	u16 duty: 6;
	u8 hour;
	u8 min;
	u8 unused[2];
};

#define ALRM_MAX_NUM ((EEPROM_ALARMS_END_ADDR - EEPROM_ALARMS_START_ADDR) / \
    sizeof(struct alarm_t))
#define AVVII_MAX_NUM ((EEPROM_AVVII_END_ADDR - EEPROM_AVVII_START_ADDR) / \
    sizeof(struct alarm_t))
#define DATA_MAX_NUM ((EEPROM_DATA_END_ADDR - EEPROM_DATA_START_ADDR) / \
    sizeof(struct data_t))

void db_start_add(void);

void db_alarm_add(int type, int sens);

int db_alarm_get(struct alarm_t *a, int pos);

void db_alarm_dump(struct alarm_t *a);

void db_alarm_dump_all();

void db_alarm_display(struct alarm_t *a);

void db_alarm_reset(void);

void db_avvii_reset(void);

int db_avvii_get(struct alarm_t *a, int pos);

void db_data_init(void);

void db_data_add(struct data_t *d);

int db_data_get(struct data_t *d, int pos);

void db_data_display(struct data_t *d, int npage);

void db_data_save_to_eeprom(void);

void db_data_reset(int erase_all);

void db_data_dump_all();
