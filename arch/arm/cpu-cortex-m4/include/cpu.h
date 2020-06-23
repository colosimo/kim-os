/*
 * Author: Aurelio Colosimo, 2016
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

#ifndef _CPU_H_
#define _CPU_H_

#include <basic.h>
#include <linker.h>
#include <cpu-cortex-m-common.h>

/* NVIC registers */
#define R_NVIC_ISER(x) reg32(0xE000E100 + 4 * (x))
#define R_NVIC_ICER(x) reg32(0xE000E180 + 4 * (x))
#define R_NVIC_ISPR(x) reg32(0xE000E200 + 4 * (x))
#define R_NVIC_ICPR(x) reg32(0xE000E280 + 4 * (x))
#define R_NVIC_IPR(x)  reg32(0xE000E400 + 4 * (x))

/* FPU registers */
#define R_FPU_CPACR    reg32(0xE000ED88)
#define R_FPU_CPCCR    reg32(0xE000EF34)
#define R_FPU_CPCAR    reg32(0xE000EF38)
#define R_FPU_FPDSCR   reg32(0xE000EF3C)

#endif /* _CPU_H_ */
