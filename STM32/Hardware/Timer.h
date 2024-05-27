#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
void Timer_Init(u16 arr, u16 psc);
extern u8 timeout1;
extern u8 count_flag;
extern u16 alarm_stop_count;
extern u16 timeout2;
#endif
