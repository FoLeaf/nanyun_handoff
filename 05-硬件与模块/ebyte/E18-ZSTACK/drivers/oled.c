#include "oled.h"

static void OLED_WriteCmd(unsigned char cmd)
{
    I2C_Start();
    I2C_SendByte(OLED_ADDR);
    I2C_WaitAck();
    I2C_SendByte(0x00);
    I2C_WaitAck();
    I2C_SendByte(cmd);
    I2C_WaitAck();
    I2C_Stop();
}

static void OLED_WriteData(unsigned char dat)
{
    I2C_Start();
    I2C_SendByte(OLED_ADDR);
    I2C_WaitAck();
    I2C_SendByte(0x40);
    I2C_WaitAck();
    I2C_SendByte(dat);
    I2C_WaitAck();
    I2C_Stop();
}

void OLED_SetPos(unsigned char x, unsigned char y)
{
    OLED_WriteCmd(0xb0 + y);
    OLED_WriteCmd(((x & 0xf0) >> 4) | 0x10);
    OLED_WriteCmd(x & 0x0f);
}

void OLED_Clear(void)
{
    unsigned char i, n;
    for (i = 0; i < 8; i++)
    {
        OLED_WriteCmd(0xb0 + i);
        OLED_WriteCmd(0x00);
        OLED_WriteCmd(0x10);
        for (n = 0; n < 128; n++) OLED_WriteData(0);
    }
}

void OLED_Init(void)
{
    I2C_Init();
    
    OLED_WriteCmd(0xAE); // Display Off
    OLED_WriteCmd(0x00); // Set Low Column Address
    OLED_WriteCmd(0x10); // Set High Column Address
    OLED_WriteCmd(0x40); // Set Start Line Address
    OLED_WriteCmd(0x81); // The Contrast Control Mode Set
    OLED_WriteCmd(0xCF); // Set Contrast
    OLED_WriteCmd(0xA1); // Set Segment Re-map
    OLED_WriteCmd(0xC8); // Set COM Output Scan Direction
    OLED_WriteCmd(0xA6); // Normal Display
    OLED_WriteCmd(0xA8); // Multiplex Ratio Set
    OLED_WriteCmd(0x3F);
    OLED_WriteCmd(0xD3); // Set Display Offset
    OLED_WriteCmd(0x00);
    OLED_WriteCmd(0xD5); // Set Display Clock Divide Ratio/Oscillator Frequency
    OLED_WriteCmd(0x80);
    OLED_WriteCmd(0xD9); // Set Pre-charge Period
    OLED_WriteCmd(0xF1);
    OLED_WriteCmd(0xDA); // Set COM Pins Hardware Configuration
    OLED_WriteCmd(0x12);
    OLED_WriteCmd(0xDB); // Set VCOMH Deselect Level
    OLED_WriteCmd(0x40);
    OLED_WriteCmd(0x20); // Set Memory Addressing Mode
    OLED_WriteCmd(0x02); // Page Addressing Mode
    OLED_WriteCmd(0x8D); // Charge Pump Setting
    OLED_WriteCmd(0x14); // Enable Charge Pump
    OLED_WriteCmd(0xA4); // Entire Display On
    OLED_WriteCmd(0xA6); // Set Normal Display
    OLED_WriteCmd(0xAF); // Display On
    
    OLED_Clear();
}

// Minimal placeholder for ShowString (needs font table)
void OLED_ShowString(unsigned char x, unsigned char y, char *str, unsigned char size)
{
    // Implementation depends on your font table.
    // Example: iterate through str, call OLED_SetPos and OLED_WriteData for each char.
}
