#include "aht30_test.h"
#include "aht30.h"
#include "myiic.h"
#include "delay.h"
#include "oled.h"
#include <stdio.h>

static uint8_t AHT30_I2C_Write(uint8_t addr, uint8_t *buf, uint8_t len)
{
    g_iic_nop_count = 3;
    IIC_Start();
    IIC_Send_Byte(addr); 
    if (IIC_Wait_Ack()) {
        IIC_Stop();
        g_iic_nop_count = 1;
        return 1;
    }
    for (uint8_t i = 0; i < len; i++) {
        IIC_Send_Byte(buf[i]);
        if (IIC_Wait_Ack()) {
            IIC_Stop();
            g_iic_nop_count = 1;
            return 1;
        }
    }
    IIC_Stop();
    g_iic_nop_count = 1;
    return 0;
}

static uint8_t AHT30_I2C_Read(uint8_t addr, uint8_t *buf, uint8_t len)
{
    g_iic_nop_count = 3;
    IIC_Start();
    IIC_Send_Byte(addr); 
    if (IIC_Wait_Ack()) {
        IIC_Stop();
        g_iic_nop_count = 1;
        return 1;
    }
    for (uint8_t i = 0; i < len; i++) {
        // 当读到最后一个字节时，发送 NACK (0)，否则发送 ACK (1)
        buf[i] = IIC_Read_Byte((i == len - 1) ? 0 : 1);
    }
    IIC_Stop();
    g_iic_nop_count = 1;
    return 0;
}

/* Delay Wrapper */
static void AHT30_Delay_ms(uint32_t ms)
{
    delay_ms(ms);
}

void AHT30_Test_Run(void)
{
    AHT30_HandleTypeDef aht30;
    char str_buf[32];

    // 初始化 OLED 和 I2C (OLED_Init 内部已调用 IIC_Init)
    OLED_Init();
    OLED_Clear();
    OLED_ShowSTR(0, 0, "AHT30 Init...", 8);

    // 绑定硬件接口
    aht30.hw.I2C_Write_Bytes = AHT30_I2C_Write;
    aht30.hw.I2C_Read_Bytes = AHT30_I2C_Read;
    aht30.hw.Delay_ms = AHT30_Delay_ms;

    // 初始化 AHT30
    if (AHT30_Init(&aht30) == 0) {
        OLED_ShowSTR(0, 2, "AHT30 OK!", 8);
    } else {
        OLED_ShowSTR(0, 2, "AHT30 FAIL!", 8);
        while(1); // 初始化失败，死机
    }

    delay_ms(1000);
    OLED_Clear();
    OLED_ShowSTR(0, 0, "AHT30 Sensor", 8);

    while(1)
    {
        // 读取温湿度
        uint8_t res = AHT30_ReadMeasure(&aht30);
        if (res == 0) 
        {
            // 温度：支持负数，格式化保留一位小数
            sprintf(str_buf, "Temp: %.1f C  ", aht30.temperature);
            OLED_ShowSTR(0, 2, str_buf, 8);
            
            // 湿度：格式化保留一位小数
            sprintf(str_buf, "Humi: %.1f %%  ", aht30.humidity);
            OLED_ShowSTR(0, 4, str_buf, 8);
        }
        else
        {
            sprintf(str_buf, "Read Err: %d  ", res);
            OLED_ShowSTR(0, 6, str_buf, 8);
        }
        
        // AHT30 读取间隔建议 >= 100ms
        delay_ms(500); 
    }
}
