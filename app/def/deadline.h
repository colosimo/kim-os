/*
 * Author: Aurelio Colosimo, 2022
 * Copyright: Elo System srl
 */

#ifndef _DEADLINE_H_
#define _DEADLINE_H_

#include <basic.h>
#include "rtc.h"

struct deadline_t {
	struct rtc_t rtc;
	u8 code[6];
	u8 enable;
};

void dl_reset_all(void);

void dl_load_all(void);

void dl_get(int idx, struct deadline_t *dl);

void dl_set(int idx, struct deadline_t *dl);

int dl_valid(struct deadline_t *dl);

void dl_reset(struct deadline_t *dl, int idx);

void dl_dump(struct deadline_t *dl);

int dl_isactive(void);

int dl_iselapsed(void);

int dl_unlock(int idx, u8 *code);

void dl_disable_all(void);

#endif /* _DEADLINE_H_ */
