/*********************************************************************
 * app_led.h - E18 模块 LED 驱动
 *
 * P1.3 = RUN_LED（运行/网络状态，低电平有效）
 * P1.2 = NWK_LED（网络操作指示，低电平有效）
 *********************************************************************/

#ifndef APP_LED_H
#define APP_LED_H

#include "ZComDef.h"

// LED 状态定义
#define APP_LED_OFF         0
#define APP_LED_ON          1
#define APP_LED_BLINK_FAST  2   // 10Hz
#define APP_LED_FLASH_ONCE  3   // 闪一下（100ms 亮后自动灭）

// LED 标识
#define APP_LED_RUN         0   // P1.3
#define APP_LED_NWK         1   // P1.2

void AppLed_Init(void);
void AppLed_Set(uint8 led, uint8 mode);
void AppLed_Process(void);  // 在事件循环中调用，处理闪烁

#endif /* APP_LED_H */
