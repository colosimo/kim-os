/*
 * Author: Aurelio Colosimo, 2021
 * Copyright: Elo System srl
 */

extern const char zero;
extern const char one;

/* Alarm is handled as a bitfield */
#define ALRM_TYPE_ANT      0
#define ALRM_TYPE_BATTERY  1
#define ALRM_TYPE_START    2
#define ALRM_TYPE_STOP     3
#define ALRM_TYPE_INVALID  0xff

#define ALRM_BITFIELD_ANT      (1 << ALRM_TYPE_ANT)
#define ALRM_BITFIELD_BATTERY  (1 << ALRM_TYPE_BATTERY)
#define ALRM_BITFIELD_START    (1 << ALRM_TYPE_START)
#define ALRM_BITFIELD_STOP     (1 << ALRM_TYPE_STOP)
#define ALRM_BITFIELD_ANY      0xff

#define BATTERY_THRES            60 /* 6V */

void set_alarm(int alrm);
void clr_alarm(int alrm);
int get_alarm(int alrm);

void show_home(void);

void set_standby(int stdby);
int get_standby(void);
void rearm_standby(void);
