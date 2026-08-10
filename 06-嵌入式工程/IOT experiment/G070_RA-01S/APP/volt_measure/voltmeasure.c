//
// Created by 19y on 2026/4/28.
//

#include "voltmeasure.h"
uint16_t volt_temp[10];
void voltmeasure_init(void)
{
    HAL_ADCEx_Calibration_Start(&hadc1);
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)volt_temp, sizeof(volt_temp)/sizeof(volt_temp[0]));
}

uint16_t voltmeasure_get_advalue(void)
{
    uint32_t ADValue=0;
    for (uint8_t i = 0;i<sizeof(volt_temp)/sizeof(volt_temp[0]);i++)
    {
        ADValue+=volt_temp[i];
    }
    return ADValue/10;
}
float voltmeasure_get_voltage(void)
{
    uint32_t ADValue=0;
    for (uint8_t i = 0;i<sizeof(volt_temp)/sizeof(volt_temp[0]);i++)
    {
        ADValue+=volt_temp[i];
    }
    return ADValue/10.0f/4095.0f*3.3f;
}