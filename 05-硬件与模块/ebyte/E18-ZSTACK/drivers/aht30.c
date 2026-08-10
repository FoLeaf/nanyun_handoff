#include "aht30.h"

static void AHT30_DelayMs(unsigned int ms)
{
    unsigned int i, j;
    for(i=0; i<ms; i++)
        for(j=0; j<535; j++); // Approx 1ms for CC2530 @ 32MHz
}

unsigned char AHT30_Init(void)
{
    AHT30_DelayMs(100);
    I2C_Start();
    I2C_SendByte(AHT30_ADDR_WRITE);
    if(I2C_WaitAck()) return 0;
    I2C_SendByte(0xBE); // Initial command
    I2C_WaitAck();
    I2C_SendByte(0x08);
    I2C_WaitAck();
    I2C_SendByte(0x00);
    I2C_WaitAck();
    I2C_Stop();
    return 1;
}

unsigned char AHT30_ReadData(AHT30_Data *data)
{
    unsigned char buf[6];
    unsigned long h = 0, t = 0;
    
    I2C_Start();
    I2C_SendByte(AHT30_ADDR_WRITE);
    I2C_WaitAck();
    I2C_SendByte(0xAC); // Trigger measurement
    I2C_WaitAck();
    I2C_SendByte(0x33);
    I2C_WaitAck();
    I2C_SendByte(0x00);
    I2C_WaitAck();
    I2C_Stop();
    
    AHT30_DelayMs(80); // Wait for measurement
    
    I2C_Start();
    I2C_SendByte(AHT30_ADDR_READ);
    I2C_WaitAck();
    buf[0] = I2C_ReadByte(1); // Status
    buf[1] = I2C_ReadByte(1); // Humid [19:12]
    buf[2] = I2C_ReadByte(1); // Humid [11:4]
    buf[3] = I2C_ReadByte(1); // Humid [3:0], Temp [19:16]
    buf[4] = I2C_ReadByte(1); // Temp [15:8]
    buf[5] = I2C_ReadByte(0); // Temp [7:0]
    I2C_Stop();
    
    if((buf[0] & 0x80) == 0) // Busy bit check
    {
        h = ((unsigned long)buf[1] << 12) | ((unsigned long)buf[2] << 4) | (buf[3] >> 4);
        t = ((unsigned long)(buf[3] & 0x0F) << 16) | ((unsigned long)buf[4] << 8) | buf[5];
        
        data->humidity = (float)h * 100.0 / 1048576.0;
        data->temperature = (float)t * 200.0 / 1048576.0 - 50.0;
        return 1;
    }
    return 0;
}
