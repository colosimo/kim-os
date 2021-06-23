/*
 * Author: Aurelio Colosimo, 2021
 * Copyright: Elo System srl
 */

struct rfrx_frame_t {
	u8 addr;
	u8 msg_id;
	i16 temp;
	u8 hum;
	i16 vread;
	u8 vbat;
	u8 parity;
};

void rfrx_frame_dump(struct rfrx_frame_t *f);

void rfrx_frame_display(struct rfrx_frame_t *f);

void rfrx_clear_lastframe(void);

struct rfrx_frame_t *rfrx_get_lastframe(void);
