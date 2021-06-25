/*
 * Author: Aurelio Colosimo, 2021
 * Copyright: Elo System srl
 */

#ifndef _EEPROM_H_
#define _EEPROM_H_

#include <basic.h>

#define EEPROM_SIGN_ADDR     0x0000
#define EEPROM_FMT_VER_ADDR  0x0004
#define EEPROM_HOURS_ADDR    0x0008

struct pwm_cfg_t {
	u32 freq;
	u32 duty;
};

#define EEPROM_PWM_CFG_ADDR  0x000c

#define EEPROM_ALARMS_CUR_POS    0x0014
#define EEPROM_AVVII_CUR_POS     0x0018
#define EEPROM_DATA_CUR_POS      0x001c

#define EEPROM_ALARMS_START_ADDR 0x0100
#define EEPROM_ALARMS_END_ADDR   0x04f8

#define EEPROM_AVVII_START_ADDR  0x0500
#define EEPROM_AVVII_END_ADDR    0x08f8

#define EEPROM_DATA_START_ADDR   0x1000
#define EEPROM_DATA_END_ADDR     0x7ff0

void eeprom_init(void);

int eeprom_write(u16 rndm_addr, void *val, int _cnt);

int eeprom_read(u16 rndm_addr, void *val, int _cnt);

#endif /* _EEPROM_H_ */
