#include "FunctionKey.h"
#include "delay.h"
#include "SwTimer.h"






// 判断按键是否按下
FunctionKey_BtnState_t _FunctionKey_GetBtnState(FunctionKey_t* fkey)
{
	FunctionKey_BtnState_t output;
    // 判断是否为矩阵键盘（一般没啥用）
	if(fkey->gpioO.pin != NULL)
    {  
		HAL_GPIO_WritePin(fkey->gpioO.port, fkey->gpioO.pin, GPIO_PIN_SET);
		my_usDelay(2);
		output = (FunctionKey_BtnState_t)(fkey->gpioI.is_inversed ^ 
                            HAL_GPIO_ReadPin(fkey->gpioI.port, fkey->gpioI.pin));
		my_usDelay(2);
		HAL_GPIO_WritePin(fkey->gpioO.port, fkey->gpioO.pin, GPIO_PIN_RESET);
		my_usDelay(2);
	}
    else
    {
        // 按位异或，两个对应二进制位不同时，结果为 1
		output = (FunctionKey_BtnState_t)(fkey->gpioI.is_inversed ^ 
            HAL_GPIO_ReadPin(fkey->gpioI.port, fkey->gpioI.pin));
        // 感知按钮是否按下
	}
    return output;
    // 返回 0，表示按下
    // 返回 1，表示释放
}

//停止按钮的定时器
void _FunctionKey_xTimer_Stop(FunctionKey_t* fkey)
{
    SwTimer_Stop(&fkey->_temp.timer);
}



// 安排按键状态（state）
// 并根据不同的活动设置回调出发时间
void _FunctionKey_xTimer_Reset(FunctionKey_t* fkey, enum _FunctionKey_State_t next_state)
{
	
    uint32_t time = 0;
    switch (next_state)
    {
        // 等待消抖
    case _FunctionKey_State_Debounce:
        time = fkey->time.debounce;
        break;
        // 复位
    case _FunctionKey_State_Reset:
        time = fkey->time.polling;
        break;
        // 长按等待
    case _FunctionKey_State_LongWaiting:
        time = fkey->time.polling;
        break;
       // 长按时触发事件的间隔
    case _FunctionKey_State_LongOngoing:
        time = fkey->time.long_ongoing;
        break;
        // 多击等待
    case _FunctionKey_State_MultiWaiting:
        time = fkey->time.polling;
        break;
        // 多击释放
    case _FunctionKey_State_MultiCheckRelease:
        time = fkey->time.polling;
        break;
    default:
        Error_Handler();
    }
	fkey->_temp.state = next_state;
    SwTimer_Start(&fkey->_temp.timer, time);
}
// 按键轮询函数（仅声明一下）
void FunctionKey_SwTimer_Callback(SwTimer_t* swTimer);

// 按键初始化
void FunctionKey_Init(FunctionKey_t* fkey)
{
    // 允许中断
    fkey->_temp.exti_sta = 1;
    fkey->activity = Defalt_FunctionKey_Activity;
    fkey->time = Defalt_FunctionKey_Time;
    fkey->event = Defalt_FunctionKey_Event;
    // 将按键定时器添加到定时器队列，加入按键轮询函数        // 定时器的参数设为了按键地址
    SwTimer_Init(&fkey->_temp.timer, FunctionKey_SwTimer_Callback, fkey); 
}

// 更新按键状态，写在 freertos里，需要轮询所有的按键
void FunctionKey_Process(FunctionKey_t* fkey)
{
    uint8_t now_btn_sta = _FunctionKey_GetBtnState(fkey); // 0表示按下
    // 如果检测到按键按下，且允许中断打断
    // 只有允许中断的按键才会判断是否按下
    if( (now_btn_sta == 0 && fkey->_temp.last_btn_sta == 1) && fkey->_temp.exti_sta == 1)
    {
        // 按下后关闭中断
        fkey->_temp.exti_sta = 0;
        // 进入消抖时间段
        _FunctionKey_xTimer_Reset(fkey, _FunctionKey_State_Debounce);
    }
    fkey->_temp.last_btn_sta = now_btn_sta;
}

// 判断是否按下
uint8_t _FunctionKey_IsBtnPressed(FunctionKey_t* fkey)
{
    return ( _FunctionKey_GetBtnState(fkey) == FunctionKey_BtnState_Pressed );
}
// 判断是否释放
uint8_t _FunctionKey_IsBtnReleased(FunctionKey_t* fkey)
{
    return ( _FunctionKey_GetBtnState(fkey) == FunctionKey_BtnState_Released );
}
// 说明按键状态
void _FunctionKey_SetBtnState(FunctionKey_t* fkey, FunctionKey_BtnState_t state)
{
    fkey->state = state;
}


// 原位回调函数，即按键在原位置
// 不需要判断是哪个按键
void _FunctionKey_CallbackSegment_Reset(FunctionKey_t* fkey)
{
    // 每隔polling时间检测一次按键状态
    // 如果按键在fkey->time.reset时间内均松开，则认为按键已经复位
    // 关闭定时器并使能中断，中断触发后将进入Debounce状态
    // 如果按键在fkey->time.reset时间内再次按下，则重新开始计时
    if(_FunctionKey_IsBtnReleased(fkey)) // 如果按键松开
    {
        fkey->_temp.curr_time += fkey->time.polling;
        if(fkey->_temp.curr_time >= fkey->time.reset)
        {
            fkey->_temp.curr_time = 0;
            fkey->_temp.exti_sta = 1;
            _FunctionKey_SetBtnState(fkey, FunctionKey_BtnState_Released);
            _FunctionKey_xTimer_Stop(fkey);
        }
        else
        {
            // 按键状态改为复位状态
            _FunctionKey_xTimer_Reset(fkey, _FunctionKey_State_Reset);
        }
    }
    else // 如果按键是按下的
    {
        fkey->_temp.curr_time = 0;
        _FunctionKey_SetBtnState(fkey, FunctionKey_BtnState_Pressed);
        _FunctionKey_xTimer_Reset(fkey, _FunctionKey_State_Reset);
    }
}
// 消抖函数
// 所有按键的复位函数都是一样的
// 不需要判断是哪个按键
void _FunctionKey_CallbackSegment_Debounce(FunctionKey_t* fkey)
{
    // 按键消抖状态
    // 当按键中断检测到按键按下时，启动定时器经过fkey->time.debounce时间后触发定时器中断
    // 并进入Debounce状态
    // 如果此时按键按下，则认为按键有效，触发click事件，并进入LongWaiting状态
    // 如果此时按键松开，则认为按键无效，重新Reset
    if( _FunctionKey_IsBtnPressed(fkey) )
    {
        _FunctionKey_SetBtnState(fkey, FunctionKey_BtnState_Pressed);
        if(fkey->event.click && fkey->activity.click) fkey->event.click(); // 执行单击任务
        _FunctionKey_xTimer_Reset(fkey, _FunctionKey_State_LongWaiting);
    }
    else
    {
        _FunctionKey_SetBtnState(fkey, FunctionKey_BtnState_Released);
        _FunctionKey_xTimer_Reset(fkey, _FunctionKey_State_Reset);
    }
}
void _FunctionKey_CallbackSegment_LongWaiting(FunctionKey_t* fkey)
{
    //* 长按等待状态
    //* 当Debounce状态检测到按键按下后，随即进入LongWaiting状态
    //* 如果在fkey->time.long_wait时内按键均未弹起，则认为长按有效，进入LongOngoing状态
    //* 如果在fkey->time.long_wait时内按键弹起(轮询检测连续两次)，则进入MultiWaiting状态
    fkey->_temp.curr_time += fkey->time.polling;
    if(fkey->_temp.curr_time <= fkey->time.long_wait)
    {
        if(_FunctionKey_IsBtnReleased(fkey) && fkey->_temp.last_check == FunctionKey_Check_Satisfied)
        {
            fkey->_temp.curr_time = 0;
            fkey->_temp.last_check = FunctionKey_Check_Unsatisfied;
            _FunctionKey_SetBtnState(fkey, FunctionKey_BtnState_Released);
            _FunctionKey_xTimer_Reset(fkey, _FunctionKey_State_MultiWaiting);
        }
        else
        {
            fkey->_temp.last_check = _FunctionKey_IsBtnReleased(fkey) ? FunctionKey_Check_Satisfied : FunctionKey_Check_Unsatisfied;
            _FunctionKey_xTimer_Reset(fkey, _FunctionKey_State_LongWaiting);
        }
    }
    else 
    {
        fkey->_temp.longpress_cnt = 0;
        fkey->_temp.curr_time = 0;
        fkey->_temp.last_check = FunctionKey_Check_Unsatisfied;
        _FunctionKey_SetBtnState(fkey, FunctionKey_BtnState_Pressed);
        _FunctionKey_xTimer_Reset(fkey, _FunctionKey_State_LongOngoing);
    }
}
void _FunctionKey_CallbackSegment_LongOngoing(FunctionKey_t* fkey)
{
    //* 长按持续状态
    //* 每隔fkey->time.long_ongoing时间检测一次按键状态
    //* 如果按键松开，则进入Reset状态
    //* 如果按键仍然按下，则触发longpress事件并再次进入LongOngoing状态
    if(_FunctionKey_IsBtnPressed(fkey))
    {
        _FunctionKey_SetBtnState(fkey, FunctionKey_BtnState_Pressed);
        if(fkey->event.longpress && fkey->activity.longpress)
        {
            if(!fkey->activity.longpress_repeat && fkey->_temp.longpress_cnt != 0)
                return;
            fkey->event.longpress(fkey->_temp.longpress_cnt++); // 执行长按任务
        }
        _FunctionKey_xTimer_Reset(fkey, _FunctionKey_State_LongOngoing);
    }
    else
    {
        fkey->_temp.longpress_cnt = 0;
        _FunctionKey_SetBtnState(fkey, FunctionKey_BtnState_Released);
        _FunctionKey_xTimer_Reset(fkey, _FunctionKey_State_Reset);
    }
}
void _FunctionKey_CallbackSegment_MultiWaiting(FunctionKey_t* fkey)
{
    //* 多击等待状态
    //* 当LongWaiting状态检测到按键弹起后(即未进入LongOngoing)，随即进入MultiWaiting状态
    //* 如果在fkey->time.multi_wait时间内按键再次按下，则认为多击有效，multi_cnt++，并进入MultiCheckRelease状态
    //* 如果在fkey->time.multi_wait时间内按键未再次按下，则认为多击退出，触发multi事件并进入Reset状态
    fkey->_temp.curr_time += fkey->time.polling;
    if(fkey->_temp.curr_time <= fkey->time.multi_wait)
    {
        if(_FunctionKey_IsBtnPressed(fkey) && fkey->_temp.last_check == FunctionKey_Check_Satisfied)
        {
            fkey->_temp.curr_time = 0;
            fkey->_temp.last_check = FunctionKey_Check_Unsatisfied;
            fkey->_temp.multi_cnt++;
            _FunctionKey_SetBtnState(fkey, FunctionKey_BtnState_Pressed);
            _FunctionKey_xTimer_Reset(fkey, _FunctionKey_State_MultiCheckRelease);
        }
        else
        {
            fkey->_temp.last_check = _FunctionKey_IsBtnPressed(fkey) ? FunctionKey_Check_Satisfied : FunctionKey_Check_Unsatisfied;
            _FunctionKey_xTimer_Reset(fkey, _FunctionKey_State_MultiWaiting);
        }
    }
    else
    {
        if(fkey->event.multi && fkey->activity.multi)
        {
            fkey->event.multi(fkey->_temp.multi_cnt + 1); // 执行多击任务
        }
        fkey->_temp.multi_cnt = 0;
        fkey->_temp.curr_time = 0;
        fkey->_temp.last_check = FunctionKey_Check_Unsatisfied;
        _FunctionKey_SetBtnState(fkey, FunctionKey_BtnState_Released);
        _FunctionKey_xTimer_Reset(fkey, _FunctionKey_State_Reset);
    }
}
void _FunctionKey_CallbackSegment_MultiCheckRelease(FunctionKey_t* fkey)
{
    //* 多击检测释放状态
    //* 当MultiWaiting状态检测到按键再次按下后，随即进入MultiCheckRelease状态
    //* 每隔fkey->time.polling时间检测一次按键状态，如果按键松开，则重新进入MultiWaiting状态
    //* 如果按键仍然按下，则继续进入MultiCheckRelease状态，直到按键松开
    if(_FunctionKey_IsBtnReleased(fkey))
    {
        _FunctionKey_SetBtnState(fkey, FunctionKey_BtnState_Released);
        _FunctionKey_xTimer_Reset(fkey, _FunctionKey_State_MultiWaiting);
    }
    else
    {
        _FunctionKey_SetBtnState(fkey, FunctionKey_BtnState_Pressed);
        _FunctionKey_xTimer_Reset(fkey, _FunctionKey_State_MultiCheckRelease);
    }
}

// 按键轮询函数的定义
void FunctionKey_SwTimer_Callback(SwTimer_t* swTimer)
{
    // 再定义一个 fkey，提高可读性
    FunctionKey_t* fkey = (FunctionKey_t*)SwTimer_GetArgument(swTimer);
    switch (fkey->_temp.state)
    {
        // 按按键状态去执行对应的回调函数
    case _FunctionKey_State_Reset:
        _FunctionKey_CallbackSegment_Reset(fkey);
        break;
    case _FunctionKey_State_Debounce:
        _FunctionKey_CallbackSegment_Debounce(fkey);
        break;
    case _FunctionKey_State_LongWaiting:
        _FunctionKey_CallbackSegment_LongWaiting(fkey);
        break;
    case _FunctionKey_State_LongOngoing:
        _FunctionKey_CallbackSegment_LongOngoing(fkey);
        break;
    case _FunctionKey_State_MultiWaiting:
        _FunctionKey_CallbackSegment_MultiWaiting(fkey);
        break;
    case _FunctionKey_State_MultiCheckRelease:
        _FunctionKey_CallbackSegment_MultiCheckRelease(fkey);
        break;
    }
}






// Event用于分配各个按键在各个状态下执行的函数
void FunctionKey_SetClickEvent(FunctionKey_t* fkey, void (*click)(void))
{
    fkey->event.click = click;
}
void FunctionKey_SetMultiEvent(FunctionKey_t* fkey, void (*multi)(uint32_t))
{
    fkey->event.multi = multi;
}
void FunctionKey_SetLongPressEvent(FunctionKey_t* fkey, void (*longpress)(uint32_t))
{
    fkey->event.longpress = longpress;
}
// Activity用于
void FunctionKey_SetClickActivity(FunctionKey_t* fkey, uint8_t click)
{
    fkey->activity.click = click;
}
void FunctionKey_SetMultiActivity(FunctionKey_t* fkey, uint8_t multi)
{
    fkey->activity.multi = multi;
}
void FunctionKey_SetLongPressActivity(FunctionKey_t* fkey, uint8_t longpress)
{
    fkey->activity.longpress = longpress;
}
void FunctionKey_SetLongPressRepeatActivity(FunctionKey_t* fkey, uint8_t longpress_repeat)
{
    fkey->activity.longpress_repeat = longpress_repeat;
}


















