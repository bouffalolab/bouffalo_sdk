#!/usr/bin/env python3
"""
Generate DWARF-based unwind table for backtrace

This script extracts Call Frame Information (CFI) from DWARF .debug_frame
and generates a compact binary table for on-device stack unwinding.

This is similar to Linux x86's ORC (Oops Rewind Capability) mechanism.

Usage:
    python3 generate_dwarf_unwind_table.py <output_file> <elf_file>
"""

import sys
import subprocess
import re
import struct
from collections import defaultdict
from pathlib import Path

# Unwind entry structure (binary format, version 4.1 with function size, no alignment)
#
# File structure (in order):
#   1. Non-standard RA table
#   2. Segment table
#   3. Entries (no padding, no alignment required)
#
# === Non-standard RA table (at file beginning) ===
#   struct {
#       uint16_t num_non_standard_ra_entries;  // Number of entries in this table
#       struct {
#           uint16_t entry_idx;    // Global entry index that uses this non-standard RA
#           int16_t  ra_offset;     // Non-standard RA offset value
#       } entries[];
#       uint16_t terminator;      // 0xFFFF
#   }
#
# === Segment table ===
#   struct {
#       uint16_t segment_base;       // Segment base address (high 16 bits)
#       uint16_t start_entry_idx;    // First entry index belonging to this segment
#       uint16_t end_entry_idx;      // Last entry index belonging to this segment
#   } segments[];
#
#   // Terminator (marks end of segment table)
#   uint32_t term1;  // 0xFFFFFFFF
#   uint32_t term2;  // 0xFFFFFFFF
#   uint32_t term3;  // 0xFFFFFFFF
#
# === Individual entry (6 bytes, no alignment required) ===
#   struct {
#       uint16_t offset_in_segment | flag;  // bits 15-1: offset
#                                           // bit 0: flag (1=非RA, 0=标RA)
#       uint16_t frame_size_words;          // frame_size / 4 (16 bits, up to 262KB)
#       uint16_t func_size;                 // 函数大小（字节）
#   }
#
# Encoding details:
#   - flag=0 (标RA):
#       offset = offset_in_segment (bit 0 cleared)
#       frame_size_words = frame_size / 4  (up to 65535 words = 262KB)
#       ra_offset = frame_size_words - 1  (standard convention)
#
#   - flag=1 (非RA):
#       offset = offset_in_segment | 1
#       frame_size_words = frame_size / 4
#       ra_offset = lookup in non-standard RA table by entry_idx
#
# Important changes:
#   - frame_size_words = 0 的函数（叶子函数）不再记录在表中
#   - 每个 entry 增加 func_size 字段（2 字节），表示函数大小
#   - Entry 大小从 4 字节增加到 6 字节
#   - 无对齐要求（entries 紧跟在 segment table 后面）
#
# Backtrace behavior:
#   - 如果 PC 在表中找到：正常 unwind
#   - 如果 PC 在表中找不到（叶子函数）：PC = [SP], SP 不变，继续 unwind
#
# Lookup process for non-standard RA:
#   1. Parse non-standard RA table into array
#   2. Calculate current entry index (global index)
#   3. Binary search in table for entry_idx
#   4. Get ra_offset from table entry
#
# File layout:
#   - Header size = non_standard_ra_table + segment_table
#   - Entries start immediately after header (no padding)
#
# Frame size limits:
#   - 16 bits frame_size_words: max 65535 words = 262,140 bytes
#
# Non-standard RA limits:
#   - No limit on number of unique RA values
#   - Each entry using non-standard RA adds one table entry
#   - Table size: num_entries × 4 bytes
#
# Total file size:
#   Non-standard RA table: (num_non_standard_ra_entries + 2) × 4 bytes
#   Segment table: (num_segments × 6) + 12 bytes (terminator)
#   Entries: num_entries × 6 bytes
#   Total = ns_ra_table + segment_table + entries (no padding)


def get_all_functions(elf_file):
    """Get all function addresses from symbol table"""
    try:
        result = subprocess.run(
            ['riscv64-unknown-elf-nm', '--defined-only', '--format=posix', '--numeric-sort', elf_file],
            capture_output=True,
            text=True,
            check=True
        )
    except subprocess.CalledProcessError as e:
        print(f"Error running nm: {e}", file=sys.stderr)
        return {}

    functions = {}
    function_ranges = {}  # To track end addresses

    for line in result.stdout.splitlines():
        parts = line.split()
        if len(parts) >= 3:
            name, sym_type = parts[0], parts[1]
            if sym_type in ['t', 'T', 'w', 'W']:
                addr = int(parts[2], 16)
                functions[addr] = name

    return functions


def parse_objdump_frames(elf_file):
    """Parse DWARF frame information from objdump output"""
    try:
        result = subprocess.run(
            ['riscv64-unknown-elf-objdump', '--dwarf=frames', elf_file],
            capture_output=True,
            text=True,
            check=True
        )
    except subprocess.CalledProcessError as e:
        print(f"Error running objdump: {e}", file=sys.stderr)
        return {}

    unwind_info = {}

    lines = result.stdout.splitlines()
    i = 0

    while i < len(lines):
        line = lines[i]

        # Look for FDE (Frame Description Entry)
        # Format: 00000010 00000018 00000000 FDE cie=00000000 pc=a0000d00..a0000d1a
        fde_match = re.search(r'pc=([0-9a-fA-F]+)\.\.([0-9a-fA-F]+)', line)

        if fde_match:
            func_start = int(fde_match.group(1), 16)
            func_end = int(fde_match.group(2), 16)

            # Skip zero-length functions
            if func_start == 0 or func_end == 0 or func_start >= func_end:
                i += 1
                continue

            # Parse the FDE instructions
            cfa_offset = 0
            ra_offset_from_CFA = 0
            ra_offset_words = -1
            max_cfa_offset = 0
            has_cfi_instructions = False

            # Look ahead for DWARF instructions
            j = i + 1
            while j < len(lines):
                line = lines[j].strip()

                # Stop at next FDE/CIE
                if ' FDE ' in line or ' CIE' in line:
                    break

                if not line:
                    j += 1
                    continue

                instr_line = line

                # DW_CFA_def_cfa_offset: N
                match = re.search(r'DW_CFA_def_cfa_offset:\s+(\d+)', instr_line)
                if match:
                    cfa_offset = int(match.group(1))
                    if cfa_offset > max_cfa_offset:
                        max_cfa_offset = cfa_offset
                    has_cfi_instructions = True
                    j += 1
                    continue

                # DW_CFA_offset: r1 (ra) at cfa-N
                match = re.search(r'DW_CFA_offset:\s+r1\s+\(ra\)\s+at\s+cfa-(\d+)', instr_line)
                if match:
                    ra_offset_from_CFA = int(match.group(1))
                    has_cfi_instructions = True
                    j += 1
                    continue

                # DW_CFA_def_cfa: r2 (sp) ofs N
                match = re.search(r'DW_CFA_def_cfa:\s+r[^\s]+\s+ofs\s+(\d+)', instr_line)
                if match:
                    cfa_offset = int(match.group(1))
                    max_cfa_offset = max(max_cfa_offset, cfa_offset)
                    has_cfi_instructions = True
                    j += 1
                    continue

                j += 1

            frame_size = max_cfa_offset
            func_size = func_end - func_start

            # Calculate RA offset from SP (in words)
            if ra_offset_from_CFA > 0 and frame_size > 0:
                ra_offset_words = (frame_size - ra_offset_from_CFA) // 4
            elif frame_size > 0:
                ra_offset_words = (frame_size // 4) - 1
            else:
                ra_offset_words = 0

            # Only add if we have valid frame size
            if frame_size > 0:
                if ra_offset_words < 0:
                    ra_offset_words = (frame_size - 4) // 4

                unwind_info[func_start] = {
                    'func_start': func_start,
                    'frame_size': frame_size,
                    'func_size': func_size,
                    'ra_offset': ra_offset_words,
                    'has_cfi': has_cfi_instructions,
                }

        i += 1

    return unwind_info


def get_function_names(elf_file):
    """Get function names from symbol table"""
    try:
        result = subprocess.run(
            ['riscv64-unknown-elf-nm', '--defined-only', '--format=posix', elf_file],
            capture_output=True,
            text=True,
            check=True
        )
    except subprocess.CalledProcessError as e:
        print(f"Error running nm: {e}", file=sys.stderr)
        return {}

    function_names = {}

    for line in result.stdout.splitlines():
        parts = line.split()
        if len(parts) >= 3:
            name, sym_type = parts[0], parts[1]
            if sym_type in ['t', 'T', 'w', 'W']:
                addr = int(parts[2], 16)
                function_names[addr] = name

    return function_names


def generate_dwarf_unwind_table(output_file, elf_file, verbose=False):
    """Main function to generate DWARF unwind table in binary format"""

    if verbose:
        print(f"Extracting DWARF CFI from {elf_file}...")

    # Get all function addresses
    all_functions = get_all_functions(elf_file)
    if verbose:
        print(f"Found {len(all_functions)} total functions in symbol table")

    # Parse DWARF frame information
    unwind_info = parse_objdump_frames(elf_file)
    if verbose:
        print(f"Found {len(unwind_info)} function entries with CFI (frame_size > 0)")

    if not unwind_info:
        print("Error: No DWARF frame information found", file=sys.stderr)
        return False

    # Get function names for debug output
    function_names = get_function_names(elf_file)

    # IMPORTANT: Only include functions with frame_size > 0
    # Leaf functions (frame_size = 0) are NOT in the table
    # During backtrace, if PC is not found, treat as leaf function

    # Sort by function start address
    entries = sorted(unwind_info.values(), key=lambda x: x['func_start'])

    # Build segment table (按高16位分段) and track entry ranges
    segment_entries = {}  # segment_id -> list of entries

    for entry in entries:
        func_start = entry['func_start']
        segment_id = func_start >> 16

        if segment_id not in segment_entries:
            segment_entries[segment_id] = []
        segment_entries[segment_id].append(entry)

    # Sort segments by segment_id and build segment info
    sorted_segments = sorted(segment_entries.items())
    if verbose:
        print(f"Number of segments: {len(sorted_segments)}")

    # Count leaf functions (not in table)
    leaf_count = len(all_functions) - len(entries)
    if verbose:
        print(f"Leaf functions (not in table): {leaf_count}")

    # Flatten entries while maintaining segment order
    segmented_entries = []
    segment_ranges = []

    current_idx = 0
    for segment_id, seg_entries in sorted_segments:
        start_idx = current_idx

        for entry in seg_entries:
            func_start = entry['func_start']
            offset_in_segment = func_start & 0xFFFF

            segmented_entries.append({
                'offset_in_segment': offset_in_segment,
                'frame_size': entry['frame_size'],
                'func_size': entry['func_size'],
                'ra_offset': entry['ra_offset'],
                'func_start': func_start,
                'segment_id': segment_id,
            })
            current_idx += 1

        end_idx = current_idx - 1
        segment_ranges.append((segment_id, start_idx, end_idx))

    # Analyze non-standard RA entries
    non_standard_ra_entries = []

    for i, entry in enumerate(segmented_entries):
        expected_ra_offset = (entry['frame_size'] - 4) // 4
        if entry['ra_offset'] != expected_ra_offset:
            non_standard_ra_entries.append({
                'entry_idx': i,
                'ra_offset': entry['ra_offset'],
                'func_start': entry['func_start'],
            })

    if verbose:
        print(f"\nFrame size analysis:")
        with_frame = len(segmented_entries)
        without_frame = leaf_count
        print(f"  With frame (in table): {with_frame} functions")
        print(f"  Without frame (leaf): {without_frame} functions")
        print(f"  Non-standard RA: {len(non_standard_ra_entries)} functions")

    # Sort non-standard RA entries by entry_idx for efficient lookup
    non_standard_ra_entries.sort(key=lambda x: x['entry_idx'])

    # Generate non-standard RA table
    if verbose:
        print(f"\nGenerating non-standard RA table...")
    ns_ra_data = struct.pack('<H', len(non_standard_ra_entries))

    for ns_entry in non_standard_ra_entries:
        ns_ra_data += struct.pack('<Hh', ns_entry['entry_idx'], ns_entry['ra_offset'])

    ns_ra_data += struct.pack('<H', 0xFFFF)  # terminator
    if verbose:
        print(f"  Non-standard RA table: {len(ns_ra_data)} bytes")

    # Generate segment table
    if verbose:
        print(f"Generating segment table...")
    seg_data = b''
    for seg_base, start_idx, end_idx in segment_ranges:
        seg_data += struct.pack('<HHH', seg_base, start_idx, end_idx)

    seg_data += struct.pack('<III', 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF)
    if verbose:
        print(f"  Segment table: {len(seg_data)} bytes")

    # Generate entries (6 bytes each: offset|flag, frame_size_words, func_size)
    if verbose:
        print(f"Generating binary entries (6 bytes each)...")
    entry_data = b''
    non_standard_count = 0

    for i, entry in enumerate(segmented_entries):
        offset_in_segment = entry['offset_in_segment']
        frame_size = entry['frame_size']
        frame_size_words = frame_size // 4
        func_size = entry['func_size']
        ra_offset = entry['ra_offset']

        if entry['ra_offset'] != (frame_size - 4) // 4:
            # Non-standard RA
            offset_with_flag = (offset_in_segment & 0xFFFE) | 0x01
            packed = struct.pack('<HHH', offset_with_flag, frame_size_words, func_size)
            non_standard_count += 1
            debug_str = f"ra_offset={ra_offset} (non-standard)"
        else:
            # Standard RA encoding
            packed = struct.pack('<HHH', offset_in_segment, frame_size_words, func_size)
            debug_str = f"ra_offset={ra_offset}"

        entry_data += packed

        # Debug output (only in verbose mode)
        if verbose:
            if i < 10 or i >= len(segmented_entries) - 5:
                func_start = entry['func_start']
                func_name = function_names.get(func_start, f"func_{func_start:x}")
                segment_id = entry['segment_id']
                full_addr = (segment_id << 16) | offset_in_segment

                print(f"  [{i}] {func_name}: "
                      f"0x{full_addr:08x} frame={frame_size}B ({frame_size_words} words) "
                      f"func_size={func_size}B {debug_str}")
            elif i == 10:
                print(f"  ... ({len(segmented_entries) - 15} more entries)")

    # Merge all data (no padding, entries immediately after header)
    header_data = ns_ra_data + seg_data
    binary_data = header_data + entry_data

    # Write output file
    output_path = Path(output_file)
    output_path.parent.mkdir(parents=True, exist_ok=True)

    with open(output_path, 'wb') as f:
        f.write(binary_data)

    if verbose:
        header_size = len(header_data)
        print(f"\nSuccessfully generated {output_file}")
        print(f"  Non-standard RA table: {len(ns_ra_data)} bytes")
        print(f"  Segment table: {len(seg_data)} bytes")
        print(f"  Header total: {header_size} bytes")
        print(f"  Entries: {len(entry_data)} bytes ({len(segmented_entries)} entries × 6 bytes)")
        print(f"  Total: {len(binary_data)} bytes")
        print(f"  Entries start at offset: {header_size} (no padding)")
        print(f"\nEncoding statistics:")
        print(f"  Total entries: {len(segmented_entries)}")
        with_frame = len(segmented_entries)
        without_frame = leaf_count
        print(f"  With frame (in table): {with_frame}")
        print(f"  Without frame (leaf, not in table): {without_frame}")
        print(f"  Non-standard RA entries: {non_standard_count}")

    return True


def main():
    import argparse

    parser = argparse.ArgumentParser(
        description='Generate DWARF-based unwind table for backtrace',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s output.cfi.bin app.elf
  %(prog)s -v output.cfi.bin app.elf
  %(prog)s --verbose output.cfi.bin app.elf
        """
    )
    parser.add_argument('-v', '--verbose', action='store_true',
                        help='Print detailed entry information')
    parser.add_argument('output_file', help='Output binary file path')
    parser.add_argument('elf_file', help='ELF file to extract DWARF from')

    args = parser.parse_args()

    output_file = args.output_file
    elf_file = args.elf_file
    verbose = args.verbose

    if not Path(elf_file).is_file():
        print(f"Error: ELF file not found: {elf_file}", file=sys.stderr)
        sys.exit(1)

    success = generate_dwarf_unwind_table(output_file, elf_file, verbose=verbose)
    sys.exit(0 if success else 1)


if __name__ == '__main__':
    try:
        result = main()
        sys.exit(result)
    except KeyboardInterrupt:
        sys.exit(1)
    except Exception as e:
        print(f"Error: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)
