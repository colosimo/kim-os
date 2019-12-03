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
#include <kim-io.h>

extern int putchar(int c);

static const char hex[] = "0123456789abcdef";

static char *buf_putchar_buf;
static int buf_putchar_buflen;

int buf_putchar(int c)
{
	buf_putchar_buf[buf_putchar_buflen++] = c;
	return 0;
}

static int _fprintf_fd;
int _fprintf(int c)
{
	k_write(_fprintf_fd, &c, 1);
	return 0;
}

static void printhex(int x, int ndigits, int (*_putchar)(int))
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

		/* FIXME: fix ndigits handling when odd */
		if (*p != 0 || (ndigits && i >= sizeof(x) - ndigits / 2) ||
		    i == sizeof(x) - 1) {
			started = 1;
		}

		if (started) {
			c = hex[*p >> 4];
			_putchar(c);
			c = hex[*p & 0xf];
			_putchar(c);
		}

		if (cpu_be)
			p++;
		else
			p--;
	}
}

static void printint(int _x, int sgnd, int ndigits, int (*_putchar)(int))
{
	char buf[13];
	unsigned int x;
	int i = sizeof(buf) - 1;
	int d = 0;

	if (_x == 0)
		buf[i--] = '0';

	if (sgnd)
		x = (_x < 0) ? -_x : _x;
	else
		x = _x;

	while ((x || d < ndigits) && i > 1) {
		buf[i--] = '0' + x % 10;
		x /= 10;
		d++;
	}

	if (sgnd && _x < 0)
		buf[i] = '-';
	else
		i++;

	for (; i < sizeof(buf); i++)
		_putchar(buf[i]);
}

static void vkprint(const char *fmt, va_list args, int (*_putchar)(int))
{
	char *s;
	int ndigits = 0;
	int fmt_prefix = 0;

	for (; *fmt; fmt++) {

		if (*fmt == '%') {
			fmt_prefix = 1;
			continue;
		}

		if (!fmt_prefix) {
			if (*fmt == '\n')
				_putchar('\r');
			_putchar(*fmt);
			continue;
		}

		fmt_prefix = 0;

		switch (*fmt) {
		case '%':
			_putchar('%');
			break;
		case 'c':
			_putchar(va_arg(args, int));
			break;
		case 's':
			while (ndigits--)
				_putchar(' ');
			s = va_arg(args, char *);
			while (*s)
				_putchar(*s++);
			ndigits = 0;
			break;

		case 'p':
			_putchar('0');
			_putchar('x');
		case 'x':
		case 'X':
			printhex(va_arg(args, int), ndigits, _putchar);
			ndigits = 0;
			break;

		case 'd':
			printint(va_arg(args, unsigned int), 1, ndigits, _putchar);
			break;

		case 'u':
			printint(va_arg(args, unsigned int), 0, ndigits, _putchar);
			break;

		default:
			if (isdigit(*fmt)) {
				fmt_prefix = 1;
				ndigits = ndigits * 10 + *fmt - '0';
			}
			break;
		}
	}
}

/* Minimal printf function. Supports strings, chars and hex numbers. */
void kprint(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vkprint(fmt, args, putchar);
	va_end(args);
}

/* Minimal printf function. Supports strings, chars and hex numbers. */
void k_sprintf(char *buf, const char *fmt, ...)
{
	va_list args;
	buf_putchar_buf = buf;
	buf_putchar_buflen = 0;
	va_start(args, fmt);
	vkprint(fmt, args, buf_putchar);
	va_end(args);
	buf_putchar('\0');
}

void k_fprintf(int fd, const char *fmt, ...)
{
	va_list args;
	_fprintf_fd = fd;
	va_start(args, fmt);
	vkprint(fmt, args, _fprintf);
	va_end(args);
}
