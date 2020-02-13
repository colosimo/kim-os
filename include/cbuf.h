/*
 * Author: Aurelio Colosimo, 2019
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

#ifndef _CBUF_H_
#define _CBUF_H_

#include <intdefs.h>

struct cbuf_t {
	size_t size;
	int wr;
	int rd;
	u8 *buf;
};

void cbuf_init(struct cbuf_t *c, void *buf, size_t size);

int cbuf_avail(struct cbuf_t *c);

int cbuf_read(struct cbuf_t *c, void *buf, size_t len);

int cbuf_write(struct cbuf_t *c, void *buf, size_t len);

void *cbuf_get_wrpos(struct cbuf_t *c, size_t *size);

void cbuf_wrdone(struct cbuf_t *c, size_t len);

void *cbuf_get_rdpos(struct cbuf_t *c, size_t *size);

void cbuf_rddone(struct cbuf_t *c, size_t len);

#endif /* _CBUF_H_ */
