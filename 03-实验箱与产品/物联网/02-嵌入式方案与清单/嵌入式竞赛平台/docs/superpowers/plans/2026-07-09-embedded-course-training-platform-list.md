# Embedded Course Training Platform List Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Create `嵌入式课程教学实训平台建设项目清单.xlsx` from the local IoT template with competition-preparation wording.

**Architecture:** Use the existing workbook as a style template, replace rows with a curated embedded-course equipment list, and preserve formulas and formatting through `openpyxl`. Validate the generated workbook by loading formulas and checking prohibited commercial claims.

**Tech Stack:** Python `openpyxl`, local Excel template, PowerShell verification commands.

## Global Constraints

- Output file is `嵌入式课程教学实训平台建设项目清单.xlsx`.
- Preserve columns `序号 / 货物名称 / 技术参数 / 单位 / 数量 / 单价 / 总价`.
- Student hardware quantities use 25 sets; platform, resources, systems, services, and environment items use 1 set or 1 item.
- Use competition-preparation language: `支持`, `可支撑`, `用于`, `拟建设`, `预留拓展`.
- Do not claim `已商用`, `成功案例`, `客户部署`, `市场验证`, awards, or completed external results.
- `总价` cells use formulas and the final row uses `SUM`.

---

### Task 1: Generate Workbook

**Files:**
- Read: `D:\nanyun\物联网\嵌入式竞赛平台\7.3物联网综合实训室建设项目清单(1).xlsx`
- Create: `D:\nanyun\物联网\嵌入式竞赛平台\嵌入式课程教学实训平台建设项目清单.xlsx`

**Interfaces:**
- Consumes: local template workbook and row data.
- Produces: one Excel workbook with 12 item rows plus one合计 row.

- [ ] **Step 1: Load template and preserve worksheet style**

Use `openpyxl.load_workbook(template_path)` and the active worksheet. Copy row/cell styles from the template rows before replacing values.

- [ ] **Step 2: Replace clear list rows**

Write headers in A1:G1 and write these货物名称 rows:

```text
嵌入式课程教学实训平台
嵌入式主控开发套件
单片机基础训练套件
嵌入式外设与接口训练套件
传感器与数据采集训练套件
通信与物联网扩展训练套件
执行器与控制对象训练套件
嵌入式竞赛训练资源包
在线题库与测评系统
教师端教学管理与资源建设服务
实训室配套工具与基础仪器
竞赛筹备环境建设及部分装修改造
合计
```

- [ ] **Step 3: Write technical parameters**

For each row, write Chinese technical parameters that map to 4T-style public course points: STM32/GD32, modular programming, LED, key, LCD/OLED, USART, I2C/AT24C02, ADC, TIM/PWM/input capture, DHT11/DS18B20/1-Wire, SPI/LoRa, ZigBee, RS485, simulated/truth-question training, and teacher process management.

- [ ] **Step 4: Write quantities, prices, and formulas**

Use 25套 for student kits and 1套/项 for platform/service/environment rows. Set row total formulas as `=E{row}*F{row}` and final total as `=SUM(G2:G13)`.

- [ ] **Step 5: Save workbook**

Save the generated file under the workspace root.

### Task 2: Verify Workbook

**Files:**
- Read: `D:\nanyun\物联网\嵌入式竞赛平台\嵌入式课程教学实训平台建设项目清单.xlsx`

**Interfaces:**
- Consumes: generated workbook.
- Produces: verification summary: sheet count, row count, formulas, forbidden-term scan.

- [ ] **Step 1: Reopen with formulas**

Use `openpyxl.load_workbook(output_path, data_only=False)` and assert one worksheet with 14 rows and 7 columns.

- [ ] **Step 2: Check formulas**

Assert G2:G13 are multiplication formulas and G14 is `=SUM(G2:G13)`.

- [ ] **Step 3: Check forbidden claims**

Scan all text cells and fail if any of `已商用`, `成功案例`, `客户部署`, `市场验证`, `获奖`, `赛事成绩` appears.

- [ ] **Step 4: Check style preservation**

Assert A:G column widths exist, row 1 headers are centered, column C wraps text, and all cells have borders.
