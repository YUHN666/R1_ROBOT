#include "SwTimer.h"



// 定时器不会单独使用，通常绑定在其他的组件里
// 定义定时器队列并赋初值为零
SwTimerQueue_t swTimerQueue = {0};

// 加入定时器队列函数
SwTimer_t* SwTimer_QueuePush(SwTimerQueue_t* timer_queue, SwTimer_t* timer)
{
    // 下面的 if语句代码有一点小问题
    // if(timer_queue->count == 0)
    // {
    //     // 判断队列是否为空   
    //     return NULL;
    // }
    timer_queue->queue[timer_queue->tail] = timer;
    timer_queue->tail = (timer_queue->tail + 1) % SWTIMER_QUEUE_SIZE;
    timer_queue->count++;
    return timer; // 返回定时器地址
}

// 移出定时器队列函数
void SwTimer_QueuePop(SwTimerQueue_t* timer_queue)
{
    if(timer_queue->count == SWTIMER_QUEUE_SIZE)
    {
        return;
    }
    timer_queue->head = (timer_queue->head + 1) % SWTIMER_QUEUE_SIZE;
    timer_queue->count--;
}

// 初始化并启动定时器
SwTimer_t* SwTimer_Init(SwTimer_t* timer, SwTimerCallback_t callback, void* argument)
{
    if(timer == NULL) return NULL;
    timer->callback = callback;
    timer->argument = argument;
    timer->_time = 0;
    return SwTimer_QueuePush(&swTimerQueue, timer); // 返回定时器地址
}

// 设置轮询回调触发时间
void SwTimer_Start(SwTimer_t* timer, uint32_t time)
{
    if(timer == NULL) return;
    // 系统时加定时时间
    timer->_time = swTimerQueue.tick + time;
}

// 停止定时器
void SwTimer_Stop(SwTimer_t* timer)
{
    if(timer == NULL) return;
    timer->_time = 0;
}

// 读取携带参数
void* SwTimer_GetArgument(SwTimer_t* timer)
{
    if(timer == NULL) return NULL;
    return timer->argument;
}

// 设置携带参数
void SwTimer_SetArgument(SwTimer_t* timer, void* argument)
{
    if(timer == NULL) return;
    timer->argument = argument;
}

// 读取结构体回调函数
SwTimerCallback_t SwTimer_GetCallback(SwTimer_t* timer)
{
    if(timer == NULL) return NULL;
    return timer->callback;
}

// 设置结构体回调函数
void SwTimer_SetCallback(SwTimer_t* timer, SwTimerCallback_t callback)
{
    if(timer == NULL) return;
    timer->callback = callback;
}


// 循环运行轮询函数
void SwTimer_TimerQueuePeriodicProcess(uint32_t timerTick)
{
    // 轮询队列
    // 定时器都放在队列里
	swTimerQueue.tick = timerTick;
    uint32_t iter = swTimerQueue.head; // 迭代器
    SwTimer_t* timer;
    while(iter != swTimerQueue.tail)
    {
        timer = swTimerQueue.queue[iter];
		iter = (iter + 1) % SWTIMER_QUEUE_SIZE;
        if(timer->callback == NULL || timer->_time == 0 
            || swTimerQueue.tick <= timer->_time) continue;
		timer->_time = 0;
        timer->callback(timer); // timer做参数，回调
    }
}
