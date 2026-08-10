#include "aht30_test.h"
#include "aht30.h"
#include "myiic.h"
#include "oled.h"
#include "delay.h"
#include <stdio.h>

static AHT30_HandleTypeDef aht30_dev;

// Wrapper for IIC write
static uint8_t AHT30_HW_Write(uint8_t addr, uint8_t *buf, uint8_t len)
{
    // The AHT30 driver passes 0x70 for write, we can use the updated myiic directly
    return IIC_Write_Bytes(addr, buf, len);
}

// Wrapper for IIC read
static uint8_t AHT30_HW_Read(uint8_t addr, uint8_t *buf, uint8_t len)
{
    // The AHT30 driver passes 0x71 for read
    return IIC_Read_Bytes(addr, buf, len);
}

// Wrapper for delay
static void AHT30_HW_Delay(uint32_t ms)
{
    delay_ms(ms);
}

void AHT30_Test_Init(void)
{
    // Init hardware IIC and OLED (assuming they are initialized elsewhere, but just in case)
    // IIC_Init(); 
    // OLED_Init();
    
    // Setup AHT30 Handle
    aht30_dev.hw.I2C_Write_Bytes = AHT30_HW_Write;
    aht30_dev.hw.I2C_Read_Bytes = AHT30_HW_Read;
    aht30_dev.hw.Delay_ms = AHT30_HW_Delay;
    
    OLED_ShowSTR(0, 0, "AHT30 Init...", 8);
    
    uint8_t ret = AHT30_Init(&aht30_dev);
    if (ret == 0) {
        OLED_ShowSTR(0, 2, "Init Success! ", 8);
    } else {
        OLED_ShowSTR(0, 2, "Init Failed!  ", 8);
    }
    delay_ms(1000);
    OLED_Clear();
}

void AHT30_Test_Run(void)
{
    char buf[20];
    uint8_t ret = AHT30_ReadMeasure(&aht30_dev);
    
    if (ret == 0) {
        // We use %f, requires -u _printf_float in CMake linker flags for LLVM/GCC
        sprintf(buf, "Temp: %.2f C ", aht30_dev.temperature);
        OLED_ShowSTR(0, 0, buf, 8);
        
        sprintf(buf, "Humi: %.2f %% ", aht30_dev.humidity);
        OLED_ShowSTR(0, 2, buf, 8);
    } else {
        sprintf(buf, "Read Err: %d  ", ret);
        OLED_ShowSTR(0, 0, buf, 8);
    }
}
