#ifndef _ANALOG_H_
#define _ANALOG_H_
#include "main.h"
#include "ADC_bsp.h"

#define ANALOG_FILTER_SIZE 60


typedef struct
{
    AdcBsp_t* adc_bsp;
    // 决定哪个摇杆的哪个分量
    uint8_t rank; 
    // 可在 CubeMX的ADC配置里查看顺序
    float map_k, map_b; // 线性映射参数，y = kx + b
    struct
    {
        uint16_t index;
        uint16_t mean_buf[ANALOG_FILTER_SIZE]; // 窗口
        int32_t sum;
        uint16_t mean; // 平均值
    } filter; // 递推均值滤波器
} Analog_t; // 定义在摇杆里

void Analog_Init(Analog_t* analog);
uint16_t Analog_GetOriginalValue(Analog_t* analog);
uint16_t Analog_GetFilteredValue(Analog_t* analog);
float Analog_GetOriginalMappedValue(Analog_t* analog);
float Analog_GetFilteredMappedValue(Analog_t* analog);


#endif