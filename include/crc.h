/*
 * Author: Aurelio Colosimo, 2016
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

#ifndef _CRC_H_
#define _CRC_H_

#include <intdefs.h>

/* CRC-CCIT computation: polynomial 0x1021 */
u16 crc16_ccit(const void *data, size_t len, u16 init);

#endif /* _CRC_H_ */
