/*
 * Author: Aurelio Colosimo, 2016
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

#include <basic.h>

static int _memcmp(const void *s1, const void *s2, size_t n, int isstring)
{
	uint8_t b1, b2;
	for (; n; s1++, s2++, n--) {
		b1 = *((uint8_t*)s1);
		b2 = *((uint8_t*)s2);

		if (isstring && b1 == '\0' && b2 == '\0')
			break;

		if (b1 > b2)
				return 1;
		else if (b1 < b2)
				return -1;
	}
	return 0;
}

static void *_memcpy(void *dest, const void *src, size_t n, int isstring)
{
	while (n--) {
		if (isstring && *((u8*)src) == 0)
			break;
		*((u8*)dest++) = *((u8*)src++);
	}
	return dest;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
	return _memcmp(s1, s2, n, 0);
}

void *memcpy(void *dest, const void *src, size_t n)
{
	return _memcpy(dest, src, n, 0);
}

void *memset(void *dest, int c, size_t n)
{
	while (n--) {
		*((u8*)dest) = c;
		dest++;
	}
	return dest;
}

int strcmp(const char *s1, const char *s2)
{
	return _memcmp(s1, s2, SIZE_MAX, 1);
}

int strncmp(char *s1, const char *s2, size_t n)
{
	return _memcmp(s1, s2, n, 1);
}

char *strcpy(char *s1, const char *s2)
{
	return _memcpy(s1, s2, SIZE_MAX, 1);
}

char *strncpy(char *s1, const char *s2, size_t n)
{
	return _memcpy(s1, s2, n, 1);
}

size_t strlen(const char *s1)
{
	size_t l = 0;
	while (*s1++)
		l++;
	return l;
}

int isspace(int c)
{
	return c == ' ' || c == '\t' || c == '\n' ||
	    c == '\v' || c == '\f' || c == '\r';
}

int isdigit(int c)
{
	return c >= '0' && c <= '9';
}

int isalpha(int c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int ishexdigit(int c)
{
	return isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

int isalnum(int c)
{
	return isalpha(c) || isdigit(c);
}

int atoi(const char *p)
{
	int sign = 0;
	int ret = 0;
	if (p[0] == '-') {
		sign = 1;
		p++;
	}
	while (isdigit(*p)) {
		ret = ret * 10 + (*p - '0');
		p++;
	}
	return sign ? -ret : ret;
}

int atoi_hex(const char *p)
{
	int ret = 0;

	if (p[0] == '0' && p[1] == 'x') /* Skip initial 0x, if any */
		p += 2;

	while(ishexdigit(*p)) {
		ret *= 16;
		if (isdigit(*p))
			ret += (*p - '0');
		else
			ret += ((*p) | BIT5) + 10 - 'a';
		p++;
	}
	return ret;
}
