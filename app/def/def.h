/*
 * Author: Aurelio Colosimo, 2021
 * Copyright: Elo System srl
 */

#ifdef BOARD_elo_new
#define HUMAN_VERSION "1.3"
#else
#define HUMAN_VERSION "v2"
#endif

extern const char zero;
extern const char one;

extern const char led_off;
extern const char led_on;

/* Alarm is handled as a bitfield */
#define ALRM_TYPE_ANT      0
#define ALRM_TYPE_UNUSED   1
#define ALRM_TYPE_START    2
#define ALRM_TYPE_STOP     3
#define ALRM_TYPE_BATTERY  4
#define ALRM_TYPE_TIME     5
#define ALRM_TYPE_TIME_END 6
#define ALRM_TYPE_LAST     7
#define ALRM_TYPE_INVALID  0xff

#define ALRM_BITFIELD_ANT          (1 << ALRM_TYPE_ANT)
#define ALRM_BITFIELD_BATTERY(s)   (1 << ((s) + ALRM_TYPE_BATTERY))
#define ALRM_BITFIELD_ANY          0xff

#define ALRM_OUT_POLARITY_CLOSE 0
#define ALRM_OUT_POLARITY_OPEN  1
#define ALRM_OUT_POLARITY_OFF   2

#define BATTERY_THRES            60 /* 6V */

void set_alarm(int alrm);
void clr_alarm(int alrm);
int get_alarm(int alrm);

void show_home(void);

void set_standby(int stdby);
int get_standby(void);
void rearm_standby(void);

void rolling_start(int mode);
void rolling_stop(void);

void ant_check_enable(int en);

u32 get_menu_timeout_ms(void);

int get_deadline_idx(void);
