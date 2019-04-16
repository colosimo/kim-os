/*
 * Author: Aurelio Colosimo, 2019
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

#include <basic.h>
#include <cbuf.h>

void cbuf_init(struct cbuf_t *c, void *buf, size_t size)
{
	c->buf = buf;
	c->wr = 0;
	c->size = size;
	c->rd = 0;
}

int cbuf_avail(struct cbuf_t *c)
{
	return (c->wr + c->size - c->rd) % c->size;
}

int cbuf_read(struct cbuf_t *c, void *buf, size_t size)
{
	int i = 0;
	for (; c->rd != c->wr && size; c->rd = (c->rd + 1) % c->size, size--, i++)
		((u8*)buf)[i] = c->buf[c->rd];
	return i;
}

int cbuf_write(struct cbuf_t *c, void *buf, size_t size)
{
	int i = 0;
	for (; size; c->wr = (c->wr + 1) % c->size, size--, i++) {
		c->buf[c->wr] = ((u8*)buf)[i];

		/* NOTE: overrun */
		if (c->rd == (c->wr + 1) % c->size)
			c->rd = (c->rd + 1) % c->size;
	}
	return i;
}
