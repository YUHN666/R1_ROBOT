#ifndef _CONFIG_H_
#define _CONFIG_H_
#include "main.h"
#include "FunctionKey.h"
#include "Rocker.h"
#include "SwTimer.h"
#include "Oled_UI.h"
#include "zigbee.h"
#include "usart.h"
#include "delay.h"
#include "FUNCTIONKEY_task.h"
#include "COMMS_task.h"
#include "Battery.h"




// 模式
#define ADJUST 0xAA
#define CONTROL 0xCC
#define RED_ZONE 0x00
#define BLUE_ZONE 0x01
#define KEY 0x12
// 倍率
#define X1 0x01
#define X10 0x11
#define X100 0x12
#define X0_1 0xF1
#define X0_01 0xF2


// KFS--------------------------------------------------------------------
#define EMPTY 0x00
#define R1KFS 0x01
#define R2KFS 0x02
#define FAULSE 0x03

#define KEY_NUM 15
// 按键值
#define PC15 0x15
#define PC14 0x14
#define PF2 0x02
#define PF0 0x00
#define PF4 0x04
#define PF3 0x03
#define PA11 0x11
#define PA10 0x10
#define PA9 0x09
#define PA8 0xA8
#define PG7 0x07
#define PG8 0x08
#define PC12 0x12
#define PC11 0xC1
#define PE1 0xE1


// 地址
#define R1_GAMEPAD 0x1921
#define R2_GAMEPAD 0x1927
#define R1_ROBOT 0x1945
#define R2_ROBOT 0x1949

void Config_Init(void);



// 中断回调
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);




#endif