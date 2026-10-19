#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Wi-Fi/BLE 共存性能测试工具。

被测对象：两块通过串口连接的 DUT（运行 wfa 固件）。
板A 同时跑 Wi-Fi + BLE，板B 作为 BLE 对端配合。
PC 与板A 连同一路由器（板A 走 WiFi，PC 建议有线接入）。

测试项：
  throughput  共存状态下 Wi-Fi 吞吐（WiFi 连接 -> coex -> BLE 100% 扫描）
  independ    共存独立性（WiFi ping 背景开 BLE；BLE scan 背景连 WiFi+coex）
  latency     共存下 ping 时延（P50/P95/IQR/丢包率）
  lowrate     1Mbps 路由器速率场景 ping（可选辅助项）

依赖：
  - pip install pyserial
  - PC 端 iperf **2.x**
  - ping：系统自带

用法示例：
  python coex_test.py throughput -a /dev/ttyUSB0 -b /dev/ttyUSB2 --pc-ip 192.168.5.200 --password 12345678
  python coex_test.py latency   -a /dev/ttyUSB0 -b /dev/ttyUSB2 --pc-ip 192.168.5.200 --password 12345678
  不带参数运行进入交互式：  python coex_test.py
"""

import sys
import os
import re
import time
import argparse
import subprocess
import statistics

try:
    import serial
except ImportError:
    print("缺少 pyserial，请先安装：  pip install pyserial")
    sys.exit(1)

BAUD = 2000000
SSID = "nx30"
LOG_DIR = "logs"


# ============================================================================
# 串口封装
# ============================================================================
class Board:
    def __init__(self, port, name, logf=None):
        try:
            self.s = serial.Serial(port, BAUD, timeout=1)
        except Exception as e:
            print(f"[错误] 打开串口 {port} 失败：{e}")
            sys.exit(1)
        self.port = port
        self.name = name
        self.logf = logf

    @staticmethod
    def _clean(s):
        return re.sub(r"\x1b\[[0-9;]*m", "", s)

    def _log(self, text):
        if self.logf:
            self.logf.write(text)
            self.logf.flush()

    def send(self, cmd):
        self.s.write((cmd + "\r\n").encode())
        self._log(f"\n>>> {cmd}\n")

    def cmd(self, cmd, wait):
        """发命令并收集 wait 秒输出"""
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
        text = self._clean(buf.decode("utf-8", errors="replace"))
        self._log(text)
        return text

    def read_until(self, keyword, timeout):
        buf = ""
        t0 = time.time()
        while time.time() - t0 < timeout:
            x = self.s.read(4096)
            if x:
                seg = self._clean(x.decode("utf-8", errors="replace"))
                buf += seg
                self._log(seg)
                if keyword in buf:
                    return True, buf, time.time() - t0
            else:
                time.sleep(0.02)
        return False, buf, time.time() - t0

    def close(self):
        try:
            self.s.close()
        except Exception:
            pass


# ============================================================================
# 工具函数
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


def iperf_summary(text):
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


def kill_iperf():
    # Do not kill global iperf/iperf3 servers on the host. The server process
    # started by this script is terminated through its Popen handle.
    return


def ms_to_units(ms):
    """毫秒 → BLE 协议单位（0.625ms）"""
    return int(round(ms / 0.625))


# ============================================================================
# WiFi 操作
# ============================================================================
def wifi_connect(board, ssid, password=None, timeout=30):
    """板子连接 WiFi，返回 (成功?, IP or None)"""
    st = board.cmd("wifi_state", 1.5)
    if "Connected" in st:
        print(f"  板{board.name} WiFi 已连接")
        return True, None
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


def wifi_get_ip(board):
    out = board.cmd("wifi_sta_dhcp", 2.5)
    m = re.search(r"IP:\s*([\d.]+)", out)
    return m.group(1) if m else None


# ============================================================================
# BLE 操作
# ============================================================================
def ble_init(board):
    board.cmd("ble_init", 1.0)
    board.cmd("ble_enable", 2.0)


def ble_scan_start(board, interval_ms=40, window_ms=20):
    """启动 BLE 扫描（占空比 window/interval）。返回成功?"""
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
    """启动可连接广播"""
    board.cmd("ble_start_adv 0 0", 1.5)


def ble_adv_stop(board):
    board.cmd("ble_stop_adv", 1.0)


# ============================================================================
# Coex 操作
# ============================================================================
def coex_enable(board):
    board.cmd("wifi_coex_start ps_pta", 1.5)


def coex_disable(board):
    board.cmd("wifi_coex_stop", 1.5)


def coex_duty_set(board, active_ms):
    """设置 WiFi active 时间（10-90ms）"""
    out = board.cmd(f"wifi_coex_duty_set {active_ms}", 1.5)
    ok = "coex duty:" in out.lower()
    if ok:
        print(f"  共存 duty 设置: WiFi active={active_ms}ms")
    else:
        print(f"  duty 设置失败: {out.strip()[:100]}")
    return ok


def coex_status(board):
    """读取共存状态，返回解析后的 dict"""
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
    """The explicit PS-PTA request must commit a PS-PTA activation."""
    return (status.get("active") is True and
            status.get("runtime") == "ps_pta")


def wifi_connect_with_coex(board, args):
    """连接 WiFi 后立即开启 coex；所有共存测试都必须走这个入口。"""
    ok, ip = wifi_connect(board, args.ssid, args.password)
    if not ok:
        return False, None
    if not ip:
        ip = wifi_get_ip(board)
    if not ip:
        print("  [失败] 获取不到板A IP")
        return False, None

    coex_enable(board)
    coex_duty_set(board, args.coex_duty)
    status = coex_status(board)
    if not coex_is_active(status):
        print(f"  [失败] coex 未进入有效状态: {status}")
        return False, ip
    print(f"  coex 已开启: {status.get('coex_state')} / "
          f"{status.get('pta_role')} / duty={status.get('wifi_duty_ms')}ms")
    return True, ip


def ble_tx_count_for_secs(seconds):
    """ble_test_tx 约 25ms/包，额外多给一点包数覆盖命令调度耗时。"""
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
    """快速验证 BLE 扫描能收到板B 的包。返回 (收到事件数, 输出)。
    用于独立性测试中验证 BLE 功能是否正常。"""
    # 板B 发少量包
    count = max(5, int(seconds * 1000 / 25))
    b.s.reset_input_buffer()
    b.send(f"ble_test_tx {count}")
    time.sleep(0.3)
    events, out = count_ble_scan_events(a, seconds)
    return events, out


def ping_command(ip, count, interval_ms=None, size=None):
    if os.name == "nt":
        cmd = ["ping", "-n", str(count), "-w", "1000"]
        if size is not None:
            cmd += ["-l", str(size)]
        cmd.append(ip)
        return cmd

    cmd = ["ping", "-c", str(count)]
    if interval_ms is not None:
        cmd += ["-i", str(interval_ms / 1000.0)]
    if size is not None:
        cmd += ["-s", str(size)]
    cmd += ["-W", "1", ip]
    return cmd


def parse_ping_metrics(out, expected_count=None):
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

    metrics = {
        "tx": tx, "rx": rx, "loss": loss, "rtts": rtts,
        "p50": None, "p95": None, "iqr": None,
        "min": None, "avg": None, "max": None,
    }
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
# iperf 辅助
# ============================================================================
def collect_tcp_up(srv, timeout=10):
    """收集 PC iperf server 输出并返回 (Mbps, 原始输出)"""
    srv.terminate()
    try:
        pc_out, _ = srv.communicate(timeout=timeout)
    except Exception:
        srv.kill()
        pc_out = ""
    kill_iperf()
    return iperf_summary(pc_out), pc_out


# ============================================================================
# 测试1：共存吞吐
# ============================================================================
def test_throughput(a, b, args):
    """共存吞吐测试：WiFi 连接后开 coex，再用 100% BLE 扫描覆盖 iperf。"""
    print(f"\n{'='*56}")
    print(f"=== 共存吞吐测试 ===")
    logf = open_log("coex_throughput")
    a.logf = logf

    ok, ip = wifi_connect_with_coex(a, args)
    if not ok:
        logf.close()
        return None

    print(f"\n--- 步骤1: 开启 100% BLE 扫描并覆盖完整 iperf ---")
    scan_interval = args.throughput_scan_interval
    scan_window = args.throughput_scan_interval
    bg_secs = args.time + args.ble_cover_extra
    scan_ok, b_count = start_ble_background(a, b, bg_secs,
                                            scan_interval, scan_window)
    if not scan_ok:
        logf.close()
        return None
    print(f"  BLE 发包数: {b_count}，扫描覆盖 {bg_secs}s")
    time.sleep(1)

    print(f"\n--- 步骤2: coex + BLE 扫描下 WiFi 吞吐 ---")
    ri = str(args.report_interval)
    kill_iperf()
    time.sleep(0.3)
    srv = subprocess.Popen(
        ["iperf", "-s", "-i", ri, "-p", str(args.port)],
        stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
    time.sleep(1)

    # 暂停 BLE 扫描，避免 [DEVICE] 输出干扰 iperf 命令发送
    ble_scan_stop(a)
    time.sleep(0.3)
    a.s.reset_input_buffer()

    dut_cmd = f"iperf -c {args.pc_ip} -i {ri} -t {args.time} -p {args.port}"
    print(f"  DUT: {dut_cmd}")
    dut_out = a.cmd(dut_cmd, args.time + 8)

    # iperf 结束后恢复 BLE 扫描（覆盖剩余时间）
    scan_remaining = bg_secs - args.time - 5
    if scan_remaining > 0:
        scan_cmd = f"ble_start_scan 1 0 {scan_interval:04X} {scan_window:04X}"
        a.cmd(scan_cmd, 1.0)
        print(f"  BLE 扫描已恢复 (剩余 {scan_remaining}s)")

    coex_mbps, pc_out = collect_tcp_up(srv)
    if coex_mbps is None:
        coex_mbps = iperf_summary(dut_out)
    logf.write(f"\n--- 共存吞吐 PC 输出 ---\n{pc_out}")

    # 清理
    ble_scan_stop(a)
    coex_disable(a)
    a.cmd("wifi_state", 1.0)  # 清理 DUT 可能残留的 iperf

    # 结果
    print(f"\n--- 结果 ---")
    print(f"  共存吞吐: {coex_mbps:.2f} Mbps" if coex_mbps
          else "  共存吞吐: 未获取")
    print("  折损比例: 请用纯 WiFi 吞吐基线结果在汇总表中计算")
    res = {"coex_mbps": coex_mbps, "ble_scan_interval_ms": scan_interval,
           "ble_scan_window_ms": scan_window, "ble_tx_count": b_count}

    # 环境标记
    if args.environment:
        print(f"  环境标记: {args.environment}")

    logf.close()
    a.logf = None
    return res


# ============================================================================
# 测试2：共存独立性
# ============================================================================
def test_independ(a, b, args):
    """共存独立性：两个场景。

    场景 A：WiFi ping 在后台跑 → 开 BLE 扫描 → 验证 WiFi(ping) 没中断、BLE 正常。
    场景 B：BLE 扫描在后台跑 → 连 WiFi + coex_enable → 验证 BLE 扫描没中断、WiFi 正常。

    两个场景中「背景任务」必须覆盖「前台操作」完整时长。
    """
    print(f"\n{'='*56}")
    print(f"=== 共存独立性测试 ===")
    logf = open_log("coex_independ")
    a.logf = logf
    passed = 0
    total = 2
    results = {}
    dur = args.independ_duration  # 背景任务持续时长

    # ==================================================================
    # 场景 A：ping 背景中开 BLE 扫描
    #   流程：WiFi 连接 → 启动 ping(后台) → 等预热 → 开 BLE 扫描
    #   → 等 ping 结束 → 验证 ping 收包率 + BLE 扫描启动成功
    # ==================================================================
    print(f"\n--- 场景 A: WiFi ping 过程中打开 BLE 扫描 ---")
    ok, ip = wifi_connect_with_coex(a, args)
    if not ok:
        print("  场景 A: 失败，WiFi/coex 准备失败")
        results["scene_a"] = {"failed": "wifi/coex"}
    else:
        # 启动 ping（后台进程）
        count = max(1, int(dur * 1000 / args.interval))
        cmd = ping_command(ip, count, args.interval, args.size)
        print(f"  后台启动 ping：{' '.join(cmd)}")
        ping_proc = subprocess.Popen(cmd, stdout=subprocess.PIPE,
                                     stderr=subprocess.STDOUT, text=True)
        # 预热：让 ping 先稳定跑几秒
        time.sleep(args.independ_warmup)

        # 开 BLE 扫描（前台操作）
        scan_ok, b_count = start_ble_background(
            a, b, dur + args.ble_cover_extra,
            args.scan_interval, args.scan_window)

        # 等 ping 跑完
        try:
            ping_out, _ = ping_proc.communicate(timeout=dur + 30)
        except Exception:
            ping_proc.kill()
            ping_out, _ = ping_proc.communicate()
        logf.write(f"\n--- 场景 A ping 输出 ---\n{ping_out}")

        # 清理
        ble_scan_stop(a)
        coex_disable(a)

        # 验证
        metrics = parse_ping_metrics(ping_out, count)
        wifi_ok = metrics["rx"] >= max(1, int(count * args.ping_pass_ratio))
        scene_ok = scan_ok and wifi_ok
        passed += 1 if scene_ok else 0
        results["scene_a"] = {
            "scan_ok": scan_ok, "ble_tx_count": b_count, **metrics}
        print(f"  BLE 扫描启动: {'通过' if scan_ok else '失败'}")
        print_ping_metrics(metrics)
        print(f"  场景 A: {'通过' if scene_ok else '失败'}")

    # ==================================================================
    # 场景 B：BLE 扫描背景中连 WiFi + coex
    #   流程：BLE 扫描已在跑 → 连 WiFi + coex_enable
    #   → 验证 BLE 扫描没中断 + WiFi ping 正常
    #   关键：BLE 扫描必须在 WiFi 连接之前就已经稳定工作
    # ==================================================================
    print(f"\n--- 场景 B: BLE 扫描过程中连接 WiFi 并进入 coex ---")
    wifi_disconnect(a)
    time.sleep(1)

    # 先开 BLE 扫描（背景任务）
    scan_secs = dur + args.ble_cover_extra
    scan_ok, b_count = start_ble_background(
        a, b, scan_secs, args.scan_interval, args.scan_window)
    if not scan_ok:
        print("  场景 B: 失败，BLE 扫描启动失败")
        results["scene_b"] = {"failed": "ble_scan"}
    else:
        # 预热：让 BLE 扫描先稳定跑几秒
        time.sleep(args.independ_warmup)

        # 暂停 BLE 扫描输出，避免干扰 WiFi 命令发送
        ble_scan_stop(a)
        time.sleep(0.3)
        a.s.reset_input_buffer()

        # 前台操作：连 WiFi + 开共存
        wifi_ok, ip = wifi_connect_with_coex(a, args)

        # WiFi 连好后恢复 BLE 扫描
        if wifi_ok:
            scan_cmd = f"ble_start_scan 1 0 {args.scan_interval:04X} {args.scan_window:04X}"
            a.cmd(scan_cmd, 1.0)
            print("  BLE 扫描已恢复")

        # 验证 BLE 扫描仍在工作（读几秒看有没有 DEVICE 事件）
        ble_events, ble_out = check_ble_receiving(
            a, b, seconds=3,
            interval_ms=args.scan_interval, window_ms=args.scan_window)
        logf.write(f"\n--- 场景 B BLE 扫描输出 ---\n{ble_out}")
        ble_alive = scan_ok and ble_events > 0

        # 验证 WiFi 连通（ping）
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

        # 清理
        ble_scan_stop(a)
        coex_disable(a)

        scene_ok = ble_alive and wifi_ok and ping_ok
        passed += 1 if scene_ok else 0
        results["scene_b"] = {
            "scan_ok": scan_ok, "ble_events": ble_events,
            "ble_tx_count": b_count, "wifi_coex_ok": wifi_ok,
            "ping_ok": ping_ok, **ping_metrics}
        print(f"  BLE 扫描启动: {'通过' if scan_ok else '失败'}")
        print(f"  BLE 扫描事件(WiFi连接后): {ble_events}")
        if ping_metrics:
            print_ping_metrics(ping_metrics)
        print(f"  场景 B: {'通过' if scene_ok else '失败'}")

    # 汇总
    print(f"\n--- 结果 ---")
    print(f"  通过 {passed}/{total} 项")
    if args.environment:
        print(f"  环境标记: {args.environment}")

    logf.close()
    a.logf = None
    return {"passed": passed, "total": total, "details": results}


# ============================================================================
# 测试3：共存下 ping 时延
# ============================================================================
def test_latency(a, b, args):
    """共存下 ping 时延：WiFi 连接 + BLE 扫描/广播。"""
    print(f"\n{'='*56}")
    print(f"=== 共存下 ping 时延测试 ===")
    logf = open_log("coex_latency")
    a.logf = logf

    ok, ip = wifi_connect_with_coex(a, args)
    if not ok:
        logf.close()
        return None

    scan_ok, b_count = start_ble_background(
        a, b, args.duration + args.ble_cover_extra,
        args.scan_interval, args.scan_window)
    if not scan_ok:
        logf.close()
        return None
    print(f"  BLE 发包数: {b_count}，扫描覆盖 "
          f"{args.duration + args.ble_cover_extra}s")
    time.sleep(1)

    # ping
    count = max(1, int(args.duration * 1000 / args.interval))
    print(f"  ping {ip}：{count} 个包，间隔 {args.interval}ms，"
          f"包大小 {args.size}B，约 {args.duration}s")

    cmd = ping_command(ip, count, args.interval, args.size)
    print(f"  执行：{' '.join(cmd)}")
    r = subprocess.run(cmd, capture_output=True, text=True,
                       timeout=args.duration + 30)
    out = r.stdout + r.stderr
    logf.write(out)

    # 清理
    ble_scan_stop(a)
    coex_disable(a)

    # 解析
    metrics = parse_ping_metrics(out, count)
    print(f"\n--- 结果 ---")
    if metrics["rtts"]:
        print_ping_metrics(metrics)
        res = {k: metrics[k] for k in (
            "tx", "rx", "loss", "min", "avg", "max", "p50", "p95", "iqr")}
        res["ble_tx_count"] = b_count
    else:
        print(f"  [失败] 未收到回包，丢包率 = {metrics['loss']}%")
        res = {k: metrics[k] for k in ("tx", "rx", "loss")}
        res.update({"min": None, "avg": None, "max": None,
                    "p50": None, "p95": None, "iqr": None,
                    "ble_tx_count": b_count})

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


def interactive():
    print("=" * 56)
    print("        Wi-Fi/BLE 共存性能测试工具（交互式）")
    print("=" * 56)
    print("提示：Windows 串口形如 COM3；Linux 形如 /dev/ttyUSB0\n")
    port_a = ask("请输入 板A(DUT) 串口")
    port_b = ask("请输入 板B(BLE 对端) 串口")
    a = Board(port_a, "A")
    b = Board(port_b, "B")

    ssid = ask(f"路由器 SSID", SSID)
    pwd = ask("路由器密码（开放网络直接回车）", "")
    env = ask("当前环境干扰情况（无干扰/轻度/重度，用于记录）", "无干扰")

    ns = argparse.Namespace(
        ssid=ssid, password=pwd or None, environment=env,
        pc_ip=None, time=30, port=5005, report_interval=1,
        scan_interval=40, scan_window=20, coex_duty=50,
        duration=60, interval=500, size=300,
        throughput_scan_interval=10, ble_cover_extra=5,
        independ_duration=20, independ_warmup=3, ping_pass_ratio=0.6,
    )

    # PC IP
    pc_ip = ask("PC 的 IP（与路由器同网段，用于吞吐测试）")
    ns.pc_ip = pc_ip if pc_ip else None

    try:
        while True:
            print("\n" + "-" * 56)
            print("请选择测试项：")
            print("  1) 共存吞吐测试（WiFi->coex->BLE 100%扫描->iperf）")
            print("  2) 共存独立性（两个背景场景）")
            print("  3) 共存下 ping 时延")
            print("  0) 退出")
            choice = ask("输入序号", "1")

            if choice == "0":
                break

            # 每项测试前可调参数
            ns.time = ask_num("吞吐测试时长(秒)", ns.time)
            ns.coex_duty = ask_num("共存 WiFi active 时间(ms, 10-90)", ns.coex_duty)
            ns.scan_interval = ask_num("BLE 扫描 interval(ms)", ns.scan_interval)
            ns.scan_window = ask_num("BLE 扫描 window(ms)", ns.scan_window)

            if choice == "1":
                if not ns.pc_ip:
                    print("  [错误] 吞吐测试需要 PC IP")
                    continue
                ns.throughput_scan_interval = ask_num(
                    "吞吐测试 BLE 100% 扫描 interval(ms)",
                    ns.throughput_scan_interval)
                ns.ble_cover_extra = ask_num(
                    "BLE 扫描额外覆盖时间(秒)", ns.ble_cover_extra)
                test_throughput(a, b, ns)
            elif choice == "2":
                ns.independ_duration = ask_num(
                    "独立性背景测试时长(秒)", ns.independ_duration)
                ns.independ_warmup = ask_num(
                    "背景切换前预热时间(秒)", ns.independ_warmup)
                test_independ(a, b, ns)
            elif choice == "3":
                ns.duration = ask_num("时延测试时长(秒)", ns.duration)
                ns.ble_cover_extra = ask_num(
                    "BLE 扫描额外覆盖时间(秒)", ns.ble_cover_extra)
                test_latency(a, b, ns)
            else:
                print("无效选择")
    finally:
        a.close()
        b.close()
        print("\n已退出。")


# ============================================================================
# 命令行
# ============================================================================
def main():
    if len(sys.argv) == 1:
        interactive()
        return

    p = argparse.ArgumentParser(description="Wi-Fi/BLE 共存性能测试工具")
    p.add_argument("test",
                   choices=["throughput", "independ", "latency"],
                   help="测试项")
    p.add_argument("-a", required=True, help="板A(DUT) 串口")
    p.add_argument("-b", required=True, help="板B(BLE 对端) 串口")
    p.add_argument("--pc-ip", help="PC 的 IP（吞吐测试需要）")
    p.add_argument("--ssid", default=SSID, help=f"路由器 SSID（默认 {SSID}）")
    p.add_argument("--password", help="路由器密码（开放网络不填）")
    p.add_argument("-t", "--time", type=int, default=30,
                   help="吞吐测试时长秒（默认30）")
    p.add_argument("-p", "--port", type=int, default=5005, help="iperf 端口（默认5005）")
    p.add_argument("-i", "--report-interval", type=int, default=1,
                   help="iperf 报告间隔秒（默认1）")
    p.add_argument("--bw", default="50M", help="UDP 带宽（保留参数）")
    p.add_argument("--coex-duty", type=int, default=50,
                   help="共存 WiFi active 时间 ms（10-90，默认50）")
    p.add_argument("--scan-interval", type=float, default=40,
                   help="BLE 扫描 interval 毫秒（默认40）")
    p.add_argument("--scan-window", type=float, default=20,
                   help="BLE 扫描 window 毫秒（默认20）")
    p.add_argument("--throughput-scan-interval", type=float, default=10,
                   help="吞吐测试 BLE 100%% 扫描 interval/window 毫秒（默认10）")
    p.add_argument("--ble-cover-extra", type=int, default=5,
                   help="BLE 背景扫描额外覆盖时间秒（默认5）")
    p.add_argument("--independ-duration", type=int, default=20,
                   help="独立性背景测试时长秒（默认20）")
    p.add_argument("--independ-warmup", type=int, default=3,
                   help="独立性背景切换前预热秒（默认3）")
    p.add_argument("--ping-pass-ratio", type=float, default=0.6,
                   help="独立性 ping 通过收包比例（默认0.6）")
    p.add_argument("--duration", type=int, default=60,
                   help="时延测试时长秒（默认60）")
    p.add_argument("--interval", type=int, default=500,
                   help="时延 ping 间隔 ms（默认500）")
    p.add_argument("--size", type=int, default=300,
                   help="时延 ping 包大小 B（默认300）")
    p.add_argument("--environment", default=None,
                   help="环境干扰标记（无干扰/轻度/重度，用于记录）")
    args = p.parse_args()

    if args.test == "throughput" and not args.pc_ip:
        p.error("throughput 测试需要 --pc-ip")

    a = Board(args.a, "A")
    b = Board(args.b, "B")
    try:
        if args.test == "throughput":
            test_throughput(a, b, args)
        elif args.test == "independ":
            test_independ(a, b, args)
        elif args.test == "latency":
            test_latency(a, b, args)
    finally:
        a.close()
        b.close()


if __name__ == "__main__":
    main()
