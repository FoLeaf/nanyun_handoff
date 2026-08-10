import sqlite3
import json
import re
from pathlib import Path

p = Path(r"02_样题/第一套/05_U盘资料/模块B1.eprj2")
con = sqlite3.connect(str(p))
cur = con.cursor()

# projects
for row in cur.execute("SELECT uuid, name, length(content), length(boards), pcb_count FROM projects"):
    print("project:", row)

# structures
for row in cur.execute("SELECT id, branch_uuid, length(structure), substr(structure,1,500) FROM project_structures"):
    print("structure id", row[0], "branch", row[1], "len", row[2])
    print(row[3][:500])
    print("---")

# history_data sizes
for row in cur.execute(
    "SELECT id, uuid, history_uuid, length(dataStr), created_at FROM history_data ORDER BY id"
):
    print("history_data", row)

# try parse largest history snapshots for keywords
rows = cur.execute(
    "SELECT id, length(dataStr), dataStr FROM history_data ORDER BY length(dataStr) DESC"
).fetchall()
for rid, ln, data in rows[:3]:
    print(f"\n== history {rid} len={ln} ==")
    if not data:
        continue
    # search keywords
    for kw in [
        "traceWidth",
        "lineWidth",
        "clearance",
        "via",
        "drill",
        "silk",
        "solder",
        "defect",
        "width",
        "LAYER",
        "copper",
        "Gerber",
        "mil",
        "pad",
    ]:
        if kw.lower() in data.lower():
            print("  has", kw)
    # show small json head if json
    s = data[:300]
    print("head:", s.replace("\n", " ")[:300])
    # find numeric width-like patterns
    nums = re.findall(r'"(?:width|traceWidth|lineWidth|clearance|hole|drill|size)"\s*:\s*([0-9.]+)', data[:200000], re.I)
    print("sample numeric fields", nums[:30])

# project_history snapshot
for t in [
    "project_history_e01cb5f6a8344ad6aa5d2829c4558664",
    "project_histories",
    "project_history_",
]:
    try:
        rows = cur.execute(f'SELECT id, uuid, length(snapshot), num, is_lock FROM "{t}"').fetchall()
        print(f"\n{t}:", rows)
        for rid, uuid, ln, num, lock in rows:
            if ln and ln > 0:
                snap = cur.execute(
                    f'SELECT substr(snapshot,1,400) FROM "{t}" WHERE id=?', (rid,)
                ).fetchone()[0]
                print("snap head", snap[:400])
    except Exception as e:
        print(t, e)

con.close()
