#ifndef __USART_H
#define __USART_H

#include "stdio.h"
#include "sys.h"


void upper_USART_init(u32 bound);
void u1_printf(char *fmt, ...);

#define USART_REC_LEN  			1024  	
#define USART_SED_LEN 			200
extern u8  USART3_RX_BUF[USART_REC_LEN];
extern u8  USART3_TX_BUF[USART_SED_LEN];  
extern u16 USART3_RX_STA; 
extern u8 RX3_count;      
void TIM4_Set(u8 sta);
void TIM4_Init(u16 arr,u16 psc);
void ESP8266_Init(u32 bound);
void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len);
void u3_printf(char *fmt, ...);
void IPD_Search(u8 c);
extern u8 search_flag;
extern u16 ipd_len;
#endif
