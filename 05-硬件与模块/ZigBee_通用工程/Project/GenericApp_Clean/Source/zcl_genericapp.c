/**************************************************************************************************
  Filename:       zcl_genericapp.c
  Revised:        $Date: 2014-10-24 16:04:46 -0700 (Fri, 24 Oct 2014) $
  Revision:       $Revision: 40796 $


  Description:    Zigbee Cluster Library - sample device application.


  Copyright 2006-2014 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
**************************************************************************************************/

/*********************************************************************
  This application is a template to get started writing an application
  from scratch.

  Look for the sections marked with "GENERICAPP_TODO" to add application
  specific code.

  Note: if you would like your application to support automatic attribute
  reporting, include the BDB_REPORTING compile flag.
*********************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "ZComDef.h"
#include "OSAL.h"
#include "AF.h"
#include "ZDApp.h"
#include "ZDObject.h"
#include "MT_SYS.h"

#if defined(ZG_BUILD_RTRONLY_TYPE) && (ZG_BUILD_RTRONLY_TYPE == TRUE)
#ifndef BDB_REPORTING
#define BDB_REPORTING
#endif
#endif

#include "nwk_util.h"

#include "zcl.h"
#include "zcl_general.h"
#include "zcl_ha.h"
#include "zcl_diagnostic.h"
#include "zcl_genericapp.h"

#include "bdb.h"
#include "bdb_interface.h"
#include "gp_interface.h"

#if defined ( INTER_PAN )
#if defined ( BDB_TL_INITIATOR )
  #include "bdb_touchlink_initiator.h"
#endif // BDB_TL_INITIATOR
#if defined ( BDB_TL_TARGET )
  #include "bdb_touchlink_target.h"
#endif // BDB_TL_TARGET
#endif // INTER_PAN

#if defined ( BDB_TL_INITIATOR ) || defined ( BDB_TL_TARGET )
  #include "bdb_touchlink.h"
#endif

#include "onboard.h"

/* HAL */
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"
#include "hal_uart.h"
#include "myiic.h"
#include "oled.h"
#include "aht30.h"
#include "aht30_port.h"
#include <string.h>
#include "zcl_ms.h"

/*********************************************************************
 * MACROS
 */


/*********************************************************************
 * CONSTANTS
 */


/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
byte zclGenericApp_TaskID;


/*********************************************************************
 * GLOBAL FUNCTIONS
 */
 
/*********************************************************************
 * LOCAL VARIABLES
 */

uint8 giGenAppScreenMode = GENERIC_MAINMODE;   // display the main screen mode first

uint8 gPermitDuration = 0;    // permit joining default to disabled

devStates_t zclGenericApp_NwkState = DEV_INIT;

#if defined(ZG_BUILD_RTRONLY_TYPE) && (ZG_BUILD_RTRONLY_TYPE == TRUE)
static AHT30_HandleTypeDef aht30_dev;
static uint8 zclGenericAppSeqNum = 0;
static uint16 s_heartbeat_cnt = 0;
#endif

// Helper: convert float to string with 2 decimal places. Returns string length.
// Needed because IAR "No float" runtime library does not support sprintf %f.
static uint8 ftoa_2dp(char *buf, float val)
{
  uint8 pos = 0;
  int16 int_part;
  uint8 frac;

  if (val < 0.0f)
  {
    buf[pos++] = '-';
    val = -val;
  }

  int_part = (int16)val;
  frac = (uint8)((val - (float)int_part) * 100.0f + 0.5f);

  if (frac >= 100) { int_part++; frac = 0; }

  if (int_part >= 100) buf[pos++] = '0' + (uint8)(int_part / 100);
  if (int_part >= 10)  buf[pos++] = '0' + (uint8)((int_part / 10) % 10);
  buf[pos++] = '0' + (uint8)(int_part % 10);
  buf[pos++] = '.';
  buf[pos++] = '0' + (frac / 10);
  buf[pos++] = '0' + (frac % 10);
  buf[pos] = '\0';
  return pos;
}

#if defined(ZG_BUILD_RTRONLY_TYPE) && (ZG_BUILD_RTRONLY_TYPE == TRUE)
static uint8 u16_to_dec(char *buf, uint16 val)
{
  char tmp[5];
  uint8 len = 0;
  uint8 i;

  do
  {
    tmp[len++] = (char)('0' + (val % 10));
    val /= 10;
  } while (val && (len < sizeof(tmp)));

  for (i = 0; i < len; i++)
  {
    buf[i] = tmp[len - 1 - i];
  }

  buf[len] = '\0';
  return len;
}

static uint8 u32_to_dec(char *buf, uint32 val)
{
  char tmp[10];
  uint8 len = 0;
  uint8 i;

  do
  {
    tmp[len++] = (char)('0' + (val % 10));
    val /= 10;
  } while (val && (len < sizeof(tmp)));

  for (i = 0; i < len; i++)
  {
    buf[i] = tmp[len - 1 - i];
  }

  buf[len] = '\0';
  return len;
}
#endif

static void AppUart0_Init(void)
{
  // UART0 on Alt-2: P1.4 RX, P1.5 TX.
  PERCFG |= 0x01;
  P1SEL |= 0x30;
  P1DIR |= 0x20;
  P1DIR &= (uint8)~0x10;
  ADCCFG &= (uint8)~0x30;
  P2DIR &= (uint8)~0xC0;

  U0CSR = 0x80;
  U0UCR = 0x80;
  U0BAUD = 216;
  U0GCR = 11;
  U0UCR = 0x02;
  URX0IE = 0;
  UTX0IF = 1;
  U0CSR |= 0x40;
}

static void AppUart0_Write(const uint8 *buf, uint16 len)
{
  while (len--)
  {
    while (!UTX0IF);
    UTX0IF = 0;
    U0DBUF = *buf++;
  }

  while (!UTX0IF);
}

static void AppUart0_Puts(const char *str)
{
  AppUart0_Write((const uint8 *)str, (uint16)strlen(str));
}


/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void zclGenericApp_HandleKeys( byte shift, byte keys );
static void zclGenericApp_BasicResetCB( void );
static void zclGenericApp_ProcessIdentifyTimeChange( uint8 endpoint );
static void zclGenericApp_BindNotification( bdbBindNotificationData_t *data );
#if ( defined ( BDB_TL_TARGET ) && (BDB_TOUCHLINK_CAPABILITY_ENABLED == TRUE) )
static void zclGenericApp_ProcessTouchlinkTargetEnable( uint8 enable );
#endif

static void zclGenericApp_ProcessCommissioningStatus(bdbCommissioningModeMsg_t *bdbCommissioningModeMsg);

// app display functions
static void zclGenericApp_LcdDisplayUpdate( void );
#ifdef LCD_SUPPORTED
static void zclGenericApp_LcdDisplayMainMode( void );
static void zclGenericApp_LcdDisplayHelpMode( void );
#endif

// Functions to process ZCL Foundation incoming Command/Response messages
static void zclGenericApp_ProcessIncomingMsg( zclIncomingMsg_t *msg );
#ifdef ZCL_READ
static uint8 zclGenericApp_ProcessInReadRspCmd( zclIncomingMsg_t *pInMsg );
#endif
#ifdef ZCL_WRITE
static uint8 zclGenericApp_ProcessInWriteRspCmd( zclIncomingMsg_t *pInMsg );
#endif
static uint8 zclGenericApp_ProcessInDefaultRspCmd( zclIncomingMsg_t *pInMsg );
#ifdef ZCL_DISCOVER
static uint8 zclGenericApp_ProcessInDiscCmdsRspCmd( zclIncomingMsg_t *pInMsg );
static uint8 zclGenericApp_ProcessInDiscAttrsRspCmd( zclIncomingMsg_t *pInMsg );
static uint8 zclGenericApp_ProcessInDiscAttrsExtRspCmd( zclIncomingMsg_t *pInMsg );
#endif

static void zclSampleApp_BatteryWarningCB( uint8 voltLevel);

/*********************************************************************
 * STATUS STRINGS
 */
#ifdef LCD_SUPPORTED
const char sDeviceName[]   = "  Generic App";
const char sClearLine[]    = " ";
const char sSwGenericApp[]      = "SW1:GENAPP_TODO";  // GENERICAPP_TODO
const char sSwBDBMode[]     = "SW2: Start BDB";
char sSwHelp[]             = "SW4: Help       ";  // last character is * if NWK open
#endif

/*********************************************************************
 * ZCL General Profile Callback table
 */
static zclGeneral_AppCallbacks_t zclGenericApp_CmdCallbacks =
{
  zclGenericApp_BasicResetCB,             // Basic Cluster Reset command
  NULL,                                   // Identify Trigger Effect command
  NULL,                                   // On/Off cluster commands
  NULL,                                   // On/Off cluster enhanced command Off with Effect
  NULL,                                   // On/Off cluster enhanced command On with Recall Global Scene
  NULL,                                   // On/Off cluster enhanced command On with Timed Off
#ifdef ZCL_LEVEL_CTRL
  NULL,                                   // Level Control Move to Level command
  NULL,                                   // Level Control Move command
  NULL,                                   // Level Control Step command
  NULL,                                   // Level Control Stop command
#endif
#ifdef ZCL_GROUPS
  NULL,                                   // Group Response commands
#endif
#ifdef ZCL_SCENES
  NULL,                                  // Scene Store Request command
  NULL,                                  // Scene Recall Request command
  NULL,                                  // Scene Response command
#endif
#ifdef ZCL_ALARMS
  NULL,                                  // Alarm (Response) commands
#endif
#ifdef SE_UK_EXT
  NULL,                                  // Get Event Log command
  NULL,                                  // Publish Event Log command
#endif
  NULL,                                  // RSSI Location command
  NULL                                   // RSSI Location Response command
};

/*********************************************************************
 * GENERICAPP_TODO: Add other callback structures for any additional application specific 
 *       Clusters being used, see available callback structures below.
 *
 *       bdbTL_AppCallbacks_t 
 *       zclApplianceControl_AppCallbacks_t 
 *       zclApplianceEventsAlerts_AppCallbacks_t 
 *       zclApplianceStatistics_AppCallbacks_t 
 *       zclElectricalMeasurement_AppCallbacks_t 
 *       zclGeneral_AppCallbacks_t 
 *       zclGp_AppCallbacks_t 
 *       zclHVAC_AppCallbacks_t 
 *       zclLighting_AppCallbacks_t 
 *       zclMS_AppCallbacks_t 
 *       zclPollControl_AppCallbacks_t 
 *       zclPowerProfile_AppCallbacks_t 
 *       zclSS_AppCallbacks_t  
 *
 */

/*********************************************************************
 * @fn          zclGenericApp_Init
 *
 * @brief       Initialization function for the zclGeneral layer.
 *
 * @param       none
 *
 * @return      none
 */
void zclGenericApp_Init( byte task_id )
{
  zclGenericApp_TaskID = task_id;

  // This app is part of the Home Automation Profile
  bdb_RegisterSimpleDescriptor( &zclGenericApp_SimpleDesc );

  // Register the ZCL General Cluster Library callback functions
  zclGeneral_RegisterCmdCallbacks( GENERICAPP_ENDPOINT, &zclGenericApp_CmdCallbacks );
  
  // GENERICAPP_TODO: Register other cluster command callbacks here

  // Register the application's attribute list
  zcl_registerAttrList( GENERICAPP_ENDPOINT, zclGenericApp_NumAttributes, zclGenericApp_Attrs );

  // Register the Application to receive the unprocessed Foundation command/response messages
  zcl_registerForMsg( zclGenericApp_TaskID );

#ifdef ZCL_DISCOVER
  // Register the application's command list
  zcl_registerCmdList( GENERICAPP_ENDPOINT, zclCmdsArraySize, zclGenericApp_Cmds );
#endif

  // Register low voltage NV memory protection application callback
  RegisterVoltageWarningCB( zclSampleApp_BatteryWarningCB );

  // Register for all key events - This app will handle all key events
  RegisterForKeys( zclGenericApp_TaskID );

  bdb_RegisterCommissioningStatusCB( zclGenericApp_ProcessCommissioningStatus );
  bdb_RegisterIdentifyTimeChangeCB( zclGenericApp_ProcessIdentifyTimeChange );
  bdb_RegisterBindNotificationCB( zclGenericApp_BindNotification );

#if ( defined ( BDB_TL_TARGET ) && (BDB_TOUCHLINK_CAPABILITY_ENABLED == TRUE) )
  bdb_RegisterTouchlinkTargetEnableCB( zclGenericApp_ProcessTouchlinkTargetEnable );
#endif

#ifdef ZCL_DIAGNOSTIC
  // Register the application's callback function to read/write attribute data.
  // This is only required when the attribute data format is unknown to ZCL.
  zcl_registerReadWriteCB( GENERICAPP_ENDPOINT, zclDiagnostic_ReadWriteAttrCB, NULL );

  if ( zclDiagnostic_InitStats() == ZSuccess )
  {
    // Here the user could start the timer to save Diagnostics to NV
  }
#endif


  AppUart0_Init();
  AppUart0_Puts("Init start\r\n");

#ifdef LCD_SUPPORTED
  HalLcdWriteString ( (char *)sDeviceName, HAL_LCD_LINE_3 );
#endif  // LCD_SUPPORTED

#if defined(ZG_BUILD_RTRONLY_TYPE) && (ZG_BUILD_RTRONLY_TYPE == TRUE)
  // Router only: OLED + AHT30 local sampling.
  AppUart0_Puts("OLED init...\r\n");
  OLED_Init();
  OLED_ShowString(0, 0, "CC2530 P2P Temp");
  OLED_ShowString(0, 2, "Role: Router");
  AppUart0_Puts("Role: Router\r\n");

  // Router only: read AHT30 locally and forward the measurements to the coordinator.
  AppUart0_Puts("AHT30 init...\r\n");
  AHT30_Port_Init();
  AHT30_Port_Bind(&aht30_dev);
  {
    uint8 aht30_ret = AHT30_Init(&aht30_dev);
    char dbg[24];
    uint8 pos = 0;

    osal_memcpy(dbg + pos, "AHT30 ret=", 10); pos += 10;
    pos += u16_to_dec(dbg + pos, aht30_ret);
    osal_memcpy(dbg + pos, "\r\n", 2); pos += 2;
    dbg[pos] = '\0';
    AppUart0_Write((const uint8 *)dbg, pos);

    if (aht30_ret == 0)
    {
      OLED_ShowString(0, 4, "AHT30 Init: OK ");
    }
    else
    {
      OLED_ShowString(0, 4, "AHT30 Init: Fail");
    }
  }

  // Periodic read + heartbeat for router side debug.
  osal_start_timerEx( zclGenericApp_TaskID, GENERICAPP_READ_SENSOR_EVT, 2000 );
  osal_start_timerEx( zclGenericApp_TaskID, GENERICAPP_HEARTBEAT_EVT, 5000 );
#else
  AppUart0_Puts("Role: Coordinator\r\n");
  AppUart0_Puts("RX only\r\n");
#endif

  // Auto-start commissioning so the link works after power-up without key presses.
  osal_start_timerEx( zclGenericApp_TaskID, GENERICAPP_START_COMMISSION_EVT, 1500 );
}

/*********************************************************************
 * @fn          zclSample_event_loop
 *
 * @brief       Event Loop Processor for zclGeneral.
 *
 * @param       none
 *
 * @return      none
 */
uint16 zclGenericApp_event_loop( uint8 task_id, uint16 events )
{
  afIncomingMSGPacket_t *MSGpkt;

  (void)task_id;  // Intentionally unreferenced parameter

  if ( events & SYS_EVENT_MSG )
  {
    while ( (MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( zclGenericApp_TaskID )) )
    {
      switch ( MSGpkt->hdr.event )
      {
        case ZCL_INCOMING_MSG:
          // Incoming ZCL Foundation command/response messages
          zclGenericApp_ProcessIncomingMsg( (zclIncomingMsg_t *)MSGpkt );
          break;

        case KEY_CHANGE:
          zclGenericApp_HandleKeys( ((keyChange_t *)MSGpkt)->state, ((keyChange_t *)MSGpkt)->keys );
          break;

        case ZDO_STATE_CHANGE:
          zclGenericApp_NwkState = (devStates_t)(MSGpkt->hdr.status);
          if (zclGenericApp_NwkState == DEV_ZB_COORD)
          {
            AppUart0_Puts("NWK: DEV_ZB_COORD\r\n");
            AppUart0_Puts("CO ready\r\n");
          }
          else if (zclGenericApp_NwkState == DEV_ROUTER)
          {
            AppUart0_Puts("NWK: DEV_ROUTER\r\n");
            AppUart0_Puts("RO ready\r\n");
          }
          else if (zclGenericApp_NwkState == DEV_END_DEVICE)
          {
            AppUart0_Puts("NWK: DEV_END_DEVICE\r\n");
          }
          else
          {
            AppUart0_Puts("NWK: state change\r\n");
          }

          // now on the network
          if ( (zclGenericApp_NwkState == DEV_ZB_COORD) ||
               (zclGenericApp_NwkState == DEV_ROUTER)   ||
               (zclGenericApp_NwkState == DEV_END_DEVICE) )
          {
            giGenAppScreenMode = GENERIC_MAINMODE;
            zclGenericApp_LcdDisplayUpdate();
          }
          break;

        default:
          break;
      }

      // Release the memory
      osal_msg_deallocate( (uint8 *)MSGpkt );
    }

    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

  if ( events & GENERICAPP_MAIN_SCREEN_EVT )
  {
    giGenAppScreenMode = GENERIC_MAINMODE;
    zclGenericApp_LcdDisplayUpdate();

    return ( events ^ GENERICAPP_MAIN_SCREEN_EVT );
  }

  if ( events & GENERICAPP_START_COMMISSION_EVT )
  {
#if defined(ZG_BUILD_COORDINATOR_TYPE) && (ZG_BUILD_COORDINATOR_TYPE == TRUE)
    AppUart0_Puts("BDB start: FORM+STEER\r\n");
    bdb_StartCommissioning(BDB_COMMISSIONING_MODE_NWK_FORMATION | BDB_COMMISSIONING_MODE_NWK_STEERING);
#elif defined(ZG_BUILD_RTRONLY_TYPE) && (ZG_BUILD_RTRONLY_TYPE == TRUE)
    AppUart0_Puts("BDB start: STEER\r\n");
    bdb_StartCommissioning(BDB_COMMISSIONING_MODE_NWK_STEERING);
#else
    AppUart0_Puts("BDB start: STEER(ED)\r\n");
    bdb_StartCommissioning(BDB_COMMISSIONING_MODE_NWK_STEERING);
#endif
    return ( events ^ GENERICAPP_START_COMMISSION_EVT );
  }

  if ( events & GENERICAPP_RETRY_COMMISSION_EVT )
  {
    if ( (zclGenericApp_NwkState != DEV_ZB_COORD) &&
         (zclGenericApp_NwkState != DEV_ROUTER) &&
         (zclGenericApp_NwkState != DEV_END_DEVICE) )
    {
#if defined(ZG_BUILD_COORDINATOR_TYPE) && (ZG_BUILD_COORDINATOR_TYPE == TRUE)
      AppUart0_Puts("BDB retry: FORM+STEER\r\n");
      bdb_StartCommissioning(BDB_COMMISSIONING_MODE_NWK_FORMATION | BDB_COMMISSIONING_MODE_NWK_STEERING);
#elif defined(ZG_BUILD_RTRONLY_TYPE) && (ZG_BUILD_RTRONLY_TYPE == TRUE)
      AppUart0_Puts("BDB retry: STEER\r\n");
      bdb_StartCommissioning(BDB_COMMISSIONING_MODE_NWK_STEERING);
#else
      AppUart0_Puts("BDB retry: STEER(ED)\r\n");
      bdb_StartCommissioning(BDB_COMMISSIONING_MODE_NWK_STEERING);
#endif
      osal_start_timerEx( zclGenericApp_TaskID, GENERICAPP_RETRY_COMMISSION_EVT, 8000 );
    }
    return ( events ^ GENERICAPP_RETRY_COMMISSION_EVT );
  }
  
#if ZG_BUILD_ENDDEVICE_TYPE
  if ( events & GENERICAPP_END_DEVICE_REJOIN_EVT )
  {
    bdb_ZedAttemptRecoverNwk();
    return ( events ^ GENERICAPP_END_DEVICE_REJOIN_EVT );
  }
#endif

  /* GENERICAPP_TODO: handle app events here */

#if defined(ZG_BUILD_RTRONLY_TYPE) && (ZG_BUILD_RTRONLY_TYPE == TRUE)
  if ( events & GENERICAPP_READ_SENSOR_EVT )
  {
    if (AHT30_ReadMeasure(&aht30_dev) == 0)
    {
      char display_str[64];
      uint8 pos;

      pos = 0;
      osal_memcpy(display_str + pos, "Temp: ", 6); pos += 6;
      pos += ftoa_2dp(display_str + pos, aht30_dev.temperature);
      osal_memcpy(display_str + pos, " C  ", 4); pos += 4;
      display_str[pos] = '\0';
      OLED_ShowString(0, 4, display_str);

      pos = 0;
      osal_memcpy(display_str + pos, "Humi: ", 6); pos += 6;
      pos += ftoa_2dp(display_str + pos, aht30_dev.humidity);
      osal_memcpy(display_str + pos, " %  ", 4); pos += 4;
      display_str[pos] = '\0';
      OLED_ShowString(0, 6, display_str);

      pos = 0;
      osal_memcpy(display_str + pos, "Local Temp: ", 12); pos += 12;
      pos += ftoa_2dp(display_str + pos, aht30_dev.temperature);
      osal_memcpy(display_str + pos, " C, Humi: ", 10); pos += 10;
      pos += ftoa_2dp(display_str + pos, aht30_dev.humidity);
      osal_memcpy(display_str + pos, " %\r\n", 4); pos += 4;
      display_str[pos] = '\0';
      AppUart0_Write((const uint8 *)display_str, pos);

      // Zigbee wire format:
      //   Temperature Measurement cluster (0x0402), attr 0x0000, int16, value = temp * 100
      //   Relative Humidity Measurement cluster (0x0405), attr 0x0000, uint16, value = humi * 100
      if ( zclGenericApp_NwkState == DEV_ROUTER )
      {
        int16 temp_val = (int16)(aht30_dev.temperature * 100.0f);
        uint16 humi_val = (uint16)(aht30_dev.humidity * 100.0f);
        ZStatus_t txStat = ZFailure;
        
        afAddrType_t dstAddr;
        dstAddr.addrMode = afAddr16Bit;
        dstAddr.addr.shortAddr = 0x0000;
        dstAddr.endPoint = GENERICAPP_ENDPOINT;
        
        // Report Temperature
        zclReportCmd_t *pReportCmd;
        pReportCmd = osal_mem_alloc(sizeof(zclReportCmd_t) + sizeof(zclReport_t));
        if (pReportCmd)
        {
          pReportCmd->numAttr = 1;
          pReportCmd->attrList[0].attrID = ATTRID_MS_TEMPERATURE_MEASURED_VALUE;
          pReportCmd->attrList[0].dataType = ZCL_DATATYPE_INT16;
          pReportCmd->attrList[0].attrData = (uint8 *)&temp_val;
          
          txStat = zcl_SendReportCmd(GENERICAPP_ENDPOINT, &dstAddr,
                            ZCL_CLUSTER_ID_MS_TEMPERATURE_MEASUREMENT, pReportCmd,
                            ZCL_FRAME_SERVER_CLIENT_DIR, TRUE, zclGenericAppSeqNum++);
          osal_mem_free(pReportCmd);
        }
        if (txStat != ZSuccess)
        {
          AppUart0_Puts("TX temp fail\r\n");
        }
        
        // Report Humidity
        pReportCmd = osal_mem_alloc(sizeof(zclReportCmd_t) + sizeof(zclReport_t));
        if (pReportCmd)
        {
          pReportCmd->numAttr = 1;
          pReportCmd->attrList[0].attrID = ATTRID_MS_RELATIVE_HUMIDITY_MEASURED_VALUE;
          pReportCmd->attrList[0].dataType = ZCL_DATATYPE_UINT16;
          pReportCmd->attrList[0].attrData = (uint8 *)&humi_val;
          
          txStat = zcl_SendReportCmd(GENERICAPP_ENDPOINT, &dstAddr,
                            ZCL_CLUSTER_ID_MS_RELATIVE_HUMIDITY, pReportCmd,
                            ZCL_FRAME_SERVER_CLIENT_DIR, TRUE, zclGenericAppSeqNum++);
          osal_mem_free(pReportCmd);
        }
        if (txStat != ZSuccess)
        {
          AppUart0_Puts("TX humi fail\r\n");
        }
      }
    }
    else
    {
      OLED_ShowString(0, 4, "Read Sensor Err ");
    }
    
    osal_start_timerEx( zclGenericApp_TaskID, GENERICAPP_READ_SENSOR_EVT, 2000 );
    return ( events ^ GENERICAPP_READ_SENSOR_EVT );
  }

  if ( events & GENERICAPP_HEARTBEAT_EVT )
  {
    s_heartbeat_cnt++;
    {
      char hb[40];
      uint8 pos = 0;

      osal_memcpy(hb + pos, "HB[", 3); pos += 3;
      pos += u16_to_dec(hb + pos, s_heartbeat_cnt);
      osal_memcpy(hb + pos, "] sysClk=", 9); pos += 9;
      pos += u32_to_dec(hb + pos, (uint32)osal_GetSystemClock());
      osal_memcpy(hb + pos, "\r\n", 2); pos += 2;
      hb[pos] = '\0';
      AppUart0_Write((const uint8 *)hb, pos);
    }
    osal_start_timerEx( zclGenericApp_TaskID, GENERICAPP_HEARTBEAT_EVT, 5000 );
    return ( events ^ GENERICAPP_HEARTBEAT_EVT );
  }
#endif

  if ( events & GENERICAPP_EVT_1 )
  {
    // toggle LED 2 state, start another timer for 500ms
    HalLedSet ( HAL_LED_2, HAL_LED_MODE_TOGGLE );
    osal_start_timerEx( zclGenericApp_TaskID, GENERICAPP_EVT_1, 500 );
    
    return ( events ^ GENERICAPP_EVT_1 );
  }
  
  /*
  if ( events & GENERICAPP_EVT_2 )
  {
    
    return ( events ^ GENERICAPP_EVT_2 );
  }
  
  if ( events & GENERICAPP_EVT_3 )
  {
    
    return ( events ^ GENERICAPP_EVT_3 );
  }
  */
  
  // Discard unknown events
  return 0;
}


/*********************************************************************
 * @fn      zclGenericApp_HandleKeys
 *
 * @brief   Handles all key events for this device.
 *
 * @param   shift - true if in shift/alt.
 * @param   keys - bit field for key events. Valid entries:
 *                 HAL_KEY_SW_5
 *                 HAL_KEY_SW_4
 *                 HAL_KEY_SW_2
 *                 HAL_KEY_SW_1
 *
 * @return  none
 */
static void zclGenericApp_HandleKeys( byte shift, byte keys )
{
  if ( keys & HAL_KEY_SW_1 )
  {
    static bool LED_OnOff = FALSE;
    
    giGenAppScreenMode = GENERIC_MAINMODE;
    
    /* GENERICAPP_TODO: add app functionality to hardware keys here */
    
    // for example, start/stop LED 2 toggling with 500ms period
    if (LED_OnOff)
    { 
      // if the LED is blinking, stop the osal timer and turn the LED off
      osal_stop_timerEx(zclGenericApp_TaskID, GENERICAPP_EVT_1);
      HalLedSet ( HAL_LED_2, HAL_LED_MODE_OFF );
      LED_OnOff = FALSE;
    }
    else
    {
      // turn on LED 2 and start an osal timer to toggle it after 500ms, search
      // for GENERICAPP_EVT_1 to see event handling after expired timer
      osal_start_timerEx( zclGenericApp_TaskID, GENERICAPP_EVT_1, 500 );
      HalLedSet ( HAL_LED_2, HAL_LED_MODE_ON );
      LED_OnOff = TRUE;
    }
  }
  // Start the BDB commissioning method
  if ( keys & HAL_KEY_SW_2 )
  {
    giGenAppScreenMode = GENERIC_MAINMODE;

    bdb_StartCommissioning(BDB_COMMISSIONING_MODE_NWK_FORMATION | BDB_COMMISSIONING_MODE_NWK_STEERING | BDB_COMMISSIONING_MODE_FINDING_BINDING | BDB_COMMISSIONING_MODE_INITIATOR_TL);
  }
  if ( keys & HAL_KEY_SW_3 )
  {
    giGenAppScreenMode = GENERIC_MAINMODE;
  
    // touchlink target commissioning, if enabled  
#if ( defined ( BDB_TL_TARGET ) && (BDB_TOUCHLINK_CAPABILITY_ENABLED == TRUE) )
    bdb_StartCommissioning(BDB_COMMISSIONING_MODE_FINDING_BINDING);
    touchLinkTarget_EnableCommissioning( 30000 );
#endif
    
  }
  if ( keys & HAL_KEY_SW_4 )
  {
    
   giGenAppScreenMode = giGenAppScreenMode ? GENERIC_MAINMODE : GENERIC_HELPMODE;
#ifdef LCD_SUPPORTED
    HalLcdWriteString( (char *)sClearLine, HAL_LCD_LINE_2 );
#endif
    
  }
  if ( keys & HAL_KEY_SW_5 )
  {
    bdb_resetLocalAction();
  }

  zclGenericApp_LcdDisplayUpdate();
}

/*********************************************************************
 * @fn      zclGenericApp_LcdDisplayUpdate
 *
 * @brief   Called to update the LCD display.
 *
 * @param   none
 *
 * @return  none
 */
void zclGenericApp_LcdDisplayUpdate( void )
{
#ifdef LCD_SUPPORTED
  if ( giGenAppScreenMode == GENERIC_HELPMODE )
  {
    zclGenericApp_LcdDisplayHelpMode();
  }
  else
  {
    zclGenericApp_LcdDisplayMainMode();
  }
#endif
}

#ifdef LCD_SUPPORTED
/*********************************************************************
 * @fn      zclGenericApp_LcdDisplayMainMode
 *
 * @brief   Called to display the main screen on the LCD.
 *
 * @param   none
 *
 * @return  none
 */
static void zclGenericApp_LcdDisplayMainMode( void )
{
  // display line 1 to indicate NWK status
  if ( zclGenericApp_NwkState == DEV_ZB_COORD )
  {
    zclHA_LcdStatusLine1( ZCL_HA_STATUSLINE_ZC );
  }
  else if ( zclGenericApp_NwkState == DEV_ROUTER )
  {
    zclHA_LcdStatusLine1( ZCL_HA_STATUSLINE_ZR );
  }
  else if ( zclGenericApp_NwkState == DEV_END_DEVICE )
  {
    zclHA_LcdStatusLine1( ZCL_HA_STATUSLINE_ZED );
  }

  // end of line 3 displays permit join status (*)
  if ( gPermitDuration )
  {
    sSwHelp[15] = '*';
  }
  else
  {
    sSwHelp[15] = ' ';
  }
  HalLcdWriteString( (char *)sSwHelp, HAL_LCD_LINE_3 );
}

/*********************************************************************
 * @fn      zclGenericApp_LcdDisplayHelpMode
 *
 * @brief   Called to display the SW options on the LCD.
 *
 * @param   none
 *
 * @return  none
 */
static void zclGenericApp_LcdDisplayHelpMode( void )
{
  HalLcdWriteString( (char *)sSwGenericApp, HAL_LCD_LINE_1 );
  HalLcdWriteString( (char *)sSwBDBMode, HAL_LCD_LINE_2 );
  HalLcdWriteString( (char *)sSwHelp, HAL_LCD_LINE_3 );
}
#endif  // LCD_SUPPORTED

/*********************************************************************
 * @fn      zclGenericApp_ProcessCommissioningStatus
 *
 * @brief   Callback in which the status of the commissioning process are reported
 *
 * @param   bdbCommissioningModeMsg - Context message of the status of a commissioning process
 *
 * @return  none
 */
static void zclGenericApp_ProcessCommissioningStatus(bdbCommissioningModeMsg_t *bdbCommissioningModeMsg)
{
  switch(bdbCommissioningModeMsg->bdbCommissioningMode)
  {
    case BDB_COMMISSIONING_FORMATION:
      if(bdbCommissioningModeMsg->bdbCommissioningStatus == BDB_COMMISSIONING_SUCCESS)
      {
        AppUart0_Puts("BDB FORM ok\r\n");
        //After formation, perform nwk steering again plus the remaining commissioning modes that has not been process yet
        bdb_StartCommissioning(BDB_COMMISSIONING_MODE_NWK_STEERING | bdbCommissioningModeMsg->bdbRemainingCommissioningModes);
      }
      else
      {
        AppUart0_Puts("BDB FORM fail\r\n");
        osal_start_timerEx( zclGenericApp_TaskID, GENERICAPP_RETRY_COMMISSION_EVT, 3000 );
        //Want to try other channels?
        //try with bdb_setChannelAttribute
      }
    break;
    case BDB_COMMISSIONING_NWK_STEERING:
      if(bdbCommissioningModeMsg->bdbCommissioningStatus == BDB_COMMISSIONING_SUCCESS)
      {
        AppUart0_Puts("BDB STEER ok\r\n");
        //YOUR JOB:
        //We are on the nwk, what now?
      }
      else
      {
        AppUart0_Puts("BDB STEER fail\r\n");
        osal_start_timerEx( zclGenericApp_TaskID, GENERICAPP_RETRY_COMMISSION_EVT, 3000 );
        //See the possible errors for nwk steering procedure
        //No suitable networks found
        //Want to try other channels?
        //try with bdb_setChannelAttribute
      }
    break;
    case BDB_COMMISSIONING_FINDING_BINDING:
      if(bdbCommissioningModeMsg->bdbCommissioningStatus == BDB_COMMISSIONING_SUCCESS)
      {
        //YOUR JOB:
      }
      else
      {
        //YOUR JOB:
        //retry?, wait for user interaction?
      }
    break;
    case BDB_COMMISSIONING_INITIALIZATION:
      //Initialization notification can only be successful. Failure on initialization
      //only happens for ZED and is notified as BDB_COMMISSIONING_PARENT_LOST notification

      //YOUR JOB:
      //We are on a network, what now?

    break;
#if ZG_BUILD_ENDDEVICE_TYPE    
    case BDB_COMMISSIONING_PARENT_LOST:
      if(bdbCommissioningModeMsg->bdbCommissioningStatus == BDB_COMMISSIONING_NETWORK_RESTORED)
      {
        //We did recover from losing parent
      }
      else
      {
        //Parent not found, attempt to rejoin again after a fixed delay
        osal_start_timerEx(zclGenericApp_TaskID, GENERICAPP_END_DEVICE_REJOIN_EVT, GENERICAPP_END_DEVICE_REJOIN_DELAY);
      }
    break;
#endif 
  }
}

/*********************************************************************
 * @fn      zclGenericApp_ProcessIdentifyTimeChange
 *
 * @brief   Called to process any change to the IdentifyTime attribute.
 *
 * @param   endpoint - in which the identify has change
 *
 * @return  none
 */
static void zclGenericApp_ProcessIdentifyTimeChange( uint8 endpoint )
{
  (void) endpoint;

  if ( zclGenericApp_IdentifyTime > 0 )
  {
    HalLedBlink ( HAL_LED_2, 0xFF, HAL_LED_DEFAULT_DUTY_CYCLE, HAL_LED_DEFAULT_FLASH_TIME );
  }
  else
  {
    HalLedSet ( HAL_LED_2, HAL_LED_MODE_OFF );
  }
}

/*********************************************************************
 * @fn      zclGenericApp_BindNotification
 *
 * @brief   Called when a new bind is added.
 *
 * @param   data - pointer to new bind data
 *
 * @return  none
 */
static void zclGenericApp_BindNotification( bdbBindNotificationData_t *data )
{
  // GENERICAPP_TODO: process the new bind information
}


/*********************************************************************
 * @fn      zclGenericApp_ProcessTouchlinkTargetEnable
 *
 * @brief   Called to process when the touchlink target functionality
 *          is enabled or disabled
 *
 * @param   none
 *
 * @return  none
 */
#if ( defined ( BDB_TL_TARGET ) && (BDB_TOUCHLINK_CAPABILITY_ENABLED == TRUE) )
static void zclGenericApp_ProcessTouchlinkTargetEnable( uint8 enable )
{
  if ( enable )
  {
    HalLedSet ( HAL_LED_1, HAL_LED_MODE_ON );
  }
  else
  {
    HalLedSet ( HAL_LED_1, HAL_LED_MODE_OFF );
  }
}
#endif

/*********************************************************************
 * @fn      zclGenericApp_BasicResetCB
 *
 * @brief   Callback from the ZCL General Cluster Library
 *          to set all the Basic Cluster attributes to default values.
 *
 * @param   none
 *
 * @return  none
 */
static void zclGenericApp_BasicResetCB( void )
{

  /* GENERICAPP_TODO: remember to update this function with any
     application-specific cluster attribute variables */
  
  zclGenericApp_ResetAttributesToDefaultValues();
  
}
/*********************************************************************
 * @fn      zclSampleApp_BatteryWarningCB
 *
 * @brief   Called to handle battery-low situation.
 *
 * @param   voltLevel - level of severity
 *
 * @return  none
 */
void zclSampleApp_BatteryWarningCB( uint8 voltLevel )
{
  if ( voltLevel == VOLT_LEVEL_CAUTIOUS )
  {
    // Send warning message to the gateway and blink LED
  }
  else if ( voltLevel == VOLT_LEVEL_BAD )
  {
    // Shut down the system
  }
}

/******************************************************************************
 *
 *  Functions for processing ZCL Foundation incoming Command/Response messages
 *
 *****************************************************************************/

/*********************************************************************
 * @fn      zclGenericApp_ProcessIncomingMsg
 *
 * @brief   Process ZCL Foundation incoming message
 *
 * @param   pInMsg - pointer to the received message
 *
 * @return  none
 */
static void zclGenericApp_ProcessIncomingMsg( zclIncomingMsg_t *pInMsg )
{
  switch ( pInMsg->zclHdr.commandID )
  {
#ifdef ZCL_READ
    case ZCL_CMD_READ_RSP:
      zclGenericApp_ProcessInReadRspCmd( pInMsg );
      break;
#endif
#ifdef ZCL_WRITE
    case ZCL_CMD_WRITE_RSP:
      zclGenericApp_ProcessInWriteRspCmd( pInMsg );
      break;
#endif
    case ZCL_CMD_CONFIG_REPORT:
    case ZCL_CMD_CONFIG_REPORT_RSP:
    case ZCL_CMD_READ_REPORT_CFG:
    case ZCL_CMD_READ_REPORT_CFG_RSP:
      break;
#if defined(ZG_BUILD_COORDINATOR_TYPE) && (ZG_BUILD_COORDINATOR_TYPE == TRUE)
    // Coordinator only: decode the incoming ZCL report and print it to UART.
    case ZCL_CMD_REPORT:
      {
        zclReportCmd_t *pReportCmd = (zclReportCmd_t *)pInMsg->attrCmd;
        uint8 i;
        AppUart0_Puts("RX ZCL REPORT\r\n");
        for (i = 0; i < pReportCmd->numAttr; i++)
        {
          uint16 attrID = pReportCmd->attrList[i].attrID;
          uint8 *attrData = pReportCmd->attrList[i].attrData;
          
          if (pInMsg->clusterId == ZCL_CLUSTER_ID_MS_TEMPERATURE_MEASUREMENT)
          {
            if (attrID == ATTRID_MS_TEMPERATURE_MEASURED_VALUE)
            {
               int16 temp_val = *((int16*)attrData);
               float temp_f = temp_val / 100.0f;
               char display_str[40];
               uint8 pos = 0;

               osal_memcpy(display_str + pos, "Remote Temp: ", 13); pos += 13;
               pos += ftoa_2dp(display_str + pos, temp_f);
               osal_memcpy(display_str + pos, " C\r\n", 4); pos += 4;
               display_str[pos] = '\0';
               AppUart0_Write((const uint8 *)display_str, pos);
            }
          }
          else if (pInMsg->clusterId == ZCL_CLUSTER_ID_MS_RELATIVE_HUMIDITY)
          {
            if (attrID == ATTRID_MS_RELATIVE_HUMIDITY_MEASURED_VALUE)
            {
               uint16 humi_val = *((uint16*)attrData);
               float humi_f = humi_val / 100.0f;
               char display_str[40];
               uint8 pos = 0;

               osal_memcpy(display_str + pos, "Remote Humi: ", 13); pos += 13;
               pos += ftoa_2dp(display_str + pos, humi_f);
               osal_memcpy(display_str + pos, " %\r\n", 4); pos += 4;
               display_str[pos] = '\0';
               AppUart0_Write((const uint8 *)display_str, pos);
            }
          }
        }
      }
      break;
#endif
      
    case ZCL_CMD_DEFAULT_RSP:
      zclGenericApp_ProcessInDefaultRspCmd( pInMsg );
      break;
#ifdef ZCL_DISCOVER
    case ZCL_CMD_DISCOVER_CMDS_RECEIVED_RSP:
      zclGenericApp_ProcessInDiscCmdsRspCmd( pInMsg );
      break;

    case ZCL_CMD_DISCOVER_CMDS_GEN_RSP:
      zclGenericApp_ProcessInDiscCmdsRspCmd( pInMsg );
      break;

    case ZCL_CMD_DISCOVER_ATTRS_RSP:
      zclGenericApp_ProcessInDiscAttrsRspCmd( pInMsg );
      break;

    case ZCL_CMD_DISCOVER_ATTRS_EXT_RSP:
      zclGenericApp_ProcessInDiscAttrsExtRspCmd( pInMsg );
      break;
#endif
    default:
      break;
  }

  if ( pInMsg->attrCmd )
    osal_mem_free( pInMsg->attrCmd );
}

#ifdef ZCL_READ
/*********************************************************************
 * @fn      zclGenericApp_ProcessInReadRspCmd
 *
 * @brief   Process the "Profile" Read Response Command
 *
 * @param   pInMsg - incoming message to process
 *
 * @return  none
 */
static uint8 zclGenericApp_ProcessInReadRspCmd( zclIncomingMsg_t *pInMsg )
{
  zclReadRspCmd_t *readRspCmd;
  uint8 i;

  readRspCmd = (zclReadRspCmd_t *)pInMsg->attrCmd;
  for (i = 0; i < readRspCmd->numAttr; i++)
  {
    // Notify the originator of the results of the original read attributes
    // attempt and, for each successfull request, the value of the requested
    // attribute
  }

  return ( TRUE );
}
#endif // ZCL_READ

#ifdef ZCL_WRITE
/*********************************************************************
 * @fn      zclGenericApp_ProcessInWriteRspCmd
 *
 * @brief   Process the "Profile" Write Response Command
 *
 * @param   pInMsg - incoming message to process
 *
 * @return  none
 */
static uint8 zclGenericApp_ProcessInWriteRspCmd( zclIncomingMsg_t *pInMsg )
{
  zclWriteRspCmd_t *writeRspCmd;
  uint8 i;

  writeRspCmd = (zclWriteRspCmd_t *)pInMsg->attrCmd;
  for ( i = 0; i < writeRspCmd->numAttr; i++ )
  {
    // Notify the device of the results of the its original write attributes
    // command.
  }

  return ( TRUE );
}
#endif // ZCL_WRITE

/*********************************************************************
 * @fn      zclGenericApp_ProcessInDefaultRspCmd
 *
 * @brief   Process the "Profile" Default Response Command
 *
 * @param   pInMsg - incoming message to process
 *
 * @return  none
 */
static uint8 zclGenericApp_ProcessInDefaultRspCmd( zclIncomingMsg_t *pInMsg )
{
  // zclDefaultRspCmd_t *defaultRspCmd = (zclDefaultRspCmd_t *)pInMsg->attrCmd;

  // Device is notified of the Default Response command.
  (void)pInMsg;

  return ( TRUE );
}

#ifdef ZCL_DISCOVER
/*********************************************************************
 * @fn      zclGenericApp_ProcessInDiscCmdsRspCmd
 *
 * @brief   Process the Discover Commands Response Command
 *
 * @param   pInMsg - incoming message to process
 *
 * @return  none
 */
static uint8 zclGenericApp_ProcessInDiscCmdsRspCmd( zclIncomingMsg_t *pInMsg )
{
  zclDiscoverCmdsCmdRsp_t *discoverRspCmd;
  uint8 i;

  discoverRspCmd = (zclDiscoverCmdsCmdRsp_t *)pInMsg->attrCmd;
  for ( i = 0; i < discoverRspCmd->numCmd; i++ )
  {
    // Device is notified of the result of its attribute discovery command.
  }

  return ( TRUE );
}

/*********************************************************************
 * @fn      zclGenericApp_ProcessInDiscAttrsRspCmd
 *
 * @brief   Process the "Profile" Discover Attributes Response Command
 *
 * @param   pInMsg - incoming message to process
 *
 * @return  none
 */
static uint8 zclGenericApp_ProcessInDiscAttrsRspCmd( zclIncomingMsg_t *pInMsg )
{
  zclDiscoverAttrsRspCmd_t *discoverRspCmd;
  uint8 i;

  discoverRspCmd = (zclDiscoverAttrsRspCmd_t *)pInMsg->attrCmd;
  for ( i = 0; i < discoverRspCmd->numAttr; i++ )
  {
    // Device is notified of the result of its attribute discovery command.
  }

  return ( TRUE );
}

/*********************************************************************
 * @fn      zclGenericApp_ProcessInDiscAttrsExtRspCmd
 *
 * @brief   Process the "Profile" Discover Attributes Extended Response Command
 *
 * @param   pInMsg - incoming message to process
 *
 * @return  none
 */
static uint8 zclGenericApp_ProcessInDiscAttrsExtRspCmd( zclIncomingMsg_t *pInMsg )
{
  zclDiscoverAttrsExtRsp_t *discoverRspCmd;
  uint8 i;

  discoverRspCmd = (zclDiscoverAttrsExtRsp_t *)pInMsg->attrCmd;
  for ( i = 0; i < discoverRspCmd->numAttr; i++ )
  {
    // Device is notified of the result of its attribute discovery command.
  }

  return ( TRUE );
}
#endif // ZCL_DISCOVER

/****************************************************************************
****************************************************************************/


