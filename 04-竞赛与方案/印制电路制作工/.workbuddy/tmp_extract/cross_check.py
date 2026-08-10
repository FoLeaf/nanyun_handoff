#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import os, re, zipfile
import docx, openpyxl

ROOT = r"D:/nanyun/04-竞赛与方案/印制电路制作工/02_样题"

# 期望骨架：子目录 -> 期望包含的文件（用子串匹配，* 表示通配）
EXPECT = {
    "01竞赛要求": ["竞赛样题（完整版）.docx"],
    "02竞赛素材/0201竞赛素材": ["模块B.eprj2", "模块D.eprj2", ("模块Cgerber.zip", "模块C.zip")],
    "02竞赛素材/0202答题卡（需打印）": ["模块C_PCB制程工艺卡.docx", "模块C_缺陷记录表.docx", "模块D_检测记录表.docx"],
    "03结果呈现": ["模块C-01PCB制程工艺卡（参考答案）.docx", "模块C-02缺陷记录表（参考答案）.docx", "模块D检测记录表（参考答案）.docx"],
    "04评分标准": [("评分标准（第一套）.xlsx", "评分标准（第二套）.xlsx", "评分标准（第三套）.xlsx")],
}

def set_num(folder_name):
    m = re.search(r"第[一二三四五六七八九十]+套", folder_name)
    return m.group(0) if m else folder_name

def list_files(set_dir):
    out = {}
    for dp, dn, fn in os.walk(set_dir):
        rel = os.path.relpath(dp, set_dir).replace("\\", "/")
        if rel == ".":
            rel = ""
        for f in fn:
            out.setdefault(rel, []).append(f)
    return out

def check_expect(set_dir, rel, wants):
    actual = list_files(set_dir).get(rel, [])
    actual_set = set(actual)
    results = []
    for w in wants:
        opts = w if isinstance(w, tuple) else (w,)
        hit = next((o for o in opts if any(o in a for a in actual)), None)
        results.append((opts, hit))
    return actual, results

def docx_title(path):
    try:
        z = zipfile.ZipFile(path)
        t = re.sub(r"<[^>]+>", "", z.read("word/document.xml").decode("utf-8", "ignore"))
        m = re.search(r"竞赛样题（第[一二三四五六七八九十]+套）", t)
        return m.group(0) if m else "(未找到套次标注)"
    except Exception as e:
        return f"(读取失败:{e})"

def xlsx_note_set(path):
    try:
        wb = openpyxl.load_workbook(path, data_only=False)
        ws = wb.active
        for r in ws.iter_rows():
            for c in r:
                if isinstance(c.value, str) and "样题配套" in c.value:
                    m = re.search(r"第[一二三四五六七八九十]+套样题配套", c.value)
                    return m.group(0) if m else "(说明未标套次)"
        return "(无说明单元格)"
    except Exception as e:
        return f"(读取失败:{e})"

sets = []
for name in ["第一套", "第二套", "第三套"]:
    d = os.path.join(ROOT, name)
    if os.path.isdir(d):
        sets.append((name, d))
# 领导更正版本/第一套 作为参考来源
ld = os.path.join(ROOT, "领导更正版本", "第一套")
if os.path.isdir(ld):
    sets.append(("领导更正版本/第一套", ld))

report = []
report.append("# 样题完整性交叉检查报告")
report.append("")
report.append(f"检查根目录：`{ROOT}`")
report.append("")

# 1) 结构骨架
report.append("## 一、目录骨架与必备文件（✔=存在，✘=缺失）")
report.append("")
for name, d in sets:
    sn = set_num(name)
    report.append(f"### {name}  （应标注为：{sn}）")
    files = list_files(d)
    # 目录骨架
    for sub in EXPECT:
        present = sub in files
        mark = "✔" if present else "✘"
        actual = files.get(sub, [])
        report.append(f"- 目录 `{sub}`：{mark}  实际文件：{actual if actual else '（空/缺失）'}")
    # 必备文件
    report.append("  必备文件核对：")
    for sub, wants in EXPECT.items():
        actual, res = check_expect(d, sub, wants)
        for opts, hit in res:
            optstr = "/".join(opts)
            mark = "✔" if hit else "✘"
            report.append(f"    - [{mark}] `{sub}` 需含 `{optstr}`  -> 命中：`{hit or '无'}`")
    # 文件非空
    report.append("  文件大小（字节）：")
    for sub in sorted(files):
        for f in sorted(files[sub]):
            fp = os.path.join(d, sub, f)
            sz = os.path.getsize(fp) if os.path.isfile(fp) else -1
            flag = "⚠空" if sz == 0 else ("⚠异常" if sz < 0 else "")
            report.append(f"    - {sub}/{f} : {sz} {flag}")
    report.append("")

# 2) 内部标注自洽
report.append("## 二、文档内部套次标注自洽性")
report.append("")
for name, d in sets:
    sn = set_num(name)
    report.append(f"### {name}（期望 {sn}）")
    # docx 标题
    docx_files = [f for f in list_files(d).get("01竞赛要求", []) if f.endswith(".docx")]
    for f in docx_files:
        tp = os.path.join(d, "01竞赛要求", f)
        t = docx_title(tp)
        ok = sn in t
        report.append(f"- 样题docx 标题标注：`{t}`  {'✔自洽' if ok else '✘与目录不符（目录为'+sn+'）'}")
    # xlsx 说明
    xlsx_files = [f for sub in list_files(d) for f in list_files(d)[sub] if f.endswith(".xlsx")]
    for f in xlsx_files:
        # 找到所属子目录
        sub = next(s for s in list_files(d) if f in list_files(d)[s])
        xp = os.path.join(d, sub, f)
        n = xlsx_note_set(xp)
        ok = sn in n
        report.append(f"- 评分标准xlsx 说明：`{n}`  {'✔自洽' if ok else '✘与目录不符（目录为'+sn+'）'}")
    report.append("")

# 3) 跨套一致性观察
report.append("## 三、跨套一致性观察")
report.append("")
# C zip 命名
report.append("- 模块C工程压缩包命名：")
for name, d in sets:
    files = list_files(d).get("02竞赛素材/0201竞赛素材", [])
    cz = [f for f in files if f.lower().startswith("模块c") and f.endswith(".zip")]
    report.append(f"    - {name}：{cz if cz else '（无）'}")
# 模块D.eprj2 存在性
report.append("- 模块D.eprj2 存在性：")
for name, d in sets:
    files = list_files(d).get("02竞赛素材/0201竞赛素材", [])
    report.append(f"    - {name}：{'✔' if any(f=='模块D.eprj2' for f in files) else '✘缺失'}")
# 评分标准存在性
report.append("- 评分标准文件存在性：")
for name, d in sets:
    xf = [f for s in list_files(d) for f in list_files(d)[s] if f.endswith('.xlsx')]
    report.append(f"    - {name}：{xf if xf else '✘缺失'}")

txt = "\n".join(report)
print(txt)

# 保存报告
out_dir = r"D:/nanyun/04-竞赛与方案/印制电路制作工/06_检查报告"
os.makedirs(out_dir, exist_ok=True)
out_path = os.path.join(out_dir, "样题完整性交叉检查_20260730.md")
with open(out_path, "w", encoding="utf-8") as fh:
    fh.write(txt)
print("\n=== 报告已保存 ===")
print(out_path)
