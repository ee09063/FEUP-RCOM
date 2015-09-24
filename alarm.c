#include "alarm.h"

int flag = 1;

void trigger(){
	flag = 1;
}

void setAlarmFlag(int f) {
	flag = f;
}

int getAlarmFlag() {
	return flag;
}

void alarmSetup() {
	(void) signal(SIGALRM, trigger);
}

