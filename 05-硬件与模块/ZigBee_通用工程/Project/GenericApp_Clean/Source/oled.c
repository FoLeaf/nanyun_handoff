#include "oled.h"
#include "oledFont.h"
#include "myiic.h"

#define OLED_ADDR 0x78 // SSD1306 7-bit I2C address (0x3C << 1)

static void OLED_WriteCmd(uint8 cmd) {
  IIC_Start();
  IIC_Send_Byte(OLED_ADDR);
  IIC_Wait_Ack();
  IIC_Send_Byte(0x00);
  IIC_Wait_Ack();
  IIC_Send_Byte(cmd);
  IIC_Wait_Ack();
  IIC_Stop();
}

static void OLED_WriteData(uint8 data) {
  IIC_Start();
  IIC_Send_Byte(OLED_ADDR);
  IIC_Wait_Ack();
  IIC_Send_Byte(0x40);
  IIC_Wait_Ack();
  IIC_Send_Byte(data);
  IIC_Wait_Ack();
  IIC_Stop();
}

void OLED_SetPos(uint8 x, uint8 y) {
  OLED_WriteCmd(0xB0 + y);
  OLED_WriteCmd(((x & 0xF0) >> 4) | 0x10);
  OLED_WriteCmd(x & 0x0F);
}

void OLED_DisplayOn(void) {
  OLED_WriteCmd(0x8D);
  OLED_WriteCmd(0x14);
  OLED_WriteCmd(0xAF);
}

void OLED_DisplayOff(void) {
  OLED_WriteCmd(0x8D);
  OLED_WriteCmd(0x10);
  OLED_WriteCmd(0xAE);
}

void OLED_Clear(void) {
  uint8 i, n;
  for (i = 0; i < 8; i++) {
    OLED_WriteCmd(0xB0 + i);
    OLED_WriteCmd(0x00);
    OLED_WriteCmd(0x10);
    IIC_Start();
    IIC_Send_Byte(OLED_ADDR);
    IIC_Wait_Ack();
    IIC_Send_Byte(0x40);
    IIC_Wait_Ack();
    for (n = 0; n < 128; n++) {
      IIC_Send_Byte(0x00);
      IIC_Wait_Ack();
    }
    IIC_Stop();
  }
}

void OLED_Init(void) {
  IIC_Init();

  OLED_WriteCmd(0xAE);
  OLED_WriteCmd(0x00);
  OLED_WriteCmd(0x10);
  OLED_WriteCmd(0x40);
  OLED_WriteCmd(0x81);
  OLED_WriteCmd(0xCF);
  OLED_WriteCmd(0xA1);
  OLED_WriteCmd(0xC8);
  OLED_WriteCmd(0xA6);
  OLED_WriteCmd(0xA8);
  OLED_WriteCmd(0x3F);
  OLED_WriteCmd(0xD3);
  OLED_WriteCmd(0x00);
  OLED_WriteCmd(0xD5);
  OLED_WriteCmd(0x80);
  OLED_WriteCmd(0xD9);
  OLED_WriteCmd(0xF1);
  OLED_WriteCmd(0xDA);
  OLED_WriteCmd(0x12);
  OLED_WriteCmd(0xDB);
  OLED_WriteCmd(0x40);
  OLED_WriteCmd(0x8D);
  OLED_WriteCmd(0x14);
  OLED_WriteCmd(0xAF);

  OLED_Clear();
}

void OLED_ShowChar(uint8 x, uint8 y, uint8 chr, uint8 size) {
  uint8 c, i;
  (void)size;
  c = chr - ' ';
  if (x > 120 || c > 94) return;

  OLED_SetPos(x, y);
  IIC_Start();
  IIC_Send_Byte(OLED_ADDR);
  IIC_Wait_Ack();
  IIC_Send_Byte(0x40);
  IIC_Wait_Ack();
  for (i = 0; i < 8; i++) {
    IIC_Send_Byte(Font8x16[c][i]);
    IIC_Wait_Ack();
  }
  IIC_Stop();

  OLED_SetPos(x, y + 1);
  IIC_Start();
  IIC_Send_Byte(OLED_ADDR);
  IIC_Wait_Ack();
  IIC_Send_Byte(0x40);
  IIC_Wait_Ack();
  for (i = 0; i < 8; i++) {
    IIC_Send_Byte(Font8x16[c][i + 8]);
    IIC_Wait_Ack();
  }
  IIC_Stop();
}

void OLED_ShowChineseChar(uint8 x, uint8 y, uint8 index) {
  uint8 i;
  if (x > 112 || index >= 9) return;

  OLED_SetPos(x, y);
  IIC_Start();
  IIC_Send_Byte(OLED_ADDR);
  IIC_Wait_Ack();
  IIC_Send_Byte(0x40);
  IIC_Wait_Ack();
  for (i = 0; i < 16; i++) {
    IIC_Send_Byte(ChineseFont[index].data[i]);
    IIC_Wait_Ack();
  }
  IIC_Stop();

  OLED_SetPos(x, y + 1);
  IIC_Start();
  IIC_Send_Byte(OLED_ADDR);
  IIC_Wait_Ack();
  IIC_Send_Byte(0x40);
  IIC_Wait_Ack();
  for (i = 0; i < 16; i++) {
    IIC_Send_Byte(ChineseFont[index].data[i + 16]);
    IIC_Wait_Ack();
  }
  IIC_Stop();
}

static int8 OLED_FindChinese(const char *gb)
{
  uint8 i;
  for (i = 0; i < 9; i++) {
    if (ChineseFont[i].name[0] == gb[0] && ChineseFont[i].name[1] == gb[1]) {
      return (int8)i;
    }
  }
  return -1;
}

void OLED_ShowString(uint8 x, uint8 y, char *str) {
  uint8 j = 0;
  while (str[j] != '\0') {
    if ((uint8)str[j] >= 0x80) {
      int8 idx = OLED_FindChinese(&str[j]);
      if (idx >= 0) {
        OLED_ShowChineseChar(x, y, (uint8)idx);
      }
      x += 16;
      j += 2;
    } else {
      OLED_ShowChar(x, y, (uint8)str[j], 16);
      x += 8;
      j++;
    }
    if (x > 120) {
      x = 0;
      y += 2;
    }
  }
}
