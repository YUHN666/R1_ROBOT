#ifndef __DELAY_H__
#define __DELAY_H__

#include "main.h"

void DWT_Delay_Init(void);
void my_nsDelay(uint32_t ns);
void my_usDelay(uint32_t us);
void my_msDelay(uint32_t ms);



#endif
