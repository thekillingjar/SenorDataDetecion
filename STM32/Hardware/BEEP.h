#ifndef __BEEP_H
#define __BEEP_H
#include "sys.h"

extern u8 alarm;
extern u8 alarm_stop;
void BEEP_Init(void);

#define BEEP(x) GPIO_WriteBit(GPIOB, GPIO_Pin_12, (BitAction)(x))

#endif
