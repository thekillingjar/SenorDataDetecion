#include "Usart.h"
#include "string.h"
#include "stm32f10x.h"
#include <stdarg.h>
#include <stdio.h>
#include "Mqtt.h"
#include "OLED.h"
#include "ESP8266.h"
#include "stdarg.h" 
#include "BEEP.h"
#include "Timer.h"
u8 USART1_TX_BUF[1024];



u8 USART3_RX_BUF[USART_REC_LEN]; 
u8 USART3_TX_BUF[USART_SED_LEN];   
u16 USART3_RX_STA=0;       
uint8_t RX3_count = 0;



//¶¨Ê±Æ÷4ÖÐ¶Ï·þÎñ³ÌÐò		    
 void TIM4_IRQHandler(void)
{ 	
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)//ÊÇ¸üÐÂÖÐ¶Ï
	{	 			   
		USART3_RX_STA|=1<<15;	//±ê¼Ç½ÓÊÕÍê³É
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );  //Çå³ýTIMx¸üÐÂÖÐ¶Ï±êÖ¾
		TIM4_Set(0);			//¹Ø±ÕTIM4  
//		u1_printf("%s", recv);
//		sprintf(recv, "");
//		recv_len = 0;
	}	    
}
//ÉèÖÃTIM4µÄ¿ª¹Ø
//sta:0£¬¹Ø±Õ;1,¿ªÆô;
static void TIM4_Set(u8 sta)
{
	if(sta)
	{
       
		TIM_SetCounter(TIM4,0);//¼ÆÊýÆ÷Çå¿Õ
		TIM_Cmd(TIM4, ENABLE);  //Ê¹ÄÜTIMx	
	}else TIM_Cmd(TIM4, DISABLE);//¹Ø±Õ¶¨Ê±Æ÷4	   
}
//Í¨ÓÃ¶¨Ê±Æ÷ÖÐ¶Ï³õÊ¼»¯
//ÕâÀïÊ¼ÖÕÑ¡ÔñÎªAPB1µÄ2±¶£¬¶øAPB1Îª36M
//arr£º×Ô¶¯ÖØ×°Öµ¡£
//psc£ºÊ±ÖÓÔ¤·ÖÆµÊý		 
static void TIM4_Init(u16 arr,u16 psc)
{	
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //Ê±ÖÓÊ¹ÄÜ//TIM4Ê±ÖÓÊ¹ÄÜ    
	
	//¶¨Ê±Æ÷TIM4³õÊ¼»¯
	TIM_TimeBaseStructure.TIM_Period = arr; //ÉèÖÃÔÚÏÂÒ»¸ö¸üÐÂÊÂ¼þ×°Èë»î¶¯µÄ×Ô¶¯ÖØ×°ÔØ¼Ä´æÆ÷ÖÜÆÚµÄÖµ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //ÉèÖÃÓÃÀ´×÷ÎªTIMxÊ±ÖÓÆµÂÊ³ýÊýµÄÔ¤·ÖÆµÖµ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //ÉèÖÃÊ±ÖÓ·Ö¸î:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIMÏòÉÏ¼ÆÊýÄ£Ê½
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //¸ù¾ÝÖ¸¶¨µÄ²ÎÊý³õÊ¼»¯TIMxµÄÊ±¼ä»ùÊýµ¥Î»
 
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //Ê¹ÄÜÖ¸¶¨µÄTIM4ÖÐ¶Ï,ÔÊÐí¸üÐÂÖÐ¶Ï

	 	  
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//ÇÀÕ¼ÓÅÏÈ¼¶3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//×ÓÓÅÏÈ¼¶3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQÍ¨µÀÊ¹ÄÜ
	NVIC_Init(&NVIC_InitStructure);	//¸ù¾ÝÖ¸¶¨µÄ²ÎÊý³õÊ¼»¯VIC¼Ä´æÆ÷
	
}


void upper_USART_init(u32 bound)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

  // USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; // PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  // USART1_RX	  GPIOA.10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; // PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  // Usart1 NVIC
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  USART_InitStructure.USART_BaudRate = bound;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);			//ÅäÖÃ½ÓÊÕÖÐ¶Ï
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);			//ÅäÖÃ×ÜÏß¿ÕÏÐÖÐ¶Ï
  USART_Init(USART1, &USART_InitStructure);
  USART_Cmd(USART1, ENABLE);
}
void ESP8266_Init(u32 bound)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;

		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
		// USART1_TX   GPIOA.9
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; // PA.9
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		// USART1_RX	  GPIOA.10
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; // PA10
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		// Usart1 NVIC
		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);

		USART_InitStructure.USART_BaudRate = bound;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_No;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

		USART_Init(USART3, &USART_InitStructure);
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);			//ÅäÖÃ½ÓÊÕÖÐ¶Ï

		USART_Cmd(USART3, ENABLE);
		TIM4_Init(3999,7199);		//10msÖÐ¶Ï
		USART3_RX_STA=0;		//ÇåÁã
		TIM4_Set(0);			//¹Ø±Õ¶¨Ê±Æ÷4
		esp_init();
}
void USART1_IRQHandler(void)
{
  u8 s;
  if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
  {
    s = USART1->DR;
    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
  }
}
u8 search_flag = 0;
u16 ipd_len = 0;
void IPD_Search(u8 c) {
		switch(search_flag) {
			case 0:
				if (c == '+') search_flag++;
				break;
			case 1:
				if (c == 'I') search_flag++;
				else if (c == '+') search_flag = 1;
				else search_flag = 0;
				break;
			case 2:
				if (c == 'P') search_flag++;
				else if (c == '+') search_flag = 1;
				else search_flag = 0;
				break;
			case 3:
				if (c == 'D') search_flag++;
				else if (c == '+') search_flag = 1;
				else search_flag = 0;
				break;
			case 4:
				if (c == ',') search_flag++;
				else if (c == '+') search_flag = 1;
				else search_flag = 0;
				break;
			case 5:
				if (c >= '0' && c <= '9') {
					ipd_len = ipd_len * 10 + c - '0';
				} else {
					if (ipd_len > 0) {
						recv_len = 0;
						search_flag++;				
					}
				}
				
				break;
			case 6:
				if (ipd_len == 0) {
					count_flag = 1;
				} else {
					recv[recv_len++] = c;
					ipd_len--;
				}
				break;
				
		}
}
void USART3_IRQHandler(void)
{
	u8 c;	    
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)//½ÓÊÕµ½Êý¾Ý
	{	 
 
		c =USART3->DR;
		IPD_Search(c);
		USART_SendData(USART1, c);

		if(USART3_RX_STA<USART_REC_LEN)		//»¹¿ÉÒÔ½ÓÊÕÊý¾Ý
		{
			TIM_SetCounter(TIM4,0);//¼ÆÊýÆ÷Çå¿Õ        				 
			if(USART3_RX_STA==0) {
				TIM4_Set(1);	 	//Ê¹ÄÜ¶¨Ê±Æ÷4µÄÖÐ¶Ï 
			}
			USART3_RX_BUF[USART3_RX_STA++]=c;		//¼ÇÂ¼½ÓÊÕµ½µÄÖµ	 
		}else 
		{
			USART3_RX_STA|=1<<15;					//Ç¿ÖÆ±ê¼Ç½ÓÊÕÍê³É
		} 
				USART_ClearITPendingBit(USART3, USART_IT_RXNE);
	}  											 
}   
	
void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len)
{

	unsigned short count = 0;
	
	for(; count < len; count++)
	{
		USART_SendData(USARTx, *str++);									//·¢ËÍÊý¾Ý
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);		//µÈ´ý·¢ËÍÍê³É
	}

}

void u1_printf(char *fmt, ...)
{
  u16 i, j;
  va_list ap;
  va_start(ap, fmt);
  vsprintf((char *)USART1_TX_BUF, fmt, ap);
  va_end(ap);
  i = strlen((const char *)USART1_TX_BUF); // 此次发送数据的长度
  for (j = 0; j < i; j++)                  // 循环发送数据
  {
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
      ; // 循环发送,直到发送完毕
    USART_SendData(USART1, USART1_TX_BUF[j]);
  }
}

void u3_printf(char *fmt, ...)
{
  u16 i, j;
  va_list ap;
  va_start(ap, fmt);
  vsprintf((char *)USART3_TX_BUF, fmt, ap);
  va_end(ap);
  i = strlen((const char *)USART3_TX_BUF); // 此次发送数据的长度
  for (j = 0; j < i; j++)                  // 循环发送数据
  {
    while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
      ; // 循环发送,直到发送完毕
    USART_SendData(USART3, USART3_TX_BUF[j]);
  }
}