# Zigbee Router/Coordinator 联调压缩记录

## 1. 目标
- Router(CC2530): AHT30 实时采集 + OLED 显示 + Zigbee 上报温湿度
- Coordinator(CC2530): 纯接收器，不使用 OLED/IIC，仅串口输出远端温湿度

## 2. 初始问题
- UART0 上电乱码、无持续输出
- `UTX0IE` 未定义编译错误
- Router 发送后 Coordinator 无法收到 `ZCL_CMD_REPORT`
- 角色边界不清：Coordinator 仍初始化了 OLED/AHT30

## 3. 关键修复
### 3.1 串口稳定化（P1.5 TX / P1.4 RX）
- 在 `zcl_genericapp.c` 使用 UART0 直驱初始化/发送
- 修复寄存器与缓冲区问题，避免溢出和异常输出
- Router/Coordinator 均可稳定输出 ASCII 日志

### 3.2 角色拆分
- Router-only:
  - 初始化 OLED + AHT30
  - 周期 `GENERICAPP_READ_SENSOR_EVT` 采样
  - 周期心跳 `GENERICAPP_HEARTBEAT_EVT`
  - 发送 ZCL Report
- Coordinator-only:
  - 不初始化 OLED/AHT30/IIC
  - 仅串口打印 `ZCL_CMD_REPORT` 解包结果

### 3.3 无线数据格式（RO -> CO）
- 温度: Cluster `0x0402`, Attr `0x0000`, `int16`, 值=`temp*100`
- 湿度: Cluster `0x0405`, Attr `0x0000`, `uint16`, 值=`humi*100`
- 发送函数: `zcl_SendReportCmd(...)`
- 方向宏: `ZCL_FRAME_SERVER_CLIENT_DIR`

### 3.4 编译宏修复（核心）
- RouterEB 增加: `BDB_REPORTING`
- CoordinatorEB 增加: `ZCL_REPORT_DESTINATION_DEVICE`
- 说明:
  - 不开 `BDB_REPORTING`，Router 侧 `zcl_SendReportCmd` 原型不可见
  - 不开 `ZCL_REPORT_DESTINATION_DEVICE`，Coordinator 底层不解析 `ZCL_CMD_REPORT`

### 3.5 入网稳定化
- 上电自动 commissioning（不依赖按键）
- 增加日志:
  - `BDB start: ...`
  - `BDB FORM/STEER ok/fail`
  - `NWK: DEV_...`
  - `RO ready` / `CO ready`
  - Router 发送失败日志 `TX temp fail` / `TX humi fail`
- 增加失败自动重试事件:
  - `GENERICAPP_RETRY_COMMISSION_EVT`

## 4. 当前确认结果
- Router 日志显示:
  - `BDB STEER ok`
  - 持续 `Local Temp/Humi`
- Coordinator 日志显示:
  - `RX ZCL REPORT`
  - 持续 `Remote Temp/Humi`
- 结论: 链路已打通（采集 -> 无线上报 -> 协调器串口输出）

## 5. 当前生效默认网络参数（来自编译清单）
- `DEFAULT_CHANLIST=0x00000800`（信道 11）
- `ZDAPP_CONFIG_PAN_ID=0xFFFF`（不固定 PAN ID，自动选择）
- 证据位置:
  - `CC2530DB/CoordinatorEB/List/ZGlobals.lst`
  - `CC2530DB/RouterEB/List/ZGlobals.lst`

## 6. 主要修改文件
- `Projects/zstack/HomeAutomation/GenericApp/Source/zcl_genericapp.c`
- `Projects/zstack/HomeAutomation/GenericApp/Source/zcl_genericapp.h`
- `Projects/zstack/HomeAutomation/GenericApp/CC2530DB/GenericApp.ewp`

## 7. 建议收尾
- 若要固定网络:
  - 固定 `ZDAPP_CONFIG_PAN_ID`（如 `0x1234`）
  - 固定 `DEFAULT_CHANLIST`（如信道 11: `0x00000800`）
- 联调完成后可关闭调试日志（`BDB...`/`NWK...`/`HB...`）保留业务输出

---

## 8. 会话接力（下次新对话直接粘贴）

请继续接手这个项目，工作目录是：
`D:\Develop\Texas Instruments\Z-Stack 3.0.2`

核心工程：
- `Projects\zstack\HomeAutomation\GenericApp\CC2530DB\GenericApp.ewp`
- `Projects\zstack\HomeAutomation\GenericApp\Source\zcl_genericapp.c`
- `Projects\zstack\HomeAutomation\GenericApp\Source\zcl_genericapp.h`

当前状态（已完成）：
1. Router: AHT30 采集 + OLED 显示 + ZCL Report 上报（正常）
2. Coordinator: 纯接收器，仅串口输出 Remote Temp/Humi（正常）
3. 两端已打通，Coordinator 可持续看到 `RX ZCL REPORT`

关键编译宏：
- RouterEB: `BDB_REPORTING`
- CoordinatorEB: `ZCL_REPORT_DESTINATION_DEVICE`

网络默认：
- `DEFAULT_CHANLIST=0x00000800`（ch11）
- `ZDAPP_CONFIG_PAN_ID=0xFFFF`（自动 PAN）

已加的可观测日志：
- `BDB start/form/steer ...`
- `NWK: DEV_...`
- `RO ready` / `CO ready`
- `RX ZCL REPORT`

你接下来先做：
1. 检查 git 改动是否干净（不要回滚我已有修改）
2. 根据我新需求继续改代码
3. 每次改动后给出“修改文件 + 验证方法 + 预期日志”

注意：Coordinator 不允许初始化 OLED/AHT30/IIC，只能做无线接收和串口打印。
