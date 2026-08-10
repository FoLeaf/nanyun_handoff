/*********************************************************************
 * app_led.c - E18 模块 LED 驱动实现
 *********************************************************************/

#include "app_led.h"
#include "OSAL.h"
#include "OnBoard.h"

// LED GPIO 定义
#define RUN_LED_PIN    BV(3)  // P1.3
#define NWK_LED_PIN    BV(2)  // P1.2

// LED 状态结构
typedef struct {
  uint8 mode;
  uint8 state;        // 当前亮灭状态
  uint16 timer;       // 闪烁计时器
  uint16 period;      // 闪烁周期
} led_state_t;

static led_state_t run_led = {0};
static led_state_t nwk_led = {0};

/*********************************************************************
 * @fn      AppLed_Init
 *
 * @brief   初始化 LED GPIO
 */
void AppLed_Init(void)
{
  // P1.2 和 P1.3 作为输出
  P1DIR |= (RUN_LED_PIN | NWK_LED_PIN);

  // 初始关闭（高电平 = 灭）
  P1 |= (RUN_LED_PIN | NWK_LED_PIN);

  run_led.mode = APP_LED_OFF;
  nwk_led.mode = APP_LED_OFF;
}

/*********************************************************************
 * @fn      led_set_output
 *
 * @brief   设置 LED 输出（低电平有效）
 */
static void led_set_output(uint8 pin, uint8 on)
{
  if (on) {
    P1 &= ~pin;  // 低电平 = 亮
  } else {
    P1 |= pin;   // 高电平 = 灭
  }
}

/*********************************************************************
 * @fn      AppLed_Set
 *
 * @brief   设置 LED 模式
 */
void AppLed_Set(uint8 led, uint8 mode)
{
  led_state_t *p = (led == APP_LED_RUN) ? &run_led : &nwk_led;
  uint8 pin = (led == APP_LED_RUN) ? RUN_LED_PIN : NWK_LED_PIN;

  p->mode = mode;
  p->timer = 0;

  switch (mode) {
    case APP_LED_OFF:
      led_set_output(pin, 0);
      p->state = 0;
      break;
    case APP_LED_ON:
      led_set_output(pin, 1);
      p->state = 1;
      break;
    case APP_LED_BLINK_FAST:
      // 10Hz = 50ms on, 50ms off
      p->period = 50;
      break;
    case APP_LED_FLASH_ONCE:
      // 闪一下：亮 100ms 后自动灭
      led_set_output(pin, 1);
      p->state = 1;
      p->timer = 0;
      break;
  }
}

/*********************************************************************
 * @fn      AppLed_Process
 *
 * @brief   处理 LED 闪烁（应在事件循环中每 10ms 调用）
 */
void AppLed_Process(void)
{
  led_state_t *leds[2] = {&run_led, &nwk_led};
  uint8 pins[2] = {RUN_LED_PIN, NWK_LED_PIN};
  int i;

  for (i = 0; i < 2; i++) {
    led_state_t *p = leds[i];

    if (p->mode == APP_LED_BLINK_FAST) {
      p->timer += 10;  // 假设每 10ms 调用一次

      if (p->timer >= p->period) {
        p->timer = 0;
        p->state = !p->state;
        led_set_output(pins[i], p->state);
      }
    }
    else if (p->mode == APP_LED_FLASH_ONCE) {
      if (p->state) {
        p->timer += 10;
        if (p->timer >= 100) {
          // 100ms 后关闭并切换到 OFF 模式
          led_set_output(pins[i], 0);
          p->state = 0;
          p->mode = APP_LED_OFF;
        }
      }
    }
  }
}
