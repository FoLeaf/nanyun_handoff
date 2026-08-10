//
// Created by 19y on 2026/4/28.
//

#include "callback_manager.h"
#include "adc.h"

// 定义管理器最多能同时记住多少个定时器的回调（因为定时器数量有限，设为4一般够用了）
#define MAX_TIM_CALLBACKS 4

// 定义一个结构体，用来存放一条“登记信息”
typedef struct {
    TIM_TypeDef *instance; // 是哪个定时器？比如 TIM1
    tim_ic_callback_t cb;  // 对应的处理函数是哪个？
} tim_ic_cb_record_t;

// 创建一个数组（类似于一个电话本），用来记录所有向管家（Manager）注册过的定时器和处理函数
static tim_ic_cb_record_t tim_ic_callbacks[MAX_TIM_CALLBACKS] = {0};
// 记录当前电话本里存了多少条记录
static uint8_t tim_ic_cb_count = 0;

/**
 * @brief 允许其他模块（如 input_capture.c）把自己和定时器绑定并“登记”到这里的电话本上
 */
void cm_register_tim_ic_callback(TIM_TypeDef *instance, tim_ic_callback_t cb)
{
    // 如果电话本还没满
    if (tim_ic_cb_count < MAX_TIM_CALLBACKS) {
        // 在电话本的最后一行，存下定时器实例（例如 TIM1）
        tim_ic_callbacks[tim_ic_cb_count].instance = instance;
        // 把传进来的处理函数存下来
        tim_ic_callbacks[tim_ic_cb_count].cb = cb;
        // 记录数加 1
        tim_ic_cb_count++;
    }
}

/**
 * @brief 统一接管 HAL 库的定时器输入捕获回调
 * 
 * 【总调度员】：当任何一个定时器的输入捕获触发中断或 DMA 完成时，
 * HAL 库都会统一调用这个函数。此时，本函数就像一个“总调度员”，
 * 去查电话本，找到是谁登记了这个定时器，然后通知它。
 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    // 调度员开始翻电话本里的所有记录
    for (int i = 0; i < tim_ic_cb_count; i++) {
        // 如果当前触发中断的定时器（htim->Instance）正好是电话本里某条记录登记的定时器
        if (tim_ic_callbacks[i].instance == htim->Instance) {
            // 确保当时登记的函数不是空的
            if (tim_ic_callbacks[i].cb != NULL) {
                // 呼叫（执行）那个模块自己定义的处理函数，并把 htim 传给它
                tim_ic_callbacks[i].cb(htim);
            }
        }
    }
}
