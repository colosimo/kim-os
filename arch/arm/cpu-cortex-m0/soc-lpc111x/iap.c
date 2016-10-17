/*
 * Author: Aurelio Colosimo, 2016
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

#include <iap.h>

#define IAP_LOCATION 0x1fff1ff1

unsigned iap_cmd[5];
unsigned iap_res[4];

iap_ptr_t iap_fun = (iap_ptr_t)IAP_LOCATION;
