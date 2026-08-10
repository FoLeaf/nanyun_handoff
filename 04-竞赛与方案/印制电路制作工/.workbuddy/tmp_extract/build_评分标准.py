import sys, openpyxl

src = sys.argv[1]   # input xlsx (currently a copy of set1)
out = sys.argv[2]   # output xlsx
setname = sys.argv[3]  # 第三套 / 第四套

wb = openpyxl.load_workbook(src)
ws = wb.active  # 评分表

# ---- Module D V01-V05 descriptions for sets 3/4 (shared D board) ----
v_map = [
    ("V01 顶层线路缺口", "V01 走线颈缩（顶层）。顶层+颈缩/变窄类+位置大致正确；建议全对2/部分1/错0"),
    ("V02 底层线宽变窄", "V02 过孔内径过小（钻孔层）。钻孔层+内径过小+位置大致正确；非3.50mm定位孔误报"),
    ("V03 孔破盘",       "V03 漏钻孔（钻孔层）。有丝印无钻孔+位置大致正确；非过孔误报"),
    ("V04 阻焊未开窗",   "V04 无法形成有效阻焊桥（顶层）。阻焊/桥类缺陷+位置大致正确"),
    # V05 丝印缺失或压焊盘 —— 与基准套一致，保持不变
]

repl_count = 0
for row in ws.iter_rows():
    for c in row:
        v = c.value
        if not isinstance(v, str):
            continue
        # V descriptions
        for old_pre, new_txt in v_map:
            if v.startswith(old_pre):
                c.value = new_txt
                repl_count += 1
                break
        else:
            # 综合判定 fatal-defect reference
            if "存在线路缺口、孔破盘等致命缺陷时应判报废" in v:
                c.value = v.replace(
                    "存在线路缺口、孔破盘等致命缺陷时应判报废",
                    "存在走线颈缩（V01）、漏钻孔（V03）等严重影响使用的致命缺陷时应判报废")
                repl_count += 1
            # 说明 note: 第一套 -> 本套
            if "第一套样题配套" in v:
                c.value = v.replace("第一套", setname)
                repl_count += 1

wb.save(out)
print(f"[{setname}] 修改单元格数={repl_count} -> {out}")
