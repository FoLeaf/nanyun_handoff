# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 构建与烧录

本项目使用 IAR Embedded Workbench for 8051 编译，无命令行构建系统。

**构建流程：**
1. 在 IAR Embedded Workbench 中打开 `CC2530DB/GenericApp.ewp`
2. 从下拉菜单选择目标配置：
   - `CoordinatorEB` — Coordinator 角色
   - `RouterEB` — Router 角色
   - `EndDeviceEB` — End Device 角色（当前未使用）
3. 按 F7 或 Project → Make All
4. 输出固件：`CC2530DB/<Configuration>/Exe/GenericApp.hex`

**烧录：**
- 使用 SmartRF Flash Programmer + CC Debugger
- 加载 `.hex` 文件到 CC2530 模块
- E18-MS1-PCB 接线：3.3V 供电，SWD 连接（P2.1=SWDIO, P2.2=SWCLK）

**调试手段（无自动化测试）：**
- UART 调试输出（115200 baud，P1.4=RX，P1.5=TX）
- OLED 状态页（仅 Router / CoordinatorNormal）
- Zigbee 抓包（Wireshark + CC2531 sniffer 固件）

## 项目概述

本仓库是基于 Z-Stack 3.0.2 GenericApp 的固件项目，目标硬件为亿佰特 E18-MS1-PCB
模块（TI CC2530）。项目不使用亿佰特出厂固件，而是在 Z-Stack GenericApp 示例基础上
重新实现所需功能。

产品方向是在可控的应用逻辑下复现出厂固件的有用功能：

- 无线侧尽量使用标准 Zigbee 3.0 / ZCL。
- Coordinator 侧添加类似 EBYTE 的 HEX UART 桥接。
- Router 设备支持 AHT30 温湿度采样。
- OLED 轮转显示 Router 网络状态和 AHT30 数据。
- 同时支持 USB dongle Coordinator 和普通开发板 Coordinator 两种硬件形态。

固件最终需支持三个目标产品：

| 目标 | Zigbee 角色 | 硬件形态 | 用途 |
| --- | --- | --- | --- |
| `CoordinatorDongle` | Coordinator | USB dongle | 连接 PC 的 Zigbee USB 网关 |
| `CoordinatorNormal` | Coordinator | 普通开发板 | 独立 Coordinator，带 OLED / 调试资源 |
| `RouterNormal` | Router | 普通开发板 | AHT30 传感器路由，带 OLED 和 ZCL 上报 |

## 架构概览

### OSAL 任务系统

本固件使用 TI 的 **OSAL（Operating System Abstraction Layer）**——协作式任务调度器，
不是抢占式 RTOS。理解 OSAL 是修改本代码库的前提。

**任务注册顺序**（定义在 `OSAL_GenericApp.c`）：
```
macEventLoop → nwk_event_loop → Hal_ProcessEvent → APS_event_loop →
ZDApp_event_loop → zcl_event_loop → bdb_event_loop → zclGenericApp_event_loop
```

每个任务具有：
- 唯一的 `taskID`（在 `osalInitTasks()` 中按序分配）
- 事件位图（`uint16 events`），每个 bit 代表一个待处理事件
- 事件循环函数，处理事件后返回未处理的事件

**常用 OSAL API：**
- `osal_start_timerEx(taskID, eventID, timeout)` — 定时触发事件
- `osal_set_event(taskID, eventID)` — 立即触发事件
- `osal_msg_send(taskID, msgPtr)` — 向其他任务发送消息
- `osal_msg_receive(taskID)` — 在事件循环中接收消息

**GenericApp 事件**（定义在 `zcl_genericapp.h`）：
```c
#define GENERICAPP_READ_SENSOR_EVT      0x0001  // Router：读取 AHT30
#define GENERICAPP_HEARTBEAT_EVT        0x0002  // Router：发送心跳
#define GENERICAPP_START_COMMISSION_EVT 0x0004  // 所有角色：启动 BDB commissioning
#define GENERICAPP_DISPLAY_UPDATE_EVT   0x0008  // Router/CoordNormal：刷新 OLED
```

### 基于角色的条件编译

代码库使用编译期宏在 Coordinator 和 Router 之间切换：

- `ZG_BUILD_RTRONLY_TYPE=TRUE` → Router 构建（启用 AHT30、OLED、传感器上报）
- `ZG_BUILD_COORD_TYPE=TRUE` → Coordinator 构建（启用接收上报、设备表）

**注意：** 这些宏在 IAR 工程文件中按配置设置，不在源码中定义。不要直接使用
`#ifdef ZG_BUILD_RTRONLY_TYPE` 来条件编译 Router 专属功能——应使用"推荐的功能宏"
一节中定义的项目级宏。

### BDB Commissioning 流程

应用使用 **BDB（Base Device Behavior）** 进行 Zigbee 3.0 组网：

1. `zclGenericApp_Init()` 在 1500ms 后触发 `GENERICAPP_START_COMMISSION_EVT`
2. 事件处理函数调用 `bdb_StartCommissioning(BDB_COMMISSIONING_MODE_NWK_FORMATION | BDB_COMMISSIONING_MODE_NWK_STEERING)`
3. Coordinator：先建网，再打开 permit join（默认 180 秒）
4. Router：启动 steering 加入已有网络
5. 回调：`zclGenericApp_ProcessCommissioningStatus()` 处理成功/失败

### 事件循环模式

主应用逻辑在 `zclGenericApp_event_loop()` 中：

```c
uint16 zclGenericApp_event_loop(uint8 task_id, uint16 events) {
  if (events & SYS_EVENT_MSG) {
    // 处理入站消息（ZCL、按键、ZDO 状态变更）
    while ((MSGpkt = osal_msg_receive(zclGenericApp_TaskID))) {
      switch (MSGpkt->hdr.event) {
        case ZCL_INCOMING_MSG: /* 处理 ZCL */ break;
        case KEY_CHANGE: /* 处理按键 */ break;
        case ZDO_STATE_CHANGE: /* 处理网络状态 */ break;
      }
      osal_msg_deallocate((uint8 *)MSGpkt);
    }
    return events ^ SYS_EVENT_MSG;
  }

  if (events & GENERICAPP_READ_SENSOR_EVT) {
    // Router：读取 AHT30，更新 OLED，发送 ZCL report
    osal_start_timerEx(zclGenericApp_TaskID, GENERICAPP_READ_SENSOR_EVT, 10000);
    return events ^ GENERICAPP_READ_SENSOR_EVT;
  }

  // ... 其他事件

  return 0;  // 所有事件已处理
}
```

**重要：** 事件处理函数必须返回 `events ^ EVENT_FLAG` 来清除已处理的位。
返回 `0` 表示所有事件都已处理。不要返回未处理的事件，除非你需要它在下一轮
重新处理。

## 仓库布局

关键文件：

- `Source/zcl_genericapp.c`
  主 GenericApp 任务。包含 commissioning、UART 输出、Router 的 AHT30/OLED
  行为、ZCL report 发送、Coordinator 的 report 解析、LED 控制、事件循环。
- `Source/zcl_genericapp.h`
  GenericApp 端点和 OSAL 事件定义。事件位包括：
  `READ_SENSOR`(0x0001), `HEARTBEAT`(0x0002), `START_COMMISSION`(0x0004),
  `RETRY_COMMISSION`(0x0008), `CLEAR_NWK`(0x0010),
  `DISPLAY_ROTATE`(0x0020), `DISPLAY_UPDATE`(0x0040), `LED_PROCESS`(0x0080)。
- `Source/zcl_genericapp_data.c`
  ZCL simple descriptor、cluster 列表和属性列表。
- `Source/OSAL_GenericApp.c`
  OSAL 任务注册顺序。
- `Source/app_led.c`, `Source/app_led.h`
  E18 LED 驱动。P1.3=RUN_LED（收发包闪一下），P1.2=NWK_LED（未入网 10Hz 快闪，入网常亮）。
  支持模式：OFF, ON, BLINK_FAST(10Hz), FLASH_ONCE(100ms)。
  LED 处理通过 10ms 定时器驱动。
- `Source/app_display.c`, `Source/app_display.h`
  Router OLED 三页轮转显示。每 5s 自动切换页面。
  页面：传感器页（T/H/AHT状态）、网络页（State/PAN/CH/Addr）、统计页（Uptime/TX OK/TX Fail）。
  通过 `AppDisplay_Update*()` 接口更新数据，仅刷新当前页面。
- `Source/aht30.c`, `Source/aht30.h`, `Source/aht30_port.c`,
  `Source/aht30_port.h`
  AHT30 驱动和 CC2530 模拟 I2C 端口绑定。
- `Source/oled.c`, `Source/oled.h`, `Source/oledFont.c`,
  `Source/oledFont.h`
  SSD1306 OLED 驱动和字库数据。
- `Source/myiic.c`, `Source/myiic.h`
  软件 I2C 实现。当前引脚：P0.1=SCL，P0.0=SDA。
- `CC2530DB/GenericApp.ewp`
  IAR 工程文件，包含 Coordinator/Router/EndDevice 构建配置。
  App 组包含：aht30, app_led, app_display, myiic, oled, zcl_genericapp 等。
- `Zigbee_联调压缩记录.md`
  之前的联调笔记。修改通信行为前务必阅读。

工作树可能包含未提交的修改。除非明确要求，不要回退或覆盖。

## 已确认的现有行为

当前项目已有可工作的基线：

- Router 仅在编译为 Router 时初始化 OLED 和 AHT30。
- Router 定期读取 AHT30 并在本地 OLED 上显示温湿度。
- Router 使用 ZCL Report Attributes 向 Coordinator 发送温湿度。
- Coordinator 在当前分工中不初始化 OLED/AHT30/I2C。
- Coordinator 接收 ZCL report 并通过 UART 打印远端温湿度。
- UART 输出通过直接操作 UART0 寄存器实现，引脚为 P1.4/P1.5。
- 已知的网络默认值（来自之前的联调笔记）：
  - `DEFAULT_CHANLIST=0x00000800`（信道 11）
  - `ZDAPP_CONFIG_PAN_ID=0xFFFF`（自动 PAN ID）
- 已发现的重要编译标志：
  - Router 必须启用 `BDB_REPORTING`。
  - Coordinator 必须启用 `ZCL_REPORT_DESTINATION_DEVICE`。

重构时不要回归这些行为。

## 产品目标

### CoordinatorDongle

PC 侧 Zigbee USB 网关。

硬性边界：

- 不得初始化 OLED。
- 不得初始化 AHT30。
- 不得初始化共享的软件 I2C 总线。
- 默认使用 EBYTE 兼容的 HEX UART 桥接模式。
- HEX 桥接模式下不得输出可读文本日志。

预期能力：

- 以 Coordinator 身份组建 Zigbee 网络。
- 打开和关闭 permit-join 窗口。
- 接收 Router 温湿度 ZCL report。
- 将选定的 Zigbee 事件转换为类似 EBYTE 的 HEX 异步帧。
- 接受主机 PC 发来的 EBYTE 风格 HEX 命令。
- 维护已入网节点的设备表。
- 支持 ZDO 端点 / simple descriptor 查询。
- 支持选定的 ZCL 读/写/控制桥接命令。

### CoordinatorNormal

普通开发板 Coordinator，硬件资源与 RouterNormal 同级。

硬性边界：

- 必须能脱离 PC 独立运行。
- 可以初始化 OLED。
- 可以在编译期功能开关后初始化 AHT30。
- 必须保持 UART 模式纪律：文本日志和 HEX 帧不得混合。

预期能力：

- 与 CoordinatorDongle 相同的 Zigbee Coordinator 行为。
- OLED 网络状态显示：
  - 角色
  - 网络状态
  - PAN ID
  - 信道
  - 短地址
  - Permit-join 剩余时间
  - 已入网节点数
- OLED 远端数据显示：
  - 最新 Router 温度
  - 最新 Router 湿度
  - 上次 report 的时间差或 report 计数
  - TX/RX 失败计数（可用时）
- 可选的本地 AHT30 显示页（如果板子上有传感器）。
- UART 模式：
  - `TEXT_LOG`：可读调试日志和简单 CLI。
  - `HEX_BRIDGE`：类似 EBYTE 的二进制 HEX 桥接。

### RouterNormal

传感器/路由设备。

硬性边界：

- 必须初始化 OLED。
- 必须初始化 AHT30。
- 必须以 Router 身份加入已有 Coordinator 网络。
- 必须使用标准 ZCL 温湿度 cluster 作为主要传感器路径。

预期能力：

- 定期采样 AHT30。
- 缓存最新温湿度值。
- OLED 轮转显示页面：
  - 传感器页：温度、湿度、采样状态。
  - 网络页：已入网/未入网、PAN ID、信道、短地址、父节点或 Coordinator 地址（可用时）。
  - 统计页：运行时间、report 成功次数、report 失败次数。
- 使用标准 ZCL 上报传感器值：
  - Temperature Measurement cluster `0x0402`
  - Relative Humidity cluster `0x0405`
- 可选调试文本日志与未来的 HEX 桥接模式保持分离。

## 推荐的功能宏

引入项目级功能宏，而非在应用代码中分散角色检查。

建议的角色/板型宏：

```c
APP_ROLE_COORDINATOR
APP_ROLE_ROUTER

APP_BOARD_DONGLE
APP_BOARD_NORMAL
```

建议的功能宏：

```c
APP_FEATURE_HEX_BRIDGE
APP_FEATURE_TEXT_LOG
APP_FEATURE_OLED
APP_FEATURE_AHT30
APP_FEATURE_SENSOR_REPORT
APP_FEATURE_COORD_DISPLAY
APP_FEATURE_DEVICE_TABLE
APP_FEATURE_EBYTE_FC08
```

建议的目标组合：

```text
CoordinatorDongle:
  APP_ROLE_COORDINATOR
  APP_BOARD_DONGLE
  APP_FEATURE_HEX_BRIDGE
  APP_FEATURE_DEVICE_TABLE

CoordinatorNormal:
  APP_ROLE_COORDINATOR
  APP_BOARD_NORMAL
  APP_FEATURE_HEX_BRIDGE
  APP_FEATURE_TEXT_LOG
  APP_FEATURE_OLED
  APP_FEATURE_COORD_DISPLAY
  APP_FEATURE_DEVICE_TABLE
  APP_FEATURE_AHT30 optional

RouterNormal:
  APP_ROLE_ROUTER
  APP_BOARD_NORMAL
  APP_FEATURE_TEXT_LOG
  APP_FEATURE_OLED
  APP_FEATURE_AHT30
  APP_FEATURE_SENSOR_REPORT
```

不要用硬件初始化的副作用来暗示产品目标。目标必须通过宏明确指定。

## UART 模式规则

UART 行为具有安全关键性，因为 HEX 桥接是二进制帧协议。

CoordinatorDongle：

- 始终默认 `HEX_BRIDGE`。
- 不得输出纯文本。
- 如需调试输出，必须编码为 EBYTE 风格的异步调试帧。

CoordinatorNormal：

- 支持 `TEXT_LOG` 和 `HEX_BRIDGE`。
- 模式应存储在 NV 中以经受复位。
- 提供硬件回退方式切换模式，例如开机时长按按键 3 秒。
- OLED 应显示当前 UART 模式。

建议的运行时切换：

- 在 `TEXT_LOG` 模式下，接受简单命令如 `mode hex`。
- 在 `HEX_BRIDGE` 模式下，接受私有本地配置命令如 `CFG_UART_MODE` 切回文本。
- 在 `HEX_BRIDGE` 模式下不得解析自由格式文本。
- 在 `HEX_BRIDGE` 模式下不得输出可读日志。

RouterNormal：

- 开发阶段可以使用文本日志。
- 如果以后添加 HEX 模式，须遵守同样的不混合输出规则。

## 亿佰特出厂固件内容摘要

参考文档：

- `D:/nanyun/ebyte/E18-MS1-PCB/E18_Series_ZigBee3.0_UserManual_CN_v1.9.pdf`
- `D:/nanyun/ebyte/E18-MS1-PCB/%e4%ba%bf%e4%bd%b0%e7%89%b9ZigBee3.0%e6%a8%a1%e7%bb%84HEX%e5%91%bd%e4%bb%a4%e6%a0%87%e5%87%86%e8%a7%84%e8%8c%83_V1.7.pdf`
- `D:/nanyun/ebyte/E18-MS1-PCB/AN2022020_HEX%e6%8c%87%e4%bb%a4%e6%a8%a1%e5%bc%8f%e4%b8%8b%e7%9a%84%e6%a8%a1%e7%bb%84%e9%85%8d%e7%bd%91%e4%b8%8e%e6%a8%a1%e7%bb%84%e8%af%86%e5%88%ab.pdf`
- `D:/nanyun/ebyte/E18-MS1-PCB/AN2024001_%e4%ba%bf%e4%bd%b0%e7%89%b9Zigbee+3.0%e6%a8%a1%e7%bb%84%e6%8e%a5%e5%85%a5%e7%ac%ac%e4%b8%89%e6%96%b9%e8%ae%be%e5%a4%87%e6%95%99%e7%a8%8b.pdf`
- `D:/nanyun/ebyte/E18-MS1-PCB/0_Zigbee3.0%e6%a8%a1%e5%9d%97%e7%9a%84%e4%b8%89%e7%a7%8d%e4%b8%b2%e5%8f%a3%e6%a8%a1%e5%bc%8f%e5%88%87%e6%8d%a2_V1.0.pdf`
- `D:/nanyun/ebyte/E18-MS1-PCB/20236141338249427.pdf`

出厂固件重要行为：

- E18 支持 HEX 命令模式和透传模式。
- AT 模式主要适用于 E180 系列，不是 E18 的主要目标。
- 出厂固件相当于 UART 到 Zigbee/ZDO/ZCL 的网关。
- Zigbee 3.0 commissioning 使用 BDB formation 和 steering。
- Coordinator 默认打开 180 秒的 permit join 窗口。
- Router 和 End Device 在 Coordinator permit 窗口打开时通过启动 steering 入网。
- 出厂固件支持网络自愈和地址更新。
- 出厂固件支持广播、组播和单播传输。
- 出厂固件支持可配置的信道和 PAN ID。
- 出厂固件支持通过 IEEE/MAC 地址查询短地址。
- 出厂固件支持复位、退网和恢复出厂设置。
- 出厂固件支持 P1.7 网络按键：
  - 未入网时短按：启动入网或建网。
  - 已入网时短按：快速配对。
  - 已入网时长按：退出当前网络。
- E18 模块 UART 引脚：
  - P1.5 = UART0 TX
  - P1.4 = UART0 RX
- E18 状态引脚（来自手册）：
  - P1.3 = RUN_LED，低电平有效，网络状态指示。
  - P1.2 = NWK_LED，低电平有效，一键配对指示。

## EBYTE HEX 协议摘要

EBYTE HEX 协议帧格式：

```text
SFD      1 字节   0x55
LEN      1 字节   负载长度
PAYLOAD  LEN 字节：
  Type   1 字节
  Code   1 字节
  Data   0..252 字节
  Check  1 字节
```

校验和：

```text
Check = Type XOR Code XOR Data[0] XOR ... XOR Data[N-1]
```

负载最小为 3 字节：Type、Code、Check。

命令类型映射：

| Type | 名称 | 含义 |
| --- | --- | --- |
| `0x00` | `TYPE_CFG` | 本地配置命令/响应 |
| `0x01` | `TYPE_ZDO_REQ` | ZDO 网络管理请求/响应 |
| `0x02` | `TYPE_ZCL_SEND` | ZCL 发送请求/响应 |
| `0x80` | `TYPE_NOTIFY` | 系统异步通知 |
| `0x81` | `TYPE_ZDO_RSP` | ZDO 异步响应 |
| `0x82` | `TYPE_ZCL_IND` | ZCL 异步指示 |
| `0x8F` | `TYPE_SEND_CNF` | 无线发送确认 |

通用协议规则：

- 输入命令的响应仅表示命令在本地被接受。
- 无线命令后续可能产生 `TYPE_SEND_CNF`。
- 无线 ZDO 请求后续可能产生 `TYPE_ZDO_RSP`。
- 无线 ZCL 请求后续可能产生 `TYPE_ZCL_IND`。
- 入站 Zigbee 消息可以在任何时刻产生异步帧。
- HEX 桥接模式下，主机必须将 UART 输出视为随机异步帧事件。

出厂协议的无效/错误行为：

- 不支持的输入命令返回相同 Type/Code，无数据。
- 校验和错误返回异步帧 `55 03 FF FE 01`。
- 帧超时或破损包返回异步帧 `55 03 FF FF 00`。

EBYTE HEX 帧中所有 Zigbee 地址字段使用小端字节序。

## 需要复现的出厂本地配置命令

先实现一个有用的子集。完整的出厂协议很大，不应被视为第一个里程碑的必须项。

优先实现的本地配置命令：

| Code | 名称 | 所需行为 |
| --- | --- | --- |
| `0x00` | `CFG_STATUS` | 查询本地入网状态、节点类型、MAC、信道、PAN ID、短地址、扩展 PAN ID，以及可选的网络密钥 |
| `0x02` | `CFG_OPEN_NET` | Coordinator 打开 permit join；Router 启动 steering |
| `0x03` | `CFG_CLOSE_NET` | Coordinator 关闭 permit join |
| `0x04` | `CFG_RESET` | 复位、退网或恢复出厂设置 |
| `0x05` | `CFG_NODE_TYPE` | 可选；仅在实现运行时角色切换时有用 |
| `0x06` | `CFG_CHANNEL` | 在 formation/join 前查询/设置启用的信道 |
| `0x07` | `CFG_GET_PANID` | 查询配置的 PAN ID |
| `0x08` | `CFG_SET_PANID` | 在 formation/join 前设置 PAN ID |
| `0x09` | `CFG_VIEW_GROUP` | 后续：查看本地组 |
| `0x0A` | `CFG_ADD_GROUP` | 后续：添加本地组 |
| `0x0B` | `CFG_REMOVE_GROUP` | 后续：移除本地组 |
| `0x10` | `CFG_READ_ATTR` | 后续：读取 EBYTE 本地属性 |
| `0x11` | `CFG_WRITE_ATTR` | 后续：写入 EBYTE 本地属性，特别是 `sendMode` |
| `0x14` | `CFG_FIND_BIND` | 后续：自动绑定 |
| `0x28` | `CFG_EZ_MODE` | 后续：请求设备信息通知重传 |

重要边界：

- `CFG_NODE_TYPE` 在第一版实现中可以返回不支持或编译期固定。当前 IAR 工程
  对 Coordinator 和 Router 使用独立的构建配置；运行时角色切换是一个大功能，
  不得假设为可用。
- 在 `CFG_STATUS` 中暴露网络密钥是敏感的。在返回真实密钥字节前应优先使用
  编译期调试开关。

## 需要复现的出厂系统通知

优先实现的 `TYPE_NOTIFY` 异步帧：

| Code | 名称 | 所需行为 |
| --- | --- | --- |
| `0x00` | `NOTIFY_BOOT` | HEX 模式下上电时输出，包含复位原因/版本/MAC |
| `0x01` | `NOTIFY_NET_STATUS` | 网络状态变更时输出 |
| `0x02` | `NOTIFY_NET_OPEN` | 输出 permit-join 剩余窗口；0 表示已关闭 |
| `0x03` | `NOTIFY_NODE_JOIN` | Coordinator 检测到节点入网/认证 |
| `0x04` | `NOTIFY_NODE_ADDR` | 节点短地址通告/更新 |
| `0x05` | `NOTIFY_DEVICE_JOIN` | 设备端点/profile/device ID/cluster 信息 |
| `0x06` | `NOTIFY_LEAVE` | 节点退网通知 |
| `0x0F` | `NOTIFY_DEBUG` | 可选调试帧；在 HEX 模式下用此替代文本 |

出厂文档解读中必须影响设计的要点：

- `NOTIFY_NODE_JOIN` 表示节点通过了 Coordinator 认证。不一定代表稳定的应用层入网。
- `NOTIFY_NODE_ADDR` 基于 Device Announce，在广播密集的入网过程中可能丢失。
- `NOTIFY_DEVICE_JOIN` 是 Coordinator 已识别应用端点和 cluster 的最佳标志。
- 检测到节点入网后，延迟约 5 秒，发出查询型 ZDO 请求以验证节点确实可达。

## ZDO 桥接范围

优先实现的 ZDO 命令：

| Code | 名称 | 用途 |
| --- | --- | --- |
| `0x00` | `ZDO_NWK_ADDR_REQ` | 通过 IEEE 地址查询短地址 |
| `0x01` | `ZDO_IEEE_ADDR_REQ` | 通过短地址查询 IEEE 地址 |
| `0x04` | `ZDO_SIMPLE_DESC_REQ` | 查询端点 profile/device/cluster 列表 |
| `0x05` | `ZDO_ACTIVE_EP_REQ` | 查询活跃端点 |
| `0x21` | `ZDO_BIND_REQ` | 后续：将 report cluster 绑定到 Coordinator |
| `0x22` | `ZDO_UNBIND_REQ` | 后续：解绑 |
| `0x33` | `ZDO_MGMT_BIND_REQ` | 后续：查看绑定表 |
| `0x34` | `ZDO_MGMT_LEAVE_REQ` | 后续：移除节点 |

Coordinator 设备表至少应跟踪：

- IEEE/MAC 地址
- 短地址
- 父节点短地址（已知时）
- 节点类型
- 上次入网模式
- 上次可见时间戳或 OSAL 时间
- 端点数量
- 端点列表
- 每个端点：
  - Profile ID
  - Device ID
  - Device version
  - Input cluster 列表
  - Output cluster 列表
- RouterNormal 节点的最新温湿度

## ZCL 桥接范围

无线负载首先保持标准兼容。

优先实现的 ZCL 桥接命令：

| Code | 名称 | 用途 |
| --- | --- | --- |
| `0x00` | `ZCL_READ_ATTR_REQ/RSP` | 读取 ZCL 属性 |
| `0x01` | `ZCL_WRITE_ATTR_REQ/RSP` | 写入 ZCL 属性 |
| `0x03` | `ZCL_WRITE_REPORT_REQ/RSP` | 后续：配置上报 |
| `0x0A` | `ZCL_REPORT_IND` | 输出接收到的属性 report |
| `0x0B` | `ZCL_DEFAULT_RSP` | 输出默认响应 |
| `0x0F` | `ZCL_CMD_SEND/IND` | 发送/接收 cluster 特定命令 |

RouterNormal 温湿度：

- 使用标准 Temperature Measurement cluster `0x0402`。
- 使用标准 Relative Humidity cluster `0x0405`。
- 测量值的 Attribute ID 为 `0x0000`。
- 温度值为有符号 int16，单位 0.01 摄氏度。
- 湿度值为 uint16，单位 0.01%。
- CoordinatorDongle 应将接收到的 ZCL report 转换为 EBYTE 风格的
  `TYPE_ZCL_IND / ZCL_REPORT_IND` 帧。
- CoordinatorNormal 应在 OLED 上显示这些值，也可以根据 UART 模式输出 HEX 或文本。

## EBYTE 私有透传范围

不要将透传作为主要的传感器上报路径。

出厂私有 cluster：

```text
Cluster ID: 0xFC08
Manufacturer code: 0x2000
```

重要属性：

| Attr ID | 名称 | 类型 | 含义 |
| --- | --- | --- | --- |
| `0x0000` | `Baud` | `uint32` | 波特率 |
| `0x0001` | `targetAddr` | `uint16` | 默认目标短地址 |
| `0x0002` | `targetEP` | `uint8` | 默认目标端点 |
| `0x0003` | `sendMode` | `bool` | 0=HEX 命令模式，1=透传模式 |
| `0x0005` | `target IEEE` | `EUI64` | 上次/默认目标 IEEE |

重要命令：

| Cmd ID | 名称 | 方向 | 含义 |
| --- | --- | --- | --- |
| `0x00` | `UartSend` | Client → Server | 透传负载发送 |
| `0x00` | `Data Notify` | Server → Client | 透传负载接收 |
| `0x01` | `SetDstAddr` | Client → Server | 设置默认目标 |
| `0x02` | `SetBaud` | Client → Server | 设置波特率，需要复位 |

透传模式应作为后续里程碑。实现时：

- `+++` 应从透传模式返回 HEX 模式。
- E18 在透传模式下可能不回复 `+++`；用户界面需设计硬件回退。
- 普通开发板应在 OLED 上显示当前串口模式。

## OLED 界面需求

通用 OLED 规则：

- 屏幕更新保持事件驱动或通过 OSAL 定时器周期执行。
- 不要让长时间 I2C 传输阻塞 Zigbee 任务。
- 不要不必要地重绘未变更的页面。
- 页面文本在 128x64 SSD1306（8x16 字体）下保持足够简短。
- 中文字体渲染视为可选；ASCII 状态页更可靠。

RouterNormal 页面：

```text
Page 1: 传感器
  T: xx.xx C
  H: xx.xx %
  AHT: OK/ERR
  RPT: ok/fail

Page 2: 网络
  Role: ROUTER
  NWK: JOIN/INIT/...
  PAN: xxxx CH:xx
  ADR: xxxx

Page 3: 统计
  Up: xxxx s
  TX OK: xxxx
  TX FAIL: xxxx
  Seq: xx
```

CoordinatorNormal 页面：

```text
Page 1: Coordinator
  Role: COORD
  PAN: xxxx CH:xx
  ADR: 0000
  Join: xxx s/off

Page 2: 远端传感器
  Node: xxxx
  T: xx.xx C
  H: xx.xx %
  Age: xx s

Page 3: 网络表
  Nodes: xx
  Last: xxxx
  UART: TEXT/HEX
  Mode: NORMAL

Page 4: 本地传感器（可选）
  Local T: xx.xx C
  Local H: xx.xx %
```

CoordinatorDongle 无 OLED 页面。

## Commissioning 需求

统一使用 BDB commissioning：

- Coordinator：
  - 如未建网，运行 `BDB_COMMISSIONING_MODE_NWK_FORMATION`。
  - 然后运行 `BDB_COMMISSIONING_MODE_NWK_STEERING` 打开 permit join。
- Router：
  - 运行 `BDB_COMMISSIONING_MODE_NWK_STEERING`。
  - 未入网则重试。
- Permit join 默认值应尽量匹配出厂行为：
  - 出厂文档使用 180 秒。
  - 如果 Z-Stack 默认值不同，需明确跟踪并显示实际时长。
- 关闭网络应使用正确的 permit-join 关闭机制。
- 成功入网后应停止重试。

网络显示应使用 Z-Stack 状态：

- `zclGenericApp_NwkState`
- `_NIB.nwkPanId`
- `_NIB.nwkLogicalChannel`
- `_NIB.nwkDevAddress`
- 扩展 PAN ID（通过 Z-Stack API/结构可用时）
- IEEE 地址（通过 Z-Stack API，如 `NLME_GetExtAddr()`，可用时）

## 实现阶段

### 当前进度（2026-05-28）

**阶段 1 已完成**，关键 commit 记录：

| Commit | 内容 |
|--------|------|
| `c5be6d0` | feat: AHT30/OLED/UART 基线功能 |
| `3c29e8c` | refactor: 移除 862 行 GenericApp 模板死代码（LCD/TouchLink/空壳） |
| `eec9a81` | fix: 修复组网不稳定的三个 bug（NV 陈旧网络 + Router 不重试 + permit join 未打开） |
| `acc8cb9` | feat: Phase 1 - LED 驱动 + OLED 三页轮转 + TX 统计 |
| `a6e7882` | feat: LED 行为更新 - NWK-LED 10Hz 快闪, RUN-LED 收发包闪烁 |
| `627907e` | fix: LED 处理在 APP_ROLE_ROUTER 守卫内导致 Coordinator LED 不工作 |
| `ac8579f` | fix: Router NV_RESTORE 假组网（NwkState != DEV_INIT 时清除再重入） |
| `4cfe714` | feat: Phase 2 - 定义产品目标宏 (APP_TARGET/APP_ROLE/APP_FEATURE) |
| `72958dc` | docs: 补记 Phase 2 完成状态 |
| `4a338ce` | feat: IAR 工程配置 - CoordinatorDongle/CoordinatorNormal/RouterNormal |

**已完成的阶段 1 内容：**

1. **清理死代码** — 移除 LCD/TouchLink/死事件/空壳函数，净减 862 行
2. **修复组网** — Router NV 陈旧网络自动清除重试 + Coordinator 显式 permit join
3. **LED 驱动** (`app_led.c/h`) — P1.3 RUN_LED, P1.2 NWK_LED
   - NWK-LED: 未入网 10Hz 快闪，入网/有设备后常亮
   - RUN-LED: 收/发包时闪一下（100ms）
4. **OLED 三页轮转** (`app_display.c/h`) — 每 5s 自动切换
   - 传感器页: T/H + AHT30 状态
   - 网络页: State/PAN/CH/Addr（使用 `_NIB` 数据）
   - 统计页: Uptime/TX OK/TX Fail
5. **采样与显示分离** — 通过 `AppDisplay_Update*()` 接口
6. **TX 统计** — `s_tx_ok` / `s_tx_fail` 计数器
7. **按键** — SW1 切换 OLED 页面，SW2 手动 BDB commissioning

**已完成的阶段 2 内容：**

1. **产品目标宏** (`app_config.h`) — `APP_TARGET_*` → 自动推导 `APP_ROLE_*` / `APP_BOARD_*` / `APP_FEATURE_*`
2. **IAR 工程配置** — CoordinatorDongle / CoordinatorNormal / RouterNormal 三个配置就绪
3. **条件编译** — Dongle 构建排除 OLED/AHT30/I2C，普通开发板构建包含
4. **移除旧宏依赖** — `ZG_BUILD_RTRONLY_TYPE` 等 Z-Stack 宏已从应用代码中替换

**下一步：阶段 3 — CoordinatorNormal OLED 显示**

### 阶段 1：稳定 RouterNormal ✅

- ✅ 保持 Router 专属的 AHT30/OLED 初始化。
- ✅ 将 AHT30 采样与 OLED 页面刷新分离。
- ✅ 添加 Router OLED 网络状态页。
- ✅ 添加 Router OLED 统计页。
- ✅ 保持现有的 ZCL report 路径到 Coordinator。
- ✅ 确认 Coordinator 仍能接收 `RX ZCL REPORT`。

### 阶段 2：定义产品目标宏 ✅

- ✅ `app_config.h` 定义产品目标宏 (`APP_TARGET_*` → `APP_ROLE_*` / `APP_BOARD_*` / `APP_FEATURE_*`)
- ✅ CoordinatorDongle 构建排除 OLED/AHT30/I2C 代码路径
- ✅ 编译期角色边界清晰
- ✅ 移除 LCD_SUPPORTED/LEGACY_LCD_DEBUG 残留

### 阶段 2：定义产品目标 ✅

- ✅ 添加 `app_config.h`，定义三个产品目标宏（CoordinatorDongle/CoordinatorNormal/RouterNormal）
- ✅ `APP_TARGET_*` → 自动推导 `APP_ROLE_*` / `APP_BOARD_*` / `APP_FEATURE_*`
- ✅ CoordinatorDongle 构建排除 OLED/AHT30/I2C
- ✅ 普通开发板构建（CoordinatorNormal/RouterNormal）包含 OLED 和可选 AHT30
- ✅ 编译期角色边界清晰，移除 `ZG_BUILD_RTRONLY_TYPE` 等 Z-Stack 宏在应用代码中的使用

### 阶段 3：CoordinatorNormal 显示

- 仅为 `APP_BOARD_NORMAL` 添加 Coordinator OLED 初始化。
- 添加 Coordinator 网络页。
- 添加最新远端温湿度页。
- 添加 UART 模式显示。
- 保持 CoordinatorDongle 不受影响。

### 阶段 4：最小 EBYTE HEX 桥接

实现：

- 帧解析器：`0x55 + LEN + payload + XOR8`。
- 帧构建器。
- 无效校验和和超时响应。
- `CFG_STATUS`。
- `CFG_OPEN_NET`。
- `CFG_CLOSE_NET`。
- `CFG_RESET`。
- `CFG_GET_PANID`。
- `CFG_SET_PANID`。
- `CFG_CHANNEL`。
- `NOTIFY_BOOT`。
- `NOTIFY_NET_STATUS`。
- `NOTIFY_NET_OPEN`。
- 接收到的温湿度 report 的 `TYPE_ZCL_IND / ZCL_REPORT_IND`。

### 阶段 5：Coordinator 设备管理

- 添加 Coordinator 设备表。
- 跟踪节点入网、节点地址更新、退网和最后可见时间。
- 添加入网后的延迟验证。
- 添加 ZDO Active EP 查询。
- 添加 ZDO Simple Descriptor 查询。
- 端点数据已知时输出 `NOTIFY_DEVICE_JOIN`。

### 阶段 6：ZCL 桥接扩展

- 添加 ZCL 读属性桥接。
- 添加 ZCL 写属性桥接。
- 添加 ZCL 命令发送桥接。
- 添加发送确认。
- 添加默认响应指示。
- 添加可选的上报配置支持。

### 阶段 7：EBYTE 透传模式兼容

- 仅在标准 ZCL 路径稳定后添加 FC08 私有 cluster。
- 添加 `sendMode`。
- 添加 `UartSend/DataNotify`。
- 添加 `+++` 处理和硬件回退。
- 添加默认目标地址/端点配置。

## 边界与非目标

未经明确批准不得实现：

- 一步完成完整的出厂命令兼容。
- Coordinator/Router/EndDevice 角色之间的运行时切换。
- 完整的动态 ZCL 端点创建命令 `0x40` 到 `0x4F`。
- 默认暴露真实网络密钥。
- 在 CoordinatorDongle 上初始化 OLED/AHT30。
- 在同一 UART 模式下混合文本日志和 HEX 帧。
- 用私有透传替代标准温湿度上报。

第一里程碑可接受的限制：

- `CFG_NODE_TYPE` 可以返回不支持，因为角色是编译期固定的。
- `CFG_STATUS` 中的网络密钥可以清零或隐藏，除非启用调试宏。
- 设备表可以先仅用 RAM，后续再持久化到 NV。
- `TYPE_ZDO_RSP` 初始可以仅支持 Active EP 和 SimpleDesc。
- `TYPE_ZCL_IND` 初始可以仅支持温湿度 report。

## 编码准则

- 遵循现有 Z-Stack 和 GenericApp 风格。
- 角色/硬件检查集中在功能宏后面。
- 不要添加大的无关抽象。
- 功能自包含时优先使用小的辅助模块：
  - `app_uart.*`
  - `app_hex_bridge.*`
  - `app_display.*`
  - `app_sensor.*`
  - `app_device_table.*`
- Coordinator 和 Router 业务逻辑保持分离。
- 避免在频繁路径中使用动态内存，除非匹配 Z-Stack 模式且立即释放。
- 在 CC2530 上，密切关注 RAM 用量。
- 避免 `sprintf` 使用浮点数。现有代码为此使用了自定义的定点两位小数转换。
- OLED 字符串保持有界，短字符串替换长字符串时清除旧文本。
- I2C/OLED 操作不要放在中断上下文中。
- UART 帧解析器要对破损或部分帧保持鲁棒。

## 验证清单

RouterNormal：

- 启动日志显示 Router 目标。
- OLED 初始化成功。
- AHT30 初始化状态可见。
- 传感器页面按配置的采样周期更新。
- 网络页面显示入网状态、PAN ID、信道和短地址。
- Coordinator 收到温度 report。
- Coordinator 收到湿度 report。
- Coordinator 不可用时 TX 失败计数递增。

CoordinatorNormal：

- 启动日志或 OLED 显示 CoordinatorNormal 目标。
- OLED 网络页面显示 Coordinator 状态。
- 打开 permit join 更新 OLED 剩余时间。
- Router 入网更新节点计数。
- Router report 后远端传感器页面更新。
- UART `TEXT_LOG` 模式输出可读日志。
- UART `HEX_BRIDGE` 模式仅输出有效 HEX 帧。
- 硬件回退可以切换 UART 模式。

CoordinatorDongle：

- 不发生 OLED/AHT30/I2C 初始化。
- 启动时在 HEX 模式下发出 `NOTIFY_BOOT`。
- `CFG_STATUS` 返回有效 HEX 帧。
- `CFG_OPEN_NET` 打开 permit join。
- `CFG_CLOSE_NET` 关闭 permit join。
- Router 入网产生相应的通知帧。
- Router report 产生 `TYPE_ZCL_IND / ZCL_REPORT_IND`。
- HEX 模式下不输出纯文本。

EBYTE HEX 兼容性：

- XOR8 验证文档中的已知样本帧。
- 校验和错误返回 `55 03 FF FE 01`。
- 破损/超时帧返回 `55 03 FF FF 00`。
- 多帧异步输出之间留有安全的帧间延迟。
- 所有地址字段为小端序。

## 给未来 Agent 的备注

- 本仓库不是亿佰特出厂固件。
- 仅在服务于本产品时才复现出厂行为。
- 首要目标是可靠的三目标架构，不是完整的协议克隆。
- 无线路径保持标准 ZCL，以便第三方 Zigbee 互操作性仍然可行。
- CoordinatorDongle 保持最小化和协议干净。
- CoordinatorNormal 通过 OLED 和可切换 UART 模式为人类提供便利。
- RouterNormal 专注于传感、显示和标准上报。

## Karpathy 编码准则

源自 Andrej Karpathy 对 LLM 编码陷阱的观察。这些规则适用于本项目的所有编码任务。

### 1. 先思考再编码

不假设，不掩饰困惑，明确权衡。

- 明确陈述假设。不确定就问。
- 存在多种理解时，列出选项——不要默默选一个。
- 存在更简单方案时就说出来。该反驳就反驳。
- 有任何不清楚的地方就停下来。说明困惑点，提问。

### 2. 简单优先

用最少的代码解决问题。不做投机性设计。

- 不添加未被要求的功能。
- 单次使用的代码不做抽象。
- 不添加未被要求的"灵活性"或"可配置性"。
- 不为不可能发生的场景写错误处理。
- 200 行能用 50 行写完的，重写。

自问："一个高级工程师会说这过于复杂吗？"如果是，简化。

### 3. 精准修改

只改必须改的。只清理自己造成的问题。

- 不"顺手改进"周边的代码、注释或格式。
- 不重构没有问题的代码。
- 匹配现有风格，即使你会用不同的方式写。
- 发现无关的死代码，提一下——不要删除。
- 移除因你的修改而变得无用的 import/变量/函数。
- 不删除已有的死代码，除非被要求。

检验标准：每一行变更都应直接追溯到用户的请求。

### 4. 目标驱动

定义成功标准。循环直到验证通过。

将任务转化为可验证的目标：
- "添加验证" → "为无效输入写测试，然后让测试通过"
- "修复 bug" → "写一个复现 bug 的测试，然后让测试通过"
- "重构 X" → "确保重构前后测试都通过"

多步骤任务，列出简要计划：
```
1. [步骤] → 验证：[检查项]
2. [步骤] → 验证：[检查项]
3. [步骤] → 验证：[检查项]
```

这些准则是否在起作用的标志：diff 中不必要的变更更少，因过度复杂化
导致的重写更少，澄清性提问发生在实现之前而非犯错之后。

## 自我改进日志

将学习、错误和功能需求记录到 `.learnings/` 以实现持续改进。
开始重要任务前回顾已有记录。

### 何时记录

| 触发条件 | 目标文件 | 分类 |
|---------|-------------|----------|
| 命令或操作失败 | `.learnings/ERRORS.md` | — |
| 用户纠正（"不对..."、"其实..."） | `.learnings/LEARNINGS.md` | `correction` |
| 请求不存在的功能 | `.learnings/FEATURE_REQUESTS.md` | — |
| 外部 API 或工具失败 | `.learnings/ERRORS.md` | — |
| 知识已过时 | `.learnings/LEARNINGS.md` | `knowledge_gap` |
| 发现更好的方法 | `.learnings/LEARNINGS.md` | `best_practice` |

### 条目 ID 格式

`TYPE-YYYYMMDD-XXX`，其中 TYPE 为 `LRN` / `ERR` / `FEAT`，XXX 为顺序号。

### 晋升到项目记忆

当一条学习记录具有广泛适用性（非一次性修复）时，提炼后添加到本 CLAUDE.md。
将原始条目的状态更新为 `promoted`。

## 已安装技能

`.claude/skills/` 中的技能：

| 技能 | 用途 |
|-------|-------|
| `karpathy-guidelines` | 完整准则与示例——见 `.claude/skills/andrej-karpathy-skills/` |
| `self-improving-agent` | 日志格式、解决流程、定期回顾——见 `.claude/skills/openclaw-skills-self-improving-agent-1-0-0/SKILL.md` |
| `find-skills` | 搜索和安装新技能——使用 `npx skills find [关键词]` 或浏览 https://skills.sh/ |
| `frontend-design` | 前端 UI 设计准则——见 `.claude/skills/frontend-design/SKILL.md` |
| `skill-creator` | 创建、评估和打包自定义技能——见 `.claude/skills/skill-creator/SKILL.md` |
