/*
 * Author: Aurelio Colosimo, 2016
 *
 * This file is part of kim-os project: https://github.com/colosimo/kim-os
 * According to kim-os license, you can do whatever you want with it,
 * as long as you retain this notice.
 */

#ifndef __IAP_H__
#define __IAP_H__

typedef void (*iap_ptr_t)(unsigned int[], unsigned int[]);
extern iap_ptr_t iap_fun;

#define iap_invoke(c, r) {cpsid(); iap_fun(c, r); cpsie();}

extern unsigned iap_cmd[5];
extern unsigned iap_res[4];

/* IAP Command codes, see UM10398, page 440 */
#define IAP_PREPARE       50
#define IAP_COPY_TO_FLASH 51
#define IAP_ERASE_SECTOR  52
#define IAP_BNK_CHK       53
#define IAP_READ_P_ID     54
#define IAP_READ_BOOT_VER 55
#define IAP_COMPARE       56
#define IAP_REINVOKE_ISP  57
#define IAP_READ_UID      58
#define IAP_ERASE_PAGE    59

/* IAP Status codes, see UM10398, page 445 */
#define IAP_CMD_SUCCESS          0
#define IAP_INVALID_COMMAND      1
#define IAP_SRC_ADDR_ERROR       2
#define IAP_DST_ADDR_ERROR       3
#define IAP_SRC_ADDR_NOT_MAPPED  4
#define IAP_DST_ADDR_NOT_MAPPED  5
#define IAP_COUNT_ERROR          6
#define IAP_INVALID_SECTOR       7
#define IAP_SECTOR_NOT_BLANK     8
#define IAP_SECTOR_NOT_PREPARED  9
#define IAP_COMPARE_ERROR       10
#define IAP_BUSY                11

#endif /* __IAP_H__ */
