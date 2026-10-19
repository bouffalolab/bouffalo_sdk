#!/usr/bin/env python3
"""Local Bocha web_search debugger for mimiclaw.

This script mirrors the important parts of
`port/src/mimiclaw_tool_web_search.c` so search behavior can be debugged on a
host machine before flashing firmware to the board.
"""

from __future__ import annotations

import argparse
import json
import os
import ssl
import sys
import textwrap
import urllib.error
import urllib.request
from typing import Any


BOCHA_API_URL = "https://api.bochaai.com/v1/web-search"
SEARCH_RESULT_COUNT_DEFAULT = 5
SEARCH_RESULT_COUNT_MAX = 8
SEARCH_TIMEOUT_S = 15
SEARCH_SNIPPET_MAX_LEN = 220

TIME_SENSITIVE_TERMS = (
    "today",
    "tonight",
    "now",
    "current",
    "currently",
    "price",
    "prices",
    "weather",
    "forecast",
    "breaking",
    "latest",
    "recent",
    "recently",
    "news",
    "update",
    "updates",
    "今天",
    "今日",
    "现在",
    "当前",
    "实时",
    "最新",
    "近期",
    "最近",
    "新闻",
    "动态",
    "消息",
    "油价",
    "股价",
    "汇率",
    "天气",
    "行情",
)


def query_is_time_sensitive(query: str) -> bool:
    query_lower = query.lower()
    return any(term in query or term in query_lower for term in TIME_SENSITIVE_TERMS)


def query_has_recency_hint(query: str) -> bool:
    query_lower = query.lower()
    hints = ("latest", "news", "current", "最新", "新闻", "当前")
    return any(hint in query or hint in query_lower for hint in hints)


def build_timely_query(query: str) -> str | None:
    if not query_is_time_sensitive(query) or query_has_recency_hint(query):
        return None

    cn_terms = ("今天", "今日", "油价", "天气", "股价", "汇率")
    if any(term in query for term in cn_terms):
        return f"{query} 最新 新闻"
    return f"{query} latest news"


def append_result(lines: list[str], index: int, title: str, url: str, text: str, date: str | None) -> None:
    lines.append(f"{index}. {title or '(no title)'}")
    lines.append(f"   {url or ''}")
    lines.append(f"   {(text or '')[:SEARCH_SNIPPET_MAX_LEN]}")
    if date:
        lines.append(f"   Published: {date}")
    lines.append("")


def bocha_find_results_array(root: dict[str, Any]) -> list[dict[str, Any]] | None:
    web_pages = root.get("webPages")
    results = web_pages.get("value") if isinstance(web_pages, dict) else None
    if isinstance(results, list):
        return results

    data = root.get("data")
    if isinstance(data, dict):
        web_pages = data.get("webPages")
        results = web_pages.get("value") if isinstance(web_pages, dict) else None
        if isinstance(results, list):
            return results

        results = data.get("value")
        if isinstance(results, list):
            return results

    news = root.get("news")
    results = news.get("value") if isinstance(news, dict) else None
    if isinstance(results, list):
        return results

    results = root.get("items")
    if isinstance(results, list):
        return results

    return None


def format_bocha_results(root: dict[str, Any]) -> tuple[str, bool]:
    results = bocha_find_results_array(root)
    if not results:
        return "No web results found.", False

    lines: list[str] = []
    count = 0
    for item in results:
        if not isinstance(item, dict):
            continue
        if count >= SEARCH_RESULT_COUNT_MAX:
            break

        text = item.get("summary") or item.get("snippet") or ""
        append_result(
            lines,
            count + 1,
            str(item.get("name") or ""),
            str(item.get("url") or ""),
            str(text),
            str(item.get("datePublished") or "") or None,
        )
        count += 1

    if count == 0:
        return "No web results found.", False
    return "\n".join(lines).rstrip(), True


def http_post_json(url: str, headers: dict[str, str], payload: dict[str, Any], timeout_s: int) -> tuple[int, str]:
    data = json.dumps(payload, ensure_ascii=False).encode("utf-8")
    req = urllib.request.Request(url=url, data=data, method="POST")
    for key, value in headers.items():
        req.add_header(key, value)

    try:
        with urllib.request.urlopen(req, timeout=timeout_s, context=ssl.create_default_context()) as rsp:
            status = getattr(rsp, "status", 200)
            body = rsp.read().decode("utf-8", errors="replace")
            return status, body
    except urllib.error.HTTPError as exc:
        body = exc.read().decode("utf-8", errors="replace")
        return exc.code, body


def perform_bocha_search(
    api_key: str,
    query: str,
    count: int,
    summary: bool,
    freshness: str | None,
    timeout_s: int,
) -> tuple[int, str, dict[str, Any] | None, str]:
    payload: dict[str, Any] = {
        "query": query,
        "count": count,
        "summary": summary,
    }
    if freshness:
        payload["freshness"] = freshness

    headers = {
        "Accept": "application/json",
        "Authorization": f"Bearer {api_key}",
        "Content-Type": "application/json",
        "Connection": "close",
    }

    status, body = http_post_json(BOCHA_API_URL, headers, payload, timeout_s)
    try:
        parsed = json.loads(body) if body else None
    except json.JSONDecodeError:
        parsed = None

    formatted = ""
    if status == 200 and isinstance(parsed, dict):
        formatted, _ = format_bocha_results(parsed)

    return status, body, parsed, formatted


def run_attempt(
    api_key: str,
    query: str,
    count: int,
    summary: bool,
    freshness: str | None,
    timeout_s: int,
    label: str,
    show_raw: bool,
) -> tuple[bool, str]:
    print(f"[{label}] query={query}")
    if freshness:
        print(f"[{label}] freshness={freshness}")

    status, body, parsed, formatted = perform_bocha_search(
        api_key=api_key,
        query=query,
        count=count,
        summary=summary,
        freshness=freshness,
        timeout_s=timeout_s,
    )

    print(f"[{label}] http_status={status}")
    if show_raw:
        raw_preview = body[:1200] if body else ""
        print(f"[{label}] raw_body:\n{raw_preview}")

    if status != 200:
        print(f"[{label}] request failed")
        return False, ""

    if not isinstance(parsed, dict):
        print(f"[{label}] parse failed")
        return False, ""

    print(f"[{label}] formatted_output_preview:\n{textwrap.shorten(formatted or '(empty)', width=400, placeholder=' ...')}")
    if formatted and formatted != "No web results found.":
        print(f"[{label}] usable results found")
        return True, formatted

    print(f"[{label}] no usable results")
    return False, formatted


def main() -> int:
    parser = argparse.ArgumentParser(description="Local mimiclaw Bocha web_search debugger")
    parser.add_argument("query", help="Search query")
    parser.add_argument("--api-key", dest="api_key", default=os.getenv("BOCHA_API_KEY", ""),
                        help="Bocha API key. Defaults to BOCHA_API_KEY env var.")
    parser.add_argument("--count", type=int, default=SEARCH_RESULT_COUNT_DEFAULT,
                        help="Result count (default: 5, max: 8)")
    parser.add_argument("--summary", action="store_true", default=True,
                        help="Request summary snippets (default: enabled)")
    parser.add_argument("--no-summary", dest="summary", action="store_false",
                        help="Disable summary snippets")
    parser.add_argument("--freshness", default=None,
                        help="Explicit freshness to send to API")
    parser.add_argument("--timeout", type=int, default=SEARCH_TIMEOUT_S,
                        help="HTTP timeout in seconds")
    parser.add_argument("--show-raw", action="store_true",
                        help="Print raw response preview")
    parser.add_argument("--no-rewrite", action="store_true",
                        help="Disable the local time-sensitive rewrite + fallback flow")
    args = parser.parse_args()

    if not args.api_key:
        print("Missing API key. Set BOCHA_API_KEY or pass --api-key.", file=sys.stderr)
        return 2

    args.count = max(1, min(args.count, SEARCH_RESULT_COUNT_MAX))

    rewritten_query = None if args.no_rewrite else build_timely_query(args.query)

    if rewritten_query and not args.freshness:
        ok, formatted = run_attempt(
            api_key=args.api_key,
            query=rewritten_query,
            count=args.count,
            summary=args.summary,
            freshness=None,
            timeout_s=args.timeout,
            label="rewrite",
            show_raw=args.show_raw,
        )
        if ok:
            print("\nFINAL RESULT\n")
            print(formatted)
            return 0

        print("[rewrite] falling back to original query")

    ok, formatted = run_attempt(
        api_key=args.api_key,
        query=args.query,
        count=args.count,
        summary=args.summary,
        freshness=args.freshness,
        timeout_s=args.timeout,
        label="original",
        show_raw=args.show_raw,
    )
    if ok:
        print("\nFINAL RESULT\n")
        print(formatted)
        return 0

    print("\nNo usable search results.")
    return 1


if __name__ == "__main__":
    raise SystemExit(main())
