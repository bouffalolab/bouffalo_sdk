#!/usr/bin/env python3
"""
自定义二进制文件镜像管理工具
在 bin 文件的指定偏移处维护一个镜像描述符数组

描述符格式（16字节）:
  - char[8] name        : 名称（最多7字符）
  - uint32_t start_addr : 起始地址
  - uint32_t end_addr   : 结束地址

数组以四个 0xFFFFFFFF 结束（16字节）
"""

import struct
import os
import sys
import argparse
from pathlib import Path
from typing import Optional, Tuple, List, Dict

# 全局 debug 变量，设置为 1 时打印详细日志
debug = 0

class CustomBinManager:
    """自定义 BIN 文件管理器"""

    # 数组在文件中的默认偏移量：0x1200
    DEFAULT_ARRAY_OFFSET = 0x1200

    # 描述符结构：8字节名称 + 4字节起始地址 + 4字节结束地址
    DESCRIPTOR_FORMAT = "8sII"
    DESCRIPTOR_SIZE = struct.calcsize(DESCRIPTOR_FORMAT)

    # 结束标记：四个 0xFFFFFFFF（16字节）
    END_MARKER = 0xFFFFFFFF
    END_MARKER_COUNT = 4
    END_MARKER_SIZE = END_MARKER_COUNT * 4

    # 默认对齐大小：4KB
    DEFAULT_ALIGNMENT = 0x1000

    @staticmethod
    def parse_offset(offset_str: str) -> int:
        """解析偏移量字符串，支持 0x 前缀"""
        offset_str = offset_str.strip()
        if offset_str.startswith('0x') or offset_str.startswith('0X'):
            return int(offset_str, 16)
        else:
            return int(offset_str)

    @staticmethod
    def align_up(value: int, alignment: int) -> int:
        """向上对齐到指定大小"""
        return ((value + alignment - 1) // alignment) * alignment

    @staticmethod
    def inspect_bin_file(bin_path: str, array_offset: Optional[int] = None):
        """
        查看 bin 文件中的描述符数组

        参数:
            bin_path: bin 文件路径
            array_offset: 数组偏移量（None 则使用默认值）
        """
        if array_offset is None:
            array_offset = CustomBinManager.DEFAULT_ARRAY_OFFSET

        if debug:
            print(f"文件: {bin_path}")

        # 读取文件
        try:
            with open(bin_path, 'rb') as f:
                data = f.read()
        except FileNotFoundError:
            print(f"错误: 文件不存在: {bin_path}")
            return
        except Exception as e:
            print(f"错误: 无法读取文件: {e}")
            return

        file_size = len(data)
        if debug:
            print(f"大小: {file_size} 字节 (0x{file_size:X})")
            print(f"数组偏移: 0x{array_offset:X} ({array_offset})")

        # 检查偏移量是否有效
        if array_offset >= file_size:
            print(f"错误: 文件太小，无法在偏移量 {array_offset} 处找到数组")
            return

        if debug:
            print("=" * 80)

        # 读取描述符
        offset = array_offset
        index = 0

        while offset + CustomBinManager.DESCRIPTOR_SIZE <= file_size:
            try:
                # 读取描述符: { char[8] name, uint32_t start_addr, uint32_t end_addr }
                name_bytes, start_addr, end_addr = struct.unpack_from(
                    CustomBinManager.DESCRIPTOR_FORMAT,
                    data,
                    offset
                )

                # 检查结束标记（name字段全为0xFF，start_addr和end_addr都是0xFFFFFFFF）
                if (start_addr == CustomBinManager.END_MARKER and
                    end_addr == CustomBinManager.END_MARKER and
                    name_bytes == b'\xff' * 8):
                    if debug:
                        print(f"[结束标记] 找到结束标记")
                        print(f"  偏移: 0x{offset:X}")
                        print(f"  数组总项数: {index}")
                    break

                # 解码名称
                try:
                    name = name_bytes.decode('ascii').rstrip('\x00')
                except UnicodeDecodeError:
                    name = f"<无效编码: {name_bytes.hex()}>"

                # 显示信息
                if debug:
                    print(f"[{index}] {name}")
                    print(f"  起始地址: 0x{start_addr:08X} ({start_addr})")
                    print(f"  结束地址: 0x{end_addr:08X} ({end_addr})")

                    if start_addr == 0 and end_addr == 0:
                        size = 0
                        print(f"  状态: 未分配")
                    elif end_addr > start_addr:
                        size = end_addr - start_addr
                        print(f"  大小: {size} 字节 (0x{size:X})")

                        # 验证地址范围
                        if start_addr >= file_size:
                            print(f"  警告: 起始地址超出文件范围")
                        elif end_addr > file_size:
                            print(f"  警告: 结束地址超出文件范围")
                    else:
                        print(f"  状态: 无效地址范围")

                    print(f"  描述符偏移: 0x{offset:X}")
                    print()

                offset += CustomBinManager.DESCRIPTOR_SIZE
                index += 1

                # 安全限制
                if index > 1000:
                    print(f"警告: 超过1000个描述符，停止读取")
                    break

            except Exception as e:
                print(f"错误: 在偏移 0x{offset:X} 处读取失败: {e}")
                break

        # 显示文件摘要
        if debug:
            print("=" * 80)
            print(f"文件摘要:")
            print(f"  文件大小: {file_size} 字节 (0x{file_size:X})")
            print(f"  描述符数量: {index}")

    @staticmethod
    def read_descriptors(data: bytes, array_offset: Optional[int] = None) -> List[Dict]:
        """
        从二进制数据中读取描述符数组
        返回: 描述符列表，每个描述符是 {'name': str, 'start_addr': int, 'end_addr': int, 'desc_offset': int}
        """
        if array_offset is None:
            array_offset = CustomBinManager.DEFAULT_ARRAY_OFFSET

        descriptors = []
        offset = array_offset

        while True:
            # 读取描述符
            if offset + CustomBinManager.DESCRIPTOR_SIZE > len(data):
                raise ValueError("文件格式错误：未找到结束标记")

            name_bytes, start_addr, end_addr = struct.unpack_from(
                CustomBinManager.DESCRIPTOR_FORMAT,
                data,
                offset
            )

            # 检查是否为结束标记
            if (start_addr == CustomBinManager.END_MARKER and
                end_addr == CustomBinManager.END_MARKER and
                name_bytes == b'\xff' * 8):
                break

            # 解码名称（去除空字符）
            try:
                name = name_bytes.decode('ascii').rstrip('\x00')
            except UnicodeDecodeError:
                name = f"<invalid: {name_bytes.hex()}>"

            descriptors.append({
                'name': name,
                'start_addr': start_addr,
                'end_addr': end_addr,
                'desc_offset': offset
            })

            offset += CustomBinManager.DESCRIPTOR_SIZE

        return descriptors

    @staticmethod
    def find_descriptor(descriptors: List[Dict], target_name: str) -> Optional[Dict]:
        """在描述符列表中查找指定名称的描述符"""
        for desc in descriptors:
            if desc['name'] == target_name:
                return desc
        return None

    @staticmethod
    def append_bin(main_bin_path: str,
                   append_bin_path: str,
                   descriptor_name: str,
                   array_offset: Optional[int] = None,
                   alignment: Optional[int] = None,
                   output_bin_path: Optional[str] = None):
        """
        追加 bin 文件到主固件末尾，并更新描述符

        参数:
            main_bin_path: 主 bin 文件路径
            append_bin_path: 要追加的 bin 文件路径
            descriptor_name: 描述符名称（最多7字符）
            array_offset: 数组偏移量（None 则使用默认值）
            alignment: 地址对齐大小（None 则使用默认值 4KB）
            output_bin_path: 输出文件路径（如果为None则覆盖原文件）
        """
        if array_offset is None:
            array_offset = CustomBinManager.DEFAULT_ARRAY_OFFSET

        if alignment is None:
            alignment = CustomBinManager.DEFAULT_ALIGNMENT

        # 验证名称长度（最多8字符，8字符时无需null终止符）
        if len(descriptor_name) > 8:
            raise ValueError(f"描述符名称过长（最多8字符）: '{descriptor_name}'")

        # 读取主 bin 文件
        if debug:
            print(f"读取主 bin 文件: {main_bin_path}")
        with open(main_bin_path, 'rb') as f:
            main_bin_data = bytearray(f.read())

        main_bin_size = len(main_bin_data)
        if debug:
            print(f"  主 bin 大小: {main_bin_size} 字节 (0x{main_bin_size:X})")

        # 读取要追加的 bin 文件
        if debug:
            print(f"读取追加 bin 文件: {append_bin_path}")
        with open(append_bin_path, 'rb') as f:
            append_bin_data = f.read()

        append_bin_size = len(append_bin_data)
        if debug:
            print(f"  追加 bin 大小: {append_bin_size} 字节 (0x{append_bin_size:X})")

        # 读取现有描述符数组
        descriptors = CustomBinManager.read_descriptors(main_bin_data, array_offset)
        if debug:
            print(f"找到 {len(descriptors)} 个现有描述符")

        # 检查名称是否已存在
        existing_desc = CustomBinManager.find_descriptor(descriptors, descriptor_name)
        if existing_desc is not None:
            if debug:
                print(f"警告: 描述符 '{descriptor_name}' 已存在，将覆盖")

        # 计算新 bin 的起始地址（对齐到指定边界）
        start_addr = CustomBinManager.align_up(main_bin_size, alignment)
        if debug:
            print(f"  起始地址: 0x{start_addr:08X} (对齐到 0x{alignment:X})")

        # 计算结束地址
        end_addr = start_addr + append_bin_size
        if debug:
            print(f"  结束地址: 0x{end_addr:08X}")
            print(f"  数据大小: {append_bin_size} 字节")

        # 扩展主 bin 数据（如果有对齐填充）
        padding_size = start_addr - main_bin_size
        if padding_size > 0:
            main_bin_data.extend(b'\x00' * padding_size)
            if debug:
                print(f"  填充: {padding_size} 字节 (0x{padding_size:X})")

        # 追加新 bin 数据
        main_bin_data.extend(append_bin_data)

        new_total_size = len(main_bin_data)
        if debug:
            print(f"  新文件大小: {new_total_size} 字节 (0x{new_total_size:X})")

        # 查找或创建描述符
        if existing_desc:
            # 更新现有描述符
            desc_offset = existing_desc['desc_offset']
            if debug:
                print(f"更新现有描述符 '{descriptor_name}' (偏移 0x{desc_offset:X})")
        else:
            # 找到第一个未分配的描述符或追加到数组末尾
            desc_offset = None
            for desc in descriptors:
                if desc['start_addr'] == 0 and desc['end_addr'] == 0:
                    desc_offset = desc['desc_offset']
                    if debug:
                        print(f"使用未分配描述符 (偏移 0x{desc_offset:X})")
                    break

            if desc_offset is None:
                # 追加到数组末尾（在结束标记之前）
                desc_offset = array_offset + len(descriptors) * CustomBinManager.DESCRIPTOR_SIZE
                if debug:
                    print(f"追加新描述符到数组末尾 (偏移 0x{desc_offset:X})")

        # 写入描述符
        # 格式：8字节名称 + 4字节起始地址 + 4字节结束地址
        # 名称编码：如果正好8字节则无需null终止符，否则用null填充
        name_bytes = descriptor_name.encode('ascii')
        if len(name_bytes) < 8:
            name_bytes = name_bytes.ljust(8, b'\x00')
        struct.pack_into(
            CustomBinManager.DESCRIPTOR_FORMAT,
            main_bin_data,
            desc_offset,
            name_bytes,
            start_addr,
            end_addr
        )

        if debug:
            print(f"写入描述符: name='{descriptor_name}', start=0x{start_addr:08X}, end=0x{end_addr:08X}")

        # 确保结束标记存在
        end_marker_offset = array_offset + (len(descriptors) + 1) * CustomBinManager.DESCRIPTOR_SIZE
        if len(main_bin_data) < end_marker_offset + CustomBinManager.END_MARKER_SIZE:
            # 扩展文件以容纳结束标记
            main_bin_data.extend(b'\x00' * (end_marker_offset + CustomBinManager.END_MARKER_SIZE - len(main_bin_data)))

        # 写入结束标记（四个 0xFFFFFFFF）
        for i in range(CustomBinManager.END_MARKER_COUNT):
            struct.pack_into("I", main_bin_data, end_marker_offset + i * 4, CustomBinManager.END_MARKER)

        # 写入输出文件
        if output_bin_path is None:
            output_bin_path = main_bin_path

        with open(output_bin_path, 'wb') as f:
            f.write(main_bin_data)

        if debug:
            print(f"✓ 成功写入文件: {output_bin_path}")


def main():
    """命令行入口点"""
    parser = argparse.ArgumentParser(
        description="自定义二进制文件镜像管理工具",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
使用示例:
  1. 查看 bin 文件结构:
     %(prog)s --show firmware.bin
     %(prog)s --show firmware.bin --offset 0x1000

  2. 追加 bin 文件到主固件:
     %(prog)s firmware.bin --append APP app.bin
     %(prog)s firmware.bin --append KERNEL kernel.bin --output new.bin --align 0x1000

说明:
  数组位置: 在 bin 文件的指定偏移处（默认 0x1200）
  描述符格式: { char[8] name, uint32_t start_addr, uint32_t end_addr } = 16字节
  结束标记: 四个 0xFFFFFFFF（16字节）
  name字段: 描述符名称（最多8字符，8字符时无需null终止符）
  start_addr: 起始地址
  end_addr: 结束地址
        """
    )

    # 查看模式
    parser.add_argument(
        "-s", "--show",
        action="store_true",
        help="查看 bin 文件结构（不修改文件）"
    )

    # 追加模式
    parser.add_argument(
        "-a", "--append",
        nargs=2,
        metavar=('NAME', 'BIN_FILE'),
        help="追加 bin 文件: --append <名称> <bin文件路径>"
    )

    # 数组偏移参数（支持 0x 格式）
    parser.add_argument(
        "--offset",
        type=str,
        default="0x1200",
        help="描述符数组在文件中的偏移量（支持 0x 格式，默认: 0x1200）"
    )

    # 对齐参数
    parser.add_argument(
        "--align",
        type=str,
        default="0x1000",
        help="地址对齐大小（支持 0x 格式，默认: 0x1000 = 4KB）"
    )

    # 位置参数
    parser.add_argument(
        "bin_file",
        nargs="?",
        help="主 bin 文件路径"
    )

    # 可选参数
    parser.add_argument(
        "-o", "--output",
        help="输出文件路径"
    )

    parser.add_argument(
        "-v", "--verbose",
        action="store_true",
        help="显示详细信息"
    )

    args = parser.parse_args()

    # 根据 verbose 参数设置 debug 全局变量
    global debug
    debug = 1 if args.verbose else 0

    # 解析偏移量和对齐值
    try:
        array_offset = CustomBinManager.parse_offset(args.offset)
        alignment = CustomBinManager.parse_offset(args.align)
    except ValueError as e:
        print(f"错误: 无效的数值: {e}")
        return 1

    # 模式1：查看文件结构
    if args.show:
        if args.bin_file:
            CustomBinManager.inspect_bin_file(args.bin_file, array_offset)
            return 0
        else:
            print("错误：查看模式需要指定 bin 文件")
            print("用法：python3 multi_bins.py --show <bin文件> [--offset 0x1200]")
            return 1

    # 模式2：追加 bin 文件
    elif args.append:
        if not args.bin_file:
            print("错误：追加模式需要指定主 bin 文件")
            print("用法：python3 multi_bins.py <主bin文件> --append <名称> <追加bin文件>")
            return 1

        if not os.path.exists(args.bin_file):
            print(f"错误: 主 bin 文件不存在: {args.bin_file}")
            return 1

        if not os.path.exists(args.append[1]):
            print(f"错误: 追加 bin 文件不存在: {args.append[1]}")
            return 1

        try:
            CustomBinManager.append_bin(
                args.bin_file,
                args.append[1],
                args.append[0],
                array_offset,
                alignment,
                args.output
            )
            return 0
        except Exception as e:
            print(f"错误: {e}", file=sys.stderr)
            if args.verbose:
                import traceback
                traceback.print_exc()
            return 1

    # 没有指定模式
    else:
        print("错误：请指定操作模式")
        print("  查看: python3 multi_bins.py --show <bin文件>")
        print("  追加: python3 multi_bins.py <主bin文件> --append <名称> <追加bin文件>")
        parser.print_help()
        return 1


if __name__ == "__main__":
    sys.exit(main())
