#ifndef _OLED_UI_H_
#define _OLED_UI_H_



#include "u8g2.h"
#include "i2c.h"
#include "delay.h"
#include "OLED_task.h"
#include "Config.h"
// 函数指针
typedef void (*OledUI_Pages_t)(void);


// OLED
typedef struct
{
    u8g2_t u8g2;              // u8g2结构体
    OledUI_Pages_t pages[32]; // 页面函数指针组 “页面 ”就是函数
    uint8_t page_count;       // 页面数量
    uint8_t page_current;     // 当前页面
} OledUI_t;

// 显示---------------------------------------------------------------------------
typedef struct 
{
    OledUI_t oled_ui;
    enum
    {
        Control_Page,
        Adjust_Page,
        KFS_Page,
    } page;
    // 页面顺序要跟添加顺序一致
} OledDisplay_t;

// 初始化函数-------------------------------------------------------------------------
void OledUI_Init(OledUI_t* oled_ui);

// 用于初始化
uint8_t u8g2_gpio_and_delay_stm32(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg,
                            U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr);
// 显示初始化
void OledDisplay_Init(void);
// 显示函数
void OledUI_Display(OledUI_t *oled_ui);
// 换下一页
void OledUI_SetPageNext(OledUI_t* oled_ui);
// 换上一页
void OledUI_SetPagePrev(OledUI_t* oled_ui);
// 换成指定页
void OledUI_SetPage(OledUI_t* oled_ui, uint8_t page);
// 获取当前页
uint8_t OledUI_GetPage(OledUI_t* oled_ui);
// 增加页面
void OledUI_AddPage(OledUI_t* oled_ui, OledUI_Pages_t newpage);
// 去除页面
void OledUI_RemovePage(OledUI_t* oled_ui, OledUI_Pages_t oldpage);
// 页面实时更新
void OledDisplay_Process(void);
// 页面群-----------------------------------------------------------------------------
void OledDisplay_Control_Page(void);
void OledDisplay_Adjust_Page(void);


// 页面实时更新
void OledDisplay_Process(void);

#endif







