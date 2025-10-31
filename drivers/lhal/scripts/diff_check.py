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
       "我有如下 Git diff，请帮我按照如下要求进行检查.\n"
       "1 是否存在拼写错误,包括连字符，如果有一定要指出\n"
       "2 是否存在潜在逻辑 Bug，如果有Bug，一定要指出,如果是函数中硬编码了用户名和密码这类问题可以放过这类问题\n"
       "3 是否违反代码规范，如果严重违反，一定要指出\n"
       "对于出现的user name,password 等安全问题可以认为没有问题，因为这个系统做不到这么安全\n"
       "如果一切正常,没有大的原则性问题,没有特别需要修改的大问题，你就直接回复 OK两个字符即可,不需要回复其它内容；否则你需要指出具体问题:\n"
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
        print("Review finished\n")
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
