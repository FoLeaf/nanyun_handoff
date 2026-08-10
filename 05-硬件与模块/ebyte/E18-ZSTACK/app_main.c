#include "ZComDef.h"
#include "OSAL.h"
#include "AF.h"
#include "ZDApp.h"
#include "ZDObject.h"
#include "ZDProfile.h"

#include "hal_i2c.h"
#include "oled.h"
#include "aht30.h"

/* Application Events */
#define APP_REPORT_EVT 0x0001

/* Zigbee Parameters */
#define APP_ENDPOINT 1
#define APP_PROFID   0x0104 // Home Automation
#define APP_DEVICEID 0x0001
#define APP_FLAGS    0

/* Cluster ID */
#define CLUSTER_SENSORS 0x0001

static byte App_TaskID;
static endPointDesc_t App_epDesc;

void App_Init(byte task_id)
{
    App_TaskID = task_id;
    
    /* Peripherals Init */
    OLED_Init();
    AHT30_Init();
    
    OLED_ShowString(0, 0, "Zigbee Sensor", 16);
    
    /* Zigbee Endpoint Setup */
    App_epDesc.endPoint = APP_ENDPOINT;
    App_epDesc.task_id = task_id;
    App_epDesc.simpleDesc = (SimpleDescriptionFormat_t *)NULL; // Simplified
    App_epDesc.latencyType = noLatencyRectls;
    afRegister(&App_epDesc);
    
    /* Start periodic reporting timer */
    osal_start_timerEx(App_TaskID, APP_REPORT_EVT, 5000); // 5 seconds
}

UINT16 App_ProcessEvent(byte task_id, UINT16 events)
{
    if (events & APP_REPORT_EVT)
    {
        AHT30_Data sensorData;
        if (AHT30_ReadData(&sensorData))
        {
            char str[20];
            /* Update OLED */
            sprintf(str, "T:%.1f C", sensorData.temperature);
            OLED_ShowString(0, 2, str, 16);
            sprintf(str, "H:%.1f %%", sensorData.humidity);
            OLED_ShowString(0, 4, str, 16);
            
            /* Send Zigbee Data */
            afAddrType_t dstAddr;
            dstAddr.addrMode = (afAddrMode_t)Addr16Bit;
            dstAddr.addr.shortAddr = 0x0000; // Coordinator
            dstAddr.endPoint = APP_ENDPOINT;
            
            byte payload[8]; // Example payload
            // Fill payload with sensorData...
            
            AF_DataRequest(&dstAddr, &App_epDesc, CLUSTER_SENSORS, 
                           8, payload, &App_TransID, APP_FLAGS, AF_DEFAULT_RADIUS);
        }
        
        osal_start_timerEx(App_TaskID, APP_REPORT_EVT, 5000);
        return (events ^ APP_REPORT_EVT);
    }
    
    return 0;
}
