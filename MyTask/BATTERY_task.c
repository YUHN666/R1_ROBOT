

#include "BATTERY_task.h"


extern Battery_t battery;
void BATTERY(void *argument)
{
  /* USER CODE BEGIN BATTERY */
  
  /* Infinite loop */
  for(;;)
  {
    Battery_Process(&battery);
    osDelay(1);
    // 可以数七万分钟
  }
  /* USER CODE END BATTERY */
}