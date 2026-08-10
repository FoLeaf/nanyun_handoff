/**************************************************************************************************
  Filename:       zcl_genericapp.h

  Description:    ZigBee GenericApp application header.
**************************************************************************************************/

#ifndef ZCL_GENERICAPP_H
#define ZCL_GENERICAPP_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "zcl.h"

/*********************************************************************
 * CONSTANTS
 */
#define GENERICAPP_ENDPOINT            8
#define GENERICAPP_NUM_GRPS            2

// Application Events
#define GENERICAPP_READ_SENSOR_EVT          0x0001
#define GENERICAPP_HEARTBEAT_EVT            0x0002
#define GENERICAPP_START_COMMISSION_EVT     0x0004
#define GENERICAPP_RETRY_COMMISSION_EVT     0x0008
#define GENERICAPP_CLEAR_NWK_EVT            0x0010
#define GENERICAPP_DISPLAY_ROTATE_EVT       0x0020  // OLED 页面轮转
#define GENERICAPP_DISPLAY_UPDATE_EVT       0x0040  // OLED 数据刷新
#define GENERICAPP_LED_PROCESS_EVT          0x0080  // LED 闪烁处理

/*********************************************************************
 * VARIABLES
 */

extern SimpleDescriptionFormat_t zclGenericApp_SimpleDesc;

extern CONST zclCommandRec_t zclGenericApp_Cmds[];
extern CONST uint8 zclCmdsArraySize;

extern CONST zclAttrRec_t zclGenericApp_Attrs[];
extern CONST uint8 zclGenericApp_NumAttributes;

extern uint16 zclGenericApp_IdentifyTime;
extern uint8  zclGenericApp_IdentifyCommissionState;

/*********************************************************************
 * FUNCTIONS
 */

extern void zclGenericApp_Init( byte task_id );
extern UINT16 zclGenericApp_event_loop( byte task_id, UINT16 events );
extern void zclGenericApp_ResetAttributesToDefaultValues(void);

#ifdef __cplusplus
}
#endif

#endif /* ZCL_GENERICAPP_H */
