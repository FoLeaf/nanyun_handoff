/*********************************************************************
 * app_display.c - OLED 多页显示实现
 *
 * Router:     传感器页 → 网络页 → 统计页
 * Coordinator: 网络页 → 远端传感器页 → 统计页 → 本地传感器页
 *********************************************************************/

#include "app_display.h"
#include "oled.h"
#include "OSAL.h"
#include <string.h>

// 网络状态字符串
static const char *nwk_state_str[] = {
  "HOLD", "INIT", "DISC", "JOIN",
  "REJN", "UNAUTH", "ENDDEV", "ROUTER",
  "CO_STRT", "COORD", "ORPHAN"
};

#define NWK_STATE_MAX   11

// 显示数据结构
typedef struct {
  // 网络页（通用）
  uint8  nwk_state;
  uint16 pan_id;
  uint8  channel;
  uint16 short_addr;

  // 统计页（通用）
  uint32 uptime_s;
  uint16 tx_ok;
  uint16 tx_fail;

  // 传感器页（Router）
  float temp;
  float humi;
  uint8 aht_ok;

  // Coordinator 专用
  uint8  permit_duration;   // permit join 剩余秒数
  uint8  node_count;        // 已入网节点数
  uint16 remote_addr;       // 最近上报的 Router 短地址
  float  remote_temp;
  float  remote_humi;
  uint32 remote_report_uptime; // 收到上次 report 时的 uptime 快照
  uint8  uart_hex_mode;     // UART 模式：0=TEXT, 1=HEX
} display_data_t;

static display_data_t g_data = {0};
static uint8 g_role = APP_DISPLAY_ROLE_ROUTER;
static uint8 g_current_page = 0;

/*********************************************************************
 * 辅助函数
 *********************************************************************/
static uint8 ftoa_2dp(char *buf, float val)
{
  uint8 pos = 0;
  int16 int_part;
  uint8 frac;

  if (val < 0.0f) {
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

static uint8 u16_to_dec(char *buf, uint16 val)
{
  char tmp[5];
  uint8 len = 0, i;
  do { tmp[len++] = (char)('0' + (val % 10)); val /= 10; } while (val && len < 5);
  for (i = 0; i < len; i++) buf[i] = tmp[len - 1 - i];
  buf[len] = '\0';
  return len;
}

static uint8 u32_to_dec(char *buf, uint32 val)
{
  char tmp[10];
  uint8 len = 0, i;
  do { tmp[len++] = (char)('0' + (val % 10)); val /= 10; } while (val && len < 10);
  for (i = 0; i < len; i++) buf[i] = tmp[len - 1 - i];
  buf[len] = '\0';
  return len;
}

static uint8 u8_to_dec(char *buf, uint8 val)
{
  char tmp[3];
  uint8 len = 0, i;
  do { tmp[len++] = (char)('0' + (val % 10)); val /= 10; } while (val && len < 3);
  for (i = 0; i < len; i++) buf[i] = tmp[len - 1 - i];
  buf[len] = '\0';
  return len;
}

/*********************************************************************
 * 页面渲染 — Router
 *********************************************************************/
static void render_router_sensor(void)
{
  char buf[20]; uint8 pos;
  OLED_ShowString(0, 0, "=== Sensor ===");

  pos = 0; osal_memcpy(buf + pos, "T: ", 3); pos += 3;
  pos += ftoa_2dp(buf + pos, g_data.temp);
  osal_memcpy(buf + pos, " C", 2); pos += 2; buf[pos] = '\0';
  OLED_ShowString(0, 2, buf);

  pos = 0; osal_memcpy(buf + pos, "H: ", 3); pos += 3;
  pos += ftoa_2dp(buf + pos, g_data.humi);
  osal_memcpy(buf + pos, " %", 2); pos += 2; buf[pos] = '\0';
  OLED_ShowString(0, 4, buf);

  OLED_ShowString(0, 6, g_data.aht_ok ? "AHT30: OK" : "AHT30: ERR");
}

// Router 网络页也共用 render_network_page

/*********************************************************************
 * 页面渲染 — Coordinator
 *********************************************************************/
static void render_coord_page(void)
{
  char buf[20]; uint8 pos;
  OLED_ShowString(0, 0, "=== COORD ===");

  // PAN ID + CH
  pos = 0; osal_memcpy(buf + pos, "PAN:0x", 6); pos += 6;
  pos += u16_to_dec(buf + pos, g_data.pan_id);
  osal_memcpy(buf + pos, " CH:", 4); pos += 4;
  pos += u8_to_dec(buf + pos, g_data.channel);
  buf[pos] = '\0';
  OLED_ShowString(0, 2, buf);

  // Short Addr + Permit Join
  pos = 0; osal_memcpy(buf + pos, "Addr:", 5); pos += 5;
  pos += u16_to_dec(buf + pos, g_data.short_addr);
  osal_memcpy(buf + pos, " J:", 3); pos += 3;
  if (g_data.permit_duration > 0) {
    pos += u8_to_dec(buf + pos, g_data.permit_duration);
    osal_memcpy(buf + pos, "s", 1); pos += 1;
  } else {
    osal_memcpy(buf + pos, "off", 3); pos += 3;
  }
  buf[pos] = '\0';
  OLED_ShowString(0, 4, buf);

  // UART mode
  pos = 0; osal_memcpy(buf + pos, "UART:", 5); pos += 5;
  osal_memcpy(buf + pos, g_data.uart_hex_mode ? "HEX" : "TEXT", g_data.uart_hex_mode ? 3 : 4);
  buf[pos + (g_data.uart_hex_mode ? 3 : 4)] = '\0';
  OLED_ShowString(0, 6, buf);
}

static void render_remote_sensor_page(void)
{
  char buf[20]; uint8 pos;
  uint32 age;
  OLED_ShowString(0, 0, "=== Remote ===");

  if (g_data.remote_addr == 0xFFFF) {
    OLED_ShowString(0, 2, "Node: -none-");
  } else {
    pos = 0; osal_memcpy(buf + pos, "Node:0x", 7); pos += 7;
    pos += u16_to_dec(buf + pos, g_data.remote_addr);
    buf[pos] = '\0';
    OLED_ShowString(0, 2, buf);

    pos = 0; osal_memcpy(buf + pos, "T: ", 3); pos += 3;
    pos += ftoa_2dp(buf + pos, g_data.remote_temp);
    osal_memcpy(buf + pos, " C", 2); pos += 2; buf[pos] = '\0';
    OLED_ShowString(0, 4, buf);

    pos = 0; osal_memcpy(buf + pos, "H: ", 3); pos += 3;
    pos += ftoa_2dp(buf + pos, g_data.remote_humi);
    osal_memcpy(buf + pos, " %", 2); pos += 2; buf[pos] = '\0';
    OLED_ShowString(0, 6, buf);

    // Age: 距上次 report 的秒数，显示在 H 行右侧
    age = (g_data.uptime_s > g_data.remote_report_uptime)
          ? (g_data.uptime_s - g_data.remote_report_uptime) : 0;
    pos = 0; osal_memcpy(buf + pos, "Age:", 4); pos += 4;
    pos += u32_to_dec(buf + pos, age);
    osal_memcpy(buf + pos, "s", 1); pos += 1; buf[pos] = '\0';
    OLED_ShowString(8, 6, buf);
  }
}

/*********************************************************************
 * 页面渲染 — 共用
 *********************************************************************/
static void render_network_page(void)
{
  char buf[20]; uint8 pos;
  const char *state;

  OLED_ShowString(0, 0, "=== Network ===");

  state = (g_data.nwk_state < NWK_STATE_MAX) ? nwk_state_str[g_data.nwk_state] : "???";
  pos = 0; osal_memcpy(buf + pos, "State: ", 7); pos += 7;
  osal_memcpy(buf + pos, state, strlen(state)); pos += strlen(state); buf[pos] = '\0';
  OLED_ShowString(0, 2, buf);

  pos = 0; osal_memcpy(buf + pos, "PAN: 0x", 7); pos += 7;
  pos += u16_to_dec(buf + pos, g_data.pan_id); buf[pos] = '\0';
  OLED_ShowString(0, 4, buf);

  pos = 0; osal_memcpy(buf + pos, "CH:", 3); pos += 3;
  pos += u8_to_dec(buf + pos, g_data.channel);
  osal_memcpy(buf + pos, " Addr:", 6); pos += 6;
  pos += u16_to_dec(buf + pos, g_data.short_addr); buf[pos] = '\0';
  OLED_ShowString(0, 6, buf);
}

static void render_stats_page(void)
{
  char buf[20]; uint8 pos;
  OLED_ShowString(0, 0, "=== Stats ===");

  pos = 0; osal_memcpy(buf + pos, "Up: ", 4); pos += 4;
  pos += u32_to_dec(buf + pos, g_data.uptime_s);
  osal_memcpy(buf + pos, "s", 1); pos += 1; buf[pos] = '\0';
  OLED_ShowString(0, 2, buf);

  pos = 0;
  if (g_role == APP_DISPLAY_ROLE_COORDINATOR) {
    osal_memcpy(buf + pos, "RX OK: ", 7); pos += 7;
  } else {
    osal_memcpy(buf + pos, "TX OK: ", 7); pos += 7;
  }
  pos += u16_to_dec(buf + pos, g_data.tx_ok); buf[pos] = '\0';
  OLED_ShowString(0, 4, buf);

  // Coordinator 显示 node_count，Router 显示 TX Fail
  if (g_role == APP_DISPLAY_ROLE_COORDINATOR) {
    pos = 0; osal_memcpy(buf + pos, "Nodes: ", 7); pos += 7;
    pos += u8_to_dec(buf + pos, g_data.node_count); buf[pos] = '\0';
  } else {
    pos = 0; osal_memcpy(buf + pos, "TX Fail: ", 9); pos += 9;
    pos += u16_to_dec(buf + pos, g_data.tx_fail); buf[pos] = '\0';
  }
  OLED_ShowString(0, 6, buf);
}

/*********************************************************************
 * 公共接口
 *********************************************************************/
void AppDisplay_Init(uint8 role)
{
  g_role = role;
  g_current_page = APP_DISPLAY_PAGE_COORD; // 从第一页开始
  OLED_Init();
  OLED_Clear();

  // 初始默认数据
  g_data.pan_id = 0xFFFF;
  g_data.channel = 0;
  g_data.short_addr = 0xFFFF;
  g_data.remote_addr = 0xFFFF;
  g_data.remote_temp = 0.0f;
  g_data.remote_humi = 0.0f;
  g_data.temp = 0.0f;
  g_data.humi = 0.0f;
  g_data.permit_duration = 0;
  g_data.node_count = 0;
  g_data.remote_report_uptime = 0;
  g_data.uart_hex_mode = 1;  // Coordinator 默认 HEX 模式
  g_data.uptime_s = 0;
  g_data.tx_ok = 0;
  g_data.tx_fail = 0;

  AppDisplay_Refresh();
}

void AppDisplay_NextPage(void)
{
  uint8 page_count;

  if (g_role == APP_DISPLAY_ROLE_COORDINATOR) {
    page_count = APP_DISPLAY_PAGE_LOCAL + 1;  // 4 pages
  } else {
    page_count = APP_DISPLAY_PAGE_STATS + 1;  // 3 pages
  }

  g_current_page = (g_current_page + 1) % page_count;
  OLED_Clear();
  AppDisplay_Refresh();
}

void AppDisplay_Refresh(void)
{
  if (g_role == APP_DISPLAY_ROLE_COORDINATOR) {
    switch (g_current_page) {
      case APP_DISPLAY_PAGE_COORD:  render_coord_page();        break;
      case APP_DISPLAY_PAGE_REMOTE: render_remote_sensor_page(); break;
      case APP_DISPLAY_PAGE_STATS:  render_stats_page();         break;
      case APP_DISPLAY_PAGE_LOCAL:  render_router_sensor();      break;  // 本地传感器
    }
  } else {
    switch (g_current_page) {
      case APP_DISPLAY_PAGE_COORD:  render_router_sensor(); break;  // Router: sensor == page 0
      case APP_DISPLAY_PAGE_REMOTE: render_network_page();  break;  // Router: network == page 1
      case APP_DISPLAY_PAGE_STATS:  render_stats_page();    break;  // Router: stats == page 2
    }
  }
}

/*********************************************************************
 * 数据更新接口
 *********************************************************************/
void AppDisplay_UpdateSensor(float temp, float humi, uint8 aht_ok)
{
  g_data.temp = temp;
  g_data.humi = humi;
  g_data.aht_ok = aht_ok;

  if (g_role == APP_DISPLAY_ROLE_ROUTER && g_current_page == APP_DISPLAY_PAGE_COORD)
    render_router_sensor();
  else if (g_role == APP_DISPLAY_ROLE_COORDINATOR && g_current_page == APP_DISPLAY_PAGE_LOCAL)
    render_router_sensor();
}

void AppDisplay_UpdateNetwork(uint8 nwk_state, uint16 pan_id, uint8 channel, uint16 short_addr)
{
  g_data.nwk_state = nwk_state;
  g_data.pan_id = pan_id;
  g_data.channel = channel;
  g_data.short_addr = short_addr;

  if (g_role == APP_DISPLAY_ROLE_COORDINATOR) {
    if (g_current_page == APP_DISPLAY_PAGE_COORD)
      render_coord_page();
  } else {
    if (g_current_page == APP_DISPLAY_PAGE_REMOTE)
      render_network_page();
  }
}

void AppDisplay_UpdateStats(uint32 uptime_s, uint16 tx_ok, uint16 tx_fail)
{
  g_data.uptime_s = uptime_s;
  g_data.tx_ok = tx_ok;
  g_data.tx_fail = tx_fail;

  if (g_current_page == APP_DISPLAY_PAGE_STATS)
    render_stats_page();
}

void AppDisplay_UpdateCoordInfo(uint8 permit_duration, uint8 node_count)
{
  g_data.permit_duration = permit_duration;
  g_data.node_count = node_count;

  if (g_role == APP_DISPLAY_ROLE_COORDINATOR && g_current_page == APP_DISPLAY_PAGE_COORD)
    render_coord_page();
}

void AppDisplay_UpdateRemoteSensor(uint16 node_addr, float temp, float humi)
{
  g_data.remote_addr = node_addr;
  g_data.remote_temp = temp;
  g_data.remote_humi = humi;
  g_data.remote_report_uptime = g_data.uptime_s;

  if (g_role == APP_DISPLAY_ROLE_COORDINATOR && g_current_page == APP_DISPLAY_PAGE_REMOTE)
    render_remote_sensor_page();
}

void AppDisplay_UpdateRemoteAge(void)
{
  if (g_role == APP_DISPLAY_ROLE_COORDINATOR && g_current_page == APP_DISPLAY_PAGE_REMOTE)
    render_remote_sensor_page();
}

void AppDisplay_UpdateUartMode(uint8 hex_mode)
{
  g_data.uart_hex_mode = hex_mode;

  if (g_role == APP_DISPLAY_ROLE_COORDINATOR && g_current_page == APP_DISPLAY_PAGE_COORD)
    render_coord_page();
}
