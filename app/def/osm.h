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

void osm_init(void);

void osm_set(int channel, u16 enable, u32 freq, u32 duty, u16 volt_perc);
void osm_set_cfg(int channel, struct osm_cfg_t *osm);

void osm_get(int channel, struct osm_cfg_t *osm);
void osm_enable(int channel);
void osm_disable(int channel);

void osm_measure(int channel, u32 *volt_mV, u32 *cur_mA, u32 *temperature);

#endif /* _OSM_H_ */
