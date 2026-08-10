#include "myiic.h"

// Initialize IIC
void IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    IIC_SCL_PORTCLK_ENABLE();
    IIC_SDA_PORTCLK_ENABLE();

    // Configure SCL and SDA as Open-Drain output
    GPIO_InitStruct.Pin = IIC_SCL_PIN | IIC_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(IIC_SCL_PORT, &GPIO_InitStruct);

    IIC_SCL(1);
    IIC_SDA(1);
}

// Generate IIC Start Signal
void IIC_Start(void)
{
    IIC_SDA(1);
    IIC_SCL(1);
    delay_us(4);
    IIC_SDA(0);
    delay_us(4);
    IIC_SCL(0); // Clamp I2C bus, ready to send or receive data
}

// Generate IIC Stop Signal
void IIC_Stop(void)
{
    IIC_SCL(0);
    IIC_SDA(0);
    delay_us(4);
    IIC_SCL(1);
    IIC_SDA(1); // Send I2C bus stop signal
    delay_us(4);
}

// Wait for ACK
// Return: 1 - NACK, 0 - ACK
uint8_t IIC_Wait_Ack(void)
{
    uint8_t ucErrTime = 0;
    IIC_SDA(1);
    delay_us(1);
    IIC_SCL(1);
    delay_us(1);
    while (IIC_READ_SDA)
    {
        ucErrTime++;
        if (ucErrTime > 250)
        {
            IIC_Stop();
            return 1;
        }
    }
    IIC_SCL(0); // Clock output 0
    return 0;
}

// Generate ACK
void IIC_Ack(void)
{
    IIC_SCL(0);
    IIC_SDA(0);
    delay_us(2);
    IIC_SCL(1);
    delay_us(2);
    IIC_SCL(0);
}

// Generate NACK
void IIC_NAck(void)
{
    IIC_SCL(0);
    IIC_SDA(1);
    delay_us(2);
    IIC_SCL(1);
    delay_us(2);
    IIC_SCL(0);
}

// Send one byte
void IIC_Send_Byte(uint8_t txd)
{
    uint8_t t;
    IIC_SCL(0); // Pull down clock to change data
    for (t = 0; t < 8; t++)
    {
        IIC_SDA((txd & 0x80) >> 7);
        txd <<= 1;
        delay_us(2);   
        IIC_SCL(1);
        delay_us(2);
        IIC_SCL(0);
        delay_us(2);
    }
}

// Read one byte
// ack: 1 - ACK, 0 - NACK
uint8_t IIC_Read_Byte(uint8_t ack)
{
    uint8_t i, receive = 0;
    IIC_SDA(1); // Release SDA
    for (i = 0; i < 8; i++)
    {
        IIC_SCL(0);
        delay_us(2);
        IIC_SCL(1);
        receive <<= 1;
        if (IIC_READ_SDA)
            receive++;
        delay_us(1);
    }
    if (!ack)
        IIC_NAck();
    else
        IIC_Ack();
    return receive;
}

// Write multiple bytes to I2C device
// addr: Device address (7-bit left shifted or 8-bit read/write address, function will assume user passes exact 8-bit address or 7-bit + R/W bit logic handled by caller. Here we assume caller passes the write address directly, e.g. 0x70)
uint8_t IIC_Write_Bytes(uint8_t dev_addr, uint8_t *buf, uint8_t len)
{
    uint8_t i;
    IIC_Start();
    IIC_Send_Byte(dev_addr); 
    if(IIC_Wait_Ack()) {
        IIC_Stop();
        return 1; 
    }
    for(i=0; i<len; i++) {
        IIC_Send_Byte(buf[i]);
        if(IIC_Wait_Ack()) {
            IIC_Stop();
            return 1; 
        }
    }
    IIC_Stop();
    return 0;
}

// Read multiple bytes from I2C device
// addr: Device read address (e.g. 0x71)
uint8_t IIC_Read_Bytes(uint8_t dev_addr, uint8_t *buf, uint8_t len)
{
    uint8_t i;
    IIC_Start();
    IIC_Send_Byte(dev_addr);
    if(IIC_Wait_Ack()) {
        IIC_Stop();
        return 1; 
    }
    for(i=0; i<len; i++) {
        // Send ACK for all bytes except the last one, which gets a NACK
        buf[i] = IIC_Read_Byte(i == (len-1) ? 0 : 1);
    }
    IIC_Stop();
    return 0;
}

