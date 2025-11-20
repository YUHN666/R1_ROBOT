#include "Analog.h"







// 含在摇杆的初始化函数里
// 收集数据，填充滤波器
void Analog_Init(Analog_t* analog)
{

    for(int i = 0;i < ANALOG_FILTER_SIZE;i++)
    {
        (void)Analog_GetFilteredValue(analog);
    }
}
// 摇杆用-----------------------------------------------------------------
// 取原始值（读取引脚的模拟电压值）
uint16_t Analog_GetOriginalValue(Analog_t* analog)
{
    if(analog->rank == 0)
    {
        return 0;
    }
	// 处理 DCatch缓存
	SCB_InvalidateDCache_by_Addr(analog->adc_bsp->conv_buf, sizeof(analog->adc_bsp->conv_buf));
    // 与两个摇杆总共四个分量是一一对应的
    return analog->adc_bsp->conv_buf[analog->rank - 1];
}
// 递推均值滤波器（初始化的时候填满）
uint16_t Analog_GetFilteredValue(Analog_t* analog)
{
    uint16_t new_val = Analog_GetOriginalValue(analog);                     //获得新的原始ADC转换值
    analog->filter.sum -= analog->filter.mean_buf[analog->filter.index];    //减去旧的滤波值
    analog->filter.sum += new_val;                                          //加上新的滤波值
    analog->filter.mean_buf[analog->filter.index] = new_val;                //更新滤波缓存
    analog->filter.index = (analog->filter.index + 1) % ANALOG_FILTER_SIZE; //更新滤波缓存索引
    analog->filter.mean = analog->filter.sum / ANALOG_FILTER_SIZE;          //计算新的滤波值
    return analog->filter.mean;
}
// 电池用-----------------------------------------------------------------
// 计算原始映射值
float Analog_GetOriginalMappedValue(Analog_t* analog)
{
    return analog->map_k * Analog_GetOriginalValue(analog) + analog->map_b;
}
// 计算滤波映射值
float Analog_GetFilteredMappedValue(Analog_t* analog)
{
    return analog->map_k * Analog_GetFilteredValue(analog) + analog->map_b;
}


