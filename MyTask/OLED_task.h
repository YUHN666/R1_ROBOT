#ifndef __OLED_TASK_H__
#define __OLED_TASK_H__

#include "main.h"
#include "cmsis_os.h"
#include "Oled_UI.h"
#include "u8g2.h"
#include "delay.h"
#include "string.h"
#include "stdio.h"
#include "Rocker.h"
#include "Swtimer.h"


// YPB -----------------------------------------------------------
#define Kxp 0x01
#define Kxd 0x02
#define Kyp 0x03
#define Kyd 0x04
#define Kop 0x05
#define Kod 0x06
#define Vs 0x07
#define Ds 0x08
#define T1 0x09
#define T2 0x0A
#define T3 0x0B
#define T4 0x0C
// ZQH -----------------------------------------------------------
#define Kf 0x80
#define Vmax 0x81
#define Amax 0x82
#define Kp1 0x83
#define Ki1 0x84
#define Kd1 0x85
#define Kp2 0x86
#define Ki2 0x87
#define Kd2 0x88
#define Kp3 0x89
#define Kd3 0x8A
#define Imax1 0x8B
#define Imax2 0x8C
#define Imax3 0x8D
#define Omax 0x8E
#define Wmax_ 0x8F
#define Wamax_ 0x90



// 调参解算
void YPB_Parameter(void);
// 预期变化计算
float Calculate(void);
// 梅花桩上 KFS放置情况
char* Judgment(uint8_t index);
char* JudgmentShort(uint8_t index);
// 遥控界面
void OledDisplay_Control_Page(void);
// 调参界面
void OledDisplay_Adjust_Page(void);
// KFS放置界面
void OledDisplay_KFS_Page(void);
// 地图加载
void ForestMap(void);
#endif
