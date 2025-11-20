#include "Battery.h"

void Battery_Init(Battery_t *battery)
{
    Analog_Init(&battery->analog_voltage);
}
uint8_t Battery_GetEnergy(Battery_t *battery)
{
    return battery->energy;
}
float Battery_GetVoltage(Battery_t *battery)
{
    return battery->battery_voltage;
}
void Battery_Process(Battery_t *battery)
{
#define volt battery->battery_voltage
#define threshold battery->threshold
#define energy battery->energy
    volt = Analog_GetFilteredMappedValue(&battery->analog_voltage);
    switch (energy)
    {
    case Battery_Energy_0pct:
        if (volt > threshold._20pct)
            energy = Battery_Energy_20pct;
        break;
    case Battery_Energy_20pct:
        if (volt > threshold._40pct)
            energy = Battery_Energy_40pct;
        else if (volt < threshold._0pct)
            energy = Battery_Energy_0pct;
        break;
    case Battery_Energy_40pct:
        if (volt > threshold._60pct)
            energy = Battery_Energy_60pct;
        else if (volt < threshold._20pct)
            energy = Battery_Energy_20pct;
        break;
    case Battery_Energy_60pct:
        if (volt > threshold._80pct)
            energy = Battery_Energy_80pct;
        else if (volt < threshold._40pct)
            energy = Battery_Energy_40pct;
        break;
    case Battery_Energy_80pct:
        if (volt > threshold._100pct)
            energy = Battery_Energy_100pct;
        else if(volt < threshold._60pct)
            energy = Battery_Energy_60pct;
        break;
    case Battery_Energy_100pct:
        if (volt < threshold._80pct)
            energy = Battery_Energy_80pct;
        break;
    default:
        break;
    }
#undef volt
#undef threshold
#undef energy
}


