/*
 * Author: Aurelio Colosimo, 2023
 * Copyright: Elo System srl
 */

#include <reg.h>
#include <gpio.h>
#include <log.h>
#include <kim-io.h>

#include "osm.h"
#include "eeprom.h"
#include "deadline.h"

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

int osm_is_enabled(int channel)
{
	if (channel < 0 || channel > OSM_CH2) {
		print_invalid_channel(channel);
		return 0;
	}

	if (osm_array[channel].enable)
		return 1;
	else
		return 0;

}

void osm_measure(int channel, u32 *volt_mV, u32 *cur_mA, u32 *temperature)
{
	int NITER = 4;
	int i, j;
	u32 adc[3];
	int adc_in_ch1[] = {0, 10, 12};
	int adc_in_ch2[] = {0, 11, 13};
	int *adc_in;

	if (channel < 0 || channel > OSM_CH2) {
		print_invalid_channel(channel);
		return;
	}

	if (channel == OSM_CH1)
		adc_in = adc_in_ch1;
	else if (channel == OSM_CH2)
		adc_in = adc_in_ch2;

	wr32(R_ADC1_CR2, BIT0);
	wr32(R_ADC1_SMPR2, (0b111 << 0) | (0b111 << 3) | (0b111 << 12));
	for (i = 0; i < 3; i++) {
		adc[i] = 0;
		for (j = 0; j < NITER; j++) {
			wr32(R_ADC1_SQR3, adc_in[i]);
			or32(R_ADC1_CR2, BIT30);
			while((rd32(R_ADC1_SR) & BIT1) == 0);
			adc[i] += rd32(R_ADC1_DR);
		}
		adc[i] /= NITER;
	}

	and32(R_ADC1_CR2, ~BIT0);

	if (temperature)
		*temperature = (adc[0] * 83) / 1024;
	if (volt_mV)
		*volt_mV = (adc[1] * 10) / 3;
	if (cur_mA) {
		*cur_mA = (adc[2] * 721) / 1000; /* current conversion: 1.118 * 3300/4096 = 0.721 */
		if (*cur_mA > 115)
			*cur_mA -= 115;
		else
			*cur_mA = 0;

		if (*cur_mA <= 35)
			*cur_mA = 0; /* filter threshold in O.A. out: if < 150mA, it is 0 */
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

void osm_restart(void)
{
	struct task_t *t;
	t = task_find("osm");
	if (t) {
		task_stop(t);
		task_start(t);
	}
}

static void osm_start(struct task_t *t)
{
	struct osm_cfg_t osm_cfg;
	u8 osm_enable;
	int i;

	osm_init();
	osm_disable(OSM_CH1);
	osm_disable(OSM_CH2);

	eeprom_read(EEPROM_ENABLE_OSM, &osm_enable, 1);
	log("osm_enable =%d\n", osm_enable);

	for (i = OSM_CH1; i <= OSM_CH2; i++) {
		eeprom_read(EEPROM_OSM_CH1_CFG + 0x10 * i, &osm_cfg, sizeof(osm_cfg));
		if (dl_iselapsed() >= 0 || i >= osm_enable)
			osm_cfg.enable = 0;
		else
			osm_cfg.enable = 1;
		eeprom_write(EEPROM_OSM_CH1_CFG + 0x10 * i, &osm_cfg, sizeof(osm_cfg));

		log("CH%d: %d %d %d %d\n", i + 1,
		    (uint)osm_cfg.enable, (uint)osm_cfg.volt_perc,
		    (uint)osm_cfg.freq, (uint)osm_cfg.duty);

		osm_set_cfg(i, &osm_cfg);
	}
}

static int deadline_lock = 0;
static int overtemp = 0;
static int last_ept_minutes = -1;
static int ept_status = -1;
static u32 ept_status_ticks;
static u16 ept_pause, ept_inv;

static void osm_step(struct task_t *t)
{
	u32 v, temp;
	int i;
	int idx;
	u8 temp_max;
	struct rtc_t r;
	u8 ept_en;
	int fd;
	u8 tmp8;

	for (i = OSM_CH1; i <= OSM_CH2; i++) {
		osm_measure(i, &v, NULL, &temp);
		dbg("CH%d: %dmV\n", i + 1, (uint)v);
	}
	dbg("T: %d\n", (uint)temp);

	eeprom_read(EEPROM_T_MAX, &temp_max, 1);
	if (!overtemp && (temp >= temp_max)) {
		dbg("OVERTEMPERATURE!\n");
		osm_disable(OSM_CH1);
		osm_disable(OSM_CH2);
		overtemp = 1;
	}
	else if (overtemp && (temp + 10 <= temp_max)) {
		overtemp = 0;
		dbg("OVERTEMPERATURE ENDED!\n");
		osm_restart();
		return;
	}

	idx = dl_iselapsed();
	if (idx >= 0 && !deadline_lock) {
		osm_disable(OSM_CH1);
		osm_disable(OSM_CH2);
	}
	else if (deadline_lock && idx < 0) {
		deadline_lock = 0;
		if (!overtemp) {
			osm_restart();
			return;
		}
	}

	if (osm_is_enabled(OSM_CH1) || osm_is_enabled(OSM_CH2)) {
		rtc_get(&r);
		if (r.min != last_ept_minutes && (r.min == 0 || r.min == 30)) {
			eeprom_read(EEPROM_EPT_EN, &ept_en, 1);
			if (ept_en) {
				last_ept_minutes = r.min;
				ept_status = 0;
				eeprom_read(EEPROM_EPT_PAUSE, &ept_pause, 2);
				eeprom_read(EEPROM_EPT_INV, &ept_inv, 2);
			}
		}
	}
	if (ept_status < 0)
		return;

	switch (ept_status) {
		case 0:
			ept_status_ticks = k_ticks();
			if (ept_pause > 0) {
				osm_disable(OSM_CH1);
				osm_disable(OSM_CH2);
				ept_status = 1;
			}
			else
				ept_status = 2;
			break;

		case 1:
			if (k_elapsed(ept_status_ticks) > MS_TO_TICKS(1000 * ept_pause)) {
				if (ept_inv > 0) {
					ept_status = 2;
					ept_status_ticks = k_ticks();
					fd = k_fd_byname("reverse");
					if (fd >= 0) {
						tmp8 = 1;
						k_write(fd, &tmp8, 1);
					}
				}
				else
					ept_status = -1;

				osm_restart();
				return;
			}
			break;

		case 2:
			if (k_elapsed(ept_status_ticks) > MS_TO_TICKS(100 * ept_inv)) {
				fd = k_fd_byname("reverse");
				if (fd >= 0) {
					tmp8 = 0;
					k_write(fd, &tmp8, 1);
				}
				ept_status = -1;
			}
			break;

		default:
			break;
	}
}

struct task_t attr_tasks task_osm = {
	.start = osm_start,
	.step = osm_step,
	.intvl_ms = 100,
	.name = "osm",
};

/* OSM out manual command */

static int osm_cmd_cb(int argc, char *argv[], int fdout)
{
	struct osm_cfg_t osm;
	u32 a, v, temp;
	int i;

	for (i = OSM_CH1; i <= OSM_CH2; i++) {
		osm_get(i, &osm);
		osm_measure(i, &v, &a, &temp);
		k_fprintf(fdout, "CH%d %s %d %d %d %dmV, %dmA\n", i + 1, osm.enable ? "EN " : "DIS",
		    (uint)osm.volt_perc, (uint)osm.freq, (uint)osm.duty, (uint)v, (uint)a);
	}

	k_printf("TEMP %d\n", (uint)temp);

	return 0;
}

const struct cli_cmd_t attr_cli cli_osm = {
	.narg = 0,
	.cmd = osm_cmd_cb,
	.name = "osm",
	.descr = "Dump osm status",
};
