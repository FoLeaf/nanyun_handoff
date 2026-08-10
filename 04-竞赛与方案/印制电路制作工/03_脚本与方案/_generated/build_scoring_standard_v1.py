# -*- coding: utf-8 -*-
"""Generate 评分标准（第一套）— 0721口径，仿宋公文白纸黑字，表无底纹。"""
from pathlib import Path

from docx import Document
from docx.enum.text import WD_ALIGN_PARAGRAPH, WD_LINE_SPACING
from docx.oxml.ns import qn
from docx.shared import Cm, Pt

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / "02_样题" / "第一套" / "02_评分标准" / "印制电路制作工赛项_评分标准（第一套·0721口径）.docx"

FONT = "仿宋"
# 公文常用：正文约 16 磅（三号），表内 12 磅
SIZE_TITLE = 22  # 二号略小用 22 半磅? 实际 Pt(22)=11pt... 用 Pt
# python-docx Pt(16) = 16 pound
SZ_DOC_TITLE = 18
SZ_H1 = 16
SZ_BODY = 14
SZ_TABLE = 12


def set_run_font(run, size=SZ_BODY, bold=False):
    run.font.name = FONT
    run._element.rPr.rFonts.set(qn("w:eastAsia"), FONT)
    run.font.name = FONT
    run.font.size = Pt(size)
    run.bold = bold
    run.font.color.rgb = None  # default black


def set_paragraph_format(p, *, center=False, first_line=False, space_after=6, space_before=0):
    p.paragraph_format.space_before = Pt(space_before)
    p.paragraph_format.space_after = Pt(space_after)
    p.paragraph_format.line_spacing_rule = WD_LINE_SPACING.ONE_POINT_FIVE
    if center:
        p.alignment = WD_ALIGN_PARAGRAPH.CENTER
    else:
        p.alignment = WD_ALIGN_PARAGRAPH.JUSTIFY if not first_line else WD_ALIGN_PARAGRAPH.LEFT
    if first_line:
        p.paragraph_format.first_line_indent = Cm(0.74)  # 约2字符


def add_text(doc, text, *, size=SZ_BODY, bold=False, center=False, first_line=False, space_after=6, space_before=0):
    p = doc.add_paragraph()
    set_paragraph_format(p, center=center, first_line=first_line, space_after=space_after, space_before=space_before)
    run = p.add_run(text)
    set_run_font(run, size=size, bold=bold)
    return p


def add_h1(doc, text):
    return add_text(doc, text, size=SZ_H1, bold=True, space_before=12, space_after=8)


def add_h2(doc, text):
    return add_text(doc, text, size=SZ_BODY, bold=True, space_before=8, space_after=6)


def set_cell_text(cell, text, *, bold=False, size=SZ_TABLE, center=False):
    cell.text = ""
    p = cell.paragraphs[0]
    p.alignment = WD_ALIGN_PARAGRAPH.CENTER if center else WD_ALIGN_PARAGRAPH.LEFT
    p.paragraph_format.space_before = Pt(2)
    p.paragraph_format.space_after = Pt(2)
    run = p.add_run(text)
    set_run_font(run, size=size, bold=bold)
    # ensure no shading left
    tc = cell._tc
    tcPr = tc.get_or_add_tcPr()
    for old in tcPr.findall(qn("w:shd")):
        tcPr.remove(old)


def add_table(doc, headers, rows):
    table = doc.add_table(rows=1 + len(rows), cols=len(headers))
    table.style = "Table Grid"
    for i, h in enumerate(headers):
        set_cell_text(table.rows[0].cells[i], h, bold=True, center=True)
    for r_i, row in enumerate(rows):
        for c_i, val in enumerate(row):
            set_cell_text(table.rows[r_i + 1].cells[c_i], str(val), center=(c_i > 0))
    # blank line after
    doc.add_paragraph()
    return table


def build():
    doc = Document()
    sec = doc.sections[0]
    sec.page_width = Cm(21.0)
    sec.page_height = Cm(29.7)
    sec.top_margin = Cm(2.54)
    sec.bottom_margin = Cm(2.54)
    sec.left_margin = Cm(2.8)
    sec.right_margin = Cm(2.6)

    # 默认样式
    style = doc.styles["Normal"]
    style.font.name = FONT
    style.font.size = Pt(SZ_BODY)
    style._element.rPr.rFonts.set(qn("w:eastAsia"), FONT)

    add_text(doc, '江西省"振兴杯"职业技能大赛', size=SZ_DOC_TITLE, bold=True, center=True, space_after=4)
    add_text(doc, '"印制电路制作工"赛项', size=SZ_DOC_TITLE, bold=True, center=True, space_after=4)
    add_text(doc, "评分标准", size=22, bold=True, center=True, space_after=4)
    add_text(doc, "（第一套 · 与技术工作文件0721定稿口径一致）", size=SZ_BODY, center=True, space_after=12)

    add_text(
        doc,
        "说明：本标准与《印制电路制作工竞赛技术工作文件（0721定稿）》及第一套竞赛样题配套使用；"
        "样题培训与正式竞赛共用同一评分骨架。正式赛仅更换题面参数、缺陷位置及标准答案数值，不改变分值结构。"
        "裁判执裁以本标准及赛前锁版的标准答案为准。",
        first_line=True,
        space_after=10,
    )

    # —— 1 总则 ——
    add_h1(doc, "一、总则")
    add_h2(doc, "（一）成绩构成")
    add_text(
        doc,
        "竞赛总成绩实行百分制。理论知识（模块A）占30%，操作技能（模块B、C、D）合计占70%。"
        "各模块满分及建议时长如下。",
        first_line=True,
    )
    add_table(
        doc,
        ["模块", "名称", "建议时长（min）", "满分"],
        [
            ["A", "理论测试（竞赛平台）", "45", "30"],
            ["B", "EDA工程设计", "75", "25"],
            ["C", "CAM审核与工艺文件编制", "60", "20"],
            ["D", "成品板质量检测与缺陷分析", "60", "25"],
            ["合计", "—", "240", "100"],
        ],
    )
    add_text(
        doc,
        "注：模块D命题材料中若出现“40分”等模块内演算分，不得作为正式满分；正式满分以本表25分为准。",
        first_line=True,
    )

    add_h2(doc, "（二）计分与小数")
    add_text(
        doc,
        "1.各模块得分及总成绩均保留两位小数，第三位小数按四舍五入处理。"
        "2.模块内允许出现0.5分档（如模块C单处缺陷部分分）。"
        "3.各模块得分不得为负，扣分后最低记0.00分。"
        "4.总成绩＝模块A得分＋模块B得分＋模块C得分＋模块D得分。",
        first_line=True,
    )

    add_h2(doc, "（三）并列名次")
    add_text(
        doc,
        "总成绩相同时，按下列顺序比较，高者名次列前："
        "（1）操作技能成绩（模块B＋C＋D之和）；（2）模块D得分；（3）模块C得分；（4）模块B得分。"
        "仍相同的，由裁判长裁定或并列名次（赛区另有规定的从其规定）。",
        first_line=True,
    )

    add_h2(doc, "（四）主观评价通用规则（适用于模块B评价分）")
    add_text(
        doc,
        "评价分采用0～3权重档描述。建议3名裁判独立给档后取算术平均，"
        "再按该子项满分换算为实际得分，保留两位小数。"
        "裁判相互间档差大于1档的，须在小组长或裁判长主持下复议。",
        first_line=True,
    )
    add_table(
        doc,
        ["权重档", "含义"],
        [
            ["0", "各方面均低于行业标准，包括未做尝试"],
            ["1", "达到行业标准"],
            ["2", "达到行业标准，且某些方面超过标准"],
            ["3", "达到行业期待的优秀水平"],
        ],
    )

    add_h2(doc, "（五）违规与异常")
    add_text(
        doc,
        "作弊、携带通讯工具、使用未批准软件、泄露身份信息、损坏样件等，按技术工作文件及赛区纪律处理，"
        "可取消相应模块或全部成绩。设备故障非选手原因的，由裁判长决定补时或处置，并记录备案。",
        first_line=True,
    )

    # —— 2 模块A ——
    add_h1(doc, "二、模块A　理论测试（满分30分）")
    add_text(
        doc,
        "模块A在竞赛平台完成，闭卷。平台卷面满分按100分计，折算计入总成绩：",
        first_line=True,
    )
    add_text(doc, "模块A得分＝平台卷面得分×0.30（保留两位小数）。", first_line=True, bold=True)
    add_text(
        doc,
        "题型、题量以竞赛平台及赛前公布为准。技术工作文件0721定稿中的参考结构为："
        "单选题40题、多选题30题、判断题30题，合计100分；多选漏选、错选不得分（以平台规则为准）。"
        "评分以平台自动评分为主，必要时裁判复核。本评分标准不另附理论试题。",
        first_line=True,
    )

    # —— 3 模块B ——
    add_h1(doc, "三、模块B　EDA工程设计（满分25分）")
    add_text(
        doc,
        "根据技术工作文件0721定稿，模块B评价分约占80%、测量分约占20%，即：",
        first_line=True,
    )
    add_table(
        doc,
        ["类别", "满分", "说明"],
        [
            ["评价分", "20", "布局、布线等主观评价"],
            ["测量分", "5", "客观检查项，通过/不通过或分档"],
            ["合计", "25", "—"],
        ],
    )

    add_h2(doc, "（一）评价分（20分）")
    add_text(
        doc,
        "评价子项及0～3档描述如下。两项可各占评价分的50%（即各10分），"
        "换算公式：子项得分＝（三名裁判档平均÷3）×该子项满分。亦可按裁判组统一的等权子项拆分，但评价分合计须为20分。",
        first_line=True,
    )
    add_table(
        doc,
        ["评价子项", "0分", "1分", "2分", "3分"],
        [
            [
                "布局合理性",
                "无模块化分组，器件随意放置",
                "有基本分组，主要接口靠边",
                "模块化清晰，功能器件就近放置，整体合理",
                "布局专业美观，信号路径最短化，EMC考量充分",
            ],
            [
                "布线策略",
                "走线杂乱，无设计策略",
                "走线基本整齐，有基本布线规范",
                "差分对等长、电源网络加宽，策略明确",
                "高速信号走线规范、回流路径优化、整体专业水准",
            ],
        ],
    )

    add_h2(doc, "（二）测量分（5分）")
    add_text(
        doc,
        "下列检查项根据选手提交的工程文件、Gerber及现场软件检查结果给分。"
        "未提交相应文件的，该项得0分。",
        first_line=True,
    )
    add_table(
        doc,
        ["序号", "检查项", "满分", "给分要点"],
        [
            ["1", "DRC检查", "1.5", "无错误或违规数在赛前公布的允许范围内；有致命错误不得分"],
            ["2", "网络连通/网表一致性", "1.5", "关键网络连通，原理图与PCB一致；严重开路/错连不得分"],
            ["3", "Gerber完整性", "1.0", "规定层齐全、可正常打开；缺关键层按比例扣完为止"],
            ["4", "提交规范", "1.0", "文件命名、目录、格式符合赛题要求"],
        ],
    )
    add_text(
        doc,
        "注：封装正确性、电源线宽等可并入上表相应项或赛前补充细则，但模块B总分不得超过25分。",
        first_line=True,
    )

    # —— 4 模块C ——
    add_h1(doc, "四、模块C　CAM审核与工艺文件编制（满分20分）")
    add_table(
        doc,
        ["类别", "满分", "说明"],
        [
            ["缺陷识别", "14", "对照标准缺陷清单"],
            ["简版制程工艺卡", "6", "仅能量/能数清字段"],
            ["合计", "20", "—"],
        ],
    )

    add_h2(doc, "（一）缺陷识别（14分）")
    add_text(
        doc,
        "标准缺陷共12处（以锁版《模块C缺陷记录表·参考答案》为准）。计分结构为：",
        first_line=True,
    )
    add_table(
        doc,
        ["分项", "满分", "说明"],
        [
            ["必检缺陷命中", "12", "12处×每处最高1.0分"],
            ["无严重误报", "2", "乱报、明显不存在的缺陷从本分项扣"],
            ["合计", "14", "—"],
        ],
    )
    add_text(doc, "1.单处缺陷给分（0／0.5／1.0）", first_line=True, bold=True)
    add_table(
        doc,
        ["得分", "条件"],
        [
            ["1.0", "类型正确（含赛前公布的同义表述）＋所在层/面正确＋位置可唯一对应标准缺陷点"],
            ["0.5", "类型正确＋层/面正确，位置同区域但不精确"],
            ["0", "类型错误，或层/面错误，或明显指错位置"],
        ],
    )
    add_text(
        doc,
        "2.误报：不在标准清单、且不能合理解释为同一缺陷同义描述的，计为误报。"
        "每处误报从“无严重误报”2分中扣1.0分，扣完为止；不倒扣命中分。"
        "3.仅承认12处标准缺陷得分；多写干扰项不另加分。",
        first_line=True,
    )

    add_h2(doc, "（二）简版制程工艺卡（6分）")
    add_text(
        doc,
        "依据Gerber与钻孔文件实测填写，不要求板厚、铜厚、表面处理、阻焊颜色等订单约定项。"
        "不得将故意缺陷尺寸（如极细缺陷线）填为“最小线宽/线距”规格。",
        first_line=True,
    )
    add_table(
        doc,
        ["序号", "采分项", "满分", "参考给分口径"],
        [
            ["1", "层数", "1", "双面/2层即得"],
            ["2", "外形尺寸（长×宽）", "1", "与板框实测一致（允许合理误差，如±0.5mm或等价mil）"],
            ["3", "最小孔径", "1", "与钻孔最小工具一致（如约12mil/0.30mm）"],
            ["4", "最小线宽＋最小线距（正常区）", "2", "正常区合理量级；写成缺陷极值不得分或酌扣"],
            ["5", "阻焊开窗方式", "1", "1:1开窗等正确表述"],
            ["6", "文件完整性", "1", "正确指出缺层（本套为缺底层丝印等）或齐全情况与事实一致"],
        ],
    )

    # —— 5 模块D ——
    add_h1(doc, "五、模块D　成品板质量检测与缺陷分析（满分25分）")
    add_text(
        doc,
        "模块D正式满分25分。分项如下（与第一套检测记录表、参考答案配套）。",
        first_line=True,
    )
    add_table(
        doc,
        ["分项", "满分", "对应内容"],
        [
            ["外观缺陷", "10", "V01～V05"],
            ["尺寸与指定点", "7", "M01～M05"],
            ["基础电气", "5", "E01、E02"],
            ["综合判定", "2", "分类与接收/返工/报废及依据"],
            ["记录规范", "1", "版本、单位、完整性等"],
            ["合计", "25", "—"],
        ],
    )

    add_h2(doc, "（一）外观缺陷（10分）")
    add_text(doc, "V01～V05各2分。类型以样题标准名及同义表述为准。", first_line=True)
    add_table(
        doc,
        ["编号", "标准类型（样题）", "满分", "给分要点"],
        [
            ["V01", "顶层线路缺口", "2", "顶层＋缺口/断线类＋位置大致正确"],
            ["V02", "底层线宽变窄（颈缩）", "2", "底层＋变窄/颈缩且不断开＋位置大致正确；与M05不同点"],
            ["V03", "孔破盘", "2", "孔破盘/环宽异常＋面与位置大致正确（非定位孔误报）"],
            ["V04", "阻焊未开窗或开窗偏移", "2", "阻焊类＋位置大致正确"],
            ["V05", "丝印缺失或压焊盘", "2", "丝印类＋位置大致正确"],
        ],
    )
    add_text(
        doc,
        "单处建议：要素齐全得2分；类型对、位置含糊得1分；类型错或漏检得0分。"
        "底面缺陷漏检（如V02、V03）按上表扣分。不设V06必检分。",
        first_line=True,
    )

    add_h2(doc, "（二）尺寸与指定点（7分）")
    add_table(
        doc,
        ["编号", "项目", "满分", "工具与口径（与样题一致，以锁版答案为准）"],
        [
            ["M01", "板长L", "1", "卡尺；名义80.00，允许误差±0.20"],
            ["M02", "板宽W", "1", "卡尺；名义60.00，允许误差±0.20"],
            ["M03", "板厚T", "1", "厚度规/千分尺；名义1.60，允许误差±0.15"],
            ["M04", "定位孔径", "1", "卡尺/孔规；名义3.50，允许误差±0.10（本套板）"],
            ["M05", "指定点线宽或线距", "3", "测量显微镜；不得与V02同点；允许误差±0.05或按锁版真值"],
        ],
    )
    add_text(
        doc,
        "读数在公差内且项目、工具使用正确的，得该项满分；超差或项目填错不得分。"
        "禁止用普通卡尺作为M05细线宽的唯一依据。",
        first_line=True,
    )

    add_h2(doc, "（三）基础电气（5分）")
    add_table(
        doc,
        ["编号", "内容", "满分", "预期与给分"],
        [
            ["E01", "TP1－TP2开路", "2.5", "判定为开路且与预期一致（如≥1MΩ或OL）得2.5；否则0"],
            ["E02", "TP3－TP4短路", "2.5", "判定为短路且与预期一致（如≤1Ω）得2.5；否则0"],
        ],
    )

    add_h2(doc, "（四）综合判定（2分）")
    add_text(
        doc,
        "分类汇总基本合理得1分；质量处置与依据正确得1分。"
        "存在线路缺口、孔破盘等致命缺陷时，处置应为“报废”（不得判“接收”）。"
        "电气符合预设预期不改变致命外观缺陷的报废结论。",
        first_line=True,
    )

    add_h2(doc, "（五）记录规范（1分）")
    add_text(
        doc,
        "版本（MOD-D-S-A/B）与实物一致、单位mm、表头与必填项完整、无姓名单位等身份信息，得1分；明显缺项得0分。",
        first_line=True,
    )

    # —— 6 统分 ——
    add_h1(doc, "六、统分与文书")
    add_text(
        doc,
        "1.各评分小组按本标准评分并签字→裁判长复核→汇总公布。"
        "2.模块A以平台导出成绩为依据折算。"
        "3.标准答案、缺陷清单、尺寸与电气真值以赛前锁版文件为准；投板三测后允许修订真值，不改变本标准分值结构。"
        "4.本标准解释权归赛项裁判组。",
        first_line=True,
    )

    add_text(doc, "", space_after=12)
    add_table(
        doc,
        ["角色", "签字", "日期"],
        [
            ["裁判长", "", ""],
            ["评分组长（B）", "", ""],
            ["评分组长（C）", "", ""],
            ["评分组长（D）", "", ""],
        ],
    )

    add_text(
        doc,
        "（正文完）",
        center=True,
        space_before=12,
        space_after=6,
    )

    OUT.parent.mkdir(parents=True, exist_ok=True)
    doc.save(OUT)
    print("wrote", OUT)


if __name__ == "__main__":
    build()
