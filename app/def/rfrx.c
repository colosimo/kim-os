/*
 * Author: Aurelio Colosimo, 2021
 * Copyright: Elo System srl
 */

#include <stdint.h>
#include <log.h>
#include <kim.h>
#include <linker.h>
#include <kim-io.h>
#include <kim-io-defs.h>
#include <errcode.h>
#include <reg.h>
#include <gpio.h>
#include <cpu.h>

#include "rfrx.h"
#include "def.h"
#include "lcd.h"
#include "db.h"

#define RXRF_MINPULSESHORT     625
#define RXRF_MAXPULSESHORT     2699

#define RXRF_MINPULSELONG      2700
#define RXRF_MAXPULSELONG      3999

#define RXRF_MINSTARTBIT       4000
#define RXRF_MAXSTARTBIT       8400

#define RXRF_SHORT_ZERO           0
#define RXRF_SHORT_ONE            1
#define RXRF_LONG_ZERO            2
#define RXRF_LONG_ONE             3
#define RXRF_START_ZERO           4
#define RXRF_START_ONE            5

static char *fname_rfrx = "rf_rx";
static int fd;

static int cnt;
static int end_ok;
static u32 evts[512];
static u32 evts_val[512];
static int evts_sym[512];
static u8 last_msg_id[4] = {0xff, 0xff, 0xff, 0xff};
static u32 last_intr;

static struct rfrx_frame_t lastf;
static struct rfrx_frame_t *lastf_ptr;

void rfrx_frame_dump(struct rfrx_frame_t *f)
{
	dbg("ADDR:  %d\n", f->addr);
	dbg("ID:    %d\n", f->msg_id);
	dbg("TEMP:  %d\n", f->temp);
	dbg("HUM:   %d\n", f->hum);
	dbg("VREAD: %d\n", f->vread);
	dbg("VBAT:  %d\n", f->vbat);
}

void rfrx_frame_display(struct rfrx_frame_t *f)
{
	char buf[24];

	k_sprintf(buf, "S:%d mV:%c%d  B:%d.%d",
	    f->addr + 1, (f->vread < 0) ? '-' : ' ',
	    (uint)abs(f->vread), f->vbat / 10, f->vbat % 10);
	lcd_write_line(buf, 0, 0);

	k_sprintf(buf, "H:%s%d T:%c%d.%d P:%d",
	    f->hum < 100 ? " " : "", f->hum, f->temp < 0 ? '-': ' ',
(uint)abs(f->temp / 10),
	    (uint)abs(f->temp % 10), f->msg_id);
	lcd_write_line(buf, 1, 0);
}

#ifdef BOARD_elo_new
void isr_exti9_5(void)
#else
void isr_exti15_10(void)
#endif
{
	volatile u32 t;
	u8 now;

#ifdef BOARD_elo_new
	wr32(R_EXTI_PR1, BIT9);
#else
	wr32(R_EXTI_PR1, BIT15);
#endif
	last_intr = k_ticks();

	k_read(fd, &now, 1);

	t = rd32(R_TIM2_CNT) / 16; /* FIXME 16 or 64 MHz */
	wr32(R_TIM2_CR1, 0);
	wr32(R_TIM2_CNT, 0);
	wr32(R_TIM2_CR1, BIT0);

	if (t < RXRF_MINPULSESHORT || t > RXRF_MAXSTARTBIT) {
		cnt = 0;
		return;
	}

	if (cnt >= 116) {
		end_ok = 1;
		return;
	}

	if (cnt < 512) {
		if (t > RXRF_MINPULSESHORT && t <= RXRF_MAXPULSESHORT)
			evts_sym[cnt] = now + RXRF_SHORT_ZERO;
		else if (t > RXRF_MINPULSELONG && t <= RXRF_MAXPULSELONG)
			evts_sym[cnt] = now + RXRF_LONG_ZERO;
		else if (t > RXRF_MINSTARTBIT && t <= RXRF_MAXSTARTBIT)
			evts_sym[cnt] = now + RXRF_START_ZERO;
		else
			return;

		if (cnt == 0 && evts_sym[cnt] != RXRF_START_ZERO && evts_sym[cnt] != RXRF_START_ONE)
			return;

		if (!cnt && !now)
			return;

		evts_val[cnt] = now;
		evts[cnt] = t;
		cnt++;
	}
}

void rfrx_clear_lastframe()
{
	lastf_ptr = NULL;
}

struct rfrx_frame_t *rfrx_get_lastframe()
{
	return lastf_ptr;
}

static void rfrx_start(struct task_t *t)
{
	fd = k_fd_byname(fname_rfrx);

	wr32(R_TIM2_CR1, 0);
	wr32(R_TIM2_CNT, 0);
	wr32(R_TIM2_CR1, BIT0);
#ifdef BOARD_elo_new
	or32(R_EXTI_FTSR1, BIT9);
	or32(R_EXTI_RTSR1, BIT9);
	or32(R_EXTI_IMR1, BIT9);
	or32(R_NVIC_ISER(0), BIT23); /* EXTI9_5 is irq 23 */
#else
	or32(R_EXTI_FTSR1, BIT15);
	or32(R_EXTI_RTSR1, BIT15);
	or32(R_EXTI_IMR1, BIT15);
	or32(R_NVIC_ISER(1), BIT8); /* EXTI15_10 is irq 40 */
#endif
	rfrx_clear_lastframe();
	cnt = 0;
	end_ok = 0;
}

static void rfrx_step(struct task_t *t)
{
	int i, j;
	int bit;
	int sign_temp = 1, sign_vread = 1;
	int pos;
	u8 status;
	struct rfrx_frame_t f;
	u8 parity_check;
	int abort = 0;

	k_read(fd, &status, 1);
	if (cnt == 0 && status) {
		wr32(R_TIM2_CR1, 0);
		wr32(R_TIM2_CNT, 0);
		wr32(R_TIM2_CR1, BIT0);
	}

	if (end_ok) {
		dbg("\nNew rxrf frame\n");
		memset(&f, 0, sizeof(f));

		parity_check = 0;
		for (i = 0; i < cnt; i++) {
			if (i < cnt - 1 && evts_val[i] == evts_val[i + 1]) {
				dbg("rxrf frame error\n");
				abort = 1;
				goto frame_error;
			}
		}

		pos = 0;
		for (i = 0; i < cnt; i += 2) {
			if (evts_sym[i] == RXRF_SHORT_ONE && evts_sym[i + 1] == RXRF_LONG_ZERO)
				bit = 1;
			else if (evts_sym[i] == RXRF_LONG_ONE && evts_sym[i + 1] == RXRF_SHORT_ZERO)
				bit = 0;
			else if (evts_sym[i] == RXRF_START_ZERO || evts_sym[i] == RXRF_START_ONE) {
				continue;
			}
			else {
				dbg("Bad frame, abort i=%d\n\n\n", i);
				for (j = 0; j <= i; j++)
					dbg("%d %d %d\n", (uint)evts_val[j], (uint)evts[j], evts_sym[j]);
				abort = 1;
				goto frame_error;
			}

			if (bit && pos <= 49)
				parity_check = !parity_check;

			if (pos <= 9)
				f.addr |= bit << (pos - 8);
			else if (pos <= 12)
				f.msg_id |= bit << (pos - 10);
			else if (pos == 13)
				sign_temp = bit ? -1 : 1;
			else if (pos <= 22)
				f.temp |= bit << (pos - 14);
			else if (pos <= 30)
				f.hum |= bit << (pos - 23);
			else if (pos == 31)
				sign_vread = bit ? -1 : 1;
			else if (pos <= 40)
				f.vread |= bit << (pos - 32);
			else if (pos <= 48)
				f.vbat |= bit << (pos - 41);
			else if (pos == 49) {
				f.parity = bit;
			}

			pos++;

		}
		f.temp *= sign_temp;
		f.vread *= sign_vread;

frame_error:
		end_ok = 0;
		cnt = 0;

		wr32(R_TIM2_CR1, 0);
		wr32(R_TIM2_CNT, 0);
		wr32(R_TIM2_CR1, BIT0);

		if (abort)
			return;

		if (parity_check) {
			dbg("Parity error\n");
			return;
		}

		if (f.vbat > 150 || f.vbat < 30 || f.hum > 100 || f.addr > 3) {
			dbg("Bad frame:\n");
			rfrx_frame_dump(&f);
		}

		if (f.msg_id != last_msg_id[f.addr] &&
		    parity_check == 0 && f.vbat > 0) {
			struct data_t d;

			d.sens = f.addr;
			d.temp = f.temp;
			d.vread = f.vread;
			d.hum = f.hum;
			d.vbat = f.vbat;

			db_data_add(&d);

			memcpy(&lastf, &f, sizeof(f));
			lastf_ptr = &lastf;
			last_msg_id[f.addr] = f.msg_id;

			if (f.vbat < BATTERY_THRES) {
				if (!get_alarm(ALRM_BITFIELD_BATTERY(f.addr))) {
					db_alarm_add(ALRM_TYPE_BATTERY, f.addr);
					set_alarm(ALRM_BITFIELD_BATTERY(f.addr));
				}
			}
			else
				clr_alarm(ALRM_BITFIELD_BATTERY(f.addr));
		}
	}
}

struct task_t attr_tasks task_rfrx = {
	.start = rfrx_start,
	.step = rfrx_step,
	.intvl_ms = 1,
	.name = "rfrx",
};

static int rfrx_cmd_cb(int argc, char *argv[], int fdout)
{
	log("cnt=%d\n", cnt);
	log("end_ok=%d\n", end_ok);
	log("R_TIM2_CR1=%08x\n", (uint)rd32(R_TIM2_CR1));
	log("R_EXTI_FTSR1=%08x\n", (uint)rd32(R_EXTI_FTSR1));
	log("R_EXTI_RTSR1=%08x\n", (uint)rd32(R_EXTI_RTSR1));
	log("elapsed(last_intr): %d\n", (uint)k_elapsed(last_intr));
	return 0;
}

const struct cli_cmd_t attr_cli cli_rfrx = {
	.narg = 0,
	.cmd = rfrx_cmd_cb,
	.name = "rfrx",
	.descr = "Dump rfrx status",
};
