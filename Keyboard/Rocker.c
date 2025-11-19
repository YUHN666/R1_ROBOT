#include "Rocker.h"
#include "math_bsp.h"
#include "Analog.h"

extern AdcBsp_t adcBsp_adc1;
extern AdcBsp_t adcBsp_adc2;
extern Rocker_t LeftRocker;
extern Rocker_t RightRocker;





int prev_x_val;
int prev_y_val;


// 将过滤器的值映射到摇杆
void _Rocker_AnalogFilterMapRefresh(int8_t *target, int measure)
{
    // 平滑的将-2048~2047的数据映射到-128~127的读取数据
    int32_t val = *target;
    if (measure > (val + 1) * 16) val++;
    else if (measure < (val - 1) * 16) val--;
    if (val > 127) val = 127;
    if (val < -128) val = -128;
    *target = val;
}
// 获取过滤器的模拟位置
void _Rocker_AnalogValRefresh(Rocker_t *rocker)
{
    // 读摇杆的模拟值
    prev_x_val = Analog_GetFilteredValue(&rocker->x.analog) - 2048;
    prev_y_val = Analog_GetFilteredValue(&rocker->y.analog) - 2048;


    // 在映射前实现offset偏移与正负向补偿系数,增高精确度
    if(prev_x_val * (rocker->x.sign ? 1.0 : -1.0) >= 0)
    {
        _Rocker_AnalogFilterMapRefresh(&rocker->x.val, 
        rocker->x.offset * (rocker->x.sign ? 1.0 : -1.0) + 
        prev_x_val * (rocker->x.sign ? 1.0 : -1.0) * rocker->x.map_positive);
    }
    else
    {
        _Rocker_AnalogFilterMapRefresh(&rocker->x.val, 
        rocker->x.offset * (rocker->x.sign ? 1.0 : -1.0) + 
        prev_x_val * (rocker->x.sign ? 1.0 : -1.0) * rocker->x.map_negative);
    }
    

    if(prev_y_val * (rocker->y.sign ? 1.0 : -1.0) >= 0)
    {
        _Rocker_AnalogFilterMapRefresh(&rocker->y.val, 
        rocker->y.offset * (rocker->y.sign ? 1.0 : -1.0) + 
        prev_y_val * (rocker->y.sign ? 1.0 : -1.0) * rocker->y.map_positive);
    }
    else
    {
        _Rocker_AnalogFilterMapRefresh(&rocker->y.val, 
        rocker->y.offset * (rocker->y.sign ? 1.0 : -1.0) + 
        prev_y_val * (rocker->y.sign ? 1.0 : -1.0) * rocker->y.map_negative);
    }
    

    float x = rocker->x.val;
    float y = rocker->y.val;

    if (pow(x, 2) + pow(y, 2) < pow(rocker->deadzone, 2))
    {
        x = 0.0f;
        y = 0.0f;
    }
    // else
    // {
    //     float theta = atan2(y, x);
    //     x = (x - rocker->deadzone * cos(theta)) * 128.0f / (128.0f - rocker->deadzone);
    //     y = (y - rocker->deadzone * sin(theta)) * 128.0f / (128.0f - rocker->deadzone);
    // }

    // if (x > 127.0f) x = 127.0f;
    // if (x < -128.0f) x = -128.0f;
    // if (y > 127.0f) y = 127.0f;
    // if (y < -128.0f) y = -128.0f;

    rocker->state.x = (int8_t)x;
    rocker->state.y = (int8_t)y;
}
void Rocker_Init(Rocker_t *rocker)
{
    Analog_Init(&rocker->x.analog);
    Analog_Init(&rocker->y.analog);
}
// 更新摇杆数值
void Rocker_Process(Rocker_t *rocker)
{
    _Rocker_AnalogValRefresh(rocker);
}
