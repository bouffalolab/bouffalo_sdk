#!/usr/bin/env python3
"""
GDB Local Target Server

This script runs a standalone GDB RSP server that creates a 4GB virtual
address space for local debugging and post-mortem analysis.

Usage:
    python gdb_target_local.py <log_file> <elf_file> [options]

    # Basic usage - auto-select free port and start GDB
    python gdb_target_local.py demo.log firmware.elf

    # Use custom GDB init file
    python gdb_target_local.py demo.log firmware.elf -x gdb.init

The script will automatically:
1. Find a free port starting from 6000
2. Start the GDB server
3. Load the log file into virtual memory
4. Start riscv64-unknown-elf-gdb with the ELF file
5. Connect GDB to the server and restore memory

If you need to connect manually from GDB:
    (gdb) file firmware.elf         # Load symbols from ELF
    (gdb) target remote localhost:8888
    (gdb) restore firmware.elf     # Restore ELF file
    (gdb) x/10x 0x23000000         # Examine memory
    (gdb) info registers            # Show registers
"""

import sys
import os
import socket
import struct
import threading
import subprocess
import time
import base64
import binascii
import re
import zlib
from pathlib import Path
from typing import Optional, Tuple, List
from dataclasses import dataclass


def find_free_port(start_port: int = 6000, max_attempts: int = 1000) -> int:
    """Find a free port starting from start_port.

    Args:
        start_port: Port number to start searching from
        max_attempts: Maximum number of ports to try

    Returns:
        Available port number
    """
    for port in range(start_port, start_port + max_attempts):
        try:
            with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
                s.bind(("127.0.0.1", port))
                return port
        except OSError:
            continue
    # If all ports are busy, let OS choose
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind(("127.0.0.1", 0))
        return s.getsockname()[1]


@dataclass
class MemoryBlock:
    """A contiguous memory block."""
    base: int
    length: int
    data: bytes

    def contains(self, addr: int, size: int) -> bool:
        """Check if this block contains the address range [addr, addr+size)."""
        return self.base <= addr and (self.base + self.length) >= (addr + size)


class MemoryBus:
    """
    Sparse memory block manager supporting up to 4GB address space.

    Features:
    - Dynamic address space creation
    - Automatic merging of adjacent/overlapping blocks
    - No fragmentation
    - Optional init mode: restrict writes to pre-initialized segments only
    """

    MAX_BLOCKS = 1000
    ADDR_MASK = 0xFFFFFFFF

    def __init__(self, init_only: bool = False):
        """
        Initialize MemoryBus.

        Args:
            init_only: If True, only allow stores within pre-initialized segments.
                       If False, allow dynamic creation of memory blocks (default).
        """
        self.blocks: List[MemoryBlock] = []
        self.init_only = init_only
        self._initialized = False

    def init_blocks(self, blocks: List[tuple]):
        """
        Initialize memory bus with pre-defined memory blocks.

        Args:
            blocks: List of (address, data_bytes, name) tuples.
                    The data will be pre-loaded into memory blocks.

        Returns:
            Number of blocks initialized.
        """
        self.blocks = []
        for addr, data, name in blocks:
            # Create block with actual data pre-loaded
            self.blocks.append(MemoryBlock(addr, len(data), data))
        self._initialized = True
        return len(self.blocks)

    def add_block(self, block: MemoryBlock):
        """
        Add a single memory block to the bus.

        Args:
            block: MemoryBlock to add.
        """
        self.blocks.append(block)
        self._initialized = True

    def _find_block(self, addr: int, size: int) -> Optional[int]:
        """Find a block that contains the address range [addr, addr+size)."""
        for i, block in enumerate(self.blocks):
            if block.contains(addr, size):
                return i
        return None

    def _is_within_initialized(self, addr: int, size: int) -> bool:
        """Check if the address range is within any initialized segment."""
        end_addr = addr + size
        for block in self.blocks:
            block_end = block.base + block.length
            # Check for overlap
            if block_end >= addr and block.base <= end_addr:
                return True
        return False

    def load(self, addr: int, size: int) -> Optional[bytes]:
        """
        Load data from address range [addr, addr+size).

        Returns None if address range is not mapped.
        """
        addr &= self.ADDR_MASK
        idx = self._find_block(addr, size)
        if idx is not None:
            block = self.blocks[idx]
            offset = addr - block.base
            return block.data[offset:offset + size]
        return None

    def store(self, addr: int, data: bytes) -> bool:
        """
        Store data to address range [addr, addr+len(data)).

        Only allows stores within pre-initialized segments.
        Does NOT create new memory blocks dynamically.

        Returns False if write is outside initialized segments, True otherwise.
        """
        addr &= self.ADDR_MASK
        size = len(data)

        if size == 0:
            return True

        # Find the block that contains this write
        for i, block in enumerate(self.blocks):
            block_end = block.base + block.length

            # Check if write is within or overlaps this block
            if block_end >= addr and block.base <= (addr + size):
                # Calculate write region within the block
                write_start = max(addr, block.base)
                write_end = min(addr + size, block_end)
                write_offset = write_start - block.base
                write_size = write_end - write_start

                # Calculate data offset (partial write if addr != write_start)
                data_offset = write_start - addr

                # Convert block data to mutable bytearray
                block_data = bytearray(block.data)

                # Copy data into block
                block_data[write_offset:write_offset + write_size] = data[data_offset:data_offset + write_size]

                # Update block
                self.blocks[i] = MemoryBlock(block.base, block.length, bytes(block_data))
                return True

        # Write is outside all initialized segments
        return False


# ==============================================================================
# Coredump Parser Module
# ==============================================================================

# Pre-compiled base64 character set for fast validation
BASE64_CHARS = set('ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=')


def detect_and_read_file(log_file):
    """
    Detect file encoding and read content.
    Handles UTF-8, UTF-16 (with BOM auto-detection for BE/LE) with optional line prefix.
    """
    # First, try reading as raw bytes to detect BOM
    with open(log_file, 'rb') as f:
        raw_data = f.read()

    # Check BOM (Byte Order Mark) for UTF-16
    # FF FE = UTF-16-LE, FE FF = UTF-16-BE
    if raw_data.startswith(b'\xff\xfe'):
        print("Detected UTF-16-LE encoding (via BOM)", file=sys.stderr)
        try:
            return raw_data.decode('utf-16-le')
        except UnicodeDecodeError:
            pass
    elif raw_data.startswith(b'\xfe\xff'):
        print("Detected UTF-16-BE encoding (via BOM)", file=sys.stderr)
        try:
            return raw_data.decode('utf-16-be')
        except UnicodeDecodeError:
            pass

    # Fallback: try UTF-16 without BOM (auto-detection)
    try:
        return raw_data.decode('utf-16')
    except UnicodeDecodeError:
        pass

    # Try UTF-8 with timestamp handling
    # GitLab CI may insert timestamps like: [2024-01-01 12:00:00]
    # We need to strip these
    try:
        # Remove timestamp patterns
        cleaned = re.sub(rb'\n\[[^\]]*\]\n', b'\n', raw_data)
        return cleaned.decode('utf-8')
    except UnicodeDecodeError:
        pass

    # Fallback to latin-1 (never fails)
    return raw_data.decode('latin-1')


def find_coredump_sections(content):
    """
    Find all coredump sections in the content.
    Returns (last_section_marker, prefix_length) tuple - only the last section.
    """
    results = []

    # Use a simpler pattern that avoids catastrophic backtracking
    # Match the core marker, then capture the prefix from the beginning of line
    core_pattern = r'-\+-\+-\+-\s*BFLB\s+COREDUMP\s+v0\.0\.1\s*\+-\+-\+-\+'

    # Find all coredump sections
    all_matches = list(re.finditer(core_pattern, content))

    if all_matches:
        # Use the last coredump section
        last_match = all_matches[-1]
        header_marker = last_match.group(0)

        # Find the line prefix by going back to the start of the line
        match_start = last_match.start()
        line_start = content.rfind('\n', 0, match_start) + 1
        line_prefix = content[line_start:match_start]

        prefix_length = len(line_prefix)
        results.append((header_marker, prefix_length))
        print(f"Found {len(all_matches)} COREDUMP sections, processing the last one with prefix_length: {prefix_length}, prefix: '{line_prefix}'", file=sys.stderr)

    return results


def extract_coredump_data(content, header_marker, prefix_length):
    """
    Extract memory data from a coredump section.
    Returns list of (address, data_bytes, name) tuples.
    prefix_length: Length of timestamp prefix to strip from each line (e.g., 16 for "10月 31 17:10:01 ")
    """
    # Find the start of this coredump section
    start_idx = content.find(header_marker)
    if start_idx == -1:
        return []

    # Extract from start to end or next coredump
    remaining = content[start_idx:]

    # Find next coredump section (if any)
    next_pattern = r'-\+-\+-\+-\s*BFLB\s+COREDUMP\s+v0\.0\.1\s*\+-\+-\+-\+'
    next_match = re.search(next_pattern, remaining[100:])  # Skip ahead to avoid self-match
    if next_match:
        end_idx = 100 + next_match.start()
        section = remaining[:end_idx]
    else:
        section = remaining

    # Parse data blocks
    # Pattern: ------ DATA BEGIN <addr>@<len>@<name> ------
    data_blocks = []

    # Find all data begin/end pairs
    data_begin_pattern = re.compile(r'-{6}\s*DATA BEGIN\s+([0-9A-Fa-f]+)@([0-9A-Fa-f]+)@([-\w. ]+)\s*-{6}')
    data_end_pattern = re.compile(r'-{6}\s*END\s+(.+?)\s*-{6}')

    lines = section.split('\n')
    i = 0
    in_data_block = False
    current_addr = None
    current_name = None
    current_data = b''

    while i < len(lines):
        line = lines[i]

        # Check for DATA BEGIN
        begin_match = data_begin_pattern.search(line)
        if begin_match and not in_data_block:
            in_data_block = True
            current_addr = int(begin_match.group(1), 16)
            current_name = begin_match.group(3)
            current_data = b''
            i += 1
            continue

        # Check for END
        end_match = data_end_pattern.search(line)
        if end_match and in_data_block:
            # Decode base64 CRC
            try:
                end_crc_bin = base64.b64decode(end_match.group(1).strip())
                end_crc = int.from_bytes(end_crc_bin, 'little')
            except:
                end_crc = None

            # CRC check
            if end_crc is not None:
                calc_crc = zlib.crc32(current_data) & 0xFFFFFFFF
                if calc_crc != end_crc:
                    print(f"crc check addr: {current_addr:08X}, end_crc: {end_crc:08X}, crc: {calc_crc:08X}",
                          file=sys.stderr)
                    # Don't add this block - only output blocks with matching CRC
                    in_data_block = False
                    current_data = b''
                    i += 1
                    continue
                else:
                    print(f"CRC OK addr: 0x{current_addr:08X}, size: {len(current_data)} bytes",
                          file=sys.stderr)

            data_blocks.append((current_addr, current_data, current_name))
            in_data_block = False
            current_data = b''
            i += 1
            continue

        # Collect base64 data
        if in_data_block:
            # Remove timestamp prefix by fixed length
            # The timestamp format is fixed length, e.g., "10月 31 17:10:01 " (16 chars)
            # Even as seconds change (01 -> 02), the length remains the same
            if prefix_length > 0 and len(line) > prefix_length:
                line = line[prefix_length:]

            line = line.strip()
            # Only process lines that look like valid base64
            # Use set for O(1) lookup instead of O(n) string search
            if line and len(line) > 10 and all(c in BASE64_CHARS for c in line):
                try:
                    decoded = base64.b64decode(line)
                    current_data += decoded
                except binascii.Error:
                    pass  # Skip invalid base64 lines

        i += 1

    return data_blocks


def parse_coredump_blocks(log_file):
    """
    Parse coredump from log file and return memory blocks directly.

    Returns:
        List of (address, data_bytes, name) tuples, or None if parsing failed.
    """
    log_path = Path(log_file)
    if not log_path.exists():
        print(f"Error: Log file not found: {log_file}", file=sys.stderr)
        return None

    print(f"Parsing coredump from: {log_file}", file=sys.stderr)

    # Detect and read file
    content = detect_and_read_file(log_file)

    # Find coredump sections
    sections = find_coredump_sections(content)

    if not sections:
        print("No COREDUMP sections found in log", file=sys.stderr)
        return None

    # Extract data from first coredump section
    header_marker, prefix_length = sections[0]
    data_blocks = extract_coredump_data(content, header_marker, prefix_length)

    if not data_blocks:
        print("No valid data blocks found", file=sys.stderr)
        return None

    print(f"Parsed {len(data_blocks)} memory blocks", file=sys.stderr)
    return data_blocks

# ==============================================================================
# RISC-V Register State
# ==============================================================================

class RiscvRegs:
    """RISC-V register state."""

    # RISCV_REG_X0_ZERO = 0, RISCV_REG_F31_FT11 = 64, RISCV_REG_MAX = 68
    REG_COUNT = 68
    # Match C code: send registers 0-64 inclusive (RISCV_REG_F31_FT11 + 1 = 65)
    REG_G_COUNT = 65

    def __init__(self):
        self.regs = [0] * self.REG_COUNT

    def get(self, idx: int) -> int:
        if 0 <= idx < len(self.regs):
            return self.regs[idx]
        return 0

    def set(self, idx: int, value: int):
        if 0 <= idx < len(self.regs):
            self.regs[idx] = value & 0xFFFFFFFF


# ==============================================================================
# GDB RSP Protocol Implementation
# ==============================================================================

class RspState:
    INIT = 0
    STARTED = 1
    CHECKSUM1 = 2
    CHECKSUM2 = 3


class GdbClientHandler:
    """GDB RSP client handler."""

    RSP_BUFF_LEN = 4096  # Larger buffer for SREC restore

    def __init__(self, conn: socket.socket, addr: Tuple[str, int], server_ref, log_file: Optional[str] = None):
        self.conn = conn
        self.addr = addr
        self.server = server_ref
        self.state = RspState.INIT
        self.no_ack_mode = False
        self.memory = MemoryBus()
        self.regs = RiscvRegs()
        self.running = True

        # Parse log file and init memory bus if provided
        if log_file:
            blocks = parse_coredump_blocks(log_file)
            if blocks:
                count = self.memory.init_blocks(blocks)
                print(f"Initialized {count} memory blocks from {log_file}")
            else:
                print(f"Warning: Failed to parse coredump from {log_file}")

        # Initialize additional 16MB region at 0xa0000000
        region_addr = 0xa0000000
        region_size = 16 * 1024 * 1024  # 16MB
        region_data = bytes(region_size)  # Zero-initialized
        region_block = MemoryBlock(region_addr, region_size, region_data)
        self.memory.add_block(region_block)
        print(f"Initialized 16MB region at 0x{region_addr:08X}")

    def send_ack(self, ack: bytes):
        if not self.no_ack_mode:
            try:
                self.conn.sendall(ack)
            except:
                pass

    def send_reply(self, data: bytes):
        checksum = sum(data) & 0xFF
        packet = b'$' + data + b'#' + f"{checksum:02X}".encode()
        try:
            self.conn.sendall(packet)
        except:
            pass

    def send_reply_hex(self, data: bytes) -> bool:
        try:
            hex_data = data.hex().upper().encode()
            self.send_reply(hex_data)
            return True
        except:
            self.send_reply(b"E06")
            return False

    def hex_to_int(self, s: str) -> int:
        return int(s, 16) if s else 0

    def hex_to_bytes(self, s: str) -> bytes:
        return bytes.fromhex(s)

    def handle_query(self, packet: str) -> bool:
        if packet[0:1] == "C":
            self.send_reply(f"QC0".encode())
        elif packet[:3] == "wr.":
            self.send_reply(b"")
        elif "Attached" in packet and packet.find("Attached") == 0:
            self.send_reply(b"1")
        elif "Offsets" in packet and packet.find("Offsets") == 0:
            self.send_reply(b"Text=0;Data=0;Bss=0")
        elif "Supported" in packet and packet.find("Supported") == 0:
            # Match gdbstub.c: do NOT enable qXfer:features:read (wrapped in #if 0)
            self.send_reply(
                f"PacketSize={self.RSP_BUFF_LEN:X};QStartNoAckMode+;type=noneocd".encode()
            )
        else:
            self.send_reply(b"")
        return True

    def handle_read_memory(self, packet: str) -> bool:
        try:
            parts = packet.split(",")
            addr = self.hex_to_int(parts[0])
            length = self.hex_to_int(parts[1].split("#")[0])

            data = self.memory.load(addr, length)
            if data is None:
                self.send_reply(b"E0A")
                return False

            return self.send_reply_hex(data)
        except:
            self.send_reply(b"E06")
            return False

    def handle_write_memory(self, packet: str) -> bool:
        try:
            parts = packet.split(",")
            addr = self.hex_to_int(parts[0])
            length_parts = parts[1].split(":")
            length = self.hex_to_int(length_parts[0])

            if length == 0:
                self.send_reply(b"OK")
                return True

            data_hex = length_parts[1].split("#")[0]
            data = self.hex_to_bytes(data_hex)

            ret = self.memory.store(addr, data)
            if ret:
                self.send_reply(b"OK")
                return True
            else:
                self.send_reply(b"E0A")
                return False
        except:
            self.send_reply(b"E06")
            return False

    def handle_write_memory_binary(self, packet: str) -> bool:
        """Handle 'X' write memory (binary, used by restore command)."""
        try:
            parts = packet.split(",")
            addr = self.hex_to_int(parts[0])
            length_parts = parts[1].split(":")
            length = self.hex_to_int(length_parts[0])

            if length == 0:
                self.send_reply(b"OK")
                return True

            # Extract binary data with escape handling
            data_str = length_parts[1].split("#")[0]
            data = bytearray()
            i = 0
            while i < len(data_str):
                if i + 1 < len(data_str) and data_str[i] == '}':
                    # Escaped character
                    i += 1
                    data.append(ord(data_str[i]) ^ 0x20)
                else:
                    data.append(ord(data_str[i]))
                i += 1

            ret = self.memory.store(addr, bytes(data))
            if ret:
                self.send_reply(b"OK")
                return True
            else:
                self.send_reply(b"E0A")
                return False
        except:
            self.send_reply(b"E06")
            return False

    def handle_get_regs(self) -> bool:
        try:
            # Send 65 registers matching C code: x0-x31 (32), pc (1), f0-f31 (32)
            # Use little-endian byte order to match C code's raw memory transmission
            reg_data = b""
            for i in range(RiscvRegs.REG_G_COUNT):  # 65 registers
                reg_data += struct.pack("<I", self.regs.get(i))
            return self.send_reply_hex(reg_data)
        except:
            return False

    def handle_set_regs(self, packet: str) -> bool:
        try:
            data = self.hex_to_bytes(packet)
            for i in range(min(len(data) // 4, RiscvRegs.REG_G_COUNT)):
                val = struct.unpack_from("<I", data, i * 4)[0]
                self.regs.set(i, val)
            self.send_reply(b"OK")
            return True
        except:
            self.send_reply(b"E0A")
            return False

    def handle_get_reg(self, packet: str) -> bool:
        try:
            reg_idx = self.hex_to_int(packet)
            if reg_idx > RiscvRegs.REG_COUNT:  # RISCV_REG_MAX
                self.send_reply(b"E0A")
                return False
            val = self.regs.get(reg_idx)
            reg_data = struct.pack("<I", val)
            return self.send_reply_hex(reg_data)
        except:
            self.send_reply(b"E0A")
            return False

    def handle_set_reg(self, packet: str) -> bool:
        try:
            equal_idx = packet.find("=")
            reg_idx = self.hex_to_int(packet[:equal_idx])
            if reg_idx > RiscvRegs.REG_COUNT:  # RISCV_REG_MAX
                self.send_reply(b"E0A")
                return False
            # Skip '=', get value
            val_hex = packet[equal_idx + 1:].split("#")[0]
            # Convert hex string to bytes, then unpack as little-endian uint32
            # This matches the byte order encoding in GDB RSP protocol
            val_bytes = self.hex_to_bytes(val_hex)
            if len(val_bytes) < 4:
                # Pad with zeros if less than 4 bytes
                val_bytes += b'\x00' * (4 - len(val_bytes))
            val = struct.unpack("<I", val_bytes)[0]
            self.regs.set(reg_idx, val)
            self.send_reply(b"OK")
            return True
        except:
            self.send_reply(b"E0A")
            return False

    def parse_packet(self, packet: str):
        if not packet:
            return

        cmd = packet[0]
        rest = packet[1:]

        if cmd == '?':
            self.send_reply(b"S02")  # SIGINT
        elif cmd == 'D':
            self.send_reply(b"OK")
        elif cmd == 'Q':
            if "StartNoAckMode" in rest and rest.find("StartNoAckMode") == 0:
                self.send_reply(b"OK")
                self.no_ack_mode = True
            else:
                self.send_reply(b"")
        elif cmd == 'q':
            self.handle_query(rest)
        elif cmd == 'g':
            self.handle_get_regs()
        elif cmd == 'G':
            self.handle_set_regs(rest)
        elif cmd == 'p':
            self.handle_get_reg(rest)
        elif cmd == 'P':
            self.handle_set_reg(rest)
        elif cmd == 'm':
            self.handle_read_memory(rest)
        elif cmd == 'M':
            self.handle_write_memory(rest)
        elif cmd == 'X':
            self.handle_write_memory_binary(rest)
        elif cmd == 'H':
            self.send_reply(b"OK")
        elif cmd == 'v':
            if "Cont?" in rest and rest.find("Cont?") == 0:
                self.send_reply(b"vCont;c;s;C;S")
            elif "Cont;" in rest and rest.find("Cont;") == 0:
                # Don't support vCont, just break without reply
                pass
            elif "MustReplyEmpty" in rest and rest.find("MustReplyEmpty") == 0:
                self.send_reply(b"")
            else:
                self.send_reply(b"")
        elif cmd == 'z':
            # Don't support breakpoint, just print (no reply)
            pass
        elif cmd == 'Z':
            # Don't support breakpoint, just print (no reply)
            pass
        else:
            self.send_reply(b"")

    def recv_packet(self) -> Optional[bytearray]:
        buffer = bytearray()
        state = RspState.INIT
        checksum_calc = 0
        checksum_recv = 0
        checksum_hex = ""

        self.conn.settimeout(0.5)

        while self.running:
            try:
                data = self.conn.recv(1)
                if not data:
                    return None

                byte = data[0]

                if state == RspState.INIT:
                    if byte == ord('$'):
                        state = RspState.STARTED
                        checksum_calc = 0
                        buffer = bytearray()

                elif state == RspState.STARTED:
                    if byte == ord('#'):
                        state = RspState.CHECKSUM1
                    else:
                        if len(buffer) < self.RSP_BUFF_LEN:
                            buffer.append(byte)
                            checksum_calc = (checksum_calc + byte) & 0xFF

                elif state == RspState.CHECKSUM1:
                    checksum_hex = chr(byte)
                    state = RspState.CHECKSUM2

                elif state == RspState.CHECKSUM2:
                    checksum_hex += chr(byte)
                    checksum_recv = int(checksum_hex, 16)

                    if not self.no_ack_mode:
                        if checksum_recv != checksum_calc:
                            self.send_ack(b"-")
                            state = RspState.INIT
                            continue
                        self.send_ack(b"+")

                    return buffer

            except socket.timeout:
                continue
            except:
                return None

        return None

    def run(self):
        print(f"Target local: Client connected from {self.addr[0]}:{self.addr[1]}")

        try:
            while self.running:
                packet = self.recv_packet()
                if packet is None:
                    break

                packet_str = packet.decode('latin-1', errors='ignore')
                self.parse_packet(packet_str)

        except Exception as e:
            print(f"Target local: Client error: {e}")
        finally:
            print(f"Target local: Client disconnected")
            self.conn.close()
            self.server.notify_client_done()


class LocalTargetServer:
    """Local GDB RSP server."""

    def __init__(self, port: int, log_file: Optional[str] = None):
        self.port = port
        self.log_file = log_file
        self.sock = None
        self.thread = None
        self.running = False
        self.clients = []
        self.lock = threading.Lock()

    def start(self):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.sock.bind(("127.0.0.1", self.port))
        self.sock.listen(1)
        self.running = True

        def server_loop():
            while self.running:
                try:
                    self.sock.settimeout(1.0)
                    conn, addr = self.sock.accept()
                    client = GdbClientHandler(conn, addr, self, self.log_file)
                    with self.lock:
                        self.clients.append(client)
                    thread = threading.Thread(target=client.run)
                    thread.daemon = True
                    thread.start()
                except socket.timeout:
                    continue
                except:
                    break

        self.thread = threading.Thread(target=server_loop)
        self.thread.daemon = True
        self.thread.start()

    def notify_client_done(self):
        with self.lock:
            self.clients = [c for c in self.clients if c.running]

    def stop(self):
        self.running = False
        if self.sock:
            self.sock.close()
        with self.lock:
            for client in self.clients:
                client.running = False


# ==============================================================================
# Main Entry Point
# ==============================================================================

def print_usage():
    """Print usage information."""
    print(__doc__)


def main():
    """Main entry point for standalone GDB server."""
    import argparse

    parser = argparse.ArgumentParser(
        description='GDB Local Target Server - Standalone GDB RSP server for post-mortem analysis',
        formatter_class=argparse.RawDescriptionHelpFormatter
    )

    parser.add_argument(
        'log_file',
        help='Path to coredump log file to load'
    )

    parser.add_argument(
        'elf_file',
        help='Path to ELF file for symbol loading'
    )

    parser.add_argument(
        '-x', '--gdb-init',
        dest='gdb_init',
        default=None,
        help='Path to GDB init file (e.g., gdb.init)'
    )

    args = parser.parse_args()

    # Auto-select free port
    port = find_free_port()
    print(f"Auto-selected free port: {port}")

    log_file = args.log_file
    elf_file = args.elf_file
    gdb_init = args.gdb_init

    # Create and start server
    print(f"Starting local GDB server on port {port}...")
    server = LocalTargetServer(port, log_file)
    server.start()

    print(f"GDB server ready. Connect from GDB using:")
    print(f"  (gdb) file {elf_file}")
    print(f"  (gdb) target remote localhost:{port}")
    print(f"  (gdb) restore {elf_file}")

    # Auto-start GDB client
    # Give server a moment to be ready
    time.sleep(0.5)

    # Build GDB command with -ex parameters
    gdb_cmd = ['riscv64-unknown-elf-gdb']
    gdb_cmd.extend(['-ex', f'file {elf_file}'])
    gdb_cmd.extend(['-ex', f'target remote localhost:{port}'])
    gdb_cmd.extend(['-ex', f'restore {elf_file}'])
    if gdb_init:
        gdb_cmd.extend(['-x', gdb_init])

    print(f"Auto-starting GDB client: {' '.join(gdb_cmd)}")

    try:
        gdb_process = subprocess.Popen(gdb_cmd)
    except FileNotFoundError:
        print(f"Warning: {args.gdb} not found. Please start GDB manually.")
        gdb_process = None
    except Exception as e:
        print(f"Warning: Failed to start GDB: {e}")
        gdb_process = None

    try:
        # Keep server running
        while server.running:
            # Check if GDB process is still running
            if gdb_process is not None:
                ret = gdb_process.poll()
                if ret is not None:
                    # GDB has exited
                    print(f"GDB client exited (code={ret})")
                    gdb_process = None
                    # Shutdown server when GDB exits
                    break

        # Normal exit after GDB disconnects
        if server.running:
            print("\nShutting down server...")
            server.stop()
            print("Server stopped.")

    except KeyboardInterrupt:
        print("\nShutting down server...")
        server.stop()

        # Wait for GDB process to exit
        if gdb_process is not None:
            ret = gdb_process.poll()
            if ret is None:
                print("Waiting for GDB client to exit...")
                gdb_process.wait()
            print("GDB client stopped.")

        print("Server stopped.")


if __name__ == '__main__':
    main()
