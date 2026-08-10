//
// Created by 19y on 2026/4/28.
//

#include "input_capture.h"
#include "tim.h"
#include "callback_manager.h"
#define CAPTURE_FILTER_SIZE 10
uint32_t dma_period_buf[CAPTURE_FILTER_SIZE];
uint32_t dma_pulse_buf[CAPTURE_FILTER_SIZE];

uint32_t capture_freq = 0;
uint8_t  capture_duty = 0;

static void input_capture_ic_cb(TIM_HandleTypeDef *htim);

void input_capture_init(void)
{
    // 【关键】：这里就像是我们拿着自己的证件去管理器那里“挂号登记”。
    // 我们告诉管理器：“你好，我只关心 TIM1 的事件，如果 TIM1 有中断或者 DMA 完成了，请你呼叫我的 input_capture_ic_cb 函数！”
    // 这样，我们的模块就和底层的中断解耦了，不需要直接去占用唯一的 HAL 回调函数名。
    cm_register_tim_ic_callback(TIM1, input_capture_ic_cb);
    
    // 使用 DMA 方式启动输入捕获
    HAL_TIM_IC_Start_DMA(&htim1, TIM_CHANNEL_1, dma_period_buf, CAPTURE_FILTER_SIZE);
    HAL_TIM_IC_Start_DMA(&htim1, TIM_CHANNEL_2, dma_pulse_buf, CAPTURE_FILTER_SIZE);
}

// 这是我们模块私有的处理函数（带 static 表示只在当前文件可用，防止和其他文件命名冲突）
// 因为我们在上面进行了登记，所以当 TIM1 产生捕获事件时，管理器会帮我们自动调用这个函数。
static void input_capture_ic_cb(TIM_HandleTypeDef *htim)
{
    // 确保是 TIM1 的通道 1 触发的 DMA 完成中断
    if (htim->Instance == TIM1 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
    {
        uint32_t period_sum = 0;
        uint32_t pulse_sum = 0;
        
        // 累加计算平均值以实现滤波
        for (int i = 0; i < CAPTURE_FILTER_SIZE; i++)
        {
            period_sum += dma_period_buf[i];
            pulse_sum += dma_pulse_buf[i];
        }
        
        uint32_t period_avg = period_sum / CAPTURE_FILTER_SIZE;
        uint32_t pulse_avg = pulse_sum / CAPTURE_FILTER_SIZE;
        
        if (period_avg != 0)
        {
            // 【动态计算频率】：自动适应系统时钟和定时器预分频变化
            // 1. 获取当前系统的主频(HCLK)和 APB1 总线频率(PCLK1)
            uint32_t pclk = HAL_RCC_GetPCLK1Freq();
            uint32_t hclk = HAL_RCC_GetHCLKFreq();
            
            // 2. 根据 STM32 的硬件设计规则：
            // 如果 APB 预分频系数是 1 (即 pclk == hclk)，那么定时器的基础时钟就等于 pclk；
            // 如果 APB 预分频系数大于 1 (即 pclk != hclk)，那么定时器的基础时钟会自动倍频，等于 pclk * 2。
            uint32_t tim_clk = (pclk == hclk) ? pclk : (pclk * 2);
            
            // 3. 算出定时器计数器真正“嘀嗒”一下的频率 (Hz) = 定时器基础时钟 / (PSC预分频寄存器值 + 1)
            uint32_t timer_tick_freq = tim_clk / (htim->Instance->PSC + 1);
            
            // 4. 计算外设信号频率：等于计数器的嘀嗒频率 除以 走完一个周期需要的嘀嗒数
            capture_freq = timer_tick_freq / period_avg;
            
            // 计算占空比：(高电平占据的嘀嗒数 / 一整个周期的嘀嗒数) * 100
            capture_duty = (pulse_avg * 100) / period_avg;
        }
        else
        {
            capture_freq = 0;
            capture_duty = 0;
        }
    }
}

uint32_t input_capture_get_freq(void)
{
    return capture_freq;
}

uint8_t input_capture_get_duty(void)
{
    return capture_duty;
}