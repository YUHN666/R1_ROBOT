

#include "OLED_task.h"

char temp[64];
char txBuffer[32] = "";
float x;


extern uint32_t timerTick;
extern OledDisplay_t oled_display;
extern Rocker_t LeftRocker;
extern Rocker_t RightRocker;
extern uint8_t Control_txData[5];
extern uint8_t Adjust_txData[5];
extern uint8_t Adjust_rxData[5];
extern float addition;
extern int index;
extern uint8_t Forest_txData[13]; 
extern uint8_t Forest_rxData[5];


extern Battery_t battery;
extern FunctionKey_t FunctionKey[KEY_NUM];
// 电池-----------------------------------------------------------------------
void Battery_inform()
{
  u8g2_SetFontDirection(&oled_display.oled_ui.u8g2, 1);            //设置字体方向为水平
  u8g2_SetFont(&oled_display.oled_ui.u8g2, u8g2_font_battery19_tn);    //设置字体为电池图标
  u8g2_SetDrawColor(&oled_display.oled_ui.u8g2, 1);                //设置绘图颜色为白色
  switch (battery.energy)                            //横向打印电池图标
  {
      case Battery_Energy_0pct:
          u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 107, 20, "0");
          break;
      case Battery_Energy_20pct:
          u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 107, 20, "1");
          break;
      case Battery_Energy_40pct:
          u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 107, 20, "2");
          break;
      case Battery_Energy_60pct:
          u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 107, 20, "3");
          break;
      case Battery_Energy_80pct:
          u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 107, 20, "4");
          break;
      case Battery_Energy_100pct:
          u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 107, 20, "5");
          break;
      default://充电动画
          u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 107, 20, "0");      //打印一个电池显示err的图标来表示错误
          u8g2_SetFontDirection(&oled_display.oled_ui.u8g2, 0);
          u8g2_SetFont(&oled_display.oled_ui.u8g2, u8g2_font_4x6_tr);  
          u8g2_DrawStr(&oled_display.oled_ui.u8g2, 111, 7, "err");
          break;
  }
}

// 调参解算(放页面里)
// -----------------------------------------------------------------------------------
void YPB_Parameter(void)
{
  // 目标参数
  switch (FunctionKey[0]._temp.multi1_cnt % 12)
  {
    case 1: u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 100, 20, "Kxp");
    Adjust_txData[1] = Kxp; break;
    case 2: u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 100, 20, "Kxd");
    Adjust_txData[1] = Kxd; break;
    case 3: u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 100, 20, "Kyp");
    Adjust_txData[1] = Kyp; break;
    case 4: u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 100, 20, "Kyd");
    Adjust_txData[1] = Kyd; break;
    case 5: u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 100, 20, "Kop");
    Adjust_txData[1] = Kop; break;
    case 6: u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 100, 20, "Kod");
    Adjust_txData[1] = Kod; break;
    case 7: u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 100, 20, "Vs");
    Adjust_txData[1] = Vs; break;
    case 8: u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 100, 20, "Ds");
    Adjust_txData[1] = Ds; break;
    case 9: u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 100, 20, "T1");
    Adjust_txData[1] = T1; break;
    case 10: u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 100, 20, "T2");
    Adjust_txData[1] = T2; break;
    case 11: u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 100, 20, "T3");
    Adjust_txData[1] = T3; break;
    case 0: u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 100, 20, "T4");
    Adjust_txData[1] = T4; break;
    default: break;
  }
};

// ----------------------------------------------------------------------------------
float Calculate(void)
{
  addition = 1;
  switch (Adjust_txData[3])
  {
  case X1: addition *= 1; break;
  case X10: addition *= 10; break;
  case X100: addition *= 100; break;
  case X0_1: addition *= 0.1; break;
  case X0_01: addition *= 0.01; break;
  default: break;
  }
  addition *= Adjust_txData[2];
  addition *= (Adjust_txData[4] == 0x01 ? 1:-1);
  return addition;
}
// ----------------------------------------------------------------------------------
char* Judgment(uint8_t index)
{
  switch(Forest_txData[index])
  {
  case EMPTY: return "EMPTY"; break;
  case R1KFS: return "R1KFS"; break;
  case R2KFS: return "R2KFS"; break;
  case FAULSE: return "FAULSE"; break;
  default: return "UNKNOWN"; break;
  }
}
char* JudgmentShort(uint8_t index)
{
  switch(Forest_txData[index])
  {
  case EMPTY: return " "; break;
  case R1KFS: return "R1"; break;
  case R2KFS: return "R2"; break;
  case FAULSE: return "F"; break;
  default: return "U"; break;
  }
}


// KFS 地图加载
void ForestMap(void)
{
  u8g2_SetFont(&oled_display.oled_ui.u8g2, u8g2_font_6x10_tf);
  // 画格子
  // 横线
  u8g2_DrawLine(&oled_display.oled_ui.u8g2, 15, 15, 105, 15);
  u8g2_DrawLine(&oled_display.oled_ui.u8g2, 15, 40, 105, 40);
  u8g2_DrawLine(&oled_display.oled_ui.u8g2, 15, 65, 105, 65);
  u8g2_DrawLine(&oled_display.oled_ui.u8g2, 15, 90, 105, 90);
  u8g2_DrawLine(&oled_display.oled_ui.u8g2, 15, 115, 105, 115);
  // 竖线
  u8g2_DrawLine(&oled_display.oled_ui.u8g2, 15, 15, 15, 115);
  u8g2_DrawLine(&oled_display.oled_ui.u8g2, 45, 15, 45, 115);
  u8g2_DrawLine(&oled_display.oled_ui.u8g2, 75, 15, 75, 115);
  u8g2_DrawLine(&oled_display.oled_ui.u8g2, 105, 15, 105, 115);
  // 操作号
  sprintf(temp, "%d", index);
  u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 5, 127, temp);
  // 树桩情况
  u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 20, 127, Judgment(index));
  switch (FunctionKey[14]._temp.multi1_cnt % 2)
  {
  case 0:
    // 显示 “KFS放置模式 ”
    u8g2_SetFont(&oled_display.oled_ui.u8g2, u8g2_font_5x8_tf);
    u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 15, 10, "Red Zone KFS_Loading");
    // 格子编号
    u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 17, 114, "3");
    u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 47, 114, "2");
    u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 77, 114, "1");
    u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 17, 89, "6");
    u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 47, 89, "5");
    u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 77, 89, "4");  
    u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 17, 64, "9");
    u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 47, 64, "8");
    u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 77, 64, "7");  
    u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 17, 39, "12");
    u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 47, 39, "11");
    u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 77, 39, "10");
    // 树桩情况
    u8g2_SetFont(&oled_display.oled_ui.u8g2, u8g2_font_6x10_tf);
    for(uint8_t i = 1; i <= 12; i++)
    {
      switch (i)
      {
      case 3: u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 20, 105, JudgmentShort(i));; break;
      case 2: u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 50, 105, JudgmentShort(i));; break;
      case 1: u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 80, 105, JudgmentShort(i));; break;
      case 6: u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 20, 80, JudgmentShort(i));; break;
      case 5: u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 50, 80, JudgmentShort(i));; break;
      case 4: u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 80, 80, JudgmentShort(i));; break;
      case 9: u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 20, 55, JudgmentShort(i));; break;
      case 8: u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 50, 55, JudgmentShort(i));; break;
      case 7: u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 80, 55, JudgmentShort(i));; break;
      case 12: u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 20, 30, JudgmentShort(i));; break;
      case 11: u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 50, 30, JudgmentShort(i));; break;
      case 10: u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 80, 30, JudgmentShort(i));; break;
      default: break;
      }
    }
    break;
  case 1:
    // 显示 “KFS放置模式 ”
    u8g2_SetFont(&oled_display.oled_ui.u8g2, u8g2_font_5x8_tf);
    u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 15, 10, "Blue Zone KFS_Loading");
    // 格子编号
    u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 17, 114, "1");
    u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 47, 114, "2");
    u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 77, 114, "3");
    u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 17, 89, "4");
    u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 47, 89, "5");
    u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 77, 89, "6");  
    u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 17, 64, "7");
    u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 47, 64, "8");
    u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 77, 64, "9");  
    u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 17, 39, "10");
    u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 47, 39, "11");
    u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 77, 39, "12");
    // 树桩情况
    u8g2_SetFont(&oled_display.oled_ui.u8g2, u8g2_font_6x10_tf);
    for(uint8_t i = 1; i <= 12; i++)
    {
      switch (i)
      {
      case 1: u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 20, 105, JudgmentShort(i));; break;
      case 2: u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 50, 105, JudgmentShort(i));; break;
      case 3: u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 80, 105, JudgmentShort(i));; break;
      case 4: u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 20, 80, JudgmentShort(i));; break;
      case 5: u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 50, 80, JudgmentShort(i));; break;
      case 6: u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 80, 80, JudgmentShort(i));; break;
      case 7: u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 20, 55, JudgmentShort(i));; break;
      case 8: u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 50, 55, JudgmentShort(i));; break;
      case 9: u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 80, 55, JudgmentShort(i));; break;
      case 10: u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 20, 30, JudgmentShort(i));; break;
      case 11: u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 50, 30, JudgmentShort(i));; break;
      case 12: u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 80, 30, JudgmentShort(i));; break;
      default: break;
      }
    }
    break;
  default: break;
  }
}








// 遥控界面 ---------------------------------------------------------------------------
void OledDisplay_Control_Page(void)
{
  u8g2_SetFont(&oled_display.oled_ui.u8g2, u8g2_font_5x8_tf);
  u8g2_SetFontDirection(&oled_display.oled_ui.u8g2, 0);
  // 显示 “控制模式 ”
  u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 45, 10, "ControlMode");
  // 显示摇杆值
  sprintf(temp, "%d %d", LeftRocker.state.x, LeftRocker.state.y);
  u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 5, 110, temp);
  sprintf(temp, "%d %d", RightRocker.state.x, RightRocker.state.y);
  u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 5, 120, temp);
  // 显示手柄时间
  // temp不能太小，否则会发生数组越界，导致 timerTick值改变
  sprintf(temp, "%d", timerTick); 
  u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 60, 75, temp);
  Control_txData[1] = LeftRocker.state.x;
  Control_txData[2] = LeftRocker.state.y;
  Control_txData[3] = RightRocker.state.x;
  Control_txData[4] = RightRocker.state.y;
  zigbee_transmit(Control_txData, 5, R1_ROBOT); // 发送数据
  // 电池
  Battery_inform();
  u8g2_SendBuffer(&oled_display.oled_ui.u8g2);
  u8g2_ClearBuffer(&oled_display.oled_ui.u8g2);
}
// 调参界面 ---------------------------------------------------------------------------
void OledDisplay_Adjust_Page(void)
{
  u8g2_SetFont(&oled_display.oled_ui.u8g2, u8g2_font_5x8_tf);
  u8g2_SetFontDirection(&oled_display.oled_ui.u8g2, 0);
  // 显示 “调参模式 ”
  u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 35, 10, "TurningMode");
  // 显示目标参数
  u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 5, 20, "Parameter");
  // 显示基数  
  u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 5, 30, "FundamentalNum");
  // 显示倍率
  u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 5, 40, "Multiplier");
  // 显示增减
  u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 5, 50, "Sign");
  // 显示改变
  u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 5, 60, "Add:");
  // 显示当前值
  u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 5, 70, "Now:");
  memcpy(&x, &Adjust_rxData[1], 4);
  sprintf(temp, "%.2f", x);// 当前值
  u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 40, 70, temp);

  YPB_Parameter(); // 目标参数
  sprintf(temp, "%d",Adjust_txData[2]);// 基数
  u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 100, 30, temp);
  sprintf(temp, "%x",Adjust_txData[3]);// 倍率
  u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 100, 40, temp);
  sprintf(temp, "%d",Adjust_txData[4]);// 增减
  u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 100, 50, temp);
  
  sprintf(temp, "%.2f", Calculate()); // 计算并显示增量
  u8g2_DrawUTF8(&oled_display.oled_ui.u8g2, 45, 60, temp);
  u8g2_SendBuffer(&oled_display.oled_ui.u8g2);
  u8g2_ClearBuffer(&oled_display.oled_ui.u8g2);
}
// KFS放置界面 ---------------------------------------------------------------------------
void OledDisplay_KFS_Page(void)
{
  u8g2_SetFontDirection(&oled_display.oled_ui.u8g2, 0);
  ForestMap();
  u8g2_SendBuffer(&oled_display.oled_ui.u8g2);
  u8g2_ClearBuffer(&oled_display.oled_ui.u8g2);
}



void OLED(void *argument)
{
  

  /* USER CODE BEGIN OLED */
  /* Infinite loop */
  for(;;)
  {
    OledDisplay_Process();
    osDelay(1);
  }
  /* USER CODE END OLED */
}
  