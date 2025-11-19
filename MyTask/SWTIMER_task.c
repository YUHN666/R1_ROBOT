#include "SWTIMER_task.h"



uint32_t timerTick = 0;
void SWTIMER(void *argument)
{
  /* USER CODE BEGIN SWTIMER */
  
  /* Infinite loop */
  for(;;)
  {
    timerTick += 1;
    SwTimer_TimerQueuePeriodicProcess(timerTick);
    osDelay(1);
    // 可以数七万分钟
  }
  /* USER CODE END SWTIMER */
}