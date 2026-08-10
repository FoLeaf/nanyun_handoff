#include "ultrasonic.h"
#include "delay.h"

/**
 * @brief  初始化超声波模块对应的 GPIO
 * @param  None
 * @retval None
 * @note   使用默认引脚定义 PA2(TRIG), PA3(ECHO)。可配置为其他引脚。
 */
void Ultrasonic_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* 开启引脚对应端口的时钟（涵盖常见端口配置） */
  if (ULTRASONIC_TRIG_PORT == GPIOA || ULTRASONIC_ECHO_PORT == GPIOA)
    __HAL_RCC_GPIOA_CLK_ENABLE();

  if (ULTRASONIC_TRIG_PORT == GPIOB || ULTRASONIC_ECHO_PORT == GPIOB)
    __HAL_RCC_GPIOB_CLK_ENABLE();

  if (ULTRASONIC_TRIG_PORT == GPIOC || ULTRASONIC_ECHO_PORT == GPIOC)
    __HAL_RCC_GPIOC_CLK_ENABLE();

  if (ULTRASONIC_TRIG_PORT == GPIOD || ULTRASONIC_ECHO_PORT == GPIOD)
    __HAL_RCC_GPIOD_CLK_ENABLE();

  /* 配置 TRIG 触发引脚为推挽输出 */
  GPIO_InitStruct.Pin = ULTRASONIC_TRIG_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(ULTRASONIC_TRIG_PORT, &GPIO_InitStruct);

  /* 配置 ECHO 接收引脚为浮空/下拉输入 */
  GPIO_InitStruct.Pin = ULTRASONIC_ECHO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(ULTRASONIC_ECHO_PORT, &GPIO_InitStruct);

  /* 默认拉低触发引脚 */
  ULTRASONIC_TRIG_LOW();
}

/**
 * @brief  获取超声波测量距离
 * @param  None
 * @retval float 测得的距离（单位：cm）。返回 -1.0 表示测量超时或错误。
 */
float Ultrasonic_GetDistance(void) {
  uint32_t wait_timeout = 0;
  uint32_t echo_time = 0;
  float distance = 0.0f;

  /* 1. 给 TRIG 引脚发送 >= 10us 的高电平脉冲启动测距 */
  ULTRASONIC_TRIG_LOW();
  delay_us(2);
  ULTRASONIC_TRIG_HIGH();
  delay_us(15); /* 保证高电平持续10us以上 */
  ULTRASONIC_TRIG_LOW();

  /* 2. 等待 ECHO 引脚变为高电平（遇到高电平开始计时） */
  wait_timeout = 0;
  while (ULTRASONIC_ECHO_READ() == GPIO_PIN_RESET) {
    delay_us(1);
    wait_timeout++;
    if (wait_timeout > 100000) {
      /* 等待高电平超时(超100ms) */
      return -1.0f;
    }
  }

  /* 3. 测量 ECHO 高电平持续的时间 (us) */
  echo_time = 0;
  while (ULTRASONIC_ECHO_READ() == GPIO_PIN_SET) {
    delay_us(1);
    echo_time++;
    /* 最大测距范围(一般为4-5米)，大概10000~20000us，设置35000us为超时比较合理
     */
    if (echo_time > 35000) {
      /* 超时返回超出范围 */
      return -1.0f;
    }
  }

  /*
   * 4. 计算距离
   * 根据声速: 340m/s = 0.034cm/us = 34cm/ms
   * 测试距离 = (高电平时间(us) * 0.034cm/us) / 2
   * 简化后等价于: distance(cm) = echo_time / 58.0f
   */
  distance = (float)echo_time * 0.034f / 2.0f;

  return distance;
}
