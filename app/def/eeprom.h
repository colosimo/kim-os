/*
 * Author: Aurelio Colosimo, 2021
 * Copyright: Elo System srl
 */

#include <basic.h>

#define EEPROM_HOURS_ADDR    0x0000

struct pwm_cfg_t {
	u32 freq;
	u32 duty;
};

#define EEPROM_PWM_CFG_ADDR 0x0004


void eeprom_init(void);

int eeprom_write(u16 rndm_addr, u8 *val, int _cnt);

int eeprom_read(u16 rndm_addr, u8 *val, int _cnt);
