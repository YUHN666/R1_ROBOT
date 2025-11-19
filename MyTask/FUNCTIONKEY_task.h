#ifndef _FUNCTIONKEY_TASK_
#define _FUNCTIONKEY_TASK_

#include "main.h"
#include "Config.h"
#include "cmsis_os.h"


void multi_PC15(uint32_t multi); // 调参用
void multi_PC14(uint32_t multi); // 调参用
void click_PC15(void);
void click_PC14(void);
void click_PF2(void);
void click_PF0(void);
void click_PF4(void);
void click_PF3(void);

void click_PA11(void); // 判定为空
void click_PA10(void); // 判定为 R1
void click_PA9(void); // 判定为 R2
void click_PA8(void); // 判定为假
void click_PG7(void); // 左移树桩
void click_PG8(void); // 右移树桩
void click_PC12(void);
void click_PC11(void); // 切换红蓝区

#endif