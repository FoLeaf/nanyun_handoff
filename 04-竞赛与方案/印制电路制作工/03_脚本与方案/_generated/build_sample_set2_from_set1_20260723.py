# -*- coding: utf-8 -*-
"""
第二套选手材料生成：

- 版式：整份从第一套 docx 复制（字体/页边距/表样式/图片框架不变）
- 内容：模块 B/C 按 BLE 传感器节点命题重写（禁止只做 LoRa→BLE 改名）
- 套次：仅标题「竞赛样题（第二套）」；正文与附件文件名不写套次
"""
from __future__ import annotations

import shutil
from pathlib import Path

from docx import Document

ROOT = Path(__file__).resolve().parents[2]
SET1 = ROOT / "02_样题" / "第一套"
SET2 = ROOT / "02_样题" / "第二套"
# 交付角色目录（与 02_样题/README.md 一致）
S1_EXAM = SET1 / "01_样题"
S1_ANS = SET1 / "03_结果呈现_参考答案"
S1_FORM = SET1 / "04_需填写内容"
S2_EXAM = SET2 / "01_样题"
S2_ANS = SET2 / "03_结果呈现_参考答案"
S2_FORM = SET2 / "04_需填写内容"
for d in (S2_EXAM, S2_ANS, S2_FORM, SET2 / "02_评分标准", SET2 / "05_U盘资料", SET2 / "99_命题规格"):
    d.mkdir(parents=True, exist_ok=True)


def set_paragraph_text(paragraph, text: str) -> None:
    if not paragraph.runs:
        paragraph.add_run(text)
        return
    paragraph.runs[0].text = text
    for r in paragraph.runs[1:]:
        r.text = ""


def set_cell_text(cell, text: str) -> None:
    if not cell.paragraphs:
        cell.text = text
        return
    set_paragraph_text(cell.paragraphs[0], text)
    for p in cell.paragraphs[1:]:
        set_paragraph_text(p, "")


def set_table_row(table, row_idx: int, values: list[str]) -> None:
    row = table.rows[row_idx]
    for ci, val in enumerate(values):
        if ci < len(row.cells):
            set_cell_text(row.cells[ci], val)


# ---------------------------------------------------------------------------
# 模块 B：相对第一套（LoRa+Pogo+AHT30+NE555）的实质差异清单
# - 产品：室内环境监测 BLE 节点（非 LoRa 远距）
# - 自建：PinHeader_2x5 + 明确 10 脚功能定义（非 8P Pogo）
# - 电源：Type-C VBUS 路径 + 不同位号
# - LDO：滤波取值不同
# - 人机：3 键 + 不同 GPIO；LED 位号/阻值/引脚不同
# - 主外设：选手补全 BLE 模组 UART/使能（第一套 LoRa 在半成品里不作为补全项）
# - 传感：BH1750 光照（非 AHT30）
# - 第7块：有源蜂鸣器驱动（非 NE555）
# - 布局：天线净空、分区命名不同
# ---------------------------------------------------------------------------

PARA_BY_INDEX: dict[int, str] = {
    5: "竞赛样题（第二套）",
    # 任务背景（完全重写）
    38: (
        "某智能楼宇方案商开发一款室内环境监测用的 BLE 传感器节点板，用于温湿度/光照采集后经蓝牙上报网关。"
        "节点基于 STM32G070CBT6 主控，板载 UART 接口 BLE 透传模组、BH1750 数字光照传感器、Type-C 5V 供电及 2×5 底板互联排针。"
        "原理图主体（MCU、晶振、调试口）已完成，但电源链、人机接口、BLE 模组互联、传感器与声光提示等子电路尚未补全；"
        "另有一颗 2×5 排针连接器需自建符号与封装后接入绿色高亮区。"
    ),
    39: (
        "作为 PCB Layout 工程师，您需要完成：自建 2×5 排针封装 → 按功能说明补全红色高亮子电路 → 设置设计规则 → "
        "PCB 布局（含射频模组天线朝向与净空）→ 布线与敷铜 → 导出生产 Gerber 与 2D 预览，满足可制造性与基本 EMC 布局要求。"
    ),
    # B-0
    45: "B-0  自建底板排针封装",
    46: (
        "本设计需要一颗 2×5 直插排针作为与底板的互联座，库中无完全匹配的器件定义，须自行建立符号与封装，并命名为 PinHeader_2x5。"
    ),
    47: "1.器件符号设计",
    48: "（1）新建器件，器件名：PinHeader_2x5；封装名建议：PinHeader_2x5_2.54mm；",
    49: (
        "（2）引脚定义（符号左右排布，上到下）：左列 1~5 为 5V、3V3、GND、GND、UART_TX；"
        "右列 6~10 为 UART_RX、I2C_SDA、I2C_SCL、nRESET、IO_INT。"
        "电气类型按电源/双向/输出合理设置；第 1 脚在符号上加标识。"
    ),
    50: "2. PCB封装设计",
    51: (
        "（1）2.54mm 间距双排直插焊盘，两排中心距 2.54mm；焊盘建议椭圆或圆焊盘，孔径约 1.0mm，焊盘外径约 1.7mm（以可焊通孔为准）；"
        "丝印绘制排针外框，第 1 脚角加圆点；不得使用弹簧针/异形连接器封装顶替。"
    ),
    52: "（2）丝印清晰，不进入相邻焊盘开窗；装配面与半成品工程一致；",
    53: "（3）封装完成后导入本工程库；",
    54: (
        "（4）将器件放到原理图绿色高亮的「底板互联」区，按上表网络名与半成品已有电源/UART/I2C 网络正确连接"
        "（网络名以半成品工程为准，若工程已预置同义网络请对接预置名）。"
    ),
    # B-1 引导语
    55: "B-1  原理图补全",
    56: (
        "打开赛场下发的立创 EDA 半成品工程，仅补全红色高亮子电路。元件型号与封装以半成品及库内推荐为准，"
        "本卷不另发完整 BOM。下列说明中的阻容为建议值，若工程标注冲突以工程标注为准。"
    ),
    57: "子电路功能说明（共 7 块）",
    58: (
        "（1）Type-C 口 5V 输入保护：VBUS 经自恢复保险丝 F2（建议 0.5A 档）→ 单向 TVS D4（约 5V 钳位）→ "
        "肖特基 D3（防反接）后得到节点 +5V，再送入 LDO。不得与 LED 位号冲突；保护器件位号使用 F2/D3/D4。"
    ),
    59: (
        "（2）LDO 稳压：采用 AMS1117-3.3（或半成品指定的同档 3.3V LDO），+5V→3V3。"
        "输入端并 10μF 陶瓷到 GND，输出端并 10μF 陶瓷到 GND（按本卷取值执行）；"
        "LDO 散热焊盘/GND 按封装要求连接。"
    ),
    60: (
        "（3）复位与 BOOT0：NRST 经 4.7kΩ 上拉到 3V3，并并联 100nF 到 GND；轻触开关 SW_RST 一端 NRST、一端 GND。"
        "BOOT0 经 10kΩ 下拉到 GND（正常从 Flash 启动）；可选在 BOOT0 与 3V3 间预留 0Ω/跳线焊盘供下载模式，"
        "若半成品已画跳线框则按工程连接，勿重复矛盾网络。"
    ),
    61: (
        "（4）LED 指示：电源指示灯 LED_PWR——阳极经 1kΩ 接 3V3，阴极接 GND（常亮表示 3V3 有电）。"
        "运行灯 LED_RUN、故障灯 LED_ERR：阳极分别接 MCU 的 PB0、PB1，阴极经 330Ω 到 GND，高电平点亮。"
        "禁止占用 D3/D4 位号。"
    ),
    62: (
        "（5）按键：3 只独立轻触键 KEY1~KEY3，一端分别接 PA0、PA1、PA2，另一端接 GND；"
        "使用 MCU 内部上拉，按下为低电平。本卷用户键为 3 只。"
    ),
    63: (
        "（6）BLE 透传模组互联：模组供电 3V3/GND；模组 UART_TX 接 MCU PA10（USART1_RX），"
        "模组 UART_RX 接 MCU PA9（USART1_TX）（交叉连接）；模组 EN/REG_ON 经 10kΩ 上拉到 3V3，"
        "并可由 MCU PA8 控制（若半成品已指定控制脚则以工程为准）；模组 STATE/连接状态脚接 MCU PA15（输入上拉）。"
        "射频天线端朝板外，原理图只完成电气连接，天线净空在布局阶段落实。"
    ),
    64: (
        "（7）BH1750 数字光照传感器（I2C）：VCC=3V3，GND=GND；SDA→PB7（I2C1_SDA），SCL→PB6（I2C1_SCL）；"
        "SDA/SCL 各 4.7kΩ 上拉到 3V3；ADDR 脚接 GND（地址按器件手册默认）。"
        "本卷第 7 块考核数字光传感链路（非时基振荡电路）。"
    ),
    # B-2 可微调表述（规则数值 E1 对齐，但写法独立成段）
    65: "B-2  设计规则设置",
    66: "（1）按本卷第（三）节工艺表设置 DRC，单位可在 mil/mm 间切换，设置完成后自查单位一致；",
    67: "（2）信号线默认 0.15mm；3V3 电源网络 ≥0.8mm；+5V 网络 ≥1.0mm；对 BLE 模组 3V3 供电走线按电源规则加宽；",
    68: "（3）过孔：内径 0.3mm、外径 0.6mm；模组地焊盘允许按封装推荐过孔阵列就近接地；",
    69: "（4）安全间距：线-线、线-盘、孔-线均为 0.15mm；",
    70: "（5）阻焊：焊盘 1:1 开窗；阻焊桥最小 0.1mm。",
    # B-3 布局
    71: "B-3  PCB布局",
    72: "将原理图更新到 PCB，在半成品给定板框内完成布局（板框尺寸与倒角不得修改）。",
    73: "布局要求",
    74: "（1）板框以半成品为准；安装孔/定位孔若已有则保持；",
    75: (
        "（2）Type-C 与输入保护、LDO 集中在「电源区」，靠近 Type-C 与 PinHeader_2x5 的 5V/3V3 引出侧，"
        "输入输出路径短、流向清晰；"
    ),
    76: "（3）晶振及负载电容紧贴 MCU 晶振脚，周围避免高速/大电流走线；",
    77: (
        "（4）BLE 模组贴短边放置，天线净空区朝板外；天线下方及推荐净空范围内禁止敷铜、走线与过孔"
        "（按模组手册常规净空，题面不要求精确仿真）；"
    ),
    78: (
        "（5）功能分区建议：电源区 | 主控区 | 射频模组区 | 传感区（BH1750 远离天线）| 底板互联区（排针靠边便于插拔）；"
    ),
    79: "（6）丝印位号完整、方向统一，不压焊盘与过孔；关键插座 1 脚标识可见。",
    80: "参考图：",
    # B-4 布线增加 BLE 相关
    81: "B-4 PCB布线",
    82: "（1）电源：3V3 ≥0.8mm，+5V ≥1.0mm；模组供电优先短而宽；信号默认 0.15mm；",
    83: "（2）地：保证 GND 连通，底层以地平面为主；模组地焊盘多过孔到地平面；",
    84: "（3）UART 交叉线尽量短并平行控距；晶振、复位线远离模组天线区；",
    85: "（4）晶振下方不走其它信号，周围敷地隔离；",
    86: "（5）优先 45° 走线，避免 90°；",
    87: "（6）过孔够用即可，避免在天线净空区打孔；",
    88: "（7）顶/底敷 GND 铜，网格或实心均可，但须遵守天线净空禁敷铜；",
    89: "（8）网络连通率 100%，DRC 无未连接网络。",
    # B-5 / B-6
    91: "B-5  DRC检查与Gerber导出",
    92: "（1）布线完成后运行 DRC，错误为 0（警告可忽略）；",
    93: "（2）导出顶/底层、顶/底阻焊、顶/底丝印、板框、钻孔等生产层；",
    94: "（3）Gerber 打包 zip，命名见提交要求；",
    95: "（4）导出顶层 2D 预览 PNG。",
    96: "B-6 注意事项",
    97: (
        "⚠ 注意：文件保存到 D:\\提交资料\\模块B\\ ，赛终拷贝到 U 盘。"
        "文件名中工位号/赛位号替换为实际赛位号；U 盘包示例：赛位号_模块B。"
    ),
    98: "1. 合理分配本模块时间；时长与评分以技术工作文件/竞赛平台为准；",
    99: "2. 及时保存，防止意外丢失；",
    100: "3. 命名错误影响评分；",
    101: "4. 禁止拷贝他人文件或使用外部存储，违者按作弊处理；",
    102: "5. 除 PinHeader_2x5 外，优先使用库内封装，不得擅自乱建封装；",
    103: "6. 软件或设备异常请举手示意裁判。",
    # 模块 C 背景加一句与 B 弱同源但不相同文件
    110: (
        "您是 PCB 板厂 CAM 工程师。客户提交了一套「室内环境监测 BLE 传感器节点」的投产 Gerber"
        "（客户自述由 BLE 节点设计导出，与赛场 EDA 半成品非同一文件），准备开料。"
        "请先完成 DFM 审核，识别并记录缺陷，再填写简版制程工艺卡。"
    ),
}


def apply_para_overrides(doc: Document) -> None:
    for idx, text in PARA_BY_INDEX.items():
        if idx < len(doc.paragraphs):
            set_paragraph_text(doc.paragraphs[idx], text)


def apply_table_overrides(doc: Document) -> None:
    # T4 提供材料 — BLE
    if len(doc.tables) > 4:
        t = doc.tables[4]
        set_table_row(t, 1, ["1", "立创EDA工程文件", "半成品原理图（含 MCU、晶振；BLE 模组位已预留未接完）"])
        set_table_row(t, 2, ["2", "子电路功能说明", "本卷第 B-1 节 7 块红色高亮电路说明"])
        set_table_row(t, 3, ["3", "板厂工艺能力参数表", "设计规则参考（mil/mm）"])
        set_table_row(t, 4, ["4", "立创EDA专业版软件", "赛场已预装"])

    # T6 子电路表 — 与 B-1 对齐
    if len(doc.tables) > 6:
        t = doc.tables[6]
        set_table_row(t, 1, ["1", "Type-C 5V 输入保护", "F2 自恢复保险 + D4 TVS + D3 防反肖特基 → +5V"])
        set_table_row(t, 2, ["2", "LDO 3.3V", "AMS1117-3.3（或同档），输入/输出 10μF"])
        set_table_row(t, 3, ["3", "复位与 BOOT0", "4.7k 上拉+100nF；SW_RST；BOOT0 10k 下拉"])
        set_table_row(t, 4, ["4", "LED 指示", "LED_PWR；LED_RUN/ERR→PB0/PB1 高电平点亮"])
        set_table_row(t, 5, ["5", "按键", "KEY1~KEY3→PA0/PA1/PA2，内部上拉"])
        set_table_row(t, 6, ["6", "BLE 模组互联", "UART 交叉至 PA9/PA10；EN/STATE 控制与状态"])
        set_table_row(t, 7, ["7", "BH1750 光照", "I2C→PB6/PB7，4.7k 上拉，ADDR=GND"])

    # T7 成果物 — 封装文件名
    if len(doc.tables) > 7:
        t = doc.tables[7]
        set_table_row(t, 2, ["2", "自建封装库", "B0_PinHeader_2x5_工位{工位号}.elibz2", "Elibz2"])

    # T8 模块C 材料
    if len(doc.tables) > 8:
        t = doc.tables[8]
        set_table_row(t, 1, ["1", "Gerber文件包", "BLE传感器节点客户投产 Gerber"])
        set_table_row(t, 2, ["2", "板厂工艺能力参数表", "缺陷判定依据"])
        set_table_row(t, 3, ["3", "缺陷记录表", "记录所在层/类型/位置/违反条款"])
        set_table_row(t, 4, ["4", "制程工艺卡", "能量参数与文件完整性"])
        set_table_row(t, 5, ["5", "GB/T 4588-2025标准摘要", "参考"])


def scrub_forbidden_tokens(doc: Document) -> list[str]:
    """正文不得残留第一套专有考点名（允许标题含第二套）。"""
    forbidden = [
        "PogoPin",
        "LoRa",
        "SX1268",
        "AHT30",
        "NE555",
        "PA11",
        "PA12",
        "PD0",
        "SW1~SW4",
        "SW1~SW4",
    ]
    # 更精确的残留检查列表
    bad_hits: list[str] = []
    texts = []
    for p in doc.paragraphs:
        texts.append(p.text)
    for table in doc.tables:
        for row in table.rows:
            for cell in row.cells:
                texts.append(cell.text)
    blob = "\n".join(texts)
    for token in [
        "PogoPin",
        "LoRa",
        "SX1268",
        "AHT30",
        "NE555",
        "多谐振荡",
        "Pogo",
    ]:
        if token in blob:
            # 标题允许「第二套」
            if token == "第二套":
                continue
            bad_hits.append(token)
    # 第二套仅允许出现在标题
    count_set2 = blob.count("第二套")
    if count_set2 != 1:
        bad_hits.append(f"第二套出现{count_set2}次(期望1)")
    return bad_hits


def build_full_exam() -> Path:
    src = S1_EXAM / "印制电路制作工赛项_竞赛样题（完整版）.docx"
    dst = S2_EXAM / "印制电路制作工赛项_竞赛样题（完整版）.docx"
    shutil.copy2(src, dst)
    doc = Document(str(dst))
    apply_para_overrides(doc)
    apply_table_overrides(doc)
    bad = scrub_forbidden_tokens(doc)
    doc.save(str(dst))
    if bad:
        print("WARN residual tokens:", bad)
    else:
        print("CHECK: first-set-only tokens cleared; title set-number ok")
    return dst


def copy_player_forms() -> list[Path]:
    names = [
        "模块C_缺陷记录表.docx",
        "模块C_PCB制程工艺卡.docx",
        "学生组_模块D_检测记录表.docx",
    ]
    out = []
    for name in names:
        src, dst = S1_FORM / name, S2_FORM / name
        shutil.copy2(src, dst)
        out.append(dst)
    return out


def build_c_answer() -> Path:
    src = S1_ANS / "模块C_缺陷记录表-参考答案.docx"
    dst = S2_ANS / "模块C_缺陷记录表-参考答案.docx"
    shutil.copy2(src, dst)
    doc = Document(str(dst))
    # BLE 语境 12 处，位置与第一套措辞区分
    answers_5col = [
        ["1", "顶层线路层", "最小线宽不足", "BLE模组焊盘扇出第3根UART信号线局部线宽约0.08mm", "违反：最小线宽0.10mm"],
        ["2", "顶层线路层", "最小线距不足", "BH1750器件旁两根I2C线间距约0.08mm", "违反：最小线距0.10mm"],
        ["3", "底层线路层", "走线开路/缺口", "3V3给BLE模组供电的底层干线在过孔附近缺口", "开路致命"],
        ["4", "顶层线路层", "走线短路", "LED_RUN限流电阻焊盘与相邻GND铜皮桥接", "短路致命"],
        ["5", "钻孔层+焊盘", "焊环不足（孔破盘）", "排针PinHeader区附近过孔偏孔，单侧焊环约0.08mm", "焊环极限0.18mm"],
        ["6", "钻孔层", "过孔焊盘过小", "主控扇出过孔焊盘外径约0.20mm", "最小过孔焊盘0.25mm"],
        ["7", "钻孔层", "孔间距过小", "模组地焊盘两过孔孔边距约0.15mm", "孔边距≥0.20mm"],
        ["8", "顶层阻焊层", "阻焊覆盖焊盘", "KEY2按键焊盘被阻焊覆盖未开窗", "焊盘需开窗"],
        ["9", "顶层阻焊层", "阻焊桥缺失/不足", "Type-C信号焊盘间无法形成有效阻焊桥", "阻焊桥要求"],
        ["10", "顶层丝印层", "丝印上焊盘", "位号U_BLE压在模组焊盘上", "丝印距露铜≥0.15mm"],
        ["11", "顶层丝印层", "字符尺寸过小", "位号R_PU字高约0.7mm", "字高≥1.0mm"],
        ["12", "文件完整性", "缺少底层丝印层文件", "压缩包缺底层丝印层", "文件不完整"],
    ]
    table = doc.tables[0]
    ncols = len(table.columns)
    for i, row_vals in enumerate(answers_5col):
        ri = i + 1
        if ri >= len(table.rows):
            break
        set_table_row(table, ri, row_vals[:ncols])
    doc.save(str(dst))
    return dst


def main() -> None:
    paths = [build_full_exam()]
    paths.extend(copy_player_forms())
    paths.append(build_c_answer())

    # 内容 diff 摘要：与第一套关键句是否仍相同
    d1 = Document(str(S1_EXAM / "印制电路制作工赛项_竞赛样题（完整版）.docx"))
    d2 = Document(str(paths[0]))
    same = 0
    diff = 0
    samples = []
    for i, (p1, p2) in enumerate(zip(d1.paragraphs, d2.paragraphs)):
        if not p1.text.strip() and not p2.text.strip():
            continue
        if p1.text == p2.text:
            same += 1
        else:
            diff += 1
            if len(samples) < 8 and p1.text.strip():
                samples.append((i, p1.text[:40], p2.text[:40]))
    print("PARA same", same, "diff", diff)
    for s in samples:
        print("  diff@", s[0], "|", s[1], "=>", s[2])
    for p in paths:
        print("OK", p.name, p.stat().st_size)


if __name__ == "__main__":
    main()
