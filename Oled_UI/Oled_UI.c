#include "Oled_UI.h"
#include <stdio.h>

#define u8         unsigned char  
#define MAX_LEN    128  //
#define OLED_ADDRESS  0x78 // oled
#define OLED_CMD   0x00  // 
#define OLED_DATA  0x40  // 



static I2C_HandleTypeDef* oled_hi2c = &hi2c1;
extern OledDisplay_t oled_display;




// 初始化硬件 I2C接口    公式化 cv，不求甚解
uint8_t u8x8_byte_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{


    /* u8g2/u8x8 在 START_TRANSFER 和 END_TRANSFER 之间发送的数据永远不会超过 32 字节 */
	static uint8_t buf_idx;
	static uint8_t buffer[128];		
	uint8_t *data;

	switch(msg)
	{
		case U8X8_MSG_BYTE_SEND:
		  data = (uint8_t *)arg_ptr;      
		  while( arg_int > 0 ){
					buffer[buf_idx++] = *data;
					data++;
					arg_int--;
				}      
		break;
				
		case U8X8_MSG_BYTE_INIT:
		  /* add your custom code to init i2c subsystem */
		break;
			
		case U8X8_MSG_BYTE_START_TRANSFER:
		  buf_idx = 0;
		break;
			
		case U8X8_MSG_BYTE_END_TRANSFER:
			HAL_I2C_Master_Transmit(oled_hi2c, u8x8_GetI2CAddress(u8x8), buffer, buf_idx, 0xFF);
		break;
			
		default:
		    return 0;
	}
	return 1;
}

// 屏幕引脚和帧率（用在初始化函数里）  公式化 cv，不求甚解
uint8_t u8g2_gpio_and_delay_stm32(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg, 
                                U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr)
{
    switch(msg){
	    case U8X8_MSG_GPIO_AND_DELAY_INIT:
	        break;
	    case U8X8_MSG_DELAY_MILLI:
		    HAL_Delay(arg_int); // arg_int是I2C速度，4就代表 400KHz
	        break;
        case U8X8_MSG_GPIO_I2C_CLOCK: // arg_int=0: Output low at I2C clock pin
            break;                    // arg_int=1: Input dir with pullup high for I2C clock pin
        case U8X8_MSG_GPIO_I2C_DATA:  // arg_int=0: Output low at I2C data pin
            break;                    // arg_int=1: Input dir with pullup high for I2C data pin
             
        case U8X8_MSG_DELAY_100NANO: // 延迟100纳秒
            // __NOP();
            my_nsDelay(100);
            break;
        case U8X8_MSG_DELAY_10MICRO: // 延迟10微秒
            // for (uint16_t n = 0; n < 320; n++) __NOP();
            my_usDelay(10);
            break;
        case U8X8_MSG_DELAY_I2C: // arg_int is the I2C speed in 100KHz, e.g. 4 = 400 KHz
            my_usDelay(5);
            break;                    // arg_int=1: delay by 5us, arg_int = 4: delay by 1.25us
        case U8X8_MSG_GPIO_MENU_SELECT:
            u8x8_SetGPIOResult(u8x8, /* get menu select pin state */ 0);
            break;
        case U8X8_MSG_GPIO_MENU_NEXT:
            u8x8_SetGPIOResult(u8x8, /* get menu next pin state */ 0);
            break;
        case U8X8_MSG_GPIO_MENU_PREV:
            u8x8_SetGPIOResult(u8x8, /* get menu prev pin state */ 0);
            break;
        case U8X8_MSG_GPIO_MENU_HOME:
            u8x8_SetGPIOResult(u8x8, /* get menu home pin state */ 0);
            break;
	    default:	
		    return 0;
	}
	return 1; // command processed successfully.
}

// 屏幕初始化
void OledUI_Init(OledUI_t* oled_ui)
{

    // U8G2_R0（正常方向） U8G2_R1（顺时针旋转90°） U8G2_R2 U8G2_R3（顺时针旋转180°）
    // U8G2_MIRROR 左右镜像
    // U8G2_MIRROR_VERTICAL 上下镜像
    /* 设置为
    --------------------------> x
    I
    I
    I
    I
    I
    I
    I
   \/ y                               */
    u8g2_Setup_sh1107_i2c_seeed_128x128_f(&oled_ui->u8g2, U8G2_R0, u8x8_byte_hw_i2c, 
                                                u8g2_gpio_and_delay_stm32);
    // u8x8_byte_hw_i2c用来写数据等 
    // u8g2_gpio_and_delay_stm32是时延相关函数
    // OLED 显示初始化
    u8g2_InitDisplay(&oled_ui->u8g2);
    // OLED打开与关闭  0是打开  1是关闭
    u8g2_SetPowerSave(&oled_ui->u8g2, 0);
    u8g2_SetContrast(&oled_ui->u8g2, 0xFF);
    // 清屏函数 （作用 = u8g2_ClearBuffer + u8g2_SendBuffer）
    u8g2_ClearDisplay(&oled_ui->u8g2);
    // 清除缓存区
    u8g2_ClearBuffer(&oled_ui->u8g2);
    // 将缓冲区的数据发送给 OLED
    u8g2_SendBuffer(&oled_ui->u8g2);
}




// 显示初始化
void OledDisplay_Init(void)
{
	
    OledUI_AddPage(&oled_display.oled_ui, OledDisplay_Control_Page);
	OledUI_AddPage(&oled_display.oled_ui, OledDisplay_Adjust_Page);
    OledUI_AddPage(&oled_display.oled_ui, OledDisplay_KFS_Page);
	OledUI_Init(&oled_display.oled_ui);
}

// 显示函数
void OledUI_Display(OledUI_t* oled_ui)
{
    if(oled_ui->pages[oled_ui->page_current] == NULL) return;
    // u8g2_ClearBuffer(&oled_ui->u8g2);
    oled_ui->pages[oled_ui->page_current](); // 展示当前页
    // u8g2_SendBuffer(&oled_ui->u8g2);
}
// 换下一页
void OledUI_SetPageNext(OledUI_t* oled_ui)
{
    oled_ui->page_current = (oled_ui->page_current + 1) % oled_ui->page_count;
}
// 换上一页
void OledUI_SetPagePrev(OledUI_t* oled_ui)
{
    // 要避免出现负数，并实现回环
    oled_ui->page_current = (oled_ui->page_current + oled_ui->page_count - 1) % oled_ui->page_count;
}
// 换成指定页
void OledUI_SetPage(OledUI_t* oled_ui, uint8_t page)
{
    oled_ui->page_current = page % oled_ui->page_count;
}
// 获取当前页
uint8_t OledUI_GetPage(OledUI_t* oled_ui)
{
    return oled_ui->page_current;
}
// 增加页面
void OledUI_AddPage(OledUI_t* oled_ui, OledUI_Pages_t newpage)
{
    if (oled_ui->page_count >= 32)
        return;
    oled_ui->pages[oled_ui->page_count] = newpage;
    oled_ui->page_count++;
}
// 去除页面
void OledUI_RemovePage(OledUI_t* oled_ui, OledUI_Pages_t oldpage)
{
    uint32_t i;
    for (i = 0; i < oled_ui->page_count; i++)
    {
        if (oled_ui->pages[i] == oldpage)
        {
            oled_ui->pages[i] = NULL;
            break;
        }
    }
    for (; i < oled_ui->page_count - 1; i++)
    {
        oled_ui->pages[i] = oled_ui->pages[i + 1];
    }
    oled_ui->page_count--;
}



// 页面实时更新
void OledDisplay_Process(void)
{
    OledUI_Display(&oled_display.oled_ui);
}


