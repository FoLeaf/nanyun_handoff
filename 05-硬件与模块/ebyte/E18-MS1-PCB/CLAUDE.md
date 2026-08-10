# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

E18-MS1-PCB 是亿佰特 (EBYTE) 的 CC2530 ZigBee 3.0 无线模块。本仓库包含模块的 PDF 文档、STM32 参考例程，以及一个 Windows 原生上位机工具。

## Build

**上位机** (`host_tool/main.c`) — 单文件，零外部依赖，Win32 API + C99：

```bash
# MinGW-w64 (推荐)
cd host_tool && gcc -O2 -Wall -municode -o E18-HostTool.exe main.c -lcomctl32 -lcomdlg32 -lgdi32

# MSVC
cd host_tool && cl /O2 /Fe:E18-HostTool.exe main.c /link user32.lib gdi32.lib comctl32.lib
```

或直接运行 `build.bat`。

## Architecture: host_tool/main.c

2127 行单文件 Win32 应用，按功能区域组织：

| 区域 | 行范围(约) | 职责 |
|------|----------|------|
| 常量/控件ID | 1–100 | `#define` 串口参数、HEX 帧常量、全部控件 ID |
| 全局状态 `AppState g` | 100–180 | 串口句柄、HEX 帧缓冲区、节点/绑定表、所有 HWND |
| 串口函数 | 180–350 | COM 枚举、打开/关闭、Overlapped 异步读线程 |
| HEX 协议 | 350–500 | `hexBuildFrame` (0x55+长度+XOR8)、`hexValidateFrame` |
| 响应解析 | 500–650 | `hexParseResponse` 解析反馈帧并更新 UI 控件 |
| 异步解析 | 650–750 | `hexParseAsync` 处理 0x80/0x81/0x82/0x8F 通知 |
| AT 协议 | 750–800 | AT 命令构建与响应解析 |
| 日志/工具 | 800–900 | `logAdd`(带毫秒时间戳)、`parseHexString` |
| UI Tab 创建 | 900–1350 | 6 个 Tab 页面的控件创建函数 |
| WndProc | 1350–1950 | 主窗口消息处理 (WM_COMMAND / WM_APP / WM_SIZE / WM_NOTIFY) |
| PanelWndProc | 1950–1960 | Panel 容器转发 WM_COMMAND 到主窗口 |
| WinMain | 1960–2127 | 入口点，创建窗口/控件/字体 |

### 关键设计决策 (避免踩坑)

1. **Panel 必须转发 WM_COMMAND**：Tab 内的按钮父窗口是 Panel 容器，按钮的 `WM_COMMAND` 只发给直接父窗口（Panel）。Panel 使用自定义 `PanelWndProc`，将 WM_COMMAND 转发给 `g.hMainWnd`。切勿使用 `DefWindowProcW` 作为 Panel 的窗口过程。

2. **Panel 和 Tab 不能同时有 WS_CLIPSIBLINGS**：Tab 控件和 Panel 容器是兄弟窗口（都是主窗口的子窗口），两者同时设置 `WS_CLIPSIBLINGS` 会导致双向裁剪，Panel 完全不可见。Panel 应注册为自定义窗口类 `E18Panel`，使用 `WS_CLIPCHILDREN`。

3. **HEX 帧反馈需要单独解析**：`hexParseAsync` 只处理 `cmdType >= 0x80` 的异步帧。同步响应帧（`cmdType < 0x80`）由 `hexParseResponse` 解析并更新 UI（如查询状态后更新 `g.hTxtStatus`）。

4. **MinGW 编译须知**：`wWinMain` 入口点需要 `-municode` 标志。MSVC 的 `#pragma comment(lib,...)` 用 `#ifdef _MSC_VER` 包裹以兼容 MinGW。

## E18-MS1-PCB 模块通信协议

### 串口参数
- 默认 115200 bps, 8N1, 无流控
- 模块通过 USB-TTL 转换器 (TX/RX/GND) 连接 PC

### HEX 指令帧格式
```
帧头(0x55) + 帧长(1B) + 命令类型(1B) + 命令码(1B) + 数据(NB) + XOR校验(1B)
XOR校验 = 命令类型 ^ 命令码 ^ 数据[0] ^ ... ^ 数据[N-1]
```

### 命令类型
| 类型 | 说明 |
|------|------|
| 0x00 | 本地配置 (信道/PANID/节点类型/功率等) |
| 0x01 | 网络管理 (设备发现/绑定/解绑) |
| 0x02 | ZCL 发送 (属性读写/透传/OnOff) |
| 0x80 | 系统通知 (异步) |
| 0x81 | 网络管理返回 (异步) |
| 0x82 | ZCL 接收 (异步) |
| 0x8F | 发送确认 (异步) |

### AT 指令格式
```
AT+命令\r\n          — 执行
AT+命令=值\r\n       — 设置
AT+命令?\r\n         — 查询
响应: +OK / +ERR=code / +<data>
```

## PDF 文档 (需用外部阅读器查看)

- `E18_Series_ZigBee3.0_UserManual_CN_v1.9.pdf` — 用户手册
- `亿佰特ZigBee3.0模块HEX命令标准规范_V1.7.pdf` — HEX 命令规范
- `AN2022020_HEX指令模式下的模块配网与模块识别.pdf` — 配网教程
- `AN2024001_亿佰特Zigbee 3.0模块接入第三方设备教程.pdf` — 第三方接入
- `STM32测试版原理图.pdf` — 测试板原理图
- `无线串口通信例程手册（STM32） - V1.0.pdf` — STM32 例程手册

## STM32 参考例程

`无线串口收发例程（STM32）/` 使用 STM32F030 + Keil MDK，通过 USART1 (PA9/PA10, 9600 8N1) 与模块通信。关键文件：`drive/uart.c`、`user/main.c`。
