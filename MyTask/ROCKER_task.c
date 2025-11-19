#include "ROCKER_task.h"
#include "cmsis_os.h"

extern AdcBsp_t adcBsp_adc1;

extern Rocker_t LeftRocker;
extern Rocker_t RightRocker;
void ROCKER(void *argument)
{
  /* USER CODE BEGIN ROCKER */




  /* Infinite loop */
  for(;;)
  {
    
    Rocker_Process(&LeftRocker);
    Rocker_Process(&RightRocker);
    osDelay(1);
  }
  /* USER CODE END ROCKER */
}






















