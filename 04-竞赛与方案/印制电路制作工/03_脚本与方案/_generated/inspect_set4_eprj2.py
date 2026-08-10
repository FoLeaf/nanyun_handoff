# -*- coding: utf-8 -*-
import sqlite3
from pathlib import Path

p = Path(__file__).resolve().parents[2] / "02_样题" / "第四套" / "05_U盘资料" / "模块B.eprj2"
con = sqlite3.connect(str(p))
cur = con.cursor()
tables = [r[0] for r in cur.execute("SELECT name FROM sqlite_master WHERE type='table' ORDER BY name")]
print("tables", len(tables), "size", p.stat().st_size)
for t in tables:
    n = cur.execute(f'SELECT COUNT(*) FROM "{t}"').fetchone()[0]
    cols = [r[1] for r in cur.execute(f'PRAGMA table_info("{t}")').fetchall()]
    if 0 < n < 3000:
        print(f"{t}({n}): {cols}")

keywords = (
    "Hi", "HI-12", "12F", "LDO", "3485", "6612", "SP3485", "DRV", "TB66",
    "MAX", "AMS", "ME62", "motor", "RELAY", "RS485", "NFC",
)
for t in tables:
    cols = [r[1] for r in cur.execute(f'PRAGMA table_info("{t}")').fetchall()]
    for c in cols:
        for kw in keywords:
            try:
                rows = cur.execute(
                    f'SELECT DISTINCT CAST("{c}" AS TEXT) FROM "{t}" '
                    f'WHERE CAST("{c}" AS TEXT) LIKE ? LIMIT 6',
                    (f"%{kw}%",),
                ).fetchall()
            except Exception:
                rows = []
            if rows:
                print(f"HIT {t}.{c} ~{kw}:", [str(r[0])[:120] for r in rows])
con.close()
