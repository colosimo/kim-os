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

struct ant_cfg_t {
	u32 freq;
	u32 duty;
};

#define EEPROM_ANT_CURRENT_MODE_ADDR  0x000c /* 3: rolling; 4: log */
#define EEPROM_ANT_STATUS_MODE_ADDR   0x000d

#define EEPROM_ENABLE_DAILY_AVG  0x0010
#define EEPROM_ENABLE_DEF_OUT    0x0011
#define EEPROM_ENABLE_OSM        0x0012
#define EEPROM_ALARMS_CUR_POS    0x0014
#define EEPROM_AVVII_CUR_POS     0x0018
#define EEPROM_ANT_CUR_POS      0x001c

#define EEPROM_ANT_MODE0_ADDR    0x0020
#define EEPROM_ANT_MODE1_ADDR    0x0028
#define EEPROM_ANT_MODE2_ADDR    0x0030

#define EEPROM_ANT_ROL_DAYS_SETTING_ADDR 0x0040
#define EEPROM_ANT_ROL_DAYS_STATUS_ADDR  0x0044

#define EEPROM_BLUETOOTH_ID      0x0050
#define EEPROM_LAST_SEEN_ON_RTC  0x0054 /* rtc is 8 bytes */
#define EEPROM_LAST_SEEN_ON_RTC2 0x0058

#define EEPROM_DEADLINE1_CFG     0x0060
#define EEPROM_DEADLINE2_CFG     0x0070
#define EEPROM_DEADLINE3_CFG     0x0080

#define EEPROM_OSM_CH1_CFG       0x0090
#define EEPROM_OSM_CH2_CFG       0x00a0

#define EEPROM_T_MAX             0x00b0
#define EEPROM_ALRM_OUT_POL      0x00b2

#define EEPROM_EPT_EN            0x00b3
#define EEPROM_EPT_PAUSE         0x00b4
#define EEPROM_EPT_INV           0x00b6

#define EEPROM_CUR_MAX1          0x00c0
#define EEPROM_CUR_MAX2          0x00c2

#define EEPROM_CUR_CHECK1        0x00c4
#define EEPROM_CUR_CHECK2        0x00c8

#define EEPROM_ALARMS_START_ADDR 0x0100
#define EEPROM_ALARMS_END_ADDR   0x04f8

#define EEPROM_AVVII_START_ADDR  0x0500
#define EEPROM_AVVII_END_ADDR    0x08f8

#define EEPROM_ANT_START_ADDR   0x1000
#define EEPROM_ANT_END_ADDR     0x3ff0

void eeprom_init(void);

int eeprom_write(u16 rndm_addr, void *val, int _cnt);

int eeprom_read(u16 rndm_addr, void *val, int _cnt);

#endif /* _EEPROM_H_ */
