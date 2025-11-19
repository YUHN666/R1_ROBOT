#ifndef _SWTIMER_H_
#define _SWTIMER_H_


#include "main.h"
#define SWTIMER_QUEUE_SIZE 40
#define TASK_QUEUE_SIZE 40




//软件单次定时器
typedef struct SwTimer_t
{
    // 携带参数  在本项目里为定时器的地址
    void* argument; 
    
    // 回调函数  用于触发超时回调
    void (*callback)(struct SwTimer_t* swTimer);  

    // 当系统时间大于该时间时，触发回调函数，然后停止定时器
    // 该时间为零时，定时器停止
    uint32_t _time;             
} SwTimer_t;

// 回调函数指针
typedef void (*SwTimerCallback_t)(SwTimer_t* swTimer);

// 定时器队列
typedef struct
{
    SwTimer_t* queue[SWTIMER_QUEUE_SIZE];
    uint32_t head;  // 头指针（用数组号表示）
    uint32_t tail;  // 尾指针（用数组号表示）
    uint32_t count; // 定时器总数
	uint32_t tick;  // 软件计时器系统时
} SwTimerQueue_t;

// 回调任务队列
typedef struct
{
    SwTimerCallback_t queue[TASK_QUEUE_SIZE];
    uint32_t head;  // 头指针（用数组号表示）
    uint32_t tail;  // 尾指针（用数组号表示）
    uint32_t count; // 回调任务总数
} TaskQueue_t;

// 加入定时器队列函数
SwTimer_t* SwTimer_QueuePush(SwTimerQueue_t* timer_queue, SwTimer_t* timer);
// 移出定时器队列函数
void SwTimer_QueuePop(SwTimerQueue_t* timer_queue);
// 初始化并启动定时器
SwTimer_t* SwTimer_Init(SwTimer_t* timer, SwTimerCallback_t callback, void* argument);
// 设置定时时间
void SwTimer_Start(SwTimer_t* timer, uint32_t time);
// ͣ停止定时器
void SwTimer_Stop(SwTimer_t* timer);
// 读取携带参数
void* SwTimer_GetArgument(SwTimer_t* timer);
// 设置携带参数
void SwTimer_SetArgument(SwTimer_t* timer, void* argument);
// 读取结构体回调函数
SwTimerCallback_t SwTimer_GetCallback(SwTimer_t* timer);
// 设置结构体回调函数
void SwTimer_SetCallback(SwTimer_t* timer, SwTimerCallback_t callback);
// 轮询回调函数
void SwTimer_TimerQueuePeriodicProcess(uint32_t timerTick);


#endif