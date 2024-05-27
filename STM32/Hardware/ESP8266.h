#ifndef _ESP8266_H_
#define _ESP8266_H_

#include "sys.h"
#define ESP8266_WIFI_INFO "AT+CWJAP=\"XM14\",\"20245515\"\r\n"
#define ESP8266_ONENET_INFO "AT+CIPSTART=\"TCP\",\"mqtts.heclouds.com\",1883\r\n"
#define ESP8266RST_PORT GPIO_Pin_5
#define ESP8266RST_GPIO GPIOA
#define ESP8266RST_SCK RCC_APB2Periph_GPIOA

#define REV_OK 0   // ������ɱ�־
#define REV_WAIT 1 // ����δ��ɱ�־
// esp8266 ʹ�õ��Ǵ�����
// ����ʹ��sendstring#define esp8266_printf u3_printf
#define esp8266_RX_STA USART3_RX_STA
#define esp8266_RX_BUF USART3_RX_BUF
#define esp8266_uart USART3

void ESP8266_Clear(void);

void ESP8266_SendData(unsigned char *data, unsigned short len);
void OneNet_Reply(u32 msg_id);
unsigned char *ESP8266_GetIPD(unsigned short timeOut);
void ESP8266_disable(void);
void ESP8266_enable(void);
void esp_init(void);
_Bool ESP8266_SendCmd(char *cmd, char *res);
extern u8 disconnet;
extern u8 recv[1024];
extern u16 recv_len;

#endif
