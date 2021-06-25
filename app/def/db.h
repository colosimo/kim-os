/*
 * Author: Aurelio Colosimo, 2021
 * Copyright: Elo System srl
 */

#define ALRM_TYPE_ANT      0x00
#define ALRM_TYPE_BATTERY  0x01
#define ALRM_TYPE_START    0x02
#define ALRM_TYPE_STOP     0x03
#define ALRM_TYPE_INVALID  0xff

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

#define ALRM_MAX_NUM ((EEPROM_ALARMS_END_ADDR - EEPROM_ALARMS_START_ADDR) / sizeof(struct alarm_t))
#define AVVII_MAX_NUM ((EEPROM_AVVII_END_ADDR - EEPROM_AVVII_START_ADDR) / sizeof(struct alarm_t))

void db_init(void);

void db_start_add(void);

void db_alarm_add(int type, int sens);

void db_alarm_get(struct alarm_t *a, int pos);

void db_alarm_dump(struct alarm_t *a);

void db_alarm_display(struct alarm_t *a);

void db_alarm_reset(void);

void db_avvii_reset(void);

int db_avvii_get(struct alarm_t *a, int pos);

void db_reset_storici(void);

void db_data_reset(void);
