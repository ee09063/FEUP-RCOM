#ifndef _ALARM_H_
#define _ALARM_H_

#include <signal.h>

void trigger();

void alarmSetup();

void setAlarmFlag(int f);

int getAlarmFlag();

#endif


