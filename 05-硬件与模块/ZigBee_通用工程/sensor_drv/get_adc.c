/**************************************************************************************************
  Filename:      sh10.c

  Description:    sh10 driver 
                  
**************************************************************************************************/

/******************************************************************************
 * INCLUDES
 */
#include "ioCC2530.h"
#include "hal_defs.h"
#include "get_adc.h"


/******************************************************************************
 * GLOBAL VARIABLES
 */

void hal_adc_Init(void)
{
	APCFG  |=1;   
        P0SEL  |= (1 << (0));	
        P0DIR  &= ~(1 << (0));	
    
}
/******************************************************************************
 * 名称       get_adc
 * 功能       读取A/D值
 * 入口参数   无
 * 出口参数   16位电压值，分辨率为10mV, 如0x0102表示2.58V
 *****************************************************************************/
float get_adc(void)
{
  uint32 value;
  hal_adc_Init(); // ADC初始化
  ADCIF = 0;   //清ADC 中断标志
  //采用基准电压avdd5:3.3V，通道0，启动AD转化
  ADCCON3 = (0x80 | 0x10 | 0x00);
  while ( !ADCIF )
  {
    ;  //等待AD转化结束
  }
   value = ADCH;
   value = value<< 8;
   value |= ADCL;
   if(value&0x8000)
       value=0;
  // AD值转化成电压值
  // 0 表示 0V ，32768 表示 3.3V
  // 电压值 = (value*3.3)/32768 （V)
  // 返回分辨率为0.01V的电压值
  return (value*3.3)/32768;
}


void hal_adc4CH_Init(void)
{
  APCFG  |= (1 << ACH1) | (1 << ACH2)| (1 << ACH3)| (1 << ACH4);   
  P0SEL  |= (1 << ACH1) | (1 << ACH2)| (1 << ACH3)| (1 << ACH4);	
  P0DIR  &= ~((1 << ACH1) | (1 << ACH2)| (1 << ACH3)| (1 << ACH4));	
}


float get_adc4CH(uint8 ch)
{
    int32 value;
    hal_adc4CH_Init();
    ADCIF = 0;   //清ADC 中断标志
   //采用基准电压avdd5:3.3V，通道ch，启动AD转化
   ADCCON3 = (0x80 | 0x10 | (ch&0x0f) );
   while ( !ADCIF )
   {
       ;  //等待AD转化结束
   }
   value = ADCH;
   value = value<< 8;
   value |= ADCL;
   if(value&0x8000)
       value=0;
   // AD值转化成电压值
   // 0 表示 0V ，32768 表示 3.3V
   // 电压值 = (value*3.3)/32768 （V)
   // 返回分辨率为0.01V的电压值
   return (value*3.3)/32768;
}

















