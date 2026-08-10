# -*- coding: utf-8 -*-
"""
学生组模块D重构：外观+尺寸质量检测
- research/student-module-d-pack/ MD 命题包
- 02_样题/学生组_模块D/ 独立完整包 + 电气旧件归档
- 选手/裁判 docx + 检测板 md
- 回写完整版样题学生组模块D段落
"""
from __future__ import annotations

import re
import shutil
from copy import deepcopy
from datetime import datetime
from pathlib import Path

from docx import Document
from docx.enum.text import WD_ALIGN_PARAGRAPH, WD_LINE_SPACING
from docx.oxml.ns import qn
from docx.shared import Cm, Pt, Twips
from docx.oxml import OxmlElement

ROOT = Path(__file__).resolve().parents[2]
SAMPLE = ROOT / "02_样题"
ARCHIVE = ROOT / "05_归档备份"
RESEARCH = ROOT / ".trellis" / "tasks" / "07-20-module-d-bare-pcb-exam" / "research" / "student-module-d-pack"
PACK = SAMPLE / "学生组_模块D"
TS = datetime.now().strftime("%Y%m%d_%H%M%S")
LEGACY_DIR = ARCHIVE / f"module-d-electrical-legacy-{TS}"
LEGACY_IN_PACK = PACK / "_归档移出_电气旧件"

# ---------- fonts ----------
def set_run_font(run, name_cn="仿宋", name_en="Times New Roman", size_pt=10.5, bold=False):
    run.bold = bold
    run.font.size = Pt(size_pt)
    run.font.name = name_en
    r = run._element
    rPr = r.get_or_add_rPr()
    rFonts = rPr.get_or_add_rFonts()
    rFonts.set(qn("w:ascii"), name_en)
    rFonts.set(qn("w:hAnsi"), name_en)
    rFonts.set(qn("w:eastAsia"), name_cn)
    rFonts.set(qn("w:cs"), name_en)


def add_para(doc, text, *, cn="仿宋", size=10.5, bold=False, align=None, space_after=6, space_before=0):
    p = doc.add_paragraph()
    if align is not None:
        p.alignment = align
    pf = p.paragraph_format
    pf.space_after = Pt(space_after)
    pf.space_before = Pt(space_before)
    pf.line_spacing_rule = WD_LINE_SPACING.SINGLE
    run = p.add_run(text)
    set_run_font(run, name_cn=cn, size_pt=size, bold=bold)
    return p


def set_cell_text(cell, text, *, cn="仿宋", size=9, bold=False, center=False):
    cell.text = ""
    p = cell.paragraphs[0]
    if center:
        p.alignment = WD_ALIGN_PARAGRAPH.CENTER
    run = p.add_run(text)
    set_run_font(run, name_cn=cn, size_pt=size, bold=bold)


def fill_table(table, rows, header_bold=True):
    for ri, row_data in enumerate(rows):
        for ci, val in enumerate(row_data):
            is_header = ri == 0 and header_bold
            set_cell_text(
                table.cell(ri, ci),
                str(val),
                cn="黑体" if is_header else "仿宋",
                size=9,
                bold=is_header,
                center=is_header or ci == 0,
            )


def setup_page(doc):
    sec = doc.sections[0]
    sec.page_width = Cm(21.0)
    sec.page_height = Cm(29.7)
    sec.left_margin = Cm(2.0)
    sec.right_margin = Cm(2.0)
    sec.top_margin = Cm(1.8)
    sec.bottom_margin = Cm(1.8)


def new_doc():
    doc = Document()
    setup_page(doc)
    return doc


# ---------- MD pack ----------
MD_FILES = {}

MD_FILES["00_边界与任务结构.md"] = r"""# 学生组模块D · 边界与任务结构

> 版本：2026-07-20 · 仅**学生组** · 取代电气通断/故障定位考核口径  
> 读者：命题 / 裁判 / 技术文件同步

---

## 1. 冻结边界

| 项 | 口径 |
|----|------|
| 组别 | **仅学生组**（不改职工组模块体系） |
| 定位 | 双面PCB光板**外观 + 关键尺寸**质量检测（高级工综合应用） |
| 时长 | **90 分钟**（D-1～D-5 分段见下） |
| 分值 | 模块内 **40 分**原始分（折算外置技术文件） |
| 对象 | 专用新设计光板；立创EDA专业版源工程；**A/B 等难度**两版 |
| 不做 | 电气通断、故障定位、上电、实际返修、技师级失效分析、量 Gerber |
| 标准 | 随卷**验收要求表**；引用 GB/T 4588-2025 须标注「正式文本待核对」+ 本卷竞赛表述 |

### 与 B/C 边界句（样题须出现）

> 本模块为独立质检任务。检验对象为赛场提供的**双面光板质检样件**（专用检测板，FR-4 双面、未贴片），与模块 B/C 的 LoRa 设计/文件任务相互独立；**不进行电气通断与故障定位，不上电，不量测 Gerber**。

---

## 2. 任务结构（90 min）

| 步骤 | 名称 | 建议时长 | 选手做什么 |
|------|------|----------|------------|
| D-1 | 检验准备 | 5 min | 样件编号、板面方向、文件核对；卡尺/放大镜/零位；基本信息 |
| D-2 | 外观缺陷检测 | 35 min | 顶/底/孔/阻焊/丝印/外形；记面、分区、特征、类型、现象；8 必检 + 干扰项 |
| D-3 | 关键尺寸测量 | 25 min | 板长/宽/厚、定位孔径、槽宽、外形特征；实测值+单位+基准+判定 |
| D-4 | 缺陷分类与质量判定 | 15 min | 分类汇总；对照验收表；**接收/返工/报废** + 依据 |
| D-5 | 复核与提交 | 10 min | 记录完整、单位、分类数量、结论签署；不要求深层制造根因 |

---

## 3. 评分结构（40 原始分）

| 项 | 分 |
|----|-----|
| 外观缺陷识别与定位 | 16 |
| 尺寸测量及合格判定 | 12 |
| 缺陷分类与数量汇总 | 4 |
| 样件质量处置及依据 | 4 |
| 量具使用、记录规范与复核 | 4 |

以客观采点为主；记录规范与处置理由保留少量评价分。细则与折算 → 技术文件待同步清单。

---

## 4. 本包文件索引

| 文件 | 说明 |
|------|------|
| `00_边界与任务结构.md` | 本文件 |
| `01_缺陷骨架与AB规则.md` | 8 必检 + 底面≥3 + 干扰项 + A/B |
| `02_尺寸项与禁止项.md` | 允许/禁止测量项 |
| `03_验收要求表骨架.md` | 可操作阈值表述 |
| `04_检测记录表字段.md` | D-1～D-5 字段，无电气 |
| `05_完整版学生组模块D正文稿.md` | 可回写完整版 |
| `06_评分骨架_40分.md` | 采点说明 |
| `07_技术文件待同步清单.md` | 本轮仅清单 |
| `08_打样与验证清单.md` | 投板/试做 |
| `09_检测板设计规格与制板说明.md` | 立创投板规格 |

正式包目录：`02_样题/学生组_模块D/`。
"""

MD_FILES["01_缺陷骨架与AB规则.md"] = r"""# 学生组模块D · 8 缺陷骨架与 A/B 规则

> 裁判用骨架。投板后据实物照片与坐标填入真值表。

---

## 1. 固定缺陷类型池（每版 8 处必检）

| 编号 | 类型 | 类别 | 建议面 | 稳定复制方式 |
|------|------|------|--------|--------------|
| D01 | 线路缺口或颈缩 | 线路 | 顶 | Gerber 线宽局部变窄/断开 |
| D02 | 线距异常 | 线路 | 顶或底 | 两线间距明显小于设计值 |
| D03 | 焊环不足 | 焊盘/孔 | 顶 | 孔偏或焊盘过小致一侧焊环不足 |
| D04 | 漏钻 | 焊盘/孔 | 顶/贯穿 | 焊盘存在、孔未钻通（工艺/钻孔文件可控） |
| D05 | 阻焊开窗偏移 | 阻焊 | 顶 | 开窗相对焊盘偏移 |
| D06 | 阻焊桥异常 | 阻焊 | 顶或底 | 桥过窄/缺失或异常桥接 |
| D07 | 丝印残缺 | 丝印 | 顶 | 字符缺笔/残缺 |
| D08 | 外形缺口 | 外形 | — | 板边局部缺口（锣形可控） |

**底面 ≥ 3 处**：建议将 D02、D06 及另一处（如 D01 镜像或 D03 底侧焊环）置于底层，防止只检正面。

---

## 2. 干扰项（不计入必检 8）

- 数量：2～4 处外观接近合格临界的特征（如轻微丝印模糊临界、线宽接近下限但合格）。
- 误报不强制扣分；仅可在「记录规范」轻微提示（见评分表）。
- 干扰项**不得**与必检 8 同位置混淆命题意图。

---

## 3. A/B 等难度规则

| 规则 | 说明 |
|------|------|
| 类型与数量 | A/B 均为上表 8 类各 1 处，类型集合相同 |
| 位置 | A/B **位置不同**（网格/特征名不同） |
| 尺寸真值 | 部分尺寸项名义相同、实测真值在公差内略有差异（模拟批次差） |
| 难度 | 识别难度、测量项数、底面必检数一致；试做均时/均分差目标 <5% |
| 版本标识 | 丝印 `MOD-D-S-A` / `MOD-D-S-B`（学生组 Student） |
| 校验 | 版本栏 + 板号 + 校验码写入 `A_B差异与版本校验.md` |

---

## 4. 真值表字段（裁判）

| 字段 | 说明 |
|------|------|
| 版本 | A / B |
| 缺陷编号 | D01–D08 |
| 类型 | 上表类型名（允许同义，见同义答案） |
| 所在面 | 顶 / 底 |
| 网格区 | A1–D4 |
| 特征名 | 走线区/孔阵/VIA区/阻焊对比区/丝印条/板框等 |
| 位置描述 | 简要方位 |
| 现象关键词 | 采点用 |
| 是否必检 | 是（干扰项另表） |
| 照片编号 | 投板后补 |

### 骨架占位（投板后填写坐标）

| 编号 | 类型 | 面(A建议) | 网格(A) | 面(B建议) | 网格(B) |
|------|------|-----------|---------|-----------|---------|
| D01 | 线路缺口或颈缩 | 顶 | B2 | 顶 | C3 |
| D02 | 线距异常 | 底 | C2 | 底 | B3 |
| D03 | 焊环不足 | 顶 | A2 | 顶 | A3 |
| D04 | 漏钻 | 顶 | B4 | 顶 | C1 |
| D05 | 阻焊开窗偏移 | 顶 | C1 | 顶 | B1 |
| D06 | 阻焊桥异常 | 底 | D2 | 底 | D3 |
| D07 | 丝印残缺 | 顶 | A3 | 顶 | D1 |
| D08 | 外形缺口 | — | 板框/D4边 | — | 板框/A1边 |

底面计数：A 版建议 D02、D06 +（可选 D01 底侧或独立底面线路项）≥3；定稿时以实物清单为准。
"""

MD_FILES["02_尺寸项与禁止项.md"] = r"""# 学生组模块D · 尺寸项与禁止项

---

## 1. 允许测量项（普通游标卡尺可可靠）

| 序号 | 项目 | 名义（建议） | 公差（竞赛用） | 备注 |
|------|------|--------------|----------------|------|
| 1 | 板长 L | 80.00 mm | ±0.20 mm | 最长边；避开毛刺 |
| 2 | 板宽 W | 60.00 mm | ±0.20 mm | 垂直板长 |
| 3 | 板厚 T | 1.60 mm | ±0.16 mm（约±10%） | 板中非异常区 |
| 4 | 定位孔径 | ≥1.00 mm（建议 2.00 mm 或 3.00 mm） | ±0.10 mm 或按验收表 | 明确内径测法 |
| 5 | 槽宽（若有槽） | 设计给定（建议 ≥2.0 mm） | ±0.15 mm | 无槽则改为「外形特征」 |
| 6 | 外形特征 1 项 | 如切口深度/台阶宽 | 按验收表 | 仅 1 项，卡尺可测 |

选手记录表可列 **5 项**：L、W、T、定位孔径、槽宽或外形特征（与尺寸基准表一致）。

---

## 2. 禁止项（本模块不考）

- 0.30 mm 级小孔（普通卡尺不可靠）
- 线到板边微距
- 需显微镜/二次元/影像仪才稳定的项目
- 量 Gerber / 与设计文件比对微距

---

## 3. 记录要求

- 单位：mm；建议保留 0.01 mm
- 每项：实测值 + 与基准比对判定（合格/不合格）
- 工艺参数理解可用 mm（mil）；**填写实测以 mm 为主**
"""

MD_FILES["03_验收要求表骨架.md"] = r"""# 学生组模块D · 验收要求表骨架（随卷）

> 用途：选手对照判定；**可操作**现象/阈值。  
> 标准引用：GB/T 4588-2025 对应条款**待正式文本核对**；下表「竞赛用验收表述」为本卷采用口径。

---

## 1. 外观类

| 类别 | 不合格现象（竞赛表述） | 合格说明 | 标准注记 |
|------|------------------------|----------|----------|
| 线路 | 可见缺口、开路；线宽局部颈缩导致明显不连续或明显小于设计意图 | 线连续、无明显缺口/致命颈缩 | GB/T 4588-2025 相关条款待核对 |
| 线距 | 相邻导体间距明显不足，存在短路风险或可见异常贴近 | 线距正常、无异常贴近 | 同上 |
| 焊盘/孔 | 焊环一侧明显不足；漏钻（有盘无孔或应钻未钻） | 焊环完整可辨；应钻孔已钻通 | 同上 |
| 阻焊 | 开窗明显偏移露出异常铜/盖住焊盘；阻焊桥异常缺失或异常桥接 | 开窗与焊盘基本对准；桥可辨 | 同上 |
| 丝印 | 关键字符残缺不可辨 | 字符完整可辨 | 同上 |
| 外形 | 板边缺口/破损超出可接受范围 | 外形完整、缺口在允许范围内 | 同上 |

## 2. 尺寸类

见尺寸基准表：任一项超出基准范围 → 该项不合格。

## 3. 单件质量处置（竞赛口径）

| 结论 | 条件（竞赛表述） |
|------|------------------|
| **接收** | 8 类必检缺陷均未发现（或仅干扰项误报不计）且尺寸全部合格 |
| **返工** | 存在可返工类缺陷（如丝印残缺、局部阻焊异常等，以验收表与赛场说明为准），尺寸基本合格或仅轻微超差且可返工 |
| **报废** | 存在致命缺陷（线路开路/缺口、漏钻、外形严重缺口、尺寸严重超差等）或缺陷导致板不可用 |

> 注：正式赛场若采用更细 IPC/国标等级，以技术文件同步后的细则为准；样题只给上表可操作表述。
"""

MD_FILES["04_检测记录表字段.md"] = r"""# 学生组模块D · 检测记录表字段（无电气）

---

## 表头

- 赛位号、日期、样件编号、版本（A/B，以实物丝印为准）、选手签署（按赛场要求，不得写姓名若规则禁止）

## D-1 检验准备

- 板面方向确认（顶/底定义与分区说明一致）□
- 文件齐全核对（记录表/验收表/尺寸基准/分区说明）□
- 卡尺零位检查 □
- 放大镜/照明可用 □
- 样件外观有无运输损伤（描述）____

## D-2 外观缺陷检测

表格列：

| 序号 | 所在面 | 缺陷类型 | 网格区 | 特征名 | 位置/现象描述 | 严重程度(轻/中/重) |

- 行数建议 ≥10（容纳 8 必检 + 干扰/多余记录）
- 提示：须检顶底；缺陷分布可不均

## D-3 关键尺寸测量

| 测量项目 | 基准要求 | 实测值(mm) | 合格/不合格 | 备注 |

项目：板长、板宽、板厚、定位孔径、槽宽或外形特征。

## D-4 分类与质量处置

1. 分类计数：线路____；焊盘/孔____；阻焊____；丝印____；外形____；其它____；合计____
2. 尺寸：□全部合格 □有不合格项：____
3. 质量处置：□接收 □返工 □报废
4. 依据（对照验收要求表）：____

## D-5 复核

- 单位齐全 □；分类合计与明细一致 □；结论已填 □；页码/附件完整 □

**禁止字段**：电阻读数、通断、NET_A/NET_B、故障段、高阻、应通应断。
"""

MD_FILES["05_完整版学生组模块D正文稿.md"] = r"""# 完整版样题 · 学生组模块D 正文稿（外观+尺寸）

> 回写目标：`02_样题/印制电路制作工赛项_竞赛样题（完整版）.docx` 中模块D段落  
> 明确标注学生组口径；取消电气通断/故障定位

---

## 模块D：双面PCB光板外观与尺寸质量检测（学生组）

### （一）模块考核点

本模块（**学生组**）考核选手对未贴片双面印制电路光板进行**外观缺陷检测、关键尺寸测量、标准比对、缺陷分类与单件质量处置**的能力。考核时长建议 **90 分钟**（D-1～D-5）；模块内原始分 **40 分**（折算与细则见技术工作文件）。  
**不考核**电气通断、故障定位、上电调试、实际返修及 Gerber 量测。

### （二）模块简介

**【任务背景】**  
说明：本模块为独立质检任务。检验对象为赛场提供的双面光板质检样件（专用检测板，FR-4 双面、未贴片），与模块 B/C 的 LoRa 核心板设计及 Gerber/工程文件任务相互独立。某批次双面光板完成后进入抽检环节，你作为质检人员，需完成规范检验、准确测量、对照验收要求进行分类判定，并给出**接收 / 返工 / 报废**结论及简要依据。  
本模块**不上电**，不进行整机或加电调试，**不进行电气通断与故障定位**，不考核贴装质量与元器件参数，不量测 Gerber 文件。

**【检验对象】**  
双面光板质检样件 1 块（未贴片）。外形约 80×60 mm，标称板厚 1.6 mm。板面丝印含版本标识（A 或 B）、A1–D4 分区网格及特征名（走线区、孔阵、VIA区、阻焊对比区、丝印条、板框等）。样件含 **8 处**须识别的稳定缺陷（顶/底均可能存在，**须检查两面**），并可能含若干接近合格临界的干扰特征。

**【提供材料】**  
1. 双面光板质检样件  
2. PCB光板检测记录表（学生组模块D，空白）  
3. 验收要求表（学生组模块D）  
4. 尺寸基准表（学生组模块D）  
5. 板面分区说明（学生组模块D）  
6. 选手材料清单（可与上述合并印发）

**【使用工具】**  
游标卡尺、放大镜、侧光/照明、防静电用品等（赛场提供清单为准）。**本模块不要求万用表作为必考工具。**

### （三）检测要求说明

尺寸基准表与验收要求表由赛场随卷下发；工艺相关参数采用 mm（mil）双单位理解，实测记录以 mm 为主。缺陷与尺寸判定采用本卷**竞赛用验收表述**；引用标准名称 GB/T 4588-2025 时，对应条款以正式文本核对为准，等级/扣分/折算等细则以技术工作文件及赛场说明为准。

### （四）模块任务

#### D-1  检验准备（建议 5 分钟）

核对样件编号与版本丝印、确认顶/底方向与分区约定；检查卡尺零位与放大镜/照明；在记录表填写基本信息并勾选准备项。

#### D-2  外观缺陷检测（建议 35 分钟）

对样件线路、焊盘/孔、阻焊、丝印、外形等进行外观检查（**须检顶面与底面**），将发现的缺陷记录在检测记录表中。

填写要求：  
（1）每发现一处缺陷，在记录表中填写一行；  
（2）填写：所在面（顶/底）、缺陷类型、网格区、特征名、位置描述、现象描述、严重程度（轻/中/重）；  
（3）位置描述应便于复核，例如“顶层 B2 / 走线区 / 中部”，无需精确坐标；  
（4）缺陷类型应具体明确（如“线路颈缩”“漏钻”而非“线路有问题”）；  
（5）缺陷类别可归入：线路、焊盘/孔、阻焊、丝印、外形、其它。

#### D-3  关键尺寸测量（建议 25 分钟）

使用卡尺测量下列项目，填写实测值，并与尺寸基准表比对判定合格/不合格：  
（1）板长；（2）板宽；（3）板厚；（4）定位孔径；（5）槽宽或指定外形特征尺寸。  

**不要求**测量普通卡尺无法可靠完成的项目（如 0.30 mm 级微孔、线到板边微距等）。记录以 mm 为主并保留适当精度。

#### D-4  缺陷分类与质量判定（建议 15 分钟）

（1）对已记录外观缺陷按类型分类计数；  
（2）汇总尺寸是否全部满足基准要求；  
（3）对照《验收要求表》，给出样件质量处置结论：**接收 / 返工 / 报废**，并写出简要依据；  
（4）不要求分析深层制造工艺根因。

#### D-5  复核与提交（建议 10 分钟）

检查记录完整性、单位、分类数量与明细一致性、结论签署；按赛场要求提交纸质/电子成果。  
电子版目录示例：`D:\提交资料\模块D\`，命名示例：`赛位号_模块D`。

注意事项：  
1. 请合理分配准备、外观、尺寸、判定与复核时间；  
2. 本模块与模块 B/C 独立；仅检验赛场光板样件；  
3. 爱护样件与量具；遵守安全与防静电要求；  
4. 成果上不得标注姓名等身份信息（按赛场规则）；  
5. 如遇设备故障，举手示意裁判。
"""

MD_FILES["06_评分骨架_40分.md"] = r"""# 学生组模块D · 评分表骨架（40 原始分）

> 供裁判/技术文件同步；样题可不列分值，但任务结构可写 90 分钟分段。

| 评分项 | 分值 | 采点原则 |
|--------|------|----------|
| 外观缺陷识别与定位 | 16 | 8 必检，建议每处 2 分：类型正确 + 面/区定位可复核；漏检/错类按点扣；干扰项误报不强制扣 |
| 尺寸测量及合格判定 | 12 | 5 项，建议每项 2～2.5 分：实测在允差内（或与裁判标定一致）+ 合格判定正确 |
| 缺陷分类与数量汇总 | 4 | 分类项齐全、计数与明细大体一致 |
| 样件质量处置及依据 | 4 | 接收/返工/报废与验收表及主要缺陷/尺寸结论匹配；依据简要合理 |
| 量具使用、记录规范与复核 | 4 | 单位、准备项、复核勾选、书写可辨；少量评价分 |

### 同义答案原则（摘要）

- 类型：颈缩/线宽不足/缺口/开路（线路类）在关键词命中时可视同  
- 定位：网格正确或特征名+方位足以唯一定位可给定位分  
- 详见包内 `裁判/同义答案与采点说明.md`
"""

MD_FILES["07_技术文件待同步清单.md"] = r"""# 技术文件待同步清单（学生组模块D）

> 本轮**不强制改正文技术文件**，仅列出待同步点，避免与职工组混用。

| 序号 | 同步点 | 建议口径 | 优先级 |
|------|--------|----------|--------|
| 1 | 学生组模块D 名称 | 双面PCB光板外观与尺寸质量检测 | 高 |
| 2 | 时长 | 90 分钟（可写 D-1～D-5 建议分段） | 高 |
| 3 | 分值 | 模块内 40 原始分及折算关系 | 高 |
| 4 | 评分项 | 外观16 + 尺寸12 + 分类4 + 处置4 + 规范4 | 高 |
| 5 | 取消电气相关评分点 | 删除通断/故障定位/电阻档/NET 等 D 模块考点 | 高 |
| 6 | 与职工组分列 | 学生组 / 职工组模块D 分条表述，禁止混用编号含义 | 高 |
| 7 | 设备清单 | 卡尺、放大镜为 D 必考；万用表不作为学生组 D 必考 | 中 |
| 8 | 材料清单 | 增加验收要求表；移除电气连通与故障定位表、通断网络表 | 高 |
| 9 | 成果物 | PCB光板检测记录表（无电气栏） | 高 |
| 10 | 标准引用 | GB/T 4588-2025 条款待核对注记 + 竞赛验收表述 | 中 |
| 11 | A/B 样件 | 等难度双版本管理与备用板 | 中 |
| 12 | 安全 | 不上电；防静电；爱护样件 | 中 |
"""

MD_FILES["08_打样与验证清单.md"] = r"""# 学生组模块D · 打样与验证清单

## 投板前

- [ ] 立创EDA专业版源工程：A/B 各一（或同工程多板号）
- [ ] Gerber / 钻孔 / 外形与设计规格、真值表一致
- [ ] 8 缺陷在 Gerber/工艺上可稳定复制；底面≥3
- [ ] 干扰项已标记且不计入必检
- [ ] 尺寸名义与公差可卡尺测量；无禁止项
- [ ] 丝印版本号 MOD-D-S-A / MOD-D-S-B 与校验信息

## 首批样件

- [ ] 标准板（全合格参考，可选）+ A + B + 备用各若干
- [ ] 双人独立目检 8 缺陷，一致率目标 ≥95%
- [ ] 尺寸三测（不同人/不同卡尺）极差可接受
- [ ] 拍照建档（顶/底/缺陷特写）写入裁判答案

## 试做

- [ ] ≥3 名目标学生试做
- [ ] A/B 均时、均分差 <5%
- [ ] 记录表字段无歧义；无电气残留措辞
- [ ] 根据试做微调验收表述与同义答案

## 定稿

- [ ] 裁判培训用答案册定稿
- [ ] 技术文件同步项关闭
- [ ] 备份源工程与 Gerber 至归档
"""

MD_FILES["09_检测板设计规格与制板说明.md"] = r"""# 学生组模块D · 检测板设计规格与制板说明要点

## 1. 基本规格

| 项 | 值 |
|----|-----|
| 外形 | 约 80 mm × 60 mm（矩形，可倒小圆角） |
| 板材 | FR-4 |
| 板厚 | 标称 1.6 mm |
| 层数 | 双面 |
| 铜厚 | 1 oz（建议） |
| 阻焊 | 绿色，常规开窗 |
| 丝印 | 白色；含板名、版本 A/B、网格参考、特征名 |
| 表面处理 | 无铅喷锡或沉金（以打样便利为准） |
| 贴装 | **无**（光板） |
| 设计工具 | **立创EDA专业版** |

## 2. 丝印与分区

- 顶层：A1–D4 网格参考（列 1–4，行 A–D）
- 特征名：走线区、孔阵、VIA区、阻焊对比区、丝印条、板框
- 版本：`MOD-D-S-A` / `MOD-D-S-B`
- **不放置** TP1–TP8 通断测试点网络（本轮取消电气考核）

## 3. 功能块

1. 板框与外形特征（含可控外形缺口位）  
2. 线路走线区（缺口/颈缩、线距异常）  
3. 焊盘/孔区（焊环不足、漏钻）  
4. 阻焊对比/开窗区  
5. 丝印字符区  
6. 定位孔（≥1.0 mm，建议 2.0/3.0 mm）+ 可选槽

## 4. 缺陷实现要点

| 类型 | 实现要点 |
|------|----------|
| 线路缺口/颈缩 | 顶层铜皮局部断开或线宽阶跃变窄 |
| 线距异常 | 两线间距明显小于其余网络 |
| 焊环不足 | 孔相对焊盘偏移或焊盘偏小 |
| 漏钻 | 焊盘保留、钻孔文件该孔删除或堵孔工艺（打样需与板厂确认可识别） |
| 阻焊开窗偏移 | 阻焊层开窗平移 |
| 阻焊桥异常 | 桥宽异常或局部无桥 |
| 丝印残缺 | 字符缺笔 |
| 外形缺口 | 外形铣刀局部内凹 |

## 5. 制板说明（给板厂）

- 按 Gerber + 钻孔 + 外形交付；注明 A/B 分板或分批  
- 缺陷为**有意设计**，勿按 DFM 自动“修掉”  
- 漏钻/外形缺口等须在订单备注中说明“竞赛检测板，保留设计缺陷”  
- 数量：试产建议 标准/A/B/备用 各不少于计划试做与裁判培训用量  

## 6. 占位目录（后续投放）

```
02_样题/学生组_模块D/检测板/
  设计规格.md          # 本说明可拆分
  制板说明.md
  A_B差异与版本校验.md
  源工程/              # 立创工程占位
  Gerber/              # 占位
  钻孔/                # 占位
```
"""

MD_FILES["README.md"] = r"""# 学生组模块D 命题研究包（外观+尺寸）

本目录取代 `research/module-d-pack/` 中**学生组电气过渡方案**口径，专用于：

**双面PCB光板外观与尺寸质量检测（学生组）**

正式选手/裁判交付见：`02_样题/学生组_模块D/`。

旧电气命题包保留在 `research/module-d-pack/` 与 `05_归档备份/module-d-electrical-legacy-*` 供追溯，**不再作为学生组模块D现行口径**。
"""


def write_md_pack():
    RESEARCH.mkdir(parents=True, exist_ok=True)
    for name, content in MD_FILES.items():
        (RESEARCH / name).write_text(content.lstrip("\n") if content.startswith("\n") else content, encoding="utf-8")
    print("[OK] research pack:", RESEARCH)


# ---------- package tree + legacy move ----------
ELECTRICAL_FILES = [
    "模块D样题_通断网络表.docx",
    "模块D样题_电气连通与故障定位表.docx",
    "模块D样题_电气连通与故障定位_参考答案.docx",
    "模块D样题_尺寸与通断_参考答案.docx",
]


def ensure_dirs():
    for sub in ["选手", "裁判", "检测板", "检测板/源工程", "检测板/Gerber", "检测板/钻孔", "_归档移出_电气旧件"]:
        (PACK / sub).mkdir(parents=True, exist_ok=True)
    LEGACY_DIR.mkdir(parents=True, exist_ok=True)
    # placeholders
    for d in ["源工程", "Gerber", "钻孔"]:
        p = PACK / "检测板" / d / "README.md"
        p.write_text(
            f"# {d} 占位\n\n立创EDA专业版导出后置于本目录。环境未完成工程文件时保持占位。\n",
            encoding="utf-8",
        )


def move_electrical_legacy():
    note_lines = [
        f"# 电气旧件移出说明",
        f"",
        f"- 时间：{TS}",
        f"- 原因：学生组模块D改为外观+尺寸，取消电气通断/故障定位",
        f"- 备份目录：`05_归档备份/{LEGACY_DIR.name}/`",
        f"- 包内镜像：`学生组_模块D/_归档移出_电气旧件/`",
        f"",
        f"## 已移出文件",
        f"",
    ]
    for fn in ELECTRICAL_FILES:
        src = SAMPLE / fn
        if not src.exists():
            note_lines.append(f"- （未找到，跳过）{fn}")
            continue
        dst1 = LEGACY_DIR / fn
        dst2 = LEGACY_IN_PACK / fn
        shutil.copy2(src, dst1)
        shutil.copy2(src, dst2)
        # remove from 02_样题 root active area
        src.unlink()
        note_lines.append(f"- {fn}")
        print("[MOVE]", fn)

    # also copy related answers that mix size+continuity name already listed
    note_path = LEGACY_IN_PACK / "00_移出说明.md"
    note_path.write_text("\n".join(note_lines) + "\n", encoding="utf-8")
    (LEGACY_DIR / "00_移出说明.md").write_text("\n".join(note_lines) + "\n", encoding="utf-8")

    # keep non-electrical module D files but mark as legacy naming at root — will be superseded by 学生组_模块D
    # 缺陷清单_参考答案 may still be electrical-era; copy to archive as historical
    for fn in ["模块D样题_缺陷清单_参考答案.docx"]:
        src = SAMPLE / fn
        if src.exists():
            shutil.copy2(src, LEGACY_DIR / fn)
            shutil.copy2(src, LEGACY_IN_PACK / fn)
            note_lines.append(f"- （历史参考，已复制归档仍保留根目录可选）{fn}")


# ---------- docx builders ----------
def build_partition_docx(path: Path):
    doc = new_doc()
    add_para(doc, "板面分区说明（学生组·模块D）", cn="黑体", size=12, bold=True, align=WD_ALIGN_PARAGRAPH.CENTER)
    add_para(
        doc,
        "选手用·随卷下发。用于理解板面坐标与特征名。本说明不是“一区一错”提示。本模块不设电气测试点网络。",
        cn="仿宋",
        size=9,
    )
    add_para(doc, "一、A1–D4 网格", cn="黑体", size=11, bold=True)
    add_para(
        doc,
        "A1–D4 是印在光板顶层丝印上的位置坐标网格，用来标明缺陷所在区域，方便书写与复核。它不是 16 个错误点，也不表示每个格子必有缺陷。",
        size=10.5,
    )
    add_para(
        doc,
        "约定：行用字母 A、B、C、D（自上而下）；列用数字 1、2、3、4（自左而右）。例如 B2 表示第 B 行第 2 列区域。以实物丝印为准。",
        size=10.5,
    )
    add_para(doc, "示意（顶层俯视）：", size=10.5)
    t = doc.add_table(rows=5, cols=5)
    t.style = "Table Grid"
    grid = [
        ["", "1", "2", "3", "4"],
        ["A", "A1", "A2", "A3", "A4"],
        ["B", "B1", "B2", "B3", "B4"],
        ["C", "C1", "C2", "C3", "C4"],
        ["D", "D1", "D2", "D3", "D4"],
    ]
    fill_table(t, grid)

    add_para(doc, "二、特征区名称（丝印）", cn="黑体", size=11, bold=True, space_before=8)
    add_para(doc, "记录时建议“所在面 + 网格 + 特征名”，例如：“顶层 B2 / 走线区”。", size=10.5)
    t2 = doc.add_table(rows=7, cols=2)
    t2.style = "Table Grid"
    fill_table(
        t2,
        [
            ["特征名", "含义（便于定位，非缺陷清单）"],
            ["走线区", "主要信号走线分布区域"],
            ["孔阵", "多个通孔/焊盘排列区域"],
            ["VIA区", "过孔集中区域"],
            ["阻焊对比区", "阻焊开窗/桥特征对比区域"],
            ["丝印条", "字符/标识集中区域"],
            ["板框", "外形边缘及外形特征区域"],
        ],
    )

    add_para(doc, "三、顶面与底面", cn="黑体", size=11, bold=True, space_before=8)
    add_para(
        doc,
        "须检查顶面与底面。底面网格可按“透过板或翻转后对应关系”在记录中写“底层 + 网格/特征”；以赛场分区说明图与实物丝印为准。版本丝印一般为顶面（MOD-D-S-A 或 MOD-D-S-B）。",
        size=10.5,
    )

    add_para(doc, "四、缺陷记录写法示例", cn="黑体", size=11, bold=True, space_before=8)
    for s in [
        "示例1：顶层 / B2 / 走线区 / 中部 — 线路颈缩、铜皮局部明显变窄。",
        "示例2：底层 / C2 / 走线区 — 线距异常、两线异常贴近。",
        "示例3：顶层 / A2 / 孔阵 — 焊环不足、铜环一侧过窄。",
        "示例4：顶层 / B4 / 孔阵 — 漏钻、有盘无孔。",
    ]:
        add_para(doc, s, size=10.5)

    add_para(doc, "五、重要说明", cn="黑体", size=11, bold=True, space_before=8)
    for s in [
        "1. 网格用于定位，不是“一格一个错误”。",
        "2. 缺陷分布可能不均匀；同一网格可有多处，部分网格可以没有缺陷。",
        "3. 请检查顶层与底层；勿只查正面。",
        "4. 本模块不进行电气通断与故障定位，不设 TP 测点考核。",
        "5. 本模块不上电，不量测 Gerber。",
    ]:
        add_para(doc, s, size=10.5)

    doc.save(str(path))
    print("[OK]", path.name)


def build_acceptance_docx(path: Path):
    doc = new_doc()
    add_para(doc, "验收要求表（学生组·模块D）", cn="黑体", size=12, bold=True, align=WD_ALIGN_PARAGRAPH.CENTER)
    add_para(
        doc,
        "选手用·随卷下发。用于外观缺陷与质量处置判定。引用标准名称 GB/T 4588-2025：对应条款待正式文本核对；下表为**本卷竞赛用验收表述**。",
        size=9,
    )
    add_para(doc, "一、外观验收（竞赛表述）", cn="黑体", size=11, bold=True)
    t = doc.add_table(rows=7, cols=3)
    t.style = "Table Grid"
    fill_table(
        t,
        [
            ["类别", "不合格现象（应判定为缺陷）", "合格说明"],
            ["线路", "可见缺口/开路；明显颈缩导致线宽局部严重变窄或不连续", "线连续，无明显缺口或致命颈缩"],
            ["线距", "相邻导体间距明显不足，存在短路风险或异常贴近", "线距正常，无异常贴近"],
            ["焊盘/孔", "焊环一侧明显不足；漏钻（应钻未钻/有盘无孔）", "焊环完整可辨；应钻孔已钻通"],
            ["阻焊", "开窗明显偏移；阻焊桥异常缺失或异常状态", "开窗基本对准；阻焊桥可辨"],
            ["丝印", "关键字符残缺不可辨", "字符完整可辨"],
            ["外形", "板边缺口/破损超出可接受范围", "外形完整，缺口在允许范围内"],
        ],
    )
    add_para(doc, "二、尺寸验收", cn="黑体", size=11, bold=True, space_before=8)
    add_para(doc, "以《尺寸基准表（学生组·模块D）》为准：任一项超出基准范围，则该尺寸项不合格。", size=10.5)

    add_para(doc, "三、单件质量处置（竞赛口径）", cn="黑体", size=11, bold=True, space_before=8)
    t2 = doc.add_table(rows=4, cols=2)
    t2.style = "Table Grid"
    fill_table(
        t2,
        [
            ["结论", "条件（竞赛表述）"],
            ["接收", "未发现须判定不合格的致命/超标缺陷，且尺寸全部合格（干扰项误报不计）"],
            ["返工", "存在可返工类缺陷（如丝印残缺、局部阻焊异常等），板仍具备返工价值；或尺寸轻微超差且可返工"],
            ["报废", "存在致命缺陷（如线路开路/严重缺口、漏钻、外形严重破损、尺寸严重超差等）导致板不可用"],
        ],
    )
    add_para(
        doc,
        "说明：正式等级划分与扣分细则以技术工作文件为准。选手须在记录表写明处置结论与简要依据。",
        size=9,
        space_before=6,
    )
    doc.save(str(path))
    print("[OK]", path.name)


def build_dimension_docx(path: Path):
    doc = new_doc()
    add_para(doc, "尺寸基准表（学生组·模块D）", cn="黑体", size=12, bold=True, align=WD_ALIGN_PARAGRAPH.CENTER)
    add_para(
        doc,
        "选手用·随卷下发。请按下列基准测量并在《PCB光板检测记录表》中填写实测值与合格判定。不含裁判标定真值。",
        size=9,
    )
    add_para(doc, "一、测量项目与基准要求", cn="黑体", size=11, bold=True)
    t = doc.add_table(rows=6, cols=4)
    t.style = "Table Grid"
    fill_table(
        t,
        [
            ["序号", "测量项目", "基准要求", "备注"],
            ["1", "板长 L", "80.00 mm ± 0.20 mm（79.80～80.20）", "最长边方向；避开外形毛刺"],
            ["2", "板宽 W", "60.00 mm ± 0.20 mm（59.80～60.20）", "垂直于板长方向"],
            ["3", "板厚 T", "1.60 mm ± 0.16 mm（1.44～1.76）", "板中非异常区；可多点取代表值"],
            ["4", "定位孔径", "2.00 mm ± 0.10 mm（1.90～2.10）", "指定定位孔内径；以丝印/说明为准"],
            ["5", "槽宽（或外形特征）", "3.00 mm ± 0.15 mm（2.85～3.15）", "有槽测槽宽；无槽则测赛场指定外形特征"],
        ],
    )
    add_para(doc, "二、测量与记录说明", cn="黑体", size=11, bold=True, space_before=8)
    for s in [
        "1. 使用游标卡尺（建议分度值 0.02 mm）测量；读数以 mm 为主，建议保留至 0.01 mm。",
        "2. 工艺参数可按 mm（mil）理解；记录实测值以 mm 为主。",
        "3. 将每项实测值与上表比对，在检测记录表判定合格/不合格。",
        "4. 本表不提供标准答案实测值；以赛场样件与本表基准为准。",
        "5. 不要求测量 0.30 mm 级微孔、线到板边微距等普通卡尺不可靠项目。",
        "6. 成果命名示例：赛位号_模块D；目录示例：D:\\提交资料\\模块D\\。",
    ]:
        add_para(doc, s, size=10.5)
    doc.save(str(path))
    print("[OK]", path.name)


def build_record_docx(path: Path):
    doc = new_doc()
    add_para(doc, "PCB光板检测记录表（学生组·模块D）", cn="黑体", size=12, bold=True, align=WD_ALIGN_PARAGRAPH.CENTER)
    add_para(
        doc,
        "备注：结合《验收要求表》《尺寸基准表》《板面分区说明》填写。命名：赛位号_模块D。不得标注姓名（按赛场规则）。本表不含电气通断栏。",
        size=9,
    )
    t0 = doc.add_table(rows=2, cols=4)
    t0.style = "Table Grid"
    fill_table(
        t0,
        [
            ["赛位号", "", "日期", ""],
            ["样件编号/版本", "", "板面方向确认", "顶/底已确认□"],
        ],
        header_bold=False,
    )
    # fix header style manually - first row labels
    for cell in t0.rows[0].cells:
        for p in cell.paragraphs:
            for r in p.runs:
                set_run_font(r, name_cn="黑体", size_pt=9, bold=True)

    add_para(doc, "一、D-1 检验准备", cn="黑体", size=11, bold=True, space_before=8)
    add_para(
        doc,
        "□ 文件齐全（记录表/验收表/尺寸基准/分区说明）  □ 卡尺零位已检  □ 放大镜/照明可用  □ 样件无异常运输损伤（如有：________）",
        size=10,
    )

    add_para(doc, "二、D-2 外观缺陷检测记录", cn="黑体", size=11, bold=True, space_before=6)
    add_para(doc, "须检顶底两面；缺陷分布可不均；同区可多处；部分区可无缺陷。", size=9)
    t1 = doc.add_table(rows=11, cols=7)
    t1.style = "Table Grid"
    rows = [["序号", "所在面", "缺陷类型", "网格区", "特征名", "位置/现象描述", "严重程度"]]
    for i in range(1, 11):
        rows.append([str(i), "", "", "", "", "", ""])
    fill_table(t1, rows)

    add_para(doc, "三、D-3 关键尺寸测量记录", cn="黑体", size=11, bold=True, space_before=8)
    t2 = doc.add_table(rows=6, cols=5)
    t2.style = "Table Grid"
    fill_table(
        t2,
        [
            ["测量项目", "基准要求", "实测值(mm)", "合格/不合格", "备注"],
            ["板长 L", "80.00±0.20 mm", "", "", ""],
            ["板宽 W", "60.00±0.20 mm", "", "", ""],
            ["板厚 T", "1.60±0.16 mm", "", "", ""],
            ["定位孔径", "2.00±0.10 mm", "", "", ""],
            ["槽宽/外形特征", "3.00±0.15 mm", "", "", ""],
        ],
    )

    add_para(doc, "四、D-4 缺陷分类与质量处置", cn="黑体", size=11, bold=True, space_before=8)
    add_para(
        doc,
        "1. 外观缺陷分类计数：线路____；焊盘/孔____；阻焊____；丝印____；外形____；其它____；合计____。",
        size=10.5,
    )
    add_para(doc, "2. 尺寸：□全部合格    □有不合格项：________________", size=10.5)
    add_para(doc, "3. 质量处置：□接收    □返工    □报废", size=10.5)
    add_para(doc, "4. 依据（对照验收要求表）：________________________________________________", size=10.5)
    add_para(doc, "5. 简要说明：________________________________________________", size=10.5)

    add_para(doc, "五、D-5 复核", cn="黑体", size=11, bold=True, space_before=8)
    add_para(
        doc,
        "□ 单位齐全  □ 分类合计与明细一致  □ 处置结论已填  □ 页码/附件完整  复核人签注（按赛场要求）：________",
        size=10.5,
    )
    add_para(
        doc,
        "（标准名称 GB/T 4588-2025；条款与细则以技术工作文件及验收要求表为准。评分结构见技术文件，样题以任务完成为主。）",
        size=9,
    )
    doc.save(str(path))
    print("[OK]", path.name)


def build_material_list_docx(path: Path):
    doc = new_doc()
    add_para(doc, "选手材料清单（学生组·模块D）", cn="黑体", size=12, bold=True, align=WD_ALIGN_PARAGRAPH.CENTER)
    add_para(doc, "印制电路制作工赛项 · 双面PCB光板外观与尺寸质量检测（学生组）", size=10.5, align=WD_ALIGN_PARAGRAPH.CENTER)
    add_para(doc, "一、选手可见材料", cn="黑体", size=11, bold=True)
    t = doc.add_table(rows=8, cols=3)
    t.style = "Table Grid"
    fill_table(
        t,
        [
            ["序号", "材料名称", "说明"],
            ["1", "双面光板质检样件", "FR-4 双面、未贴片；约 80×60 mm；含 8 处必检缺陷及干扰项；版本 A 或 B"],
            ["2", "PCB光板检测记录表（学生组·模块D）", "提交用；含 D-1～D-5，无电气栏"],
            ["3", "验收要求表（学生组·模块D）", "外观判定与接收/返工/报废口径"],
            ["4", "尺寸基准表（学生组·模块D）", "5 项基准；无裁判真值"],
            ["5", "板面分区说明（学生组·模块D）", "A1–D4 与特征名；无 TP 通断说明"],
            ["6", "游标卡尺、放大镜等", "赛场工位提供；以现场清单为准"],
            ["7", "本清单", "可与任务书合并印发"],
        ],
    )
    add_para(doc, "二、裁判内部材料（不下发选手）", cn="黑体", size=11, bold=True, space_before=8)
    add_para(
        doc,
        "缺陷参考答案（A/B）、尺寸参考答案、同义答案与采点说明、评分表骨架、标准板/备用板、缺陷照片与坐标。",
        size=10.5,
    )
    add_para(doc, "三、明确不提供/不考核", cn="黑体", size=11, bold=True, space_before=8)
    add_para(
        doc,
        "通断网络表、电气连通与故障定位表、万用表电阻档通断考核、上电、Gerber 量测、实际返修操作。",
        size=10.5,
    )
    doc.save(str(path))
    print("[OK]", path.name)


def build_defect_answer_docx(path: Path):
    doc = new_doc()
    add_para(doc, "缺陷参考答案（学生组·模块D · A/B 合订骨架）", cn="黑体", size=12, bold=True, align=WD_ALIGN_PARAGRAPH.CENTER)
    add_para(doc, "裁判用。投板后据实物补坐标与照片。类型固定 8 类；A/B 位置不同。", size=9)
    add_para(doc, "版本栏：□A  □B    板号：________    校验：________", size=10.5)

    def defect_table(title, face_plan):
        add_para(doc, title, cn="黑体", size=11, bold=True, space_before=8)
        t = doc.add_table(rows=9, cols=7)
        t.style = "Table Grid"
        rows = [["编号", "类型", "面", "网格", "特征名", "现象关键词", "必检"]]
        data = [
            ("D01", "线路缺口或颈缩", face_plan[0], "走线区", "颈缩/缺口/开路", "是"),
            ("D02", "线距异常", face_plan[1], "走线区", "线距不足/异常贴近", "是"),
            ("D03", "焊环不足", face_plan[2], "孔阵", "焊环一侧不足", "是"),
            ("D04", "漏钻", face_plan[3], "孔阵", "有盘无孔/未钻通", "是"),
            ("D05", "阻焊开窗偏移", face_plan[4], "阻焊对比区", "开窗偏移", "是"),
            ("D06", "阻焊桥异常", face_plan[5], "阻焊对比区", "桥缺失/异常", "是"),
            ("D07", "丝印残缺", face_plan[6], "丝印条", "字符残缺", "是"),
            ("D08", "外形缺口", face_plan[7], "板框", "外形缺口", "是"),
        ]
        grids_a = ["B2", "C2", "A2", "B4", "C1", "D2", "A3", "D4边"]
        for i, (n, typ, face, feat, key, must) in enumerate(data):
            rows.append([n, typ, face, grids_a[i], feat, key, must])
        fill_table(t, rows)

    defect_table(
        "一、A 版建议布局（投板后核定）",
        ["顶", "底", "顶", "顶", "顶", "底", "顶", "—"],
    )
    # B version with different grids note
    add_para(doc, "二、B 版规则", cn="黑体", size=11, bold=True, space_before=8)
    add_para(
        doc,
        "类型与 A 相同；网格/特征位置整体平移或镜像（示例：D01→C3，D02→B3，D03→A3，D04→C1，D05→B1，D06→D3，D07→D1，D08→A1边）。底面缺陷数仍 ≥3。正式以投板真值表为准。",
        size=10.5,
    )
    add_para(doc, "三、干扰项（不计入 8 必检）", cn="黑体", size=11, bold=True, space_before=8)
    add_para(doc, "2～4 处临界合格特征；误报不强制扣分。投板后另表列出。", size=10.5)
    add_para(doc, "四、底面计数核对", cn="黑体", size=11, bold=True, space_before=8)
    add_para(doc, "A/B 定稿时确认底层缺陷 ≥3 处并在照片中可复核。", size=10.5)
    doc.save(str(path))
    print("[OK]", path.name)


def build_size_answer_docx(path: Path):
    doc = new_doc()
    add_para(doc, "尺寸参考答案（学生组·模块D · A/B）", cn="黑体", size=12, bold=True, align=WD_ALIGN_PARAGRAPH.CENTER)
    add_para(doc, "裁判用。下列为命题名义与允许判定带；投板后填入三测均值作为标定真值。", size=9)
    t = doc.add_table(rows=6, cols=5)
    t.style = "Table Grid"
    fill_table(
        t,
        [
            ["项目", "名义", "选手合格带", "A标定真值(投板后)", "B标定真值(投板后)"],
            ["板长 L", "80.00 mm", "79.80～80.20", "____", "____"],
            ["板宽 W", "60.00 mm", "59.80～60.20", "____", "____"],
            ["板厚 T", "1.60 mm", "1.44～1.76", "____", "____"],
            ["定位孔径", "2.00 mm", "1.90～2.10", "____", "____"],
            ["槽宽/外形特征", "3.00 mm", "2.85～3.15", "____", "____"],
        ],
    )
    add_para(doc, "采点：实测在合格带内且判定正确给满分；轻微读数偏差但判定正确可酌情给测量分。", size=10.5, space_before=8)
    add_para(doc, "禁止将微孔、线到板边等未列入基准表的项目作为扣分点。", size=10.5)
    doc.save(str(path))
    print("[OK]", path.name)


def write_referee_mds():
    (PACK / "裁判" / "同义答案与采点说明.md").write_text(
        r"""# 同义答案与采点说明（学生组·模块D）

## 1. 缺陷类型同义

| 标准类型 | 可接受同义（关键词命中） |
|----------|--------------------------|
| 线路缺口或颈缩 | 缺口、开路、断线、线宽不足、颈缩、变窄 |
| 线距异常 | 线距不足、间距过小、快短路、异常贴近 |
| 焊环不足 | 焊环过小、铜环不足、偏孔致环窄、annular ring 不足 |
| 漏钻 | 未钻孔、有盘无孔、钻孔遗漏 |
| 阻焊开窗偏移 | 绿油偏、开窗不正、窗偏 |
| 阻焊桥异常 | 桥断、无桥、桥过窄、桥异常 |
| 丝印残缺 | 字符缺、丝印不全、缺笔、标识不清（须指向残缺） |
| 外形缺口 | 板边缺口、外形破损、缺角（非毛刺级） |

## 2. 定位采点

- 优先：所在面 + 网格正确
- 或：所在面 + 特征名 + 方位足以与真值唯一对应
- 面写错（顶/底）原则上定位分不给

## 3. 干扰项

- 选手多写干扰项：不强制扣外观识别分
- 漏写必检：按点扣

## 4. 质量处置

- 与主要必检缺陷及尺寸结论逻辑一致即可
- 表述“不合格”但勾选接收：处置分不给
- 接收/返工/报废三选一；须有简要依据
""",
        encoding="utf-8",
    )
    (PACK / "裁判" / "评分表骨架_40分.md").write_text(
        (RESEARCH / "06_评分骨架_40分.md").read_text(encoding="utf-8")
        if (RESEARCH / "06_评分骨架_40分.md").exists()
        else MD_FILES["06_评分骨架_40分.md"],
        encoding="utf-8",
    )


def write_board_mds():
    (PACK / "检测板" / "设计规格.md").write_text(MD_FILES["09_检测板设计规格与制板说明.md"], encoding="utf-8")
    (PACK / "检测板" / "制板说明.md").write_text(
        r"""# 制板说明（学生组模块D检测板）

1. 设计工具：立创EDA专业版；交付 Gerber + 钻孔 + 外形 + 源工程。  
2. 板材 FR-4，1.6 mm，双面，绿阻焊，白丝印；未贴片。  
3. 外形约 80×60 mm；定位孔标称 2.00 mm；可选 3.00 mm 槽。  
4. **有意缺陷保留**：缺口/颈缩、线距、焊环、漏钻、阻焊、丝印、外形等，禁止板厂 DFM 自动修复。  
5. 漏钻与外形缺口须在订单中书面说明“竞赛用检测板，保留设计缺陷”。  
6. A/B 分版本丝印：MOD-D-S-A / MOD-D-S-B。  
7. 数量与包装：按试做与裁判培训计划；防混板，分袋标注版本。  
8. 回片后执行 `research/student-module-d-pack/08_打样与验证清单.md`。
""",
        encoding="utf-8",
    )
    (PACK / "检测板" / "A_B差异与版本校验.md").write_text(
        r"""# A/B 差异与版本校验

| 项 | A | B |
|----|----|----|
| 丝印版本 | MOD-D-S-A | MOD-D-S-B |
| 缺陷类型集合 | 8 类固定池 | 同左 |
| 缺陷位置 | 真值表 A 列 | 真值表 B 列（平移/镜像） |
| 尺寸名义 | 同基准表 | 同基准表 |
| 尺寸标定真值 | 投板后三测 | 投板后三测（可在公差内微差） |
| 底面缺陷数 | ≥3 | ≥3 |
| 校验码（示例） | 待定 | 待定 |

## 校验步骤

1. 核对丝印版本与袋装标签一致  
2. 抽检 8 缺陷位置与答案表版本栏一致  
3. 尺寸标定录入裁判答案  
4. 混板风险：A/B 分色标签或分盒
""",
        encoding="utf-8",
    )


def write_pack_readme():
    (PACK / "00_说明与版本.md").write_text(
        f"""# 学生组_模块D 独立完整包

- 版本日期：{TS[:8]}
- 定位：双面PCB光板**外观与尺寸**质量检测（学生组）
- 时长：90 分钟（D-1～D-5）
- 分值：40 原始分（折算见技术文件待同步清单）
- **不含**电气通断/故障定位

## 目录

- `选手/` 任务相关表单与说明  
- `裁判/` 答案与采点  
- `检测板/` 设计规格与占位（源工程/Gerber/钻孔）  
- `_归档移出_电气旧件/` 自 `02_样题` 根目录移出的电气旧文件镜像  

## 完整版样题

正文回写于：`02_样题/印制电路制作工赛项_竞赛样题（完整版）.docx` 模块D段落。

## 研究包

`.trellis/tasks/07-20-module-d-bare-pcb-exam/research/student-module-d-pack/`
""",
        encoding="utf-8",
    )
    # player task pointer
    (PACK / "选手" / "任务书要点.md").write_text(
        r"""# 任务书要点（学生组·模块D）

完整表述以《印制电路制作工赛项_竞赛样题（完整版）》模块D为准。

- 对象：双面光板质检样件（专用检测板）
- 任务：D-1 准备 → D-2 外观 → D-3 尺寸 → D-4 分类与接收/返工/报废 → D-5 复核提交
- 材料：记录表、验收要求表、尺寸基准表、分区说明
- 不做：通断、故障定位、上电、量 Gerber
""",
        encoding="utf-8",
    )


# ---------- rewrite complete exam ----------
def replace_paragraph_text(paragraph, new_text, *, cn="仿宋", size=10.5, bold=False):
    """Replace all runs in paragraph with single formatted run, keep paragraph properties."""
    # clear
    p = paragraph._p
    for child in list(p):
        if child.tag == qn("w:r"):
            p.remove(child)
    run = paragraph.add_run(new_text)
    set_run_font(run, name_cn=cn, size_pt=size, bold=bold)
    return paragraph


def rewrite_complete_exam():
    path = SAMPLE / "印制电路制作工赛项_竞赛样题（完整版）.docx"
    # backup first
    bak_dir = ARCHIVE / f"module-d-student-rewrite-{TS}"
    bak_dir.mkdir(parents=True, exist_ok=True)
    shutil.copy2(path, bak_dir / path.name)
    print("[BAK]", bak_dir / path.name)

    doc = Document(str(path))

    # Map paragraph index -> new content from dump (indices may shift if we only replace text)
    # We'll scan by content markers
    replacements = []

    def set_by_exact(old_substr, new_text, **kwargs):
        replacements.append((old_substr, new_text, kwargs))

    # Title / overview tables handled separately

    para_updates = {
        # module title line
        "模块D：光板质量检测与缺陷分析": (
            "模块D：双面PCB光板外观与尺寸质量检测（学生组）",
            {"cn": "黑体", "size": 16, "bold": True},
        ),
    }

    # We'll do a broader pass
    for i, para in enumerate(doc.paragraphs):
        t = para.text.strip()
        if not t:
            continue

        if t == "模块D：光板质量检测与缺陷分析" or t.startswith("模块D：光板质量检测"):
            replace_paragraph_text(para, "模块D：双面PCB光板外观与尺寸质量检测（学生组）", cn="黑体", size=16, bold=True)
            continue

        if "（一）模块考核点" in t and i > 120:
            # next content para will be updated by content match
            pass

        if t.startswith("本模块考核选手对未贴片") or ("外观缺陷检测" in t and "通断" in t and "合格判定" in t):
            replace_paragraph_text(
                para,
                "本模块（学生组）考核选手对未贴片双面印制电路光板进行外观缺陷检测、关键尺寸测量、标准比对、缺陷分类与单件质量处置（接收/返工/报废）的能力。"
                "建议时长90分钟（D-1～D-5）；模块内原始分40分（折算与细则见技术工作文件）。"
                "不考核电气通断、故障定位、上电调试、实际返修及Gerber量测。",
                cn="仿宋",
                size=10.5,
            )
            continue

        if t.startswith("说明：本模块为独立质检任务") and "通断" in t:
            replace_paragraph_text(
                para,
                "说明：本模块为独立质检任务。检验对象为赛场提供的双面光板质检样件（专用检测板，FR-4双面、未贴片），"
                "与模块B/C的LoRa核心板设计及Gerber/工程文件任务相互独立。某批次双面光板完成后进入抽检环节，你作为质检人员，"
                "需完成规范检验、准确测量、对照验收要求进行分类判定，并给出接收/返工/报废结论及简要依据。",
                cn="仿宋",
                size=10.5,
            )
            continue

        if t.startswith("本模块不上电") and "贴装" in t:
            replace_paragraph_text(
                para,
                "本模块不上电，不进行整机或加电调试，不进行电气通断与故障定位，不考核贴装质量与元器件参数，不量测Gerber文件。",
                cn="仿宋",
                size=10.5,
            )
            continue

        if t.startswith("双面光板质检样件1块") and ("TP" in t or "测试点" in t):
            replace_paragraph_text(
                para,
                "双面光板质检样件1块（未贴片）。外形约80×60 mm，标称板厚1.6 mm。"
                "板面丝印含版本标识（A或B）、A1–D4分区网格及特征名（走线区、孔阵、VIA区、阻焊对比区、丝印条、板框等）。"
                "样件含8处须识别的稳定缺陷（顶/底均可能存在，须检查两面），并可能含若干接近合格临界的干扰特征。",
                cn="仿宋",
                size=10.5,
            )
            continue

        if "尺寸基准要求表与电气连通" in t or ("电气连通检测与故障定位表" in t and "GB/T 4588" in t):
            replace_paragraph_text(
                para,
                "尺寸基准表与验收要求表由赛场随卷下发；工艺相关参数采用mm（mil）双单位理解，实测记录以mm为主。"
                "缺陷与尺寸判定采用本卷竞赛用验收表述；引用标准名称GB/T 4588-2025时，对应条款以正式文本核对为准；"
                "等级/扣分/折算等细则以技术工作文件及赛场说明为准。",
                cn="仿宋",
                size=10.5,
            )
            continue

        # Task headings and bodies
        if t.startswith("D-1") and "外观" in t:
            replace_paragraph_text(para, "D-1  检验准备", cn="微软雅黑", size=12, bold=True)
            continue
        if t.startswith("对样件线路、焊盘") and "外观检查" in t:
            replace_paragraph_text(
                para,
                "核对样件编号与版本丝印、确认顶/底方向与分区约定；检查卡尺零位与放大镜/照明；在记录表填写基本信息并勾选准备项。（建议5分钟）",
                cn="仿宋",
                size=10.5,
            )
            continue

        # Old D-1 fill requirements become D-2
        if t == "填写要求" and i >= 148:
            # leave label; surrounding will change
            pass

        if t.startswith("D-2") and "尺寸" in t:
            replace_paragraph_text(para, "D-2  外观缺陷检测", cn="微软雅黑", size=12, bold=True)
            continue

        if t.startswith("使用卡尺等工具测量下列项目"):
            replace_paragraph_text(
                para,
                "对样件线路、焊盘/孔、阻焊、丝印、外形等进行外观检查（须检顶面与底面），将发现的缺陷记录在检测记录表中。（建议35分钟）",
                cn="仿宋",
                size=10.5,
            )
            continue

        # Old dimension list items under old D-2 - repurpose carefully by context
        # We'll handle with a state machine below after first pass using indices from dump

        if t.startswith("D-3") and ("电气" in t or "通断" in t):
            replace_paragraph_text(para, "D-3  关键尺寸测量", cn="微软雅黑", size=12, bold=True)
            continue

        if "数字万用表" in t or "电阻档" in t or t.startswith("依据《电气连通"):
            replace_paragraph_text(
                para,
                "使用卡尺测量板长、板宽、板厚、定位孔径、槽宽或指定外形特征尺寸，填写实测值，并与尺寸基准表比对判定合格/不合格。（建议25分钟）",
                cn="仿宋",
                size=10.5,
            )
            continue

        if t.startswith("测量判据：R≤") or "禁止对样件上电" in t and "R≤" in t:
            replace_paragraph_text(
                para,
                "不要求测量普通卡尺无法可靠完成的项目（如0.30 mm级微孔、线到板边微距等）。记录以mm为主并保留适当精度。",
                cn="仿宋",
                size=10.5,
            )
            continue

        if "基础连通测量" in t or "分段网络" in t or "网络间是否存在短路" in t or "最可能故障段" in t:
            replace_paragraph_text(para, "（详见尺寸基准表五项；在检测记录表填写实测值与合格判定。）", cn="仿宋", size=10.5)
            continue

        if "样题不公布全部应通" in t or "故障定位与判断依据为主" in t:
            replace_paragraph_text(
                para,
                "说明：尺寸与外观真值不在选手卷公布；以赛场样件与随卷基准/验收表为准。",
                cn="仿宋",
                size=10.5,
            )
            continue

        if t.startswith("D-4") and "合格判定" in t:
            replace_paragraph_text(para, "D-4  缺陷分类与质量判定", cn="微软雅黑", size=12, bold=True)
            continue

        if "汇总电气连通" in t:
            replace_paragraph_text(
                para,
                "（3）对照《验收要求表》，给出样件质量处置结论：接收/返工/报废，并写出简要依据；",
                cn="仿宋",
                size=10.5,
            )
            continue

        if t.startswith("（4）综合以上结果，判定该样件合格或不合格"):
            replace_paragraph_text(
                para,
                "（4）不要求分析深层制造工艺根因；",
                cn="仿宋",
                size=10.5,
            )
            continue

        if "判定时参考标准名称" in t and "网络表" in t:
            replace_paragraph_text(
                para,
                "（5）判定时参考标准名称GB/T 4588-2025、赛场提供的验收要求表与尺寸基准表；细则以技术工作文件及赛场说明为准。",
                cn="仿宋",
                size=10.5,
            )
            continue

        if t.startswith("D-5") and "注意" in t:
            replace_paragraph_text(para, "D-5  复核与提交", cn="微软雅黑", size=12, bold=True)
            continue

        if "合理分配外观检测、尺寸测量与电气" in t:
            replace_paragraph_text(
                para,
                "1. 请合理分配检验准备、外观检测、尺寸测量、质量判定与复核时间（建议合计90分钟）；时长与评分以技术工作文件/竞赛平台为准；",
                cn="仿宋",
                size=10.5,
            )
            continue

        if t.startswith("2. 本模块与模块B/C独立") and "光板样件" in t:
            replace_paragraph_text(
                para,
                "2. 本模块与模块B/C独立，检验对象仅为赛场提供的光板样件，不量测Gerber，不上电，不进行电气通断与故障定位；",
                cn="仿宋",
                size=10.5,
            )
            continue

        # Tools paragraph if exists
        if "万用表" in t and "卡尺" in t and ("模块" in t or "工具" in t):
            if "D" in t or "光板" in t or i > 130:
                replace_paragraph_text(
                    para,
                    t.replace("万用表、", "").replace("万用表，", "").replace("、万用表", ""),
                    cn="仿宋",
                    size=10.5,
                )

    # Second pass: fix D-1/D-2 body numbering that may be inconsistent
    # Rebuild module D task section more carefully by finding D-1..D-5 block
    texts = [p.text for p in doc.paragraphs]
    # Find D-1 heading index
    d1_idx = None
    for i, t in enumerate(texts):
        if t.strip().startswith("D-1"):
            d1_idx = i
            break

    if d1_idx is not None:
        # Expected structure after rewrite - inject fill requirements for D-2 if missing
        # Find paragraphs between D-1 and D-5 and normalize key lines
        block_map = {
            "D-1": "D-1  检验准备",
            "D-2": "D-2  外观缺陷检测",
            "D-3": "D-3  关键尺寸测量",
            "D-4": "D-4  缺陷分类与质量判定",
            "D-5": "D-5  复核与提交",
        }
        for i, p in enumerate(doc.paragraphs):
            ts = p.text.strip()
            for key, val in block_map.items():
                if ts.startswith(key) and ("检验" in ts or "外观" in ts or "尺寸" in ts or "判定" in ts or "复核" in ts or "注意" in ts or len(ts) < 40):
                    if ts != val and ts.startswith(key):
                        # only if it's a heading-like short line
                        if len(ts) < 50:
                            replace_paragraph_text(p, val, cn="微软雅黑", size=12, bold=True)

        # Fix fill requirement lines that still describe old D-1 appearance under D-1
        for i, p in enumerate(doc.paragraphs):
            ts = p.text.strip()
            if ts.startswith("（1）每发现一处缺陷"):
                # ensure we're in appearance section - OK for D-2
                pass
            if ts.startswith("（1）板长") or ts == "（1）板长；":
                # old dimension bullets - replace whole set if still present after D-3
                pass

    # Update tables
    for table in doc.tables:
        for row in table.rows:
            for cell in row.cells:
                ct = cell.text.strip()
                if ct == "光板质量检测与缺陷分析":
                    set_cell_text(cell, "双面PCB光板外观与尺寸质量检测（学生组）", cn="仿宋", size=10.5)
                elif "通断" in ct and "判定" in ct:
                    set_cell_text(cell, "用于记录外观、尺寸与质量处置", cn="仿宋", size=9)
                elif "电气连通检测与故障定位表" in ct:
                    set_cell_text(cell, "验收要求表（学生组·模块D）", cn="仿宋", size=9)
                elif "电阻测量与分段故障定位" in ct:
                    set_cell_text(cell, "外观判定与接收/返工/报废口径", cn="仿宋", size=9)
                elif ct == "尺寸基准要求表":
                    set_cell_text(cell, "尺寸基准表（学生组·模块D）", cn="仿宋", size=9)
                elif "分区与测试点说明" in ct:
                    set_cell_text(cell, "板面分区说明（学生组·模块D）", cn="仿宋", size=9)
                elif "A1–D4网格及TP说明" in ct or "TP说明" in ct:
                    set_cell_text(cell, "A1–D4网格及特征名（无电气测点）", cn="仿宋", size=9)
                elif "含预设缺陷（专用简化检测板）" in ct and "未贴片" in ct:
                    set_cell_text(cell, "FR-4双面、未贴片；约80×60 mm；8处必检缺陷（专用检测板，A/B版）", cn="仿宋", size=9)

    # Third pass: clean residual electrical terms in module D region only
    # Identify range: from first 模块D： to end of document (or before any 职工 if exists)
    start = None
    for i, p in enumerate(doc.paragraphs):
        if p.text.strip().startswith("模块D："):
            start = i
            break
    if start is not None:
        for i in range(start, len(doc.paragraphs)):
            p = doc.paragraphs[i]
            t = p.text
            if not t:
                continue
            # skip if somehow employee section - none expected
            new_t = t
            for bad, good in [
                ("通断结果", "尺寸结果"),
                ("测试点通断检测", "关键尺寸测量"),
                ("通断网络表", "验收要求表"),
                ("应通/应断", "合格/不合格"),
                ("应通应断", "合格不合格"),
                ("NET_A", "（已取消）"),
                ("NET_B", "（已取消）"),
                ("电阻档", "卡尺测量"),
                ("故障定位", "质量判定"),
                ("高阻", "尺寸超差"),
            ]:
                if bad in new_t:
                    # only auto-replace if sentence still makes sense; safer to blank electrical sentences
                    pass
            # Hard clean specific residual phrases
            if any(k in t for k in ["万用表电阻", "应通", "应断", "NET_", "蜂鸣档", "开路、高阻", "分段故障"]):
                if "不进行电气" not in t and "不考核电气" not in t:
                    # neutralize
                    if "D-" in t or "测量" in t or "（" in t:
                        replace_paragraph_text(
                            p,
                            "（本项已改为尺寸测量或质量判定相关要求，详见 D-3/D-4；不进行电气通断与故障定位。）",
                            cn="仿宋",
                            size=10.5,
                        )

    # Fix dimension list under D-3: if old (1)板长 still exists after D-3 heading
    mode = None
    for i, p in enumerate(doc.paragraphs):
        ts = p.text.strip()
        if ts.startswith("D-1"):
            mode = "D1"
        elif ts.startswith("D-2"):
            mode = "D2"
        elif ts.startswith("D-3"):
            mode = "D3"
        elif ts.startswith("D-4"):
            mode = "D4"
        elif ts.startswith("D-5"):
            mode = "D5"
        elif mode == "D1":
            if ts.startswith("（1）每发现"):
                # appearance fills wrongly under D1 - move content expectation: replace with prepare checklist items
                replace_paragraph_text(p, "（1）核对样件编号、版本丝印（A/B）与记录表基本信息；", cn="仿宋", size=10.5)
            elif ts.startswith("（2）填写内容：所在面"):
                replace_paragraph_text(p, "（2）确认顶/底方向与《板面分区说明》一致；", cn="仿宋", size=10.5)
            elif ts.startswith("（3）位置描述"):
                replace_paragraph_text(p, "（3）检查卡尺零位、放大镜与照明；", cn="仿宋", size=10.5)
            elif ts.startswith("（4）缺陷类型"):
                replace_paragraph_text(p, "（4）勾选检验准备项；如有运输损伤先记录；", cn="仿宋", size=10.5)
            elif ts.startswith("（5）缺陷类别"):
                replace_paragraph_text(p, "（5）确认随卷材料齐全：记录表、验收要求表、尺寸基准表、分区说明。", cn="仿宋", size=10.5)
            elif ts == "填写要求":
                replace_paragraph_text(p, "准备要求", cn="黑体", size=10.5, bold=True)
        elif mode == "D2":
            if ts.startswith("（1）板长"):
                replace_paragraph_text(p, "（1）每发现一处缺陷，在记录表中填写一行；", cn="仿宋", size=10.5)
            elif ts.startswith("（2）板宽"):
                replace_paragraph_text(
                    p,
                    "（2）填写内容：所在面（顶/底）、缺陷类型、网格区、特征名、位置描述、现象描述、严重程度（轻/中/重）；",
                    cn="仿宋",
                    size=10.5,
                )
            elif ts.startswith("（3）板厚"):
                replace_paragraph_text(
                    p,
                    "（3）位置描述应便于复核，例如“顶层B2/走线区/中部”，无需精确坐标；",
                    cn="仿宋",
                    size=10.5,
                )
            elif ts.startswith("（4）最小孔径") or ts.startswith("（4）定位"):
                replace_paragraph_text(p, "（4）缺陷类型应具体明确（如“线路颈缩”“漏钻”而非“线路有问题”）；", cn="仿宋", size=10.5)
            elif ts.startswith("（5）走线距板边") or ts.startswith("（5）槽宽"):
                replace_paragraph_text(
                    p,
                    "（5）缺陷类别可归入：线路、焊盘/孔、阻焊、丝印、外形、其它。",
                    cn="仿宋",
                    size=10.5,
                )
            elif "记录实测值时以mm为主" in ts:
                replace_paragraph_text(
                    p,
                    "填写要求：须检查顶面与底面；缺陷分布可不均。类型示例：线路缺口或颈缩、线距异常、焊环不足、漏钻、阻焊开窗偏移、阻焊桥异常、丝印残缺、外形缺口等。",
                    cn="仿宋",
                    size=10.5,
                )
        elif mode == "D3":
            if "对样件线路" in ts and "外观" in ts:
                replace_paragraph_text(
                    p,
                    "使用卡尺测量下列项目，填写实测值，并与尺寸基准表比对，判定合格/不合格：（建议25分钟）",
                    cn="仿宋",
                    size=10.5,
                )
            elif ts.startswith("（1）每发现"):
                replace_paragraph_text(p, "（1）板长；", cn="仿宋", size=10.5)
            elif ts.startswith("（2）填写内容"):
                replace_paragraph_text(p, "（2）板宽；", cn="仿宋", size=10.5)
            elif ts.startswith("（3）位置描述"):
                replace_paragraph_text(p, "（3）板厚；", cn="仿宋", size=10.5)
            elif ts.startswith("（4）缺陷类型"):
                replace_paragraph_text(p, "（4）定位孔径；", cn="仿宋", size=10.5)
            elif ts.startswith("（5）缺陷类别"):
                replace_paragraph_text(p, "（5）槽宽或指定外形特征尺寸。", cn="仿宋", size=10.5)
            elif "详见尺寸基准表" in ts or "本项已改为尺寸" in ts:
                replace_paragraph_text(
                    p,
                    "不要求测量普通卡尺无法可靠完成的项目（如0.30 mm级微孔、线到板边微距等）。记录以mm为主并保留适当精度。",
                    cn="仿宋",
                    size=10.5,
                )

        elif mode == "D4":
            if ts.startswith("（1）对已记录外观缺陷"):
                pass  # ok
            elif ts.startswith("（2）汇总尺寸"):
                pass
            elif ts.startswith("（3）对照"):
                pass
            elif "合格或不合格" in ts and "简要理由" in ts:
                replace_paragraph_text(
                    p,
                    "（4）不要求分析深层制造工艺根因；",
                    cn="仿宋",
                    size=10.5,
                )

        elif mode == "D5":
            if "纸质记录表填写完整" in ts or ts.startswith("⚠"):
                replace_paragraph_text(
                    p,
                    "检查记录完整性、单位、分类数量与明细一致性、处置结论；纸质记录表填写完整后按赛场要求提交；"
                    "如需电子版，保存到示例目录 D:\\提交资料\\模块D\\，U盘命名示例：赛位号_模块D。",
                    cn="仿宋",
                    size=10.5,
                )

    doc.save(str(path))
    print("[OK] rewritten", path.name)
    return path


def scan_residuals(path: Path):
    doc = Document(str(path))
    start = None
    hits = []
    keys = ["通断", "故障定位", "电阻档", "应通", "应断", "NET_", "万用表", "高阻", "蜂鸣"]
    for i, p in enumerate(doc.paragraphs):
        if p.text.strip().startswith("模块D："):
            start = i
        if start is not None and i >= start:
            for k in keys:
                if k in p.text:
                    # allow negations
                    if any(n in p.text for n in ["不进行电气", "不考核电气", "不设", "取消", "不要求万用表", "不上电"]):
                        if k in ["通断", "故障定位", "万用表"] and ("不" in p.text or "取消" in p.text):
                            continue
                    hits.append((i, k, p.text[:120]))
    for table in doc.tables:
        for row in table.rows:
            row_t = " | ".join(c.text.strip() for c in row.cells)
            if "模块D" in row_t or "光板" in row_t or "验收" in row_t or "电气" in row_t:
                for k in keys:
                    if k in row_t and "不" not in row_t:
                        hits.append((-1, k, row_t[:120]))
    return hits


def copy_research_scoring_into_pack():
    write_referee_mds()
    write_board_mds()
    write_pack_readme()


def main():
    print("ROOT", ROOT)
    write_md_pack()
    ensure_dirs()
    move_electrical_legacy()
    # player docs
    build_partition_docx(PACK / "选手" / "学生组_模块D_板面分区说明.docx")
    build_acceptance_docx(PACK / "选手" / "学生组_模块D_验收要求表.docx")
    build_dimension_docx(PACK / "选手" / "学生组_模块D_尺寸基准表.docx")
    build_record_docx(PACK / "选手" / "学生组_模块D_检测记录表.docx")
    build_material_list_docx(PACK / "选手" / "学生组_模块D_选手材料清单.docx")
    # referee docs
    build_defect_answer_docx(PACK / "裁判" / "学生组_模块D_缺陷参考答案_AB.docx")
    build_size_answer_docx(PACK / "裁判" / "学生组_模块D_尺寸参考答案_AB.docx")
    copy_research_scoring_into_pack()
    # also place copies of player docs naming without long prefix? already clear
    rewrite_complete_exam()
    hits = scan_residuals(SAMPLE / "印制电路制作工赛项_竞赛样题（完整版）.docx")
    print("=== RESIDUAL SCAN (module D region) count=", len(hits))
    for h in hits[:40]:
        print(h)
    # risk note
    risk = PACK / "00_后续风险与未完成项.md"
    risk.write_text(
        r"""# 后续风险与未完成项

1. **未实际立创EDA布线/投板**：源工程、Gerber、钻孔仅为占位目录。  
2. **缺陷坐标与照片**：裁判答案为骨架，须打样后回填。  
3. **A/B 试做未做**：等难度验证、裁判一致率未实测。  
4. **技术文件正文未改**：仅有待同步清单。  
5. **职工组**：本轮未改；若完整版仅学生口径模块D，职工组若共用文件需另册。  
6. **根目录旧模块D表单**：部分非电气旧文件（如旧检测记录表）仍留在 `02_样题/` 根目录作历史参考；现行以 `学生组_模块D/` 为准。  
7. **国标条款**：GB/T 4588-2025 正式条款待核对。  
""",
        encoding="utf-8",
    )
    print("[DONE]")


if __name__ == "__main__":
    main()
