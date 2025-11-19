#include "ADC_bsp.h"
// ADC板级支持包初始化





void AdcBsp_Init(AdcBsp_t* adc_bsp)
{
    // 开启一个基于 DMA 的模数转换
    HAL_ADC_Start_DMA(adc_bsp->hadc, (void*)adc_bsp->conv_buf, adc_bsp->noc);
}

