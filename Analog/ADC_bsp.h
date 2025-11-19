
// ADC板级支持包
#ifndef _ADC_BSP_H
#define _ADC_BSP_H
#include "main.h"
#include "adc.h"

// 定义在 Analog_t里
typedef struct
{
    ADC_HandleTypeDef *hadc; // ADC句柄
    uint16_t conv_buf[8];    // DMA缓存
    // number of convertion
    // DMA的采样次数，两个摇杆四个分量，则 noc至少设置为 4
    // 而且必须和 CubeMX上的 ADC通道数一样
    uint8_t noc; 
} AdcBsp_t;

void AdcBsp_Init(AdcBsp_t* adc_bsp);
#endif