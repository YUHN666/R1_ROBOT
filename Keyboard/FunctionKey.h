#ifndef _FUNCTIONKEY_H_
#define _FUNCTIONKEY_H_
#include "main.h"
#include "SwTimer.h"







#define PULL_UP 0
#define PULL_DOWN 1

// 按键引脚
typedef struct
{
    uint16_t pin;
    GPIO_TypeDef* port;
    // is_inversed = 0时，按键按下时为低电平
    // is_inversed = 1时，按键按下时为高电平
    uint8_t is_inversed;    
} FunctionKey_Gpio_t;

// 按击类型
typedef struct
{
    uint8_t click : 1;            // :1表示只占 1 位
    uint8_t multi : 1;            // :1表示只占 1 位
    uint8_t longpress : 1;        // :1表示只占 1 位
    uint8_t longpress_repeat : 1; // :1表示只占 1 位
} FunctionKey_Activity_t;

// 击发事件
typedef struct
{
    void (*click)(void);
    void (*multi)(uint32_t);
    void (*longpress)(uint32_t);
} FunctionKey_Event_t;
 
// 时间参数
typedef struct
{
    uint32_t polling;       //轮询间隔
    uint32_t debounce;      //消抖时间
    uint32_t reset;         //复位时间
    uint32_t long_wait;     //长按等待时间
    uint32_t long_ongoing;  //长按轮询触发间隔
    uint32_t multi_wait;    //多击等待时间
} FunctionKey_Time_t;

// 按键状态
typedef struct
{
	SwTimer_t timer;
    enum _FunctionKey_State_t
    {
        _FunctionKey_State_Reset,
        _FunctionKey_State_Debounce,
        _FunctionKey_State_LongWaiting,
        _FunctionKey_State_LongOngoing,
        _FunctionKey_State_MultiWaiting,
        _FunctionKey_State_MultiCheckRelease,
    } state;
    uint8_t multi1_cnt; // 单击总次数(multi = 1的次数)
    uint8_t multi2_cnt; // 单击总次数(multi = 1的次数)
    uint32_t longpress_cnt; // 长按次数
    uint32_t multi_cnt; // 多击次数
    uint32_t curr_time; // 当前参考时间
    uint8_t exti_sta; // 中断许可，1表示允许中断
    uint8_t last_check;
    uint8_t last_btn_sta;   //按键状态，0代表按下，1代表松开
} FunctionKey_Temp_t;

// 完整按键
typedef struct
{
    FunctionKey_Gpio_t gpioO;
    FunctionKey_Gpio_t gpioI;
    FunctionKey_Activity_t activity;
    FunctionKey_Event_t event;
    FunctionKey_Time_t time;
    uint8_t state;
    FunctionKey_Temp_t _temp;
} FunctionKey_t;

// 任务函数数组
typedef void (*functionket_click_t)(void);
typedef void (*functionket_multi_t)(uint32_t);

// 初始化
#define Defalt_FunctionKey_Activity (FunctionKey_Activity_t){ \
    .click = 1,           /* 使具有单击功能 */                \
    .multi = 1,           /* 使具有多击功能 */                \
    .longpress = 1,       /* 使具有长击功能 */                \
    .longpress_repeat = 1 /* 使具有重复长按功能 */            \
}
#define Defalt_FunctionKey_Time (FunctionKey_Time_t) {              \
    .polling = 5,                                                   \
    .debounce = 10,                                                 \
    .reset = 15,                                                    \
    .long_wait = 500,                                               \
    .long_ongoing = 100,                                            \
    .multi_wait = 100                                               \
}
#define Defalt_FunctionKey_Event (FunctionKey_Event_t) {            \
    .click = NULL,                                                  \
    .multi = NULL,                                                  \
    .longpress = NULL,                                              \
}


// 按键的按下与释放
typedef enum{
    FunctionKey_BtnState_Released = 1, // 1代表释放
    FunctionKey_BtnState_Pressed = 0, // 0代表按下
    // 谁 1谁 2存疑
} FunctionKey_BtnState_t;
// 按键
typedef enum{
    FunctionKey_Check_Satisfied = 1,
    FunctionKey_Check_Unsatisfied = 0,
} FunctionKey_Check_t;

// 判断按键是否按下
FunctionKey_BtnState_t _FunctionKey_GetBtnState(FunctionKey_t* fkey);

//停止按钮的定时器
void _FunctionKey_xTimer_Stop(FunctionKey_t* fkey);

// 按键状态复位
void _FunctionKey_xTimer_Reset(FunctionKey_t* fkey, enum _FunctionKey_State_t next_state);



// 按键初始化
void FunctionKey_Init(FunctionKey_t* fkey);
// 更新按键状态，写在 freertos里，需要轮询所有的按键
void FunctionKey_Process(FunctionKey_t* fkey);

// 判断按下还是释放，用在状态检测函数中，提高可读性
uint8_t _FunctionKey_IsBtnPressed(FunctionKey_t* fkey);
uint8_t _FunctionKey_IsBtnReleased(FunctionKey_t* fkey);
// 确认按键状态，用在状态检测函数中，提高可读性
void _FunctionKey_SetBtnState(FunctionKey_t* fkey, FunctionKey_BtnState_t state);


// 各个状态所击发的回调函数，统一用在按键轮询函数里
// 所有按键的复位函数都是一样的
// 不需要判断是哪个按键
void _FunctionKey_CallbackSegment_Reset(FunctionKey_t* fkey);

// 所有按键的消抖函数也是一样的
// 不需要判断是哪个按键
void _FunctionKey_CallbackSegment_Debounce(FunctionKey_t* fkey);

// 下面的函数需要在内部添加 if判断
// 执行对应按键的任务
void _FunctionKey_CallbackSegment_LongWaiting(FunctionKey_t* fkey);
void _FunctionKey_CallbackSegment_LongOngoing(FunctionKey_t* fkey);
void _FunctionKey_CallbackSegment_MultiWaiting(FunctionKey_t* fkey);
void _FunctionKey_CallbackSegment_MultiCheckRelease(FunctionKey_t* fkey);
// 按键轮询函数
void FunctionKey_SwTimer_Callback(SwTimer_t* swTimer);



// 单击时做什么
void FunctionKey_SetClickEvent(FunctionKey_t* fkey, void (*click)(void));
// 多击时做什么
void FunctionKey_SetMultiEvent(FunctionKey_t* fkey, void (*multi)(uint32_t));
// 长按时做什么
void FunctionKey_SetLongPressEvent(FunctionKey_t* fkey, void (*longpress)(uint32_t));

// 按键活动，原本可能是写在 event函数里的
void FunctionKey_SetClickActivity(FunctionKey_t* fkey, uint8_t click);
void FunctionKey_SetMultiActivity(FunctionKey_t* fkey, uint8_t multi);
void FunctionKey_SetLongPressActivity(FunctionKey_t* fkey, uint8_t longpress);
void FunctionKey_SetLongPressRepeatActivity(FunctionKey_t* fkey, uint8_t longpress_repeat);

#endif