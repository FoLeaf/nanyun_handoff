import sys, zipfile, re
from pathlib import Path
root = Path(sys.argv[1])
hits = []
for p in sorted(root.rglob('*')):
    if p.is_file() and p.suffix.lower() in ('.docx', '.xlsx'):
        try:
            z = zipfile.ZipFile(p)
            for name in z.namelist():
                if name.endswith('.xml') or name.endswith('.rels'):
                    data = z.read(name).decode('utf-8', 'ignore')
                    # strip xml tags for readable text
                    text = re.sub(r'<[^>]+>', '', data)
                    if '第四套' in text:
                        # count occurrences
                        cnt = text.count('第四套')
                        hits.append((str(p.relative_to(root)), name, cnt))
        except Exception as e:
            hits.append((str(p.relative_to(root)), f'ERR {e}', 0))
print("=== 文档内容中含'第四套'的位置 ===")
if not hits:
    print("（无）")
for h in hits:
    print(f"{h[0]}\n    -> {h[1]}  出现 {h[2]} 次")
