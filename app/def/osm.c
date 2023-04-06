/*
 * Author: Aurelio Colosimo, 2023
 * Copyright: Elo System srl
 */

#include <reg.h>
#include <gpio.h>
#include <log.h>
#include <kim-io.h>

#include "osm.h"

/*
 * PWM1: PB0  TIM3_CH3  30Hz - 1kHz 0-100%
 * PWM2: PB1  TIM3_CH4  30Hz - 1kHz 0-100%
 * PWM3: PA11 TIM1_CH4  -> range?
 * PWM4: PA1  TIM5_CH2  -> range?
*/

static struct osm_cfg_t osm_array[2];

#define PRESCALER 40
#define COUNTER_HZ ((16 * MHZ) / PRESCALER)

#define PWM1_PWM2_HZ 500
#define ARR_TIM3_PWM1_PWM2 (COUNTER_HZ / PWM1_PWM2_HZ)

static void print_invalid_channel(int channel)
{
	err("Invalid osm channel %d\n", channel);
}

void osm_init(void)
{
	u32 arr;

	/* PWM1 initialization: TIM3_CH3 on PB0 */
	gpio_func(IO(PORTB, 0), 2);
	and32(R_TIM3_CCMR2, ~0x7f);
	or32(R_TIM3_CCMR2, 0b1111000);
	or32(R_TIM3_CCER, BIT8);

	/* PWM2 initialization: TIM3_CH4 on PB1 */
	gpio_func(IO(PORTB, 1), 2);
	and32(R_TIM3_CCMR2, ~0x7f00);
	or32(R_TIM3_CCMR2, 0b1111000 << 8);
	or32(R_TIM3_CCER, BIT12);

	/* PWM3 initialization: TIM1_CH4 on PA11 */
	gpio_func(IO(PORTA, 11), 1);
	and32(R_TIM1_CCMR2, ~0x7f00);
	or32(R_TIM1_CCMR2, 0b1111000 << 8);
	or32(R_TIM1_CCER, BIT12);
	wr32(R_TIM1_PSC, PRESCALER);

	/* PWM4 initialization: TIM5_CH2 on PA1 */
	gpio_func(IO(PORTA, 1), 2);
	and32(R_TIM5_CCMR1, ~0x7f00);
	or32(R_TIM5_CCMR1, 0b1111000 << 8);
	or32(R_TIM5_CCER, BIT4);
	wr32(R_TIM5_PSC, PRESCALER);

	/* PWM1 and PWM2 initialization for VREF */
	wr32(R_TIM3_PSC, PRESCALER);
	or32(R_TIM3_CR1, BIT0);
	arr = ARR_TIM3_PWM1_PWM2;
	wr32(R_TIM3_ARR, arr - 1);
}

void osm_set_cfg(int channel, struct osm_cfg_t *osm)
{
	u32 arr;

	if (channel < 0 || channel > OSM_CH2) {
		print_invalid_channel(channel);
		return;
	}

	if (osm->enable)
		osm_enable(channel);
	else
		osm_disable(channel);
	arr = COUNTER_HZ / osm->freq;

	if (channel == OSM_CH1) {
		wr32(R_TIM1_ARR, arr - 1);
		wr32(R_TIM1_CCR4, (arr * (100 - osm->duty)) / 100);
		wr32(R_TIM3_CCR3, (ARR_TIM3_PWM1_PWM2 * osm->volt_perc) / 100);
	}
	else if (channel == OSM_CH2) {
		wr32(R_TIM5_ARR, arr - 1);
		wr32(R_TIM5_CCR2, (arr * (100 - osm->duty)) / 100);
		wr32(R_TIM3_CCR4, (ARR_TIM3_PWM1_PWM2 * osm->volt_perc) / 100);
	}

	osm_array[channel] = *osm;
}

void osm_set(int channel, u16 enable, u32 freq, u32 duty, u16 volt_perc)
{
	struct osm_cfg_t osm;
	osm.duty = duty;
	osm.freq = freq;
	osm.volt_perc = volt_perc;
	osm.enable = enable;
	osm_set_cfg(channel, &osm);
}

void osm_get(int channel, struct osm_cfg_t *osm)
{
	if (channel < 0 || channel > OSM_CH2) {
		print_invalid_channel(channel);
		return;
	}

	*osm = osm_array[channel];
}

void osm_measure(int channel, u32 *volt, u32 *cur_mA)
{
	if (channel < 0 || channel > OSM_CH2) {
		print_invalid_channel(channel);
		return;
	}

	if (channel == OSM_CH1) {
	}
	else if (channel == OSM_CH2) {
	}
}

void osm_enable(int channel)
{
	u8 tmp = 0;

	if (channel < 0 || channel > OSM_CH2) {
		print_invalid_channel(channel);
		return;
	}

	if (channel == OSM_CH1) {
		or32(R_TIM1_CR1, BIT0);
		or32(R_TIM1_BDTR, BIT15); /* Moe */
		k_write(k_fd_byname("el1pwr"), &tmp, 1);
	}
	else if (channel == OSM_CH2) {
		or32(R_TIM5_CR1, BIT0);
		k_write(k_fd_byname("el2pwr"), &tmp, 1);
	}

	osm_array[channel].enable = 1;
}

void osm_disable(int channel)
{
	u8 tmp = 1;

	if (channel < 0 || channel > OSM_CH2) {
		print_invalid_channel(channel);
		return;
	}

	if (channel == OSM_CH1) {
		and32(R_TIM1_BDTR, ~BIT15); /* Moe */
		and32(R_TIM1_CR1, ~BIT0);
		k_write(k_fd_byname("el1pwr"), &tmp, 1);
	}
	else if (channel == OSM_CH2) {
		and32(R_TIM5_CR1, ~BIT0);
		k_write(k_fd_byname("el2pwr"), &tmp, 1);
	}

	osm_array[channel].enable = 0;

}
