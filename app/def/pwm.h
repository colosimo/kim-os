/*
 * Author: Aurelio Colosimo, 2021
 * Copyright: Elo System srl
 */

void pwm_init(void);

void pwm_set(u32 freq, u32 duty);

void pwm_get(u32 *freq, u32 *duty);
