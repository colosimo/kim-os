/*
 * Author: Aurelio Colosimo, 2016
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

#include <kim.h>
#include <kprint.h>

u32 k_ticks(void) /* dummy */
{
	return 0;
}

u32 k_ticks_freq(void) /* dummy */
{
	return 100;
}

int main(int argc, char *argv[])
{
	kprint("Hello!\n");
	k_main();
	return 0;
}
