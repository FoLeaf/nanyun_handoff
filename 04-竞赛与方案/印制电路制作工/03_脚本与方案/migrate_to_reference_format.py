# -*- coding: utf-8 -*-
# 注意：本脚本为历史/辅助生成器，权威提交文件以 01_技术文件 与 02_样题 中现有 docx 为准。
# 请勿用本脚本输出直接覆盖提交集权威文件（尤其 V1 技术文件与完整版样题）。
# 默认输出到 03_脚本与方案/_generated/（可复跑草稿目录）。
"""
将印制电路制作工职业技能竞赛技术文件迁移至省级参考格式
参考格式：江西省省级职业技能竞赛项目技术工作文件.doc（9章结构）
"""
import os
import copy
from docx import Document
from docx.shared import Pt, Cm, Inches, RGBColor, Emu
from docx.enum.text import WD_ALIGN_PARAGRAPH
from docx.enum.table import WD_TABLE_ALIGNMENT
from docx.enum.section import WD_ORIENT
from docx.oxml.ns import qn, nsdecls
from docx.oxml import parse_xml
import re

# ==================== 工具函数 ====================

def set_cell_shading(cell, color):
    shading = parse_xml(f'<w:shd {nsdecls("w")} w:fill="{color}" w:val="clear"/>')
    cell._tc.get_or_add_tcPr().append(shading)

def set_cell_border(cell, **kwargs):
    tc = cell._tc
    tcPr = tc.get_or_add_tcPr()
    tcBorders = parse_xml(f'<w:tcBorders {nsdecls("w")}></w:tcBorders>')
    for edge, val in kwargs.items():
        element = parse_xml(
            f'<w:{edge} {nsdecls("w")} w:val="{val.get("val", "single")}" '
            f'w:sz="{val.get("sz", "4")}" w:space="0" '
            f'w:color="{val.get("color", "000000")}"/>'
        )
        tcBorders.append(element)
    tcPr.append(tcBorders)

def add_formatted_paragraph(doc, text, font_name='宋体', font_size=Pt(12),
                            bold=False, alignment=WD_ALIGN_PARAGRAPH.JUSTIFY,
                            space_before=Pt(0), space_after=Pt(0),
                            line_spacing=Pt(28), first_line_indent=None,
                            font_name_ascii='Times New Roman'):
    p = doc.add_paragraph()
    p.alignment = alignment
    pf = p.paragraph_format
    pf.space_before = space_before
    pf.space_after = space_after
    pf.line_spacing = line_spacing
    if first_line_indent:
        pf.first_line_indent = first_line_indent
    run = p.add_run(text)
    run.font.name = font_name
    run._element.rPr.rFonts.set(qn('w:eastAsia'), font_name)
    run._element.rPr.rFonts.set(qn('w:ascii'), font_name_ascii)
    run._element.rPr.rFonts.set(qn('w:hAnsi'), font_name_ascii)
    run.font.size = font_size
    run.font.bold = bold
    return p

def add_heading_custom(doc, text, level=1):
    if level == 1:
        font_name = '黑体'
        font_size = Pt(18)
        alignment = WD_ALIGN_PARAGRAPH.LEFT
        space_before = Pt(12)
        space_after = Pt(6)
    elif level == 2:
        font_name = '黑体'
        font_size = Pt(15)
        alignment = WD_ALIGN_PARAGRAPH.LEFT
        space_before = Pt(6)
        space_after = Pt(3)
    elif level == 3:
        font_name = '黑体'
        font_size = Pt(12)
        alignment = WD_ALIGN_PARAGRAPH.LEFT
        space_before = Pt(3)
        space_after = Pt(3)
    else:
        font_name = '黑体'
        font_size = Pt(12)
        alignment = WD_ALIGN_PARAGRAPH.LEFT
        space_before = Pt(0)
        space_after = Pt(0)

    p = doc.add_paragraph()
    p.alignment = alignment
    pf = p.paragraph_format
    pf.space_before = space_before
    pf.space_after = space_after
    pf.line_spacing = Pt(28)
    run = p.add_run(text)
    run.font.name = font_name
    run._element.rPr.rFonts.set(qn('w:eastAsia'), font_name)
    run.font.size = font_size
    run.font.bold = True
    return p

def add_body_text(doc, text, indent=True):
    return add_formatted_paragraph(
        doc, text,
        font_name='宋体', font_size=Pt(12),
        first_line_indent=Cm(0.74) if indent else None
    )

def add_table_with_style(doc, headers, rows, col_widths=None):
    table = doc.add_table(rows=1 + len(rows), cols=len(headers))
    table.alignment = WD_TABLE_ALIGNMENT.CENTER
    table.style = 'Table Grid'

    for i, header in enumerate(headers):
        cell = table.rows[0].cells[i]
        cell.text = ''
        p = cell.paragraphs[0]
        p.alignment = WD_ALIGN_PARAGRAPH.CENTER
        run = p.add_run(header)
        run.font.name = '黑体'
        run._element.rPr.rFonts.set(qn('w:eastAsia'), '黑体')
        run.font.size = Pt(10.5)
        run.font.bold = True
        set_cell_shading(cell, 'D9E2F3')

    for r_idx, row_data in enumerate(rows):
        for c_idx, cell_text in enumerate(row_data):
            cell = table.rows[r_idx + 1].cells[c_idx]
            cell.text = ''
            p = cell.paragraphs[0]
            p.alignment = WD_ALIGN_PARAGRAPH.CENTER
            run = p.add_run(str(cell_text))
            run.font.name = '宋体'
            run._element.rPr.rFonts.set(qn('w:eastAsia'), '宋体')
            run.font.size = Pt(10.5)

    if col_widths:
        for i, width in enumerate(col_widths):
            for row in table.rows:
                row.cells[i].width = width
    return table

# ==================== 读取原文件内容 ====================

_SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
_ROOT = os.path.dirname(_SCRIPT_DIR)
_GEN_DIR = os.path.join(_SCRIPT_DIR, '_generated')
os.makedirs(_GEN_DIR, exist_ok=True)
_src_candidates = [
    os.path.join(_GEN_DIR, '印制电路制作工职业技能竞赛技术文件_draft.docx'),
    os.path.join(_ROOT, '05_归档备份', 'cross-review-2026-07-16', '01_技术文件', '印制电路制作工职业技能竞赛技术文件.docx'),
    os.path.join(_ROOT, '01_技术文件', '印制电路制作工竞赛技术工作文件V1.docx'),
]
_src = next((p for p in _src_candidates if os.path.exists(p)), _src_candidates[-1])
src = Document(_src)

def get_para_text(idx):
    if idx < len(src.paragraphs):
        return src.paragraphs[idx].text.strip()
    return ''

def get_table_data(idx):
    if idx < len(src.tables):
        t = src.tables[idx]
        data = []
        for row in t.rows:
            data.append([cell.text.strip() for cell in row.cells])
        return data
    return []

# ==================== 创建新文档 ====================

doc = Document()

# 页面设置 A4
section = doc.sections[0]
section.page_width = Cm(21)
section.page_height = Cm(29.7)
section.top_margin = Cm(2.54)
section.bottom_margin = Cm(2.54)
section.left_margin = Cm(3.17)
section.right_margin = Cm(3.17)

# ===== 封面 =====
for _ in range(4):
    doc.add_paragraph()

add_formatted_paragraph(doc, '江西省"振兴杯"职业技能竞赛',
                        font_name='小标宋', font_size=Pt(22),
                        alignment=WD_ALIGN_PARAGRAPH.CENTER,
                        font_name_ascii='小标宋')

doc.add_paragraph()

add_formatted_paragraph(doc, '印制电路制作工',
                        font_name='黑体', font_size=Pt(36),
                        bold=True, alignment=WD_ALIGN_PARAGRAPH.CENTER)

add_formatted_paragraph(doc, '技术工作文件',
                        font_name='黑体', font_size=Pt(36),
                        bold=True, alignment=WD_ALIGN_PARAGRAPH.CENTER)

doc.add_paragraph()

add_formatted_paragraph(doc, '工程资料处理与质量检测方向',
                        font_name='宋体', font_size=Pt(16),
                        alignment=WD_ALIGN_PARAGRAPH.CENTER)

for _ in range(6):
    doc.add_paragraph()

add_formatted_paragraph(doc, '主办单位：XXXX',
                        font_name='宋体', font_size=Pt(14),
                        alignment=WD_ALIGN_PARAGRAPH.CENTER)
add_formatted_paragraph(doc, '承办单位：XXXX',
                        font_name='宋体', font_size=Pt(14),
                        alignment=WD_ALIGN_PARAGRAPH.CENTER)
add_formatted_paragraph(doc, '技术支持：南云信息科技有限公司',
                        font_name='宋体', font_size=Pt(14),
                        alignment=WD_ALIGN_PARAGRAPH.CENTER)

doc.add_paragraph()
add_formatted_paragraph(doc, '2026年XX月XX日',
                        font_name='宋体', font_size=Pt(14),
                        alignment=WD_ALIGN_PARAGRAPH.CENTER)

# ===== 目录页 =====
doc.add_page_break()
add_formatted_paragraph(doc, '目  录',
                        font_name='黑体', font_size=Pt(22),
                        bold=True, alignment=WD_ALIGN_PARAGRAPH.CENTER,
                        space_after=Pt(12))

add_formatted_paragraph(doc, '（请在Word中右键点击此处 → 更新域 → 更新整个目录）',
                        font_name='宋体', font_size=Pt(10.5),
                        alignment=WD_ALIGN_PARAGRAPH.CENTER)

# 插入TOC域代码
p_toc = doc.add_paragraph()
run_toc = p_toc.add_run()
fldChar1 = parse_xml(f'<w:fldChar {nsdecls("w")} w:fldCharType="begin"/>')
run_toc._element.append(fldChar1)
run_toc2 = p_toc.add_run()
instrText = parse_xml(f'<w:instrText {nsdecls("w")} xml:space="preserve"> TOC \\o "1-3" \\h \\z \\u </w:instrText>')
run_toc2._element.append(instrText)
run_toc3 = p_toc.add_run()
fldChar2 = parse_xml(f'<w:fldChar {nsdecls("w")} w:fldCharType="separate"/>')
run_toc3._element.append(fldChar2)
run_toc4 = p_toc.add_run('（目录将在Word中更新域后显示）')
run_toc4.font.color.rgb = RGBColor(128, 128, 128)
run_toc5 = p_toc.add_run()
fldChar3 = parse_xml(f'<w:fldChar {nsdecls("w")} w:fldCharType="end"/>')
run_toc5._element.append(fldChar3)

# ===== 正文开始 =====
doc.add_page_break()

# ============================================================
# 第一章 项目简介
# ============================================================
add_heading_custom(doc, '1. 项目简介', level=1)

# 1.1 项目描述
add_heading_custom(doc, '1.1 项目描述', level=2)

add_body_text(doc, '本赛项为印制电路制作工职业技能竞赛，竞赛方向为工程资料处理与质量检测。'
    '竞赛以《印制电路制作工国家职业技能标准（2019年版）》相关要求为依据，'
    '结合当前PCB数字化设计与制造流程，重点考查选手在PCB工程资料处理、制造规则应用、'
    'DFM审查和裸板缺陷检测方面的综合能力。')

add_body_text(doc, '本赛项面向从事或学习印制电路板设计、制造、检测相关工作的从业人员及在校学生，'
    '重点考查选手在PCB工程资料处理、制造规则应用、DFM审查和裸板缺陷检测方面的综合能力。')

add_body_text(doc, '竞赛方向说明：传统印制电路制作工竞赛依托制造产线设备，考核蚀刻、电镀、钻孔、'
    '丝印等制造工艺操作技能。本赛项在不具备产线设备的条件下，根据《印制电路制作工国家职业技能标准'
    '（2019年版）》中"工程资料输出""设计规则应用""品质检验"等工作内容，设置"工程资料处理与质量检测"'
    '方向，重点考核PCB设计文件处理、可制造性分析和裸板质量检测能力。')

add_body_text(doc, '该方向对应PCB行业中CAM工程师、品质工程师、工程审核等核心岗位，'
    '是印制电路制作工职业技能体系的重要组成部分。')

# 竞赛基本信息表格
add_body_text(doc, '竞赛基本信息如下：', indent=False)
info_rows = [
    ['竞赛名称', '江西省"振兴杯"职业技能竞赛印制电路制作工赛项'],
    ['竞赛工种', '印制电路制作工'],
    ['竞赛方向', '工程资料处理与质量检测'],
    ['参赛对象', '从事或学习PCB设计、制造、检测相关工作的从业人员及在校学生'],
    ['竞赛形式', '个人赛，理论考试+实操考核'],
]
add_table_with_style(doc, ['项目', '内容'], info_rows,
                     col_widths=[Cm(3), Cm(11)])

# 1.2 考核目的
add_heading_custom(doc, '1.2 考核目的', level=2)

add_body_text(doc, '1. 考查选手PCB工程资料处理能力，包括原理图导入、封装匹配、规则设置和布局布线。')
add_body_text(doc, '2. 考查选手Gerber文件、钻孔文件、工艺文件输出能力，确保工程资料符合制造要求。')
add_body_text(doc, '3. 考查选手DFM检查和制造缺陷识别能力，能够发现并分析常见PCB制造缺陷。')
add_body_text(doc, '4. 考查选手安全、规范、质量意识，养成标准化作业习惯。')
add_body_text(doc, '5. 推动印制电路制作相关技能人才培养，促进产教融合和技能竞赛成果转化。')

# 1.3 相关文件
add_heading_custom(doc, '1.3 相关文件', level=2)

add_body_text(doc, '本赛项技术文件依据以下标准和规范制定：')
add_body_text(doc, '1. 《印制电路制作工国家职业技能标准（2019年版）》')
add_body_text(doc, '2. 《GB/T 4588-2025 单双面刚性印制板分规范》（2026年7月1日起实施）')
add_body_text(doc, '3. 《IPC-A-600M 印制板的可接受性》（2025年5月发布）')
add_body_text(doc, '4. 《T/CPCA 6043A-2023 单双面碳膜印制电路板》')
add_body_text(doc, '5. 《IPC-2221 印制板设计通用标准》')
add_body_text(doc, '6. 竞赛组委会相关规则与管理办法')

add_body_text(doc, '特别说明：本赛项定位为PCB制造相关工程能力考核，不涉及完整电子产品设计流程，'
    '重点考查印制电路板工程资料处理、可制造性分析和质量检测等核心岗位技能。')

# ============================================================
# 第二章 基本能力与职业标准
# ============================================================
add_heading_custom(doc, '2. 基本能力与职业标准', level=1)

add_body_text(doc, '根据《印制电路制作工国家职业技能标准（2019年版）》相关要求，'
    '参加本赛项的选手应具备以下能力：')

ability_rows = get_table_data(1)  # Table 1: 能力表格
add_table_with_style(doc, ability_rows[0], ability_rows[1:],
                     col_widths=[Cm(1.5), Cm(3.5), Cm(9)])

# ============================================================
# 第三章 竞赛内容
# ============================================================
add_heading_custom(doc, '3. 竞赛内容', level=1)

# 3.1 考核内容
add_heading_custom(doc, '3.1 考核内容', level=2)

add_body_text(doc, '竞赛分为四个模块，总分100分。具体如下：')

module_rows = get_table_data(2)  # Table 2: 模块表格
add_table_with_style(doc, module_rows[0], module_rows[1:],
                     col_widths=[Cm(3.5), Cm(7), Cm(2)])

# 3.2 竞赛模块
add_heading_custom(doc, '3.2 竞赛模块', level=2)

add_body_text(doc, '本赛项四个竞赛模块与《印制电路制作工国家职业技能标准（2019年版）》'
    '工作内容的对应关系如下：')

correspond_rows = get_table_data(0)  # Table 0: 对应关系表格
add_table_with_style(doc, correspond_rows[0], correspond_rows[1:],
                     col_widths=[Cm(3.5), Cm(7.5), Cm(3)])

# 3.3 模块简述
add_heading_custom(doc, '3.3 模块简述', level=2)

# 模块A：理论知识
add_heading_custom(doc, '3.3.1 模块A：理论知识', level=3)
add_body_text(doc, '理论知识模块采用闭卷考试形式，考核内容包括：')
add_body_text(doc, '1. 《印制电路制作工国家职业技能标准》相关知识。')
add_body_text(doc, '2. PCB基础理论：材料、工艺流程、制造方法等。')
add_body_text(doc, '3. 安全规范与质量标准。')
add_body_text(doc, '4. 常见制造缺陷类型与判定标准。')

# 模块B：PCB工程资料处理
add_heading_custom(doc, '3.3.2 模块B：PCB工程资料处理', level=3)
add_body_text(doc, '选手根据题目提供的原理图、封装库、板框尺寸和制造规则，'
    '在嘉立创EDA中完成PCB设计。具体要求如下：')
add_body_text(doc, '1. 选手不得更改原理图电气连接，不得删除指定元件和接口。')
add_body_text(doc, '2. 选手需根据制造规则设置线宽、线距、过孔尺寸等参数。')
add_body_text(doc, '3. 选手需完成元器件布局，布局应合理、紧凑、符合制造要求。')
add_body_text(doc, '4. 选手需完成布线，布线应满足DRC检查要求，无严重错误。')
add_body_text(doc, '5. 选手需运行DRC检查，确保设计无严重违规。')

add_body_text(doc, '选手需输出以下工程文件：')
add_body_text(doc, '1. Gerber文件：包含所有层文件，命名规范，层别完整。')
add_body_text(doc, '2. NC Drill钻孔文件：钻孔文件完整，包含钻孔表。')
add_body_text(doc, '3. 钻孔表：包含孔径、数量、属性等信息。')
add_body_text(doc, '4. DRC报告：不得存在严重错误。')
add_body_text(doc, '5. PDF预览图：包含各层预览，便于制造核对。')

# 模块C：DFM审查
add_heading_custom(doc, '3.3.3 模块C：DFM审查', level=3)
add_body_text(doc, '选手需根据制造规则，对设计进行DFM（可制造性设计）审查，具体检查项目包括：')
add_body_text(doc, '1. 线宽、线距是否满足制造能力要求。')
add_body_text(doc, '2. 孔径、过孔尺寸是否在制造能力范围内。')
add_body_text(doc, '3. 焊盘环宽是否满足最小要求。')
add_body_text(doc, '4. 板边距是否满足制造和装配要求。')
add_body_text(doc, '5. 阻焊开窗是否正确，丝印方向是否规范。')
add_body_text(doc, '6. 定位孔、工艺边是否设置正确。')
add_body_text(doc, '7. 填写DFM审查表，记录检查结果和发现的问题。')

# 模块D：裸板缺陷检测
add_heading_custom(doc, '3.3.4 模块D：裸板缺陷检测', level=3)
add_body_text(doc, '选手对裁判提供的缺陷板或缺陷图片进行检测，具体要求如下：')
add_body_text(doc, '1. 识别缺陷类型（如开路、短路、缺口、针孔、偏位、阻焊不良等）。')
add_body_text(doc, '2. 准确标注缺陷位置。')
add_body_text(doc, '3. 分析缺陷可能产生的原因。')
add_body_text(doc, '4. 提出合理的处理建议。')
add_body_text(doc, '5. 填写缺陷检测记录表。')

# 3.4 命题方式
add_heading_custom(doc, '3.4 命题方式', level=2)

add_body_text(doc, '1. 赛前公布竞赛样题，样题内容、题型、难度与正式赛题相当，供选手熟悉竞赛形式和要求。')
add_body_text(doc, '2. 正式赛题在样题基础上进行变化，难度不低于样题，具体变化范围赛前不另行通知。')
add_body_text(doc, '3. 理论知识模块从题库中随机抽取或由裁判组封闭命题。')
add_body_text(doc, '4. 实操模块（PCB工程资料处理、DFM审查、缺陷检测）由裁判组统一命题，赛前保密。')
add_body_text(doc, '5. 缺陷检测模块的缺陷样板由裁判组在赛前统一制备，确保每工位样板缺陷类型和数量一致。')

# 3.5 竞赛日程及地点安排
add_heading_custom(doc, '3.5 竞赛日程及地点安排', level=2)

add_body_text(doc, '竞赛采用个人赛方式进行，理论考试与实操考核相结合。')
add_body_text(doc, '竞赛总时长为4小时（含检录与准备时间）。各模块时间分配由组委会在赛前统一公布，'
    '选手须在规定时间内完成全部竞赛任务。')

schedule_rows = get_table_data(9)  # Table 9: 日程表格
add_table_with_style(doc, schedule_rows[0], schedule_rows[1:],
                     col_widths=[Cm(1.5), Cm(3), Cm(9)])

# ============================================================
# 第四章 评分标准
# ============================================================
add_heading_custom(doc, '4. 评分标准', level=1)

add_body_text(doc, '竞赛总分100分，各模块评分标准如下：')

# 4.1 评价分（主观）
add_heading_custom(doc, '4.1 评价分（主观）', level=2)

add_body_text(doc, '（一）理论知识（25分）')
theory_rows = get_table_data(4)  # Table 4: 理论评分
add_table_with_style(doc, theory_rows[0], theory_rows[1:],
                     col_widths=[Cm(1.5), Cm(3.5), Cm(2), Cm(7)])

add_body_text(doc, '（二）PCB工程资料处理（30分）')
pcb_rows = get_table_data(5)  # Table 5: PCB评分
add_table_with_style(doc, pcb_rows[0], pcb_rows[1:],
                     col_widths=[Cm(1.5), Cm(3.5), Cm(2), Cm(7)])

# 4.2 测量分（客观）
add_heading_custom(doc, '4.2 测量分（客观）', level=2)

add_body_text(doc, '（三）DFM与工艺文件（25分）')
dfm_rows = get_table_data(6)  # Table 6: DFM评分
add_table_with_style(doc, dfm_rows[0], dfm_rows[1:],
                     col_widths=[Cm(1.5), Cm(3.5), Cm(2), Cm(7)])

add_body_text(doc, '（四）裸板缺陷检测（20分）')
defect_rows = get_table_data(7)  # Table 7: 缺陷检测评分
add_table_with_style(doc, defect_rows[0], defect_rows[1:],
                     col_widths=[Cm(1.5), Cm(3.5), Cm(2), Cm(7)])

# 4.3 评分流程说明
add_heading_custom(doc, '4.3 评分流程说明', level=2)

add_body_text(doc, '1. 理论知识模块采用机考自动评分或人工流水阅卷。')
add_body_text(doc, '2. 实操模块采用裁判分项评分，每份作品至少由2名裁判独立评分。')
add_body_text(doc, '3. 评分结果取平均分，分差超过规定范围的由总裁判长裁定。')

# 4.4 统分方法
add_heading_custom(doc, '4.4 统分方法', level=2)

add_body_text(doc, '1. 竞赛成绩由理论知识（25分）、PCB工程资料处理（30分）、DFM与工艺文件（25分）、'
    '裸板缺陷检测（20分）四个模块成绩相加得出，满分100分。')
add_body_text(doc, '2. 总成绩相同的，按以下顺序优先排名：')
add_body_text(doc, '（1）实操成绩高者优先；')
add_body_text(doc, '（2）PCB工程资料处理模块成绩高者优先；')
add_body_text(doc, '（3）缺陷检测模块成绩高者优先；')
add_body_text(doc, '（4）提交时间早者优先。')
add_body_text(doc, '3. 成绩计算保留小数点后两位，四舍五入。')

# 4.5 裁判构成和分组
add_heading_custom(doc, '4.5 裁判构成和分组', level=2)

add_heading_custom(doc, '4.5.1 裁判组', level=3)
add_body_text(doc, '1. 设总裁判长1名，负责竞赛总体裁判工作。')
add_body_text(doc, '2. 设裁判员若干名，负责各模块的评判工作。')
add_body_text(doc, '3. 裁判员应具备相关专业背景和竞赛执裁经验。')

add_heading_custom(doc, '4.5.2 仲裁规则', level=3)
add_body_text(doc, '1. 设仲裁组，负责处理竞赛过程中的争议和申诉。')
add_body_text(doc, '2. 选手对成绩有异议的，须在成绩公布后1小时内以书面形式提出申诉。')
add_body_text(doc, '3. 仲裁组在收到申诉后2小时内作出裁决，裁决为最终结果。')

# ============================================================
# 第五章 竞赛相关设施设备
# ============================================================
add_heading_custom(doc, '5. 竞赛相关设施设备', level=1)

# 5.1 场地设备
add_heading_custom(doc, '5.1 场地设备', level=2)

add_heading_custom(doc, '（一）软件环境', level=3)
add_body_text(doc, '1. 统一使用嘉立创EDA专业版客户端（参考版本：v3.2.x，以赛前公布为准）。')
add_body_text(doc, '2. 竞赛电脑预装嘉立创EDA软件、封装库、规则文件和题目包。')
add_body_text(doc, '3. 竞赛期间不得自行安装软件、插件或连接外部存储设备。')
add_body_text(doc, '4. 理论考试系统由组委会统一提供，支持机考或纸笔考试。')

add_heading_custom(doc, '（二）硬件环境', level=3)
add_body_text(doc, '1. 每名选手配备计算机一台、显示器一台、鼠标键盘一套。')
add_body_text(doc, '2. 计算机配置不低于：Intel i5处理器、8GB内存、256GB固态硬盘、1920×1080显示器。')
add_body_text(doc, '3. 组委会统一提供缺陷检测用样板（含10种以上常见缺陷类型）及缺陷记录表。')

hw_rows = get_table_data(10)  # Table 10: 设备清单
add_table_with_style(doc, hw_rows[0], hw_rows[1:],
                     col_widths=[Cm(1), Cm(3.5), Cm(3.5), Cm(2.5), Cm(3)])

# 5.2 材料
add_heading_custom(doc, '5.2 材料', level=2)

add_body_text(doc, '1. 统一提供原理图文件、封装库文件、板框文件。')
add_body_text(doc, '2. 统一提供制造规则文件、工艺要求说明。')
add_body_text(doc, '3. 统一提供缺陷样板（含10种以上常见缺陷类型，由组委会定制或收集）。')
add_body_text(doc, '4. 统一提供DFM审查表、缺陷检测记录表等空白表格。')

# 5.3 竞赛选手自备的设备和工具
add_heading_custom(doc, '5.3 竞赛选手自备的设备和工具', level=2)

add_body_text(doc, '缺陷检测环节所需工具由选手自行携带，组委会不统一提供。选手可自带以下工具：')
add_body_text(doc, '1. 放大镜、显微镜等观察工具。')
add_body_text(doc, '2. 万用表、游标卡尺、千分尺等测量工具。')
add_body_text(doc, '3. 其他常规检测辅助工具。')

tool_rows = get_table_data(11)  # Table 11: 自带工具
add_table_with_style(doc, tool_rows[0], tool_rows[1:],
                     col_widths=[Cm(1.5), Cm(3.5), Cm(3.5), Cm(5)])

# 5.4 竞赛场地禁止自带使用的设备和材料
add_heading_custom(doc, '5.4 竞赛场地禁止自带使用的设备和材料', level=2)

add_body_text(doc, '选手不得携带具有通信、存储、联网功能的电子设备（如手机、平板电脑、'
    '智能手表等），不得携带与竞赛无关的参考资料。裁判有权对选手携带的工具进行检查，'
    '不符合要求的工具不得带入赛场。')

# ============================================================
# 第六章 项目特别规定
# ============================================================
add_heading_custom(doc, '6. 项目特别规定', level=1)

add_body_text(doc, '1. 选手须持有效证件按时检录入场，迟到15分钟以上视为弃赛。')
add_body_text(doc, '2. 竞赛期间不得携带手机、U盘等电子设备进入赛场。')
add_body_text(doc, '3. 不得抄袭、协助他人或接受他人协助。')
add_body_text(doc, '4. 不得擅自更改竞赛设备、软件配置。')
add_body_text(doc, '5. 不得向场外传递任何竞赛信息。')
add_body_text(doc, '6. 违反竞赛纪律者，取消竞赛资格，成绩作废。')
add_body_text(doc, '7. 提交截止后，选手不得再修改或补充任何文件。未按要求提交的作品，相关模块按零分处理。')

# ============================================================
# 第七章 赛场布局要求
# ============================================================
add_heading_custom(doc, '7. 赛场布局要求', level=1)

add_body_text(doc, '1. 赛场应光线充足、通风良好、温度适宜（22-26℃）。')
add_body_text(doc, '2. 每个工位面积不小于2平方米，工位间距不小于0.8米。')
add_body_text(doc, '3. 竞赛区域与观摩区域应有明显分隔。')
add_body_text(doc, '4. 赛场应配备UPS不间断电源，防止突然断电导致数据丢失。')
add_body_text(doc, '5. 赛场应覆盖无线网络（仅供竞赛系统使用），禁止选手自行联网。')
add_body_text(doc, '6. 赛场应配备监控设备，全程录像备查。')
add_body_text(doc, '7. 赛场应设置医疗急救点，配备基本急救药品和器材。')

# ============================================================
# 第八章 健康安全和绿色环保
# ============================================================
add_heading_custom(doc, '8. 健康安全和绿色环保', level=1)

add_body_text(doc, '1. 选手须遵守赛场安全管理规定，服从裁判和工作人员指挥。')
add_body_text(doc, '2. 缺陷检测环节使用工具时注意安全，防止划伤、扎伤。')
add_body_text(doc, '3. 赛场内严禁吸烟、饮食。')
add_body_text(doc, '4. 发生紧急情况时，按赛场应急预案执行。')

# ============================================================
# 第九章 开放赛场
# ============================================================
add_heading_custom(doc, '9. 开放赛场', level=1)

add_body_text(doc, '竞赛期间，赛场在不影响比赛正常进行的前提下，可根据组委会安排向相关人员开放观摩。'
    '观摩人员须遵守赛场管理规定，不得干扰选手比赛。')

# ============================================================
# 技术规范说明（附加章节）
# ============================================================
add_heading_custom(doc, '10. 技术规范说明', level=1)

add_heading_custom(doc, '10.1 PCB设计技术规范', level=2)
add_body_text(doc, '1. 最小线宽/线距：根据题目要求设定，一般不低于0.15mm（约6mil）。')
add_body_text(doc, '2. 最小孔径：根据题目要求设定，一般不低于0.3mm。')
add_body_text(doc, '3. 最小焊盘环宽：单边不小于0.15mm。')
add_body_text(doc, '4. 板边距：走线和焊盘距板边不小于0.3mm。')
add_body_text(doc, '5. 定位孔：按制造要求设置，一般为3.2mm非金属化孔。')

add_heading_custom(doc, '10.2 文件输出规范', level=2)
add_body_text(doc, '1. Gerber文件格式：RS-274X。')
add_body_text(doc, '2. NC Drill格式：Excellon。')
add_body_text(doc, '3. 坐标单位：公制（mm）。')
add_body_text(doc, '4. 双层板应包含以下层别：Top Layer（顶层线路）、Bottom Layer（底层线路）、'
    'Top Overlay（顶层丝印）、Bottom Overlay（底层丝印）、Top Solder（顶层阻焊）、'
    'Bottom Solder（底层阻焊）、Keep Out Layer（禁止布线层）、Drill Drawing（钻孔图）。')

add_heading_custom(doc, '10.3 缺陷检测技术规范', level=2)
add_body_text(doc, '1. 缺陷类型参照IPC-A-600M《印制板的可接受性》标准。')
add_body_text(doc, '2. 常见缺陷类型包括但不限于：开路、短路、缺口、针孔、偏位、阻焊不良、'
    '丝印不良、翘曲、分层、起泡、露铜等。')
add_body_text(doc, '3. 缺陷严重程度分为：致命缺陷（影响电气功能）、严重缺陷（影响可靠性）、'
    '一般缺陷（影响外观但不影响功能）、轻微缺陷（可接受范围内的工艺偏差）。')
add_body_text(doc, '4. 缺陷判定应结合产品等级（Class 1/2/3）进行，竞赛默认按Class 2（高可靠性产品）标准判定。')

# ============================================================
# 保存主文件
# ============================================================
output_path = os.path.join(_GEN_DIR, '印制电路制作工_技术工作文件_省级格式_draft.docx')
doc.save(output_path)
print(f'主文件已保存: {output_path}')
