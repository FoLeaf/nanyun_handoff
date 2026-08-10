/*********************************************************************
 * app_display.h - OLED 多页显示管理
 *
 * Router: 传感器页 / 网络页 / 统计页
 * Coordinator: 网络页 / 远端传感器页 / 统计页 / 本地传感器页(可选)
 *********************************************************************/

#ifndef APP_DISPLAY_H
#define APP_DISPLAY_H

#include "ZComDef.h"

// 显示角色
#define APP_DISPLAY_ROLE_ROUTER      0
#define APP_DISPLAY_ROLE_COORDINATOR 1

// 页面定义
#define APP_DISPLAY_PAGE_COORD      0   // Coordinator: 网络概览
#define APP_DISPLAY_PAGE_REMOTE     1   // Coordinator: 远端温湿度
#define APP_DISPLAY_PAGE_STATS      2   // 统计页
#define APP_DISPLAY_PAGE_LOCAL      3   // Coordinator: 本地传感器

void AppDisplay_Init(uint8 role);
void AppDisplay_NextPage(void);
void AppDisplay_Refresh(void);

// 通用数据更新接口（Router 和 Coordinator 共用）
void AppDisplay_UpdateStats(uint32 uptime_s, uint16 tx_ok, uint16 tx_fail);

// Router 专用（传感器页）
void AppDisplay_UpdateSensor(float temp, float humi, uint8 aht_ok);

// Router 和 Coordinator 共用（网络页）
void AppDisplay_UpdateNetwork(uint8 nwk_state, uint16 pan_id, uint8 channel, uint16 short_addr);

// Coordinator 专用
void AppDisplay_UpdateCoordInfo(uint8 permit_duration, uint8 node_count);
void AppDisplay_UpdateRemoteSensor(uint16 node_addr, float temp, float humi);
void AppDisplay_UpdateRemoteAge(void);
void AppDisplay_UpdateUartMode(uint8 hex_mode);

#endif /* APP_DISPLAY_H */
