/*************************************************************************************************
  Filename:      adc.h
 
 *****/


#ifndef _GET_ADC_H_
#define _GET_ADC_H_

#define ACH1    0
#define ACH2    4
#define ACH3    5
#define ACH4    6
extern void hal_adc_Init(void); 
extern float get_adc(void);
extern void hal_adc4CH_Init();
extern float get_adc4CH(uint8 ch);
#endif


