/*
 * Author: Aurelio Colosimo, 2021
 * Copyright: Elo System srl
 */

#define PWM_DEF_FREQ 100
#define PWM_DEF_DUTY 5

void pwm_init(void);

void pwm_set(u32 freq, u32 duty);

void pwm_get(u32 *freq, u32 *duty);
