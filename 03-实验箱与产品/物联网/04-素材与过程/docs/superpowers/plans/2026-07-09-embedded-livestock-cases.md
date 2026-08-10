# Embedded Livestock Cases Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Replace two generic scenario rows in `D:\nanyun\物联网\嵌入式综合实验室建设项目清单1.xlsx` with livestock intelligent farming cases aligned to Jiangxi Biotech Vocational College's 畜禽智能化养殖 professional group.

**Architecture:** This is a scoped workbook content update. The workbook structure, formulas, row count, quantities, units, prices, and total budget remain unchanged; only row 10 and row 11 item names and technical parameters change.

**Tech Stack:** Excel `.xlsx`, Python `openpyxl`, Excel COM recalculation.

## Global Constraints

- Modify only `项目清单` row 10 and row 11 item names and technical parameter cells.
- Row 10 and row 11 remain `1` 套, unit price `25119`, total formula `=Erow*Frow`.
- Workbook total remains `960000` 元 with formula `=SUM(G2:G12)`.
- New row 10 name: `生猪智能养殖环境监测与精准环控实训系统`.
- New row 11 name: `蛋鸡智能养殖状态监测与禽舍联动控制实训系统`.
- Remove the old case names `工业设备状态监测与预测性维护系统` and `智慧仓储与资产盘点管理系统`.

---

### Task 1: Update Workbook Scenario Rows

**Files:**
- Modify: `D:\nanyun\物联网\嵌入式综合实验室建设项目清单1.xlsx`

**Interfaces:**
- Consumes: existing workbook sheet `项目清单`.
- Produces: same workbook with updated row 10 and row 11 scenario case content.

- [ ] **Step 1: Create a timestamped backup**

Run:

```powershell
$ts = Get-Date -Format 'yyyyMMdd-HHmmss'
$backupDir = "D:\nanyun\物联网\_嵌入式综合实验室生成过程\livestock-case-backup-$ts"
New-Item -ItemType Directory -Path $backupDir -Force | Out-Null
Copy-Item -LiteralPath "D:\nanyun\物联网\嵌入式综合实验室建设项目清单1.xlsx" -Destination $backupDir -Force
```

Expected: backup copy exists in the timestamped directory.

- [ ] **Step 2: Write the new row names and technical parameters**

Use `openpyxl` to set the following exact names and technical-parameter strings:

```python
ws["B10"] = "生猪智能养殖环境监测与精准环控实训系统"
ws["C10"] = """一、系统架构与功能
1.系统采用“猪舍环境感知层、嵌入式边缘控制层、执行联动层、云端教学应用层”分层架构，面向畜禽智能化养殖专业群开展生猪舍环境监测、精准环控、异常告警和运维管理实训。
2.环境感知层支持温度、湿度、氨气、二氧化碳、光照、PM2.5、门磁/人体红外、饮水状态、料位或称重等数据采集，采样周期可配置，支持实时采集、事件触发上报和历史曲线记录。
3.嵌入式边缘控制层支持STM32/OpenHarmony节点、ZigBee/WiFi/LoRa通信节点和中心网关接入，支持MQTT、HTTP、Modbus RTU/TCP、TCP/UDP等协议，具备数据缓存、断网续传、阈值判定和本地规则联动能力。
4.执行联动层支持风机、喷雾、补光灯、继电器、声光告警、水泵、窗帘/卷帘或低压模拟执行机构控制，可完成通风降温、湿度调节、异常报警和远程控制等教学任务。
5.云端教学应用层支持猪舍单元建模、设备台账、实时数据、历史曲线、告警记录、控制日志、实训任务、实验报告和教师端评价，支持真实设备与虚拟猪舍场景混合实训。
二、监测对象与控制策略
1.温湿度监测：支持猪舍温湿度采集、舒适区间设置、热应激风险提示、通风降温和喷雾联动。
2.有害气体监测：支持氨气、二氧化碳或空气质量参数采集，支持浓度阈值设置、越限告警、风机联动和平台记录。
3.饮水与饲喂状态：支持水位、流量、料位、称重或开关量状态模拟，用于饮水异常、料位不足、采食趋势和补料提醒实验。
4.栏舍安全监测：支持门磁、人体红外、红外对射、火焰/烟雾等信号采集，用于异常进入、设备故障和安全预警实验。
三、实验项目与教学内容
1.基础验证类：温湿度采集、气体传感器读取、继电器控制、风机/喷雾联动、串口调试和本地看板显示实验。
2.通信接入类：ZigBee节点入网、WiFi/MQTT数据上报、LoRa远距采集、Modbus设备接入和边缘网关协议转换实验。
3.系统集成类：猪舍环境多参数同步采集、阈值告警、通风降温联动、饮水/料位异常提醒、云端数据可视化和实训报告生成。
4.创新应用类：基于OpenHarmony的移动巡检终端、猪舍环境舒适度评价、异常趋势分析、养殖设备远程运维和AI辅助养殖问答。"""
ws["B11"] = "蛋鸡智能养殖状态监测与禽舍联动控制实训系统"
ws["C11"] = """一、系统架构与功能
1.系统采用“禽舍状态感知层、嵌入式采集控制层、边缘网关层、云端教学应用层”分层架构，面向蛋鸡智能养殖、禽舍环控和养殖装备装调开展综合实训。
2.禽舍状态感知层支持温度、湿度、光照、氨气、二氧化碳、PM2.5、门磁、红外对射、重量、料位、水位或产蛋计数等参数采集，支持秒级采集、周期上报、异常触发和历史数据留存。
3.嵌入式采集控制层支持STM32/OpenHarmony节点、RFID/NFC识别、条码/二维码、ZigBee/WiFi/LoRa通信节点和执行器模块组合，完成禽舍设备接入、状态判断和联动控制。
4.边缘网关层支持MQTT、HTTP、Modbus RTU/TCP、WebSocket、TCP/UDP等协议，具备设备接入、协议转换、数据缓存、本地规则、断网续传和云端同步能力。
5.云端教学应用层支持禽舍单元、笼位/栏位、设备、传感器、执行器和学生任务建模，支持实时看板、历史曲线、告警记录、联动日志、巡检记录、实验报告和教学评价。
二、监测对象与联动策略
1.光照与温湿度控制：支持禽舍光照强度、光照时长、温湿度阈值设置，联动补光灯、风机、喷雾或继电器完成环境调控实验。
2.空气质量监测：支持氨气、二氧化碳、PM2.5等参数采集和越限告警，联动通风设备并记录处理过程。
3.产蛋与巡检记录：支持红外对射、重量或计数信号模拟产蛋记录，支持RFID/NFC或二维码完成设备巡检、笼位识别和任务追踪。
4.饲喂饮水状态：支持料位、水位、称重或开关量状态采集，用于缺料缺水提醒、饲喂状态判断和异常记录。
三、实验项目与教学内容
1.基础验证类：光照采集、温湿度采集、气体检测、红外计数、RFID/NFC识别、继电器控制和LED补光实验。
2.通信接入类：ZigBee多节点采集、WiFi/MQTT上报、LoRa远距采集、Modbus执行设备接入和边缘网关协议转换实验。
3.系统集成类：禽舍环境监测、光照补偿、通风联动、产蛋计数、饲喂饮水异常提醒、平台看板配置和告警闭环实验。
4.创新应用类：基于OpenHarmony的禽舍巡检终端、蛋鸡舍环控策略优化、养殖设备运行日志分析、智能巡检报告生成和AI辅助养殖决策展示。"""
```

Preserve:

```python
ws["D10"] == "套"
ws["E10"] == 1
ws["F10"] == 25119
ws["G10"] == "=E10*F10"
ws["D11"] == "套"
ws["E11"] == 1
ws["F11"] == 25119
ws["G11"] == "=E11*F11"
ws["G13"] == "=SUM(G2:G12)"
```

- [ ] **Step 3: Recalculate and save in Excel**

Run Excel COM `CalculateFullRebuild()` on the workbook and save.

Expected: workbook opens, recalculates, saves, and closes without error.

- [ ] **Step 4: Verify content and formulas**

Verify:

```text
B10 = 生猪智能养殖环境监测与精准环控实训系统
B11 = 蛋鸡智能养殖状态监测与禽舍联动控制实训系统
old case names count = 0
G10 = 25119
G11 = 25119
G13 = 960000
formula errors = 0
```
