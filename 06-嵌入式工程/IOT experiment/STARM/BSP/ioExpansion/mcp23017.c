//
// Created by 19y on 2026/4/10.
//

#include "mcp23017.h"
#include "myiic.h"
/*IOA0 - A3 输出
B3-B4 输出
B7输出
雨滴 IOB2 输入
*/
void mcp23017_init(void)
{
    // 确保I2C已初始化
    IIC_Init();

    // 设置I/O方向（1 = 输入，0 = 输出）
    // IODIRA: A0-A3输出(0)，A4-A7输入(1) -> 1111 0000 -> 0xF0
    mcp23017_write_reg(MCP23017_IODIRA, 0xF0);

    // IODIRB: B7,B4,B3 Output (0); B2(RainSensor),B6,B5 Input (1); B1(M1), B0(M0) Output (0) -> 0110 0100 -> 0x64
    mcp23017_write_reg(MCP23017_IODIRB, 0x64);
    
    // 为IOB2（雨传感器）使能上拉电阻（如果需要）（1 = 使能）
    // GPPU B: 0000 0100 -> 0x04
    mcp23017_write_reg(MCP23017_GPPUB, 0x04);
    
    // 将端口A和端口B的默认输出状态设置为0
    mcp23017_write_reg(MCP23017_GPIOA, 0x00);
    mcp23017_write_reg(MCP23017_GPIOB, 0x00);
}

void mcp23017_write_reg(uint8_t reg, uint8_t value)
{
    IIC_Start();
    IIC_Send_Byte(MCP23017_ADDRESS_WRITE);
    IIC_Wait_Ack();
    IIC_Send_Byte(reg);
    IIC_Wait_Ack();
    IIC_Send_Byte(value);
    IIC_Wait_Ack();
    IIC_Stop();
}

uint8_t mcp23017_read_reg(uint8_t reg)
{
    uint8_t val;
    IIC_Start();
    IIC_Send_Byte(MCP23017_ADDRESS_WRITE);
    IIC_Wait_Ack();
    IIC_Send_Byte(reg);
    IIC_Wait_Ack();
    
    IIC_Start();
    IIC_Send_Byte(MCP23017_ADDRESS_READ);
    IIC_Wait_Ack();
    val = IIC_Read_Byte(0); // 0表示NACK（不对最后一个字节发送应答）
    IIC_Stop();
    
    return val;
}