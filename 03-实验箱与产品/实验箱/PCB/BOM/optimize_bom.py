import pandas as pd
import glob
import os
import re

directory = "d:/nanyun/实验箱/PCB/BOM/"
files = [f for f in glob.glob(os.path.join(directory, "*.xlsx")) if not os.path.basename(f).startswith("Optimized_") and not os.path.basename(f).startswith("Combined_Optimized_BOM") and not os.path.basename(f).startswith("~$")]

# Mapping for missing data
part_mapping = {
    ('0.1uF', 'C0603'): {'Supplier Part': 'C14663', 'Manufacturer Part': 'CL10B104KB8NNNC', 'Manufacturer': 'Samsung', 'Supplier': 'LCSC'},
    ('100nF', 'C0603'): {'Supplier Part': 'C14663', 'Manufacturer Part': 'CL10B104KB8NNNC', 'Manufacturer': 'Samsung', 'Supplier': 'LCSC'},
    ('10uF', 'C0603'): {'Supplier Part': 'C19702', 'Manufacturer Part': 'CL10A106KP8NNNC', 'Manufacturer': 'Samsung', 'Supplier': 'LCSC'},
    ('100uF', 'C0603'): {'Supplier Part': 'C85966', 'Manufacturer Part': 'CL10A107MQ8NNNC', 'Manufacturer': 'Samsung', 'Supplier': 'LCSC'},
    ('15pF', 'C0603'): {'Supplier Part': 'C1649', 'Manufacturer Part': 'CC0603JRNPO9BN150', 'Manufacturer': 'YAGEO', 'Supplier': 'LCSC'},
    ('10NF', 'C0603'): {'Supplier Part': 'C15195', 'Manufacturer Part': 'CC0603KRX7R9BB103', 'Manufacturer': 'YAGEO', 'Supplier': 'LCSC'},
    ('1K', 'R0603'): {'Supplier Part': 'C21190', 'Manufacturer Part': '0603WAF1001T5E', 'Manufacturer': 'UNI-ROYAL', 'Supplier': 'LCSC'},
    ('1.5k', 'R0603'): {'Supplier Part': 'C22843', 'Manufacturer Part': '0603WAF1501T5E', 'Manufacturer': 'UNI-ROYAL', 'Supplier': 'LCSC'},
    ('10K', 'R0603'): {'Supplier Part': 'C25804', 'Manufacturer Part': '0603WAF1002T5E', 'Manufacturer': 'UNI-ROYAL', 'Supplier': 'LCSC'},
    ('22m', 'R0603'): {'Supplier Part': 'C26071', 'Manufacturer Part': '0603WGF2205T5E', 'Manufacturer': 'UNI-ROYAL', 'Supplier': 'LCSC'}, # 22M欧姆
    ('3.3R', 'R0603'): {'Supplier Part': 'C25126', 'Manufacturer Part': '0603WAF3R30T5E', 'Manufacturer': 'UNI-ROYAL', 'Supplier': 'LCSC'},
    ('LED_0603-G', 'LED_0603'): {'Supplier Part': 'C2286', 'Manufacturer Part': 'LTST-C190GKT', 'Manufacturer': 'LITEON', 'Supplier': 'LCSC'},
}

def fill_missing(row):
    # Try to match based on Comment and Footprint
    key = (str(row['Comment']).strip(), str(row['Footprint']).strip())
    # If not found, try Value and Footprint
    if key not in part_mapping and pd.notna(row['Value']):
        key = (str(row['Value']).strip(), str(row['Footprint']).strip())
        
    if pd.isna(row['Supplier Part']) or str(row['Supplier Part']).strip() == '':
        if key in part_mapping:
            info = part_mapping[key]
            row['Supplier Part'] = info['Supplier Part']
            row['Manufacturer Part'] = info['Manufacturer Part']
            row['Manufacturer'] = info['Manufacturer']
            row['Supplier'] = info['Supplier']
    return row

def merge_designators(series):
    # series is a pandas Series of strings like "C1, C2", "C3"
    designators = []
    for d_str in series.dropna():
        # split by comma
        designators.extend([d.strip() for d in str(d_str).replace('，', ',').split(',') if d.strip()])
    # deduplicate and sort
    # to sort logically like C1, C2 ... C10
    def natural_sort_key(s):
        return [int(text) if text.isdigit() else text.lower() for text in re.split('([0-9]+)', s)]
    unique_d = sorted(list(set(designators)), key=natural_sort_key)
    return ', '.join(unique_d)

def optimize_df(df):
    df = df.apply(fill_missing, axis=1)
    
    # Fill NA for groupby
    df['Comment'] = df['Comment'].fillna('')
    df['Footprint'] = df['Footprint'].fillna('')
    df['Value'] = df['Value'].fillna('')
    df['Supplier Part'] = df['Supplier Part'].fillna('')
    df['Manufacturer Part'] = df['Manufacturer Part'].fillna('')
    df['Manufacturer'] = df['Manufacturer'].fillna('')
    df['Supplier'] = df['Supplier'].fillna('')
    
    # Group by
    groupby_cols = ['Comment', 'Footprint', 'Value', 'Supplier Part', 'Manufacturer Part', 'Manufacturer', 'Supplier']
    if 'Pin Count' in df.columns:
        df['Pin Count'] = df['Pin Count'].fillna('')
        groupby_cols.append('Pin Count')
        
    grouped = df.groupby(groupby_cols).agg({
        'Quantity': 'sum',
        'Designator': merge_designators
    }).reset_index()
    
    # Sort by component type based on the first designator
    def get_sort_keys(designator_str):
        if not designator_str or pd.isna(designator_str):
            return (99, "ZZ", 0)
        first_d = str(designator_str).split(',')[0].strip()
        match = re.match(r'([A-Za-z_]+)(\d*)', first_d)
        if match:
            type_str = match.group(1).upper()
            num = int(match.group(2)) if match.group(2) else 0
            prefix_order = {'U': 1, 'R': 2, 'C': 3, 'L': 4, 'D': 5, 'LED': 5, 'Q': 6, 'Y': 7, 'X': 7, 'J': 8, 'P': 8, 'SW': 9, 'K': 9}
            priority = prefix_order.get(type_str, 99)
            return (priority, type_str, num)
        return (99, "ZZ", 0)

    sort_keys = grouped['Designator'].apply(get_sort_keys)
    grouped['SortPriority'] = sort_keys.apply(lambda x: x[0])
    grouped['SortType'] = sort_keys.apply(lambda x: x[1])
    grouped['SortNum'] = sort_keys.apply(lambda x: x[2])
    
    grouped = grouped.sort_values(by=['SortPriority', 'SortType', 'SortNum', 'Value'])
    grouped = grouped.drop(columns=['SortPriority', 'SortType', 'SortNum']).reset_index(drop=True)
    
    # Ensure No. column exists
    grouped.insert(0, 'No.', range(1, len(grouped) + 1))
    
    # Reorder columns to standard
    std_cols = ['No.', 'Quantity', 'Comment', 'Designator', 'Footprint', 'Value', 'Manufacturer Part', 'Manufacturer', 'Supplier Part', 'Supplier']
    if 'Pin Count' in df.columns:
        std_cols.append('Pin Count')
        
    return grouped[std_cols]

all_dfs = []

output_dir = os.path.join(directory, "Optimized")
os.makedirs(output_dir, exist_ok=True)

for f in files:
    try:
        df = pd.read_excel(f)
        optimized = optimize_df(df)
        out_name = "Optimized_" + os.path.basename(f)
        optimized.to_excel(os.path.join(output_dir, out_name), index=False)
        print(f"Processed and optimized: {os.path.basename(f)}")
        
        # for combined
        df_for_all = optimized.copy()
        df_for_all['Source_File'] = os.path.basename(f)
        all_dfs.append(df_for_all)
    except Exception as e:
        print(f"Error processing {f}: {e}")

if all_dfs:
    combined_raw = pd.concat(all_dfs, ignore_index=True)
    combined_optimized = optimize_df(combined_raw)
    combined_optimized.to_excel(os.path.join(directory, "Combined_Optimized_BOM_Sorted.xlsx"), index=False)
    print("Combined BOM generated: Combined_Optimized_BOM_Sorted.xlsx")

print("Optimization complete.")
