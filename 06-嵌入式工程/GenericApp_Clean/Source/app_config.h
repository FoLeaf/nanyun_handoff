/*********************************************************************
 * app_config.h — 产品目标与功能宏定义
 *
 * IAR 工程只需定义 APP_TARGET_* 宏（如下），其余 APP_ROLE_*、
 * APP_BOARD_*、APP_FEATURE_* 由此文件自动推导。
 *
 * 可选补充开关（在 IAR CCDefines 中追加）：
 *   APP_FEATURE_AHT30 — CoordinatorNormal 板载 AHT30 时使用
 *********************************************************************/

#ifndef APP_CONFIG_H
#define APP_CONFIG_H

/*********************************************************************
 * 目标选择
 *
 * 在 IAR 工程 CCDefines 中定义其中之一：
 *   APP_TARGET_COORDINATOR_DONGLE
 *   APP_TARGET_COORDINATOR_NORMAL
 *   APP_TARGET_ROUTER_NORMAL
 *********************************************************************/

// =================== CoordinatorDongle ===================
#if defined(APP_TARGET_COORDINATOR_DONGLE)

  #define APP_ROLE_COORDINATOR
  #define APP_BOARD_DONGLE
  #define APP_FEATURE_HEX_BRIDGE
  #define APP_FEATURE_DEVICE_TABLE

  // 显式排除：无 OLED、无 AHT30、无 TEXT_LOG

// =================== CoordinatorNormal ===================
#elif defined(APP_TARGET_COORDINATOR_NORMAL)

  #define APP_ROLE_COORDINATOR
  #define APP_BOARD_NORMAL
  #define APP_FEATURE_HEX_BRIDGE
  #define APP_FEATURE_TEXT_LOG
  #define APP_FEATURE_OLED
  #define APP_FEATURE_DEVICE_TABLE
  // APP_FEATURE_AHT30 可选（由 IAR 项目定义）

// =================== RouterNormal ===================
#elif defined(APP_TARGET_ROUTER_NORMAL)

  #define APP_ROLE_ROUTER
  #define APP_BOARD_NORMAL
  #define APP_FEATURE_TEXT_LOG
  #define APP_FEATURE_OLED
  #define APP_FEATURE_AHT30
  #define APP_FEATURE_SENSOR_REPORT

// =================== 错误检测 ===================
#else
  #error "Must define one of: APP_TARGET_COORDINATOR_DONGLE, APP_TARGET_COORDINATOR_NORMAL, or APP_TARGET_ROUTER_NORMAL"

#endif

#endif /* APP_CONFIG_H */
