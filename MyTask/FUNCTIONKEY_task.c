
#include "FUNCTIONKEY_task.h"

int8_t index = 0;

extern FunctionKey_t FunctionKey[KEY_NUM];
extern uint8_t Control_txData[5];
extern uint8_t Adjust_txData[5];
extern uint8_t Forest_txData[13]; 
extern float addition;
extern OledDisplay_t oled_display;
extern uint8_t Key_txData[2];
extern uint8_t Key_rxData[2];
// 按键任务--------------------------------------------------------------------------

void click_PC15(void)
{
  if(HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_6) == GPIO_PIN_SET)
  {
    Key_txData[1] = PC15;
    zigbee_transmit(Key_txData, 2, R1_ROBOT);
  }
}
void multi_PC15(uint32_t multi) // 调参用
{
  if(HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_6) == GPIO_PIN_SET) return;
  if(multi == 1)// 必须加这个判断，否则单击也会触发
  { // 参数
    FunctionKey[0]._temp.multi1_cnt++;
  }
  if(multi == 2)// 必须加这个判断，否则单击也会触发
  {   // 数值
    FunctionKey[0]._temp.multi2_cnt++;
    switch (FunctionKey[0]._temp.multi2_cnt % 5)
    {
        case 1: Adjust_txData[2] = 0x01; break;// 1
        case 2: Adjust_txData[2] = 0x02; break;// 2
        case 3: Adjust_txData[2] = 0x03; break;// 3
        case 4: Adjust_txData[2] = 0x05; break;// 5
        case 0: Adjust_txData[2] = 0x0A; break;// 10
        default: break;
    }
  }
}
void click_PC14(void)
{
  if(HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_6) == GPIO_PIN_SET)
  {
    Key_txData[1] = PC14;
    zigbee_transmit(Key_txData, 2, R1_ROBOT);
  }
}
void multi_PC14(uint32_t multi) // 调参用
{ 
  
  
  if(multi == 1)// 必须加这个判断，否则单击也会触发
  {   // 增减
    FunctionKey[1]._temp.multi2_cnt++;
    switch (FunctionKey[1]._temp.multi2_cnt % 2)
    {
      case 1: Adjust_txData[4] = 0x01; break;// +
      case 0: Adjust_txData[4] = 0x00; break;// -
      default: break;
    }
  }  
  if(multi == 2)// 必须加这个判断，否则单击也会触发
  {
    // 倍率
    FunctionKey[1]._temp.multi1_cnt++;
    switch (FunctionKey[1]._temp.multi1_cnt % 5)
    {
      case 1: Adjust_txData[3] = X1; break;// ×1
      case 2: Adjust_txData[3] = X10; break;// ×10
      case 3: Adjust_txData[3] = X100; break;// ×100
      case 4: Adjust_txData[3] = X0_1; break;// ×0.1  
      case 0: Adjust_txData[3] = X0_01; break;// ×0.01     
      default: break;
    }
  }
}
void click_PF2(void)
{
  if(HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_6) == GPIO_PIN_SET)
  {
    Key_txData[1] = PF2;
    zigbee_transmit(Key_txData, 2, R1_ROBOT);
  }
  else
  { 
    // PF6电亮才能发送
    // 发送调参指令
    zigbee_transmit(Adjust_txData, 5, R1_ROBOT);
  }
}

// 发送 KFS位置信息------------------------------------------------
void click_PF0(void)
{
  if(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_8) == GPIO_PIN_SET)
  {
    Key_txData[1] = PF0;
    zigbee_transmit(Key_txData, 2, R1_ROBOT);
  }
  else
  { 
    // PF6电亮才能发送
    // 发送调参指令
    zigbee_transmit(Forest_txData, 13, R1_ROBOT);
  }
}

// 切换界面（也会切换模式）
void click_PF4(void)
{
  if(HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_6) == GPIO_PIN_SET)
  {
    Key_txData[1] = PF4;
    zigbee_transmit(Key_txData, 2, R1_ROBOT);
  }
  else
  { 
    FunctionKey[4]._temp.multi1_cnt++;
    switch (FunctionKey[4]._temp.multi1_cnt % 3)
    {
      case 1: OledUI_SetPage(&oled_display.oled_ui, Adjust_Page); break;
      case 2: OledUI_SetPage(&oled_display.oled_ui, KFS_Page); break;
      case 0: OledUI_SetPage(&oled_display.oled_ui, Control_Page); break;
      default: break;
    }
  }
  
}


void click_PF3(void)
{
  if(HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_6) == GPIO_PIN_SET)
  {
    Key_txData[1] = PF3;
    zigbee_transmit(Key_txData, 2, R1_ROBOT);
  }
  else
  { 
    ;
  }
}

// 判定为空
void click_PA11(void)
{ // 打开开关才能配置
  if(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_8) == GPIO_PIN_SET)
  {
    Key_txData[1] = PA11;
    zigbee_transmit(Key_txData, 2, R1_ROBOT);
  }
  else
  {
    Forest_txData[index] = EMPTY;
  }
}
// 判定为 R1
void click_PA10(void)
{ // 打开开关才能配置
  if(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_8) == GPIO_PIN_SET)
  {
    Key_txData[1] = PA10;
    zigbee_transmit(Key_txData, 2, R1_ROBOT);
  }
  else
  {
    Forest_txData[index] = R1KFS;
  }
}// 判定为假
void click_PA9(void)
{ // 打开开关才能配置
  if(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_8) == GPIO_PIN_SET)
  {
    Key_txData[1] = PA9;
    zigbee_transmit(Key_txData, 2, R1_ROBOT);
  }
  else
  {
    Forest_txData[index] = FAULSE;
  }
}
// 判定为 R2
void click_PA8(void)
{ // 打开开关才能配置
  if(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_8) == GPIO_PIN_SET)
  {
    Key_txData[1] = PA8;
    zigbee_transmit(Key_txData, 2, R1_ROBOT);
  }
  else
  {
    Forest_txData[index] = R2KFS;
  }
}
// 左移树桩
void click_PG7(void)
{ // 打开开关才能配置
  if(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_8) == GPIO_PIN_SET)
  {
    Key_txData[1] = PG7;
    zigbee_transmit(Key_txData, 2, R1_ROBOT);
  }
  else
  {
    index = (--index < 1) ? 12 : index;
  }
}
// 右移树桩
void click_PG8(void)
{ // 打开开关才能配置
  if(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_8) == GPIO_PIN_SET)
  {
    Key_txData[1] = PG8;
    zigbee_transmit(Key_txData, 2, R1_ROBOT);
  }
  else
  {
    index = (++index > 12) ? 1 : index;
  }
}

void click_PC12(void)
{ 
  // 打开开关才能配置
  if(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_8) == GPIO_PIN_SET)
  {
    Key_txData[1] = PC12;
    zigbee_transmit(Key_txData, 2, R1_ROBOT);
  }
  else
  {
    
  } 
}

// 切换红区蓝区
void click_PC11(void)
{ 
  // 打开开关才能配置
  if(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_8) == GPIO_PIN_SET)
  {
    Key_txData[1] = PC11;
    zigbee_transmit(Key_txData, 2, R1_ROBOT);
  }
  else
  {
    FunctionKey[14]._temp.multi1_cnt++;
    switch(FunctionKey[14]._temp.multi1_cnt % 2)
    {
      case 0: Forest_txData[0] = RED_ZONE; break;
      case 1: Forest_txData[0] = BLUE_ZONE; break;
      default: break;
    }
  } 
}


// 按键线程 ---------------------------------------------------------------
void FUNCTIONKEY(void *argument)
{
  /* USER CODE BEGIN FunctionKey */
  /* Infinite loop */
  for (;;)
  {
    // 更新每个按键的状态
    for(int i = 0; i < KEY_NUM; i++)
    {
      FunctionKey_Process(&FunctionKey[i]);
    }
    osDelay(1);
  }
  /* USER CODE END FunctionKey */
}
