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

attr_packed struct avvii_t
{
	u8 start_year;
	u8 start_month;
	u8 start_day;
	u8 stop_year;
	u8 stop_month;
	u8 stop_day;
	u16 days;
};

attr_packed struct data_ant_t
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

attr_packed struct data_osm_t
{
	u8 year;
	u8 month;
	u8 day;
	u8 mA1;
	u8 mA2;
	u8 unused[11];
};

#define ALRM_MAX_NUM ((EEPROM_ALARMS_END_ADDR - EEPROM_ALARMS_START_ADDR) / \
    sizeof(struct alarm_t))
#define AVVII_MAX_NUM ((EEPROM_AVVII_END_ADDR - EEPROM_AVVII_START_ADDR) / \
    sizeof(struct alarm_t))
#define ANT_MAX_NUM ((EEPROM_ANT_END_ADDR - EEPROM_ANT_START_ADDR) / \
    sizeof(struct data_ant_t))
#define OSM_MAX_NUM ((EEPROM_OSM_END_ADDR - EEPROM_OSM_START_ADDR) / \
    sizeof(struct data_ant_t))

void db_alarm_add(int type, int sens);

int db_alarm_get(struct alarm_t *a, int pos);

void db_alarm_dump(struct alarm_t *a);

void db_alarm_dump_all();

void db_alarm_display(struct alarm_t *a, u16 idx);

void db_alarm_reset(void);

void db_avvii_add_start();

void db_avvii_refresh_stop(u16 days);

void db_avvii_dump_all(void);

void db_avvii_display(struct avvii_t *a, u16 pos);

void db_avvii_reset(void);

int db_avvii_get(struct avvii_t *a, int pos);

void db_ant_init(void);

void db_ant_add(struct data_ant_t *d);

int db_ant_get(struct data_ant_t *d, int pos);

void db_ant_display(struct data_ant_t *d, int npage);

void db_ant_save_to_eeprom(void);

void db_ant_dump_all();

void db_data_reset(void);
