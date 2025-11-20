#ifndef BATTERY_BATTERY_H_
#define BATTERY_BATTERY_H_
#include "main.h"
#include "Analog.h"


typedef enum 
{
    Battery_Energy_0pct = 0,
    Battery_Energy_20pct = 1,
    Battery_Energy_40pct = 2,
    Battery_Energy_60pct = 3,
    Battery_Energy_80pct = 4,
    Battery_Energy_100pct = 5,
} Battery_Energy_t;

typedef struct
{
    //电池电量
    Battery_Energy_t energy;
    Analog_t analog_voltage;
    float battery_voltage;
    struct 
    {
        float _0pct;
        float _20pct;
        float _40pct;
        float _60pct;
        float _80pct;
        float _100pct;
    } threshold;
} Battery_t;

void Battery_Init(Battery_t *battery);
float Battery_GetVoltage(Battery_t *battery);
void Battery_Process(Battery_t *battery);

#endif // BATTERY_BATTERY_H_