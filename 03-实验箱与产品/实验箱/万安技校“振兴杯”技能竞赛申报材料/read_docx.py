import zipfile
import xml.etree.ElementTree as ET
import glob
import os

try:
    files = glob.glob('*.docx')
    if not files:
        print("No docx files found")
    for docx_file in files:
        print(f"Reading {docx_file}:")
        doc = zipfile.ZipFile(docx_file)
        root = ET.fromstring(doc.read('word/document.xml'))
        text = '\n'.join([node.text for node in root.iter() if node.tag.endswith('}t') and node.text])
        with open('extract.txt', 'w', encoding='utf-8') as f:
            f.write(text)
        print("Successfully extracted to extract.txt")
except Exception as e:
    print(f"Error: {e}")
