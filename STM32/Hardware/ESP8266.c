#include "ESP8266.h"
#include "Usart.h"
#include "delay.h"
#include <stdarg.h>
#include <stdarg.h>
#include <stdio.h>
// C¿â
#include <string.h>
#include <stdio.h>

u8 disconnet = 0;
u8 recv[1024];
u16 recv_len = 0;

void ESP8266_Clear(void)
{

	memset(esp8266_RX_BUF, 0, sizeof(esp8266_RX_BUF));
	esp8266_RX_STA = 0;
}

//==========================================================
//	º¯ÊýÃû³Æ£º	ESP8266_WaitRecive
//
//	º¯Êý¹¦ÄÜ£º	µÈ´ý½ÓÊÕÍê³É
//
//	Èë¿Ú²ÎÊý£º	ÎÞ
//
//	·µ»Ø²ÎÊý£º	REV_OK-½ÓÊÕÍê³É		REV_WAIT-½ÓÊÕ³¬Ê±Î´Íê³É
//
//	ËµÃ÷£º		Ñ­»·µ÷ÓÃ¼ì²âÊÇ·ñ½ÓÊÕÍê³É
//==========================================================
_Bool ESP8266_WaitRecive(void)
{
	if (esp8266_RX_STA & 0x8000)
	{
		esp8266_RX_STA = 0;
		return REV_OK;
	}
	return REV_WAIT; // ·µ»Ø½ÓÊÕÎ´Íê³É±êÖ¾
}

//==========================================================
//	º¯ÊýÃû³Æ£º	ESP8266_SendCmd
//
//	º¯Êý¹¦ÄÜ£º	·¢ËÍÃüÁî
//
//	Èë¿Ú²ÎÊý£º	cmd£ºÃüÁî
//				res£ºÐèÒª¼ì²éµÄ·µ»ØÖ¸Áî
//
//	·µ»Ø²ÎÊý£º	0-³É¹¦	1-Ê§°Ü
//
//	ËµÃ÷£º
//==========================================================
_Bool ESP8266_SendCmd(char *cmd, char *res)
{

	unsigned char timeOut = 200;

	Usart_SendString(esp8266_uart, (unsigned char *)cmd, strlen((const char *)cmd));
	while (timeOut--)
	{
		if (ESP8266_WaitRecive() == REV_OK) // Èç¹ûÊÕµ½Êý¾Ý
		{
			if (strstr((const char *)esp8266_RX_BUF, res) != NULL) // Èç¹û¼ìË÷µ½¹Ø¼ü´Ê
			{

				ESP8266_Clear(); // Çå¿Õ»º´æ
				return 0;
			}
			else if (strstr((const char *)esp8266_RX_BUF, "link is not valid") != NULL)
			{
				disconnet = 1;
			}
		}

		delay_ms(10);
	}
	return 1;
}

//==========================================================
//	º¯ÊýÃû³Æ£º	ESP8266_SendData
//
//	º¯Êý¹¦ÄÜ£º	·¢ËÍÊý¾Ý
//
//	Èë¿Ú²ÎÊý£º	data£ºÊý¾Ý
//				len£º³¤¶È
//
//	·µ»Ø²ÎÊý£º	ÎÞ
//
//	ËµÃ÷£º
//==========================================================
void ESP8266_SendData(unsigned char *data, unsigned short len)
{

	char cmdBuf[32];

	ESP8266_Clear(); // Çå¿Õ½ÓÊÕ»º´æ

	// ÏÈ·¢ËÍÒª·¢ËÍÊý¾ÝµÄÖ¸Áî×ö×¼±¸
	sprintf(cmdBuf, "AT+CIPSEND=%d\r\n", len); // ·¢ËÍÃüÁî
	if (!ESP8266_SendCmd(cmdBuf, ">"))				 // ÊÕµ½¡®>¡¯Ê±¿ÉÒÔ·¢ËÍÊý¾Ý
	{
		// ¼ÈÈ»×¼±¸Íê±Ï¼´¿É¿ªÊ¼·¢ËÍÊý¾Ý
		// esp8266_printf("%s",data);
		Usart_SendString(esp8266_uart, data, len); // ·¢ËÍÉè±¸Á¬½ÓÇëÇóÊý¾Ý
																							 // Usart_SendString(USART3, data, len);		//·¢ËÍÉè±¸Á¬½ÓÇëÇóÊý¾Ý
	}
}

//==========================================================
//	º¯ÊýÃû³Æ£º	ESP8266_GetIPD
//
//	º¯Êý¹¦ÄÜ£º	»ñÈ¡Æ½Ì¨·µ»ØµÄÊý¾Ý
//
//	Èë¿Ú²ÎÊý£º	µÈ´ýµÄÊ±¼ä(³ËÒÔ10ms)
//
//	·µ»Ø²ÎÊý£º	Æ½Ì¨·µ»ØµÄÔ­Ê¼Êý¾Ý
//
//	ËµÃ÷£º		²»Í¬ÍøÂçÉè±¸·µ»ØµÄ¸ñÊ½²»Í¬£¬ÐèÒªÈ¥µ÷ÊÔ
//				ÈçESP8266µÄ·µ»Ø¸ñÊ½Îª	"+IPD,x:yyy"	x´ú±íÊý¾Ý³¤¶È£¬yyyÊÇÊý¾ÝÄÚÈÝ
//==========================================================
unsigned char *ESP8266_GetIPD(unsigned short timeOut)
{

	char *ptrIPD = NULL;

	do
	{
		if (ESP8266_WaitRecive() == REV_OK) // Èç¹û½ÓÊÕÍê³É
		{
			ptrIPD = strstr((char *)esp8266_RX_BUF, "IPD,"); // ËÑË÷¡°IPD¡±Í·
			if (ptrIPD == NULL)															 // Èç¹ûÃ»ÕÒµ½£¬¿ÉÄÜÊÇIPDÍ·µÄÑÓ³Ù£¬»¹ÊÇÐèÒªµÈ´ýÒ»»á£¬µ«²»»á³¬¹ýÉè¶¨µÄÊ±¼ä
			{
				// UsartPrintf(USART_DEBUG, "\"IPD\" not found\r\n");
			}
			else
			{
				ptrIPD = strchr(ptrIPD, ':'); // ÕÒµ½':'
				if (ptrIPD != NULL)
				{
					ptrIPD++;
					return (unsigned char *)(ptrIPD);
				}
				else
					return NULL;
			}
		}

		delay_ms(5); // ÑÓÊ±µÈ´ý
	} while (timeOut--);

	return NULL; // ³¬Ê±»¹Î´ÕÒµ½£¬·µ»Ø¿ÕÖ¸Õë

	//	while (!(search_flag == 6 && ipd_len == 0)) {
	//		u1_printf("%d %d\r\n", search_flag, ipd_len);
	//		delay_ms(5);
	//		timeOut--;

	//		if (timeOut == 0) {
	//			u1_printf("wewqe\r\n");
	//			return NULL;
	//		}
	//	}
	//	u1_printf("213\r\n");
	//	recv[recv_len] = 0;
	//	recv_len = 0;
	//	search_flag = 0;
	//
	//	return recv;
}

//==========================================================
//	º¯ÊýÃû³Æ£º	ESP8266_Init
//
//	º¯Êý¹¦ÄÜ£º	³õÊ¼»¯ESP8266
//
//	Èë¿Ú²ÎÊý£º	ÎÞ
//
//	·µ»Ø²ÎÊý£º	ÎÞ
//
//	ËµÃ÷£º
//==========================================================
void esp_init(void)
{

	GPIO_InitTypeDef GPIO_Initure;

	RCC_APB2PeriphClockCmd(ESP8266RST_SCK, ENABLE);

	// ESP8266¸´Î»Òý½Å
	GPIO_Initure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Initure.GPIO_Pin = ESP8266RST_PORT; // GPIOA5-¸´Î»
	GPIO_Initure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ESP8266RST_GPIO, &GPIO_Initure);

	GPIO_WriteBit(ESP8266RST_GPIO, ESP8266RST_PORT, Bit_RESET);
	delay_ms(250);
	GPIO_WriteBit(ESP8266RST_GPIO, ESP8266RST_PORT, Bit_SET);
	delay_ms(500);

	ESP8266_Clear();

	//	LCD_ShowString(0,line_no,"1.AT",WHITE,BLUE,word_wide,0);
	//	line_no+=line_wide;
	u1_printf("AT\r\n");
	while (ESP8266_SendCmd("AT\r\n", "OK"))
		delay_ms(500);
	//	LCD_ShowString(0,line_no,"2.AT+CWMODE=1",WHITE,BLUE,word_wide,0);
	//	line_no+=line_wide;
	u1_printf("AT+CWMODE=1\r\n");
	while (ESP8266_SendCmd("AT+CWMODE=1\r\n", "OK"))
		delay_ms(500);

	//	LCD_ShowString(0,line_no,"3.AT+CWDHCP",WHITE,BLUE,word_wide,0);
	//	line_no+=line_wide;
	u1_printf("AT+CWDHCP=1,1\r\n");
	while (ESP8266_SendCmd("AT+CWDHCP=1,1\r\n", "OK"))
		delay_ms(500);

	//	LCD_ShowString(0,line_no,"4.CWJAP",WHITE,BLUE,word_wide,0);
	//	line_no+=line_wide;
	u1_printf("%s", ESP8266_WIFI_INFO);
	while (ESP8266_SendCmd(ESP8266_WIFI_INFO, "GOT IP"))
		delay_ms(500);

	//	LCD_ShowString(0,line_no,"5.CIPSTART",WHITE,BLUE,word_wide,0);

	//	line_no+=line_wide;
	u1_printf("%s", ESP8266_ONENET_INFO);
	while (ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT"))
		delay_ms(500);

	//	LCD_ShowString(0,line_no,"6.ESP8266 Init OK",WHITE,BLUE,word_wide,0);
	u1_printf("ESP8266 Init OK\r\n");
	delay_ms(500);
	//	line_no=0;
	//	LCD_Fill(0,0,160,128,BLUE);
}
