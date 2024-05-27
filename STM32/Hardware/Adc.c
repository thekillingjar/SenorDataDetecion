#include "Adc.h"
#include "delay.h"
#include "math.h"
uint16_t AD_Value[1];
uint16_t average_Value[2];
void AD_Init(void)
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  RCC_ADCCLKConfig(RCC_PCLK2_Div6);

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_55Cycles5);

  ADC_InitTypeDef ADC_InitStructure;
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_NbrOfChannel = 1;
  ADC_Init(ADC1, &ADC_InitStructure);

  DMA_InitTypeDef DMA_InitStructure;
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; // 16Î»
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)AD_Value;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = 1;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; // Ñ­»·
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;    // Ã»ÓÐÄÚ´æµ½ÄÚ´æµÄÔËÊä,Ñ¡ÔñÓ²¼þ´¥·¢
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);

  DMA_Cmd(DMA1_Channel1, ENABLE);
  ADC_DMACmd(ADC1, ENABLE);
  ADC_Cmd(ADC1, ENABLE);

  ADC_ResetCalibration(ADC1);
  while (ADC_GetResetCalibrationStatus(ADC1) == SET)
    ;
  ADC_StartCalibration(ADC1);
  while (ADC_GetCalibrationStatus(ADC1) == SET)
    ;

  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

void ADC1_Average_Data()
{
  u32 temp_val[1] = {0};
  u8 t;
  for (t = 0; t < 5; t++) // #define SMOG_READ_TIMES	10	定义烟雾传感器读取次数,读这么多次,然后取平均值

  {
    temp_val[0] += AD_Value[0]; // 读取ADC值
    delay_ms(5);
  }
  average_Value[0] = temp_val[0] / 5; // 得到平均值
}

// 读取MQ7传感器的电压值
float Smog_Get_Vol(void)
{
  float voltage = 0; // MQ-7传感器模块的电压输出，与一氧化碳的浓度成正比
  voltage = (3.3 / 4096.0) * (average_Value[0]);

  return voltage;
}
/*********************
// 传感器校准函数，根据当前环境PPM值与测得的RS电压值，反推出R0值。
// 在空气中运行过后测出R0为26
float MQ7_PPM_Calibration()
{
  float RS = 0;
  float R0 = 0;
  RS = (3.3f - Smog_Get_Vol()) / Smog_Get_Vol() * RL;//RL	10  // RL阻值
  R0 = RS / pow(CAL_PPM / 98.322, 1 / -1.458f);//CAL_PPM  10  // 校准环境中PPM值
  return R0;
}
**********************/

// 计算Smog_ppm
float Smog_GetPPM()
{
  float RS = (3.3f - Smog_Get_Vol()) / Smog_Get_Vol() * RL;
  float ppm = 98.322f * pow(RS / R0, -1.458f);
  return ppm;
}