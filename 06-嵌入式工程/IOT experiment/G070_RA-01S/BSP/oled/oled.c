#include "oled.h"
#include <stdio.h>

// 全局显存 8页 * 128列
static uint8_t OLED_GRAM[8][128];

void OLED_WriteCMD(uint8_t cmd)
{
    IIC_Start();
    IIC_Send_Byte(0x78);
    IIC_Wait_Ack();
    IIC_Send_Byte(0x00);
    IIC_Wait_Ack();
    IIC_Send_Byte(cmd);
    IIC_Wait_Ack();
    IIC_Stop();
}

void OLED_WriteData(uint8_t data)
{
    IIC_Start();
    IIC_Send_Byte(0x78);
    IIC_Wait_Ack();
    IIC_Send_Byte(0x40);
    IIC_Wait_Ack();
    IIC_Send_Byte(data);
    IIC_Wait_Ack();
    IIC_Stop();
}

void OLED_Refresh(void)
{
    for(uint8_t i = 0; i < 8; i++)
    {
        OLED_WriteCMD(0xb0 + i);
        OLED_WriteCMD(0x00);
        OLED_WriteCMD(0x10);
        IIC_Start();
        IIC_Send_Byte(0x78);
        IIC_Wait_Ack();
        IIC_Send_Byte(0x40);
        IIC_Wait_Ack();
        for(uint8_t j = 0; j < 128; j++)
        {
            IIC_Send_Byte(OLED_GRAM[i][j]);
            IIC_Wait_Ack();
        }
        IIC_Stop();
    }
}

void OLED_ClearBuffer(void)
{
    memset(OLED_GRAM, 0, sizeof(OLED_GRAM));
}

void OLED_Clear(void)
{
    OLED_ClearBuffer();
    OLED_Refresh();
}

void OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t color)
{
    if(x > 127 || y > 63) return;
    if(color) OLED_GRAM[y/8][x] |= (1 << (y%8));
    else      OLED_GRAM[y/8][x] &= ~(1 << (y%8));
}

void OLED_Init(void)
{
    IIC_Init();
    delay_ms(100);
    OLED_WriteCMD(0xae); //关闭屏幕
    OLED_WriteCMD(0xd5); 
    OLED_WriteCMD(0x80);
    OLED_WriteCMD(0xA8);
    OLED_WriteCMD(0x3F);
    OLED_WriteCMD(0xD3);
    OLED_WriteCMD(0x00);
    OLED_WriteCMD(0x40);
    OLED_WriteCMD(0xA1);
    OLED_WriteCMD(0xC8);
    OLED_WriteCMD(0xDA);
    OLED_WriteCMD(0x12);
    OLED_WriteCMD(0x81);
    OLED_WriteCMD(0xCF);
    OLED_WriteCMD(0xD9);
    OLED_WriteCMD(0xF1);
    OLED_WriteCMD(0xDB);
    OLED_WriteCMD(0x30);
    OLED_WriteCMD(0xA4);
    OLED_WriteCMD(0xA6);
    OLED_WriteCMD(0x8D);
    OLED_WriteCMD(0x14);
    OLED_WriteCMD(0xAF);
    delay_ms(100);
    OLED_Clear();
}

uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;	//结果默认为1
	while (Y --)			//累乘Y次
	{
		Result *= X;		//每次把X累乘到结果上
	}
	return Result;
}

void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t ch,uint8_t FontSize)
{
    uint8_t c = ch - ' ';
    if(FontSize == 8) // 这里兼容老代码的 FontSize==8 其实指的是 8x16 字体
    {
        for(uint8_t i = 0; i < 8; i++) // col
        {
            uint8_t col_top = Font8x16[c][i];
            uint8_t col_bot = Font8x16[c][i+8];
            for(uint8_t j = 0; j < 8; j++) // row
            {
                OLED_DrawPoint(x+i, y+j,   (col_top & (1<<j)) ? 1 : 0);
                OLED_DrawPoint(x+i, y+j+8, (col_bot & (1<<j)) ? 1 : 0);
            }
        }
    }
    else // FontSize == 6, 6x8 字体
    {
        for(uint8_t i = 0; i < 6; i++) // col
        {
            uint8_t col = Font6x8[c][i];
            for(uint8_t j = 0; j < 8; j++) // row
            {
                OLED_DrawPoint(x+i, y+j, (col & (1<<j)) ? 1 : 0);
            }
        }
    }
}

void OLED_ShowSTR(uint8_t x,uint8_t y,char *str,uint8_t FontSize)
{
    uint8_t charWidth = (FontSize == 8) ? 8 : 6;
    while(*str != '\0')
    {
        OLED_ShowChar(x, y, *str, FontSize);
        x += charWidth;
        str++;
    }
}

void OLED_ShowIMG(uint8_t x,uint8_t y,uint8_t width,uint8_t height,const uint8_t *img)
{
    uint8_t pages = (height+7)/8;
    for(uint8_t p = 0; p < pages; p++)
    {
        for(uint8_t i = 0; i < width; i++)
        {
            uint8_t col = img[p * width + i];
            for(uint8_t j = 0; j < 8; j++)
            {
                if(p*8+j < height) {
                    OLED_DrawPoint(x+i, y+p*8+j, (col & (1<<j)) ? 1 : 0);
                }
            }
        }
    }
}

void OLED_ShowNUM(uint8_t x,uint8_t y,uint32_t num,uint8_t length,uint8_t FontSize)
{
    uint8_t charWidth = (FontSize == 8) ? 8 : 6;
    for(int8_t i = length-1;i>=0;i--)
    {
        uint32_t bit = (uint32_t)(num/OLED_Pow(10,i))%10;
        OLED_ShowChar(x+(length-i-1)*charWidth, y, bit+'0', FontSize);
    }
}

void OLED_ShowSignedNum(uint8_t x,uint8_t y,int32_t num,uint8_t length,uint8_t FontSize)
{
    uint8_t charWidth = (FontSize == 8) ? 8 : 6;
    if(num<0)
    {
        OLED_ShowChar(x,y,'-',FontSize);
        num*=-1;
    }
    else
    {
        OLED_ShowChar(x,y,'+',FontSize);
    }
    x = x + charWidth;
    for(int8_t i = length-1;i>=0;i--)
    {
        uint32_t bit=(uint32_t)(num/OLED_Pow(10,i))%10;
        OLED_ShowChar(x+charWidth*(length-i-1),y,bit+'0',FontSize);
    }
}

void OLED_ShowCNStr(uint8_t x,uint8_t y,char *str)
{
    uint8_t j = 0;
    while(*str!='\0')
    {
        uint8_t matched = 0;
        for(uint8_t i = 0; i<sizeof(ChineseFont)/sizeof(ChineseFont[0]);i++)
        {
            if(strncmp(str,ChineseFont[i].name,3)==0)
            {
                OLED_ShowIMG(x+16*j, y, 16, 16, ChineseFont[i].data);
                j++;
                matched = 1;
                break;
            }
        }
        if (matched) {
            str += 3;
        } else {
            str++;
        }
    }
}

void OLED_Print(uint8_t x,uint8_t y,uint8_t fontsize,char *fmt,...)
{
    char dispBuf[32];
    va_list args;
    va_start(args,fmt);
    vsnprintf(dispBuf, sizeof(dispBuf), fmt, args);
    va_end(args);
    OLED_ShowSTR(x,y,dispBuf,fontsize);
}
