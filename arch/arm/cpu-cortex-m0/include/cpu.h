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
#define R_NVIC_ISER   reg32(0xE000E100)
#define R_NVIC_ICER   reg32(0xE000E180)
#define R_NVIC_ISPR   reg32(0xE000E200)
#define R_NVIC_IPR(x) reg32(0xE000E400 + 4 * (x))

#endif /* _CPU_H_ */
