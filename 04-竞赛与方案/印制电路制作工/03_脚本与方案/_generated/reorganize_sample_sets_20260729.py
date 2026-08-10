# -*- coding: utf-8 -*-
"""Reorganize 02_样题 into 5 categories + archive intermediate junk."""
from __future__ import annotations

import shutil
from datetime import datetime
from pathlib import Path

ROOT = Path(r"D:\nanyun\04-竞赛与方案\印制电路制作工")
SAMPLE = ROOT / "02_样题"
ARCHIVE = ROOT / "05_归档备份" / f"sample-prep-cleanup-{datetime.now().strftime('%Y%m%d_%H%M%S')}"
GEN = ROOT / "03_脚本与方案" / "_generated"

CATS = {
    "01_样题": "样题（完整版题面）",
    "02_评分标准": "评分标准",
    "03_结果呈现_参考答案": "结果呈现（参考答案）",
    "04_需填写内容": "需填写内容（空白表）",
    "05_U盘资料": "U盘资料（赛场下发电子文件）",
    "99_命题规格": "命题规格（内部，不下发选手）",
}

# set_name -> category -> list of (src_name, dest_name|None)
PLAN: dict[str, dict[str, list[tuple[str, str | None]]]] = {
    "第一套": {
        "01_样题": [
            ("印制电路制作工赛项_竞赛样题（完整版）.docx", None),
        ],
        "02_评分标准": [
            ("印制电路制作工赛项_评分标准（第一套·0721口径）.docx", None),
            ("印制电路制作工赛项_评分标准（第一套·0721口径）.xlsx", None),
        ],
        "03_结果呈现_参考答案": [
            ("模块C_缺陷记录表-参考答案.docx", None),
            ("模块C_PCB制程工艺卡（参考答案版）.docx", None),
            ("模块D_检测记录表_参考答案.docx", None),
        ],
        "04_需填写内容": [
            ("模块C_缺陷记录表.docx", None),
            ("模块C_PCB制程工艺卡.docx", None),
            ("学生组_模块D_检测记录表.docx", None),
        ],
        "05_U盘资料": [
            ("模块B.eprj2", None),
            ("模块B1.eprj2", None),
            ("模块Cgerber.zip", None),
            ("模块D.eprj2", None),
        ],
        "99_命题规格": [],
    },
    "第二套": {
        "01_样题": [
            ("印制电路制作工赛项_竞赛样题（完整版）.docx", None),
        ],
        "02_评分标准": [],
        "03_结果呈现_参考答案": [
            ("模块C_缺陷记录表-参考答案.docx", None),
        ],
        "04_需填写内容": [
            ("模块C_缺陷记录表.docx", None),
            ("模块C_PCB制程工艺卡.docx", None),
            ("学生组_模块D_检测记录表.docx", None),
        ],
        "05_U盘资料": [],
        "99_命题规格": [
            ("00_README_阶段1.md", None),
            ("00_命题边界确认表.md", None),
            ("00_工程回填清单.md", None),
            ("模块C_缺陷规格_12处.md", None),
            ("模块D_缺陷规格_D01-D08_AB.md", None),
        ],
    },
    "第三套": {
        "01_样题": [
            ("印制电路制作工赛项_竞赛样题（完整版）.docx", None),
        ],
        "02_评分标准": [],
        "03_结果呈现_参考答案": [
            ("模块C_缺陷记录表-参考答案.docx", None),
            ("模块D_检测记录表_参考答案.docx", None),
        ],
        "04_需填写内容": [
            ("模块C_缺陷记录表.docx", None),
            ("模块C_PCB制程工艺卡.docx", None),
            ("学生组_模块D_检测记录表.docx", None),
        ],
        "05_U盘资料": [
            ("模块B 第三套.eprj2", "模块B.eprj2"),
            ("模块D第三套.eprj2", "模块D.eprj2"),
        ],
        "99_命题规格": [
            ("00_README_阶段1.md", None),
            ("00_主题锚点.md", None),
            ("00_命题边界确认表.md", None),
            ("00_工程回填清单.md", None),
            ("模块C_缺陷规格_12处.md", None),
            ("模块D_缺陷规格_D01-D08_AB.md", None),
            ("2D_PCB6_2026-07-25.png", None),
            ("Gerber_PCB6_1_2026-07-25.zip", None),
            ("Netlist_PCB6_2026-07-25.tel", None),
            ("Netlist_Schematic1_2026-07-28.tel", None),
        ],
    },
    "第四套": {
        "01_样题": [
            ("印制电路制作工赛项_竞赛样题（完整版）.docx", None),
        ],
        "02_评分标准": [],
        "03_结果呈现_参考答案": [
            ("模块C_缺陷记录表-参考答案.docx", None),
            ("模块D_检测记录表_参考答案.docx", None),
        ],
        "04_需填写内容": [
            ("模块C_缺陷记录表.docx", None),
            ("模块C_PCB制程工艺卡.docx", None),
            ("学生组_模块D_检测记录表.docx", None),
        ],
        "05_U盘资料": [
            ("模块B.eprj2", None),
            ("模块C.zip", "模块Cgerber.zip"),
        ],
        "99_命题规格": [
            ("00_README_阶段1.md", None),
            ("00_命题边界确认表.md", None),
            ("00_工程回填清单.md", None),
            ("模块B_半成品工程规格.md", None),
            ("模块C_缺陷规格_12处.md", None),
            ("模块D_缺陷规格_D01-D08_AB.md", None),
        ],
    },
}

JUNK_IN_SETS: dict[str, list[str]] = {
    "第一套": [
        "学生组_模块D_检测记录表 - 副本.docx",
        "评分标准demo物联网.xlsx",
    ],
    "第三套": [
        "模块C_缺陷记录表-参考答案 (修复的).docx",
        "2D_PCB6_2026-07-215.png",
    ],
    "第二套": [],
    "第四套": [],
}

ROOT_JUNK = [
    "02_样题.zip",
]

GEN_JUNK_FILES = [
    "module_c_ref_ans.txt",
    "set1_full_now.txt",
    "set1_full_para_index.txt",
    "set1_full_tables.txt",
    "set2_full_now.txt",
    "build_sample_set2_phase1_20260723.py",
]
GEN_JUNK_DIRS = [
    "module_c_check_zip",
    "module_c_gerber_inspect",
    "review_as_real_exam",
]

ROOT_TMP = [
    "_tmp_0721_extract.txt",
]


def ensure_dir(p: Path) -> None:
    p.mkdir(parents=True, exist_ok=True)


def move_to(src: Path, dest: Path, log: list[str]) -> None:
    if not src.exists():
        log.append(f"MISSING: {src}")
        return
    ensure_dir(dest.parent)
    if dest.exists():
        if src.resolve() == dest.resolve():
            log.append(f"SKIP same: {src}")
            return
        bak = ARCHIVE / "_collisions" / dest.relative_to(ROOT)
        ensure_dir(bak.parent)
        shutil.move(str(dest), str(bak))
        log.append(f"COLLISION archive dest: {dest} -> {bak}")
    shutil.move(str(src), str(dest))
    log.append(f"MOVE {src.relative_to(ROOT)} -> {dest.relative_to(ROOT)}")


def write_set_readme(set_dir: Path, set_name: str) -> None:
    lines = [
        f"# {set_name} · 目录说明",
        "",
        "按交付角色分目录存放；**仅 `01`–`05` 面向赛场/外发**，`99_命题规格` 为内部命题资产。",
        "",
        "| 目录 | 角色 |",
        "|------|------|",
    ]
    for k, v in CATS.items():
        lines.append(f"| `{k}/` | {v} |")
    lines += [
        "",
        "## 各目录文件",
        "",
    ]
    for cat in CATS:
        d = set_dir / cat
        files = sorted(p.name for p in d.iterdir() if p.is_file()) if d.exists() else []
        lines.append(f"### {cat}")
        lines.append("")
        if not files:
            lines.append("_（空 — 待回填）_")
        else:
            for f in files:
                lines.append(f"- `{f}`")
        lines.append("")
    (set_dir / "README.md").write_text("\n".join(lines), encoding="utf-8")


def main() -> None:
    log: list[str] = []
    ensure_dir(ARCHIVE)
    (ARCHIVE / "00_移出说明.md").write_text(
        "\n".join(
            [
                "# 样题筹备中间文件清理",
                "",
                f"- 时间：{datetime.now().isoformat(timespec='seconds')}",
                "- 原因：按「样题 / 评分标准 / 结果呈现 / 需填写内容 / U盘资料」重组后，移出筹备期无用中间文件与重复副本。",
                "- 正式交付文件已迁入各套 `01`–`05` 目录；本目录仅作归档，不参与外发。",
                "",
            ]
        ),
        encoding="utf-8",
    )

    for set_name, cats in PLAN.items():
        set_dir = SAMPLE / set_name
        if not set_dir.exists():
            log.append(f"NO SET: {set_name}")
            continue
        for cat in CATS:
            ensure_dir(set_dir / cat)
        for cat, items in cats.items():
            for src_name, dest_name in items:
                src = set_dir / src_name
                dest = set_dir / cat / (dest_name or src_name)
                move_to(src, dest, log)
        for junk in JUNK_IN_SETS.get(set_name, []):
            src = set_dir / junk
            if src.exists():
                dest = ARCHIVE / set_name / junk
                move_to(src, dest, log)
        write_set_readme(set_dir, set_name)
        log.append(f"README written: {set_name}")

        leftovers = [
            p
            for p in set_dir.iterdir()
            if p.is_file() and p.name not in {"README.md"}
        ]
        for p in leftovers:
            dest = ARCHIVE / set_name / "_leftovers" / p.name
            move_to(p, dest, log)

    for name in ROOT_JUNK:
        src = SAMPLE / name
        if src.exists():
            move_to(src, ARCHIVE / "02_样题_root" / name, log)

    for name in GEN_JUNK_FILES:
        src = GEN / name
        if src.exists():
            move_to(src, ARCHIVE / "03_generated" / name, log)
    for name in GEN_JUNK_DIRS:
        src = GEN / name
        if src.exists():
            dest = ARCHIVE / "03_generated" / name
            ensure_dir(dest.parent)
            if dest.exists():
                shutil.rmtree(dest)
            shutil.move(str(src), str(dest))
            log.append(f"MOVE DIR {src.relative_to(ROOT)} -> {dest.relative_to(ROOT)}")

    for name in ROOT_TMP:
        src = ROOT / name
        if src.exists():
            move_to(src, ARCHIVE / "project_root" / name, log)

    log_path = ARCHIVE / "00_操作日志.txt"
    log_path.write_text("\n".join(log), encoding="utf-8")
    print(f"ARCHIVE={ARCHIVE}")
    print(f"LOG lines={len(log)}")
    for set_name in PLAN:
        set_dir = SAMPLE / set_name
        print(f"\n=== {set_name} ===")
        for p in sorted(set_dir.rglob("*")):
            if p.is_file():
                print(p.relative_to(SAMPLE).as_posix())


if __name__ == "__main__":
    main()
