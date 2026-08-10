#ifndef __ULTRASONIC_H
#define __ULTRASONIC_H

#include "main.h"

/*
 * 超声波模块引脚定义
 * 用户可根据实际连线修改这里的宏定义
 * 默认使用 PA2 作为 TRIG (触发), PA3 作为 ECHO (接收)
 */
#define ULTRASONIC_TRIG_PORT GPIOA
#define ULTRASONIC_TRIG_PIN GPIO_PIN_2

#define ULTRASONIC_ECHO_PORT GPIOA
#define ULTRASONIC_ECHO_PIN GPIO_PIN_3

/* 引脚电平控制和读取宏 */
#define ULTRASONIC_TRIG_HIGH()                                                 \
  HAL_GPIO_WritePin(ULTRASONIC_TRIG_PORT, ULTRASONIC_TRIG_PIN, GPIO_PIN_SET)
#define ULTRASONIC_TRIG_LOW()                                                  \
  HAL_GPIO_WritePin(ULTRASONIC_TRIG_PORT, ULTRASONIC_TRIG_PIN, GPIO_PIN_RESET)
#define ULTRASONIC_ECHO_READ()                                                 \
  HAL_GPIO_ReadPin(ULTRASONIC_ECHO_PORT, ULTRASONIC_ECHO_PIN)

/* 函数声明 */
void Ultrasonic_Init(void);
float Ultrasonic_GetDistance(void);

#endif /* __ULTRASONIC_H */
