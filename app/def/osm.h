/*
 * Author: Aurelio Colosimo, 2023
 * Copyright: Elo System srl
 */

#ifndef _OSM_H_
#define _OSM_H_

#include <kim.h>

#define OSM_CH1 0
#define OSM_CH2 1

attr_packed struct osm_cfg_t {
	u16 enable;
	u16 volt_perc;
	u32 freq;
	u32 duty;
};

attr_packed struct osm_cur_check_t {
	u8 enable;
	u8 max_perc;
	u8 intvl;
	u8 unused;
};

void osm_init(void);
void osm_restart(void);

void osm_set(int channel, u16 enable, u32 freq, u32 duty, u16 volt_perc);
void osm_set_cfg(int channel, struct osm_cfg_t *osm);

void osm_get(int channel, struct osm_cfg_t *osm);
void osm_enable(int channel);
void osm_disable(int channel);

void osm_set_max(int channel, u16 cur_max);
void osm_get_max(int channel, u16 *cur_max);
void osm_set_cur_check(int channel, struct osm_cur_check_t *check);
void osm_get_cur_check(int channel, struct osm_cur_check_t *check);

int osm_is_enabled(int channel);

void osm_measure(int channel, u32 *volt_mV, u32 *cur_mA, u32 *temperature);

int osm_short_circuit(int channel);

#endif /* _OSM_H_ */
