/*
 * Author: Aurelio Colosimo, 2021
 * Copyright: Elo System srl
 */

#include <basic.h>

#define EEPROM_SIGN_ADDR     0x0000
#define EEPROM_FMT_VER_ADDR  0x0004
#define EEPROM_HOURS_ADDR    0x0008

struct pwm_cfg_t {
	u32 freq;
	u32 duty;
};

#define EEPROM_PWM_CFG_ADDR  0x000c


void eeprom_init(void);

int eeprom_write(u16 rndm_addr, void *val, int _cnt);

int eeprom_read(u16 rndm_addr, void *val, int _cnt);
