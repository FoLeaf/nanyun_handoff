#include "monitor.h"
#include "oled.h"
#include "voltmeasure.h"
#include "input_capture.h"
#include "rtc.h"
#include "ra01s_test.h"

void monitor_task(void)
{
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};
    
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

    // 1. 清空本地显存（不直接操作硬件，无闪烁）
    OLED_ClearBuffer();

    OLED_Print(0,  4, 6,"V:%.2fV %s", voltmeasure_get_voltage(), RA01S_Test_GetTxStatus());
    OLED_Print(0, 20, 6,"F:%dHz", input_capture_get_freq());
    OLED_Print(0, 36, 6,"D:%d%% %s", input_capture_get_duty(), RA01S_Test_GetRxStatus());
    
    // 时间放在最底下
    OLED_Print(0, 52, 6,"T:%02d:%02d:%02d", sTime.Hours, sTime.Minutes, sTime.Seconds);

    // 3. 一次性将整个画面推送到屏幕上
    OLED_Refresh();
}

