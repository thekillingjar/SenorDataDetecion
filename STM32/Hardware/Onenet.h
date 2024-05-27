#ifndef _ONENET_H_
#define _ONENET_H_
//???????l?
#include "stm32f10x.h"

_Bool OneNet_DevLink(void);

void OneNet_SendData(u8 temp, u8 humi, float mq2, float bh1750,
                     u8 temp_alarm, u8 humi_alarm, u8 smoke_alarm, u8 light_alarm,
                     float temp_min, float temp_max, float humi_min, float humi_max,
                     float smoke_min, float smoke_max, float light_min, float light_max);

void OneNet_RevPro(unsigned char *cmd);
void OneNET_Subscribe(void);
uint8_t onenet_state(void);
void Topic_Init(void);
void OneNet_Alarm(u8 kind, u8 flag);
#define PROID "4Ikzr7wSr2" // ��ƷID

//#define AUTH_INFO "version=2018-10-31&res=products%2F4Ikzr7wSr2%2Fdevices%2Fdev2&et=1880328113&method=sha256&sign=WXJqy8zxlnkzxzL2GtFBQXqabXPTCHJGjlmltnZ%2Fp1Q%3D" // ��Ȩ��Ϣ
 #define AUTH_INFO "version=2018-10-31&res=products%2F4Ikzr7wSr2%2Fdevices%2Fdev1&et=1880328113&method=sha256&sign=oKmyQo%2FhYSEsS04l3j4a9bt6%2BeXzwzhRU43bmqhbiWQ%3D"
//#define DEVID "dev2" // �豸����
 #define DEVID "dev1"
extern u8 set_topic[100];
#endif
