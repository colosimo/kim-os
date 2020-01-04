/*
 * Author: Aurelio Colosimo, 2016
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

#ifndef _KPRINT_H_
#define _KPRINT_H_

/* printf-like function */
void k_printf(const char *fmt, ...) __attribute__((format(printf,1,2)));

void k_sprintf(char *buf, const char *fmt, ...) __attribute__((format(printf,2,3)));

void k_fprintf(int fd, const char *fmt, ...) __attribute__((format(printf,2,3)));

#define kprint k_printf

/* Dump a hex buffer byte by byte */
void k_dumphex(const char *descr, void *buf, int len);

#endif
