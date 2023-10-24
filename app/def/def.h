/*
 * Author: Aurelio Colosimo, 2021
 * Copyright: Elo System srl
 */

#ifdef BOARD_elo_new
#define HUMAN_VERSION "0.13"
#else
#define HUMAN_VERSION "v2"
#endif

extern const char zero;
extern const char one;

extern const char led_off;
extern const char led_on;

#ifdef BOARD_elo_new
extern const char buzzer_on;
extern const char buzzer_off;
#endif

/* Alarm is handled as a bitfield */
#define ALRM_TYPE_ANT           0
#define ALRM_TYPE_OVERTEMP      1
#define ALRM_TYPE_START         2
#define ALRM_TYPE_STOP          3
#define ALRM_TYPE_BATTERY       4
#define ALRM_TYPE_TIME          5
#define ALRM_TYPE_TIME_END      6
#define ALRM_TYPE_UNUSED0       7
#define ALRM_TYPE_UNUSED1       8
#define ALRM_TYPE_UNUSED2       9
#define ALRM_TYPE_UNUSED3      10
#define ALRM_TYPE_UNUSED4      11
#define ALRM_TYPE_SHORT(c)    (12 + (c))
#define ALRM_TYPE_PEAK(c)     (14 + (c))
#define ALRM_TYPE_LAST         16
#define ALRM_TYPE_INVALID    0xff

#define ALRM_BITFIELD_ANT          (1 << ALRM_TYPE_ANT)
#define ALRM_BITFIELD_BATTERY(s)   (1 << ((s) + ALRM_TYPE_BATTERY))
#define ALRM_BITFIELD_TIME         (1 << (ALRM_TYPE_TIME))
#define ALRM_BITFIELD_OVERTEMP     (1 << ALRM_TYPE_OVERTEMP)
#define ALRM_BITFIELD_SHORT(c)     (1 << (ALRM_TYPE_SHORT(c)))
#define ALRM_BITFIELD_PEAK(c)      (1 << (ALRM_TYPE_PEAK(c)))
#define ALRM_BITFIELD_ANY          0xffffffff

#define ALRM_OUT_POLARITY_CLOSE 0
#define ALRM_OUT_POLARITY_OPEN  1
#define ALRM_OUT_POLARITY_OFF   2

#define BATTERY_THRES            60 /* 6V */

#define CUR_MAX_DEF 500 /* mA */

void set_alarm(int alrm);
void clr_alarm(int alrm);
int get_alarm(int alrm);
const char *get_alarm_str_by_type(u32 alrm_type);

int get_alarm_bitfield(void);

void show_home(void);
void reset_active_alarms(void);

void set_standby(int stdby);
int get_standby(void);
void rearm_standby(void);

void rolling_start(int mode);
void rolling_stop(void);

void ant_check_enable(int en);

u32 get_menu_timeout_ms(void);

int get_deadline_idx(void);

void def_restart(void);
