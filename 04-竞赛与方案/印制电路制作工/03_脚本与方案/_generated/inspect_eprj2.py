import sqlite3
from pathlib import Path

p = Path(r"02_样题/第一套/05_U盘资料/模块B1.eprj2")
print("size", p.stat().st_size)
con = sqlite3.connect(str(p))
cur = con.cursor()
tables = [
    r[0]
    for r in cur.execute(
        "SELECT name FROM sqlite_master WHERE type='table' ORDER BY name"
    ).fetchall()
]
print("tables", len(tables))
for t in tables:
    try:
        n = cur.execute(f'SELECT COUNT(*) FROM "{t}"').fetchone()[0]
    except Exception as e:
        n = repr(e)
    print(f"  {t}: {n}")

# Heuristic: dump small text-ish tables / columns
for t in tables:
    cols = [r[1] for r in cur.execute(f'PRAGMA table_info("{t}")').fetchall()]
    print(f"schema {t}: {cols}")

# Look for document titles / board names
for t in tables:
    cols = [r[1] for r in cur.execute(f'PRAGMA table_info("{t}")').fetchall()]
    for c in cols:
        if c.lower() in ("name", "title", "displayname", "filename", "path"):
            try:
                rows = cur.execute(
                    f'SELECT DISTINCT "{c}" FROM "{t}" LIMIT 20'
                ).fetchall()
                if rows:
                    print(f"values {t}.{c}:", [r[0] for r in rows][:20])
            except Exception:
                pass

con.close()
