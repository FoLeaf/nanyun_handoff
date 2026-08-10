# -*- coding: utf-8 -*-
"""
从人工复审版生成正式完整版样题，并交叉对齐 B/C 配套文件。
不修改 01_技术文件 正文；不删除人工复审版。
"""
from __future__ import annotations

import re
import shutil
from copy import deepcopy
from datetime import datetime
from pathlib import Path

from docx import Document
from docx.oxml.ns import qn
from docx.text.paragraph import Paragraph

ROOT = Path(__file__).resolve().parents[2]
SAMPLE = ROOT / "02_样题"
BACKUP_ROOT = ROOT / "05_归档备份"
TASK = ROOT / ".trellis" / "tasks" / "07-17-sample-exam-review-refine"
STAMP = datetime.now().strftime("%Y%m%d_%H%M%S")
BACKUP_DIR = BACKUP_ROOT / f"sample-exam-refine-{STAMP}"

HUMAN = SAMPLE / "印制电路制作工赛项_竞赛样题（完整版）人工复审.docx"
OFFICIAL = SAMPLE / "印制电路制作工赛项_竞赛样题（完整版）.docx"

# 将改动的配套文件（旧路径）
COMPANION_MAP = {
    SAMPLE / "模块A样题_EDA工程设计_v4.docx": SAMPLE / "模块B样题_EDA工程设计.docx",
    SAMPLE / "模块B样题_CAM审核与工艺文件编制.docx": SAMPLE / "模块C样题_CAM审核与工艺文件编制.docx",
    SAMPLE / "模块B样题_缺陷记录表.docx": SAMPLE / "模块C样题_缺陷记录表.docx",
    SAMPLE / "模块B样题_缺陷记录表_参考答案.docx": SAMPLE / "模块C样题_缺陷记录表_参考答案.docx",
    SAMPLE / "模块C样题_检测记录表.docx": SAMPLE / "模块D样题_检测记录表.docx",
    SAMPLE / "简版制程工艺卡模板（仅基本信息）.docx": SAMPLE / "简版制程工艺卡模板（仅基本信息）.docx",
    SAMPLE / "简版制程工艺卡（参考答案版）.docx": SAMPLE / "简版制程工艺卡（参考答案版）.docx",
}


def set_run_text(run, text: str) -> None:
    run.text = text


def replace_paragraph_text(paragraph: Paragraph, new_text: str) -> None:
    """尽量保留首 run 样式，整段替换文本。"""
    if not paragraph.runs:
        paragraph.add_run(new_text)
        return
    paragraph.runs[0].text = new_text
    for run in paragraph.runs[1:]:
        run.text = ""


def set_cell_text(cell, text: str) -> None:
    """替换单元格首段文本，清空多余段落。"""
    if not cell.paragraphs:
        return
    # 保留第一段
    p0 = cell.paragraphs[0]
    replace_paragraph_text(p0, text)
    # 清空后续段落
    for p in cell.paragraphs[1:]:
        replace_paragraph_text(p, "")


def paragraph_full_text(doc: Document) -> str:
    parts = [p.text for p in doc.paragraphs]
    for t in doc.tables:
        for row in t.rows:
            for cell in row.cells:
                parts.append(cell.text)
    return "\n".join(parts)


def backup_files() -> None:
    BACKUP_DIR.mkdir(parents=True, exist_ok=True)
    to_backup = [OFFICIAL, HUMAN]
    for src in COMPANION_MAP:
        if src.exists():
            to_backup.append(src)
    # 也可能已是新名
    for dst in COMPANION_MAP.values():
        if dst.exists() and dst not in to_backup:
            to_backup.append(dst)
    for f in to_backup:
        if f.exists():
            shutil.copy2(f, BACKUP_DIR / f.name)
    print(f"[backup] -> {BACKUP_DIR}")


# ---------------------------------------------------------------------------
# 正式完整版
# ---------------------------------------------------------------------------

FULL_PARA_EXACT = {
    11: (
        "本样题以国家职业技能标准《印制电路制作工》为依据，结合行业发展新技术、新工艺进行命制，"
        "涵盖模块A平台测试（理论知识，在竞赛平台完成）及模块B/C/D操作技能。总时长框架约4小时。"
        "各模块时长与评分以技术工作文件及竞赛平台为准；请合理分配本模块各子任务时间。"
    ),
    24: "模块A：平台测试（理论知识）",
    25: (
        "模块A为理论知识平台测试，试题由竞赛平台统一下发与作答。"
        "正式题型、题量与评分细则以竞赛平台及技术工作文件为准。"
        "本竞赛样题仅对模块A作简要说明，不提供理论试题或样题展示；操作技能样题见模块B/C/D。"
    ),
    26: "模块B：EDA工程设计（续设计）",
    32: (
        "作为PCB Layout工程师，您需要完成以下工作：自建定制连接器封装 → 补全原理图 → 设置设计规则 "
        "→ PCB布局 → PCB布线 → 输出生产Gerber文件，确保电路板满足可制造性要求和电气性能要求。"
    ),
    38: "B-0  自建连接器封装",
    49: (
        "打开提供的立创EDA工程文件，根据以下子电路功能描述，补全对应子电路（原理图中已红色高亮）。"
        "元件型号与封装以赛场下发半成品工程文件及库内推荐封装为准，本样题不单独下发BOM清单与封装对照表。"
    ),
    53: (
        "（3）复位与BOOT0电路：NRST引脚通过10kΩ上拉电阻接3V3，并可并联约100nF电容至GND构成RC复位；"
        "复位按键SW-RST一端接NRST，另一端接GND，按下时拉低复位（MCU的NRST为低电平复位有效）。"
        "BOOT0引脚通过10kΩ下拉电阻接GND（正常启动模式）；若半成品工程已预置BOOT0网络，请按工程网络连接补全，"
        "确保复位与启动配置可独立完成。"
    ),
    54: (
        "（4）LED指示电路：共3个LED，位号与二极管区分。"
        "1个电源指示灯LED-PWR：阳极经限流电阻（建议470Ω）接3V3，阴极接GND（或按工程给定接法）；"
        "2个用户LED（LED1、LED2）：阳极分别接MCU的PA11、PA12，阴极经限流电阻（建议1kΩ）接GND，"
        "MCU输出低电平时点亮。"
        "注意：电源保护电路中的肖特基二极管位号为D1、TVS为D2，不得与LED位号混用。"
    ),
    57: (
        "（7）NE555多谐振荡器：采用NE555构成无稳态（多谐）振荡电路，供电为+5V与GND。"
        "定时网络由电阻、电位器与电容构成（可按常见无稳态接法：RA、RB/电位器、C），"
        "通过电位器调节输出方波频率；OUT输出接MCU的PA8（TIM1_CH1，用于输入捕获测频）。"
        "复位脚（RESET）接+5V，控制脚（CONT）可按常规经电容旁路至GND。"
        "请补全至可独立振荡并输出至PA8，具体阻容取值以半成品工程标注或库内常用值为准。"
    ),
    60: (
        "（2）设置线宽规则：信号线 0.15mm（6mil）；"
        "电源线分级：3V3 ≥ 0.8mm（32mil），5V ≥ 1.0mm（40mil）；"
    ),
    61: "（3）设置过孔规则：内径 0.3mm（12mil），外径 0.6mm（24mil）；",
    62: "（4）设置安全间距规则：线线间距、线焊盘间距、孔线间距均为 0.15mm（6mil）；",
    63: "（5）设置阻焊规则：焊盘1:1开窗，阻焊桥最小宽度 0.1mm（4mil）。",
    67: (
        "（1）板框尺寸：以赛场下发半成品工程文件中的板框为准（含外形与倒圆角），"
        "不得自行改动板框尺寸；在给定板框区域内完成布局。"
    ),
    69: "（3）晶振电路靠近MCU晶振引脚；",
    70: "（4）LoRa模块放置在板边缘，天线端靠近板边缘；",
    71: "（5）元件布局按功能模块分区（电源区、主控区、接口区、传感器区）；",
    72: "（6）调整位号丝印位置，确保清晰可读，不与焊盘重叠。",
    75: (
        "（1）电源线加宽：3V3线宽 ≥ 0.8mm（32mil），5V线宽 ≥ 1.0mm（40mil）；"
        "普通信号线宽 0.15mm（6mil）；"
    ),
    77: "（3）信号线：普通信号线宽 0.15mm（6mil），关键信号（晶振、复位）尽量短；",
    90: (
        "⚠ 注意：所有文件保存到 \"D:\\提交资料\\模块B\\\" 目录下，比赛结束前统一拷贝到U盘根目录提交。"
        "文件名中 {工位号}/赛位号 请替换为您的实际赛位号。U盘提交包命名示例：赛位号_模块B。"
    ),
    91: "1. 请合理分配本模块各子任务时间；时长与评分以技术工作文件/竞赛平台为准；",
    98: "模块C：CAM审核与工艺文件编制",
    130: "1. 请合理分配缺陷识别和工艺卡填写的时间；时长与评分以技术工作文件/竞赛平台为准；",
    135: "模块D：成品板质量检测与缺陷分析",
    137: (
        "本模块考核选手对成品PCB板进行质量检测和缺陷分析的能力，"
        "包括外观检测、尺寸测量、电气连通性检查、缺陷分类及质量判定等。"
        "时长与评分以技术工作文件为准。"
    ),
    140: (
        "说明：本模块为独立质检任务，检测对象为赛场提供的成品板（与模块B/C的LoRa核心板设计任务相互独立）。"
        "某批次成品板生产完成后，质量检验部门需要对成品板进行抽检。"
        "你作为质检员，需要对一块待测PCB板进行全面检测，识别其中的缺陷并给出合格/不合格判定。"
    ),
    157: (
        "  将缺陷按“线路缺陷、焊盘/孔缺陷、阻焊缺陷、丝印缺陷、外形缺陷”分类，"
        "依据GB/T 4588-2025标准综合判定该板是否合格，并说明理由。"
        "具体判定细则以技术工作文件及赛场下发材料为准。"
    ),
    160: "2. 文件命名：赛位号_模块D（纸质表按赛场要求提交；如需电子版命名同此规则）",
}


def transform_full_exam(src: Path, dst: Path) -> Document:
    doc = Document(str(src))

    # 段落级精确替换
    for idx, text in FULL_PARA_EXACT.items():
        if idx < len(doc.paragraphs):
            replace_paragraph_text(doc.paragraphs[idx], text)

    # 表格
    # T1 模块表
    t1 = doc.tables[1]
    set_cell_text(t1.rows[1].cells[0], "模块A")
    set_cell_text(t1.rows[1].cells[1], "平台测试（理论知识，详见竞赛平台/技术工作文件）")
    set_cell_text(t1.rows[2].cells[0], "模块B")
    set_cell_text(t1.rows[2].cells[1], "EDA工程设计（续设计）")
    set_cell_text(t1.rows[3].cells[0], "模块C")
    set_cell_text(t1.rows[3].cells[1], "CAM审核与工艺文件编制")
    set_cell_text(t1.rows[4].cells[0], "模块D")
    set_cell_text(t1.rows[4].cells[1], "成品板质量检测与缺陷分析")

    # T2 提交表
    t2 = doc.tables[2]
    set_cell_text(t2.rows[1].cells[0], "模块B")
    set_cell_text(t2.rows[1].cells[1], "立创EDA工程文件 + Gerber压缩包 + 2D预览图等（见模块B成果物表）")
    set_cell_text(t2.rows[1].cells[2], "赛位号_模块B（本地目录：D:\\提交资料\\模块B\\）")
    set_cell_text(t2.rows[2].cells[0], "模块C")
    set_cell_text(t2.rows[2].cells[1], "缺陷记录表 + 简版制程工艺卡")
    set_cell_text(t2.rows[2].cells[2], "赛位号_模块C（本地目录：D:\\提交资料\\模块C\\）")
    set_cell_text(t2.rows[3].cells[0], "模块D")
    set_cell_text(t2.rows[3].cells[1], "PCB质量检测记录表")
    set_cell_text(t2.rows[3].cells[2], "赛位号_模块D（本地目录：D:\\提交资料\\模块D\\）")

    # T3 设计要求表（模块B简介区）
    t3 = doc.tables[3]
    set_cell_text(
        t3.rows[2].cells[1],
        "以赛场下发半成品工程文件中的板框为准（矩形板框及倒圆角以工程为准），不得自行改动板框",
    )
    set_cell_text(
        t3.rows[4].cells[1],
        "信号线 0.15mm（6mil）/ 线距 0.15mm（6mil）；"
        "电源线：3V3 ≥ 0.8mm（32mil），5V ≥ 1.0mm（40mil）",
    )
    set_cell_text(
        t3.rows[5].cells[1],
        "内径 0.3mm（12mil），外径 0.6mm（24mil）（焊盘；公差按板厂能力，全卷统一）",
    )
    set_cell_text(
        t3.rows[7].cells[1],
        "白色丝印，字符高度 ≥ 1.0mm（40mil），字符不允许上焊盘",
    )

    # T5 工艺参数表
    t5 = doc.tables[5]
    set_cell_text(t5.rows[1].cells[1], "1.6mm（63mil）")
    set_cell_text(t5.rows[3].cells[1], "信号线 0.15mm（6mil）")
    set_cell_text(
        t5.rows[3].cells[2],
        "电源线：3V3 ≥ 0.8mm（32mil）；5V ≥ 1.0mm（40mil）",
    )
    set_cell_text(t5.rows[4].cells[1], "0.15mm（6mil）")
    set_cell_text(t5.rows[5].cells[1], "内径 0.3mm（12mil），外径 0.6mm（24mil）")
    set_cell_text(t5.rows[6].cells[1], "字符高度 ≥ 1.0mm（40mil），字宽 ≥ 0.15mm（6mil）")

    # T6 子电路关键要求 — 复位补全表述已在正文
    set_cell_text(doc.tables[6].rows[3].cells[2], "上拉+可选RC + BOOT0下拉 + 复位按键")
    set_cell_text(doc.tables[6].rows[4].cells[2], "LED1/LED2 用户灯 + LED-PWR 电源灯（位号勿与D1/D2冲突）")
    set_cell_text(doc.tables[6].rows[7].cells[2], "无稳态接法 + 电位器调频，OUT→PA8，供电+5V")

    # T7 成果物：去掉独立BOM要求
    t7 = doc.tables[7]
    # 删除最后一行 BOM 或改为可选备注；直接改为“不要求单独提交BOM”
    set_cell_text(t7.rows[5].cells[1], "（不要求）不单独提交BOM清单")
    set_cell_text(t7.rows[5].cells[2], "—")
    set_cell_text(t7.rows[5].cells[3], "—")
    # 文件名中工位号说明保留 B 前缀

    # T9 CAM 工艺能力表：mm 主、mil 辅
    t9 = doc.tables[9]
    set_cell_text(t9.rows[0].cells[2], "工艺能力（mm / mil）")
    dual_updates = {
        (1, 2): "0.10mm（4mil）",
        (2, 2): "0.10mm（4mil）",
        (4, 2): "0.30mm（12mil）",
        (5, 2): "0.25mm（10mil）",
        (6, 2): "0.18mm（7mil）",
        (7, 2): "0.20mm（8mil）",
        (8, 2): "0.10mm（4mil）",
        (10, 2): "1.0mm（40mil）",
        (11, 2): "0.15mm（6mil）",
        (12, 2): "0.15mm（6mil）",
        (13, 2): "±0.20mm（±8mil）",
    }
    for (r, c), val in dual_updates.items():
        set_cell_text(t9.rows[r].cells[c], val)
    # 外径比内径说明
    set_cell_text(t9.rows[5].cells[3], "外径必须比内径大 0.10mm（4mil）以上")
    set_cell_text(t9.rows[6].cells[3], "极限值，建议 ≥ 0.25mm（10mil）")
    set_cell_text(t9.rows[8].cells[3], "绿油（黑色/白色需 0.15mm（6mil））")

    # 全局轻量清理：残留“模块T”“ViewMate”等（段落内仍可能漏网）
    for p in doc.paragraphs:
        t = p.text
        if not t:
            continue
        nt = t
        nt = nt.replace("模块T", "模块A")  # 兜底；精确段已改
        # 避免把已正确的“模块A：平台”二次破坏——仅对明显旧串
        if "ViewMate" in nt:
            nt = nt.replace("或 ViewMate", "").replace("ViewMate 或 ", "").replace("ViewMate", "Gerbv")
        if nt != t:
            # 若已被精确替换过则跳过含正确模块A说明的
            if p.text.startswith("模块A：平台") or p.text.startswith("模块A为理论"):
                continue
            replace_paragraph_text(p, nt)

    doc.save(str(dst))
    print(f"[full] saved {dst}")
    return doc


# ---------------------------------------------------------------------------
# 模块B分册（原模块A EDA）
# ---------------------------------------------------------------------------

def transform_module_b_booklet(src: Path, dst: Path) -> None:
    doc = Document(str(src))

    exact = {
        7: "模块B：EDA工程设计（续设计）",
        17: (
            "⚠ 注意：本模块所有成果文件保存到 \"D:\\提交资料\\模块B\\\" 文件夹下，"
            "比赛结束前统一拷贝到U盘根目录提交（U盘命名示例：赛位号_模块B）。"
            "请严格按照命名规范保存文件。"
        ),
        19: (
            "某物联网公司开发一款LoRa（低功耗广域网）核心板，用于远程数据采集与传输。"
            "核心板基于STM32G070CBT6主控芯片，集成SX1268 LoRa模块、AHT30温湿度传感器、USB转串口等功能。"
            "硬件工程师已完成核心电路的原理图设计，但部分子电路尚未完成，且有一颗定制连接器需要自建封装。"
        ),
        20: (
            "作为PCB Layout工程师，您需要完成以下工作：自建定制连接器封装 → 补全原理图 → 设置设计规则 "
            "→ PCB布局 → PCB布线 → 输出生产Gerber文件，确保电路板满足可制造性要求和电气性能要求。"
        ),
        25: "▍子任务 B-0  自建连接器封装",
        27: "1. 器件符号设计",
        31: "2. PCB封装设计",
        38: "▍子任务 B-1  原理图补全",
        39: (
            "打开提供的立创EDA工程文件，根据以下子电路功能描述，补全对应子电路（原理图中已红色高亮）。"
            "元件型号与封装以赛场下发半成品工程文件及库内推荐封装为准，本样题不单独下发BOM清单与封装对照表。"
        ),
        43: (
            "（3）复位与BOOT0电路：NRST引脚通过10kΩ上拉电阻接3V3，并可并联约100nF电容至GND构成RC复位；"
            "复位按键SW-RST一端接NRST，另一端接GND，按下时拉低复位（MCU的NRST为低电平复位有效）。"
            "BOOT0引脚通过10kΩ下拉电阻接GND（正常启动模式）；若半成品工程已预置BOOT0网络，请按工程网络连接补全。"
        ),
        44: (
            "（4）LED指示电路：共3个LED，位号与二极管区分。"
            "电源指示灯LED-PWR：阳极经限流电阻（建议470Ω）接3V3，阴极接GND；"
            "用户LED1、LED2：阳极分别接MCU的PA11、PA12，阴极经限流电阻（建议1kΩ）接GND，MCU输出低电平时点亮。"
            "电源保护中的肖特基二极管为D1、TVS为D2，不得与LED位号混用。"
        ),
        45: (
            "（5）按键电路：4个独立按键（SW1~SW4），一端分别接MCU的PD0、PD1、PD2、PD3引脚，另一端接GND。"
            "MCU引脚使用内部上拉，按键按下时引脚为低电平。按键无需外部上拉电阻。"
        ),
        47: (
            "（7）NE555多谐振荡器：采用NE555构成无稳态振荡电路，供电为+5V与GND；"
            "定时网络由电阻、电位器与电容构成，通过电位器调节输出方波频率；"
            "OUT接MCU的PA8（TIM1_CH1）。复位脚接+5V，控制脚可按常规旁路。"
            "请补全至可独立振荡并输出至PA8。"
        ),
        48: "💡 提示：优先使用立创EDA库中元件；除PogoPin定制连接器外，不得自行创建其他封装。",
        49: "▍子任务 B-2  设计规则设置",
        51: (
            "（2）设置线宽规则：信号线 0.15mm（6mil）；"
            "电源线分级：3V3 ≥ 0.8mm（32mil），5V ≥ 1.0mm（40mil）；"
        ),
        52: "（3）设置过孔规则：内径 0.3mm（12mil），外径 0.6mm（24mil）；",
        53: "（4）设置安全间距规则：线线间距、线焊盘间距、孔线间距均为 0.15mm（6mil）；",
        54: "（5）设置阻焊规则：焊盘1:1开窗，阻焊桥最小宽度 0.1mm（4mil）。",
        58: (
            "（1）板框尺寸：以赛场下发半成品工程文件中的板框为准（含外形与倒圆角），"
            "不得自行改动板框；在给定板框区域内完成布局。"
        ),
        60: "（3）晶振电路靠近MCU晶振引脚；",
        61: "（4）LoRa模块放置在板上方边缘，天线端靠近板边缘；",
        62: "（5）元件布局按功能模块分区（电源区、主控区、接口区、传感器区）；",
        63: "（6）调整位号丝印位置，确保清晰可读，不与焊盘重叠。",
        66: "▍子任务 B-4  PCB布线",
        67: (
            "（1）电源线加宽：3V3线宽 ≥ 0.8mm（32mil），5V线宽 ≥ 1.0mm（40mil）；"
        ),
        69: "（3）信号线：普通信号线宽 0.15mm（6mil），关键信号（晶振、复位）尽量短；",
        75: "▍子任务 B-5  DRC检查与Gerber导出",
        81: (
            "⚠ 注意：所有文件保存到 \"D:\\提交资料\\模块B\\\" 目录下，比赛结束前统一拷贝到U盘根目录提交。"
            "文件名中 {工位号} 请替换为您的实际赛位号。"
        ),
        83: "1. 请合理分配各子任务时间；时长与评分以技术工作文件/竞赛平台为准；",
        88: "6. 如遇软件故障或其他问题，请举手示意裁判。",
    }
    for idx, text in exact.items():
        if idx < len(doc.paragraphs):
            replace_paragraph_text(doc.paragraphs[idx], text)

    # 封面信息表：去掉分值/精确时长/组别
    t0 = doc.tables[0]
    set_cell_text(t0.rows[2].cells[0], "竞赛工具")
    set_cell_text(t0.rows[2].cells[1], "立创EDA专业版")
    set_cell_text(t0.rows[3].cells[0], "本地提交目录")
    set_cell_text(t0.rows[3].cells[1], "D:\\提交资料\\模块B\\")
    set_cell_text(t0.rows[4].cells[0], "U盘命名示例")
    set_cell_text(t0.rows[4].cells[1], "赛位号_模块B")

    t1 = doc.tables[1]
    set_cell_text(t1.rows[1].cells[1], "EDA工程设计（续设计）")
    set_cell_text(t1.rows[2].cells[1], "PCB设计能力：自建封装、原理图补全、布局布线、DRC检查、Gerber输出")
    set_cell_text(t1.rows[3].cells[0], "时间分配")
    set_cell_text(t1.rows[3].cells[1], "请合理分配各子任务时间；以技术工作文件/竞赛平台为准")
    set_cell_text(t1.rows[4].cells[0], "评分")
    set_cell_text(t1.rows[4].cells[1], "以技术工作文件及评分表为准（样题不列分值）")
    set_cell_text(t1.rows[5].cells[1], "epro工程文件 + Gerber压缩包 + 2D预览图等")

    # 材料：去掉BOM/封装表
    t2 = doc.tables[2]
    # 原 7 行含 BOM、封装；改为与完整版一致 4 项材料 + 软件
    # rows: 0 header, 1-6 data — 重写 1-6
    materials = [
        ("1", "立创EDA工程文件", "半成品原理图（核心部分已完成，含MCU、LoRa模块）"),
        ("2", "子电路功能说明", "待补全子电路的功能描述和接口定义（见下文；型号封装以工程/库为准）"),
        ("3", "板厂工艺能力参数表", "设计规则参考依据（含mm/mil双单位）"),
        ("4", "立创EDA专业版软件", "赛场已预装"),
        ("—", "—", "不单独下发BOM清单、封装对照表"),
        ("—", "—", "—"),
    ]
    for i, (a, b, c) in enumerate(materials, start=1):
        if i < len(t2.rows):
            set_cell_text(t2.rows[i].cells[0], a)
            set_cell_text(t2.rows[i].cells[1], b)
            set_cell_text(t2.rows[i].cells[2], c)

    t3 = doc.tables[3]
    set_cell_text(t3.rows[2].cells[1], "1.6mm（63mil）")
    set_cell_text(t3.rows[4].cells[1], "信号线 0.15mm（6mil）")
    set_cell_text(t3.rows[4].cells[2], "电源线：3V3 ≥ 0.8mm（32mil）；5V ≥ 1.0mm（40mil）")
    set_cell_text(t3.rows[5].cells[1], "0.15mm（6mil）")
    set_cell_text(t3.rows[6].cells[1], "内径 0.3mm（12mil），外径 0.6mm（24mil）")
    set_cell_text(t3.rows[7].cells[1], "字符高度 ≥ 1.0mm（40mil），字宽 ≥ 0.15mm（6mil）")

    t4 = doc.tables[4]
    set_cell_text(t4.rows[3].cells[2], "上拉+可选RC + BOOT0下拉 + 复位按键")
    set_cell_text(t4.rows[4].cells[2], "LED1/LED2 + LED-PWR（勿与D1/D2冲突）")
    set_cell_text(t4.rows[7].cells[2], "无稳态 + 电位器调频，OUT→PA8，+5V供电")

    t5 = doc.tables[5]
    set_cell_text(t5.rows[2].cells[2], "B0_PogoPin_工位{工位号}.efoo")
    set_cell_text(t5.rows[5].cells[1], "（不要求）不单独提交BOM清单")
    set_cell_text(t5.rows[5].cells[2], "—")
    set_cell_text(t5.rows[5].cells[3], "—")

    doc.save(str(dst))
    print(f"[booklet-B] {dst.name}")


# ---------------------------------------------------------------------------
# 模块C分册（原模块B CAM）
# ---------------------------------------------------------------------------

def transform_module_c_booklet(src: Path, dst: Path) -> None:
    doc = Document(str(src))
    exact = {
        7: "模块C：CAM审核与工艺文件编制",
        17: (
            "⚠ 注意：本模块所有成果文件保存到 \"D:\\提交资料\\模块C\\\" 文件夹下，"
            "比赛结束前统一拷贝到U盘根目录提交（U盘命名示例：赛位号_模块C）。"
            "请严格按照命名规范保存文件。"
        ),
        24: "• Gerbv — Gerber文件查看与测量工具（赛场已预装）",
        25: "💡 提示：本模块仅可使用 Gerbv 查看Gerber文件，禁止使用其他未批准EDA软件或辅助工具。",
        30: (
            "使用Gerbv打开提供的Gerber文件包，对照板厂工艺能力参数表，"
            "全面检查PCB各层的设计缺陷，将发现的缺陷记录在缺陷记录表中。"
        ),
        44: "▍子任务 C-2  制程工艺卡编制",
        45: "根据Gerber文件中的信息，结合板厂工艺能力，填写简版制程工艺卡的基本信息。",
        48: "1. 请合理分配缺陷识别和工艺卡填写的时间；时长与评分以技术工作文件/竞赛平台为准；",
        49: "2. 可使用 Gerbv 查看Gerber文件，禁止使用其他未批准工具；",
    }
    for idx, text in exact.items():
        if idx < len(doc.paragraphs):
            replace_paragraph_text(doc.paragraphs[idx], text)

    t0 = doc.tables[0]
    set_cell_text(t0.rows[2].cells[0], "竞赛工具")
    set_cell_text(t0.rows[2].cells[1], "Gerbv")
    set_cell_text(t0.rows[3].cells[0], "本地提交目录")
    set_cell_text(t0.rows[3].cells[1], "D:\\提交资料\\模块C\\")
    set_cell_text(t0.rows[4].cells[0], "U盘命名示例")
    set_cell_text(t0.rows[4].cells[1], "赛位号_模块C")

    t1 = doc.tables[1]
    set_cell_text(t1.rows[3].cells[0], "时间分配")
    set_cell_text(t1.rows[3].cells[1], "请合理分配各子任务时间；以技术工作文件/竞赛平台为准")
    set_cell_text(t1.rows[4].cells[0], "评分")
    set_cell_text(t1.rows[4].cells[1], "以技术工作文件及评分表为准（样题不列分值）")
    set_cell_text(t1.rows[5].cells[1], "缺陷记录表 + 简版制程工艺卡")

    # 工艺能力双单位 mm 主
    t3 = doc.tables[3]
    set_cell_text(t3.rows[0].cells[2], "工艺能力（mm / mil）")
    dual = {
        1: "0.10mm（4mil）",
        2: "0.10mm（4mil）",
        4: "0.30mm（12mil）",
        5: "0.25mm（10mil）",
        6: "0.18mm（7mil）",
        7: "0.20mm（8mil）",
        8: "0.10mm（4mil）",
        10: "1.0mm（40mil）",
        11: "0.15mm（6mil）",
        12: "0.15mm（6mil）",
        13: "±0.20mm（±8mil）",
    }
    for r, v in dual.items():
        set_cell_text(t3.rows[r].cells[2], v)
    set_cell_text(t3.rows[5].cells[3], "外径必须比内径大 0.10mm（4mil）以上")
    set_cell_text(t3.rows[6].cells[3], "极限值，建议 ≥ 0.25mm（10mil）")
    set_cell_text(t3.rows[8].cells[3], "绿油（黑色/白色需 0.15mm（6mil））")

    # 工艺卡填写表：去掉“信息来源”列内容（改为仅填写项）
    t5 = doc.tables[5]
    # 原 3 列：序号、填写项、信息来源 → 第二列表头改为填写项，第三列清空或改为“—”
    if len(t5.rows[0].cells) >= 3:
        set_cell_text(t5.rows[0].cells[2], "备注")
        for r in range(1, len(t5.rows)):
            set_cell_text(t5.rows[r].cells[2], "—")

    doc.save(str(dst))
    print(f"[booklet-C] {dst.name}")


# ---------------------------------------------------------------------------
# 缺陷记录表 / 工艺卡 / 模块D表 — 最小对齐
# ---------------------------------------------------------------------------

def transform_defect_forms() -> None:
    # 空白表
    blank_src = SAMPLE / "模块B样题_缺陷记录表.docx"
    blank_dst = SAMPLE / "模块C样题_缺陷记录表.docx"
    if blank_src.exists():
        doc = Document(str(blank_src))
        for p in doc.paragraphs:
            if "缺陷记录表" in p.text and "参考" not in p.text:
                replace_paragraph_text(p, "缺陷记录表（模块C）")
                break
        # 备注加模块
        for p in doc.paragraphs:
            if p.text.startswith("备注") or "请将发现的缺陷" in p.text:
                if "模块C" not in p.text:
                    replace_paragraph_text(
                        p,
                        "备注：请将发现的缺陷按序号填写在上表中，要求写明所在层、缺陷类型、位置描述以及违反的工艺参数条目。"
                        "成果提交命名示例：赛位号_模块C（本地目录 D:\\提交资料\\模块C\\）。",
                    )
        doc.save(str(blank_dst))
        print(f"[defect-blank] {blank_dst.name}")

    ans_src = SAMPLE / "模块B样题_缺陷记录表_参考答案.docx"
    ans_dst = SAMPLE / "模块C样题_缺陷记录表_参考答案.docx"
    if ans_src.exists():
        doc = Document(str(ans_src))
        for p in doc.paragraphs:
            t = p.text
            if "缺陷记录表" in t and "参考答案" in t:
                replace_paragraph_text(p, "缺陷记录表（参考答案·模块C）")
            # 去掉评分分值句或弱化
            if "评分说明" in t or "每正确识别" in t:
                replace_paragraph_text(
                    p,
                    "说明：以下为参考答案示例，供命题/裁判使用；正式评分细则以技术工作文件及评分表为准。",
                )
        doc.save(str(ans_dst))
        print(f"[defect-ans] {ans_dst.name}")


def transform_process_cards() -> None:
    # 模板已是简版基本信息，仅轻量加模块C说明
    tpl = SAMPLE / "简版制程工艺卡模板（仅基本信息）.docx"
    if tpl.exists():
        doc = Document(str(tpl))
        for p in doc.paragraphs:
            if "选手填写用" in p.text:
                replace_paragraph_text(p, "（模块C·选手填写用·简版基本信息）")
            if p.text.startswith("注："):
                replace_paragraph_text(
                    p,
                    "注：请根据提供的Gerber文件及GB/T 4588-2025标准，填写以上基本信息。"
                    "提交命名示例：赛位号_模块C（与缺陷记录表一并提交）。",
                )
        doc.save(str(tpl))
        print(f"[process-tpl] {tpl.name}")

    ans = SAMPLE / "简版制程工艺卡（参考答案版）.docx"
    if ans.exists():
        doc = Document(str(ans))
        for p in doc.paragraphs:
            t = p.text
            if "参考答案版" in t and "制程" in "".join(x.text for x in doc.paragraphs[:3]):
                pass
            if p.text.strip() == "（参考答案版）":
                replace_paragraph_text(p, "（参考答案版·模块C简版基本信息，供命题/裁判使用）")
            # 弱化分值与“模块B总分”
            if "完整性6分" in t or "参数正确性4分" in t:
                replace_paragraph_text(
                    p,
                    "（以下为参考填写示例；正式评分以技术工作文件及评分表为准）",
                )
            if "评分说明" in t:
                replace_paragraph_text(p, "四、使用说明")
            if "工艺卡部分共10分" in t or "占模块B" in t:
                replace_paragraph_text(
                    p,
                    "注：本参考答案供裁判对照；样题链仅要求选手填写简版基本信息，完整工序展开不作为选手必填项。",
                )
        # 表格内评分表弱化
        if len(doc.tables) >= 4:
            t3 = doc.tables[3]
            for row in t3.rows:
                for cell in row.cells:
                    if "模块B" in cell.text:
                        set_cell_text(
                            cell,
                            cell.text.replace("模块B", "模块C").replace("占模块C总分的50%", "详见技术工作文件评分表"),
                        )
                    if "配分" in cell.text and row == t3.rows[0]:
                        pass
        # 验收标准去掉 Class 2 组别暗示可保留国标名
        if doc.tables:
            t0 = doc.tables[0]
            for row in t0.rows:
                for cell in row.cells:
                    if "Class 2" in cell.text or "IPC-A-600 Class" in cell.text:
                        set_cell_text(
                            cell,
                            cell.text.replace(" / IPC-A-600 Class 2", "").replace("IPC-A-600 Class 2", "IPC-A-600"),
                        )
        doc.save(str(ans))
        print(f"[process-ans] {ans.name}")


def transform_module_d_form() -> None:
    src = SAMPLE / "模块C样题_检测记录表.docx"
    dst = SAMPLE / "模块D样题_检测记录表.docx"
    if not src.exists() and dst.exists():
        src = dst
    if not src.exists():
        print("[module-D] skip, source missing")
        return
    doc = Document(str(src))
    for p in doc.paragraphs:
        t = p.text
        if "模块C" in t:
            replace_paragraph_text(p, t.replace("模块C", "模块D"))
        t2 = p.text
        if "学生组" in t2 or "职工组" in t2 or "2级" in t2 or "3级" in t2:
            nt = t2
            nt = re.sub(r"[；;]?学生组按2级[^）\)]*[）\)]?", "", nt)
            nt = re.sub(r"[；;]?职工组按3级[^）\)]*[）\)]?", "", nt)
            nt = nt.replace("学生组2级/职工组3级", "对应要求")
            nt = nt.replace("（学生组按2级/职工组按3级判定）", "")
            nt = nt.replace("学生组按2级产品标准判定，职工组按3级产品标准判定（同板不同等级阈值）。", "")
            nt = re.sub(r"对应等级标准要求（学生组2级/职工组3级）", "标准要求", nt)
            nt = re.sub(r"（模块D·选手填写；[^）]*）", "（模块D·选手填写）", nt)
            if "合格（符合GB/T 4588-2025" in nt:
                nt = "□ 合格（符合GB/T 4588-2025标准要求）"
            if nt != t2:
                replace_paragraph_text(p, nt)
    # 不深化任务；外形尺寸标准若写死 80×60，改为“以赛场下发检测板/图纸为准”仅在记录表标准列——最小改
    if len(doc.tables) >= 3:
        t2 = doc.tables[2]
        for row in t2.rows:
            if "外形尺寸" in row.cells[0].text:
                set_cell_text(row.cells[1], "以赛场下发检测板/图纸为准")
    doc.save(str(dst))
    print(f"[module-D] {dst.name}")


# ---------------------------------------------------------------------------
# 清理旧文件名（备份后删除旧路径副本，避免目录混淆）
# ---------------------------------------------------------------------------

def cleanup_old_names() -> None:
    old_names = [
        SAMPLE / "模块A样题_EDA工程设计_v4.docx",
        SAMPLE / "模块B样题_CAM审核与工艺文件编制.docx",
        SAMPLE / "模块B样题_缺陷记录表.docx",
        SAMPLE / "模块B样题_缺陷记录表_参考答案.docx",
        SAMPLE / "模块C样题_检测记录表.docx",
    ]
    for p in old_names:
        if p.exists():
            # 若新文件已存在且不同路径，删除旧名
            p.unlink()
            print(f"[cleanup] removed old name {p.name}")


# ---------------------------------------------------------------------------
# 清单与对照表
# ---------------------------------------------------------------------------

def write_task_docs() -> None:
    sync = TASK / "待同步清单.md"
    sync.write_text(
        """# 技术工作文件待同步清单

> 本轮**未修改** `01_技术文件` 与评分表正文。以下为样题完善后，技术工作文件/评分表仍需同步的要点，供后续任务使用。

## 1. 模块字母重映射（全文）

| 新编号 | 内容 | 旧编号（技术文件/评分表可能仍在用） |
|--------|------|--------------------------------------|
| 模块A | 平台测试（理论知识） | 模块T 或未单列 |
| 模块B | EDA工程设计（续设计） | 模块A |
| 模块C | CAM审核与工艺文件编制 | 模块B |
| 模块D | 成品板质量检测与缺陷分析 | 模块C |

待同步位置（预期）：技术工作文件模块划分表、日程/时长表、提交物命名、评分表分模块标题、裁判手册引用。

## 2. 样题边界（技术文件保留、样题不写）

技术文件/评分表可继续保留；样题已删除或不写：

- 各模块分值、满分句
- 各模块精确分钟
- 学生组/职工组差异（含模块D 2级/3级绑定）
- ViewMate 等非 Gerbv 工具
- 软件版本明细（如需可仅在技术文件）

## 3. 工具与材料

- 模块C（原CAM）：工具统一为 **仅 Gerbv**
- 模块B（原EDA）：**不单独下发** BOM 清单、封装对照表；型号/封装以半成品工程与库为准
- 工艺卡：选手链为**简版基本信息**；完整工序卡不进赛题链（技术文件若仍写“完整工艺卡”需对齐）

## 4. 设计/工艺参数口径

- 双单位统一格式：`mm（mil）`，如 `0.15mm（6mil）`
- 板框：**不写死**冲突尺寸，统一“以赛场下发半成品工程文件中的板框为准”
- 电源线分级保留：信号线 / 3V3≥0.8mm（32mil） / 5V≥1.0mm（40mil）
- 国标名称：`GB/T 4588-2025`；**不写** 2级/3级与组别绑定

## 5. 位号与可做性（样题已修，技术文件/工程需一致）

- 二极管 D1（肖特基）、D2（TVS）；LED 为 LED1/LED2/LED-PWR，禁止 LED 再标 D1/D2/D3
- 复位：上拉 + 可选 RC + SW-RST；BOOT0：下拉至 GND（正常启动）
- NE555：无稳态关键节点补全至可做（OUT→PA8，+5V）
- 定制件表述：连接器（非“定制芯片”）

## 6. 提交路径与命名

- 本地示例：`D:\\提交资料\\模块B|C|D\\`
- U盘：`赛位号_模块X`
- 模块A（理论）不占本地提交目录

## 7. 产品叙事

- 模块B/C：同源 LoRa 核心板
- 模块D：独立成品板质检任务（一句话声明已进样题）

## 8. 建议后续动作

1. 用本清单批量替换技术工作文件模块字母与提交命名。
2. 评分表模块标题与配分表与新字母对齐（样题不列分值）。
3. 半成品工程/缺陷 Gerber 与样题位号、子电路说明交叉验收。
4. 删除或归档仍含“模块T / 旧模块A=EDA”表述的过期附件。
""",
        encoding="utf-8",
    )

    cmp = TASK / "样题-技术文件-评分表对照表.md"
    cmp.write_text(
        """# 样题 · 技术文件 · 评分表 对照表

| 主题 | 样题完整版（本轮后） | 技术工作文件（待同步） | 评分表（待同步） | 备注 |
|------|----------------------|------------------------|------------------|------|
| 模块A | 平台测试短说明，无例题/分值 | 可能仍写模块T或理论权重/时长 | 理论分模块 | 样题不列权重 |
| 模块B | EDA 续设计，LoRa 叙事 | 可能仍称模块A | EDA 评分项 | 子任务 B-0…B-5 |
| 模块C | CAM+简版工艺卡，仅 Gerbv | 可能仍称模块B，含 ViewMate | 缺陷+工艺卡分值 | 去掉信息来源列 |
| 模块D | 独立质检，最小命名对齐 | 可能仍称模块C，含组别/等级 | 检测评分 | 不深化任务 |
| 分值 | **不写** | 应保留总分与模块分 | 应保留细则 | 边界冻结 |
| 精确时长 | **不写**（可有中性提示） | 可保留日程 | 一般不写 | 总述可保留约4小时框架 |
| 学生/职工组 | **不写**差异 | 若竞赛需要则仅技术文件 | 若分卷则仅评分表 | 样题统一口径 |
| 板框尺寸 | 以半成品工程为准 | 应去掉 80×60 与 2560mil 冲突 | — | 勿写死冲突数 |
| 线宽规则 | 信号 0.15mm（6mil）；3V3/5V 分级 | 与样题双单位对齐 | 可制造性相关扣分项 | mm 主 |
| 工具 CAM | Gerbv only | 删除 ViewMate | 工具符合性 | |
| 材料 EDA | 无独立 BOM/封装表 | 材料清单对齐 | 不因缺 BOM 表扣分 | |
| 位号 | D1/D2 二极管；LED1/2/PWR | 与工程一致 | 原理图完整性评分 | |
| 提交命名 | 赛位号_模块B/C/D | 全文替换 | 提交物检查表 | 路径示例已统一 |
| 国标 | GB/T 4588-2025 名称 | 同 | 判定引用 | 无 2/3 级组别绑定 |
| 工艺卡 | 简版基本信息 | 勿要求完整工序卡进赛题 | 评分范围对齐简版 | 参考答案供裁判 |

## 本轮样题已落地、技术文件未改的“差异点”摘要

1. 字母：样题 A理论+B/C/D实操；技术文件若仍 T+A/B/C 则不一致。  
2. 工具：样题 CAM 仅 Gerbv；技术文件若仍 Gerbv/ViewMate 则不一致。  
3. 材料：样题 EDA 无 BOM/封装表；技术文件材料表若仍列则不一致。  
4. 组别与等级：样题已删；技术文件/评分表若保留需明确“仅技术文件有效”。  
5. 板框：样题已去冲突绝对尺寸；技术文件参数表需同样处理。  
""",
        encoding="utf-8",
    )
    print(f"[docs] {sync.name}, {cmp.name}")


# ---------------------------------------------------------------------------
# 校验
# ---------------------------------------------------------------------------

def validate_full(path: Path) -> list[str]:
    doc = Document(str(path))
    text = paragraph_full_text(doc)
    issues = []

    def must_not(pat: str, label: str | None = None):
        if re.search(pat, text):
            issues.append(f"FORBIDDEN: {label or pat}")

    def must(pat: str, label: str | None = None):
        if not re.search(pat, text):
            issues.append(f"MISSING: {label or pat}")

    must_not(r"模块T", "模块T")
    must_not(r"ViewMate")
    must_not(r"学生组")
    must_not(r"职工组")
    must_not(r"80mm\s*[×x]\s*60mm")
    must_not(r"2560\s*mil")
    must_not(r"满分\d+分")
    must_not(r"权重\d+%")
    # 旧主标题残留
    if re.search(r"模块A：EDA", text):
        issues.append("FORBIDDEN: 模块A：EDA（应为模块B）")
    if re.search(r"模块B：CAM", text):
        issues.append("FORBIDDEN: 模块B：CAM（应为模块C）")
    if re.search(r"模块C：成品", text):
        issues.append("FORBIDDEN: 模块C：成品（应为模块D）")

    must(r"模块A：平台测试", "模块A理论")
    must(r"模块B：EDA", "模块B EDA")
    must(r"模块C：CAM", "模块C CAM")
    must(r"模块D：成品", "模块D 检测")
    must(r"Gerbv")
    must(r"0\.15mm（6mil）")
    must(r"以赛场下发半成品工程文件中的板框为准|以赛场下发半成品工程文件中的板框")
    must(r"独立质检")
    must(r"D:\\\\提交资料\\\\模块B|D:\\提交资料\\模块B")
    must(r"LED-PWR|LED1")
    must(r"BOOT0")
    must(r"GB/T 4588-2025")

    # 人工复审保留
    if not HUMAN.exists():
        issues.append("MISSING: 人工复审.docx 被删除")

    return issues


def main() -> None:
    assert HUMAN.exists(), f"missing human review: {HUMAN}"
    backup_files()

    transform_full_exam(HUMAN, OFFICIAL)

    # 配套：从备份或当前旧文件
    b_src = SAMPLE / "模块A样题_EDA工程设计_v4.docx"
    if not b_src.exists():
        b_src = BACKUP_DIR / "模块A样题_EDA工程设计_v4.docx"
    transform_module_b_booklet(b_src, SAMPLE / "模块B样题_EDA工程设计.docx")

    c_src = SAMPLE / "模块B样题_CAM审核与工艺文件编制.docx"
    if not c_src.exists():
        c_src = BACKUP_DIR / "模块B样题_CAM审核与工艺文件编制.docx"
    transform_module_c_booklet(c_src, SAMPLE / "模块C样题_CAM审核与工艺文件编制.docx")

    # 缺陷表需在 cleanup 前从旧名读取
    transform_defect_forms()
    transform_process_cards()
    transform_module_d_form()
    cleanup_old_names()
    write_task_docs()

    issues = validate_full(OFFICIAL)
    print("\n=== VALIDATION ===")
    if issues:
        for i in issues:
            print(" -", i)
    else:
        print(" ALL CHECKS PASSED")
    print("done")


if __name__ == "__main__":
    main()
