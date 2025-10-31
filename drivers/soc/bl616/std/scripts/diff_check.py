#!/usr/bin/env python3
import sys
import re
from openai import OpenAI

def chunk_text_by_file(diff_text, max_lines=500):
    blocks, current = [], ""
    for line in diff_text.splitlines(True):
        if line.startswith("diff --git"):
            if current:
                blocks.append(current)
            current = line
        else:
            current += line
    if current:
        blocks.append(current)

    chunks, buf, buf_lines = [], "", 0
    for blk in blocks:
        blk_lines = blk.count("\n")
        if buf and buf_lines + blk_lines > max_lines:
            chunks.append(buf)
            buf, buf_lines = blk, blk_lines
        else:
            buf += blk
            buf_lines += blk_lines
    if buf:
        chunks.append(buf)
    return chunks

def review_diff(diff_path, api_key):
    prompt_header = (
      "请基于以下 Git diff 内容，检查修改后的代码（注意，以`+` 开头的行才是修改后的代码），遵循以下规则：\n"
      "1. **拼写检查**\n"
      "   - 检查变量名、函数名、注释中的拼写错误\n"
      "2. **逻辑 Bug 审查**\n"
      "   - 高风险问题：空指针引用、未处理异常、循环边界错误、资源泄漏、并发竞争\n"
      "   - 其它中风险和低风险的问题可以忽略\n"
      "3. **代码规范检查**\n"
      "   - 强制规则：缩进、单函数长度超过 50 行、魔法数字、重复代码块。轻微格式问题（如多余空格）可忽略。\n"
      "4. **安全例外白名单**\n"
      "   - 明确忽略：硬编码凭证、敏感信息明文存储、缺乏输入过滤\n"
      "**输出要求**：\n"
      "   - 无问题则返回 \"OK\",CI脚本会据此判断是否通过\n"
      "   发现问题时按格式反馈：\n"
      "   [严重级别] 文件名:行号\n"
      "   问题类型：拼写错误/逻辑 Bug/规范违反\n"
      "   代码片段：`错误行内容`\n"
      "   建议修复：（可选）\n"
   )

    try:
        client = OpenAI(api_key=api_key, base_url="https://api.deepseek.com")
        diff = open(diff_path, encoding="utf-8").read()

        all_ok = True
        for idx, chunk in enumerate(chunk_text_by_file(diff, max_lines=500), start=1):
            resp = client.chat.completions.create(
                model="deepseek-chat",
                messages=[
                    {"role":"system","content":"You are a senior code reviewer, the code may be written in C C++ Python or other languages."},
                    {"role":"user","content": prompt_header + f"(part {idx}):\n```diff\n{chunk}\n```"}
                ],
                stream=False
            )
            result = resp.choices[0].message.content.strip()
            print(f"\n=== Review Part {idx} ===\n{result}\n")
            if result != "OK":
                all_ok = False

        return all_ok, None
    except Exception as e:
        print("Error during code review:", e)
        return False, "code review check failed"


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python diff_check.py <diff_file> <deepseek_api_key>")
        sys.exit(1)

    diff_file, api_key = sys.argv[1], sys.argv[2]
    valid, _ = review_diff(diff_file, api_key)
    sys.exit(0 if valid else 1)
