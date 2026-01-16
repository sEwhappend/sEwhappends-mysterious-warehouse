# -*- coding: utf-8 -*-
import os
import re
import sys
import pandas as pd
from pathlib import Path
import streamlit as st 


def extract_student_id_from_filename(filename):
    """从文件名中提取前9位数字作为学号"""
    match = re.search(r'\d{9}', filename)
    if match:
        return match.group()
    return None

def get_student_info_from_roster(roster_file):

    try:

        df = pd.read_excel(roster_file, header=3) 
        
        student_id_col = None
        for col in df.columns:
            col_str = str(col)
            if '学号' in col_str:
                student_id_col = col
                break
        
        if student_id_col is None:
            for col in df.columns:
                sample_values = df[col].dropna().head(5)
                if len(sample_values) > 0:
                    has_9digit = any(re.search(r'\d{9}', str(v)) for v in sample_values)
                    if has_9digit:
                        student_id_col = col
                        break

        if student_id_col is None:
            student_id_col = df.columns[0]
        
        name_col = None
        for col in df.columns:
            col_str = str(col)
            if '姓名' in col_str:
                name_col = col
                break
        
        if name_col is None:
            if student_id_col == df.columns[0] and len(df.columns) > 1:
                name_col = df.columns[1]
            else:
                col_index = list(df.columns).index(student_id_col)
                if col_index + 1 < len(df.columns):
                    name_col = df.columns[col_index + 1]

        student_id_to_name = {}
        student_ids = set()

        for idx, row in df.iterrows():
            id_value = row[student_id_col]
            if pd.isna(id_value):
                continue
            
            str_value = str(id_value).strip()
            student_id = None
            
            # 格式清洗
            if "." in str_value:
                str_value = str_value.split(".")[0]

            if str_value.isdigit() and len(str_value) >= 9:
                student_id = str_value[:9]
            else:
                match = re.search(r'\d{9}', str_value)
                if match:
                    student_id = match.group()
            
            if student_id:
                student_ids.add(student_id)
                if name_col and not pd.isna(row[name_col]):
                    name = str(row[name_col]).strip()
                    student_id_to_name[student_id] = name
                else:
                    student_id_to_name[student_id] = "未知"
        
        return student_id_to_name, student_ids

    except Exception as e:
        st.error(f"读取花名册失败: {e}")
        return None, None

# --- Streamlit 界面部分 ---
st.set_page_config(page_title="作业检查系统", layout="wide")
st.title("🎓 作业提交情况检查 (网页版)")

with st.sidebar:
    st.header("配置信息")
    uploaded_file = st.file_uploader("1. 上传花名册.xlsx", type=["xlsx"])
    base_dir = st.text_input("2. 输入作业存放的根目录", value="")
    run_button = st.button("开始检查")

if uploaded_file and run_button:
    # 调用保留了查找逻辑的函数
    student_id_to_name, all_student_ids = get_student_info_from_roster(uploaded_file)
    
    if all_student_ids:
        root_path = Path(base_dir)
        # 兼容性修改：如果找不到“学生作业”字样，尝试匹配所有子文件夹
        homework_folders = [d for d in root_path.iterdir() if d.is_dir()]
        
        if not homework_folders:
            st.warning(f"在 {base_dir} 下未找到任何文件夹。")
        else:
            all_missing_students = {}
            for folder in homework_folders:
                st.subheader(f"📁 检查文件夹: {folder.name}")
                
                files = os.listdir(folder)
                submitted_ids = {extract_student_id_from_filename(f) for f in files if extract_student_id_from_filename(f)}
                missing_ids = all_student_ids - submitted_ids
                all_missing_students[folder.name] = missing_ids
                
                col1, col2, col3 = st.columns(3)
                col1.metric("应交", len(all_student_ids))
                col2.metric("已交", len(submitted_ids))
                col3.metric("未交", len(missing_ids), delta_color="inverse")
                
                if missing_ids:
                    missing_data = [{"学号": sid, "姓名": student_id_to_name.get(sid, "未知")} for sid in sorted(missing_ids)]
                    st.dataframe(pd.DataFrame(missing_data), use_container_width=True)
                else:
                    st.success("🎉 全部交齐了！")
            
            total_missing = sum(len(ids) for ids in all_missing_students.values())
            st.divider()
            st.write(f"### 🏁 总计缺交人次: {total_missing}")
            
            if total_missing > 0:
                summary_text = "未交作业汇总\n" + "="*20 + "\n"
                for f_name, m_ids in all_missing_students.items():
                    summary_text += f"\n{f_name}: {len(m_ids)}人未交\n"
                    for sid in sorted(m_ids):
                        summary_text += f"  - {sid} {student_id_to_name.get(sid, '')}\n"
                st.download_button("📩 下载未交名单汇总", summary_text, file_name="汇总报告.txt")
else:
    st.info("💡 请先在左侧上传花名册并点击‘开始检查’。")