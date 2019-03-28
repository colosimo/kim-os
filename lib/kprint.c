/*
 * Author: Aurelio Colosimo, 2016
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

#include <stdarg.h>
#include <endiannes.h>
#include <basic.h>

extern int putchar(int c);

const char hex[] = "0123456789abcdef";

static void printhex(int x, int ndigits)
{
	unsigned char *p;
	int i;
	int c;
	int started = 0;

	if (cpu_be)
		p = (unsigned char *)&x;
	else
		p = &((unsigned char *)&x)[3];

	for (i = 0; i < sizeof(x); i++) {

		if (*p != 0 || (ndigits && i >= sizeof(x) - ndigits / 2) ||
		    i == sizeof(x) - 1) {
			started = 1;
		}

		if (started) {
			c = hex[*p >> 4];
			putchar(c);
			c = hex[*p & 0xf];
			putchar(c);
		}

		if (cpu_be)
			p++;
		else
			p--;
	}
}

/* Minimal printf function. Supports strings, chars and hex numbers. */
void kprint(const char *fmt, ...)
{
	va_list args;
	char *s;
	va_start(args, fmt);
	int ndigits = 0;
	int fmt_prefix = 0;

	for (; *fmt; fmt++) {

		if (*fmt == '%') {
			fmt_prefix = 1;
			continue;
		}

		if (!fmt_prefix) {
			if (*fmt == '\n')
				putchar('\r');
			putchar(*fmt);
			continue;
		}

		fmt_prefix = 0;

		switch (*fmt) {
		case '%':
			putchar('%');
			break;
		case 'c':
			putchar(va_arg(args, int));
			break;
		case 's':
			while (ndigits--)
				putchar(' ');
			s = va_arg(args, char *);
			while (*s)
				putchar(*s++);
			ndigits = 0;
			break;

		case 'p':
			putchar('0');
			putchar('x');
		case 'd':
		case 'u':
		case 'x':
			printhex(va_arg(args, int), ndigits);
			ndigits = 0;
			break;

		default:
			if (isdigit(*fmt)) {
				fmt_prefix = 1;
				ndigits = ndigits * 10 + *fmt - '0';
			}
			break;
		}
	}

	va_end(args);
}
