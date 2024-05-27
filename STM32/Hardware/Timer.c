#include "Timer.h"
#include "stm32f10x.h"
#include "ESP8266.h"
#include "Usart.h"
#include "BEEP.h"
u8 timeout1 = 0;
u16 t = 0;
u8 count_flag;
u16 count;
u16 alarm_stop_count = 0;
u16 timeout2 = 0;
void Timer_Init(u16 arr, u16 psc)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  TIM_TimeBaseStructure.TIM_Period = arr;
  TIM_TimeBaseStructure.TIM_Prescaler = psc;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  TIM_ClearFlag(TIM2, TIM_FLAG_Update);
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_Init(&NVIC_InitStructure);

  TIM_Cmd(TIM2, ENABLE);
}

void TIM2_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
  {
    if (timeout1 == 0)
    {
      t++;
      if (t >= 20)
      {
        timeout1 = 1;
        t = 0;
      }
    }
    timeout2++;
    if (timeout2 == 20)
      timeout2 = 0;
    if (count_flag == 1)
    {
      count++;
      if (count >= 20)
      {
        count_flag = 0;
        count = 0;
        recv_len = 0;
        search_flag = 0;
      }
    }
    if (alarm_stop == 1)
    {
      alarm_stop_count++;
    }
    else
      alarm_stop_count = 0;
    if (alarm_stop_count >= 1200)
    {
      alarm_stop = 0;
      alarm_stop_count = 0;
    }
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update); // 清除中断标志位
  }
}
