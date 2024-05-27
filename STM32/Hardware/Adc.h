#ifndef __MQ2_H
#define __MQ2_H
#include "stm32f10x.h"
extern uint16_t AD_Value[1];
void AD_Init(void);
void ADC1_Average_Data(void);
float Smog_GetPPM(void);
#define CAL_PPM 10 // ?????PPM?
#define RL 10      // RL??
#define R0 26      // R0??

#endif
