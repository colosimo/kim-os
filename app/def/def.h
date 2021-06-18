/*
 * Author: Aurelio Colosimo, 2021
 * Copyright: Elo System srl
 */

extern const char zero;
extern const char one;

void set_alarm(int alrm);
int get_alarm(void);

void set_standby(int stdby);
int get_standby(void);
void rearm_standby(void);
