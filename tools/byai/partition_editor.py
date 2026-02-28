#!/usr/bin/env python3
"""
分区配置交互式编辑器
支持左右键切换分区，上下键调整大小，q保存退出
"""

import curses
import sys
import os
import toml
from pathlib import Path

# 对齐粒度 (1KB)
ALIGNMENT = 0x1000

# Flash总大小 (4MB)
FLASH_SIZE = 4 * 1024 * 1024  # 0x400000

class PartitionEditor:
    def __init__(self, toml_path):
        self.toml_path = toml_path
        self.config = toml.load(toml_path)
        self.partitions = self.config.get('pt_entry', [])

        # 找出主分区（device=0且有size0的）
        self.main_partitions = []
        self.fw_idx = None
        self.mfg_idx = None

        for idx, pt in enumerate(self.partitions):
            if pt.get('device', 0) == 0 and pt.get('size0', 0) > 0:
                pt_name = pt.get('name', '')
                if pt_name == 'FW':
                    self.fw_idx = idx
                elif pt_name == 'mfg':
                    self.mfg_idx = idx

                self.main_partitions.append({
                    'index': idx,
                    'type': pt.get('type', 0),
                    'name': pt_name,
                    'address0': pt.get('address0', 0),
                    'size0': pt.get('size0', 0),
                })

        # 当前选中的分区索引
        self.selected_idx = 0

        # 步长（每次调整的大小，默认4KB）
        self.step = 0x1000

    def get_flash_usage(self):
        """计算flash使用情况"""
        total_used = 0
        for pt in self.main_partitions:
            total_used += pt['size0']
        return total_used

    def get_free_space(self):
        """获取剩余空间"""
        return FLASH_SIZE - self.get_flash_usage()

    def adjust_partition_size(self, direction):
        """
        调整分区大小
        direction: 1 为增加当前分区（减少下一个分区），-1 为减少当前分区（增加下一个分区）
        """
        if self.selected_idx >= len(self.main_partitions) - 1:
            return False

        current = self.main_partitions[self.selected_idx]
        next_pt = self.main_partitions[self.selected_idx + 1]

        # 计算当前分区可以调整的最大值
        # 需要保留至少一个对齐粒度给下一个分区
        max_adjust = next_pt['size0'] - ALIGNMENT

        # 向上：增加当前分区，减少下一个分区
        if direction == 1:
            if max_adjust >= self.step:
                current['size0'] += self.step
                next_pt['size0'] -= self.step
                # 更新下一个分区的起始地址
                next_pt['address0'] = current['address0'] + current['size0']
                return True
        # 向下：减少当前分区，增加下一个分区
        elif direction == -1:
            if current['size0'] >= self.step + ALIGNMENT:
                current['size0'] -= self.step
                next_pt['size0'] += self.step
                # 更新下一个分区的起始地址
                next_pt['address0'] = current['address0'] + current['size0']
                return True

        return False

    def move_selection(self, direction):
        """移动选择"""
        new_idx = self.selected_idx + direction
        if 0 <= new_idx < len(self.main_partitions):
            self.selected_idx = new_idx
            return True
        return False

    def set_step(self, step):
        """设置调整步长"""
        self.step = step

    def save(self):
        """保存配置到TOML文件，保持原文件格式和注释"""
        # 更新原始配置的 address0 和 size0
        for pt in self.main_partitions:
            orig_idx = pt['index']
            self.partitions[orig_idx]['address0'] = pt['address0']
            self.partitions[orig_idx]['size0'] = pt['size0']

        # 自动更新 FW 分区的 address1 和 size1 为 mfg 分区的 address0 和 size0
        if self.fw_idx is not None and self.mfg_idx is not None:
            mfg_partition = self.partitions[self.mfg_idx]
            fw_partition = self.partitions[self.fw_idx]
            fw_partition['address1'] = mfg_partition['address0']
            fw_partition['size1'] = mfg_partition['size0']

        # 备份原文件
        backup_path = self.toml_path + '.bak'
        if os.path.exists(backup_path):
            os.remove(backup_path)
        import shutil
        shutil.copy2(self.toml_path, backup_path)

        # 构建替换映射：key -> hex value
        replacements = {}
        for idx, pt in enumerate(self.partitions):
            pt_name = pt.get('name', '')
            replacements[(idx, 'address0')] = f"0x{pt['address0']:X}"
            replacements[(idx, 'size0')] = f"0x{pt['size0']:X}"
            replacements[(idx, 'address1')] = f"0x{pt['address1']:X}"
            replacements[(idx, 'size1')] = f"0x{pt['size1']:X}"

        # 逐行读取并替换
        with open(self.toml_path, 'r') as f:
            lines = f.readlines()

        # 替换指定字段
        current_partition_idx = -1
        output_lines = []
        for line in lines:
            # 检测新的分区开始 [[pt_entry]]
            if line.strip().startswith('[[') and 'pt_entry' in line:
                current_partition_idx += 1
                output_lines.append(line)
            # 检测字段行并替换
            elif line.strip().startswith(('address0', 'size0', 'address1', 'size1')):
                # 提取字段名
                field_name = line.split('=')[0].strip()
                key = (current_partition_idx, field_name)
                if key in replacements:
                    # 保持原有的缩进和格式，只替换值
                    indent = len(line) - len(line.lstrip())
                    value = replacements[key]
                    # 检查原行是否有 address1 = 0 这种格式
                    if ' = ' in line:
                        output_lines.append(' ' * indent + f"{field_name} = {value}\n")
                    else:
                        output_lines.append(line)
                else:
                    output_lines.append(line)
            else:
                output_lines.append(line)

        # 写入新配置
        with open(self.toml_path, 'w') as f:
            f.writelines(output_lines)

        return True

    def render(self, stdscr):
        """渲染界面"""
        stdscr.clear()
        height, width = stdscr.getmaxyx()

        # 标题
        title = f"分区配置编辑器 - {self.toml_path}"
        stdscr.addstr(0, (width - len(title)) // 2, title, curses.A_BOLD)

        # Flash使用情况
        info_line1 = f"Flash大小: {FLASH_SIZE // 1024}KB"
        stdscr.addstr(2, 2, info_line1)

        info_line2 = f"调整步长: {self.step // 1024}KB  当前选择: [{self.selected_idx}] {self.main_partitions[self.selected_idx]['name']}"
        stdscr.addstr(3, 2, info_line2)

        # 显示 FW 和 mfg 分区的关联关系
        if self.fw_idx is not None and self.mfg_idx is not None:
            mfg_pt = next((pt for pt in self.main_partitions if pt['name'] == 'mfg'), None)
            if mfg_pt:
                info_line3 = f"提示: FW.address1/size1 将自动设置为 mfg.address0/size0 = 0x{mfg_pt['address0']:08X}/{mfg_pt['size0'] // 1024}KB"
                stdscr.addstr(4, 2, info_line3, curses.A_BOLD)

        # 分区列表表头
        header = f"{'序号':>4}  {'名称':>6}  {'起始地址':>10}  {'大小(KB)':>8}  {'结束地址':>4}"
        stdscr.addstr(6, 2, header, curses.A_REVERSE)

        # 分区列表
        for i, pt in enumerate(self.main_partitions):
            name = pt['name'][:10]  # 限制名称长度
            line = f"{i:>4}  {name:>10}       0x{pt['address0']:08X}  {pt['size0'] // 1024:>8}  0x{pt['address0'] + pt['size0']:08X}"

            y = 7 + i
            if i == self.selected_idx:
                stdscr.addstr(y, 2, line, curses.A_REVERSE)
            else:
                stdscr.addstr(y, 2, line)

        # 帮助信息
        help_y = height - 7
        help_text = [
            "操作说明:",
            "  ↑/↓      : 切换选中分区",
            "  ←/→      : 增加/减少当前分区大小（从下一个分区调整）",
            "  1-8      : 设置调整步长 (1/2/4/8/16/32/64/128 KB)",
            "  q        : 保存并退出 (FW的address1/size1将自动设置为mfg的address0/size0)",
            "            原文件会备份为 .bak",
            "  ESC      : 不保存退出"
        ]
        for i, text in enumerate(help_text):
            stdscr.addstr(help_y + i, 2, text)

        stdscr.refresh()


def main(stdscr):
    # 初始化curses
    curses.curs_set(0)  # 隐藏光标
    stdscr.nodelay(0)   # 阻塞模式
    stdscr.keypad(True) # 启用功能键

    # 获取命令行参数
    if len(sys.argv) < 2:
        stdscr.addstr(0, 0, "错误: 未指定分区配置文件")
        stdscr.addstr(2, 0, "用法: python3 partition_editor.py <分区配置文件.toml>")
        stdscr.addstr(4, 0, "示例: python3 partition_editor.py bsp/board/bl616dk/config/partition_cfg_4M.toml")
        stdscr.addstr(6, 0, "按任意键退出...")
        stdscr.refresh()
        stdscr.getch()
        return 1

    toml_path = sys.argv[1]

    if not os.path.exists(toml_path):
        stdscr.addstr(0, 0, f"错误: 文件不存在: {toml_path}")
        stdscr.addstr(2, 0, "请检查文件路径是否正确")
        stdscr.addstr(4, 0, "按任意键退出...")
        stdscr.refresh()
        stdscr.getch()
        return 1

    # 创建编辑器
    editor = PartitionEditor(toml_path)

    # 主循环
    while True:
        editor.render(stdscr)

        key = stdscr.getch()

        # 方向键
        if key == curses.KEY_UP:
            editor.move_selection(-1)
        elif key == curses.KEY_DOWN:
            editor.move_selection(1)
        elif key == curses.KEY_RIGHT:
            editor.adjust_partition_size(1)  # 增加当前分区
        elif key == curses.KEY_LEFT:
            editor.adjust_partition_size(-1)  # 减少当前分区

        # 数字键设置步长
        elif key == ord('1'):
            editor.set_step(0x400)     # 1KB
        elif key == ord('2'):
            editor.set_step(0x800)     # 2KB
        elif key == ord('3'):
            editor.set_step(0x1000)    # 4KB
        elif key == ord('4'):
            editor.set_step(0x2000)    # 8KB
        elif key == ord('5'):
            editor.set_step(0x4000)    # 16KB
        elif key == ord('6'):
            editor.set_step(0x8000)    # 32KB
        elif key == ord('7'):
            editor.set_step(0x10000)   # 64KB
        elif key == ord('8'):
            editor.set_step(0x20000)   # 128KB

        # 退出
        elif key == ord('q') or key == ord('Q'):
            # 保存并退出
            editor.save()
            stdscr.addstr(20, 2, "配置已保存！按任意键退出...")
            stdscr.refresh()
            stdscr.getch()
            return 0
        elif key == 27:  # ESC
            # 不保存退出
            return 1


if __name__ == "__main__":
    try:
        # 检查是否安装了toml库
        import toml
    except ImportError:
        print("错误: 需要安装 toml 库")
        print("请运行: pip install toml")
        sys.exit(1)

    try:
        result = curses.wrapper(main)
        sys.exit(result)
    except KeyboardInterrupt:
        sys.exit(1)
    except Exception as e:
        print(f"错误: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)
