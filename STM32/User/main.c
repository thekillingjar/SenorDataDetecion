#include "stm32f10x.h" // Device header
#include "Delay.h"
#include "OLED.h"
#include "BEEP.h"
#include "Usart.h"
#include "ESP8266.h"
#include "DHT11.h"
#include "Adc.h"
#include "Timer.h"
#include "Mqtt.h"
#include "Common.h"
#include "Onenet.h"
#include "string.h"
#include "bh1750.h"
void RCC_Configuration(void);

u8 temp;
u8 humi;
float mq2;
float bh1750;
u8 msg[20];
u8 device_id[10] = "12323";
uint8_t *dataPtr;
u32 msg_id = 0;
float smoke_max = 200;
float smoke_min = 0;
float humi_max = 50;
float humi_min = 0;
float temp_max = 40;
float temp_min = 20;
float light_max = 50;
float light_min = 0;
u8 smoke_alarm = 0;
u8 humi_alarm = 0;
u8 temp_alarm = 0;
u8 light_alarm = 0;
char cmp_str[200];
char params[200];

// 解析JSON字符串并提取值
void parseJSON(const char *json_string)
{
	// 查找每个键，并提取对应的值
	sscanf(json_string, "{\"humi_max\":%f,\"humi_min\":%f,\"light_max\":%f,\"light_min\":%f,\"smoke_max\":%f,\"smoke_min\":%f,\"temp_max\":%f,\"temp_min\":%f}",
				 &(humi_max), &(humi_min), &(light_max), &(light_min),
				 &(smoke_max), &(smoke_min), &(temp_max), &(temp_min));

	u1_printf("humi_max: %f\n", humi_max);
	u1_printf("humi_min: %f\n", humi_min);
	u1_printf("light_max: %f\n", light_max);
	u1_printf("light_min: %f\n", light_min);
	u1_printf("smoke_max: %f\n", smoke_max);
	u1_printf("smoke_min: %f\n", smoke_min);
	u1_printf("temp_max: %f\n", temp_max);
	u1_printf("temp_min: %f\n", temp_min);
}

int main(void)
{

	RCC_Configuration();

	delay_init();

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	OLED_Init();

	OLED_ShowString(1, 3, DEVID);

	BEEP_Init();

	AD_Init();

	upper_USART_init(115200);

	Timer_Init(499, 7199); // 0.05s
	if (DHT11_Init() == 0)
		OLED_ShowString(2, 3, "DHT11 Init");

	u1_printf("start\r\n");

	delay_ms(5000);

	//	OneNet_DevLink();
	u1_printf("connect\r\n");
	OLED_ShowString(3, 2, "ESP8266 Init");
	BH1750_Init();
	ESP8266_Init(115200);
	while (OneNet_DevLink())
		;
	OneNET_Subscribe();
	Topic_Init();
	while (1)
	{
		//		delay_ms(1000);
		// BEEP(1);
		if (timeout1)
		{
			timeout1 = 0;
			OLED_Clear();
			DHT11_Read_Data(&temp, &humi);
			ADC1_Average_Data();
			mq2 = Smog_GetPPM();
			if (!i2c_CheckDevice(BH1750_Addr))
			{
				u1_printf("get\r\n");
				bh1750 = LIght_Intensity(); // 获取光照强度
			}
			if (!alarm_stop)
			{
				if (mq2 < 300)
				{
					alarm = 1;
				}
				else
				{
					alarm = 0;
				}
			}

			sprintf((char *)msg, "temperature:%d", temp);
			OLED_ShowString(1, 3, (char *)msg);
			sprintf((char *)msg, "humidity:%d", humi);
			OLED_ShowString(2, 3, (char *)msg);
			sprintf((char *)msg, "s:%f", mq2);
			OLED_ShowString(3, 3, (char *)msg);
			sprintf((char *)msg, "i:%f", bh1750);
			OLED_ShowString(4, 3, (char *)msg);
			OneNet_SendData(temp, humi, mq2, bh1750,
											temp_alarm, humi_alarm, smoke_alarm, light_alarm,
											temp_min, temp_max, humi_min, humi_max,
											smoke_min, smoke_max, light_min, light_max);
		}
		if (disconnet)
		{
			while (ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT"))
				delay_ms(500);
			while (OneNet_DevLink())
				;
			disconnet = 0;
		}
		/*
		kind
		0 temp
		1 humi
		2 smoke
		3 light
		*/
		if (timeout2 == 0)
		{
			if (humi < humi_min || humi > humi_max)
			{
				if (humi_alarm == 0)
					OneNet_Alarm(1, 0);
				humi_alarm = 1;
			}
			else
			{
				if (humi_alarm == 1)
					OneNet_Alarm(1, 1);
				humi_alarm = 0;
			}
		}
		if (timeout2 == 5)
		{
			if (mq2 < smoke_min || mq2 > smoke_max)
			{
				if (smoke_alarm == 0)
					OneNet_Alarm(2, 0);
				smoke_alarm = 1;
			}
			else
			{
				if (smoke_alarm == 1)
					OneNet_Alarm(2, 1);
				smoke_alarm = 0;
			}
		}
		if (timeout2 == 10)
		{
			if (bh1750 < light_min || bh1750 > light_max)
			{
				if (light_alarm == 0)
					OneNet_Alarm(3, 0);
				light_alarm = 1;
			}
			else
			{
				if (light_alarm == 1)
					OneNet_Alarm(3, 1);
				light_alarm = 0;
			}
		}
		if (timeout2 == 15)
		{
			if (temp < temp_min || temp > temp_max)
			{
				if (temp_alarm == 0)
					OneNet_Alarm(0, 0);
				temp_alarm = 1;
			}
			else
			{
				if (temp_alarm == 1)
					OneNet_Alarm(0, 1);
				temp_alarm = 0;
			}
		}

		if (smoke_alarm || light_alarm || temp_alarm || humi_alarm)
			BEEP(1);
		else
			BEEP(0);
		if (search_flag == 6 && ipd_len == 0)
		{
			recv[recv_len] = 0;

			sprintf(cmp_str, "%s", set_topic);
			u8 *rcv = recv;
			if (recv_len < 4)
				continue;
			rcv = rcv + 4;
			u1_printf("\r\nwahaha\r\n%s\r\n", rcv);
			char *ret = strstr((char *)rcv, cmp_str);
			if (ret != NULL)
			{
				u1_printf("1111\r\n");
				rcv = ret + strlen(cmp_str);
				sprintf(cmp_str, "\"id\":");
				ret = strstr((char *)rcv, cmp_str);
				if (ret != NULL)
				{

					rcv = ret + strlen(cmp_str);
					u1_printf("2222%c\r\n", *rcv);
					msg_id = 0;
					do
					{
						rcv = rcv + 1;
						if (!(*rcv >= '0' && *rcv <= '9'))
							break;
						msg_id = msg_id * 10 + (*rcv) - '0';
					} while (*rcv != 0);
					sprintf(cmp_str, "\"params\":");
					ret = strstr((char *)rcv, cmp_str);
					if (ret != NULL)
					{
						u1_printf("333\r\n");
						rcv = ret + strlen(cmp_str);
						memset(params, 0, sizeof(params));
						u16 idx = 0;
						while ((*rcv != 0))
						{
							params[idx++] = *rcv;
							if (*rcv == '}')
								break;
							rcv = rcv + 1;
						}
						params[idx] = 0;
						parseJSON(params);
						OneNet_Reply(msg_id);
					}
				}
			}
			recv_len = 0;
			search_flag = 0;
		}
	}
}
void RCC_Configuration(void)
{
	RCC_DeInit();							 // 初始化为缺省值
	RCC_HSEConfig(RCC_HSE_ON); // 使能外部的高速时钟
	while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET)
		;
	FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
	FLASH_SetLatency(FLASH_Latency_2); // Flash 2 wait state

	RCC_HCLKConfig(RCC_SYSCLK_Div1);										 // HCLK = SYSCLK
	RCC_PCLK2Config(RCC_HCLK_Div1);											 // PCLK2 = HCLK
	RCC_PCLK1Config(RCC_HCLK_Div2);											 // PCLK1 = HCLK / 2
	RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9); // PLLCLK = 8MHZ * 9

	RCC_PLLCmd(ENABLE);

	while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
		;
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	while (RCC_GetSYSCLKSource() != 0x08) // 配置PLL为系统时钟
		;
}
