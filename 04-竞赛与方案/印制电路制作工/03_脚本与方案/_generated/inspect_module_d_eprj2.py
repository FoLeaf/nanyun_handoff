# -*- coding: utf-8 -*-
import re
import sqlite3
from pathlib import Path

FILES = {
    "set1_D_repo": Path(r"D:\nanyun\04-竞赛与方案\印制电路制作工\02_样题\第一套\05_U盘资料\模块D.eprj2"),
    "set1_D_lceda": Path(r"D:\Study\lceda\projects\模块D.eprj2"),
    "set2_D_lceda": Path(r"D:\Study\lceda\projects\第二套模块D.eprj2"),
    "set3_B_lceda": Path(r"D:\Study\lceda\projects\模块B 第三套.eprj2"),
}

PAT = re.compile(
    r"MOD-D|TP[0-9]|网格|检测|缺陷|S-A|S-B|S2-|S3-|S4-|WM2|CAN|80|60|走线|阻焊|丝印",
    re.I,
)


def main() -> None:
    for name, p in FILES.items():
        if not p.exists():
            print(name, "MISSING")
            continue
        print("\n====", name, p.name, "size", p.stat().st_size, "====")
        con = sqlite3.connect(str(p))
        cur = con.cursor()
        tables = [
            r[0]
            for r in cur.execute(
                "SELECT name FROM sqlite_master WHERE type='table'"
            ).fetchall()
        ]
        print("tables", len(tables))
        hits = []
        for t in tables:
            cols = [r[1] for r in cur.execute(f'PRAGMA table_info("{t}")').fetchall()]
            for c in cols:
                try:
                    rows = cur.execute(
                        f'SELECT "{c}" FROM "{t}" WHERE typeof("{c}")="text" LIMIT 1000'
                    ).fetchall()
                except Exception:
                    continue
                for (val,) in rows:
                    if not isinstance(val, str) or len(val) < 3:
                        continue
                    if PAT.search(val):
                        hits.append((t, c, val.replace("\n", " ")[:160]))
                        if len(hits) >= 40:
                            break
                if len(hits) >= 40:
                    break
            if len(hits) >= 40:
                break
        for t, c, s in hits[:20]:
            print(f"{t}.{c} => {s}")
        for t in tables:
            cols = [r[1] for r in cur.execute(f'PRAGMA table_info("{t}")').fetchall()]
            for c in cols:
                if c.lower() in (
                    "name",
                    "title",
                    "displayname",
                    "doc_name",
                    "schematic_name",
                    "pcb_name",
                ):
                    try:
                        vals = [
                            r[0]
                            for r in cur.execute(
                                f'SELECT DISTINCT "{c}" FROM "{t}" LIMIT 20'
                            ).fetchall()
                            if r[0]
                        ]
                        if vals:
                            print("NAME", t, c, vals[:12])
                    except Exception:
                        pass
        con.close()


if __name__ == "__main__":
    main()
