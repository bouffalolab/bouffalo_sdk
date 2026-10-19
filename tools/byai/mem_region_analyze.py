# -*- coding: UTF-8 -*-

import argparse
import csv
import cmd
import os
import re
import sys
from collections import OrderedDict, defaultdict
from html import escape
from pydoc import pager


APP_NAME = "Memory Region Analyze"

if getattr(sys, "frozen", False):
    app_path = os.path.dirname(sys.executable)
else:
    app_path = os.path.dirname(os.path.abspath(__file__))

mapdir = os.path.join(app_path, "map")

if not os.path.isdir(mapdir):
    os.makedirs(mapdir)


HTML_HEAD = """
<html>
  <head><title>Memory Region Analyze</title>
   <link rel="stylesheet" type="text/css" href="df_style.css"/>
   <style type="text/css">
    body {
      font-family: "Microsoft YaHei", monospace;
      background: #f7f9fb;
    }
    table {
      border-collapse: collapse;
      width: 100%;
    }
    th, td {
      padding: 4px 8px;
      border: 1px solid #d8dde6;
      text-align: left;
    }
    th {
      background: #e8edf3;
    }
   </style>
  </head>
  <body>
"""

HTML_BODY = """
    <table width="100%" align="center">
    <tr>
    <td width="100%" style="vertical-align:top; float:left; margin:0px; padding: 0px;">
    <font face=Verdana>{table1}</font>
    </td>
    </tr>
    </table>
"""

HTML_FOOTER = """
    </body>
</html>
"""


def mixed_basename(path):
    parts = re.split(r"[\\/]+", path.strip())
    return parts[-1] if parts and parts[-1] else path.strip()


def ordered_unique(items):
    seen = set()
    result = []
    for item in items:
        if item not in seen:
            seen.add(item)
            result.append(item)
    return result


def read_text_file(path):
    encodings = ("utf-8", "gbk", "latin-1")
    last_error = None
    for encoding in encodings:
        try:
            with open(path, "r", encoding=encoding) as handle:
                return handle.read().replace("\r\n", "\n")
        except UnicodeDecodeError as error:
            last_error = error
    raise last_error


def extract_first_group(text, patterns, label):
    for pattern in patterns:
        match = re.search(pattern, text)
        if match:
            return match.group(1)
    raise ValueError("未找到 {0} 区域".format(label))


def split_archive_object(text):
    cleaned = text.strip()
    match = re.match(r"^(.*?)(?:\(([^()]*)\))?$", cleaned)
    if not match:
        return cleaned, mixed_basename(cleaned), mixed_basename(cleaned)

    archive_path = (match.group(1) or cleaned).strip()
    object_name = (match.group(2) or mixed_basename(archive_path)).strip()
    archive_name = mixed_basename(archive_path)
    return archive_path, archive_name, object_name


def ensure_dir(path):
    if path and not os.path.isdir(path):
        os.makedirs(path)


def display_text(text):
    if sys.stdout.isatty():
        pager(text)
    else:
        print(text)


def rows_to_text(rows, columns):
    if not rows:
        return "(empty)"

    widths = {}
    for column in columns:
        widths[column] = len(column)

    for row in rows:
        for column in columns:
            widths[column] = max(widths[column], len(str(row.get(column, ""))))

    header = "  ".join(column.ljust(widths[column]) for column in columns)
    separator = "  ".join("-" * widths[column] for column in columns)
    body = []
    for row in rows:
        body.append("  ".join(str(row.get(column, "")).ljust(widths[column]) for column in columns))
    return "\n".join([header, separator] + body)


def write_csv_rows(path, columns, rows):
    with open(path, "w", encoding="utf-8", newline="") as handle:
        writer = csv.DictWriter(handle, fieldnames=columns)
        writer.writeheader()
        for row in rows:
            writer.writerow(dict((column, row.get(column, "")) for column in columns))


def rows_to_html(rows, columns):
    head = "".join("<th>{0}</th>".format(escape(column)) for column in columns)
    body = []
    for row in rows:
        cells = "".join("<td>{0}</td>".format(escape(str(row.get(column, "")))) for column in columns)
        body.append("<tr>{0}</tr>".format(cells))
    return "<table bgcolor=\"#F7F9FB\" cellspacing=\"0\"><thead><tr>{0}</tr></thead><tbody>{1}</tbody></table>".format(
        head,
        "".join(body),
    )


def render_tree(title, tree):
    lines = [title]
    if not tree:
        lines.append("(empty)")
        return "\n".join(lines)

    def _walk(node, prefix):
        items = list(node.items())
        for index, (label, child) in enumerate(items):
            last = index == len(items) - 1
            connector = "`- " if last else "|- "
            lines.append(prefix + connector + label)
            next_prefix = prefix + ("   " if last else "|  ")
            if child:
                _walk(child, next_prefix)

    _walk(tree, "")
    return "\n".join(lines)


def iter_tree_paths(tree, path=None):
    current_path = path or []
    for label, child in tree.items():
        next_path = current_path + [label]
        yield next_path
        if child:
            for item in iter_tree_paths(child, next_path):
                yield item


def prompt_existing_path(prompt_text):
    while True:
        raw = input(prompt_text).strip().strip('"').strip("'")
        if not raw:
            print("请输入有效路径")
            continue

        path = os.path.abspath(os.path.expanduser(raw))
        if os.path.isfile(path):
            return path
        print("文件不存在: {0}".format(path))


def extract_symbol_size_map(content):
    try:
        mem_map = extract_first_group(
            content,
            [
                r"Linker script and memory map([\s\S]+?)OUTPUT\(",
                r"链结器命令稿和内存映射([\s\S]+?)OUTPUT\(",
            ],
            "Linker script and memory map",
        )
    except ValueError:
        return {}

    size_by_symbol = {}
    pattern = re.compile(
        r"[ \t]+(0x[0-9A-Fa-f]+)[ \t]+(0x[0-9A-Fa-f]+)[ \t]+(\S+)\n"
        r"[ \t]+(0x[0-9A-Fa-f]+)[ \t]+([^\s=][^\n=]*)",
        re.MULTILINE,
    )
    for match in pattern.finditer(mem_map):
        symbol = match.group(5).strip()
        size_by_symbol[symbol] = int(match.group(2), 16)
    return size_by_symbol


class CrossReferenceResult(object):

    def __init__(
        self,
        filepath,
        original_tree,
        parsed_tree,
        symbol_sources,
        archive_paths,
        object_index,
        size_by_symbol,
        export_paths,
    ):
        self.filepath = filepath
        self.original_tree = original_tree
        self.parsed_tree = parsed_tree
        self.symbol_sources = symbol_sources
        self.archive_paths = archive_paths
        self.object_index = object_index
        self.size_by_symbol = size_by_symbol
        self.export_paths = export_paths

    def render_original(self):
        title = "Cross Reference Table (original view): {0}".format(os.path.basename(self.filepath))
        return render_tree(title, self.original_tree)

    def render_parsed(self):
        title = "Cross Reference Table (parsed view): {0}".format(os.path.basename(self.filepath))
        return render_tree(title, self.parsed_tree)

    def search(self, keyword):
        needle = keyword.lower()
        results = []
        for tree_name, tree in (("original", self.original_tree), ("parsed", self.parsed_tree)):
            for path in iter_tree_paths(tree):
                if needle in path[-1].lower():
                    results.append("{0}: {1}".format(tree_name, " > ".join(path)))
        return results

    def describe(self, label):
        lines = ["Label: {0}".format(label)]
        exact_paths = self.search(label)
        exact_paths = [item for item in exact_paths if item.lower().endswith(label.lower())]

        if label in self.symbol_sources:
            size = self.size_by_symbol.get(label)
            lines.append("type: symbol")
            lines.append("size: {0}".format(size if size is not None else "unknown"))
            lines.append("sources:")
            for source in self.symbol_sources[label]:
                lines.append("  {0}".format(source))

        if label in self.archive_paths:
            lines.append("type: archive")
            lines.append("path: {0}".format(self.archive_paths[label]))

        if label in self.object_index:
            lines.append("type: object")
            lines.append("symbols:")
            for symbol in self.object_index[label]:
                lines.append("  {0}".format(symbol))

        if exact_paths:
            lines.append("tree paths:")
            for path in exact_paths:
                lines.append("  {0}".format(path))

        if len(lines) == 1:
            lines.append("未找到精确匹配，建议先用 search 命令模糊搜索")
        return "\n".join(lines)


def parse_cross_reference(filepath):
    content = read_text_file(filepath)
    size_by_symbol = extract_symbol_size_map(content)
    lines = content.splitlines()

    start_index = None
    for index, line in enumerate(lines):
        if re.search(r"Symbol\s+File", line) or re.search(r"符号\s+文件", line):
            start_index = index + 1
            break

    if start_index is None:
        raise ValueError("未找到 Symbol/File 表")

    entries = []
    current_symbol = ""
    for raw_line in lines[start_index:]:
        if not raw_line.strip():
            continue

        match = re.match(r"^(\S+)\s{2,}(.+?)\s*$", raw_line)
        if match:
            current_symbol = match.group(1).strip()
            file_full = match.group(2).strip()
        else:
            if not current_symbol:
                continue
            file_full = raw_line.strip()

        archive_path, archive_name, object_name = split_archive_object(file_full)
        entries.append(
            {
                "func": current_symbol,
                "file_full": file_full,
                "archive_path": archive_path,
                "archive_name": archive_name,
                "object_name": object_name,
                "size": size_by_symbol.get(current_symbol),
            }
        )

    if not entries:
        raise ValueError("Symbol/File 表为空")

    symbol_to_entries = OrderedDict()
    archive_paths = OrderedDict()
    object_index = defaultdict(list)

    for entry in entries:
        symbol_to_entries.setdefault(entry["func"], []).append(entry)
        archive_paths.setdefault(entry["archive_name"], entry["archive_path"])
        object_index[entry["object_name"]].append(entry["func"])

    original_tree = OrderedDict()
    parsed_tree = OrderedDict()
    symbol_sources = OrderedDict()

    for symbol in sorted(symbol_to_entries):
        symbol_entries = symbol_to_entries[symbol]
        providers = ordered_unique([item["archive_name"] for item in symbol_entries])
        provider_label = " + ".join(providers) if providers else "(unknown)"
        objects = sorted(set(item["object_name"] for item in symbol_entries if item["object_name"]))
        symbol_sources[symbol] = ordered_unique([item["file_full"] for item in symbol_entries])

        provider_node = original_tree.setdefault(provider_label, OrderedDict())
        symbol_node = provider_node.setdefault(symbol, OrderedDict())
        for object_name in objects:
            symbol_node.setdefault(object_name, OrderedDict())

        for provider in providers:
            parsed_provider_node = parsed_tree.setdefault(provider, OrderedDict())
            if provider_label == provider:
                parsed_symbol_node = parsed_provider_node.setdefault(symbol, OrderedDict())
            else:
                parsed_group_node = parsed_provider_node.setdefault(provider_label, OrderedDict())
                parsed_symbol_node = parsed_group_node.setdefault(symbol, OrderedDict())
            provider_objects = sorted(
                set(
                    item["object_name"]
                    for item in symbol_entries
                    if item["archive_name"] == provider and item["object_name"]
                )
            )
            for object_name in provider_objects:
                if object_name:
                    parsed_symbol_node.setdefault(object_name, OrderedDict())

    object_index = OrderedDict(
        (name, sorted(set(symbols))) for name, symbols in sorted(object_index.items()) if name
    )

    base = os.path.join(mapdir, os.path.splitext(os.path.basename(filepath))[0])
    export_paths = {
        "original_txt": base + "_crt_original.txt",
        "parsed_txt": base + "_crt_parsed.txt",
        "symbols_csv": base + "_crt_symbols.csv",
    }

    with open(export_paths["original_txt"], "w", encoding="utf-8") as handle:
        handle.write(render_tree("Cross Reference Table (original view)", original_tree))

    with open(export_paths["parsed_txt"], "w", encoding="utf-8") as handle:
        handle.write(render_tree("Cross Reference Table (parsed view)", parsed_tree))

    write_csv_rows(
        export_paths["symbols_csv"],
        ["func", "file_full", "archive_name", "object_name", "size"],
        entries,
    )

    return CrossReferenceResult(
        filepath=filepath,
        original_tree=original_tree,
        parsed_tree=parsed_tree,
        symbol_sources=symbol_sources,
        archive_paths=archive_paths,
        object_index=object_index,
        size_by_symbol=size_by_symbol,
        export_paths=export_paths,
    )


class MemoryMapSession(object):

    def __init__(self, filepath):
        self.filepath = filepath
        self.filename = os.path.basename(filepath)
        self.archive_rows = []
        self.object_rows = []
        self.metric_columns = []
        self.metric_limits = OrderedDict()
        self._load()

    def _load(self):
        content = read_text_file(self.filepath)
        mem_map = extract_first_group(
            content,
            [
                r"Linker script and memory map([\s\S]+?)OUTPUT\(",
                r"链结器命令稿和内存映射([\s\S]+?)OUTPUT\(",
            ],
            "Linker script and memory map",
        )

        list_fill = re.findall(r"(\*fill\*)[ \t]+(0x[0-9A-Fa-f]+)[ \t]+(0x[0-9A-Fa-f]+)[ \t]+\n", mem_map)
        list_module = re.findall(r"(0x[0-9A-Fa-f]+)[ \t]+(0x[0-9A-Fa-f]+)[ \t]+(\S+)\n", mem_map)

        if not list_module and not list_fill:
            raise ValueError("未解析出 Memory Map 内容")

        archive_rows = []
        object_rows = []

        for address_hex, size_hex, module_full in list_module:
            address = int(address_hex, 16)
            size = int(size_hex, 16)
            _, archive_name, object_name = split_archive_object(module_full)
            archive_rows.append({"address": address, "size": size, "module": archive_name})
            object_rows.append(
                {
                    "address": address,
                    "size": size,
                    "module": archive_name,
                    "name": object_name,
                }
            )

        for module_full, address_hex, size_hex in list_fill:
            address = int(address_hex, 16)
            size = int(size_hex, 16)
            archive_rows.append({"address": address, "size": size, "module": module_full})
            object_rows.append(
                {
                    "address": address,
                    "size": size,
                    "module": module_full,
                    "name": module_full,
                }
            )

        self.archive_rows = archive_rows
        self.object_rows = object_rows
        self._assign_memory_regions(content)

    def _assign_memory_regions(self, content):
        metric_columns = []
        metric_limits = OrderedDict()

        mem_config_text = ""
        for pattern in (
            r"(?:Memory Configuration|内存配置).*?[\r\n]+([\s\S]+?)\nLinker script and memory map",
            r"属性\n([\s\S]+?)\n链结器命令稿和内存映射",
        ):
            match = re.search(pattern, content)
            if match:
                mem_config_text = "\n" + match.group(1)
                break

        ram_config = re.findall(r"\n(\S+)\s+(0x[0-9A-Fa-f]+)\s+(0x[0-9A-Fa-f]+)", mem_config_text)

        for region_name, origin_hex, length_hex in ram_config:
            if "default" in region_name.lower():
                continue
            origin = int(origin_hex, 16)
            limit = int(length_hex, 16)
            end = origin + limit
            metric_columns.append(region_name)
            metric_limits[region_name] = limit
            for row in self.archive_rows:
                row[region_name] = row["size"] if origin <= row["address"] < end else 0
            for row in self.object_rows:
                row[region_name] = row["size"] if origin <= row["address"] < end else 0

        if not metric_columns:
            metric_columns = ["size"]
            metric_limits = OrderedDict()

        self.metric_columns = metric_columns
        self.metric_limits = metric_limits

    def sort_columns(self, mode):
        base = ["module"]
        if mode == "ofile":
            base.append("name")
        return base + list(self.metric_columns)

    def build_table(self, mode, sort=None):
        if mode not in ("afile", "ofile"):
            raise ValueError("mode 仅支持 afile 或 ofile")

        metrics = list(self.metric_columns)
        if mode == "afile":
            columns = ["module"] + metrics
            source_rows = self.archive_rows
            group_fields = ["module"]
        else:
            columns = ["module", "name"] + metrics
            source_rows = self.object_rows
            group_fields = ["module", "name"]

        grouped_map = OrderedDict()
        for row in source_rows:
            key = tuple(row.get(field, "") for field in group_fields)
            if key not in grouped_map:
                grouped_map[key] = OrderedDict((field, key[index]) for index, field in enumerate(group_fields))
                for metric in metrics:
                    grouped_map[key][metric] = 0
            for metric in metrics:
                grouped_map[key][metric] += int(row.get(metric, 0))

        grouped_rows = list(grouped_map.values())
        if not grouped_rows:
            return columns, []

        if sort is None:
            sort = columns[0]

        if sort not in columns:
            raise ValueError("无效排序列: {0}".format(sort))

        reverse = sort in metrics
        grouped_rows.sort(key=lambda row: row.get(sort, ""), reverse=reverse)

        total_row = OrderedDict((column, "") for column in columns)
        total_row[columns[0]] = "total"
        for metric in metrics:
            total_row[metric] = sum(int(row.get(metric, 0)) for row in grouped_rows)

        table_rows = list(grouped_rows)

        if self.metric_limits:
            limit_row = OrderedDict((column, "") for column in columns)
            limit_row[columns[0]] = "limit"
            for metric, limit in self.metric_limits.items():
                if metric in columns:
                    limit_row[metric] = limit
            table_rows.append(limit_row)

        table_rows.append(total_row)
        return columns, table_rows

    def export(self, mode, sort=None):
        columns, rows = self.build_table(mode, sort=sort)
        base = os.path.join(mapdir, os.path.splitext(self.filename)[0] + "_" + mode)
        paths = {
            "csv": base + ".csv",
            "txt": base + ".txt",
            "html": base + ".html",
        }

        write_csv_rows(paths["csv"], columns, rows)
        with open(paths["txt"], "w", encoding="utf-8") as handle:
            handle.write(rows_to_text(rows, columns))
        html_table = rows_to_html(rows, columns)
        with open(paths["html"], "w", encoding="utf-8") as handle:
            handle.write(HTML_HEAD + HTML_BODY.format(table1=html_table) + HTML_FOOTER)

        return columns, rows, paths


class CrossReferenceShell(cmd.Cmd):
    prompt = "(crt) "

    def __init__(self, result):
        cmd.Cmd.__init__(self)
        self.result = result
        self.intro = (
            "已载入 Cross Reference Table\n"
            "命令: show original | show parsed | search <keyword> | info <label> | export | back"
        )

    def do_show(self, arg):
        value = arg.strip().lower()
        if value == "original":
            display_text(self.result.render_original())
            return
        if value == "parsed":
            display_text(self.result.render_parsed())
            return
        print("用法: show original | show parsed")

    def do_search(self, arg):
        keyword = arg.strip()
        if not keyword:
            print("用法: search <keyword>")
            return
        results = self.result.search(keyword)
        if not results:
            print("未找到匹配项")
            return
        display_text("\n".join(results))

    def do_info(self, arg):
        label = arg.strip()
        if not label:
            print("用法: info <label>")
            return
        display_text(self.result.describe(label))

    def do_export(self, arg):
        lines = ["已导出文件:"]
        for name, path in sorted(self.result.export_paths.items()):
            lines.append("{0}: {1}".format(name, path))
        print("\n".join(lines))

    def do_back(self, arg):
        return True

    def do_quit(self, arg):
        return True

    def do_exit(self, arg):
        return True

    def do_EOF(self, arg):
        print("")
        return True


class MemoryMapShell(cmd.Cmd):
    prompt = "(map) "

    def __init__(self, session, mode, default_sort=None):
        cmd.Cmd.__init__(self)
        self.session = session
        self.mode = mode
        self.sort = default_sort or session.sort_columns(mode)[0]
        self.columns, self.table_rows, self.export_paths = self.session.export(self.mode, self.sort)
        self.intro = (
            "已载入 Memory Map ({0})\n"
            "命令: show | columns | sort <column> | export | back".format(self.mode)
        )

    def do_show(self, arg):
        display_text(rows_to_text(self.table_rows, self.columns))

    def do_columns(self, arg):
        print("可排序列: {0}".format(", ".join(self.session.sort_columns(self.mode))))

    def do_sort(self, arg):
        column = arg.strip()
        if not column:
            print("用法: sort <column>")
            return
        self.columns, self.table_rows, self.export_paths = self.session.export(self.mode, column)
        self.sort = column
        print("已按 {0} 重排并更新导出文件".format(column))

    def do_export(self, arg):
        lines = ["已导出文件:"]
        for name, path in sorted(self.export_paths.items()):
            lines.append("{0}: {1}".format(name, path))
        print("\n".join(lines))

    def do_back(self, arg):
        return True

    def do_quit(self, arg):
        return True

    def do_exit(self, arg):
        return True

    def do_EOF(self, arg):
        print("")
        return True


def run_tui():
    while True:
        print("")
        print("{0} TUI".format(APP_NAME))
        print("1. Cross Reference Table")
        print("2. Memory Map (.a)")
        print("3. Memory Map (.o)")
        print("q. Exit")
        choice = input("请选择功能: ").strip().lower()

        if choice == "1":
            try:
                filepath = prompt_existing_path("请输入 .map 文件路径: ")
                result = parse_cross_reference(filepath)
                CrossReferenceShell(result).cmdloop()
            except Exception as error:
                print("解析失败: {0}".format(error))
        elif choice == "2":
            try:
                filepath = prompt_existing_path("请输入 .map 文件路径: ")
                session = MemoryMapSession(filepath)
                MemoryMapShell(session, "afile").cmdloop()
            except Exception as error:
                print("解析失败: {0}".format(error))
        elif choice == "3":
            try:
                filepath = prompt_existing_path("请输入 .map 文件路径: ")
                session = MemoryMapSession(filepath)
                MemoryMapShell(session, "ofile").cmdloop()
            except Exception as error:
                print("解析失败: {0}".format(error))
        elif choice in ("q", "quit", "exit"):
            return 0
        else:
            print("无效选项")


def build_parser():
    parser = argparse.ArgumentParser(description="{0} TUI/CLI".format(APP_NAME))
    subparsers = parser.add_subparsers(dest="command")

    parser_tui = subparsers.add_parser("tui", help="进入终端交互界面")
    parser_tui.set_defaults(command="tui")

    parser_crt = subparsers.add_parser("crt", help="解析 Cross Reference Table")
    parser_crt.add_argument("map_file", help="map 文件路径")
    parser_crt.add_argument(
        "--tree",
        choices=["original", "parsed"],
        default="original",
        help="输出树视图",
    )
    parser_crt.add_argument(
        "--search",
        default="",
        help="可选，附带搜索关键字",
    )

    for command_name in ("afile", "ofile"):
        parser_map = subparsers.add_parser(command_name, help="解析 Memory Map ({0})".format(command_name))
        parser_map.add_argument("map_file", help="map 文件路径")
        parser_map.add_argument("--sort", default="", help="排序列名")

    return parser


def main(argv=None):
    parser = build_parser()
    args = parser.parse_args(argv)

    if not args.command:
        return run_tui()

    if args.command == "tui":
        return run_tui()

    if args.command == "crt":
        result = parse_cross_reference(os.path.abspath(args.map_file))
        output = result.render_original() if args.tree == "original" else result.render_parsed()
        display_text(output)
        if args.search:
            matches = result.search(args.search)
            if matches:
                print("\nSearch Results:")
                print("\n".join(matches))
            else:
                print("\nSearch Results:\n(no match)")
        print("\nExports:")
        for name, path in sorted(result.export_paths.items()):
            print("{0}: {1}".format(name, path))
        return 0

    if args.command in ("afile", "ofile"):
        session = MemoryMapSession(os.path.abspath(args.map_file))
        sort = args.sort or session.sort_columns(args.command)[0]
        columns, rows, paths = session.export(args.command, sort)
        display_text(rows_to_text(rows, columns))
        print("\nExports:")
        for name, path in sorted(paths.items()):
            print("{0}: {1}".format(name, path))
        return 0

    parser.print_help()
    return 1


if __name__ == "__main__":
    sys.exit(main())
