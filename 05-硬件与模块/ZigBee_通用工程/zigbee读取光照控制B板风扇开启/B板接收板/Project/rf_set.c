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
#define MY_ADDR                   0x0101     //本机模块地址
#define SEND_ADDR                 0x0102     //发送地址
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


#define JD1 P2_0

void Init_JD()
{
  P2SEL &= ~0x01;
  P2DIR |=  0x01;
  JD1 = 0;

}

void main(void)
{ 
    halBoardInit();//选手不得在此函数内添加代码
    halRfInit();
    ConfigRf_Init();//选手不得在此函数内添加代码
    Init_JD();
    while(1)
    {
    /* user code start */
      if(basicRfPacketIsReady())
     {
       uint8 dat[10];
       basicRfReceive(dat,1,NULL);
       if(dat[0] == 0x01)
       {
         JD1 = 1;
       }else if(dat[0] == 0x02)
       {
         JD1 = 0;
       }
     }
     /* user code end */      
    }
}