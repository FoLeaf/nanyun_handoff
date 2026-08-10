import sys, openpyxl

path = sys.argv[1]
out = sys.argv[2]
wb = openpyxl.load_workbook(path, data_only=True)
lines = []
lines.append(f"FILE: {path}")
lines.append(f"SHEETS: {wb.sheetnames}")
for ws in wb.worksheets:
    lines.append("")
    lines.append(f"===== SHEET: {ws.title}  dims={ws.dimensions} max_row={ws.max_row} max_col={ws.max_column} =====")
    # merged cells
    merges = sorted(str(m) for m in ws.merged_cells.ranges)
    if merges:
        lines.append("MERGED: " + " | ".join(merges))
    for row in ws.iter_rows(min_row=1, max_row=ws.max_row, max_col=ws.max_column):
        cells = []
        for c in row:
            v = c.value
            if v is None:
                cells.append("")
            else:
                cells.append(str(v))
        # skip fully empty rows
        if any(x.strip() for x in cells):
            lines.append(" | ".join(cells))
text = "\n".join(lines)
with open(out, "w", encoding="utf-8") as f:
    f.write(text)
print(f"wrote {len(lines)} lines -> {out}")
