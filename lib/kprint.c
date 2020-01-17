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

static int buf_putchar(int c)
{
	buf_putchar_buf[buf_putchar_buflen++] = c;
	return 0;
}

static int log_putchar(int c)
{
	if (c == '\n')
		log_putchar('\r');
	return putchar(c);
}

static int _fprintf_fd;
static int _fprintf(int c)
{
	if (c == '\n')
		_fprintf('\r');
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
	char buf[20];
	unsigned int x;
	int i = sizeof(buf) - 1;
	int d = 0;

	if (!_x && !ndigits)
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
	int perc = 0;

	for (; *fmt; fmt++) {

		if (*fmt == '%' && !perc) {
			perc = 1;
			continue;
		}

		if (!perc) {
			_putchar(*fmt);
			continue;
		}

		switch (*fmt) {
		case '%':
			_putchar('%');
			break;
		case 'c':
			_putchar(va_arg(args, int));
			break;
		case 's':
			s = va_arg(args, char *);

			ndigits -= strlen(s);

			while (*s)
				_putchar(*s++);

			while (ndigits-- > 0)
				_putchar(' ');

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
			ndigits = 0;
			break;

		case 'u':
			printint(va_arg(args, unsigned int), 0, ndigits, _putchar);
			ndigits = 0;
			break;

		default:
			if (isdigit(*fmt))
				ndigits = ndigits * 10 + *fmt - '0';
			break;
		}

		if (!isdigit(*fmt))
			perc = 0;
	}
}

/* Minimal printf functions. */

void k_printf(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vkprint(fmt, args, log_putchar);
	va_end(args);
}

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

void k_dumphex(const char *descr, void *buf, int len)
{
	int i;
	if (descr)
		k_printf("-- begin %s --\n", descr);
	for (i = 0; i < len; i++) {
		k_printf("%02x ", ((u8*)buf)[i]);
		if (i % 8 == 7)
			k_printf("\n");
	}
	k_printf("\n-- end %s --\n", descr);
}
