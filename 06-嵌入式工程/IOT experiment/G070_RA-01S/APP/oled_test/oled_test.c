#include "oled_test.h"
#include "oled.h"
#include "delay.h"

/**
 * @brief OLED 刷屏与显示测试任务
 * @note  调用此函数前需要确保系统时钟已经初始化。
 *        它会展示各种字符、字符串、数字，并做一个简单的计数刷新测试。
 */
void OLED_Test_Run(void)
{
    uint32_t count = 0;

    /* 1. 初始化 OLED */
    OLED_Init();

    /* 2. 静态文本显示测试 */
    OLED_Clear();
    OLED_ShowSTR(0, 0, "OLED Test OK!", 6);   // 使用6x8小字体
    OLED_ShowSTR(0, 2, "STM32G070", 8);       // 使用8x16大字体
    
    // 显示汉字(需确保oledFont中包含对应汉字字模, 如果没有汉字字模则忽略或者替换)
    // 根据oledFont.c的内容，这里为了通用性不直接调用汉字，防止字库缺失报错
    
    delay_ms(2000);

    /* 3. 动态刷屏计数测试 */
    OLED_Clear();
    OLED_ShowSTR(0, 0, "Refresh Test", 8);
    OLED_ShowSTR(0, 2, "Count:", 8);
    OLED_ShowSTR(0, 4, "Speed: Max", 8);

    while(1)
    {
        // 动态刷新数字，测试刷新率和是否存在撕裂感
        OLED_ShowNUM(48, 2, count, 5, 8);
        count++;
        
        // 可选：加一点延时方便肉眼看清数字，如果不加延时数字会跳得非常快
        // delay_ms(10);
    }
}
