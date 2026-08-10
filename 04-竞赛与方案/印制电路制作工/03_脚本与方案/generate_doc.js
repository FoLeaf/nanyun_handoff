// 历史生成器：勿覆盖 01/02 权威提交 docx。输出请落在 03_脚本与方案/_generated/
const fs = require("fs");
const { Document, Packer, Paragraph, TextRun, Table, TableRow, TableCell,
        Header, Footer, AlignmentType, HeadingLevel, BorderStyle, WidthType,
        ShadingType, PageBreak, PageNumber, TabStopType, TabStopPosition } = require("docx");

// Helper: create a table cell
function cell(text, opts = {}) {
  const { bold = false, font = "宋体", size = 24, align = AlignmentType.CENTER,
          color = "000000", colspan = 1, rowspan = 1, shading = null, width = null } = opts;
  const cellOpts = {
    borders: {
      top: { style: BorderStyle.SINGLE, size: 1, color: "000000" },
      bottom: { style: BorderStyle.SINGLE, size: 1, color: "000000" },
      left: { style: BorderStyle.SINGLE, size: 1, color: "000000" },
      right: { style: BorderStyle.SINGLE, size: 1, color: "000000" },
    },
    margins: { top: 60, bottom: 60, left: 100, right: 100 },
    verticalAlign: "center",
    children: [
      new Paragraph({
        alignment: align,
        spacing: { line: 300 },
        children: [new TextRun({ text, bold, font, size, color })],
      }),
    ],
  };
  if (colspan > 1) cellOpts.columnSpan = colspan;
  if (rowspan > 1) cellOpts.rowSpan = rowspan;
  if (shading) cellOpts.shading = { fill: shading, type: ShadingType.CLEAR };
  if (width) cellOpts.width = { size: width, type: WidthType.DXA };
  return new TableCell(cellOpts);
}

// Helper: body paragraph
function bodyPara(text, opts = {}) {
  const { bold = false, indent = 480, align = AlignmentType.JUSTIFIED, italic = false,
          font = "宋体", size = 28, color = "000000", spacing = { line: 360, after: 0 },
          pageBreakBefore = false } = opts;
  const children = [];
  // Handle mixed bold segments in text (using |bold:...| syntax)
  const parts = text.split(/(\|\|bold:[^|]+\|\|)/);
  for (const part of parts) {
    if (part.startsWith("||bold:")) {
      children.push(new TextRun({ text: part.slice(7, -2), bold: true, font, size, color, italic }));
    } else if (part) {
      children.push(new TextRun({ text: part, bold, font, size, color, italic }));
    }
  }
  if (children.length === 0) children.push(new TextRun({ text, bold, font, size, color, italic }));
  return new Paragraph({
    alignment: align,
    indent: { firstLine: indent },
    spacing,
    pageBreakBefore,
    children,
  });
}

// Helper: heading 1
function h1(text, opts = {}) {
  const { pageBreakBefore = true } = opts;
  return new Paragraph({
    heading: HeadingLevel.HEADING_1,
    alignment: AlignmentType.JUSTIFIED,
    spacing: { before: 120, after: 120, line: 360 },
    pageBreakBefore,
    children: [new TextRun({ text, bold: true, font: "黑体", size: 36 })],
  });
}

// Helper: heading 2
function h2(text) {
  return new Paragraph({
    heading: HeadingLevel.HEADING_2,
    alignment: AlignmentType.JUSTIFIED,
    spacing: { before: 0, after: 0, line: 360 },
    children: [new TextRun({ text, bold: true, font: "宋体", size: 32 })],
  });
}

// Helper: heading 3
function h3(text) {
  return new Paragraph({
    heading: HeadingLevel.HEADING_3,
    alignment: AlignmentType.JUSTIFIED,
    spacing: { before: 0, after: 0, line: 360 },
    children: [new TextRun({ text, bold: true, font: "宋体", size: 28 })],
  });
}

// Helper: empty line
function emptyLine() {
  return new Paragraph({ spacing: { line: 360 }, children: [] });
}

// Helper: table with header row
function makeTable(headers, rows, colWidths) {
  const tableWidth = colWidths.reduce((a, b) => a + b, 0);
  const headerRow = new TableRow({
    tableHeader: true,
    children: headers.map((h, i) => cell(h, { bold: true, size: 24, width: colWidths[i] })),
  });
  const dataRows = rows.map(row =>
    new TableRow({
      children: row.map((c, i) => cell(c, { size: 24, width: colWidths[i] })),
    })
  );
  return new Table({
    width: { size: tableWidth, type: WidthType.DXA },
    columnWidths: colWidths,
    rows: [headerRow, ...dataRows],
  });
}

// ==================== DOCUMENT CONTENT ====================

const children = [];

// ---- COVER PAGE ----
children.push(emptyLine());
children.push(emptyLine());
children.push(emptyLine());
children.push(new Paragraph({
  alignment: AlignmentType.CENTER,
  spacing: { after: 120, line: 400 },
  children: [new TextRun({ text: "XX市第X届职业技能大赛", font: "小标宋", size: 44, bold: true })],
}));
children.push(new Paragraph({
  alignment: AlignmentType.CENTER,
  spacing: { after: 200, line: 400 },
  children: [new TextRun({ text: "\u201C印制电路制作工\u201D项目", font: "黑体", size: 44, bold: true })],
}));
children.push(new Paragraph({
  alignment: AlignmentType.CENTER,
  spacing: { after: 120, line: 400 },
  children: [new TextRun({ text: "技术工作文件", font: "黑体", size: 44, bold: true })],
}));
children.push(emptyLine());
children.push(emptyLine());
children.push(emptyLine());
children.push(emptyLine());
children.push(new Paragraph({
  alignment: AlignmentType.CENTER,
  spacing: { line: 400 },
  children: [new TextRun({ text: "20XX年XX月XX日", font: "宋体", size: 36 })],
}));
children.push(new Paragraph({ children: [new PageBreak()] }));

// ---- TOC PAGE ----
children.push(emptyLine());
children.push(new Paragraph({
  alignment: AlignmentType.CENTER,
  spacing: { after: 200, line: 360 },
  children: [new TextRun({ text: "目  录", font: "宋体", size: 36 })],
}));
children.push(bodyPara("本项目技术工作文件（技术描述）是对本竞赛项目内容的框架性描述，正式比赛内容及要求以竞赛最终公布的赛题为准。", { indent: 0, size: 28, bold: false }));
children.push(emptyLine());

const tocItems = [
  ["1.项目简介", "1"], ["1.1项目描述", "1"], ["1.2考核目的", "1"], ["1.3 相关文件", "1"],
  ["2.基本能力与职业标准", "1"],
  ["3.竞赛内容", "2"], ["3.1 考核内容", "2"], ["3.2 竞赛模块", "2"], ["3.3 模块简述", "3"],
  ["3.3.1 模块A：EDA工程设计（续设计）", "3"], ["3.3.2 模块B：CAM审核与工艺文件编制", "3"],
  ["3.3.3 模块C：成品板质量检测与缺陷分析", "3"], ["3.3.4 模块D：电气测试与失效分析", "3"],
  ["3.4命题方式", "3"], ["3.5竞赛日程及地点安排", "4"],
  ["4.评分标准", "4"], ["4.1 评价分（主观）", "4"], ["4.2 测量分（客观）", "5"],
  ["4.3评分流程说明", "6"], ["4.4统分方法", "6"], ["4.5裁判构成和分组", "6"],
  ["4.5.1 裁判组", "6"], ["4.5.2 裁判任职条件", "6"], ["4.5.3 裁判长职责", "6"],
  ["4.5.4 裁判员职责", "7"], ["4.5.5 裁判评判工作及纪律要求", "7"], ["4.5.6 预期分组与分工方案", "8"],
  ["5.竞赛相关设施设备", "8"], ["5.1场地设备", "8"], ["5.2材料", "9"],
  ["5.3竞赛选手自备的设备和工具", "9"], ["5.4竞赛场地禁止自带使用的设备和材料", "9"],
  ["6.项目特别规定", "9"], ["7.赛场布局要求", "9"],
  ["8.健康安全和绿色环保", "10"], ["9.开放赛场", "10"],
];
tocItems.forEach(([title, page]) => {
  children.push(new Paragraph({
    alignment: AlignmentType.JUSTIFIED,
    spacing: { line: 360 },
    tabStops: [{ type: TabStopType.RIGHT, position: TabStopPosition.MAX }],
    children: [
      new TextRun({ text: title, font: "宋体", size: 28 }),
      new TextRun({ text: `\t${page}`, font: "宋体", size: 28 }),
    ],
  }));
});
children.push(new Paragraph({ children: [new PageBreak()] }));

// ========== CHAPTER 1: 项目简介 ==========
children.push(h1("1.项目简介"));

children.push(h2("1.1项目描述"));
children.push(bodyPara("印制电路制作工（职业编码6-25-01-13）是电子制造行业的基础工种，负责印制电路板（PCB）的设计、制造、检测与质量控制。本竞赛项目在无产线条件下，采用EDA设计、CAM审核和成品板检测相结合的考核方式，全面检验选手在印制电路设计、工艺审核和质量检测方面的综合能力。"));
children.push(bodyPara("竞赛要求选手掌握EDA软件操作、原理图续设计与PCB布局布线、Gerber文件审核与DFM缺陷识别、成品板缺陷检测与失效分析等核心技能。通过多维度、多层次的考核设计，反映选手的工程实践能力和问题解决能力。"));
children.push(bodyPara("该项目所对应的职业（工种）：印制电路制作工，电子CAD设计师。"));

children.push(h2("1.2考核目的"));
children.push(bodyPara("本次竞赛以《印制电路制作工国家职业技能标准（2019年版）》（人社厅发〔2019〕9号）为竞赛依据，参照世界技能大赛电子技术项目技术框架和第一届全国职业技能大赛组织模式，具体考核目的如下："));
children.push(bodyPara("（1）对标依据：以2019版国家职业技能标准为竞赛依据，职工组以技师（国家职业资格二级）标准为基础，学生组以高级工（国家职业资格三级）标准为基础，确保竞赛内容与职业标准紧密衔接。"));
children.push(bodyPara("（2）关键技能：竞赛内容选择PCB设计、CAM工艺审核和质量检测三项关键技能，全面覆盖印制电路制作工的核心职业功能，反映行业对复合型技能人才的需求。"));
children.push(bodyPara("（3）选手潜质：通过“续设计+缺陷识别+失效分析”多维度考核，反映选手的工程理解力、问题解决能力和创新思维，选拔具有发展潜力的技能人才。"));
children.push(bodyPara("（4）公平公正：采用DRC自动评分+缺陷标准答案为主、人工评审为辅的评分方式，最大限度减少主观因素影响，确保竞赛结果客观公正。"));

children.push(h2("1.3 相关文件"));
children.push(bodyPara("本项目技术工作文件只包含项目技术工作的相关信息。除阅读本文件外，开展本技能项目竞赛还需配合其他相关文件一同使用："));
children.push(bodyPara("——竞赛规则及实施细则；"));
children.push(bodyPara("——竞赛正式赛题（含模块A续设计原理图、模块B缺陷Gerber包、模块C/D缺陷板及配套电路图）；"));
children.push(bodyPara("——评分细则及评分表；"));
children.push(bodyPara("——EDA软件操作手册（立创EDA专业版）；"));
children.push(bodyPara("——IPC-2221 通用印制板设计标准（参考）；"));
children.push(bodyPara("——IPC-A-600 印制板验收条件（参考）；"));
children.push(bodyPara("——《印制电路制作工国家职业技能标准（2019年版）》；"));
children.push(bodyPara("——化学品安全数据表（MSDS）（如涉及）；"));
children.push(bodyPara("——赛场设备工具使用说明。"));

// ========== CHAPTER 2: 基本能力与职业标准 ==========
children.push(h1("2.基本能力与职业标准"));

children.push(bodyPara("本项目以《印制电路制作工国家职业技能标准（2019年版）》（人社厅发〔2019〕9号）为竞赛依据，全面引入国家职业技能标准和世界技能大赛组织模式。其中职工组以技师（国家职业资格二级）标准为基础，学生组以高级工（国家职业资格三级）标准为基础。竞赛内容覆盖印制电路制作工各核心职业功能模块的理论知识和实操技能要求，确保竞赛标准与行业实际需求相匹配。"));

children.push(bodyPara("根据本项目和组别的要求，以表格形式描述选手应完成的模块及在各模块中应具备的理论（应知）和工作能力（应会）要求。"));

// Table 1: 学生组能力要求
children.push(bodyPara("表1  学生组（高级工/三级）能力要求", { bold: true, size: 24, indent: 0 }));
children.push(makeTable(
  ["竞赛模块", "对应标准职业功能", "应具备的理论（应知）", "应具备的工作能力（应会）"],
  [
    ["A: EDA工程设计", "基础知识(2.2)\n电工基础、电子元件\n机械制图、质量管理", "电工基础、电子元件识别、机械制图基础、质量管理基本知识", "使用EDA软件完成原理图续设计和PCB布局布线、输出Gerber文件"],
    ["B: CAM审核与工艺文件编制", "光化学图形转移(1)\n光化学图形转移基本知识", "光化学图形转移基本知识、感光材料技术指标、制程工艺参数", "审核Gerber文件识别DFM缺陷、编制简版制程工艺卡"],
    ["C: 成品板质量检测与缺陷分析", "全部8个职业功能模块的检验要求", "各工序质量标准、缺陷类型与产生原因、检测方法与工具使用", "使用放大镜/显微镜检验板面状态、识别缺陷分类判定"],
  ],
  [1800, 2200, 2600, 2800]
));
children.push(emptyLine());

// Table 2: 职工组能力要求
children.push(bodyPara("表2  职工组（技师/二级）能力要求", { bold: true, size: 24, indent: 0 }));
children.push(makeTable(
  ["竞赛模块", "对应标准职业功能", "应具备的理论（应知）", "应具备的工作能力（应会）"],
  [
    ["A: EDA工程设计", "基础知识(2.2)\n同上+特殊工艺知识", "阻抗控制、高速信号走线、EMC/EMI设计原理", "完成含特殊设计要求的PCB布局布线"],
    ["B: CAM审核与工艺文件编制", "光化学图形转移(1)\n同上+工艺改善", "制程工艺参数优化、拼版设计、DFM分析方法", "审核复杂Gerber包、提出系统性改善方案"],
    ["C: 成品板质量检测与缺陷分析", "全部8个职业功能模块\n同上+失效分析", "缺陷根因分析方法、SPC统计过程控制、IPC标准", "识别缺陷+分析产生原因+提出改善对策"],
    ["D: 电气测试与失效分析", "图形制作(1.1.3)\n电路原理分析", "电路原理分析、互连失效机理、测试方法", "用万用表排查开路/短路故障点、分析互连失效原因"],
  ],
  [1800, 2200, 2600, 2800]
));
children.push(emptyLine());

children.push(bodyPara("基本职业道德与素质要求：选手应弘扬工匠精神，具有尽职尽责、精益求精的职业态度，具备安全意识、环保意识和规范化工程实践能力（对标标准2.1节）。"));

// ========== CHAPTER 3: 竞赛内容 ==========
children.push(h1("3.竞赛内容"));

children.push(h2("3.1 考核内容"));
children.push(bodyPara("本项目竞赛内容将理论知识融入技能考核过程中，不单独设置理论笔试环节。竞赛成绩实行百分制，总成绩由操作技能成绩加权合成，操作技能成绩权重为100%。全部模块均为实操考核，包括EDA工程设计、CAM审核与工艺文件编制、成品板质量检测与缺陷分析，职工组增加电气测试与失效分析模块。"));

children.push(h2("3.2 竞赛模块"));
children.push(bodyPara("学生组竞赛模块及分值分配如下：", { indent: 0 }));
children.push(makeTable(
  ["模块编号", "模块名称", "竞赛时间(min)", "评价分", "测量分", "合计"],
  [
    ["A", "EDA工程设计（续设计）", "120", "5", "30", "35"],
    ["B", "CAM审核与工艺文件编制", "90", "5", "20", "25"],
    ["C", "成品板质量检测与缺陷分析", "90", "5", "35", "40"],
    ["总计", "", "300", "15", "85", "100"],
  ],
  [1200, 2800, 1500, 1000, 1000, 1000]
));
children.push(emptyLine());

children.push(bodyPara("职工组竞赛模块及分值分配如下：", { indent: 0 }));
children.push(makeTable(
  ["模块编号", "模块名称", "竞赛时间(min)", "评价分", "测量分", "合计"],
  [
    ["A", "EDA工程设计（续设计）", "120", "5", "25", "30"],
    ["B", "CAM审核与工艺文件编制", "60", "5", "15", "20"],
    ["C", "成品板质量检测与缺陷分析", "60", "5", "25", "30"],
    ["D", "电气测试与失效分析", "60", "5", "15", "20"],
    ["总计", "", "300", "20", "80", "100"],
  ],
  [1200, 2800, 1500, 1000, 1000, 1000]
));
children.push(emptyLine());

children.push(bodyPara("评价分（主观）占比15%-20%，测量分（客观）占比80%-85%，符合电子类职业技能竞赛客观分占80%以上的一般规律。"));

children.push(h2("3.3 模块简述"));

children.push(h3("3.3.1 模块A：EDA工程设计（续设计）"));
children.push(bodyPara("选手使用指定EDA软件（立创EDA专业版），根据提供的一份已完成约70%的原理图（含核心子电路和主要IC），完成剩余外围电路的原理图绘制，并进行完整的PCB布局布线设计，最终输出Gerber加工文件。考核技术要点包括：原理图连接完整性与规范性、PCB布局的模块化分组与接口放置、布线质量（差分对等长、电源网络优化、高速信号走线规范）、设计规则设置与DRC零违规。评分以EDA软件DRC自动检查和网络表一致性校验为主（测量分），辅以人工评审布局合理性与布线策略专业性（评价分）。"));

children.push(h3("3.3.2 模块B：CAM审核与工艺文件编制"));
children.push(bodyPara("选手获得一套含8-10处预设缺陷的Gerber文件包，使用Gerber查看工具（Gerbv/ViewMate）逐一识别并标注问题，同时编制简版制程工艺卡。缺陷类型包括：线宽/间距不足、阻焊开窗错误、丝印覆盖焊盘、铜皮间距违规、钻孔偏移等DFM问题。工艺卡需包含关键工序参数和质量控制点。考核技术要点包括：Gerber缺陷识别准确性、缺陷分类正确性、工艺卡完整性和参数正确性。评分以缺陷识别数量和工艺卡质量为主（测量分），辅以审核报告规范性评价（评价分）。"));

children.push(h3("3.3.3 模块C：成品板质量检测与缺陷分析"));
children.push(bodyPara("选手获得一块含12-15处预设缺陷的成品PCB板，使用放大镜/体视显微镜、万用表和检测记录表，逐区域检查并记录缺陷位置、类型和严重度，给出合格/不合格判定。学生组侧重缺陷识别与分类判定，职工组增加缺陷原因分析和改善建议。考核技术要点包括：缺陷检出率、缺陷分类正确性、合格/不合格判定准确性。评分以缺陷检出数量和判定准确性为主（测量分），辅以检测报告规范性和分析深度评价（评价分）。"));

children.push(h3("3.3.4 模块D：电气测试与失效分析（仅职工组）"));
children.push(bodyPara("选手获得含开路/短路故障的PCB板、数字万用表和电路原理图，使用万用表逐点排查，定位故障点，分析失效原因，提出修复方案。考核技术要点包括：故障点定位准确性、失效原因分析正确性、修复方案可行性。评分以故障点定位和原因分析为主（测量分），辅以修复方案合理性评价（评价分）。"));

children.push(h2("3.4命题方式"));
children.push(bodyPara("本项目竞赛题采用“赛前修改部分参数的半公开命题”方式。具体如下："));
children.push(bodyPara("（1）赛前4周公布样题框架，包括模块A的电路功能说明、模块B的缺陷类型范围、模块C的缺陷板照片样例，供选手熟悉竞赛形式和软件环境。"));
children.push(bodyPara("（2）赛前由裁判长结合赛场条件，对正式赛题进行不超过30%的电路调整和缺陷替换，确保竞赛公平性和挑战性。"));
children.push(bodyPara("（3）正式赛题由裁判长签字确认，赛前向选手公布修改内容。"));
children.push(bodyPara("选择该方式的原因：EDA赛题完全保密对选手不公平（需要提前熟悉软件操作），完全公开又容易提前准备针对性训练，半公开是平衡公平性与保密性的合理方式。"));

children.push(h2("3.5竞赛日程及地点安排"));
children.push(bodyPara("竞赛总时长5小时（300分钟），具体日程安排如下："));
children.push(makeTable(
  ["时间", "内容", "地点"],
  [
    ["08:30-09:00", "检录、设备检查、选手签字确认", "赛场入口"],
    ["09:00-11:00", "模块A：EDA工程设计", "计算机教室"],
    ["11:00-11:10", "换场", "—"],
    ["11:10-12:40", "模块B：CAM审核（学生组）", "计算机教室"],
    ["12:40-13:40", "午休", "—"],
    ["13:40-15:10", "模块C：成品板检测（学生组）", "检测操作区"],
    ["11:10-12:10", "模块B：CAM审核（职工组）", "计算机教室"],
    ["12:10-13:10", "午休", "—"],
    ["13:10-14:10", "模块C：成品板检测（职工组）", "检测操作区"],
    ["14:10-15:10", "模块D：电气测试（职工组）", "检测操作区"],
    ["15:10-16:30", "裁判评分、成绩汇总", "评分室"],
    ["16:30-17:00", "技术点评、成绩公布", "会议室"],
  ],
  [2000, 4000, 2200]
));
children.push(emptyLine());
children.push(bodyPara("注：如学生组和职工组同日比赛且共用机房，需错开时段或分上/下午场。上述日程假设两组可分区同时进行。"));

// ========== CHAPTER 4: 评分标准 ==========
children.push(h1("4.评分标准"));
children.push(bodyPara("本项目评分标准分为测量和评价两类。凡可采用客观数据表述的评判称为测量；凡需要采用主观描述进行的评判称为评价。"));

children.push(h2("4.1 评价分（主观）"));
children.push(bodyPara("评价分（Judgement）打分方式：3名裁判为一组，各自单独评分，计算出平均权重分，除以3后再乘以该子项的分值计算出实际得分（四舍五入，保留小数点后两位）。裁判相互间分差必须小于等于1分，否则需要给出确切理由并在小组长或裁判长的监督下进行调分。"));
children.push(bodyPara("权重表如下："));
children.push(makeTable(
  ["权重分值", "要求描述"],
  [
    ["0分", "各方面均低于行业标准，包括“未做尝试”"],
    ["1分", "达到行业标准"],
    ["2分", "达到行业标准，且某些方面超过标准"],
    ["3分", "达到行业期待的优秀水平"],
  ],
  [1500, 7700]
));
children.push(emptyLine());

children.push(bodyPara("模块A评价维度：", { bold: true }));
children.push(makeTable(
  ["评价子项", "0分", "1分", "2分", "3分"],
  [
    ["布局合理性", "无模块化分组，器件随意放置", "有基本分组，主要接口靠边", "模块化清晰，功能器件就近放置，整体合理", "布局专业美观，信号路径最短化，EMC考量充分"],
    ["布线策略", "走线杂乱，无设计策略", "走线基本整齐，有基本布线规范", "差分对等长、电源网络加宽，策略明确", "高速信号走线规范、回流路径优化、整体专业水准"],
  ],
  [1400, 2000, 2000, 2000, 2000]
));
children.push(emptyLine());

children.push(bodyPara("模块C评价维度（职工组增加）：", { bold: true }));
children.push(makeTable(
  ["评价子项", "0分", "1分", "2分", "3分"],
  [
    ["缺陷分析报告", "未提交或无分析", "识别缺陷但分析浅显", "分析到位，能关联到工序原因", "分析深入，提出系统性改善建议"],
  ],
  [1400, 2000, 2000, 2000, 2000]
));
children.push(emptyLine());

children.push(h2("4.2 测量分（客观）"));
children.push(bodyPara("测量分（Measurement）打分方式：按模块设置若干个评分组，每组由3名及以上裁判构成。每个组所有裁判一起商议，对该选手在该项中的实际得分达成一致后最终只给出一个分值。"));

children.push(bodyPara("模块A测评表：", { bold: true }));
children.push(makeTable(
  ["测评内容", "项目特征描述", "配分", "标准值", "测量值", "得分"],
  [
    ["DRC违规数", "设计规则检查报错数", "10", "0", "N", "10-N×2（最低0）"],
    ["网络表一致性", "原理图-PCB连接匹配度", "8", "0错误", "N", "8-N×2（最低0）"],
    ["封装正确性", "新建封装尺寸/焊盘合规", "5", "全部正确", "N错误", "5-N×1（最低0）"],
    ["Gerber完整性", "输出文件层数/格式完整", "4", "全部完整", "缺N层", "4-N×1"],
    ["设计规则设置", "最小线宽/间距/过孔符合工艺要求", "3", "全部设置正确", "N项未设", "3-N×1"],
  ],
  [1500, 2200, 800, 1200, 1000, 1800]
));
children.push(emptyLine());

children.push(bodyPara("模块B测评表：", { bold: true }));
children.push(makeTable(
  ["测评内容", "配分", "标准值", "得分规则"],
  [
    ["Gerber缺陷识别", "12", "共8-10处缺陷", "每正确识别1处得1.5分"],
    ["误报扣分", "—", "0", "每误报1处扣1分（从缺陷分中扣）"],
    ["工艺卡完整性", "5", "包含全部关键工序", "缺1项扣1分"],
    ["工艺参数正确性", "3", "参数值在合理范围", "每错1项扣1分"],
  ],
  [2000, 1200, 2400, 3600]
));
children.push(emptyLine());

children.push(bodyPara("模块C测评表：", { bold: true }));
children.push(makeTable(
  ["测评内容", "配分", "标准值", "得分规则"],
  [
    ["缺陷检出数", "25", "共12-15处缺陷", "每正确识别1处得2分（上限25）"],
    ["缺陷分类正确性", "5", "类型判断正确", "每错1类扣1分"],
    ["合格/不合格判定", "5", "判定正确", "正确得5分，错误得0分"],
    ["误报扣分", "—", "0", "每误报1处扣2分"],
  ],
  [2000, 1200, 2400, 3600]
));
children.push(emptyLine());

children.push(bodyPara("模块D测评表（仅职工组）：", { bold: true }));
children.push(makeTable(
  ["测评内容", "配分", "标准值", "得分规则"],
  [
    ["故障点定位", "8", "正确定位全部故障点", "每正确定位1处得4分"],
    ["失效原因分析", "4", "原因分析正确", "正确得4分，部分正确得2分"],
    ["修复方案", "3", "方案可行", "可行得3分，基本可行得1分"],
  ],
  [2000, 1200, 2400, 3600]
));
children.push(emptyLine());

children.push(bodyPara("4.2.1 测评点", { bold: true, size: 28 }));
children.push(bodyPara("客观测评点的确定方式：模块A由EDA软件DRC引擎自动判定；模块B/C/D由裁判组根据标准答案进行判定。测评点随评分细则一同公布。"));

children.push(bodyPara("4.2.2 测评工具", { bold: true, size: 28 }));
children.push(bodyPara("——模块A：EDA软件DRC功能、Gerber查看器；"));
children.push(bodyPara("——模块B：Gerber查看器（Gerbv/ViewMate）；"));
children.push(bodyPara("——模块C：游标卡尺/千分尺、体视显微镜、标准缺陷答案图；"));
children.push(bodyPara("——模块D：数字万用表、电路原理图。"));

children.push(h2("4.3评分流程说明"));
children.push(bodyPara("（1）模块A、B采用结果评分方式，选手提交文件后由裁判组统一评判。"));
children.push(bodyPara("（2）模块C、D采用过程+结果评分方式，裁判巡视记录操作规范性，同时对检测记录表结果进行评判。"));
children.push(bodyPara("（3）主观评判先于客观评判进行。"));
children.push(bodyPara("（4）成绩并列处理：先比较模块C得分（分值最大/区分度最高），再比较模块A得分，再比较模块B/D得分。"));

children.push(h2("4.4统分方法"));
children.push(bodyPara("裁判长复核统分流程：各评分小组完成评分并签字→裁判长复核→工作人员录入系统→裁判长签字公布最终成绩。"));

children.push(h2("4.5裁判构成和分组"));
children.push(bodyPara("建议裁判总数：7-9人，具体构成如下："));

children.push(bodyPara("4.5.1 裁判组", { bold: true, size: 28 }));
children.push(bodyPara("——裁判长1人（PCB行业资深专家或高校教授）；"));
children.push(bodyPara("——EDA评分组：3人（负责模块A主观+客观验证）；"));
children.push(bodyPara("——CAM/文件评分组：2人（负责模块B）；"));
children.push(bodyPara("——检测评分组：3人（负责模块C+D，裁判长可兼任此组组长）。"));

children.push(bodyPara("4.5.2 裁判任职条件", { bold: true, size: 28 }));
children.push(bodyPara("裁判应具备电子/微电子相关专业中级以上职称或PCB行业5年以上从业经验，熟悉EDA软件和CAM工具操作，了解印制电路制造工艺和质量检测标准。"));

children.push(bodyPara("4.5.3 裁判长职责", { bold: true, size: 28 }));
children.push(bodyPara("裁判长负责竞赛技术工作的全面组织与协调，包括：审核竞赛技术文件和赛题、组织裁判培训和分工、监督评分过程的公正性、处理竞赛中的技术争议、签字确认最终成绩。"));

children.push(bodyPara("4.5.4 裁判员职责", { bold: true, size: 28 }));
children.push(bodyPara("裁判员负责按照分工完成各模块的评分工作，包括：赛前参加培训和试评、竞赛过程中巡视记录、赛后按标准进行评分、参与评分讨论和统分。"));

children.push(bodyPara("4.5.5 裁判评判工作及纪律要求", { bold: true, size: 28 }));
children.push(bodyPara("裁判应严格遵守竞赛纪律，做到公平公正评判。评分过程中不得与选手进行非必要交流，不得泄露评分标准和赛题信息，不得接受选手或相关人员的请托。违反纪律者取消裁判资格。"));

children.push(bodyPara("4.5.6 预期分组与分工方案", { bold: true, size: 28 }));
children.push(bodyPara("根据竞赛模块设置，裁判分组及分工如下："));
children.push(makeTable(
  ["评分组", "人数", "负责模块", "评分方式"],
  [
    ["EDA评分组", "3人", "模块A（主观+客观验证）", "结果评分"],
    ["CAM/文件评分组", "2人", "模块B", "结果评分"],
    ["检测评分组", "3人", "模块C+模块D", "过程+结果评分"],
  ],
  [2200, 1200, 2800, 2800]
));

// ========== CHAPTER 5: 竞赛相关设施设备 ==========
children.push(h1("5.竞赛相关设施设备"));

children.push(h2("5.1场地设备"));
children.push(bodyPara("每人配备以下场地设备："));
children.push(makeTable(
  ["序号", "设备名称", "型号/规格（建议）", "单位", "数量", "备注"],
  [
    ["1", "计算机工作站", "i5以上/16GB/SSD/24寸显示器", "台", "1", "预装立创EDA+Gerbv"],
    ["2", "体视显微镜", "10-40倍可调，带LED光源", "台", "1", "模块C/D用"],
    ["3", "数字万用表", "Fluke 15B+或同档", "台", "1", "模块C/D用"],
    ["4", "游标卡尺", "0-150mm，精度0.02mm", "把", "1", "模块C用"],
    ["5", "放大镜", "10倍带灯", "个", "1", "模块C辅助"],
  ],
  [600, 1800, 2600, 600, 600, 2800]
));
children.push(emptyLine());

children.push(bodyPara("成本估算：电脑利用学校现有机房（¥0增量），显微镜约¥500-2000/台，万用表约¥200-400/台，游标卡尺约¥50-100/把，放大镜约¥20-50/个。单工位增量设备成本约¥800-2500。20工位总计约¥1.6-5万。"));

children.push(h2("5.2材料"));
children.push(bodyPara("每人配备以下材料："));
children.push(makeTable(
  ["序号", "材料名称", "规格", "单位", "数量"],
  [
    ["1", "缺陷成品PCB板（模块C）", "统一制板，含12-15处预设缺陷", "块", "1"],
    ["2", "故障PCB板（模块D，职工组）", "统一制板，含2处开路/短路", "块", "1"],
    ["3", "检测记录表", "A4打印", "份", "2"],
    ["4", "制程工艺卡模板（模块B）", "A4打印", "份", "1"],
  ],
  [600, 2400, 3000, 600, 600]
));
children.push(emptyLine());

children.push(bodyPara("缺陷板制备建议：联系PCB工厂按设计文件制作两批板——良品板5块（裁判标定用+备用），缺陷板N+5块（选手用+备用）。缺陷通过修改Gerber文件后制板实现（如故意缩小间距、偏移钻孔、删除阻焊开窗等），每块板缺陷位置和类型一致。预估制板成本：¥20-50/块，30块约¥600-1500。"));

children.push(h2("5.3竞赛选手自备的设备和工具"));
children.push(makeTable(
  ["序号", "名称", "备注"],
  [
    ["1", "防静电手环", "建议自备"],
    ["2", "计算器", "允许使用"],
  ],
  [600, 3000, 5600]
));
children.push(emptyLine());

children.push(h2("5.4竞赛场地禁止自带使用的设备和材料"));
children.push(makeTable(
  ["序号", "名称"],
  [
    ["1", "预制Gerber文件/PCB设计文件/元件库"],
    ["2", "存储设备（U盘/移动硬盘）"],
    ["3", "通信设备（手机/平板）"],
    ["4", "未经审核的第三方软件/插件"],
  ],
  [600, 8600]
));

// ========== CHAPTER 6: 项目特别规定 ==========
children.push(h1("6.项目特别规定"));

children.push(bodyPara("（1）工具箱检查规定：赛前检查选手自带工具，确认无预制文件、存储设备和通信设备。"));
children.push(bodyPara("（2）赛题语种：中文。"));
children.push(bodyPara("（3）软件环境规定：赛场统一安装立创EDA专业版和Gerbv/ViewMate，选手不得安装其他软件；允许使用软件内置功能，禁止使用外部脚本/插件。"));
children.push(bodyPara("（4）缺陷板操作规定：模块C/D的缺陷板不得进行任何物理修改（不得刮铜、焊接、修补），仅限观察和测量；违反者取消该模块成绩。"));
children.push(bodyPara("（5）技术违规处罚："));
children.push(bodyPara("——使用预制文件：取消该模块成绩；"));
children.push(bodyPara("——使用通信/存储设备：取消项目成绩；"));
children.push(bodyPara("——修改缺陷板物理状态：取消该模块成绩；"));
children.push(bodyPara("——干扰其他选手：首次警告扣5分，再次取消项目成绩。"));

// ========== CHAPTER 7: 赛场布局要求 ==========
children.push(h1("7.赛场布局要求"));

children.push(bodyPara("竞赛赛场按功能分区布置，具体要求如下："));
children.push(bodyPara("（1）计算机区（机房）：用于模块A+B竞赛，每个工位一台电脑，工位间距≥1.2m，配备独立电源和网络接口。"));
children.push(bodyPara("（2）检测区：用于模块C+D竞赛，每个工位配备显微镜、万用表和缺陷板，配备标准照明（≥500lux），环境温度20-26℃。"));
children.push(bodyPara("（3）裁判区：评分室，与赛场隔离，配备评分用计算机和打印设备。"));
children.push(bodyPara("（4）候赛区/观摩区：选手等候和观众观摩区域，与赛场隔离。"));
children.push(bodyPara("建议：如学校机房和电子实训室相邻，可直接使用。计算机区和检测区可以在同一间大教室分区布置（电脑在一侧、检测台在另一侧），减少换场时间。"));

// ========== CHAPTER 8: 健康安全和绿色环保 ==========
children.push(h1("8.健康安全和绿色环保"));

children.push(bodyPara("本项目不涉及化学品操作（无桌面制版环节），安全要求大幅简化："));
children.push(bodyPara("安全规定："));
children.push(bodyPara("——用电安全：计算机设备接地良好，禁止私接电源；"));
children.push(bodyPara("——设备操作安全：显微镜轻拿轻放，万用表正确选择量程；"));
children.push(bodyPara("——紧急通道：赛场设置不少于2个紧急出口，保持畅通。"));
children.push(bodyPara("环保要求："));
children.push(bodyPara("——缺陷板赛后统一回收，交由PCB工厂按电子废弃物规范处理；"));
children.push(bodyPara("——竞赛用纸双面打印；"));
children.push(bodyPara("——计算机设备赛后正常关机，节约用电。"));
children.push(bodyPara("对比优势：相比含桌面制板的方案，本方案无化学品风险、无废液处理需求、无通风排风要求，安全合规成本几乎为零。"));

// ========== CHAPTER 9: 开放赛场 ==========
children.push(h1("9.开放赛场"));

children.push(bodyPara("（1）计算机区为封闭赛场（屏幕内容涉及赛题，防止泄题），禁止非参赛人员进入。"));
children.push(bodyPara("（2）检测区在不影响选手的前提下，允许观众在隔离带外观看（距离≥2m）。"));
children.push(bodyPara("（3）摄影/录像须经组委会批准，不得使用闪光灯，不得进入选手工位范围。"));

// ==================== BUILD DOCUMENT ====================

const doc = new Document({
  styles: {
    default: {
      document: {
        run: { font: "宋体", size: 28 },  // 14pt
      },
    },
    paragraphStyles: [
      {
        id: "Heading1", name: "Heading 1", basedOn: "Normal", next: "Normal", quickFormat: true,
        run: { size: 36, bold: true, font: "黑体" },
        paragraph: { spacing: { before: 120, after: 120 }, outlineLevel: 0 },
      },
      {
        id: "Heading2", name: "Heading 2", basedOn: "Normal", next: "Normal", quickFormat: true,
        run: { size: 32, bold: true, font: "宋体" },
        paragraph: { spacing: { before: 0, after: 0 }, outlineLevel: 1 },
      },
      {
        id: "Heading3", name: "Heading 3", basedOn: "Normal", next: "Normal", quickFormat: true,
        run: { size: 28, bold: true, font: "宋体" },
        paragraph: { spacing: { before: 0, after: 0 }, outlineLevel: 2 },
      },
    ],
  },
  sections: [{
    properties: {
      page: {
        size: { width: 11906, height: 16838 },  // A4
        margin: { top: 1440, right: 1440, bottom: 1440, left: 1440 },
      },
    },
    headers: {
      default: new Header({
        children: [new Paragraph({
          alignment: AlignmentType.CENTER,
          children: [new TextRun({ text: "XX市第X届职业技能大赛\u201c印制电路制作工\u201c项目技术工作文件", font: "宋体", size: 18, color: "808080" })],
        })],
      }),
    },
    footers: {
      default: new Footer({
        children: [new Paragraph({
          alignment: AlignmentType.CENTER,
          children: [
            new TextRun({ text: "第 ", font: "宋体", size: 20 }),
            new TextRun({ children: [PageNumber.CURRENT], font: "宋体", size: 20 }),
            new TextRun({ text: " 页", font: "宋体", size: 20 }),
          ],
        })],
      }),
    },
    children,
  }],
});

// Generate file — 输出到 _generated/，禁止覆盖 01/02 权威提交集
Packer.toBuffer(doc).then(buffer => {
  const path = require("path");
  const genDir = path.join(__dirname, "_generated");
  if (!fs.existsSync(genDir)) {
    fs.mkdirSync(genDir, { recursive: true });
  }
  const outPath = path.join(genDir, "印制电路制作工竞赛技术工作文件_draft.docx");
  fs.writeFileSync(outPath, buffer);
  console.log("Document created: " + outPath);
  console.log("Size: " + (buffer.length / 1024).toFixed(1) + " KB");
});
