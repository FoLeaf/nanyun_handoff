#include "aht30_port.h"
#include "myiic.h"
#include "delay.h"

static uint8_t g_port_inited = 0;

static uint8_t AHT30_I2C_Write(uint8_t addr, uint8_t *buf, uint8_t len)
{

    g_iic_nop_count = 5;
    IIC_Start();
    IIC_Send_Byte(addr);
    if (IIC_Wait_Ack())
    {
        IIC_Stop();
        g_iic_nop_count = 1;
        return 1;
    }
    for (uint8_t i = 0; i < len; i++)
    {
        IIC_Send_Byte(buf[i]);
        if (IIC_Wait_Ack())
        {
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
    g_iic_nop_count = 5;
    IIC_Start();
    IIC_Send_Byte(addr);
    if (IIC_Wait_Ack())
    {
        IIC_Stop();
        g_iic_nop_count = 1;
        return 1;
    }
    for (uint8_t i = 0; i < len; i++)
    {
        buf[i] = IIC_Read_Byte((i == (uint8_t)(len - 1)) ? 0 : 1);
    }
    IIC_Stop();
    g_iic_nop_count = 1;
    return 0;
}

static void AHT30_Delay_ms(uint32_t ms)
{
    delay_ms(ms);
}

void AHT30_Port_Init(void)
{
    if (!g_port_inited)
    {
        IIC_Init();
        g_port_inited = 1;
    }
}

void AHT30_Port_Bind(AHT30_HandleTypeDef *dev)
{
    if (dev == NULL)
    {
        return;
    }

    dev->hw.I2C_Write_Bytes = AHT30_I2C_Write;
    dev->hw.I2C_Read_Bytes = AHT30_I2C_Read;
    dev->hw.Delay_ms = AHT30_Delay_ms;
}
