#include "Config.h"




// 摇杆------------------------------------------------------------
AdcBsp_t adcBsp_adc1 =
{   
    .hadc = &hadc1, .noc = 4, // noc为该 ADC通道的 rank总数
};

Rocker_t LeftRocker =
{
    // 原理图上 x和 y标反了，故此处 x和 y的rank反过来用
    .x = 
    {
        .analog = {.adc_bsp = &adcBsp_adc1, .rank = 1},
        .offset = -160,
        .sign = 1,
        .map_positive = 1.080,
        .map_negative = 1.030,
        
    },
    .y = 
    {
        .analog = {.adc_bsp = &adcBsp_adc1, .rank = 2}, 
        .offset = -210, 
        .sign = 0, 
        .map_positive = 1.000,
        .map_negative = 1.110, 
        
    },
    .deadzone = 7,
};
Rocker_t RightRocker =
{
    // 原理图上 x和 y标反了，故此处 x和 y的rank反过来用
    .x = 
    {
        .analog = {.adc_bsp = &adcBsp_adc1, .rank = 4},
        .offset = -140,
        .sign = 1,
        .map_positive = 1.070,
        .map_negative = 0.960,
        
    },
    .y = 
    {
        .analog = {.adc_bsp = &adcBsp_adc1, .rank = 3}, 
        .offset = -30, 
        .sign = 0, 
        .map_positive = 1.060,
        .map_negative = 1.050, 
        
    },
    .deadzone = 6,
};

// zigbee-----------------------------------------------------------------------
zigbee_msg_t zigbee_msg;
zigbee_state_t zigbee_state = 
{
    .huart = &huart4,
    .rx_completed = 1,
};
struct zigbee_conf_t zigbee_conf;
uint8_t zigbee_txBuffer[100];
uint8_t zigbee_rxBuffer[100];
// 遥控指令：标识、左 x、左 y、右 x、右 y
// 主机接收方zigbee_rxBuffer[4] = 0xCC
uint8_t Control_txData[5] = {CONTROL}; 
// 调参指令：标识、目标(1~5)、基数、倍率、增减
// 主机接收方zigbee_rxBuffer[4] = 0xAA
uint8_t Adjust_txData[5] = {ADJUST}; 
uint8_t Adjust_rxData[5];
// 梅林信息
uint8_t Forest_txData[13] = {RED_ZONE}; 
uint8_t Forest_rxData[5];
// 按键
uint8_t Key_txData[2] = {KEY};
uint8_t Key_rxData[2];
float addition = 1.00;
extern DMA_HandleTypeDef hdma_uart4_rx; // DMA通道的指针地址


// 显示屏---------------------------------------------------------------------------------
OledDisplay_t oled_display;
static I2C_HandleTypeDef* oled_hi2c = &hi2c1;


// 按键-------------------------------------------------------------------------------
FunctionKey_t FunctionKey[KEY_NUM] = 
{
    {.gpioI = {.port = GPIOC, .pin = GPIO_PIN_15, .is_inversed = PULL_UP,}},
    {.gpioI = {.port = GPIOC, .pin = GPIO_PIN_14, .is_inversed = PULL_UP,}},
    {.gpioI = {.port = GPIOF, .pin = GPIO_PIN_2, .is_inversed = PULL_UP,}},
    {.gpioI = {.port = GPIOF, .pin = GPIO_PIN_0, .is_inversed = PULL_UP,}},
    {.gpioI = {.port = GPIOF, .pin = GPIO_PIN_4, .is_inversed = PULL_UP,}},
    {.gpioI = {.port = GPIOF, .pin = GPIO_PIN_3, .is_inversed = PULL_UP,}},
    {.gpioI = {.port = GPIOA, .pin = GPIO_PIN_11, .is_inversed = PULL_UP,}},
    {.gpioI = {.port = GPIOA, .pin = GPIO_PIN_10, .is_inversed = PULL_UP,}},
    {.gpioI = {.port = GPIOA, .pin = GPIO_PIN_9, .is_inversed = PULL_UP,}},
    {.gpioI = {.port = GPIOA, .pin = GPIO_PIN_8, .is_inversed = PULL_UP,}},
    {.gpioI = {.port = GPIOG, .pin = GPIO_PIN_7, .is_inversed = PULL_UP,}},
    {.gpioI = {.port = GPIOG, .pin = GPIO_PIN_8, .is_inversed = PULL_UP,}},
    {.gpioI = {.port = GPIOE, .pin = GPIO_PIN_1, .is_inversed = PULL_UP,}},
    {.gpioI = {.port = GPIOC, .pin = GPIO_PIN_12, .is_inversed = PULL_UP,}},
    {.gpioI = {.port = GPIOC, .pin = GPIO_PIN_11, .is_inversed = PULL_UP,}},
};
functionket_click_t functionket_click[KEY_NUM] =
{   // 要按顺序来
    click_PC15, // 1
    click_PC14, // 2
    click_PF2, // 3
    click_PF0, // 4
    click_PF4, // 5
    click_PF3, // 6
    click_PA11, // 7
    click_PA10, // 8
    click_PA9, // 9
    click_PA8, // 10
    click_PG7, // 11
    click_PG8, // 12
    NULL,
    click_PC12,
    click_PC11,
};
functionket_multi_t functionket_multi[KEY_NUM] =
{   // 要按顺序来
    multi_PC15, // 1
    multi_PC14, // 2
    NULL, // 3
    NULL, // 4
    NULL, // 5
    NULL, // 6
    NULL, // 7
    NULL, // 8
    NULL, // 9
    NULL, // 10
    NULL, // 11
    NULL, // 12
    NULL,
    NULL,
    NULL,
};








// 初始化函数-------------------------------------------------------
void Config_Init(void)
{

    // 延时函数初始化
    DWT_Delay_Init();

    // 按键初始化
    for(int i = 0; i < KEY_NUM; i++)
    {
        FunctionKey_Init(&FunctionKey[i]);
        FunctionKey_SetClickEvent(&FunctionKey[i], functionket_click[i]);
        FunctionKey_SetMultiEvent(&FunctionKey[i], functionket_multi[i]);
    }

    // 摇杆初始化
    AdcBsp_Init(&adcBsp_adc1);
    Rocker_Init(&LeftRocker);
    Rocker_Init(&RightRocker);

    // OLED初始化
    OledUI_Init(&oled_display.oled_ui);
    OledDisplay_Init();

    // zigbee初始化
    // 开启接收中断，由于数据不定长，故使用 DMA中断
	HAL_UARTEx_ReceiveToIdle_DMA(zigbee_state.huart, zigbee_rxBuffer, 100);	
    // 关闭 DMA传输过半中断，防止数据丢失
    __HAL_DMA_DISABLE_IT(&hdma_uart4_rx, DMA_IT_HT);
    // 更改地址和工作模式后必须重新上电才有效
    zigbee_init(R1_GAMEPAD, R1_ROBOT, MASTER);
}

