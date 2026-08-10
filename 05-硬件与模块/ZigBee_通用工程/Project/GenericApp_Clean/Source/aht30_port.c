#include "aht30_port.h"
#include "myiic.h"

static uint8 g_port_inited = 0;

static void delay_us(uint16 us)
{
  while (us--)
  {
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop");
  }
}

static void delay_ms(uint16 ms)
{
  while (ms--)
  {
    delay_us(1000);
  }
}

static uint8 AHT30_I2C_Write(uint8 addr, uint8 *buf, uint8 len)
{
    IIC_Start();
    IIC_Send_Byte(addr);
    if (IIC_Wait_Ack())
    {
        IIC_Stop();
        return 1;
    }
    {
        uint8 i;
        for (i = 0; i < len; i++)
        {
            IIC_Send_Byte(buf[i]);
            if (IIC_Wait_Ack())
            {
                IIC_Stop();
                return 1;
            }
        }
    }
    IIC_Stop();
    return 0;
}

static uint8 AHT30_I2C_Read(uint8 addr, uint8 *buf, uint8 len)
{
    IIC_Start();
    IIC_Send_Byte(addr);
    if (IIC_Wait_Ack())
    {
        IIC_Stop();
        return 1;
    }
    {
        uint8 i;
        for (i = 0; i < len; i++)
        {
            buf[i] = IIC_Read_Byte((i == (uint8)(len - 1)) ? 0 : 1);
        }
    }
    IIC_Stop();
    return 0;
}

static void AHT30_Delay_ms(uint32 ms)
{
    delay_ms((uint16)ms);
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
