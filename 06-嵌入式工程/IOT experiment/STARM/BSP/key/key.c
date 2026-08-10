/**
 ******************************************************************************
 * @file    key.c
 * @brief   按键驱动模块实现
 *          支持9个按键的状态扫描和消抖处理
 * @author  19y
 * @date    2026/4/10
 ******************************************************************************
 */

#include "key.h"

/**
 * @brief 按键信息结构体
 */
typedef struct {
    GPIO_TypeDef* port;      /**< GPIO端口 */
    uint16_t pin;            /**< GPIO引脚 */
    uint8_t count;           /**< 消抖计数器 */
    key_state_t state;       /**< 按键当前状态 */
} key_info_t;

/**
 * @brief 按键配置表
 * @note  索引0为KEY_NONE占位，实际按键从索引1开始
 */
static key_info_t keys[KEY_MAX] = {
    {NULL, 0, 0, KEY_UP},     /**< KEY_NONE - 占位符 */
    {GPIOB, GPIO_PIN_0,  0, KEY_UP}, /**< SW1 - PB0 */
    {GPIOB, GPIO_PIN_1,  0, KEY_UP}, /**< SW2 - PB1 */
    {GPIOB, GPIO_PIN_5,  0, KEY_UP}, /**< SW3 - PB5 */
    {GPIOB, GPIO_PIN_6,  0, KEY_UP}, /**< SW4 - PB6 */
    {GPIOB, GPIO_PIN_7,  0, KEY_UP}, /**< SW5 - PB7 */
    {GPIOB, GPIO_PIN_12, 0, KEY_UP}, /**< SW6 - PB12 */
    {GPIOB, GPIO_PIN_14, 0, KEY_UP}, /**< SW7 - PB14 */
    {GPIOA, GPIO_PIN_11, 0, KEY_UP}, /**< SW8 - PA11 */
    {GPIOA, GPIO_PIN_12, 0, KEY_UP}, /**< SW9 - PA12 */
};

/**
 ******************************************************************************
 * @brief  按键GPIO初始化
 * @note   配置所有按键引脚为上拉输入模式
 * @retval None
 ******************************************************************************
 */
void key_init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    /* 初始化Port B的按键引脚: PB0, PB1, PB5, PB6, PB7, PB12, PB14 */
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_5 | GPIO_PIN_6 | 
                          GPIO_PIN_7 | GPIO_PIN_12 | GPIO_PIN_14;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* 初始化Port A的按键引脚: PA11, PA12 */
    GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/**
 ******************************************************************************
 * @brief  按键扫描函数（含软件消抖）
 * @note   需要周期性调用，建议每10ms调用一次
 *         消抖逻辑：连续2次检测到低电平判定为按下，
 *         连续3次检测到高电平判定为释放
 * @retval None
 ******************************************************************************
 */
void key_scan(void)
{
    for (int i = SW1; i < KEY_MAX; i++)
    {
        if (HAL_GPIO_ReadPin(keys[i].port, keys[i].pin) == GPIO_PIN_RESET)
        {
            if (keys[i].count < 3) keys[i].count++;
            if (keys[i].count >= 2) keys[i].state = KEY_DOWN;
        }
        else
        {
            if (keys[i].count > 0) keys[i].count--;
            if (keys[i].count == 0) keys[i].state = KEY_UP;
        }
    }
}

/**
 ******************************************************************************
 * @brief  获取指定按键的状态
 * @param  key_id: 按键ID，取值范围为key_id_t枚举类型
 * @retval 按键状态，返回key_state_t枚举值
 *         - KEY_UP: 按键未按下
 *         - KEY_DOWN: 按键已按下
 * @note   如果传入无效的key_id，默认返回KEY_UP
 ******************************************************************************
 */
key_state_t key_get_state(key_id_t key_id)
{
    if (key_id >= KEY_MAX) return KEY_UP;
    return keys[key_id].state;
}


