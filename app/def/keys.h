/*
 * Author: Aurelio Colosimo, 2021
 * Copyright: Elo System srl
 */

#define KEY_UP		0
#define KEY_DOWN	1
#define KEY_ESC		2
#define KEY_ENTER	3

u32 keys_get_evts(void);

void keys_clear_evts(u32 evts);
