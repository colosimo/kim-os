/*
 * Author: Aurelio Colosimo, 2021
 * Copyright: Elo System srl
 */

#define PWM_DEF_FREQ 100
#define PWM_DEF_DUTY 5

#define MIN_FREQ 20
#define MAX_FREQ 200
#define MIN_DUTY 2
#define MAX_DUTY 10

void pwm_init(void);

int pwm_check(u32 *freq, u32 *duty);

void pwm_set(u32 freq, u32 duty);

void pwm_get(u32 *freq, u32 *duty);
