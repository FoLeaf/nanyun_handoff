#include "lib_radio.h"
#include "basic_rf.h"
static basicRfCfg_t basicRfConfig;
void Radio_Init(uint16 panId,uint8 channel,uint16 addr)
{
    basicRfConfig.panId       =   panId;
    basicRfConfig.channel     =   channel;
    basicRfConfig.myAddr      =   addr;
    basicRfConfig.ackRequest  =   TRUE;
    while(basicRfInit(&basicRfConfig) == FAILED);
    basicRfReceiveOn();
}


void Radio_Send(uint16 addr,uint8 *buf,uint8 len)
{
   basicRfSendPacket(addr,buf,len);
}