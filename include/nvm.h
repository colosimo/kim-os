/*
 * Author: Aurelio Colosimo, 2016
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

#ifndef __NVM_H__
#define __NVM_H__

#include <basic.h>
#include <linker.h>

int nvm_prepare_pages(unsigned s_start, unsigned s_end);
int nvm_erase_pages(unsigned s_start, unsigned s_end);
int nvm_copy_to_flash(void *ptr, const void *data, int cnt);
unsigned nvm_get_page(const void *addr);

#define NVM_CMD_SUCCESS          0

#endif /* __NVM_H__ */
