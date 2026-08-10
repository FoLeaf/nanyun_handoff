#include "lib_flash.h"


/***************************************************************
* 功能: 擦除FLASH中的第page页
****************************************************************/
void Flash_Erase(uint8 page)
{
    FADDRH = page << 1;
    FCTL |= 0x01;        //启动页面擦除
    while (FCTL & 0x80);  //等待擦除完成
}

/***************************************************************
* 功能: 将buf中的length个字节写入FLASH第page页offset开始的位置 
****************************************************************/
uint8 DmaDesc[8]={0,0,0x62,0x73,0,0,0x12,0x42};//定义DMA描述符
void Flash_Write(uint8 page, uint16 offset, uint8 *buf, uint16 length)
{
    FADDRH = (page << 1)|((offset >> 10) & 1);  //计算FLASH地址
    FADDRL = (offset >> 2) & 0xFF;
    DmaDesc[0] = (uint16)buf >> 8;            //设置数据地址
    DmaDesc[1] = (uint16)buf & 0xFF; 
    DmaDesc[4] = (length >> 8) & 0x1F;        //设置字节数
    DmaDesc[5] = length & 0xFC; 
   
    DMA0CFGH = (uint16)DmaDesc >> 8;    //设置DMA通道0描述符地址
    DMA0CFGL = (uint16)DmaDesc & 0xFF;
    DMAIRQ = 0x01;                      //激活DMA通道0
    DMAARM = 0x01;                      //使DMA通道0进入工作状态
    FCTL |=  0x02;                      //开始DMA写入操作
    while (FCTL & 0x80);                //等待写入完成
}

/***************************************************************
*功能:将FLASH第page页offset开始的length个字节读入buf中
****************************************************************/
void Flash_Read(uint8 page, uint16 offset, uint8 *buf, uint16 length)
{
    uint8 *pData = (uint8 *)(offset + (page & 0xF) * 2048 + 0x8000);
    uint8 memctr = MEMCTR;                           //暂存当前存储器映射状态
    MEMCTR = (MEMCTR & 0xF8)|((page >> 4) & 7);   //将被读FLASH映射到XDATA
    while (length--) *buf++ = *pData++;           //读取数据
    MEMCTR = memctr;                              //回复存储器映射状态
}