#include "COMMS_task.h"

// zigbee ----------------------------------------------------------------------------------
extern zigbee_state_t zigbee_state;
extern uint8_t zigbee_rxBuffer[100];
extern uint8_t Adjust_rxData[5];
extern DMA_HandleTypeDef hdma_uart4_rx;

// ZIGBEE的线程 -----------------------------------------------------------------------------
void ZIGBEE(void *argument)
{
  /* USER CODE BEGIN ZIGBEE */
  /* Infinite loop */
  for(;;)
  {
    
    osDelay(1);
  }
  /* USER CODE END ZIGBEE */
}

// DMA中断回调-------------------------------------------------------
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
  /* Prevent unused argument(s) compilation warning */
  if(huart == zigbee_state.huart)
  {
    // ID
    if(zigbee_rxBuffer[0] == 0xAB && zigbee_rxBuffer[1] == 0xD1 && zigbee_rxBuffer[Size-1] == 0xBA)
    {
      zigbee_state.MyAddr = (zigbee_rxBuffer[2] << 8) | zigbee_rxBuffer[3];
      zigbee_state.ProtocolType = (zigbee_rxBuffer[4] << 8) | zigbee_rxBuffer[5];
    }
    // 全部配置
    if(zigbee_rxBuffer[0] == 0xAB && zigbee_rxBuffer[1] == 0xD2 && zigbee_rxBuffer[Size-1] == 0xBA)
    {
      // 加载配置
      load_Conf();
    }
    // 接收调参回传
    if(zigbee_rxBuffer[0] == 0xAA && zigbee_rxBuffer[1] == 0xD1 
      && ((zigbee_rxBuffer[2]<<8)|zigbee_rxBuffer[3]) == R1_ROBOT
      && zigbee_rxBuffer[4] == ADJUST)
    {
      Adjust_rxData[0] = zigbee_rxBuffer[4];
      Adjust_rxData[1] = zigbee_rxBuffer[5]; // 参数当前值
      Adjust_rxData[2] = zigbee_rxBuffer[6];
      Adjust_rxData[3] = zigbee_rxBuffer[7];
      Adjust_rxData[4] = zigbee_rxBuffer[8];
    }
    // 
    HAL_UARTEx_ReceiveToIdle_DMA(zigbee_state.huart, zigbee_rxBuffer, 100);
    // 关闭 DMA传输过半中断，防止数据丢失
    __HAL_DMA_DISABLE_IT(&hdma_uart4_rx, DMA_IT_HT);
    zigbee_state.rx_completed = 1;
  }
  
  
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_UART_RxCpltCallback can be implemented in the user file.
   */
}

