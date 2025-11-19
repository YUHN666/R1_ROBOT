// 模拟 I2C所需要的软件时钟

#include "delay.h"
#include "stm32h7xx_hal.h"




// 初始化 DWT (Data Watchpoint and Trace) 单元的循环计数器
// 用于实现高精度的微秒级或纳秒级延时
// DWT: Data watchpoint and trace unit 数据观察点与跟踪单元
// CYCCNT为 DWT里的32位向上计数器
void DWT_Delay_Init(void)
{
    // 检查是否已经使能了DWT跟踪功能
    if ( !(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk) ) 
    {
        // 如果没有使能，则使能 DWT跟踪
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
        // 将 DWT周期计数器清零
        DWT->CYCCNT = 0;
        // 使能 DWT周期计数器
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    }
}
// 纳秒级延时
void my_nsDelay(uint32_t ns)
{
    DWT->CYCCNT = 0;
    // HAL_RCC_GetHCLKFreq() 用于获取内部时钟频率，单位是 Hz
    uint32_t ns_count_ticks = (ns * HAL_RCC_GetHCLKFreq()) / 1000000000;
    if(ns_count_ticks < 5) ns_count_ticks = 5;
    while (DWT->CYCCNT < ns_count_ticks);
}

// 微秒级延时
void my_usDelay(uint32_t us)
{
    DWT->CYCCNT = 0;
    // HAL_RCC_GetHCLKFreq() 用于获取内部时钟频率，单位是 Hz
    uint32_t us_count_ticks = us * (HAL_RCC_GetHCLKFreq() / 1000000);
    while (DWT->CYCCNT < us_count_ticks);
}
// 毫秒级延时
void my_msDelay(uint32_t ms)
{
	my_usDelay(1000 * ms);
}
