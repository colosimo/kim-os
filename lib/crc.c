/*
 * Author: Aurelio Colosimo, 2016
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

#include <basic.h>

u16 crc16_ccit(const void *data, size_t n, u16 init)
{
	size_t i, j;
	uint16_t c = init;
	uint8_t d;
	for (i = 0; i < n; i++) {
			d = ((const uint8_t*)data)[i];
			for (j = 0; j < 8; j++) {
					if (((c & 0x8000) >> 8) ^ (d  & 0x80))
							c = (c << 1) ^ 0x1021;
					else
							c <<= 1;
					d <<= 1;
			}
	}
	return c;
}
