/*********************************************************************
 * zcl_genericapp.c — ZigBee GenericApp 主应用任务
 *
 * 包含：BDB commissioning、UART 输出、Router AHT30/OLED、
 *       ZCL report 发送、Coordinator report 接收。
 *********************************************************************/

#include "ZComDef.h"
#include "OSAL.h"
#include "AF.h"
#include "ZDApp.h"
#include "ZDObject.h"
#include "MT_SYS.h"

#include "nwk_util.h"

#include "zcl.h"
#include "zcl_general.h"
#include "zcl_genericapp.h"
#include "app_config.h"

#include "bdb.h"
#include "bdb_interface.h"
#include "gp_interface.h"

#include "onboard.h"
#include "hal_key.h"

#include "app_led.h"
#if defined(APP_FEATURE_OLED) || defined(APP_FEATURE_AHT30)
#include "myiic.h"
#endif
#ifdef APP_FEATURE_OLED
#include "oled.h"
#include "app_display.h"
#endif
#ifdef APP_FEATURE_AHT30
#include "aht30.h"
#include "aht30_port.h"
#endif
#include <string.h>
#include "zcl_ms.h"

/*********************************************************************
 * 全局变量
 */
byte zclGenericApp_TaskID;

/*********************************************************************
 * 局部变量
 */
uint8 gPermitDuration = 0;

devStates_t zclGenericApp_NwkState = DEV_INIT;

static uint8 s_steer_fail_cnt = 0;

#ifdef APP_ROLE_ROUTER
static AHT30_HandleTypeDef aht30_dev;
static uint8 zclGenericAppSeqNum = 0;
#endif

static uint16 s_tx_ok = 0;

#ifdef APP_FEATURE_OLED
static uint16 s_heartbeat_cnt = 0;
static uint16 s_tx_fail = 0;
static uint32 s_uptime_s = 0;
#endif

#if defined(APP_ROLE_COORDINATOR) && defined(APP_FEATURE_OLED)
static float  s_remote_temp = 0.0f;
static float  s_remote_humi = 0.0f;
static uint16 s_remote_addr = 0xFFFF;
#endif

/*********************************************************************
 * 辅助函数
 */

// 将浮点数转为保留 2 位小数的字符串，返回字符串长度。
// IAR "No float" 运行库不支持 sprintf %f，因此需要该函数。
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

#ifdef APP_ROLE_ROUTER
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
#endif

#ifdef APP_FEATURE_OLED
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

/*********************************************************************
 * UART0 驱动（直接寄存器操作，P1.4=RX, P1.5=TX, 115200 baud）
 */
static void AppUart0_Init(void)
{
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
 * 本地函数声明
 */
static void zclGenericApp_HandleKeys( byte shift, byte keys );
static void zclGenericApp_BasicResetCB( void );
static void zclGenericApp_ProcessIdentifyTimeChange( uint8 endpoint );
static void zclGenericApp_BindNotification( bdbBindNotificationData_t *data );
static void zclGenericApp_ProcessCommissioningStatus(bdbCommissioningModeMsg_t *bdbCommissioningModeMsg);
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

/*********************************************************************
 * ZCL 通用 Cluster 回调表
 */
static zclGeneral_AppCallbacks_t zclGenericApp_CmdCallbacks =
{
  zclGenericApp_BasicResetCB,
  NULL,                                   // Identify Trigger Effect
  NULL,                                   // On/Off
  NULL,                                   // On/Off Off with Effect
  NULL,                                   // On/Off On with Recall Global Scene
  NULL,                                   // On/Off On with Timed Off
#ifdef ZCL_LEVEL_CTRL
  NULL, NULL, NULL, NULL,                // Level Control
#endif
#ifdef ZCL_GROUPS
  NULL,                                   // Group Response
#endif
#ifdef ZCL_SCENES
  NULL, NULL, NULL,                      // Scene
#endif
#ifdef ZCL_ALARMS
  NULL,                                   // Alarm
#endif
#ifdef SE_UK_EXT
  NULL, NULL,                            // Event Log
#endif
  NULL,                                   // RSSI Location
  NULL                                    // RSSI Location Response
};

/*********************************************************************
 * @fn      zclGenericApp_Init
 *
 * @brief   应用任务初始化。
 */
void zclGenericApp_Init( byte task_id )
{
  zclGenericApp_TaskID = task_id;

  bdb_RegisterSimpleDescriptor( &zclGenericApp_SimpleDesc );
  zclGeneral_RegisterCmdCallbacks( GENERICAPP_ENDPOINT, &zclGenericApp_CmdCallbacks );
  zcl_registerAttrList( GENERICAPP_ENDPOINT, zclGenericApp_NumAttributes, zclGenericApp_Attrs );
  zcl_registerForMsg( zclGenericApp_TaskID );

#ifdef ZCL_DISCOVER
  zcl_registerCmdList( GENERICAPP_ENDPOINT, zclCmdsArraySize, zclGenericApp_Cmds );
#endif

  RegisterForKeys( zclGenericApp_TaskID );

  bdb_RegisterCommissioningStatusCB( zclGenericApp_ProcessCommissioningStatus );
  bdb_RegisterIdentifyTimeChangeCB( zclGenericApp_ProcessIdentifyTimeChange );
  bdb_RegisterBindNotificationCB( zclGenericApp_BindNotification );

  AppUart0_Init();
  AppUart0_Puts("Init start\r\n");

  // LED 初始化（Coordinator 和 Router 都需要）
  AppLed_Init();
  AppLed_Set(APP_LED_RUN, APP_LED_OFF);        // 收发包时才闪
  AppLed_Set(APP_LED_NWK, APP_LED_BLINK_FAST); // 未入网时 10Hz 快闪

  // 启动 LED 闪烁处理定时器（10ms 周期）
  osal_start_timerEx( zclGenericApp_TaskID, GENERICAPP_LED_PROCESS_EVT, 10 );

#ifdef APP_FEATURE_OLED
  {

  #ifdef APP_ROLE_ROUTER
    AppUart0_Puts("OLED init...\r\n");
    AppDisplay_Init(APP_DISPLAY_ROLE_ROUTER);
    AppUart0_Puts("Role: Router\r\n");
  #elif defined(APP_ROLE_COORDINATOR)
    AppUart0_Puts("OLED init...\r\n");
    AppDisplay_Init(APP_DISPLAY_ROLE_COORDINATOR);
    AppUart0_Puts("Role: Coordinator\r\n");
  #endif

  #ifdef APP_FEATURE_AHT30
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

      AppDisplay_UpdateSensor(0.0f, 0.0f, (aht30_ret == 0));
    }
  #endif

    // 启动 OLED 页面轮转定时器
    osal_start_timerEx( zclGenericApp_TaskID, GENERICAPP_DISPLAY_ROTATE_EVT, 5000 );
  }

  #ifdef APP_ROLE_ROUTER
    osal_start_timerEx( zclGenericApp_TaskID, GENERICAPP_READ_SENSOR_EVT, 2000 );
  #endif
    // 所有角色启动心跳
    osal_start_timerEx( zclGenericApp_TaskID, GENERICAPP_HEARTBEAT_EVT, 5000 );
#else
  #ifdef APP_ROLE_COORDINATOR
    AppUart0_Puts("Role: Coordinator\r\n");
    AppUart0_Puts("RX only\r\n");
  #endif
#endif

  osal_start_timerEx( zclGenericApp_TaskID, GENERICAPP_START_COMMISSION_EVT, 1500 );
}

/*********************************************************************
 * @fn      zclGenericApp_event_loop
 *
 * @brief   应用事件循环。
 */
uint16 zclGenericApp_event_loop( uint8 task_id, uint16 events )
{
  afIncomingMSGPacket_t *MSGpkt;

  (void)task_id;

  if ( events & SYS_EVENT_MSG )
  {
    while ( (MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( zclGenericApp_TaskID )) )
    {
      switch ( MSGpkt->hdr.event )
      {
        case ZCL_INCOMING_MSG:
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

            // Coordinator：显式打开 permit join 180s
            gPermitDuration = 180;
            ZMacSetReq(ZMacAssociationPermit, &gPermitDuration);
            AppUart0_Puts("Permit join: 180s\r\n");

            // LED: NWK-LED 保持快闪（等待设备入网）
            AppLed_Set(APP_LED_NWK, APP_LED_BLINK_FAST);

            // 更新 OLED 网络信息
#ifdef APP_FEATURE_OLED
            AppDisplay_UpdateNetwork(
              (uint8)zclGenericApp_NwkState,
              _NIB.nwkPanId,
              _NIB.nwkLogicalChannel,
              _NIB.nwkDevAddress
            );
            AppDisplay_UpdateCoordInfo(gPermitDuration, 0);
#endif
          }
          else if (zclGenericApp_NwkState == DEV_ROUTER)
          {
            AppUart0_Puts("NWK: DEV_ROUTER\r\n");
            AppUart0_Puts("RO ready\r\n");

            // LED: 已入网，NWK-LED 常亮
            AppLed_Set(APP_LED_NWK, APP_LED_ON);

            // 更新显示网络状态
#ifdef APP_ROLE_ROUTER
            AppDisplay_UpdateNetwork(
              (uint8)zclGenericApp_NwkState,
              _NIB.nwkPanId,
              _NIB.nwkLogicalChannel,
              _NIB.nwkDevAddress
            );
#endif
          }
          else if (zclGenericApp_NwkState == DEV_END_DEVICE)
          {
            AppUart0_Puts("NWK: DEV_END_DEVICE\r\n");
          }
          else
          {
            AppUart0_Puts("NWK: state change\r\n");
            // LED: 未入网时 NWK-LED 快闪
            AppLed_Set(APP_LED_NWK, APP_LED_BLINK_FAST);
          }
          break;

        default:
          break;
      }

      osal_msg_deallocate( (uint8 *)MSGpkt );
    }

    return (events ^ SYS_EVENT_MSG);
  }

  if ( events & GENERICAPP_START_COMMISSION_EVT )
  {
#ifdef APP_ROLE_ROUTER
    // Router: NV_RESTORE 可能导致设备假入网。
    // 检测到 NwkState != DEV_INIT 说明有陈旧网络残留，先清除再入网。
    if (zclGenericApp_NwkState != DEV_INIT)
    {
      AppUart0_Puts("Stale NV, clear\r\n");
      bdb_resetLocalAction();
      osal_start_timerEx(zclGenericApp_TaskID, GENERICAPP_START_COMMISSION_EVT, 2000);
      return ( events ^ GENERICAPP_START_COMMISSION_EVT );
    }
    AppUart0_Puts("BDB start: STEER\r\n");
    bdb_StartCommissioning(BDB_COMMISSIONING_MODE_NWK_STEERING);
#elif defined(APP_ROLE_COORDINATOR)
    AppUart0_Puts("BDB start: FORM+STEER\r\n");
    bdb_StartCommissioning(BDB_COMMISSIONING_MODE_NWK_FORMATION | BDB_COMMISSIONING_MODE_NWK_STEERING);
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
#ifdef APP_ROLE_COORDINATOR
      AppUart0_Puts("BDB retry: FORM+STEER\r\n");
      bdb_StartCommissioning(BDB_COMMISSIONING_MODE_NWK_FORMATION | BDB_COMMISSIONING_MODE_NWK_STEERING);
#elif defined(APP_ROLE_ROUTER)
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

#ifdef APP_ROLE_ROUTER
  // Router：清除 NV 陈旧网络后重新发起 STEER
  if ( events & GENERICAPP_CLEAR_NWK_EVT )
  {
    bdb_resetLocalAction();
    AppUart0_Puts("NV cleared, restart STEER in 2s\r\n");
    osal_start_timerEx( zclGenericApp_TaskID, GENERICAPP_START_COMMISSION_EVT, 2000 );
    return ( events ^ GENERICAPP_CLEAR_NWK_EVT );
  }
#endif

#ifdef APP_ROLE_ROUTER
  if ( events & GENERICAPP_READ_SENSOR_EVT )
  {
    if (AHT30_ReadMeasure(&aht30_dev) == 0)
    {
      char display_str[64];
      uint8 pos;

      // 更新显示
      AppDisplay_UpdateSensor(aht30_dev.temperature, aht30_dev.humidity, 1);

      // UART 输出
      pos = 0;
      osal_memcpy(display_str + pos, "Local Temp: ", 12); pos += 12;
      pos += ftoa_2dp(display_str + pos, aht30_dev.temperature);
      osal_memcpy(display_str + pos, " C, Humi: ", 10); pos += 10;
      pos += ftoa_2dp(display_str + pos, aht30_dev.humidity);
      osal_memcpy(display_str + pos, " %\r\n", 4); pos += 4;
      display_str[pos] = '\0';
      AppUart0_Write((const uint8 *)display_str, pos);

      if ( zclGenericApp_NwkState == DEV_ROUTER )
      {
        int16 temp_val = (int16)(aht30_dev.temperature * 100.0f);
        uint16 humi_val = (uint16)(aht30_dev.humidity * 100.0f);
        ZStatus_t txStat = ZFailure;

        afAddrType_t dstAddr;
        dstAddr.addrMode = afAddr16Bit;
        dstAddr.addr.shortAddr = 0x0000;
        dstAddr.endPoint = GENERICAPP_ENDPOINT;

        // 上报温度：cluster 0x0402, attr 0x0000, int16, 值=温度*100
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
        if (txStat == ZSuccess)
        {
          s_tx_ok++;
        }
        else
        {
          s_tx_fail++;
          AppUart0_Puts("TX temp fail\r\n");
        }

        // 上报湿度：cluster 0x0405, attr 0x0000, uint16, 值=湿度*100
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
        if (txStat == ZSuccess)
        {
          s_tx_ok++;
        }
        else
        {
          s_tx_fail++;
          AppUart0_Puts("TX humi fail\r\n");
        }

        // RUN-LED 闪一下（表示有数据包发送）
        AppLed_Set(APP_LED_RUN, APP_LED_FLASH_ONCE);

        // 更新统计显示
        AppDisplay_UpdateStats(s_uptime_s, s_tx_ok, s_tx_fail);
      }
    }
    else
    {
      AppDisplay_UpdateSensor(0.0f, 0.0f, 0);
    }

    osal_start_timerEx( zclGenericApp_TaskID, GENERICAPP_READ_SENSOR_EVT, 2000 );
    return ( events ^ GENERICAPP_READ_SENSOR_EVT );
  }
#endif

  // 心跳：所有 OLED 角色共享（uptime 统计 + 显示刷新）
#ifdef APP_FEATURE_OLED
  if ( events & GENERICAPP_HEARTBEAT_EVT )
  {
    s_heartbeat_cnt++;
    s_uptime_s += 5;

    // Coordinator: permit join 倒计时（每 5 秒减 5，下限 0）
#ifdef APP_ROLE_COORDINATOR
    if (gPermitDuration > 0) {
      if (gPermitDuration >= 5) {
        gPermitDuration -= 5;
      } else {
        gPermitDuration = 0;
        ZMacSetReq(ZMacAssociationPermit, &gPermitDuration);
      }
      AppDisplay_UpdateCoordInfo(gPermitDuration, 0);
    }
#endif

    AppDisplay_UpdateStats(s_uptime_s, s_tx_ok, s_tx_fail);

#ifdef APP_ROLE_COORDINATOR
    AppDisplay_UpdateRemoteAge();
#endif

#ifdef APP_ROLE_ROUTER
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
#endif

    osal_start_timerEx( zclGenericApp_TaskID, GENERICAPP_HEARTBEAT_EVT, 5000 );
    return ( events ^ GENERICAPP_HEARTBEAT_EVT );
  }
#endif

#ifdef APP_FEATURE_OLED
  if ( events & GENERICAPP_DISPLAY_ROTATE_EVT )
  {
    AppDisplay_NextPage();
    osal_start_timerEx( zclGenericApp_TaskID, GENERICAPP_DISPLAY_ROTATE_EVT, 5000 );
    return ( events ^ GENERICAPP_DISPLAY_ROTATE_EVT );
  }

  if ( events & GENERICAPP_DISPLAY_UPDATE_EVT )
  {
    AppDisplay_Refresh();
    return ( events ^ GENERICAPP_DISPLAY_UPDATE_EVT );
  }
#endif

  // LED 处理：所有角色都需要
  if ( events & GENERICAPP_LED_PROCESS_EVT )
  {
    AppLed_Process();
    osal_start_timerEx( zclGenericApp_TaskID, GENERICAPP_LED_PROCESS_EVT, 10 );
    return ( events ^ GENERICAPP_LED_PROCESS_EVT );
  }

  return 0;
}

/*********************************************************************
 * @fn      zclGenericApp_HandleKeys
 *
 * @brief   按键处理。
 *
 * @param   shift, keys
 */
static void zclGenericApp_HandleKeys( byte shift, byte keys )
{
  (void)shift;

#ifdef APP_FEATURE_OLED
  if ( keys & HAL_KEY_SW_1 )
  {
    // 手动切换 OLED 页面（Router 和 CoordinatorNormal 均适用）
    AppDisplay_NextPage();
    // 重置自动轮转定时器
    osal_stop_timerEx(zclGenericApp_TaskID, GENERICAPP_DISPLAY_ROTATE_EVT);
    osal_start_timerEx( zclGenericApp_TaskID, GENERICAPP_DISPLAY_ROTATE_EVT, 5000 );
  }
#endif

  if ( keys & HAL_KEY_SW_2 )
  {
    // 手动启动 BDB commissioning
    bdb_StartCommissioning(BDB_COMMISSIONING_MODE_NWK_FORMATION | BDB_COMMISSIONING_MODE_NWK_STEERING);
  }
}

/*********************************************************************
 * @fn      zclGenericApp_ProcessCommissioningStatus
 *
 * @brief   入网流程状态回调。
 */
static void zclGenericApp_ProcessCommissioningStatus(bdbCommissioningModeMsg_t *bdbCommissioningModeMsg)
{
  switch(bdbCommissioningModeMsg->bdbCommissioningMode)
  {
    case BDB_COMMISSIONING_FORMATION:
      if(bdbCommissioningModeMsg->bdbCommissioningStatus == BDB_COMMISSIONING_SUCCESS)
      {
        AppUart0_Puts("BDB FORM ok\r\n");

        // Coordinator：建网成功后显式打开 permit join
        gPermitDuration = 180;
        ZMacSetReq(ZMacAssociationPermit, &gPermitDuration);
        AppUart0_Puts("Permit join: 180s\r\n");

        bdb_StartCommissioning(BDB_COMMISSIONING_MODE_NWK_STEERING | bdbCommissioningModeMsg->bdbRemainingCommissioningModes);
      }
      else
      {
        AppUart0_Puts("BDB FORM fail\r\n");
        osal_start_timerEx( zclGenericApp_TaskID, GENERICAPP_RETRY_COMMISSION_EVT, 3000 );
      }
    break;

    case BDB_COMMISSIONING_NWK_STEERING:
      if(bdbCommissioningModeMsg->bdbCommissioningStatus == BDB_COMMISSIONING_SUCCESS)
      {
        AppUart0_Puts("BDB STEER ok\r\n");
        s_steer_fail_cnt = 0;
      }
      else
      {
        s_steer_fail_cnt++;
        AppUart0_Puts("BDB STEER fail\r\n");

        // 连续失败 2 次后清除 NV 陈旧网络数据，确保从 DEV_INIT 重新扫描
        if (s_steer_fail_cnt >= 2)
        {
          AppUart0_Puts("CLR NV nwk\r\n");
          osal_start_timerEx(zclGenericApp_TaskID, GENERICAPP_CLEAR_NWK_EVT, 100);
        }
        else
        {
          osal_start_timerEx( zclGenericApp_TaskID, GENERICAPP_RETRY_COMMISSION_EVT, 5000 );
        }
      }
    break;

    case BDB_COMMISSIONING_FINDING_BINDING:
      if(bdbCommissioningModeMsg->bdbCommissioningStatus == BDB_COMMISSIONING_SUCCESS)
      {
      }
      else
      {
      }
    break;

    case BDB_COMMISSIONING_INITIALIZATION:
      if(bdbCommissioningModeMsg->bdbCommissioningStatus == BDB_COMMISSIONING_SUCCESS)
      {
      }
      else
      {
      }
    break;

    case BDB_COMMISSIONING_PARENT_LOST:
#if ZG_BUILD_ENDDEVICE_TYPE
      if(bdbCommissioningModeMsg->bdbCommissioningStatus == BDB_COMMISSIONING_NETWORK_RESTORED)
      {
      }
      else
      {
      }
#endif
    break;
  }
}

/*********************************************************************
 * @fn      zclGenericApp_ProcessIdentifyTimeChange
 *
 * @brief   处理 IdentifyTime 属性变化。
 */
static void zclGenericApp_ProcessIdentifyTimeChange( uint8 endpoint )
{
  (void) endpoint;
  // LED 驱动将在后续阶段实现
}

/*********************************************************************
 * @fn      zclGenericApp_BindNotification
 *
 * @brief   新增绑定时调用。
 */
static void zclGenericApp_BindNotification( bdbBindNotificationData_t *data )
{
  (void)data;
}

/*********************************************************************
 * @fn      zclGenericApp_BasicResetCB
 *
 * @brief   Basic Cluster Reset 回调。
 */
static void zclGenericApp_BasicResetCB( void )
{
  zclGenericApp_ResetAttributesToDefaultValues();
}

/*********************************************************************
 * ZCL Foundation 入站消息处理
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

#ifdef APP_ROLE_COORDINATOR
    case ZCL_CMD_REPORT:
      {
        zclReportCmd_t *pReportCmd = (zclReportCmd_t *)pInMsg->attrCmd;
        uint8 i;
        AppUart0_Puts("RX ZCL REPORT\r\n");

        // RUN-LED 闪一下（表示有数据包接收）
        AppLed_Set(APP_LED_RUN, APP_LED_FLASH_ONCE);

        // 收到 report 说明有设备入网，NWK-LED 常亮
        AppLed_Set(APP_LED_NWK, APP_LED_ON);

        // 更新接收统计
        s_tx_ok++;

#ifdef APP_FEATURE_OLED
        // 记录源地址（仅 OLED 构建需要缓存）
        s_remote_addr = pInMsg->srcAddr.addr.shortAddr;
#endif

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

#ifdef APP_FEATURE_OLED
               s_remote_temp = temp_f;
#endif

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

#ifdef APP_FEATURE_OLED
               s_remote_humi = humi_f;
#endif

               osal_memcpy(display_str + pos, "Remote Humi: ", 13); pos += 13;
               pos += ftoa_2dp(display_str + pos, humi_f);
               osal_memcpy(display_str + pos, " %\r\n", 4); pos += 4;
               display_str[pos] = '\0';
               AppUart0_Write((const uint8 *)display_str, pos);
            }
          }
        }

        // 更新 OLED 远端传感器显示（仅有 OLED 的构建）
#ifdef APP_FEATURE_OLED
        AppDisplay_UpdateRemoteSensor(s_remote_addr, s_remote_temp, s_remote_humi);
        AppDisplay_UpdateStats(s_uptime_s, s_tx_ok, s_tx_fail);
#endif
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
static uint8 zclGenericApp_ProcessInReadRspCmd( zclIncomingMsg_t *pInMsg )
{
  (void)pInMsg;
  return ( TRUE );
}
#endif

#ifdef ZCL_WRITE
static uint8 zclGenericApp_ProcessInWriteRspCmd( zclIncomingMsg_t *pInMsg )
{
  (void)pInMsg;
  return ( TRUE );
}
#endif

static uint8 zclGenericApp_ProcessInDefaultRspCmd( zclIncomingMsg_t *pInMsg )
{
  (void)pInMsg;
  return ( TRUE );
}

#ifdef ZCL_DISCOVER
static uint8 zclGenericApp_ProcessInDiscCmdsRspCmd( zclIncomingMsg_t *pInMsg )
{
  (void)pInMsg;
  return ( TRUE );
}

static uint8 zclGenericApp_ProcessInDiscAttrsRspCmd( zclIncomingMsg_t *pInMsg )
{
  (void)pInMsg;
  return ( TRUE );
}

static uint8 zclGenericApp_ProcessInDiscAttrsExtRspCmd( zclIncomingMsg_t *pInMsg )
{
  (void)pInMsg;
  return ( TRUE );
}
#endif
