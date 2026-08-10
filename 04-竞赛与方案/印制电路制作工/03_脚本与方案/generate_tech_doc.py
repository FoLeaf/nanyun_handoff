# 注意：本脚本为历史/辅助生成器，权威提交文件以 01_技术文件 与 02_样题 中现有 docx 为准。
# 请勿用本脚本输出直接覆盖提交集权威文件（尤其 V1 技术文件与完整版样题）。
# 默认输出到 03_脚本与方案/_generated/（可复跑草稿目录）。
"""
印制电路制作工职业技能竞赛技术文件 - Word文档生成脚本（第二版）

第二版修订说明（2026-06-18）：
1. 第一章增加"竞赛方向说明"，明确无产线条件下的赛项定位
2. 第二章补充 GB/T 4588-2025、IPC-A-600M、T/CPCA 6043A-2023 等标准引用
3. 新增"选手需具备的能力"章节（第三章），原章节序号后移
4. 模块分值调整：理论30→25分，DFM 20→25分
5. PCB工程资料处理内部权重调整：减少布局布线，增加工程文件输出
6. 补充缺陷样板来源说明、竞赛与国家职业标准对应关系表
7. 补充阻焊开窗"单边"表述、线宽单位统一、Gerber层别命名修正
8. DFM审查表增加阻焊桥检查项、评分表增加模块小计行
9. TOC层级改为1-2级、增加命题方式说明
"""
import os
from docx import Document
from docx.shared import Pt, Cm, RGBColor
from docx.enum.text import WD_ALIGN_PARAGRAPH
from docx.enum.table import WD_TABLE_ALIGNMENT, WD_ALIGN_VERTICAL
from docx.oxml.ns import qn, nsdecls
from docx.oxml import parse_xml
from docx.oxml import OxmlElement

doc = Document()

# ============================================================
# 全局样式设置
# ============================================================
style = doc.styles['Normal']
style.font.name = '宋体'
style.font.size = Pt(12)  # 小四
style.element.rPr.rFonts.set(qn('w:eastAsia'), '宋体')
pf = style.paragraph_format
pf.line_spacing = Pt(28)  # 固定值28磅
pf.space_before = Pt(0)
pf.space_after = Pt(0)

# 页边距
for section in doc.sections:
    section.top_margin = Cm(2.8)
    section.bottom_margin = Cm(2.5)
    section.left_margin = Cm(3.0)
    section.right_margin = Cm(2.6)

# ============================================================
# 辅助函数
# ============================================================
def set_cell_shading(cell, color="D9D9D9"):
    # 设置单元格底纹颜色
    shading_elm = parse_xml(f'<w:shd {nsdecls("w")} w:fill="{color}"/>')
    cell._tc.get_or_add_tcPr().append(shading_elm)

def add_heading1(text):
    # 一级标题：使用 Heading 1 样式以支持原生目录
    p = doc.add_heading(text, level=1)
    # 自定义样式：黑体，三号，居中
    p.alignment = WD_ALIGN_PARAGRAPH.CENTER
    for run in p.runs:
        run.font.name = '黑体'
        run.font.size = Pt(16)  # 三号
        run.element.rPr.rFonts.set(qn('w:eastAsia'), '黑体')
    p.paragraph_format.space_before = Pt(12)
    p.paragraph_format.space_after = Pt(6)
    p.paragraph_format.line_spacing = Pt(28)
    return p

def add_heading2(text):
    # 二级标题：黑体，四号，左对齐
    p = doc.add_paragraph()
    p.alignment = WD_ALIGN_PARAGRAPH.LEFT
    run = p.add_run(text)
    run.bold = True
    run.font.name = '黑体'
    run.font.size = Pt(14)  # 四号
    run.element.rPr.rFonts.set(qn('w:eastAsia'), '黑体')
    p.paragraph_format.space_before = Pt(6)
    p.paragraph_format.space_after = Pt(3)
    p.paragraph_format.line_spacing = Pt(28)
    return p

def add_heading3(text):
    # 三级标题：黑体，小四
    p = doc.add_paragraph()
    p.alignment = WD_ALIGN_PARAGRAPH.LEFT
    run = p.add_run(text)
    run.bold = True
    run.font.name = '黑体'
    run.font.size = Pt(12)  # 小四
    run.element.rPr.rFonts.set(qn('w:eastAsia'), '黑体')
    p.paragraph_format.space_before = Pt(3)
    p.paragraph_format.space_after = Pt(2)
    p.paragraph_format.line_spacing = Pt(28)
    return p

def add_body(text, indent=False):
    # 正文段落
    p = doc.add_paragraph()
    p.alignment = WD_ALIGN_PARAGRAPH.JUSTIFY
    run = p.add_run(text)
    run.font.name = '宋体'
    run.font.size = Pt(12)
    run.element.rPr.rFonts.set(qn('w:eastAsia'), '宋体')
    p.paragraph_format.line_spacing = Pt(28)
    if indent:
        p.paragraph_format.first_line_indent = Cm(0.74)
    return p

def add_body_bold_prefix(prefix, text, indent=False):
    # 正文段落，前缀加粗
    p = doc.add_paragraph()
    p.alignment = WD_ALIGN_PARAGRAPH.JUSTIFY
    run1 = p.add_run(prefix)
    run1.bold = True
    run1.font.name = '宋体'
    run1.font.size = Pt(12)
    run1.element.rPr.rFonts.set(qn('w:eastAsia'), '宋体')
    run2 = p.add_run(text)
    run2.font.name = '宋体'
    run2.font.size = Pt(12)
    run2.element.rPr.rFonts.set(qn('w:eastAsia'), '宋体')
    p.paragraph_format.line_spacing = Pt(28)
    if indent:
        p.paragraph_format.first_line_indent = Cm(0.74)
    return p

def add_table(headers, rows, col_widths=None):
    # 创建表格，表头黑体浅灰底纹居中，内容宋体五号
    table = doc.add_table(rows=1 + len(rows), cols=len(headers))
    table.style = 'Table Grid'
    table.alignment = WD_TABLE_ALIGNMENT.CENTER

    # 表头
    for i, header in enumerate(headers):
        cell = table.rows[0].cells[i]
        cell.text = ''
        p = cell.paragraphs[0]
        p.alignment = WD_ALIGN_PARAGRAPH.CENTER
        run = p.add_run(header)
        run.bold = True
        run.font.name = '黑体'
        run.font.size = Pt(10.5)  # 五号
        run.element.rPr.rFonts.set(qn('w:eastAsia'), '黑体')
        set_cell_shading(cell, "D9D9D9")
        cell.vertical_alignment = WD_ALIGN_VERTICAL.CENTER

    # 数据行
    for r_idx, row in enumerate(rows):
        for c_idx, val in enumerate(row):
            cell = table.rows[r_idx + 1].cells[c_idx]
            cell.text = ''
            p = cell.paragraphs[0]
            p.alignment = WD_ALIGN_PARAGRAPH.CENTER
            run = p.add_run(str(val))
            run.font.name = '宋体'
            run.font.size = Pt(10.5)  # 五号
            run.element.rPr.rFonts.set(qn('w:eastAsia'), '宋体')
            cell.vertical_alignment = WD_ALIGN_VERTICAL.CENTER

    # 设置列宽
    if col_widths:
        for row in table.rows:
            for i, width in enumerate(col_widths):
                row.cells[i].width = Cm(width)

    return table

def add_page_break():
    doc.add_page_break()

def add_attachment_title(text):
    # 附件标题：分页 + 居中黑体三号
    add_page_break()
    p = doc.add_paragraph()
    p.alignment = WD_ALIGN_PARAGRAPH.CENTER
    run = p.add_run(text)
    run.bold = True
    run.font.name = '黑体'
    run.font.size = Pt(16)
    run.element.rPr.rFonts.set(qn('w:eastAsia'), '黑体')
    p.paragraph_format.space_after = Pt(12)
    return p

def add_page_field(paragraph, field_name):
    # 向段落中插入 Word 域代码（PAGE / NUMPAGES）
    run_begin = paragraph.add_run()
    run_begin._r.append(parse_xml(f'<w:fldChar {nsdecls("w")} w:fldCharType="begin"/>'))
    run_instr = paragraph.add_run()
    run_instr._r.append(parse_xml(f'<w:instrText {nsdecls("w")} xml:space="preserve"> {field_name} </w:instrText>'))
    run_end = paragraph.add_run()
    run_end._r.append(parse_xml(f'<w:fldChar {nsdecls("w")} w:fldCharType="end"/>'))

# ============================================================
# 封面
# ============================================================
for _ in range(6):
    doc.add_paragraph()

p = doc.add_paragraph()
p.alignment = WD_ALIGN_PARAGRAPH.CENTER
run = p.add_run('江西省”振兴杯”职业技能竞赛')
run.font.name = '黑体'
run.font.size = Pt(22)
run.element.rPr.rFonts.set(qn('w:eastAsia'), '黑体')
p.paragraph_format.line_spacing = Pt(36)

doc.add_paragraph()

p = doc.add_paragraph()
p.alignment = WD_ALIGN_PARAGRAPH.CENTER
run = p.add_run("印制电路制作工")
run.bold = True
run.font.name = '黑体'
run.font.size = Pt(36)
run.element.rPr.rFonts.set(qn('w:eastAsia'), '黑体')
p.paragraph_format.line_spacing = Pt(48)

p = doc.add_paragraph()
p.alignment = WD_ALIGN_PARAGRAPH.CENTER
run = p.add_run("技术文件")
run.bold = True
run.font.name = '黑体'
run.font.size = Pt(28)
run.element.rPr.rFonts.set(qn('w:eastAsia'), '黑体')
p.paragraph_format.line_spacing = Pt(42)

doc.add_paragraph()

p = doc.add_paragraph()
p.alignment = WD_ALIGN_PARAGRAPH.CENTER
run = p.add_run("工程资料处理与质量检测方向")
run.font.name = '黑体'
run.font.size = Pt(16)
run.element.rPr.rFonts.set(qn('w:eastAsia'), '黑体')
p.paragraph_format.line_spacing = Pt(28)

for _ in range(4):
    doc.add_paragraph()

info_lines = [
    ("主办单位：", "XXXX"),
    ("承办单位：", "XXXX"),
    ("技术支持：", "南云信息科技有限公司"),
    ("发布日期：", "2026年X月X日"),
]
for label, value in info_lines:
    p = doc.add_paragraph()
    p.alignment = WD_ALIGN_PARAGRAPH.CENTER
    run1 = p.add_run(label)
    run1.font.name = '宋体'
    run1.font.size = Pt(14)
    run1.element.rPr.rFonts.set(qn('w:eastAsia'), '宋体')
    run2 = p.add_run(value)
    run2.font.name = '宋体'
    run2.font.size = Pt(14)
    run2.element.rPr.rFonts.set(qn('w:eastAsia'), '宋体')
    run2.underline = True
    p.paragraph_format.line_spacing = Pt(28)

add_page_break()

# ============================================================
# 目录页
# ============================================================
p = doc.add_paragraph()
p.alignment = WD_ALIGN_PARAGRAPH.CENTER
run = p.add_run("目  录")
run.bold = True
run.font.name = '黑体'
run.font.size = Pt(22)
run.element.rPr.rFonts.set(qn('w:eastAsia'), '黑体')
p.paragraph_format.space_after = Pt(12)

# 插入 Word 原生 TOC 域代码
# 打开文档后需要右键点击目录 -> 更新域 -> 更新整个目录
p = doc.add_paragraph()
run = p.add_run()
fldChar_begin = OxmlElement('w:fldChar')
fldChar_begin.set(qn('w:fldCharType'), 'begin')
run._r.append(fldChar_begin)

run2 = p.add_run()
instrText = OxmlElement('w:instrText')
instrText.set(qn('xml:space'), 'preserve')
instrText.text = ' TOC \\o "1-2" \\h \\z \\u '
run2._r.append(instrText)

run3 = p.add_run()
fldChar_separate = OxmlElement('w:fldChar')
fldChar_separate.set(qn('w:fldCharType'), 'separate')
run3._r.append(fldChar_separate)

# 添加占位文本
run4 = p.add_run("（请右键此处 -> 更新域 -> 更新整个目录）")
run4.font.name = '宋体'
run4.font.size = Pt(12)
run4.font.color.rgb = RGBColor(128, 128, 128)

run5 = p.add_run()
fldChar_end = OxmlElement('w:fldChar')
fldChar_end.set(qn('w:fldCharType'), 'end')
run5._r.append(fldChar_end)

add_page_break()

# ============================================================
# 一、竞赛项目说明
# ============================================================
add_heading1("一、竞赛项目说明")

add_body("本赛项为印制电路制作工职业技能竞赛，竞赛方向为工程资料处理与质量检测。竞赛以《印制电路制作工国家职业技能标准（2019年版）》相关要求为依据，结合当前PCB数字化设计、工程文件输出、可制造性审查和印制电路板质量检测岗位能力设置竞赛内容。", indent=True)

add_body("本赛项面向从事或学习印制电路板设计、制造、检测相关工作的从业人员及在校学生，重点考查选手在PCB工程资料处理、制造规则应用、DFM审查和裸板缺陷检测方面的综合能力。", indent=True)

add_heading2("竞赛方向说明")
add_body("传统印制电路制作工竞赛依托制造产线设备，考核蚀刻、电镀、钻孔、丝印等制造工艺操作技能。本赛项在不具备产线设备的条件下，根据《印制电路制作工国家职业技能标准（2019年版）》中'工程资料处理''质量检测''工艺审查'等岗位能力要求，设置'工程资料处理与质量检测'竞赛方向。", indent=True)
add_body("该方向对应PCB行业中CAM工程师、品质工程师、工程审核等核心岗位，是印制电路制作工职业技能体系的重要组成部分。竞赛内容覆盖从设计文件输出、制造规则审查到裸板质量检验的全流程，不需要制造产线设备，仅需计算机和缺陷样板即可完成全部考核。", indent=True)

# 竞赛内容与国家职业标准对应关系表
add_body("本赛项四个竞赛模块与《印制电路制作工国家职业技能标准（2019年版）》工作内容的对应关系如下：", indent=True)

add_table(
    ["竞赛模块", "对应国家职业标准工作内容", "是否需要产线"],
    [
        ["理论知识", "基础知识、工艺流程、质量标准、安全规范", "不需要"],
        ["PCB工程资料处理", "工程资料输出、制造文件审核、设计规则应用", "不需要"],
        ["DFM与工艺文件", "可制造性审查、工艺规则制定、工艺文件编制", "不需要"],
        ["裸板缺陷检测", "产品检验、缺陷判定、质量分析", "不需要（需缺陷样板）"],
    ],
    col_widths=[4.0, 8.5, 3.0]
)

add_body_bold_prefix('竞赛名称：', '江西省"振兴杯"职业技能竞赛印制电路制作工赛项')
add_body_bold_prefix("竞赛工种：", "印制电路制作工")
add_body_bold_prefix("竞赛方向：", "工程资料处理与质量检测")
add_body_bold_prefix("参赛对象：", "从事或学习PCB设计、制造、检测相关工作的从业人员及在校学生")
add_body_bold_prefix("竞赛形式：", "个人赛，理论考试+实操考核")

# ============================================================
# 二、竞赛依据
# ============================================================
add_heading1("二、竞赛依据")

add_body("本赛项技术文件依据以下标准和规范制定：", indent=True)

refs = [
    "1. 《印制电路制作工国家职业技能标准（2019年版）》",
    "2. 《GB/T 4588-2025 单双面刚性印制板分规范》（2026年7月1日起实施，替代GB/T 4588.1-1996、GB/T 4588.2-1996）",
    "3. 《IPC-A-600M 印制板的可接受性》（2025年5月发布）",
    "4. 《T/CPCA 6043A-2023 单双面碳膜印制电路板》",
    "5. 《IPC-2221 印制板设计通用标准》",
    "6. 竞赛组委会相关规则与管理办法",
]
for ref in refs:
    add_body(ref)

add_body("特别说明：本赛项定位为PCB制造相关工程能力考核，不涉及完整电子产品设计流程，重点考查印制电路板工程资料处理、可制造性分析和质量检测等核心岗位技能。", indent=True)

# ============================================================
# 三、选手需具备的能力
# ============================================================
add_heading1("三、选手需具备的能力")

add_body("根据《印制电路制作工国家职业技能标准（2019年版）》相关要求，参加本赛项的选手应具备以下能力：", indent=True)

add_table(
    ["序号", "能力领域", "具体能力要求"],
    [
        ["1", "PCB设计基础", "掌握原理图绘制、元器件封装匹配、PCB布局布线基本方法"],
        ["2", "工程文件输出", "能够正确输出Gerber文件、NC Drill钻孔文件、钻孔表、DRC报告等制造文件"],
        ["3", "制造规则应用", "掌握线宽线距、孔径环宽、板边距、阻焊开窗等制造规则的设置与应用"],
        ["4", "DFM审查能力", "能够根据制造能力对设计进行可制造性审查，识别潜在制造问题"],
        ["5", "质量标准知识", "掌握IPC-A-600等标准中的缺陷分类、产品等级和验收判定方法"],
        ["6", "缺陷检测能力", "能够使用放大镜、万用表等工具对裸板进行外观检验和缺陷识别"],
        ["7", "工艺分析能力", "能够分析缺陷产生的工艺原因，提出合理的处理建议"],
        ["8", "安全与规范意识", "掌握生产安全规范，养成标准化作业习惯"],
    ],
    col_widths=[1.2, 3.5, 10.8]
)

doc.add_paragraph()

# ============================================================
# 四、竞赛目标
# ============================================================
add_heading1("四、竞赛目标")

goals = [
    "1. 考查选手PCB工程资料处理能力，包括原理图导入、封装匹配、规则设置和布局布线。",
    "2. 考查选手Gerber文件、钻孔文件、工艺文件输出能力，确保工程资料符合制造要求。",
    "3. 考查选手DFM检查和制造缺陷识别能力，能够发现并分析常见PCB制造缺陷。",
    "4. 考查选手安全、规范、质量意识，养成标准化作业习惯。",
    "5. 推动印制电路制作相关技能人才培养，促进产教融合和技能竞赛成果转化。",
]
for g in goals:
    add_body(g)

# ============================================================
# 五、竞赛内容与模块
# ============================================================
add_heading1("五、竞赛内容与模块")

add_body("竞赛分为四个模块，总分100分。具体如下：", indent=True)

add_table(
    ["模块", "内容", "分值"],
    [
        ["理论知识", "国家标准、PCB基础、安全规范、制造流程、质量缺陷", "25分"],
        ["PCB工程资料处理", "使用嘉立创EDA完成规则设置、布局布线、DRC检查、Gerber与钻孔文件输出", "30分"],
        ["DFM与工艺文件", "根据制造规则对设计进行全面DFM审查，检查线宽线距、孔径、环宽、板边距、阻焊开窗、定位孔、工艺边等，填写工艺审查记录", "25分"],
        ["裸板缺陷检测", "对裁判提供的缺陷样板进行检测、记录、原因分析和处理建议", "20分"],
    ],
    col_widths=[4.0, 9.5, 2.0]
)

doc.add_paragraph()

# ============================================================
# 六、竞赛方式与时间安排
# ============================================================
add_heading1("六、竞赛方式与时间安排")

add_body("竞赛采用个人赛方式进行，理论考试与实操考核相结合。", indent=True)
add_body("竞赛总时长为4小时（含检录与准备时间）。各模块时间分配由组委会在赛前统一公布，选手须在规定时间内完成全部竞赛任务。", indent=True)

# ============================================================
# 七、竞赛软件、设备与材料
# ============================================================
add_heading1("七、竞赛软件、设备与材料")

add_heading2("（一）软件环境")
add_body("1. 统一使用嘉立创EDA专业版客户端（参考版本：v3.2.x，以赛前公布为准）。")
add_body("2. 竞赛电脑预装嘉立创EDA软件、封装库、规则文件和题目包。")
add_body("3. 竞赛期间不得自行安装软件、插件或连接外部存储设备。")
add_body("4. 理论考试系统由组委会统一提供，支持机考或纸笔考试。")

add_heading2("（二）硬件环境")
add_body("1. 每名选手配备计算机一台、显示器一台、鼠标键盘一套。")
add_body("2. 计算机配置不低于：Intel i5处理器、8GB内存、256GB固态硬盘、1920×1080显示器。")
add_body("3. 组委会统一提供缺陷检测用样板（含10种以上常见缺陷类型，由组委会向PCB制造企业定制或收集含真实缺陷的报废板）及缺陷记录表。")

add_heading2("（三）选手自带工具")
add_body("缺陷检测环节所需工具由选手自行携带，组委会不统一提供。选手可自带以下工具：")
add_body("1. 放大镜、显微镜等观察工具。")
add_body("2. 万用表、游标卡尺、千分尺等测量工具。")
add_body("3. 其他常规检测辅助工具。")
add_body("注意：选手不得携带具有通信、存储、联网功能的电子设备（如手机、平板电脑、智能手表等），不得携带与竞赛无关的参考资料。裁判有权对选手携带的工具进行检查，不符合要求的工具不得带入赛场。")

add_heading2("（四）竞赛材料")
add_body("1. 统一提供原理图文件、封装库文件、板框文件。")
add_body("2. 统一提供制造规则文件、工艺要求说明。")
add_body("3. 统一提供缺陷样板（含10种以上常见缺陷类型，由组委会定制或收集）。")
add_body("4. 统一提供DFM审查表、缺陷检测记录表等空白表格。")

# ============================================================
# 八、竞赛任务说明
# ============================================================
add_heading1("八、竞赛任务说明")

add_heading2("（一）任务一：PCB工程资料处理")
add_body("选手根据题目提供的原理图、封装库、板框尺寸和制造规则，在立创EDA中完成PCB设计。具体要求如下：", indent=True)
add_body("1. 选手不得更改原理图电气连接，不得删除指定元件和接口。")
add_body("2. 选手需根据制造规则设置线宽、线距、过孔尺寸等参数。")
add_body("3. 选手需完成元器件布局，布局应合理、紧凑、符合制造要求。")
add_body("4. 选手需完成布线，布线应满足DRC检查要求，无严重错误。")
add_body("5. 选手需运行DRC检查，确保设计无严重违规。")

add_heading2("（二）任务二：工程文件输出")
add_body("选手需输出以下工程文件：", indent=True)
add_body("1. Gerber文件：包含所有层文件，命名规范，层别完整。")
add_body("2. NC Drill钻孔文件：钻孔文件完整，包含钻孔表。")
add_body("3. 钻孔表：包含孔径、数量、属性等信息。")
add_body("4. DRC报告：不得存在严重错误。")
add_body("5. PDF预览图：包含各层预览，便于制造核对。")

add_heading2("（三）任务三：DFM审查")
add_body("选手需根据制造规则，对设计进行DFM（可制造性设计）审查，具体检查项目包括：", indent=True)
add_body("1. 线宽、线距是否满足制造能力要求。")
add_body("2. 孔径、过孔尺寸是否在制造能力范围内。")
add_body("3. 焊盘环宽是否满足最小要求。")
add_body("4. 板边距是否满足制造和装配要求。")
add_body("5. 阻焊开窗是否正确，丝印方向是否规范。")
add_body("6. 定位孔、工艺边是否设置正确。")
add_body("7. 填写DFM审查表，记录检查结果和发现的问题。")

add_heading2("（四）任务四：裸板缺陷检测")
add_body("选手对裁判提供的缺陷板或缺陷图片进行检测，具体要求如下：", indent=True)
add_body("1. 识别缺陷类型（如开路、短路、缺口、针孔、偏位、阻焊不良等）。")
add_body("2. 准确标注缺陷位置。")
add_body("3. 分析缺陷可能产生的原因。")
add_body("4. 提出合理的处理建议。")
add_body("5. 填写缺陷检测记录表。")

# ============================================================
# 九、作品提交要求
# ============================================================
add_heading1("九、作品提交要求")

add_heading2("（一）文件夹结构")
add_body("选手须按以下统一文件夹结构提交作品：", indent=True)

folder_structure = [
    "选手编号_作品提交/",
    "├── 01_工程源文件/",
    "├── 02_Gerber文件/",
    "├── 03_NC_Drill文件/",
    "├── 04_DRC报告/",
    "├── 05_DFM审查表/",
    "├── 06_缺陷检测记录表/",
    "└── 07_作品说明.pdf",
]
for line in folder_structure:
    p = doc.add_paragraph()
    run = p.add_run(line)
    run.font.name = 'Consolas'
    run.font.size = Pt(10.5)
    p.paragraph_format.line_spacing = Pt(22)

add_heading2("（二）提交物清单")

add_table(
    ["序号", "文件", "要求"],
    [
        ["1", "工程源文件", "立创EDA工程文件（.epro格式）"],
        ["2", "Gerber文件", "层文件完整，命名规范，包含钻孔图"],
        ["3", "NC Drill文件", "钻孔文件完整，格式正确"],
        ["4", "钻孔表", "包含孔径、数量、属性等信息"],
        ["5", "DRC报告", "不得存在严重错误"],
        ["6", "DFM审查表", "填写完整，检查项目无遗漏"],
        ["7", "缺陷检测记录表", "缺陷类型、位置、原因、建议完整"],
        ["8", "作品说明", "PDF格式，简要说明设计思路和检查结果"],
    ],
    col_widths=[1.5, 4.0, 10.0]
)

doc.add_paragraph()
add_body("注意：提交截止后，选手不得再修改或补充任何文件。未按要求提交的作品，相关模块按零分处理。", indent=True)

# ============================================================
# 十、评分标准
# ============================================================
add_heading1("十、评分标准")

add_body("竞赛总分100分，各模块评分标准如下：", indent=True)

add_heading2("（一）理论知识（25分）")

add_table(
    ["序号", "评分项目", "分值", "评分说明"],
    [
        ["1", "国家标准与规范", "6分", "掌握GB/T 4588、IPC-A-600等标准相关知识"],
        ["2", "PCB基础理论", "5分", "掌握PCB材料、工艺流程、制造方法等基础知识"],
        ["3", "安全规范", "3分", "掌握生产安全、设备操作安全等规范"],
        ["4", "制造流程", "5分", "掌握PCB制造全流程及各工序要点"],
        ["5", "质量缺陷", "4分", "掌握常见PCB缺陷类型、成因及IPC-A-600判定标准"],
        ["6", "工程文件规范", "2分", "掌握Gerber、钻孔文件等工程输出规范"],
    ],
    col_widths=[1.5, 4.0, 1.5, 8.5]
)

doc.add_paragraph()
add_heading2("（二）PCB工程资料处理（30分）")

add_table(
    ["序号", "评分项目", "分值", "评分说明"],
    [
        ["1", "规则设置正确", "5分", "线宽、线距、过孔等规则设置符合制造要求"],
        ["2", "布局合理", "4分", "元器件布局紧凑合理，信号流向清晰"],
        ["3", "布线符合制造要求", "6分", "布线满足DRC，无严重违规，走线规范"],
        ["4", "DRC检查通过", "4分", "DRC报告无严重错误"],
        ["5", "Gerber/钻孔文件完整", "7分", "层文件完整，命名规范，钻孔文件正确"],
        ["6", "工程文件规范性", "4分", "PDF预览图完整、BOM清单准确、文件夹结构规范"],
    ],
    col_widths=[1.5, 5.0, 1.5, 7.5]
)

doc.add_paragraph()
add_heading2("（三）DFM与工艺文件（25分）")

add_table(
    ["序号", "评分项目", "分值", "评分说明"],
    [
        ["1", "线宽线距检查", "4分", "正确检查线宽线距是否满足制造能力"],
        ["2", "孔径与环宽检查", "4分", "正确检查孔径和焊盘环宽"],
        ["3", "板边距、定位孔检查", "3分", "正确检查板边距和定位孔设置"],
        ["4", "阻焊、丝印检查", "4分", "正确检查阻焊开窗、阻焊桥和丝印规范"],
        ["5", "工艺边与拼板检查", "3分", "正确检查工艺边宽度和拼板设计"],
        ["6", "工艺记录完整", "4分", "DFM审查表填写完整、规范，问题描述准确"],
        ["7", "整改建议合理性", "3分", "对不合格项提出的整改建议具有可操作性"],
    ],
    col_widths=[1.5, 5.0, 1.5, 7.5]
)

doc.add_paragraph()
add_heading2("（四）裸板缺陷检测（20分）")

add_table(
    ["序号", "评分项目", "分值", "评分说明"],
    [
        ["1", "缺陷识别准确", "8分", "正确识别缺陷类型，无漏检误检"],
        ["2", "缺陷位置标注准确", "4分", "缺陷位置标注清晰准确"],
        ["3", "原因分析合理", "4分", "缺陷原因分析符合制造工艺逻辑"],
        ["4", "处理建议合理", "4分", "提出的处理建议具有可操作性"],
    ],
    col_widths=[1.5, 5.0, 1.5, 7.5]
)

# ============================================================
# 十一、竞赛纪律与安全要求
# ============================================================
add_heading1("十一、竞赛纪律与安全要求")

add_heading2("（一）竞赛纪律")
add_body("1. 选手须持有效证件按时检录入场，迟到15分钟以上视为弃赛。")
add_body("2. 竞赛期间不得携带手机、U盘等电子设备进入赛场。")
add_body("3. 不得抄袭、协助他人或接受他人协助。")
add_body("4. 不得擅自更改竞赛设备、软件配置。")
add_body("5. 不得向场外传递任何竞赛信息。")
add_body("6. 违反竞赛纪律者，取消竞赛资格，成绩作废。")

add_heading2("（二）安全要求")
add_body("1. 选手须遵守赛场安全管理规定，服从裁判和工作人员指挥。")
add_body("2. 缺陷检测环节使用工具时注意安全，防止划伤、扎伤。")
add_body("3. 赛场内严禁吸烟、饮食。")
add_body("4. 发生紧急情况时，按赛场应急预案执行。")

# ============================================================
# 十二、裁判与仲裁规则
# ============================================================
add_heading1("十二、裁判与仲裁规则")

add_heading2("（一）裁判组成")
add_body("1. 设总裁判长1名，负责竞赛总体裁判工作。")
add_body("2. 设裁判员若干名，负责各模块的评判工作。")
add_body("3. 裁判员应具备相关专业背景和竞赛执裁经验。")

add_heading2("（二）评判方式")
add_body("1. 理论知识模块采用机考自动评分或人工流水阅卷。")
add_body("2. 实操模块采用裁判分项评分，每份作品至少由2名裁判独立评分。")
add_body("3. 评分结果取平均分，分差超过规定范围的由总裁判长裁定。")

add_heading2("（三）仲裁规则")
add_body("1. 设仲裁组，负责处理竞赛过程中的争议和申诉。")
add_body("2. 选手对成绩有异议的，须在成绩公布后1小时内以书面形式提出申诉。")
add_body("3. 仲裁组在收到申诉后2小时内作出裁决，裁决为最终结果。")

# ============================================================
# 十三、成绩计算与排名办法
# ============================================================
add_heading1("十三、成绩计算与排名办法")

add_body("1. 竞赛成绩由理论知识（25分）、PCB工程资料处理（30分）、DFM与工艺文件（25分）、裸板缺陷检测（20分）四个模块成绩相加得出，满分100分。", indent=True)
add_body("2. 总成绩相同的，按以下顺序优先排名：", indent=True)
add_body("  （1）实操成绩高者优先；")
add_body("  （2）PCB工程资料处理模块成绩高者优先；")
add_body("  （3）缺陷检测模块成绩高者优先；")
add_body("  （4）提交时间早者优先。")
add_body("3. 成绩计算保留小数点后两位，四舍五入。", indent=True)

# ============================================================
# 十四、赛场环境要求
# ============================================================
add_heading1("十四、赛场环境要求")

add_body("1. 赛场应光线充足、通风良好、温度适宜（22-26℃）。")
add_body("2. 每个工位面积不小于2平方米，工位间距不小于0.8米。")
add_body("3. 竞赛区域与观摩区域应有明显分隔。")
add_body("4. 赛场应配备UPS不间断电源，防止突然断电导致数据丢失。")
add_body("5. 赛场应覆盖无线网络（仅供竞赛系统使用），禁止选手自行联网。")
add_body("6. 赛场应配备监控设备，全程录像备查。")
add_body("7. 赛场应设置医疗急救点，配备基本急救药品和器材。")

# ============================================================
# 十五、技术规范说明
# ============================================================
add_heading1("十五、技术规范说明")

add_heading2("（一）PCB设计技术规范")
add_body("1. 最小线宽/线距：根据题目要求设定，一般不低于0.15mm（约6mil）。")
add_body("2. 最小孔径：根据题目要求设定，一般不低于0.3mm。")
add_body("3. 最小焊盘环宽：单边不小于0.15mm。")
add_body("4. 板边距：走线和焊盘距板边不小于0.3mm。")
add_body("5. 定位孔：按制造要求设置，一般为3.2mm非金属化孔。")

add_heading2("（二）文件输出规范")
add_body("1. Gerber文件格式：RS-274X。")
add_body("2. NC Drill格式：Excellon。")
add_body("3. 坐标单位：公制（mm）。")
add_body("4. 双层板应包含以下层别（以嘉立创EDA导出名称为准）：Top Layer（顶层线路）、Bottom Layer（底层线路）、Top Overlay（顶层丝印）、Bottom Overlay（底层丝印）、Top Solder Mask（顶层阻焊）、Bottom Solder Mask（底层阻焊）、Board Outline（板框）、NC Drill（钻孔文件）。")

add_heading2("（三）缺陷检测技术规范")
add_body("1. 缺陷类型参照IPC-A-600M《印制板的可接受性》标准。")
add_body("2. 常见缺陷类型包括但不限于：开路、短路、缺口、针孔、偏位、阻焊不良、丝印不良、翘曲、分层、起泡、露铜等。")
add_body("3. 缺陷严重程度分为：致命缺陷（影响电气功能）、严重缺陷（影响可靠性）、一般缺陷（影响外观但不影响功能）、轻微缺陷（可接受范围内的工艺偏差）。")
add_body("4. 缺陷判定应结合产品等级（Class 1/2/3）进行，竞赛默认按Class 2（高可靠性产品）标准判定。")

# ============================================================
# 十六、命题方式
# ============================================================
add_heading1("十六、命题方式")

add_body("1. 赛前公布竞赛样题，样题内容、题型、难度与正式赛题相当，供选手熟悉竞赛形式和要求。", indent=True)
add_body("2. 正式赛题在样题基础上进行变化，难度不低于样题，具体变化范围赛前不另行通知。", indent=True)
add_body("3. 理论知识模块从题库中随机抽取或由裁判组封闭命题。", indent=True)
add_body("4. 实操模块（PCB工程资料处理、DFM审查、缺陷检测）由裁判组统一命题，赛前保密。", indent=True)
add_body("5. 缺陷检测模块的缺陷样板由裁判组在赛前统一制备，确保每工位样板缺陷类型和数量一致。", indent=True)

# ============================================================
# 十七、附件
# ============================================================
add_heading1("十七、附件")

add_body("以下附件为本技术文件的组成部分，与正文具有同等效力：", indent=True)

attachments = [
    "附件1：选手报名表",
    "附件2：竞赛日程表",
    "附件3：设备与软件清单",
    "附件4：PCB制造规则表",
    "附件5：作品提交清单",
    "附件6：DFM审查表",
    "附件7：缺陷检测记录表",
    "附件8：裁判评分表",
    "附件9：安全与纪律承诺书",
    "附件10：赛场异常情况记录表",
]
for att in attachments:
    add_body(att)

# ============================================================
# 附件1：选手报名表
# ============================================================
add_attachment_title("附件1：选手报名表")

add_table(
    ["项目", "内容"],
    [
        ["姓名", ""],
        ["性别", ""],
        ["出生年月", ""],
        ["身份证号", ""],
        ["工作/学习单位", ""],
        ["联系电话", ""],
        ["电子邮箱", ""],
        ["参赛项目", "印制电路制作工"],
        ["竞赛方向", "工程资料处理与质量检测"],
        ["个人简历", ""],
        ["推荐单位意见", ""],
    ],
    col_widths=[4.0, 11.5]
)

# ============================================================
# 附件2：竞赛日程表
# ============================================================
add_attachment_title("附件2：竞赛日程表")

add_body("竞赛总时长为4小时，各阶段时间安排由组委会在赛前统一公布。竞赛流程如下：", indent=True)
doc.add_paragraph()

add_table(
    ["序号", "阶段", "内容"],
    [
        ["1", "赛前检录", "身份核验、抽签、入场"],
        ["2", "理论考试", "闭卷机考或纸笔考试"],
        ["3", "PCB工程资料处理", "立创EDA设计与文件输出"],
        ["4", "DFM与工艺文件", "工艺审查与记录表填写"],
        ["5", "裸板缺陷检测", "裸板缺陷判断与分析"],
        ["6", "文件提交", "按规定目录提交作品"],
        ["7", "裁判评分与成绩公布", "裁判评分后统一公布成绩"],
    ],
    col_widths=[1.5, 5.0, 9.0]
)

# ============================================================
# 附件3：设备与软件清单
# ============================================================
add_attachment_title("附件3：设备与软件清单")

add_body("一、组委会统一提供：")
add_table(
    ["序号", "设备/软件名称", "规格/版本", "数量", "备注"],
    [
        ["1", "计算机", "Intel i5/8GB/256GB SSD", "每工位1台", ""],
        ["2", "显示器", "24寸/1920×1080", "每工位1台", ""],
        ["3", "鼠标键盘", "标准套装", "每工位1套", ""],
        ["4", "嘉立创EDA专业版", "组委会统一版本", "预装", ""],
        ["5", "缺陷样板", "含10种以上缺陷", "每工位1套", "缺陷检测用"],
    ],
    col_widths=[1.2, 4.0, 4.0, 3.0, 3.3]
)

doc.add_paragraph()
add_body("二、选手自带工具（缺陷检测环节）：")
add_table(
    ["序号", "工具名称", "参考规格", "备注"],
    [
        ["1", "放大镜/显微镜", "10倍以上", "观察工具"],
        ["2", "万用表", "数字万用表", "测量工具"],
        ["3", "游标卡尺/千分尺", "0.02mm精度", "测量工具"],
        ["4", "其他常规检测工具", "—", "不得携带通信、联网设备"],
    ],
    col_widths=[1.2, 4.0, 4.0, 6.3]
)

# ============================================================
# 附件4：PCB制造规则表
# ============================================================
add_attachment_title("附件4：PCB制造规则表")

add_table(
    ["序号", "规则项目", "参数要求", "备注"],
    [
        ["1", "最小线宽", "≥0.15mm（约6mil）", "根据题目调整"],
        ["2", "最小线距", "≥0.15mm（约6mil）", "根据题目调整"],
        ["3", "最小孔径", "≥0.3mm", ""],
        ["4", "最小焊盘环宽", "单边≥0.15mm", ""],
        ["5", "板边距", "≥0.3mm", ""],
        ["6", "定位孔直径", "3.2mm", "非金属化孔"],
        ["7", "工艺边宽度", "≥5mm", ""],
        ["8", "阻焊开窗", "比焊盘单边大0.05-0.1mm", ""],
        ["9", "阻焊桥", "细间距引脚间设置阻焊桥", "QFP/SOP等"],
        ["10", "丝印线宽", "≥0.15mm", ""],
        ["11", "丝印距焊盘", "≥0.2mm", ""],
    ],
    col_widths=[1.2, 4.0, 5.0, 5.3]
)

# ============================================================
# 附件5：作品提交清单
# ============================================================
add_attachment_title("附件5：作品提交清单")

add_table(
    ["序号", "提交物", "是否提交", "备注"],
    [
        ["1", "工程源文件（.epro）", "□ 是  □ 否", ""],
        ["2", "Gerber文件", "□ 是  □ 否", ""],
        ["3", "NC Drill文件", "□ 是  □ 否", ""],
        ["4", "DRC报告", "□ 是  □ 否", ""],
        ["5", "DFM审查表", "□ 是  □ 否", ""],
        ["6", "缺陷检测记录表", "□ 是  □ 否", ""],
        ["7", "作品说明.pdf", "□ 是  □ 否", ""],
    ],
    col_widths=[1.5, 5.0, 4.0, 5.0]
)

doc.add_paragraph()
add_body("选手签名：____________    日期：____________")

# ============================================================
# 附件6：DFM审查表
# ============================================================
add_attachment_title("附件6：DFM审查表")

add_body("选手编号：____________    审查日期：____________")
doc.add_paragraph()

add_table(
    ["序号", "检查项目", "标准要求", "检查结果", "问题描述"],
    [
        ["1", "最小线宽", "≥0.15mm", "□合格 □不合格", ""],
        ["2", "最小线距", "≥0.15mm", "□合格 □不合格", ""],
        ["3", "最小孔径", "≥0.3mm", "□合格 □不合格", ""],
        ["4", "焊盘环宽", "单边≥0.15mm", "□合格 □不合格", ""],
        ["5", "板边距", "≥0.3mm", "□合格 □不合格", ""],
        ["6", "定位孔", "3.2mm非金属化", "□合格 □不合格", ""],
        ["7", "工艺边", "≥5mm", "□合格 □不合格", ""],
        ["8", "阻焊开窗", "比焊盘单边大0.05-0.1mm", "□合格 □不合格", ""],
        ["9", "阻焊桥", "细间距引脚间是否设置阻焊桥", "□合格 □不合格", ""],
        ["10", "丝印规范", "线宽≥0.15mm", "□合格 □不合格", ""],
        ["11", "丝印距焊盘", "≥0.2mm", "□合格 □不合格", ""],
    ],
    col_widths=[1.2, 3.0, 4.5, 3.0, 3.8]
)

doc.add_paragraph()
add_body("审查结论：□ 全部合格    □ 存在不合格项（需整改）")
add_body("审查人签名：____________")

# ============================================================
# 附件7：缺陷检测记录表
# ============================================================
add_attachment_title("附件7：缺陷检测记录表")

add_body("选手编号：____________    检测日期：____________")
doc.add_paragraph()

add_table(
    ["缺陷编号", "缺陷类型", "缺陷位置", "严重程度", "原因分析", "处理建议"],
    [
        ["1", "", "", "□致命 □严重 □一般 □轻微", "", ""],
        ["2", "", "", "□致命 □严重 □一般 □轻微", "", ""],
        ["3", "", "", "□致命 □严重 □一般 □轻微", "", ""],
        ["4", "", "", "□致命 □严重 □一般 □轻微", "", ""],
        ["5", "", "", "□致命 □严重 □一般 □轻微", "", ""],
        ["6", "", "", "□致命 □严重 □一般 □轻微", "", ""],
        ["7", "", "", "□致命 □严重 □一般 □轻微", "", ""],
        ["8", "", "", "□致命 □严重 □一般 □轻微", "", ""],
    ],
    col_widths=[1.8, 2.5, 2.5, 3.2, 2.8, 2.7]
)

doc.add_paragraph()
add_body("检测人签名：____________    日期：____________")

# ============================================================
# 附件8：裁判评分表
# ============================================================
add_attachment_title("附件8：裁判评分表")

add_body("选手编号：____________    裁判签名：____________")
doc.add_paragraph()

add_table(
    ["模块", "评分项目", "满分", "得分", "备注"],
    [
        ["理论知识", "国家标准与规范", "6", "", ""],
        ["", "PCB基础理论", "5", "", ""],
        ["", "安全规范", "3", "", ""],
        ["", "制造流程", "5", "", ""],
        ["", "质量缺陷", "4", "", ""],
        ["", "工程文件规范", "2", "", ""],
        ["", "模块小计", "25", "", ""],
        ["PCB资料处理", "规则设置正确", "5", "", ""],
        ["", "布局合理", "4", "", ""],
        ["", "布线符合制造要求", "6", "", ""],
        ["", "DRC检查通过", "4", "", ""],
        ["", "Gerber/钻孔文件完整", "7", "", ""],
        ["", "工程文件规范性", "4", "", ""],
        ["", "模块小计", "30", "", ""],
        ["DFM与工艺文件", "线宽线距检查", "4", "", ""],
        ["", "孔径与环宽检查", "4", "", ""],
        ["", "板边距、定位孔检查", "3", "", ""],
        ["", "阻焊、丝印检查", "4", "", ""],
        ["", "工艺边与拼板检查", "3", "", ""],
        ["", "工艺记录完整", "4", "", ""],
        ["", "整改建议合理性", "3", "", ""],
        ["", "模块小计", "25", "", ""],
        ["缺陷检测", "缺陷识别准确", "8", "", ""],
        ["", "缺陷位置标注准确", "4", "", ""],
        ["", "原因分析合理", "4", "", ""],
        ["", "处理建议合理", "4", "", ""],
        ["", "模块小计", "20", "", ""],
        ["合计", "", "100", "", ""],
    ],
    col_widths=[3.5, 5.0, 1.5, 2.0, 3.5]
)

doc.add_paragraph()
add_body("裁判长签名：____________    日期：____________")

# ============================================================
# 附件9：安全与纪律承诺书
# ============================================================
add_attachment_title("附件9：安全与纪律承诺书")

add_body("本人自愿参加本次印制电路制作工职业技能竞赛，郑重承诺如下：", indent=True)

promises = [
    "1. 本人已认真阅读并理解竞赛技术文件和各项规则。",
    "2. 本人保证所提交的个人信息真实有效。",
    "3. 本人承诺遵守竞赛纪律，不携带违禁物品进入赛场。",
    "4. 本人承诺独立完成竞赛任务，不抄袭、不协助他人。",
    "5. 本人承诺遵守赛场安全管理规定，注意人身安全。",
    "6. 本人承诺服从裁判和工作人员的管理和指挥。",
    "7. 如违反上述承诺，本人愿意接受取消竞赛成绩等处理。",
]
for pro in promises:
    add_body(pro)

doc.add_paragraph()
doc.add_paragraph()
add_body("承诺人签名：____________    日期：____________")
add_body("身份证号：____________")
add_body("联系电话：____________")

# ============================================================
# 附件10：赛场异常情况记录表
# ============================================================
add_attachment_title("附件10：赛场异常情况记录表")

add_table(
    ["序号", "项目", "内容"],
    [
        ["1", "异常发生时间", ""],
        ["2", "异常发生地点", ""],
        ["3", "涉及选手编号", ""],
        ["4", "异常情况描述", ""],
        ["5", "处理措施", ""],
        ["6", "处理结果", ""],
        ["7", "记录人签名", ""],
        ["8", "裁判长签名", ""],
        ["9", "备注", ""],
    ],
    col_widths=[1.5, 4.0, 10.0]
)

# ============================================================
# 页眉页脚（需要在最后设置，因为节的处理）
# ============================================================
# python-docx 页眉页脚设置
for section in doc.sections:
    # 页眉
    header = section.header
    header.is_linked_to_previous = False
    hp = header.paragraphs[0]
    hp.alignment = WD_ALIGN_PARAGRAPH.CENTER
    hr = hp.add_run('江西省"振兴杯"职业技能竞赛印制电路制作工技术文件')
    hr.font.name = '宋体'
    hr.font.size = Pt(9)
    hr.element.rPr.rFonts.set(qn('w:eastAsia'), '宋体')
    # 页眉下划线
    hp.paragraph_format.space_after = Pt(6)

    # 页脚 - "第 X 页 / 共 X 页"
    footer = section.footer
    footer.is_linked_to_previous = False
    fp = footer.paragraphs[0]
    fp.alignment = WD_ALIGN_PARAGRAPH.CENTER

    def add_footer_text(paragraph, text):
        r = paragraph.add_run(text)
        r.font.name = '宋体'
        r.font.size = Pt(9)
        r.element.rPr.rFonts.set(qn('w:eastAsia'), '宋体')

    add_footer_text(fp, "第 ")
    add_page_field(fp, "PAGE")
    add_footer_text(fp, " 页 / 共 ")
    add_page_field(fp, "NUMPAGES")
    add_footer_text(fp, " 页")

# ============================================================
# 保存文档
# ============================================================
# 草稿输出（勿覆盖 01_技术文件 权威 V1）
_SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
_GEN_DIR = os.path.join(_SCRIPT_DIR, "_generated")
os.makedirs(_GEN_DIR, exist_ok=True)
output_path = os.path.join(_GEN_DIR, "印制电路制作工职业技能竞赛技术文件_draft.docx")
doc.save(output_path)
print(f"文档已生成: {output_path}")
