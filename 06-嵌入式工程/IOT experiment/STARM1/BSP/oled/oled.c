#include "oled.h"

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

void OLED_SetCursor(uint8_t x,uint8_t y)
{
    OLED_WriteCMD(0x00|(0x0F&x));
    OLED_WriteCMD(0x10|((0xF0&x)>>4));
    OLED_WriteCMD(0xb0|(0x0F&y));
}

void OLED_SetXCursor(uint8_t x)
{
    OLED_WriteCMD(0x00|(0x0F&x));
    OLED_WriteCMD(0x10|((0xF0&x)>>4));
}

void OLED_SetYCursor(uint8_t y)
{
    OLED_WriteCMD(0xb0|(0x0F&y));
}

void OLED_Clear(void)
{
    for(uint8_t j = 0; j<8 ;j++)
    {
        OLED_SetCursor(0,j);
        for(uint8_t k = 0; k<128; k++)
        {
            OLED_WriteData(0x00);
        }
    }
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
    uint8_t width=0;
    OLED_SetCursor(x,y);
    if(FontSize==8)
    {
        for(width=0;width<8;width++)
        {
            OLED_WriteData(Font8x16[ch-' '][width]);
        }
        OLED_SetCursor(x,y+1);
        for(width=8;width<16;width++)
        {
            OLED_WriteData(Font8x16[ch-' '][width]);
        }
        OLED_SetYCursor(y);
    }
    else
    {
        for(width=0;width<6;width++)
        {
            OLED_WriteData(Font6x8[ch-' '][width]);
        }
    }
}

void OLED_ShowSTR(uint8_t x,uint8_t y,char *str,uint8_t FontSize)
{
    OLED_SetCursor(x,y);
    for(uint8_t length = 0; str[length]!='\0';length++)
    {
        if(FontSize==8)
        {
            for(uint8_t j = 0;j<8;j++)
            {
                OLED_WriteData(Font8x16[str[length]-' '][j]);
            }
            OLED_SetCursor(x+length*8,y+1);
            for(uint8_t k = 8;k<16;k++)
            {
                OLED_WriteData(Font8x16[str[length]-' '][k]);
            }
            OLED_SetYCursor(y);
        }
        else
        {
            for(uint8_t j = 0;j<6;j++)
            {
                OLED_WriteData(Font6x8[str[length]-' '][j]);
            }
        }
        
    }
}


void OLED_ShowIMG(uint8_t x,uint8_t y,uint8_t width,uint8_t height,const uint8_t *img)
{
    height=(height+7)/8;
    for(uint8_t i = 0;i<height;i++)
    {
        OLED_SetCursor(x,y+i);
        for(uint8_t j = 0;j<width;j++)
        {
            OLED_WriteData(img[i*width+j]);
        }
    }
}

void OLED_ShowNUM(uint8_t x,uint8_t y,uint32_t num,uint8_t length,uint8_t FontSize)
{
    OLED_SetCursor(x,y);
    for(int8_t i = length-1;i>=0;i--)
    {
        uint32_t bit = (uint32_t)(num/pow(10,i))%10;
        if(FontSize==8)
        {
            for(uint8_t j = 0;j<8;j++)
            {
                OLED_WriteData(Font8x16[('0'+bit)-' '][j]);
            }
            OLED_SetCursor(x+(length-i-1)*FontSize,y+1);
            for(uint8_t k = 8;k<16;k++)
            {
                OLED_WriteData(Font8x16[('0'+bit)-' '][k]);
            }
        }
        //FontSize 6
        else
        {
            for(uint8_t k = 0;k<6;k++)
            {
                OLED_WriteData(Font6x8[('0'+bit)-' '][k]);
            }
        }
        
        OLED_SetYCursor(y);
    }
}

void OLED_ShowSignedNum(uint8_t x,uint8_t y,int32_t num,uint8_t length,uint8_t FontSize)
{
    if(num<0)
    {
        OLED_ShowChar(x,y,'-',FontSize);
        num*=-1;
    }
    else
    {
        OLED_ShowChar(x,y,'+',FontSize);
    }
    x=x+FontSize;
    for(int8_t i = length-1;i>=0;i--)
    {
        uint32_t bit=(uint32_t)(num/pow(10,i))%10;
        OLED_ShowChar(x+FontSize*(length-i-1),y,bit+'0',FontSize);
    }
    
}

void OLED_ShowCNStr(uint8_t x,uint8_t y,char *str)
{
    uint8_t j = 0;
    while(*str!='\0')
    {
        for(uint8_t i = 0; i<sizeof(ChineseFont)/sizeof(ChineseFont[0]);i++)
        {
            if(strncmp(str,ChineseFont[i].name,3)==0)
            {
                OLED_ShowIMG(x+16*(j++),y,16,16,ChineseFont[i].data);
                break;
            }
        }
        str++;
    }
}

