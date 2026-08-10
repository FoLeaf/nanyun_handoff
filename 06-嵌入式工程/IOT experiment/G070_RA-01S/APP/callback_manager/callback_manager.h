#ifndef G070_RA_01S_CALLBACK_MANAGER_H
#define G070_RA_01S_CALLBACK_MANAGER_H
#include "main.h"

// 定义 TIM 输入捕获回调函数指针类型
// 【通俗解释】：这就像是定义了一种“统一规格的插座”。
// tim_ic_callback_t 代表一个“接收定时器句柄参数、且无返回值”的函数。
// 以后任何想处理定时器捕获中断的模块，都要写一个符合这个规格的函数。
typedef void (*tim_ic_callback_t)(TIM_HandleTypeDef *htim);

// 注册 TIM 输入捕获回调函数
// 【通俗解释】：这个函数相当于一个“登记处”。
// 其他模块（如 input_capture.c）调用这个函数，把自己需要监听的定时器（比如 TIM1）
// 以及自己写好的处理函数（cb）告诉管理器，管理器会把它们记录下来。
void cm_register_tim_ic_callback(TIM_TypeDef *instance, tim_ic_callback_t cb);

#endif //G070_RA_01S_CALLBACK_MANAGER_H
