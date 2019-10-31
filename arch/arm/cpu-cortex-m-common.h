/*
 * Author: Aurelio Colosimo, 2019
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

#ifndef _CPU_CORTEX_M_COMMON_H_
#define _CPU_CORTEX_M_COMMON_H_

void attr_weak isr_hf(void);

void attr_weak isr_none(void);

void isr_reset(void);

void sleep(void);

void attr_weak isr_systick(void);

u32 attr_weak k_ticks(void);

#endif /* _CPU_CORTEX_M_COMMON_H_ */
