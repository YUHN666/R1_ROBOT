#ifndef _ROCKER_H_
#define _ROCKER_H_




#include "main.h"
#include "Analog.h"


typedef struct 
{
    struct 
    {
        Analog_t analog;
        uint8_t sign; // 配置正负号
        float offset; // 偏移量
        float map_positive;
        float map_negative;
        int8_t val;
    } x, y; // 定义了两个 Analog
    struct
    {
        int8_t x;
        int8_t y;
    } state;  
	float deadzone;	
} Rocker_t;

void Rocker_Init(Rocker_t *rocker);
void Rocker_Process(Rocker_t *rocker);
// 将过滤器的值映射到摇杆
void _Rocker_AnalogFilterMapRefresh(int8_t *target, int measure);
// 获取过滤器的模拟位置
void _Rocker_AnalogValRefresh(Rocker_t *rocker);
#endif






