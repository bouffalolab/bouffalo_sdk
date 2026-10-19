#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
BL616CL 统一测试工具（WiFi / BLE / 共存），合并自原 wifi/ble/coex 三个脚本。

被测对象：运行 wfa 固件的 BL616CL 开发板，所有测试命令都在同一固件里。
  - WiFi 测试：1 块板(DUT, 板A) + 1 台 PC（PC 与板A 同路由器，建议 PC 有线）
  - BLE  测试：板A(DUT) + 板B(对端)，可选先让板A 连 WiFi（测试要求空载连接）
  - 共存测试：板A(DUT, 同时 WiFi+BLE) + 板B(BLE 对端) + PC

测试项：
  wifi  tcp-up/tcp-down/udp-up/udp-down/latency/conn/stable/all
  ble   rx/master/slave
  coex  throughput/independ/latency

特性：
  - PC 侧 iperf 输出实时打印（[PC] 前缀）并写入日志，避免管道阻塞
  - UDP 上行：PC server 加 -t 到时自动出汇总报告（固件 client 不发 UDP-FIN），
    并以 -i 分段报告兜底，丢包率/抖动可得
  - ping 支持 --ping-count/--interval，默认 100 次 / 200ms
  - 测试结束打印结论表格（支持中文对齐）

依赖：
  - pip install pyserial
  - PC 端 iperf **2.0.9**（与固件版本严格一致；2.1.x 收固件 UDP 流会段错误崩溃）
  - 系统自带 ping

用法：
  交互式（推荐）：  python test_tool.py
  命令行：
    python test_tool.py wifi all   -a /dev/ttyUSB0 --pc-ip 192.168.5.200 --password 12345678
    python test_tool.py wifi udp-up -a COM3 --pc-ip 192.168.5.200 -t 60
    python test_tool.py wifi latency -a COM3 --ping-count 100 --interval 200
    python test_tool.py ble  rx     -a COM3 -b COM4 --count 100
    python test_tool.py coex throughput -a /dev/ttyUSB0 -b /dev/ttyUSB2 --pc-ip 192.168.5.200
"""

import sys
import os
import re
import csv
import time
import signal
import argparse
import subprocess
import statistics
import threading
import unicodedata

try:
    import serial
except ImportError:
    print("缺少 pyserial，请先安装：  pip install pyserial")
    sys.exit(1)

BAUD = 2000000
SSID = "nx30"
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
LOG_DIR = os.path.join(SCRIPT_DIR, "logs")
RESULT_CSV = os.path.join(SCRIPT_DIR, "测试结果.csv")


# ============================================================================
# 通用：ANSI / 表格
# ============================================================================
_ANSI_RE = re.compile(r"\x1b\[[0-9;]*m")


def strip_ansi(s):
    return _ANSI_RE.sub("", s)


def _disp_w(s):
    """字符串的终端显示宽度（中文/全角算 2）。"""
    return sum(2 if unicodedata.east_asian_width(c) in "WF" else 1
               for c in str(s))


def _ljust_w(s, w):
    return str(s) + " " * max(0, w - _disp_w(s))


def print_table(title, headers, rows):
    """打印对齐的 ASCII 表格（支持中文宽度）。rows 为“行列表”的列表。"""
    all_rows = [list(headers)] + [list(r) for r in rows]
    widths = [max(_disp_w(r[i]) for r in all_rows) for i in range(len(headers))]
    sep = "+" + "+".join("-" * (w + 2) for w in widths) + "+"

    def fmt(r):
        return "| " + " | ".join(_ljust_w(c, w) for c, w in zip(r, widths)) + " |"

    print()
    if title:
        print(title)
    print(sep)
    print(fmt(headers))
    print(sep)
    for r in rows:
        print(fmt(r))
    print(sep)


# ============================================================================
# 通用：时间 / 日志
# ============================================================================
def ts():
    return time.strftime("%Y%m%d_%H%M%S")


def now():
    return time.strftime("%Y-%m-%d %H:%M:%S")


def open_log(name):
    os.makedirs(LOG_DIR, exist_ok=True)
    path = os.path.join(LOG_DIR, f"{name}_{ts()}.log")
    f = open(path, "w", encoding="utf-8")
    f.write(f"# {name} 测试日志  {now()}\n")
    print(f"  日志：{path}")
    return f


# ============================================================================
# 通用：iperf 子进程实时输出
# ============================================================================
class StreamPump:
    """实时读取子进程(iperf) stdout：逐行打印到终端(带前缀) + 写日志 + 累积全文。

    用于把 PC 侧 iperf 输出实时显示（尤其 UDP 上行 PC server 的分段报告），
    同时避免 stdout 管道写满导致 iperf 阻塞。"""

    def __init__(self, proc, prefix="[PC] ", logf=None):
        self.proc = proc
        self.prefix = prefix
        self.logf = logf
        self._lines = []
        self._lock = threading.Lock()
        self._thread = threading.Thread(target=self._pump, daemon=True)
        self._thread.start()

    def _pump(self):
        try:
            for raw in iter(self.proc.stdout.readline, ""):
                line = strip_ansi(raw.rstrip("\r\n"))
                with self._lock:
                    self._lines.append(line)
                if line.strip():
                    print(f"{self.prefix}{line}", flush=True)
                if self.logf:
                    self.logf.write(f"{self.prefix}{line}\n")
                    self.logf.flush()
        except Exception:
            pass

    def text(self):
        with self._lock:
            return "\n".join(self._lines)

    def stop(self, timeout=10):
        """发 SIGINT 让 iperf 打印汇总报告（UDP 上行丢包/抖动依赖此，因固件
        client 不发 FIN），再回收线程，返回累积全文。

        SIGINT 后 iperf 可能进入 "Waiting for server threads" 不立即退出，故先
        轮询等待并让 pump 线程读完报告，必要时二次 SIGINT 强退。"""
        try:
            if self.proc.poll() is None:
                _safe_sigint(self.proc)
                deadline = time.time() + 6
                while time.time() < deadline:
                    time.sleep(0.3)
                    if self.proc.poll() is not None:
                        break
                if self.proc.poll() is None:
                    _safe_sigint(self.proc)  # "Interrupt again to force quit"
                    try:
                        self.proc.wait(timeout=3)
                    except Exception:
                        try:
                            self.proc.kill()
                        except Exception:
                            pass
        except Exception:
            pass
        self._thread.join(timeout=timeout)
        return self.text()


def _safe_sigint(proc):
    """安全地对子进程发 SIGINT（iperf 收到后打印汇总报告）。进程已退出则跳过。"""
    try:
        if proc.poll() is None:
            proc.send_signal(signal.SIGINT)
    except (ValueError, OSError, ProcessLookupError):
        pass


# ============================================================================
# 通用：iperf 解析
# ============================================================================
def iperf_summary(text):
    """取最后一条 iperf 汇总/分段行的吞吐(Mbps)，找不到返回 None。"""
    pat = re.compile(
        r"([\d.]+)\s*-\s*([\d.]+)\s*sec\s+[\d.]+\s*[KMG]?Byte[s]?\s+"
        r"([\d.]+)\s*([KMG]?)bits/sec"
    )
    last = None
    for m in pat.finditer(text):
        val = float(m.group(3))
        unit = m.group(4)
        mbps = {"K": val / 1000, "M": val, "G": val * 1000, "": val / 1e6}[unit]
        last = mbps
    return last


def iperf_udp_loss(text):
    """取最后一条 UDP 分段/汇总行的 (丢包率%, 抖动ms)。找不到返回 (None, None)。

    匹配：  0.0- 1.0 sec 11.4 MByte 95.6 Mbits/sec 0.038 ms 0/6831 (0%)
    即便固件 client 不发 UDP-FIN，PC server 的 -i 分段报告也含丢包/抖动。"""
    pat = re.compile(
        r"sec\s+[\d.]+\s*[KMG]?Byte[s]?\s+[\d.]+\s*[KMG]?bits/sec\s+"
        r"([\d.]+)\s*ms\s+\d+/\d+\s+\(([\d.]+)%\)"
    )
    last = None
    for m in pat.finditer(text):
        last = (float(m.group(2)), float(m.group(1)))
    return last if last else (None, None)


def parse_rx_unique(out):
    """解析 ble_test_rx_stat 输出的 unique 值（coex 后台 seq 扫描累计去重数）。"""
    m = re.search(r"type=rx_stat\b.*?unique=(\d+)", out)
    return int(m.group(1)) if m else None


def kill_iperf():
    """兜底清理残留 iperf 进程（正常由 StreamPump.stop 管理自己的进程）。"""
    subprocess.run(["pkill", "-f", "iperf"], capture_output=True)
    if os.name == "nt":
        subprocess.run(["taskkill", "/F", "/IM", "iperf.exe"], capture_output=True)


# ============================================================================
# 通用：ping
# ============================================================================
def ping_command(ip, count, interval_ms, size):
    """构造跨平台 ping 命令。Linux -i 最小 0.2s（普通用户），故 interval 默认 200ms。"""
    if os.name == "nt":
        return ["ping", "-n", str(count), "-l", str(size), "-w", "1000", ip]
    return ["ping", "-c", str(count), "-i", f"{interval_ms / 1000.0:g}",
            "-s", str(size), "-W", "1", ip]


def parse_ping_metrics(out, expected_count=None):
    """解析 ping 输出，返回含 tx/rx/loss/p50/p95/iqr/min/avg/max/rtts 的 dict。"""
    rtts = [float(x) for x in re.findall(r"time[=<]\s*([\d.]+)\s*ms", out)]
    tx = rx = None
    m = re.search(r"(\d+)\s+packets transmitted,\s+(\d+)\s+received", out)
    if m:
        tx, rx = int(m.group(1)), int(m.group(2))
    elif os.name == "nt":
        m = re.search(r"Sent\s*=\s*(\d+),\s*Received\s*=\s*(\d+)", out)
        if m:
            tx, rx = int(m.group(1)), int(m.group(2))
    if tx is None:
        tx = expected_count
    if rx is None:
        rx = len(rtts)

    mloss = re.search(r"([\d.]+)%\s*packet loss", out)
    loss = float(mloss.group(1)) if mloss else None
    if os.name == "nt" and loss is None:
        mloss = re.search(r"\((\d+)%\s*", out)
        loss = float(mloss.group(1)) if mloss else None
    if loss is None and tx:
        loss = round((1 - rx / tx) * 100, 1)

    metrics = {"tx": tx, "rx": rx, "loss": loss, "rtts": rtts,
               "min": None, "avg": None, "max": None,
               "p50": None, "p95": None, "iqr": None}
    if rtts:
        rtts.sort()
        metrics["min"] = rtts[0]
        metrics["avg"] = sum(rtts) / len(rtts)
        metrics["max"] = rtts[-1]
        metrics["p50"] = statistics.median(rtts)
        metrics["p95"] = rtts[min(len(rtts) - 1, int(len(rtts) * 0.95))]
        q1 = rtts[int(len(rtts) * 0.25)]
        q3 = rtts[int(len(rtts) * 0.75)]
        metrics["iqr"] = q3 - q1
    return metrics


def print_ping_metrics(metrics):
    print(f"  发包/收包: {metrics['tx']}/{metrics['rx']} | "
          f"丢包率 = {metrics['loss']}%")
    if metrics["rtts"]:
        print(f"  min/avg/max = {metrics['min']:.2f}/"
              f"{metrics['avg']:.2f}/{metrics['max']:.2f} ms")
        print(f"  P50 = {metrics['p50']:.2f} ms | "
              f"P95 = {metrics['p95']:.2f} ms | "
              f"IQR = {metrics['iqr']:.2f} ms")


# ============================================================================
# 通用：BLE 单位换算 / 结果解析
# ============================================================================
def ms_to_units(ms):
    """毫秒 → BLE 协议单位（0.625ms）。10ms->0x10，40ms->0x40"""
    return int(round(ms / 0.625))


def grep_result(text, type_):
    # 要求 RESULT 行完整（以换行结尾）。wait_result 用 read_chunk 循环拼 buf，
    # 连接次数多时 RESULT 行可能被串口分片读到一半（buf 末尾出现
    # "RESULT type=conn_master status=o"，截断在 success= 之前）；splitlines
    # 会把这种半行当完整行误匹配，导致 parse_kv 拿不到 success=/rate=。
    # 要求行尾换行可确保只在整行到齐后才匹配。
    pat = re.compile(r"\[BLE_TEST\] RESULT[^\n]*type=" + re.escape(type_)
                     + r"[^\n]*\n")
    m = pat.search(text)
    if not m:
        return None
    line = m.group(0).strip()
    idx = line.find("[BLE_TEST]")
    return line[idx:].strip()


def parse_kv(line):
    d = {}
    for m in re.finditer(r"(\w+)=([^\s]+)", line):
        d[m.group(1)] = m.group(2)
    return d


def wait_result(board, type_, timeout, progress=True):
    """边读边等，直到出现指定 RESULT 行或超时。返回 (结果行 or None, 全部输出)。"""
    deadline = time.time() + timeout
    buf = ""
    last_tick = 0
    while time.time() < deadline:
        buf += board.read_chunk()
        line = grep_result(buf, type_)
        if line:
            return line, buf
        if "bt_assert" in buf or "COREDUMP" in buf or "mcause" in buf:
            return None, buf
        if progress:
            elapsed = int(timeout - (deadline - time.time()))
            if elapsed >= last_tick + 10:
                last_tick = elapsed
                print(f"    ... 测试进行中，已等待 {elapsed} 秒")
        time.sleep(0.3)
    return None, buf


def crashed(text):
    return ("bt_assert" in text) or ("COREDUMP" in text) or ("mcause" in text)


# ============================================================================
# 串口封装（板A / 板B 通用）
# ============================================================================
class Board:
    def __init__(self, port, name, logf=None):
        try:
            self.s = serial.Serial(port, BAUD, timeout=0.5)
        except Exception as e:
            print(f"[错误] 打开串口 {port} 失败：{e}")
            sys.exit(1)
        self.port = port
        self.name = name
        self.logf = logf

    def _log(self, text):
        if self.logf:
            self.logf.write(text)
            self.logf.flush()

    def flush_in(self):
        time.sleep(0.2)
        self.s.reset_input_buffer()

    def send(self, cmd):
        """发命令（不清输入缓冲）。"""
        self.s.write((cmd + "\r\n").encode())
        self._log(f"\n>>> {cmd}\n")

    def send_flush(self, cmd):
        """清空输入缓冲后发命令（用于异步等待、丢弃旧输出的场景）。"""
        self.s.reset_input_buffer()
        self.send(cmd)

    def cmd(self, cmd, wait):
        """发命令并收集 wait 秒输出。"""
        self.s.reset_input_buffer()
        self.send(cmd)
        return self.read_for(wait)

    def read_for(self, secs):
        buf = b""
        t = time.time()
        while time.time() - t < secs:
            x = self.s.read(8192)
            if x:
                buf += x
            else:
                time.sleep(0.05)
        text = strip_ansi(buf.decode("utf-8", errors="replace"))
        self._log(text)
        return text

    def read_until(self, keyword, timeout):
        buf = ""
        t0 = time.time()
        while time.time() - t0 < timeout:
            x = self.s.read(4096)
            if x:
                seg = strip_ansi(x.decode("utf-8", errors="replace"))
                buf += seg
                self._log(seg)
                if keyword in buf:
                    return True, buf, time.time() - t0
            else:
                time.sleep(0.02)
        return False, buf, time.time() - t0

    def read_chunk(self):
        try:
            return strip_ansi(self.s.read(8192).decode("utf-8", errors="replace"))
        except Exception:
            return ""

    def reboot(self, wait=9):
        """重启板子并等待恢复（ble_init 不完全复位，大项切换时用它清残留）。"""
        try:
            self.send("reboot")
        except Exception:
            pass
        time.sleep(wait)
        try:
            self.s.reset_input_buffer()
        except Exception:
            pass

    def close(self):
        try:
            self.s.close()
        except Exception:
            pass


def reboot_both(a, b=None, wait=9):
    """重启板A（和板B 若有），大项切换时清状态残留。"""
    who = f"板A {a.port}" + (f" + 板B {b.port}" if b is not None else "")
    print(f"  [reboot] 重启 {who}，清状态残留...")
    a.reboot(wait)
    if b is not None:
        b.reboot(wait)


# ============================================================================
# WiFi 操作
# ============================================================================
def wifi_is_connected(board):
    return "Connected" in board.cmd("wifi_state", 1.5)


def wifi_get_ip(board):
    out = board.cmd("wifi_sta_dhcp", 2.5)
    m = re.search(r"IP:\s*([\d.]+)", out)
    return m.group(1) if m else None


def wifi_connect(board, ssid, password=None, timeout=30):
    """连接 WiFi，返回 (成功?, IP or None)。"""
    if wifi_is_connected(board):
        ip = wifi_get_ip(board)
        print(f"  板{board.name} WiFi 已连接" + (f"，IP={ip}" if ip else ""))
        return True, ip
    cmd = f"wifi_sta_connect {ssid}"
    if password:
        cmd += f" {password}"
    board.s.reset_input_buffer()
    board.send(cmd)
    ok, text, _ = board.read_until("CODE_WIFI_ON_GOT_IP", timeout)
    ip = None
    m = re.search(r"IP:\s*([\d.]+)", text)
    if m:
        ip = m.group(1)
    if ok:
        print(f"  板{board.name} WiFi 连接成功" + (f"，IP={ip}" if ip else ""))
        return True, ip
    print(f"  [失败] 板{board.name} WiFi 连接超时")
    return False, None


def wifi_disconnect(board):
    board.cmd("wifi_sta_disconnect", 3.0)


def ensure_connected(board, ssid, password=None):
    """确保板已连网，返回 IP。"""
    if wifi_is_connected(board):
        ip = wifi_get_ip(board)
        if ip:
            print(f"  板{board.name} 已连接，IP={ip}")
            return ip
    print(f"  板{board.name} 连接 {ssid} ...")
    ok, ip = wifi_connect(board, ssid, password)
    if ok and ip:
        return ip
    print("  [失败] 未能连上网")
    return None


# ============================================================================
# BLE 操作
# ============================================================================
def ble_init(board):
    board.cmd("ble_init", 1.0)
    board.cmd("ble_enable", 2.0)


def ble_scan_start(board, interval_ms=40, window_ms=20):
    iu, wu = ms_to_units(interval_ms), ms_to_units(window_ms)
    board.s.reset_input_buffer()
    board.send(f"ble_start_scan 1 0 {iu:04X} {wu:04X}")
    time.sleep(1.5)
    out = board.read_for(0.5)
    ok = "successfully" in out.lower() or "start" in out.lower()
    if ok:
        print(f"  板{board.name} BLE 扫描已启动 "
              f"(interval={interval_ms}ms window={window_ms}ms)")
    else:
        print(f"  板{board.name} BLE 扫描启动失败：{out.strip()[:100]}")
    return ok


def ble_scan_stop(board):
    board.cmd("ble_stop_scan", 1.0)


def ble_adv_start(board):
    board.cmd("ble_start_adv 0 0", 1.5)


def ble_adv_stop(board):
    board.cmd("ble_stop_adv", 1.0)


def ble_read_mac(board):
    out = board.cmd("ble_read_local_address", 1.0)
    m = re.search(r"Local public addr : ([0-9A-Fa-f:]{17})", out)
    return m.group(1).replace(":", "").upper() if m else None


# ============================================================================
# Coex 操作
# ============================================================================
def coex_enable(board):
    board.cmd("wifi_coex_start ps_pta", 1.5)


def coex_disable(board):
    board.cmd("wifi_coex_stop", 1.5)


def coex_duty_set(board, active_ms):
    out = board.cmd(f"wifi_coex_duty_set {active_ms}", 1.5)
    ok = "coex duty:" in out.lower()
    if ok:
        print(f"  共存 duty 设置: WiFi active={active_ms}ms")
    else:
        print(f"  duty 设置失败: {out.strip()[:100]}")
    return ok


def coex_status(board):
    out = board.cmd("wifi_coex_status", 1.5)
    d = {}
    m = re.search(
        r"coex active=(\d+) runtime=(\S+) ps_pta_running=(\d+) "
        r"band=(\S+) duty=(\d+) ms", out)
    if m:
        d["active"] = bool(int(m.group(1)))
        d["runtime"] = m.group(2)
        d["ps_pta_running"] = bool(int(m.group(3)))
        d["band"] = m.group(4)
        d["wifi_duty_ms"] = int(m.group(5))
    return d


def coex_is_active(status):
    return (status.get("active") is True and
            status.get("runtime") == "ps_pta")


def wifi_connect_with_coex(board, ssid, password, coex_duty):
    """连 WiFi 后立即开 coex；所有共存测试都走这个入口。返回 (成功?, IP)。"""
    ok, ip = wifi_connect(board, ssid, password)
    if not ok:
        return False, None
    if not ip:
        ip = wifi_get_ip(board)
    if not ip:
        print("  [失败] 获取不到板A IP")
        return False, None
    coex_enable(board)
    coex_duty_set(board, coex_duty)
    status = coex_status(board)
    if not coex_is_active(status):
        print(f"  [失败] coex 未进入有效状态: {status}")
        return False, ip
    print(f"  coex 已开启: {status.get('coex_state')} / "
          f"{status.get('pta_role')} / duty={status.get('wifi_duty_ms')}ms")
    return True, ip


def ble_tx_count_for_secs(seconds):
    """ble_test_tx 约 25ms/包，多给一点覆盖调度耗时。"""
    return max(1, int(seconds * 1000 / 25) + 20)


def start_ble_background(a, b, seconds, interval_ms, window_ms):
    """板B 持续发包，板A 开扫描。seconds 要覆盖完整背景测试时长。"""
    ble_init(b)
    ble_init(a)
    b_count = ble_tx_count_for_secs(seconds)
    b.s.reset_input_buffer()
    b.send(f"ble_test_tx {b_count}")
    time.sleep(0.3)
    ok = ble_scan_start(a, interval_ms, window_ms)
    return ok, b_count


def count_ble_scan_events(board, seconds):
    out = board.read_for(seconds)
    return len(re.findall(r"\[DEVICE\]:", out)), out


def check_ble_receiving(a, b, seconds=3, interval_ms=40, window_ms=20):
    count = max(5, int(seconds * 1000 / 25))
    b.s.reset_input_buffer()
    b.send(f"ble_test_tx {count}")
    time.sleep(0.3)
    return count_ble_scan_events(a, seconds)


# ============================================================================
# 结果格式化
# ============================================================================
def fmt_thru(r):
    if not r or r.get("mbps") is None:
        return "失败/无数据"
    s = f"{r['mbps']:.2f} Mbps"
    if r.get("loss") is not None:
        s += f" | 丢包 {r['loss']}%"
    if r.get("jitter") is not None:
        s += f" | 抖动 {r['jitter']} ms"
    return s


def fmt_wlatency(r):
    if not r:
        return "失败"
    if r.get("p50") is None:
        return f"丢包 {r.get('loss')}% (无回包)"
    return (f"P50 {r['p50']:.2f} / P95 {r['p95']:.2f} / "
            f"IQR {r['iqr']:.2f} ms | 丢包 {r['loss']}%")


def fmt_conn(r):
    if not r or r.get("seconds") is None:
        return "失败"
    return f"{r['seconds']} 秒"


def fmt_stable(r):
    if not r:
        return "失败"
    mm = f"{r['min_mbps']:.2f}" if r.get("min_mbps") else "?"
    md = f"{r['median_mbps']:.2f}" if r.get("median_mbps") else "?"
    return f"断连 {r['disconnects']} 次 | 最低 {mm} | 中位 {md} Mbps"


def fmt_ble(r):
    if not r:
        return "失败"
    return f"{r['got']}/{r['count']} ({r['rate']}%)"


def fmt_coex_thru(r):
    """格式化共存吞吐（4 方向）。汇总表一般用 run 的逐方向 append，这里给
    单值场景兜底。"""
    if not r or not r.get("results"):
        return "失败"
    parts = []
    for key, tag in (("tcp-up", "TCP↑"), ("tcp-down", "TCP↓"),
                     ("udp-up", "UDP↑"), ("udp-down", "UDP↓")):
        rr = r["results"].get(key) or {}
        m = rr.get("mbps")
        parts.append(f"{tag} {m:.1f}" if m else f"{tag} ?")
    return " | ".join(parts)


def append_coex_thru(summary, r):
    """把共存吞吐 4 方向结果追加到汇总表（每方向一行，含 BLE 精确接收数）。"""
    if r and r.get("results"):
        b_count = r.get("ble_tx_per_dir") or r.get("ble_tx_count")
        for key, label in (("tcp-up", "共存 TCP 上行"),
                           ("tcp-down", "共存 TCP 下行"),
                           ("udp-up", "共存 UDP 上行"),
                           ("udp-down", "共存 UDP 下行")):
            rr = r["results"].get(key) or {}
            s = fmt_thru(rr)
            be = rr.get("ble_unique")
            if be is not None and b_count:
                s += f" | BLE接收 {be}/{b_count}"
            summary.append((label, s))
    else:
        summary.append(("共存吞吐", "失败"))


def fmt_coex_independ(r):
    if not r:
        return "失败"
    return f"通过 {r['passed']}/{r['total']}"


def fmt_coex_latency(r):
    return fmt_wlatency(r)


# ============================================================================
# WiFi 测试
# ============================================================================
def test_throughput(board, args, proto, direction, ble_ctl=None):
    """proto: tcp/udp   direction: up(DUT->PC) / down(PC->DUT)

    PC 侧 iperf 输出经 StreamPump 实时打印并写日志；UDP 上行丢包/抖动从
    PC server 的汇总/分段报告解析（server 加 -t 到时自动出汇总）。

    ble_ctl: 可选 BLE 扫描控制器（共存吞吐用），有 pause(board)/resume(board)。
    传 None 走纯 WiFi 路径；传了则在发 DUT 命令前 pause（停扫描+清 buffer），
    发完立即 resume，使打流期间 BLE 仍在扫描（真共存）。"""
    name = f"{proto}-{direction}"
    title = {
        "tcp-up": "TCP 上行吞吐 (DUT->PC)",
        "tcp-down": "TCP 下行吞吐 (PC->DUT)",
        "udp-up": "UDP 上行吞吐 (DUT->PC)",
        "udp-down": "UDP 下行吞吐 (PC->DUT)",
    }[name]
    print(f"\n=== {title} ===")
    logf = open_log(name)
    board.logf = logf

    if ble_ctl:
        ble_ctl.pause(board)  # 干净执行 ensure_connected（BLE 停扫期间查 WiFi 状态）
    ip = ensure_connected(board, args.ssid, args.password)
    if not ip:
        if ble_ctl:
            ble_ctl.resume(board)
        logf.close()
        board.logf = None
        return None
    if ble_ctl:
        ble_ctl.resume(board)

    is_udp = proto == "udp"
    udp_flag = ["-u"] if is_udp else []
    bw = ["-b", args.bw] if is_udp else []
    port = ["-p", str(args.port)]
    dut_bw = f" -b {args.bw}" if is_udp else ""
    ri = str(args.report_interval)
    kill_iperf()
    time.sleep(0.5)
    result = {}

    if direction == "up":
        # PC=server, DUT=client。server 不加 -t：持续监听，结束时靠 pump.stop
        # 的 SIGINT 触发汇总报告（固件 UDP client 不发 FIN，iperf2 server 平时
        # 不打印分段，SIGINT 才调报告函数输出丢包/抖动）。
        srv_cmd = ["iperf", "-s", "-i", ri] + udp_flag + port
        print(f"  PC(server): {' '.join(srv_cmd)}")
        logf.write(f"\n>>> PC(server): {' '.join(srv_cmd)}\n")
        srv = subprocess.Popen(srv_cmd, stdout=subprocess.PIPE,
                               stderr=subprocess.STDOUT,
                               bufsize=1, text=True)
        pump = StreamPump(srv, "[PC] ", logf)
        time.sleep(1)
        dut_cmd = (f"iperf -c {args.pc_ip} -i {ri} -t {args.time} -p {args.port}"
                   f"{' -u' if is_udp else ''}{dut_bw}")
        print(f"  DUT(client): {dut_cmd}")
        # 关键：在 DUT 打流结束时刻(time+1)发 SIGINT。iperf2 server 平时不打印
        # 分段报告，固件 UDP client 又不发 FIN，靠 SIGINT 触发 server 输出汇总
        # （含丢包/抖动）。board.cmd 仍读满 time+4 以收 DUT client 汇总。
        timer = threading.Timer(args.time + 1.0, _safe_sigint, args=(srv,))
        timer.start()
        try:
            if ble_ctl:
                # 共存：停 BLE 扫描+清 buffer 后发命令，立即恢复扫描，使打流
                # 期间 BLE 也在扫描（真共存）；[DEVICE] 只混入 dut_out，不影响
                # 解析（上行从 pc_out 取）
                ble_ctl.pause(board)
                board.send(dut_cmd)
                ble_ctl.resume(board)
                dut_out = board.read_for(args.time + 4)
            else:
                dut_out = board.cmd(dut_cmd, args.time + 4)
        finally:
            timer.cancel()
        pc_out = pump.stop(timeout=args.time + 12)
        srv_rc = srv.returncode
        result["mbps"] = iperf_summary(pc_out)
        if result["mbps"] is None:
            result["mbps"] = iperf_summary(dut_out)
        if is_udp:
            loss, jitter = iperf_udp_loss(pc_out)
            result["loss"], result["jitter"] = loss, jitter
            # iperf2 2.1.9 收固件 UDP client 流时可能 SIGSEGV 崩溃，无法出报告
            result["server_crash"] = (loss is None and srv_rc is not None
                                      and srv_rc < 0)
    else:
        # PC=client, DUT=server。下行 UDP 丢包/抖动看 DUT server。
        if ble_ctl:
            ble_ctl.pause(board)
        else:
            board.s.reset_input_buffer()
        dut_srv = f"iperf -s -i {ri} -p {args.port}{' -u' if is_udp else ''}"
        board.send(dut_srv)
        if ble_ctl:
            ble_ctl.resume(board)  # 打流期间 BLE 扫描（真共存）
        time.sleep(1.5)
        board.read_for(0.5)
        pc_cmd = (["iperf", "-c", ip, "-i", ri, "-t", str(args.time)]
                  + port + udp_flag + bw)
        print(f"  PC(client): {' '.join(pc_cmd)}")
        logf.write(f"\n>>> PC(client): {' '.join(pc_cmd)}\n")
        pc = subprocess.Popen(pc_cmd, stdout=subprocess.PIPE,
                              stderr=subprocess.STDOUT,
                              bufsize=1, text=True)
        pump = StreamPump(pc, "[PC] ", logf)
        try:
            pc.wait(timeout=args.time + 15)
        except subprocess.TimeoutExpired:
            pc.kill()
        pc_out = pump.stop()
        dut_out = board.read_for(3)
        if ble_ctl:
            ble_ctl.pause(board)  # 停扫描，干净发 wifi_state 打断 DUT server
        board.cmd("wifi_state", 1.0)  # 打断 DUT 残留 server
        if is_udp:
            result["mbps"] = iperf_summary(dut_out)
            if result["mbps"] is None:
                result["mbps"] = iperf_summary(pc_out)
            loss, jitter = iperf_udp_loss(dut_out)
            result["loss"], result["jitter"] = loss, jitter
        else:
            result["mbps"] = iperf_summary(pc_out)
            if result["mbps"] is None:
                result["mbps"] = iperf_summary(dut_out)

    if ble_ctl:
        # 统计打流期间板A BLE 扫描收到板B 的广播事件数（共存对 BLE 的影响面）
        result["ble_events"] = len(re.findall(r"\[DEVICE\]:", dut_out))
    kill_iperf()
    print(f"  结果：{fmt_thru(result)}")
    if ble_ctl and result.get("ble_events") is not None:
        print(f"  BLE 扫描事件（打流期间板A 收板B 广播）: {result['ble_events']}")
    if is_udp and direction == "up" and result.get("server_crash"):
        print("  注：PC iperf server 异常退出——iperf2 2.1.9 收固件 UDP client 流"
              "会 SIGSEGV 崩溃，无法生成丢包/抖动报告。建议改测 UDP 下行"
              "（DUT server 端统计正常），或升级 PC 端 iperf2 到 2.2.x。")
    logf.close()
    board.logf = None
    return result


def test_wlatency(board, args):
    """PC ping DUT，按次数测试。默认 100 次 / 200ms 间隔。"""
    print(f"\n=== 时延测试 (PC ping DUT) ===")
    logf = open_log("latency")
    board.logf = logf

    ip = ensure_connected(board, args.ssid, args.password)
    if not ip:
        logf.close()
        board.logf = None
        return None

    count = args.ping_count
    interval_s = args.interval / 1000.0
    print(f"  ping {ip}：{count} 次，间隔 {args.interval}ms，"
          f"包大小 {args.size}B，约 {count * interval_s:.0f}s")

    cmd = ping_command(ip, count, args.interval, args.size)
    print(f"  执行：{' '.join(cmd)}")
    r = subprocess.run(cmd, capture_output=True, text=True,
                       timeout=count * interval_s + 30)
    out = r.stdout + r.stderr
    logf.write(out)

    rtts = [float(x) for x in re.findall(r"time[=<]\s*([\d.]+)\s*ms", out)]
    mloss = re.search(r"([\d.]+)%\s*packet loss", out)
    loss = float(mloss.group(1)) if mloss else None
    if os.name == "nt" and loss is None:
        mloss = re.search(r"\((\d+)%\s*", out)
        loss = float(mloss.group(1)) if mloss else None

    if rtts:
        rtts.sort()
        p50 = statistics.median(rtts)
        p95 = rtts[min(len(rtts) - 1, int(len(rtts) * 0.95))]
        q1 = rtts[int(len(rtts) * 0.25)]
        q3 = rtts[int(len(rtts) * 0.75)]
        iqr = q3 - q1
        print(f"  收到 {len(rtts)}/{count} | 丢包率 = {loss}%")
        print(f"  P50 = {p50:.2f} ms | P95 = {p95:.2f} ms | IQR = {iqr:.2f} ms")
        res = {"count": count, "rx": len(rtts), "loss": loss,
               "p50": p50, "p95": p95, "iqr": iqr}
    else:
        print(f"  [失败] 未收到回包，丢包率 = {loss}%")
        res = {"count": count, "rx": 0, "loss": loss,
               "p50": None, "p95": None, "iqr": None}
    logf.close()
    board.logf = None
    return res


def test_conn(board, args):
    """连接耗时：先断开，再发起连接，计时到拿到 IP。"""
    print(f"\n=== 连接耗时测试 ===")
    logf = open_log("conn")
    board.logf = logf

    print("  先断开当前连接...")
    board.cmd("wifi_sta_disconnect", 2.0)
    time.sleep(1)
    print(f"  发起连接 {args.ssid} 并计时...")
    board.s.reset_input_buffer()
    t0 = time.time()
    cmd = f"wifi_sta_connect {args.ssid}"
    if args.password:
        cmd += f" {args.password}"
    board.send(cmd)
    ok, text, _ = board.read_until("CODE_WIFI_ON_GOT_IP", 30)
    elapsed = time.time() - t0
    ip = None
    m = re.search(r"IP:\s*([\d.]+)", text)
    if m:
        ip = m.group(1)
    if ok:
        print(f"  连接耗时 = {elapsed:.2f} 秒（IP {ip}）")
        res = {"seconds": round(elapsed, 2), "ip": ip}
    else:
        print(f"  [失败] 30s 内未拿到 IP")
        res = {"seconds": None, "ip": None}
    logf.close()
    board.logf = None
    return res


def test_stable(board, args):
    """连接稳定性：长时间 TCP 上行 iperf，记录断连次数和最低分段吞吐。"""
    hours = args.hours
    total_sec = int(hours * 3600)
    print(f"\n=== 连接稳定性测试（{hours} 小时 / {total_sec}s）===")
    logf = open_log("stable")
    board.logf = logf

    ip = ensure_connected(board, args.ssid, args.password)
    if not ip:
        logf.close()
        board.logf = None
        return None

    ri = str(args.report_interval)
    kill_iperf()
    time.sleep(0.5)
    # PC server 加 -t，到时自停；StreamPump 实时打印并防管道阻塞
    srv_cmd = ["iperf", "-s", "-i", ri, "-t", str(total_sec + 10),
               "-p", str(args.port)]
    print(f"  PC(server): {' '.join(srv_cmd)}")
    logf.write(f"\n>>> PC(server): {' '.join(srv_cmd)}\n")
    srv = subprocess.Popen(srv_cmd, stdout=subprocess.PIPE,
                           stderr=subprocess.STDOUT, bufsize=1, text=True)
    pump = StreamPump(srv, "[PC] ", logf)
    time.sleep(1)

    dut_cmd = f"iperf -c {args.pc_ip} -i {ri} -t {total_sec} -p {args.port}"
    print(f"  DUT(client): {dut_cmd}")
    print(f"  开始，预计 {hours} 小时。每 60s 打印一次进度...")
    board.s.reset_input_buffer()
    board.send(dut_cmd)

    disconnects = 0
    min_mbps = None
    samples = []
    t0 = time.time()
    last_print = 0
    seg_pat = re.compile(
        r"([\d.]+)\s*-\s*([\d.]+)\s*sec\s+[\d.]+\s*[KMG]?Byte[s]?\s+"
        r"([\d.]+)\s*([KMG]?)bits/sec")
    buf = ""
    while time.time() - t0 < total_sec + 30:
        x = board.s.read(4096)
        if x:
            seg = strip_ansi(x.decode("utf-8", errors="replace"))
            board._log(seg)
            buf += seg
            if "DISCONNECT" in buf.upper() or "CODE_WIFI_ON_DISCONNECT" in buf:
                disconnects += 1
                buf = ""
            for m in seg_pat.finditer(buf):
                t_start, t_end = float(m.group(1)), float(m.group(2))
                val = float(m.group(3))
                unit = m.group(4)
                mbps = {"K": val / 1000, "M": val, "G": val * 1000, "": val / 1e6}[unit]
                seg_dur = t_end - t_start
                # 跳过异常短分段（CPU 高负载致 -i 间隔不准，瞬时虚低不代表链路下界）
                if mbps > 0.01 and seg_dur >= args.report_interval * 0.6:
                    samples.append(mbps)
                    if min_mbps is None or mbps < min_mbps:
                        min_mbps = mbps
            buf = buf[-2000:]
        else:
            time.sleep(0.1)
        elapsed = time.time() - t0
        if elapsed - last_print >= 60:
            last_print = elapsed
            mm = f"{min_mbps:.2f}" if min_mbps else "?"
            print(f"  ... 已运行 {int(elapsed)}s/{total_sec}s | "
                  f"断连 {disconnects} 次 | 最低吞吐 {mm} Mbps")

    pump.stop(timeout=15)
    kill_iperf()
    median = statistics.median(samples) if samples else None
    mm = f"{min_mbps:.2f}" if min_mbps else "?"
    md = f"{median:.2f}" if median else "?"
    print(f"  完成 | 断连次数 = {disconnects} | 最低吞吐 = {mm} Mbps "
          f"| 吞吐中位数 = {md} Mbps | 有效采样 {len(samples)} 段")
    print("  说明：最低吞吐为单个分段瞬时值，模组打流时 CPU 负载高、报告间隔可能"
          "不准，建议结合中位数判断稳定性。")
    res = {"disconnects": disconnects, "min_mbps": min_mbps,
           "median_mbps": median, "segments": len(samples)}
    logf.close()
    board.logf = None
    return res


# ============================================================================
# BLE 测试
# ============================================================================
def save_csv(row):
    new = not os.path.exists(RESULT_CSV)
    with open(RESULT_CSV, "a", newline="", encoding="utf-8-sig") as f:
        w = csv.writer(f)
        if new:
            w.writerow(["时间", "测试项", "距离", "目标次数", "成功/收到", "成功率%", "备注"])
        w.writerow(row)
    print(f"  已记录到 {os.path.basename(RESULT_CSV)}")


def test_rx(a, b, args):
    """扫描/包接收成功率：板B 发 count 个包，板A 扫描统计。"""
    count = args.count
    interval_ms, window_ms = args.rx_interval, args.rx_window
    rx_dur_ms = int(count * 25 * 1.5) + 2000
    if window_ms > interval_ms or interval_ms <= 0:
        print(f"  [错误] window({window_ms}ms) 不能大于 interval({interval_ms}ms)")
        return None
    iu, wu = ms_to_units(interval_ms), ms_to_units(window_ms)
    duty = int(window_ms * 100 / interval_ms)
    print(f"\n=== 包接收成功率测试（距离 {args.distance}）===")
    print(f"  板A({a.port}) 接收 | 板B({b.port}) 发送 {count} 个包")
    print(f"  扫描参数：interval={interval_ms}ms(0x{iu:X}) "
          f"window={window_ms}ms(0x{wu:X}) 占空比={duty}%")

    ble_init(a)
    ble_init(b)
    a.send_flush(f"ble_test_rx {rx_dur_ms} {count} {iu} {wu}")
    time.sleep(1.0)
    b.send_flush(f"ble_test_tx {count}")

    line, raw = wait_result(a, "rx", rx_dur_ms / 1000.0 + 5)
    if not line:
        print("  [失败] 未取到结果，原始输出：")
        print("  " + raw.replace("\n", "\n  ")[:800])
        return None
    kv = parse_kv(line)
    got = kv.get("unique", "?")
    rate = kv.get("rate", "?")
    print(f"  结果：收到唯一包 {got}/{count}，接收成功率 = {rate}%")
    return {"name": "包接收成功率", "distance": args.distance,
            "count": count, "got": got, "rate": rate}


def test_master(a, b, args):
    """连接成功率 Master：板A 主动连板B（板B 广播）。"""
    count = args.count
    print(f"\n=== 连接成功率(Master) 测试（距离 {args.distance}）===")
    print(f"  板A({a.port}) 做主机连 {count} 次 | 板B({b.port}) 做从机广播")
    ble_init(a)
    ble_init(b)

    peer_mac = ble_read_mac(b)
    if not peer_mac:
        print("  [失败] 读不到板B 地址")
        return None
    print(f"  板B 地址：{peer_mac}")
    ble_adv_start(b)
    time.sleep(0.5)

    a.send_flush(f"ble_test_conn_master 0 {peer_mac} {count}")
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
    return {"name": "连接成功率(Master)", "distance": args.distance,
            "count": count, "got": succ, "rate": rate}


def test_slave(a, b, args):
    """连接成功率 Slave：板A 做从机被连，板B 做主机连 count 次。"""
    count = args.count
    print(f"\n=== 连接成功率(Slave) 测试（距离 {args.distance}）===")
    print(f"  板A({a.port}) 做从机被连 | 板B({b.port}) 做主机连 {count} 次")
    ble_init(a)
    ble_init(b)

    dut_mac = ble_read_mac(a)
    if not dut_mac:
        print("  [失败] 读不到板A 地址")
        return None
    print(f"  板A 地址：{dut_mac}")
    ble_adv_start(a)
    slv = a.cmd(f"ble_test_conn_slave {count}", 1.0)
    if "status=error" in slv:
        print("  [失败] 板A 启动 Slave 测试失败：")
        print("  " + (grep_result(slv, "conn_slave") or slv.strip()))
        return None
    time.sleep(0.5)

    b.send_flush(f"ble_test_conn_master 0 {dut_mac} {count}")
    timeout = count * 8 + 15
    print(f"  预计最长耗时约 {timeout} 秒，请耐心等待...")
    line_m, raw_m = wait_result(b, "conn_master", timeout)
    if crashed(raw_m):
        print("  [失败] 检测到固件崩溃！")
        return None

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
    return {"name": "连接成功率(Slave)", "distance": args.distance,
            "count": count, "got": succ, "rate": rate}


# ============================================================================
# 共存测试
# ============================================================================
def test_coex_throughput(a, b, args):
    """共存吞吐：WiFi+coex 下，BLE 背景扫描中跑 TCP/UDP 上下行 4 方向。

    参数与纯 WiFi 吞吐一致（time / bw / port 等，UDP --bw 默认 80M）。BLE 扫描
    在打流期间保持（真共存），仅在发 iperf 命令的瞬间暂停清 buffer（ble_ctl）。"""
    print(f"\n{'=' * 56}")
    print(f"=== 共存吞吐测试（TCP/UDP × 上下行）===")
    logf = open_log("coex_throughput")
    a.logf = logf

    if not args.pc_ip:
        print("  [失败] 共存吞吐需要 PC IP（--pc-ip）")
        logf.close()
        a.logf = None
        return None

    ok, _ = wifi_connect_with_coex(a, args.ssid, args.password, args.coex_duty)
    if not ok:
        logf.close()
        a.logf = None
        return None

    # BLE 背景：板B 持续发 seq 包覆盖 4 方向总时长；板A 后台 seq 扫描（静默累计
    # unique），替代旧的普通扫描+[DEVICE]事件计数。板A 扫描静默不打印，无需为
    # 发 iperf 命令暂停/恢复（旧 ble_ctl.pause/resume 不再需要）。
    si = sw = args.throughput_scan_interval
    total_secs = args.time * 4 + args.ble_cover_extra * 4 + 20
    ble_init(b)
    ble_init(a)
    b_count = ble_tx_count_for_secs(total_secs)
    per_dir_count = ble_tx_count_for_secs(args.time)  # 每方向时长对应的发包数（unique delta 的分母）
    b.s.reset_input_buffer()
    b.send(f"ble_test_tx {b_count}")
    time.sleep(0.3)
    iu, wu = ms_to_units(si), ms_to_units(sw)
    start_out = a.cmd(f"ble_test_rx_start {iu:04X} {wu:04X}", 1.5)
    if "rx_start status=error" in start_out:
        print(f"  [失败] 板A 后台 seq 扫描启动失败: {start_out.strip()[:120]}")
        ble_scan_stop(a)
        coex_disable(a)
        logf.close()
        a.logf = None
        return None
    print(f"  BLE 发包数: {b_count}（每方向~{per_dir_count}），板A 后台 seq 扫描（{si}ms/{sw}ms），覆盖 {total_secs}s")

    titles = {"tcp-up": "共存 TCP 上行", "tcp-down": "共存 TCP 下行",
              "udp-up": "共存 UDP 上行", "udp-down": "共存 UDP 下行"}
    results = {}
    for key in ("tcp-up", "tcp-down", "udp-up", "udp-down"):
        proto, direction = key.split("-")
        print(f"\n--- {titles[key]} ---")
        logf.write(f"\n>>> === {titles[key]} ===\n")
        u0 = parse_rx_unique(a.cmd("ble_test_rx_stat", 1.0))
        # ble_ctl=None：板A 后台 seq 扫描静默运行，iperf 打流与扫描并发，不数 [DEVICE]
        r = test_throughput(a, args, proto, direction, ble_ctl=None)
        u1 = parse_rx_unique(a.cmd("ble_test_rx_stat", 1.0))
        if u0 is not None and u1 is not None:
            r["ble_unique"] = u1 - u0
        results[key] = r

    # 停扫描 + 总计（expect=板B 发包数 → ble_test_rx_stop 输出整体 rate）
    a.cmd(f"ble_test_rx_stop {b_count}", 1.5)
    ble_scan_stop(a)  # 兜底
    coex_disable(a)
    a.cmd("wifi_state", 1.0)

    print(f"\n--- 结果 ---")
    for key in ("tcp-up", "tcp-down", "udp-up", "udp-down"):
        rr = results.get(key) or {}
        line = fmt_thru(rr)
        if rr.get("ble_unique") is not None:
            line += f" | BLE接收 {rr['ble_unique']}/{per_dir_count}"
        print(f"  {titles[key]}：{line}")
    print("  折损比例: 请用纯 WiFi 吞吐基线结果在汇总表中对比")
    if args.environment:
        print(f"  环境标记: {args.environment}")

    res = {"results": results, "ble_tx_count": b_count,
           "ble_tx_per_dir": per_dir_count,
           "ble_scan_interval_ms": si, "ble_scan_window_ms": sw}
    logf.close()
    a.logf = None
    return res


def test_coex_independ(a, b, args):
    """共存独立性：两个背景场景，验证 WiFi 与 BLE 互不影响。"""
    print(f"\n{'=' * 56}")
    print(f"=== 共存独立性测试 ===")
    logf = open_log("coex_independ")
    a.logf = logf
    passed = 0
    total = 2
    results = {}
    dur = args.independ_duration

    # 场景 A：WiFi ping 背景中开 BLE 扫描
    print(f"\n--- 场景 A: WiFi ping 过程中打开 BLE 扫描 ---")
    ok, ip = wifi_connect_with_coex(a, args.ssid, args.password, args.coex_duty)
    if not ok:
        print("  场景 A: 失败，WiFi/coex 准备失败")
        results["scene_a"] = {"failed": "wifi/coex"}
    else:
        count = max(1, int(dur * 1000 / args.interval))
        cmd = ping_command(ip, count, args.interval, args.size)
        print(f"  后台启动 ping：{' '.join(cmd)}")
        ping_proc = subprocess.Popen(cmd, stdout=subprocess.PIPE,
                                     stderr=subprocess.STDOUT, text=True)
        time.sleep(args.independ_warmup)
        scan_ok, b_count = start_ble_background(
            a, b, dur + args.ble_cover_extra,
            args.scan_interval, args.scan_window)
        try:
            ping_out, _ = ping_proc.communicate(timeout=dur + 30)
        except Exception:
            ping_proc.kill()
            ping_out, _ = ping_proc.communicate()
        logf.write(f"\n--- 场景 A ping 输出 ---\n{ping_out}")
        ble_scan_stop(a)
        coex_disable(a)
        metrics = parse_ping_metrics(ping_out, count)
        wifi_ok = metrics["rx"] >= max(1, int(count * args.ping_pass_ratio))
        scene_ok = scan_ok and wifi_ok
        passed += 1 if scene_ok else 0
        results["scene_a"] = {"scan_ok": scan_ok, "ble_tx_count": b_count, **metrics}
        print(f"  BLE 扫描启动: {'通过' if scan_ok else '失败'}")
        print_ping_metrics(metrics)
        print(f"  场景 A: {'通过' if scene_ok else '失败'}")

    # 场景 B：BLE 扫描背景中连 WiFi + coex
    print(f"\n--- 场景 B: BLE 扫描过程中连接 WiFi 并进入 coex ---")
    wifi_disconnect(a)
    time.sleep(1)
    scan_secs = dur + args.ble_cover_extra
    scan_ok, b_count = start_ble_background(
        a, b, scan_secs, args.scan_interval, args.scan_window)
    if not scan_ok:
        print("  场景 B: 失败，BLE 扫描启动失败")
        results["scene_b"] = {"failed": "ble_scan"}
    else:
        time.sleep(args.independ_warmup)
        ble_scan_stop(a)
        time.sleep(0.3)
        a.s.reset_input_buffer()
        wifi_ok, ip = wifi_connect_with_coex(a, args.ssid, args.password, args.coex_duty)
        if wifi_ok:
            scan_cmd = (f"ble_start_scan 1 0 {args.scan_interval:04X} "
                        f"{args.scan_window:04X}")
            a.cmd(scan_cmd, 1.0)
            print("  BLE 扫描已恢复")
        ble_events, ble_out = check_ble_receiving(
            a, b, seconds=3,
            interval_ms=args.scan_interval, window_ms=args.scan_window)
        logf.write(f"\n--- 场景 B BLE 扫描输出 ---\n{ble_out}")
        ble_alive = scan_ok and ble_events > 0

        ping_ok = False
        ping_metrics = {}
        if wifi_ok and ip:
            count = max(1, int(dur * 1000 / args.interval))
            cmd = ping_command(ip, count, args.interval, args.size)
            print(f"  执行 ping 验证：{' '.join(cmd)}")
            r = subprocess.run(cmd, capture_output=True, text=True,
                               timeout=dur + 30)
            ping_out = r.stdout + r.stderr
            logf.write(f"\n--- 场景 B ping 输出 ---\n{ping_out}")
            ping_metrics = parse_ping_metrics(ping_out, count)
            ping_ok = ping_metrics["rx"] >= max(1, int(count * args.ping_pass_ratio))

        ble_scan_stop(a)
        coex_disable(a)
        scene_ok = ble_alive and wifi_ok and ping_ok
        passed += 1 if scene_ok else 0
        results["scene_b"] = {"scan_ok": scan_ok, "ble_events": ble_events,
                              "ble_tx_count": b_count, "wifi_coex_ok": wifi_ok,
                              "ping_ok": ping_ok, **ping_metrics}
        print(f"  BLE 扫描启动: {'通过' if scan_ok else '失败'}")
        print(f"  BLE 扫描事件(WiFi连接后): {ble_events}")
        if ping_metrics:
            print_ping_metrics(ping_metrics)
        print(f"  场景 B: {'通过' if scene_ok else '失败'}")

    print(f"\n--- 结果 ---")
    print(f"  通过 {passed}/{total} 项")
    if args.environment:
        print(f"  环境标记: {args.environment}")
    logf.close()
    a.logf = None
    return {"passed": passed, "total": total, "details": results}


def test_coex_latency(a, b, args):
    """共存下 ping 时延：WiFi 连接 + coex + BLE 扫描，PC ping DUT。"""
    print(f"\n{'=' * 56}")
    print(f"=== 共存下 ping 时延测试 ===")
    logf = open_log("coex_latency")
    a.logf = logf

    ok, ip = wifi_connect_with_coex(a, args.ssid, args.password, args.coex_duty)
    if not ok:
        logf.close()
        a.logf = None
        return None

    ping_secs = args.ping_count * args.interval / 1000.0
    scan_ok, b_count = start_ble_background(
        a, b, ping_secs + args.ble_cover_extra,
        args.scan_interval, args.scan_window)
    if not scan_ok:
        logf.close()
        a.logf = None
        return None
    print(f"  BLE 发包数: {b_count}，扫描覆盖 {ping_secs + args.ble_cover_extra:.0f}s")
    time.sleep(1)

    count = args.ping_count
    print(f"  ping {ip}：{count} 次，间隔 {args.interval}ms，"
          f"包大小 {args.size}B，约 {ping_secs:.0f}s")
    cmd = ping_command(ip, count, args.interval, args.size)
    print(f"  执行：{' '.join(cmd)}")
    r = subprocess.run(cmd, capture_output=True, text=True,
                       timeout=ping_secs + 30)
    out = r.stdout + r.stderr
    logf.write(out)

    ble_scan_stop(a)
    coex_disable(a)

    metrics = parse_ping_metrics(out, count)
    print(f"\n--- 结果 ---")
    if metrics["rtts"]:
        print_ping_metrics(metrics)
    else:
        print(f"  [失败] 未收到回包，丢包率 = {metrics['loss']}%")
    res = {k: metrics[k] for k in
           ("tx", "rx", "loss", "min", "avg", "max", "p50", "p95", "iqr")}
    res["ble_tx_count"] = b_count
    if args.environment:
        print(f"  环境标记: {args.environment}")
    logf.close()
    a.logf = None
    return res


# ============================================================================
# 交互式
# ============================================================================
def ask(prompt, default=None):
    s = input(f"{prompt}" + (f" [{default}]" if default else "") + "：").strip()
    return s if s else (default or "")


def ask_num(prompt, default):
    s = input(f"{prompt} [{default}]：").strip()
    if not s:
        return default
    try:
        v = float(s)
        return int(v) if isinstance(default, int) else v
    except ValueError:
        print(f"  输入无效，使用默认值 {default}")
        return default


def default_args(ssid, password, pc_ip, environment):
    return argparse.Namespace(
        ssid=ssid, password=password, pc_ip=pc_ip, environment=environment,
        port=5005, time=60, report_interval=1, bw="80M",
        ping_count=100, interval=200, size=300, hours=1.0,
        count=100, distance="1m", csv=True,
        coex_duty=50, scan_interval=40, scan_window=20,
        throughput_scan_interval=10, ble_cover_extra=5,
        independ_duration=20, independ_warmup=3, ping_pass_ratio=0.6,
        rx_interval=40, rx_window=20,
    )


WIFI_MENU = [
    ("1", "TCP 上行吞吐", "tcp-up"),
    ("2", "TCP 下行吞吐", "tcp-down"),
    ("3", "UDP 上行吞吐", "udp-up"),
    ("4", "UDP 下行吞吐", "udp-down"),
    ("5", "时延 (ping)", "latency"),
    ("6", "连接耗时", "conn"),
    ("7", "连接稳定性", "stable"),
    ("8", "WiFi 全套", "all"),
]
BLE_MENU = [
    ("11", "扫描/包接收成功率", "rx"),
    ("12", "连接成功率 Master", "master"),
    ("13", "连接成功率 Slave", "slave"),
]
COEX_MENU = [
    ("21", "共存吞吐", "throughput"),
    ("22", "共存独立性", "independ"),
    ("23", "共存下 ping 时延", "clatency"),
]


def print_menu():
    print("\n" + "-" * 56)
    print("  [WiFi 性能]（板A + PC）")
    for k, n, _ in WIFI_MENU:
        print(f"   {k}) {n}")
    print("  [BLE 性能]（板A + 板B）")
    for k, n, _ in BLE_MENU:
        print(f"  {k}) {n}")
    print("  [Wi-Fi/BLE 共存]（板A + 板B）")
    for k, n, _ in COEX_MENU:
        print(f"  {k}) {n}")
    print("   0) 退出")


def run_wifi(a, args, key, summary):
    if key == "all":
        seq = [("TCP 上行", "tcp-up"), ("TCP 下行", "tcp-down"),
               ("UDP 上行", "udp-up"), ("UDP 下行", "udp-down"),
               ("时延", "latency"), ("连接耗时", "conn")]
        for label, sub in seq:
            run_wifi_item(a, args, sub, label, summary)
        return
    label = dict((k[2], k[1]) for k in WIFI_MENU)[key]
    run_wifi_item(a, args, key, label, summary)


def run_wifi_item(a, args, key, label, summary):
    if key in ("tcp-up", "tcp-down", "udp-up", "udp-down", "stable", "all"):
        if not args.pc_ip:
            print("  [错误] 该项需要 PC IP，请用 --pc-ip 或交互式输入")
            return
    if key in ("tcp-up", "tcp-down", "udp-up", "udp-down"):
        args.time = ask_num("测试时长(秒)", args.time)
        if key.startswith("udp"):
            args.bw = ask("UDP 发送带宽", args.bw)
        r = test_throughput(a, args, key.split("-")[0], key.split("-")[1])
        summary.append((label, fmt_thru(r)))
    elif key == "latency":
        args.ping_count = ask_num("测试次数", args.ping_count)
        args.interval = ask_num("间隔(ms)", args.interval)
        r = test_wlatency(a, args)
        summary.append((label, fmt_wlatency(r)))
    elif key == "conn":
        r = test_conn(a, args)
        summary.append((label, fmt_conn(r)))
    elif key == "stable":
        if not args.pc_ip:
            print("  [错误] 稳定性测试需要 PC IP")
            return
        args.hours = ask_num("测试时长(小时)", args.hours)
        r = test_stable(a, args)
        summary.append((label, fmt_stable(r)))


def run_ble(a, b, args, key, summary):
    labels = {"rx": "包接收成功率", "master": "连接成功率(Master)",
              "slave": "连接成功率(Slave)"}
    args.count = ask_num("次数/包数", args.count)
    args.distance = ask("距离标记", args.distance)
    if key == "rx":
        args.rx_interval = ask_num("扫描 interval(ms)", args.rx_interval)
        args.rx_window = ask_num("扫描 window(ms)", args.rx_window)
        r = test_rx(a, b, args)
    elif key == "master":
        r = test_master(a, b, args)
    else:
        r = test_slave(a, b, args)
    summary.append((labels[key], fmt_ble(r)))
    if r and ask("是否记录到 CSV？(y/n)", "y") == "y":
        save_csv([now(), r["name"], r["distance"], r["count"], r["got"], r["rate"], ""])


def run_coex(a, b, args, key, summary):
    args.coex_duty = ask_num("共存 WiFi active(ms,10-90)", args.coex_duty)
    args.scan_interval = ask_num("BLE 扫描 interval(ms)", args.scan_interval)
    args.scan_window = ask_num("BLE 扫描 window(ms)", args.scan_window)
    if key == "throughput":
        if not args.pc_ip:
            print("  [错误] 共存吞吐需要 PC IP")
            return
        args.time = ask_num("吞吐测试时长(秒)", args.time)
        args.bw = ask("UDP 发送带宽", args.bw)
        args.throughput_scan_interval = ask_num(
            "吞吐 BLE 100% 扫描 interval(ms)", args.throughput_scan_interval)
        args.ble_cover_extra = ask_num("BLE 扫描额外覆盖(秒)", args.ble_cover_extra)
        r = test_coex_throughput(a, b, args)
        append_coex_thru(summary, r)
    elif key == "independ":
        args.independ_duration = ask_num("独立性背景时长(秒)", args.independ_duration)
        args.independ_warmup = ask_num("背景切换前预热(秒)", args.independ_warmup)
        r = test_coex_independ(a, b, args)
        summary.append(("共存独立性", fmt_coex_independ(r)))
    elif key == "clatency":
        args.ping_count = ask_num("测试次数", args.ping_count)
        args.interval = ask_num("间隔(ms)", args.interval)
        args.ble_cover_extra = ask_num("BLE 扫描额外覆盖(秒)", args.ble_cover_extra)
        r = test_coex_latency(a, b, args)
        summary.append(("共存 ping 时延", fmt_coex_latency(r)))


def interactive():
    print("=" * 56)
    print("        BL616CL 统一测试工具（WiFi / BLE / 共存）")
    print("=" * 56)
    print("提示：Windows 串口 COM3；Linux /dev/ttyUSB0\n")
    port_a = ask("板A(DUT) 串口")
    a = Board(port_a, "A")
    pc_ip = ask("PC 的 IP（吞吐/共存需要，没有直接回车）", "")
    ssid = ask("路由器 SSID", SSID)
    pwd = ask("路由器密码（开放网络回车）", "")
    env = ask("环境干扰标记（无干扰/轻度/重度）", "无干扰")

    args = default_args(ssid, pwd or None, pc_ip or None, env)
    b = None
    summary = []

    def need_b():
        nonlocal b
        if b is None:
            pb = ask("该测试需要 板B(对端) 串口")
            b = Board(pb, "B")
        return b

    # BLE 测试前提：Wi-Fi 连接空载（可选先连）
    if ask("是否先让板A 连接 WiFi？(BLE 测试要求 Wi-Fi 空载)(y/n)", "y") == "y":
        wifi_connect(a, ssid, pwd or None)

    last_group = None
    try:
        while True:
            print_menu()
            c = ask("选择", "0")
            if c == "0":
                break
            # 判定大类
            if c in dict((k, k) for k, _, _ in WIFI_MENU):
                grp = "wifi"
            elif c in dict((k, k) for k, _, _ in BLE_MENU):
                grp = "ble"
            elif c in dict((k, k) for k, _, _ in COEX_MENU):
                grp = "coex"
            else:
                grp = None
            # 切换大类时重启板子清状态残留（ble_init 不完全复位，见 §七）
            if grp and grp != last_group and last_group is not None:
                bb = need_b() if grp in ("ble", "coex") else b
                reboot_both(a, bb)
            if grp:
                last_group = grp
            try:
                if grp == "wifi":
                    run_wifi(a, args, c, summary)
                elif grp == "ble":
                    run_ble(a, need_b(), args, dict((k, v) for k, _, v in BLE_MENU)[c], summary)
                elif grp == "coex":
                    run_coex(a, need_b(), args, dict((k, v) for k, _, v in COEX_MENU)[c], summary)
                else:
                    print("无效选择")
            except Exception as e:
                print(f"  [测试异常] {e}")
    finally:
        a.close()
        if b:
            b.close()
        if summary:
            print_table("本次测试汇总", ["测试项", "结果"], summary)
        print("\n已退出。")


# ============================================================================
# 命令行
# ============================================================================
def build_parser():
    # 公共参数放 parent parser，每个子命令继承，使其在子命令前后均可用
    common = argparse.ArgumentParser(add_help=False)
    common.add_argument("-a", help="板A(DUT) 串口，如 COM3 或 /dev/ttyUSB0")
    common.add_argument("-b", help="板B(对端) 串口，BLE/共存测试需要")
    common.add_argument("--ssid", default=SSID, help=f"路由器 SSID（默认 {SSID}）")
    common.add_argument("--password", default=None, help="路由器密码（开放网络不填）")
    common.add_argument("--pc-ip", default=None, help="PC 的 IP（吞吐/共存吞吐需要）")
    common.add_argument("--environment", default=None, help="环境干扰标记（用于记录）")
    # iperf
    common.add_argument("-t", "--time", type=int, default=60, help="吞吐测试时长秒（默认60）")
    common.add_argument("-p", "--port", type=int, default=5005, help="iperf 端口（默认5005）")
    common.add_argument("-i", "--report-interval", type=int, default=1,
                        help="iperf 报告间隔秒（默认1；模组高负载建议调大）")
    common.add_argument("--bw", default="80M", help="UDP 发送带宽（默认80M）")
    # ping
    common.add_argument("--ping-count", type=int, default=100, help="ping 次数（默认100）")
    common.add_argument("--interval", type=int, default=200, help="ping 间隔ms（默认200）")
    common.add_argument("--size", type=int, default=300, help="ping 包大小B（默认300）")
    # stable
    common.add_argument("--hours", type=float, default=1, help="稳定性测试小时数（默认1）")
    # ble
    common.add_argument("--count", type=int, default=100, help="BLE 次数/包数（默认100）")
    common.add_argument("--distance", default="-", help="BLE 距离标记（如 1m）")
    common.add_argument("--rx-interval", type=float, default=40, help="BLE rx 扫描 interval ms（默认40）")
    common.add_argument("--rx-window", type=float, default=20, help="BLE rx 扫描 window ms（默认20）")
    common.add_argument("--csv", action="store_true", help="BLE 结果追加到 测试结果.csv")
    # coex
    common.add_argument("--coex-duty", type=int, default=50, help="共存 WiFi active ms（10-90，默认50）")
    common.add_argument("--scan-interval", type=float, default=40, help="BLE 扫描 interval ms（默认40）")
    common.add_argument("--scan-window", type=float, default=20, help="BLE 扫描 window ms（默认20）")
    common.add_argument("--throughput-scan-interval", type=float, default=10,
                        help="共存吞吐 BLE 100%% 扫描 interval/window ms（默认10）")
    common.add_argument("--ble-cover-extra", type=int, default=5, help="BLE 背景扫描额外覆盖秒（默认5）")
    common.add_argument("--independ-duration", type=int, default=20, help="独立性背景测试时长秒（默认20）")
    common.add_argument("--independ-warmup", type=int, default=3, help="独立性背景切换前预热秒（默认3）")
    common.add_argument("--ping-pass-ratio", type=float, default=0.6, help="独立性 ping 通过收包比例（默认0.6）")
    common.add_argument("--reboot", action="store_true", help="测试前重启板子清状态残留（ble_init 不完全复位）")

    p = argparse.ArgumentParser(description="BL616CL 统一测试工具（WiFi/BLE/共存）",
                                parents=[common])
    sub = p.add_subparsers(dest="group", required=True)
    wp = sub.add_parser("wifi", parents=[common], help="WiFi 性能测试")
    wp.add_argument("test", choices=["tcp-up", "tcp-down", "udp-up", "udp-down",
                                     "latency", "conn", "stable", "all"])
    bp = sub.add_parser("ble", parents=[common], help="BLE 性能测试")
    bp.add_argument("test", choices=["rx", "master", "slave"])
    cp = sub.add_parser("coex", parents=[common], help="共存测试")
    cp.add_argument("test", choices=["throughput", "independ", "latency"])
    return p


def main():
    if len(sys.argv) == 1:
        interactive()
        return

    p = build_parser()
    args = p.parse_args()

    if not args.a:
        p.error("需要 -a 指定板A 串口")

    need_b = args.group in ("ble", "coex")
    if need_b and not args.b:
        p.error(f"{args.group} 测试需要 -b 指定板B 串口")

    need_pcip = (args.group == "wifi" and args.test in
                 ("tcp-up", "tcp-down", "udp-up", "udp-down", "stable", "all")) or \
                (args.group == "coex" and args.test == "throughput")
    if need_pcip and not args.pc_ip:
        p.error(f"{args.group} {args.test} 需要 --pc-ip")

    a = Board(args.a, "A")
    b = Board(args.b, "B") if need_b else None
    if args.reboot:
        reboot_both(a, b)
    summary = []
    try:
        if args.group == "wifi":
            coex_disable(a)  # wifi-only: ensure coex disabled (fw default/prev test may enable it)
            if args.test == "all":
                seq = [("TCP 上行", "tcp", "up"), ("TCP 下行", "tcp", "down"),
                       ("UDP 上行", "udp", "up"), ("UDP 下行", "udp", "down")]
                for label, proto, d in seq:
                    r = test_throughput(a, args, proto, d)
                    summary.append((label, fmt_thru(r)))
                r = test_wlatency(a, args)
                summary.append(("时延", fmt_wlatency(r)))
                r = test_conn(a, args)
                summary.append(("连接耗时", fmt_conn(r)))
            elif args.test in ("tcp-up", "tcp-down", "udp-up", "udp-down"):
                proto, d = args.test.split("-")
                r = test_throughput(a, args, proto, d)
                summary.append(({"tcp-up": "TCP 上行", "tcp-down": "TCP 下行",
                                 "udp-up": "UDP 上行", "udp-down": "UDP 下行"}[args.test],
                                fmt_thru(r)))
            elif args.test == "latency":
                r = test_wlatency(a, args)
                summary.append(("时延", fmt_wlatency(r)))
            elif args.test == "conn":
                r = test_conn(a, args)
                summary.append(("连接耗时", fmt_conn(r)))
            elif args.test == "stable":
                r = test_stable(a, args)
                summary.append(("连接稳定性", fmt_stable(r)))
        elif args.group == "ble":
            if args.test == "rx":
                r = test_rx(a, b, args)
            elif args.test == "master":
                r = test_master(a, b, args)
            else:
                r = test_slave(a, b, args)
            summary.append(({"rx": "包接收成功率", "master": "连接成功率(Master)",
                             "slave": "连接成功率(Slave)"}[args.test], fmt_ble(r)))
            if r and args.csv:
                save_csv([now(), r["name"], r["distance"], r["count"],
                          r["got"], r["rate"], ""])
        elif args.group == "coex":
            if args.test == "throughput":
                r = test_coex_throughput(a, b, args)
                append_coex_thru(summary, r)
            elif args.test == "independ":
                r = test_coex_independ(a, b, args)
                summary.append(("共存独立性", fmt_coex_independ(r)))
            elif args.test == "latency":
                r = test_coex_latency(a, b, args)
                summary.append(("共存 ping 时延", fmt_coex_latency(r)))
    finally:
        a.close()
        if b:
            b.close()
        if summary:
            print_table("测试汇总", ["测试项", "结果"], summary)


if __name__ == "__main__":
    main()
