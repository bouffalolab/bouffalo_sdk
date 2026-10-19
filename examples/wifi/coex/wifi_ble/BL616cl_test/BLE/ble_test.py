#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
BLE 性能测试工具（扫描/包接收成功率、连接成功率 Master/Slave）。

适用固件：wfa example（含 btble_test_port.c 的测试命令）。
跨平台：Windows 串口写 COM3、COM4；Linux 写 /dev/ttyUSB0、/dev/ttyUSB2。

两种用法：
  1) 交互式（推荐，照提示操作）：
        python ble_test.py
  2) 命令行（适合批量/脚本）：
        python ble_test.py rx     -a COM3 -b COM4 --count 100 --distance 1m
        python ble_test.py master -a COM3 -b COM4 --count 100 --distance 1m
        python ble_test.py slave  -a COM3 -b COM4 --count 100 --distance 1m

约定：
  -a / 板A = 被测板(DUT)      -b / 板B = 配合用的对端板
  rx     ：板A 接收统计，板B 发包      （测「扫描/包接收成功率」）
  master ：板A 做主机主动连板B          （测「连接成功率 Master」）
  slave  ：板A 做从机被板B 连           （测「连接成功率 Slave」）

测试前提（来自测试用例）：Wi-Fi 连接空载状态。
  交互式会询问是否先让板A连 WiFi；命令行加 --wifi-ssid/--wifi-password。

依赖：pip install pyserial
"""

import sys
import time
import re
import argparse
import csv
import os

try:
    import serial
except ImportError:
    print("缺少 pyserial，请先安装：  pip install pyserial")
    sys.exit(1)

BAUD = 2000000
RESULT_CSV = "测试结果.csv"


# ----------------------------------------------------------------------------
# 串口封装
# ----------------------------------------------------------------------------
class Board:
    def __init__(self, port, name):
        self.name = name
        try:
            self.s = serial.Serial(port, BAUD, timeout=0.2)
        except Exception as e:
            print(f"[错误] 打开串口 {port} 失败：{e}")
            sys.exit(1)
        self.port = port

    @staticmethod
    def _clean(s):
        return re.sub(r"\x1b\[[0-9;]*m", "", s)  # 去除终端颜色码

    def write(self, cmd):
        self.s.reset_input_buffer()
        self.s.write(cmd.encode() + b"\r\n")

    def write_keep(self, cmd):
        """发命令但不清空输入缓冲（用于已在读取的场景）"""
        self.s.write(cmd.encode() + b"\r\n")

    def read_chunk(self):
        try:
            return self._clean(self.s.read(8192).decode("utf-8", errors="replace"))
        except Exception:
            return ""

    def cmd(self, cmd, wait=1.0):
        """发命令并等待 wait 秒，返回这段时间的输出"""
        self.write(cmd)
        time.sleep(wait)
        return self.read_chunk()

    def close(self):
        try:
            self.s.close()
        except Exception:
            pass


# ----------------------------------------------------------------------------
# 工具函数
# ----------------------------------------------------------------------------
def grep_result(text, type_):
    """从文本中找 [BLE_TEST] RESULT type=<type_> 那一行，返回纯结果串"""
    for line in text.splitlines():
        if "[BLE_TEST] RESULT" in line and f"type={type_}" in line:
            idx = line.find("[BLE_TEST]")
            return line[idx:].strip()
    return None


def parse_kv(line):
    """把 'key=value key=value' 解析成 dict"""
    d = {}
    for m in re.finditer(r"(\w+)=([^\s]+)", line):
        d[m.group(1)] = m.group(2)
    return d


def wait_result(board, type_, timeout, progress=True):
    """边读边等，直到出现指定 RESULT 行或超时。返回 (结果行 or None, 全部输出)"""
    deadline = time.time() + timeout
    buf = ""
    last_tick = 0
    while time.time() < deadline:
        buf += board.read_chunk()
        line = grep_result(buf, type_)
        if line:
            return line, buf
        if "bt_assert" in buf or "COREDUMP" in buf or "mcause" in buf:
            return None, buf  # 检测到崩溃，提前返回
        if progress:
            elapsed = int(timeout - (deadline - time.time()))
            if elapsed >= last_tick + 10:
                last_tick = elapsed
                print(f"    ... 测试进行中，已等待 {elapsed} 秒")
        time.sleep(0.3)
    return None, buf


def crashed(text):
    return ("bt_assert" in text) or ("COREDUMP" in text) or ("mcause" in text)


def init_ble(board):
    board.cmd("ble_init", 1.0)
    board.cmd("ble_enable", 2.0)


def wifi_connect(board, ssid, password=None, timeout=30):
    """让板子连接 WiFi 路由器（测试前提：Wi-Fi 连接空载状态）。
    固件命令: wifi_sta_connect <ssid> [password]，拿到 IP 时打印
    CODE_WIFI_ON_GOT_IP。返回 (成功?, IP or None)。"""
    # 已连接则跳过
    st = board.cmd("wifi_state", 1.5)
    if "Connected" in st:
        print(f"  板{board.name} WiFi 已连接")
        return True, None
    cmd = f"wifi_sta_connect {ssid}"
    if password:
        cmd += f" {password}"
    board.write(cmd)
    buf = ""
    t0 = time.time()
    while time.time() - t0 < timeout:
        buf += board.read_chunk()
        if "CODE_WIFI_ON_GOT_IP" in buf:
            m = re.search(r"IP:\s*([\d.]+)", buf)
            ip = m.group(1) if m else None
            print(f"  板{board.name} WiFi 连接成功" + (f"，IP={ip}" if ip else ""))
            return True, ip
        time.sleep(0.2)
    print(f"  [失败] 板{board.name} {timeout}s 内未连上 WiFi")
    return False, None


def read_mac(board):
    """读板子的本地公共地址，返回去冒号的 12 位 HEX（用于连接命令）"""
    out = board.cmd("ble_read_local_address", 1.0)
    m = re.search(r"Local public addr : ([0-9A-Fa-f:]{17})", out)
    if m:
        return m.group(1).replace(":", "").upper()
    return None


def save_csv(row):
    new = not os.path.exists(RESULT_CSV)
    with open(RESULT_CSV, "a", newline="", encoding="utf-8-sig") as f:
        w = csv.writer(f)
        if new:
            w.writerow(["时间", "测试项", "距离", "目标次数", "成功/收到", "成功率%", "备注"])
        w.writerow(row)
    print(f"  已记录到 {RESULT_CSV}")


def now():
    return time.strftime("%Y-%m-%d %H:%M:%S")


def ms_to_units(ms):
    """毫秒 → BLE 协议单位（0.625ms）。如 10ms -> 16(0x10)，40ms -> 64(0x40)"""
    return int(round(ms / 0.625))


# ----------------------------------------------------------------------------
# 三个测试
# ----------------------------------------------------------------------------
def test_rx(a, b, count, distance, rx_dur_ms=None, interval_ms=40.0, window_ms=20.0):
    """扫描/包接收成功率：板B 发 count 个包，板A 扫描统计收到多少。
    interval_ms/window_ms：扫描间隔/窗口（毫秒），默认 40/20 即占空比 50%
    （满足用例「BLE 占空比不高于 50%」要求）。脚本自动换算成 0.625ms 单位。"""
    if rx_dur_ms is None:
        # 发送总时长约 count*25ms，接收窗口留 1.5 倍余量 + 2 秒
        rx_dur_ms = int(count * 25 * 1.5) + 2000
    if window_ms > interval_ms or interval_ms <= 0:
        print(f"  [错误] window({window_ms}ms) 不能大于 interval({interval_ms}ms)")
        return None
    iu, wu = ms_to_units(interval_ms), ms_to_units(window_ms)
    duty = int(window_ms * 100 / interval_ms)
    print(f"\n=== 包接收成功率测试（距离 {distance}）===")
    print(f"  板A({a.port}) 接收 | 板B({b.port}) 发送 {count} 个包")
    print(f"  扫描参数：interval={interval_ms}ms(0x{iu:X}) "
          f"window={window_ms}ms(0x{wu:X}) 占空比={duty}%")

    init_ble(a)
    init_ble(b)

    # 板A 先启动扫描接收
    a.write(f"ble_test_rx {rx_dur_ms} {count} {iu} {wu}")
    time.sleep(1.0)  # 等扫描起来
    # 板B 发包
    b.write(f"ble_test_tx {count}")

    line, raw = wait_result(a, "rx", rx_dur_ms / 1000.0 + 5)
    if not line:
        print("  [失败] 未取到结果，原始输出：")
        print("  " + raw.replace("\n", "\n  ")[:800])
        return None

    kv = parse_kv(line)
    unique = kv.get("unique", "?")
    rate = kv.get("rate", "?")
    print(f"  结果：收到唯一包 {unique}/{count}，接收成功率 = {rate}%")
    print(f"  ({line})")
    return ("包接收成功率", distance, count, f"{unique}/{count}", rate)


def test_master(a, b, count, distance):
    """连接成功率 Master：板A 主动连板B（板B 广播）。"""
    print(f"\n=== 连接成功率(Master) 测试（距离 {distance}）===")
    print(f"  板A({a.port}) 做主机连 {count} 次 | 板B({b.port}) 做从机广播")

    init_ble(a)
    init_ble(b)

    peer_mac = read_mac(b)
    if not peer_mac:
        print("  [失败] 读不到板B 地址")
        return None
    print(f"  板B 地址：{peer_mac}")

    # 板B 开可连接广播
    b.cmd("ble_start_adv 0 0", 1.5)
    time.sleep(0.5)

    # 板A 跑 Master 测试（阻塞，每次约 1~3 秒，超时给足）
    a.write(f"ble_test_conn_master 0 {peer_mac} {count}")
    timeout = count * 8 + 15
    print(f"  预计最长耗时约 {timeout} 秒，请耐心等待...")
    line, raw = wait_result(a, "conn_master", timeout)

    if crashed(raw):
        print("  [失败] 检测到固件崩溃！")
        print("  " + raw[:600])
        return None
    if not line:
        print("  [失败] 未取到结果（可能超时）")
        return None

    kv = parse_kv(line)
    succ = kv.get("success", "?")
    rate = kv.get("rate", "?")
    print(f"  结果：连接成功 {succ}/{count}，成功率 = {rate}%")
    print(f"  ({line})")
    return ("连接成功率(Master)", distance, count, f"{succ}/{count}", rate)


def test_slave(a, b, count, distance):
    """连接成功率 Slave：板A 做从机被连，板B 做主机连 count 次。"""
    print(f"\n=== 连接成功率(Slave) 测试（距离 {distance}）===")
    print(f"  板A({a.port}) 做从机被连 | 板B({b.port}) 做主机连 {count} 次")

    init_ble(a)
    init_ble(b)

    dut_mac = read_mac(a)
    if not dut_mac:
        print("  [失败] 读不到板A 地址")
        return None
    print(f"  板A 地址：{dut_mac}")

    # 板A：先配可连接广播，再进 Slave 测试模式
    a.cmd("ble_start_adv 0 0", 1.5)
    slv = a.cmd(f"ble_test_conn_slave {count}", 1.0)
    if "status=error" in slv:
        print("  [失败] 板A 启动 Slave 测试失败：")
        print("  " + (grep_result(slv, "conn_slave") or slv.strip()))
        return None
    time.sleep(0.5)

    # 板B：做主机连 count 次
    b.write(f"ble_test_conn_master 0 {dut_mac} {count}")
    timeout = count * 8 + 15
    print(f"  预计最长耗时约 {timeout} 秒，请耐心等待...")
    line_m, raw_m = wait_result(b, "conn_master", timeout)

    if crashed(raw_m):
        print("  [失败] 检测到固件崩溃！")
        return None

    # 等一下让板A 统计落定，再读 Slave 结果
    time.sleep(1.5)
    a.cmd("ble_test_conn_stats", 1.0)
    stop_out = a.cmd("ble_test_conn_stop", 1.0)
    line_s = grep_result(stop_out, "conn_slave")

    if not line_s:
        print("  [失败] 未取到板A(Slave) 结果")
        return None

    kv = parse_kv(line_s)
    succ = kv.get("success", "?")
    rate = kv.get("rate", "?")
    print(f"  结果：板A 被成功连接 {succ}/{count}，成功率 = {rate}%")
    print(f"  板A(Slave): {line_s}")
    if line_m:
        print(f"  板B(Master): {line_m}")
    return ("连接成功率(Slave)", distance, count, f"{succ}/{count}", rate)


# ----------------------------------------------------------------------------
# 交互式
# ----------------------------------------------------------------------------
def ask(prompt, default=None):
    s = input(f"{prompt}" + (f" [{default}]" if default else "") + "：").strip()
    return s if s else (default or "")


def interactive():
    print("=" * 56)
    print("           BLE 性能测试工具（交互式）")
    print("=" * 56)
    print("提示：Windows 串口形如 COM3；Linux 形如 /dev/ttyUSB0\n")
    port_a = ask("请输入 板A(被测板/DUT) 串口")
    port_b = ask("请输入 板B(对端配合板) 串口")
    a = Board(port_a, "A")
    b = Board(port_b, "B")

    # 测试前提：Wi-Fi 连接空载状态（来自测试用例要求）
    if ask("是否先让板A(DUT)连接 WiFi 路由器？(测试要求 Wi-Fi 连接空载) (y/n)",
           "y").lower() == "y":
        ssid = ask("路由器 SSID")
        pwd = ask("路由器密码（开放网络直接回车）", "")
        ok, _ = wifi_connect(a, ssid, pwd or None)
        if not ok and ask("WiFi 连接失败，是否继续测试？(y/n)", "n").lower() != "y":
            a.close()
            b.close()
            print("已退出。")
            return

    try:
        while True:
            print("\n" + "-" * 56)
            print("请选择测试项：")
            print("  1) 扫描/包接收成功率   （板A收，板B发）")
            print("  2) 连接成功率 Master   （板A主动连板B）")
            print("  3) 连接成功率 Slave    （板A被板B连）")
            print("  4) 退出")
            choice = ask("输入序号", "1")

            if choice == "4":
                break
            if choice not in ("1", "2", "3"):
                print("无效选择")
                continue

            distance = ask("当前测试距离（如 1m/5m/10m，仅用于记录）", "1m")
            count = int(ask("测试次数/包数", "100"))

            if choice == "1":
                # 扫描占空比可配（用例要求不高于 50%）。输入十进制毫秒，
                # 脚本自动换算成 0.625ms 协议单位（如 10ms -> 0x10）。
                iv = float(ask("扫描 interval（毫秒）", "40"))
                wv = float(ask("扫描 window（毫秒，需≤interval）", "20"))
                r = test_rx(a, b, count, distance,
                            interval_ms=iv, window_ms=wv)
            elif choice == "2":
                r = test_master(a, b, count, distance)
            else:
                r = test_slave(a, b, count, distance)

            if r and ask("是否记录到 CSV？(y/n)", "y").lower() == "y":
                save_csv([now(), r[0], r[1], r[2], r[3], r[4], ""])
    finally:
        a.close()
        b.close()
        print("\n已退出。")


# ----------------------------------------------------------------------------
# 命令行
# ----------------------------------------------------------------------------
def main():
    if len(sys.argv) == 1:
        interactive()
        return

    p = argparse.ArgumentParser(description="BLE 性能测试工具")
    p.add_argument("test", choices=["rx", "master", "slave"], help="测试项")
    p.add_argument("-a", required=True, help="板A(被测板) 串口，如 COM3 或 /dev/ttyUSB0")
    p.add_argument("-b", required=True, help="板B(对端) 串口，如 COM4 或 /dev/ttyUSB2")
    p.add_argument("--count", type=int, default=100, help="次数/包数（默认100）")
    p.add_argument("--distance", default="-", help="距离标记，用于记录（如 1m）")
    p.add_argument("--csv", action="store_true", help="结果追加到 测试结果.csv")
    p.add_argument("--wifi-ssid",
                   help="测试前先让板A连接此 WiFi 路由器（测试要求 Wi-Fi 连接空载）")
    p.add_argument("--wifi-password", help="WiFi 密码（开放网络不填）")
    p.add_argument("--scan-interval", type=float, default=40,
                   help="rx 扫描 interval，毫秒（默认 40ms=0x40）")
    p.add_argument("--scan-window", type=float, default=20,
                   help="rx 扫描 window，毫秒（默认 20ms=0x20，配合默认 interval 占空比 50%%）")
    args = p.parse_args()

    a = Board(args.a, "A")
    b = Board(args.b, "B")

    # 测试前提：Wi-Fi 连接空载状态
    if args.wifi_ssid:
        ok, _ = wifi_connect(a, args.wifi_ssid, args.wifi_password)
        if not ok:
            a.close()
            b.close()
            sys.exit(1)

    try:
        if args.test == "rx":
            r = test_rx(a, b, args.count, args.distance,
                        interval_ms=args.scan_interval,
                        window_ms=args.scan_window)
        elif args.test == "master":
            r = test_master(a, b, args.count, args.distance)
        else:
            r = test_slave(a, b, args.count, args.distance)
        if r and args.csv:
            save_csv([now(), r[0], r[1], r[2], r[3], r[4], ""])
    finally:
        a.close()
        b.close()


if __name__ == "__main__":
    main()
