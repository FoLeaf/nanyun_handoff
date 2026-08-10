#include "hal_defs.h"
#include "hal_cc8051.h"
#include "hal_int.h"
#include "hal_mcu.h"
#include "hal_board.h"
#include "hal_led.h"
#include "hal_rf.h"
#include "basic_rf.h"
#include "hal_uart.h" 
#include "sensor_drv/sensor.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
/*****点对点通讯地址设置******/
#define RF_CHANNEL                 15         // 频道 11~26
#define PAN_ID                    0x0924    //网络id 
#define MY_ADDR                   0x0102     //本机模块地址
#define SEND_ADDR                 0x0101     //发送地址
/**************************************************/
static basicRfCfg_t basicRfConfig;
// 无线RF初始化
void ConfigRf_Init(void)
{
    basicRfConfig.panId       =   PAN_ID;
    basicRfConfig.channel     =   RF_CHANNEL;
    basicRfConfig.myAddr      =   MY_ADDR;
    basicRfConfig.ackRequest  =   TRUE;
    halRfInit();
    while(basicRfInit(&basicRfConfig) == FAILED);
    basicRfReceiveOn();
}

/********************MAIN************************/

uint8 sand_data[10];
uint8 is_sand_low = 0;
uint8 is_sand_up = 0;
void Delay(uint32 t)
{
  while(t--);
}


void main(void)
{
   
    halBoardInit();//选手不得在此函数内添加代码
    halRfInit();
    ConfigRf_Init();//选手不得在此函数内添加代码
     uint16 value;
    while(1)
    {
    /* user code start */
     
         value = get_guangdian_ad();
         
         if(value <100)
         {
           if(!is_sand_low)
           {
             is_sand_low = 1;
             is_sand_up = 0;
             sand_data[0] = 0x01;  
             basicRfSendPacket(SEND_ADDR,sand_data,1);
           }
         }else
         {
           if(!is_sand_up)
           {
             is_sand_low = 0;
             is_sand_up = 1;
             sand_data[0] = 0x02;  
             basicRfSendPacket(SEND_ADDR,sand_data,1);
           }
         }
                 
    /* user code end */
    }
}