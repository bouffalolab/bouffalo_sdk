#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
WiFi 性能测试工具（吞吐/时延/连接耗时/稳定性）。

被测对象：一块通过串口连接的 DUT（运行 wfa 固件），与本机(PC)对测。
PC 需与 DUT 连接同一路由器（DUT 走 WiFi，PC 建议有线接入）。

测试项：
  tcp-up    TCP 上行吞吐 (DUT -> PC)
  tcp-down  TCP 下行吞吐 (PC -> DUT)
  udp-up    UDP 上行吞吐 (DUT -> PC)   含丢包率
  udp-down  UDP 下行吞吐 (PC -> DUT)   含丢包率 + 抖动
  latency   时延测试 (PC ping DUT)     P50/P95/IQR/丢包率
  conn      连接耗时 (发起连接 -> 拿到IP)
  stable    连接稳定性 (长时间 iperf, 记录断连次数/最低吞吐)
  all       依次跑 tcp-up/tcp-down/udp-up/udp-down/latency/conn

依赖：
  - pip install pyserial
  - PC 端 iperf **2.x**（与固件 iperf2 协议匹配，不能用 iperf3！）
      Linux: apt install iperf      Windows: 下载 iperf 2.x 放到 PATH 或用 --iperf 指定
  - ping：用系统自带 ping（无需 root，跨平台）

用法示例：
  python wifi_test.py all      -P /dev/ttyUSB0 --pc-ip 192.168.5.200 --password 12345678
  python wifi_test.py tcp-up   -P COM3 --pc-ip 192.168.5.200 -t 60 -p 5005
  python wifi_test.py udp-down -P COM3 --pc-ip 192.168.5.200 -t 60 -b 50M
  python wifi_test.py latency  -P COM3 --duration 600 --interval 200 --size 300
  python wifi_test.py stable   -P COM3 --pc-ip 192.168.5.200 --hours 1
  不带参数运行进入交互式：  python wifi_test.py
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
SSID = "nx30"            # 默认路由器 SSID，可用 --ssid 改
LOG_DIR = "logs"


# ============================================================================
# 串口（DUT）封装
# ============================================================================
class Dut:
    def __init__(self, port, logf=None):
        try:
            self.s = serial.Serial(port, BAUD, timeout=1)
        except Exception as e:
            print(f"[错误] 打开串口 {port} 失败：{e}")
            sys.exit(1)
        self.port = port
        self.logf = logf

    @staticmethod
    def _clean(s):
        return re.sub(r"\x1b\[[0-9;]*m", "", s)

    def _log(self, text):
        if self.logf:
            self.logf.write(text)
            self.logf.flush()

    def flush_in(self):
        time.sleep(0.2)
        self.s.reset_input_buffer()

    def send(self, cmd):
        """只发命令不等待"""
        self.s.write((cmd + "\r\n").encode())
        self._log(f"\n>>> {cmd}\n")

    def cmd(self, cmd, wait):
        """发命令并收集 wait 秒输出"""
        self.s.reset_input_buffer()
        self.send(cmd)
        return self.read_for(wait)

    def read_for(self, secs):
        """读取 secs 秒内的所有输出"""
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
        """读到含 keyword 的输出或超时。返回 (命中?, 文本, 耗时秒)"""
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
    """从 iperf 文本里取最后一条汇总行的吞吐(Mbits/sec)。
    匹配形如：  0.0- 5.0 sec  2.43 MByte  4.07 Mbits/sec
    返回 (吞吐Mbps or None, 命中的行)。"""
    pat = re.compile(
        r"([\d.]+)\s*-\s*([\d.]+)\s*sec\s+[\d.]+\s*[KMG]?Byte[s]?\s+"
        r"([\d.]+)\s*([KMG]?)bits/sec"
    )
    last = None
    for m in pat.finditer(text):
        val = float(m.group(3))
        unit = m.group(4)
        mbps = {"K": val / 1000, "M": val, "G": val * 1000, "": val / 1e6}[unit]
        last = (mbps, m.group(0).strip())
    return last if last else (None, None)


def iperf_udp_loss(text):
    """从 UDP server 汇总行取丢包率和抖动。
    匹配：  0.0- 8.6 sec 22.8 MByte 22.3 Mbits/sec 3.941 ms 18/15962 (0.1%)
    返回 (丢包率%, 抖动ms, 命中行) 或 (None,None,None)。"""
    pat = re.compile(
        r"sec\s+[\d.]+\s*[KMG]?Byte[s]?\s+[\d.]+\s*[KMG]?bits/sec\s+"
        r"([\d.]+)\s*ms\s+(\d+)/(\d+)\s+\(([\d.]+)%\)"
    )
    last = None
    for m in pat.finditer(text):
        jitter = float(m.group(1))
        loss = float(m.group(4))
        last = (loss, jitter, m.group(0).strip())
    return last if last else (None, None, None)


def kill_iperf():
    subprocess.run(["pkill", "-f", "iperf"], capture_output=True)
    if os.name == "nt":
        subprocess.run(["taskkill", "/F", "/IM", "iperf.exe"], capture_output=True)


# ============================================================================
# DUT 连接管理
# ============================================================================
def dut_is_connected(dut):
    out = dut.cmd("wifi_state", 1.5)
    return "Connected" in out


def dut_get_ip(dut):
    """从 wifi_sta_dhcp / 连接日志里取 DUT 的 IP"""
    out = dut.cmd("wifi_sta_dhcp", 2.5)
    m = re.search(r"IP:\s*([\d.]+)", out)
    if m:
        return m.group(1)
    return None


def dut_connect(dut, ssid, password=None, timeout=20):
    """发起连接并等到拿到 IP。返回 (成功?, 耗时秒, IP)。
    固件命令格式: wifi_sta_connect <ssid> [password]"""
    dut.s.reset_input_buffer()
    t0 = time.time()
    cmd = f"wifi_sta_connect {ssid}"
    if password:
        cmd += f" {password}"
    dut.send(cmd)
    ok, text, _ = dut.read_until("CODE_WIFI_ON_GOT_IP", timeout)
    elapsed = time.time() - t0
    ip = None
    m = re.search(r"IP:\s*([\d.]+)", text)
    if m:
        ip = m.group(1)
    return ok, elapsed, ip


def ensure_connected(dut, args):
    """确保 DUT 已连网，返回 IP"""
    if dut_is_connected(dut):
        ip = dut_get_ip(dut)
        if ip:
            print(f"  DUT 已连接，IP={ip}")
            return ip
    print(f"  DUT 连接 {args.ssid} ...")
    ok, t, ip = dut_connect(dut, args.ssid, args.password)
    if ok and ip:
        print(f"  连接成功，IP={ip}（耗时 {t:.1f}s）")
        return ip
    print("  [失败] DUT 未能连上网")
    return None


# ============================================================================
# 各测试
# ============================================================================
def test_throughput(dut, args, proto, direction):
    """proto: tcp/udp   direction: up(DUT->PC) / down(PC->DUT)"""
    name = f"{proto}-{direction}"
    title = {
        "tcp-up": "TCP 上行吞吐 (DUT->PC)",
        "tcp-down": "TCP 下行吞吐 (PC->DUT)",
        "udp-up": "UDP 上行吞吐 (DUT->PC)",
        "udp-down": "UDP 下行吞吐 (PC->DUT)",
    }[name]
    print(f"\n=== {title} ===")
    logf = open_log(name)
    dut.logf = logf

    ip = ensure_connected(dut, args)
    if not ip:
        logf.close()
        return None

    is_udp = proto == "udp"
    udp_flag = ["-u"] if is_udp else []
    bw = ["-b", args.bw] if is_udp else []
    port = ["-p", str(args.port)]
    dut_bw = f" -b {args.bw}" if is_udp else ""
    kill_iperf()
    time.sleep(0.5)
    result = {}

    ri = str(args.report_interval)
    if direction == "up":
        # PC=server, DUT=client。吞吐看 PC server；UDP 丢包看 PC server。
        srv = subprocess.Popen(
            ["iperf", "-s", "-i", ri] + udp_flag + port,
            stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        time.sleep(1)
        dut_cmd = (f"iperf -c {args.pc_ip} -i {ri} -t {args.time} -p {args.port}"
                   f"{' -u' if is_udp else ''}{dut_bw}")
        print(f"  DUT: {dut_cmd}")
        dut_out = dut.cmd(dut_cmd, args.time + 6)
        srv.terminate()
        try:
            pc_out, _ = srv.communicate(timeout=5)
        except Exception:
            srv.kill(); pc_out = ""
        logf.write("\n----- PC(server) 输出 -----\n" + pc_out)
        mbps, _ = iperf_summary(pc_out)
        if mbps is None:  # 退回 DUT 端汇总
            mbps, _ = iperf_summary(dut_out)
        result["mbps"] = mbps
        if is_udp:
            loss, jitter, _ = iperf_udp_loss(pc_out)
            result["loss"] = loss
            result["jitter"] = jitter
    else:
        # PC=client, DUT=server。下行 UDP 丢包/抖动看 DUT server。
        dut.s.reset_input_buffer()
        dut.send(f"iperf -s -i {ri} -p {args.port}{' -u' if is_udp else ''}")
        time.sleep(1.5)
        dut.read_for(0.5)  # 吃掉 server 启动回显
        pc_cmd = (["iperf", "-c", ip, "-i", ri, "-t", str(args.time)]
                  + port + udp_flag + bw)
        print(f"  PC: {' '.join(pc_cmd)}")
        r = subprocess.run(pc_cmd, capture_output=True, text=True,
                           timeout=args.time + 15)
        pc_out = r.stdout + r.stderr
        logf.write("\n----- PC(client) 输出 -----\n" + pc_out)
        # 等 DUT server 把收尾汇总打完
        dut_out = dut.read_for(3)
        # 停掉 DUT server（再发个回车/命令打断）
        dut.cmd("wifi_state", 1.0)
        if is_udp:
            mbps, _ = iperf_summary(dut_out)
            loss, jitter, _ = iperf_udp_loss(dut_out)
            result["loss"] = loss
            result["jitter"] = jitter
        else:
            mbps, _ = iperf_summary(pc_out)
        result["mbps"] = mbps

    kill_iperf()
    # 打印结果
    if result.get("mbps") is not None:
        msg = f"  吞吐 = {result['mbps']:.2f} Mbps"
        if is_udp:
            if result.get("loss") is not None:
                msg += f" | 丢包率 = {result['loss']}%"
            if result.get("jitter") is not None:
                msg += f" | 抖动 = {result['jitter']} ms"
        print(msg)
        # UDP 上行丢包率说明：固件 iperf client 不发 UDP-FIN，
        # PC server 无法生成接收报告，故上行丢包率/抖动不可得（非脚本 bug）。
        if is_udp and direction == "up" and result.get("loss") is None:
            print("  注：UDP 上行的丢包率/抖动由接收端(PC)统计，"
                  "但当前固件不发 UDP-FIN，PC 无法生成报告，故此项不可得。")
    else:
        print("  [失败] 未解析到吞吐，请查看日志")
    logf.close()
    dut.logf = None
    return result


def test_latency(dut, args):
    """PC ping DUT，算 P50/P95/IQR/丢包率。"""
    print(f"\n=== 时延测试 (PC ping DUT) ===")
    logf = open_log("latency")
    dut.logf = logf

    ip = ensure_connected(dut, args)
    if not ip:
        logf.close()
        return None

    count = max(1, int(args.duration * 1000 / args.interval))
    print(f"  ping {ip}：{count} 个包，间隔 {args.interval}ms，"
          f"包大小 {args.size}B，约 {args.duration}s")

    if os.name == "nt":
        cmd = ["ping", "-n", str(count), "-l", str(args.size),
               "-w", "1000", ip]
    else:
        cmd = ["ping", "-c", str(count), "-i", str(args.interval / 1000.0),
               "-s", str(args.size), "-W", "1", ip]
    print(f"  执行：{' '.join(cmd)}")
    r = subprocess.run(cmd, capture_output=True, text=True,
                       timeout=args.duration + 30)
    out = r.stdout + r.stderr
    logf.write(out)

    # 解析每个 rtt
    rtts = [float(x) for x in re.findall(r"time[=<]\s*([\d.]+)\s*ms", out)]
    # 丢包率
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
        print(f"  收到 {len(rtts)} 个回包 | 丢包率 = {loss}%")
        print(f"  P50 = {p50:.2f} ms | P95 = {p95:.2f} ms | IQR = {iqr:.2f} ms")
        res = {"loss": loss, "p50": p50, "p95": p95, "iqr": iqr, "n": len(rtts)}
    else:
        print(f"  [失败] 未收到回包，丢包率 = {loss}%")
        res = {"loss": loss, "p50": None, "p95": None, "iqr": None, "n": 0}
    logf.close()
    dut.logf = None
    return res


def test_conn(dut, args):
    """连接耗时：先断开，再发起连接，计时到拿到 IP。"""
    print(f"\n=== 连接耗时测试 ===")
    logf = open_log("conn")
    dut.logf = logf

    print("  先断开当前连接...")
    dut.cmd("wifi_sta_disconnect", 2.0)
    time.sleep(1)
    print(f"  发起连接 {args.ssid} 并计时...")
    ok, elapsed, ip = dut_connect(dut, args.ssid, args.password, timeout=30)
    if ok:
        print(f"  连接耗时 = {elapsed:.2f} 秒（拿到 IP {ip}）")
        res = {"seconds": round(elapsed, 2), "ip": ip}
    else:
        print(f"  [失败] {30}s 内未拿到 IP")
        res = {"seconds": None, "ip": None}
    logf.close()
    dut.logf = None
    return res


def test_stable(dut, args):
    """连接稳定性：长时间 TCP 上行 iperf，记录断连次数和最低分段吞吐。"""
    hours = args.hours
    print(f"\n=== 连接稳定性测试（{hours} 小时）===")
    logf = open_log("stable")
    dut.logf = logf

    ip = ensure_connected(dut, args)
    if not ip:
        logf.close()
        return None

    total_sec = int(hours * 3600)
    ri = str(args.report_interval)
    kill_iperf()
    time.sleep(0.5)
    srv = subprocess.Popen(["iperf", "-s", "-i", ri, "-p", str(args.port)],
                           stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
    time.sleep(1)

    # DUT 持续做 client（-t 设总时长，-i 设报告间隔）
    dut_cmd = f"iperf -c {args.pc_ip} -i {ri} -t {total_sec} -p {args.port}"
    print(f"  DUT: {dut_cmd}")
    print(f"  开始，预计 {hours} 小时。每 60s 打印一次进度...")
    dut.s.reset_input_buffer()
    dut.send(dut_cmd)

    disconnects = 0
    min_mbps = None
    samples = []          # 所有有效分段吞吐，用于算中位数（识别离群点）
    t0 = time.time()
    last_print = 0
    # 分段行同时捕获起止时间，用于判断间隔是否异常（CPU 高负载会使间隔不准）
    seg_pat = re.compile(
        r"([\d.]+)\s*-\s*([\d.]+)\s*sec\s+[\d.]+\s*[KMG]?Byte[s]?\s+"
        r"([\d.]+)\s*([KMG]?)bits/sec")
    buf = ""
    while time.time() - t0 < total_sec + 30:
        x = dut.s.read(4096)
        if x:
            seg = dut._clean(x.decode("utf-8", errors="replace"))
            dut._log(seg)
            buf += seg
            # 检测断连
            if "DISCONNECT" in buf.upper() or "CODE_WIFI_ON_DISCONNECT" in buf:
                disconnects += 1
                buf = ""
            # 解析分段吞吐
            for m in seg_pat.finditer(buf):
                t_start, t_end = float(m.group(1)), float(m.group(2))
                val = float(m.group(3))
                unit = m.group(4)
                mbps = {"K": val/1000, "M": val, "G": val*1000, "": val/1e6}[unit]
                seg_dur = t_end - t_start
                # 跳过 0（断连/收尾）和异常短分段（间隔不准导致的瞬时虚低）。
                # 模组打流时 CPU 负载高，-i 报告间隔可能不准，过短分段的瞬时
                # 吞吐不代表真实链路下界，只纳入接近设定间隔的分段。
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

    srv.terminate()
    try:
        srv.communicate(timeout=5)
    except Exception:
        srv.kill()
    kill_iperf()
    mm = f"{min_mbps:.2f}" if min_mbps else "?"
    median = statistics.median(samples) if samples else None
    md = f"{median:.2f}" if median else "?"
    print(f"  完成 | 断连次数 = {disconnects} | 最低吞吐 = {mm} Mbps "
          f"| 吞吐中位数 = {md} Mbps | 有效采样 {len(samples)} 段")
    print("  说明：最低吞吐为单个分段瞬时值，模组打流时 CPU 负载高、报告间隔可能"
          "不准，建议结合中位数判断稳定性。")
    res = {"disconnects": disconnects, "min_mbps": min_mbps,
           "median_mbps": median, "segments": len(samples)}
    logf.close()
    dut.logf = None
    return res


# ============================================================================
# 交互式
# ============================================================================
def ask_num(prompt, default):
    """带默认值的数字输入（按默认值类型自动转 int/float）"""
    s = input(f"{prompt} [{default}]：").strip()
    if not s:
        return default
    try:
        v = float(s)
        return int(v) if isinstance(default, int) else v
    except ValueError:
        print(f"  输入无效，使用默认值 {default}")
        return default


def interactive(args):
    print("=" * 56)
    print("           WiFi 性能测试工具（交互式）")
    print("=" * 56)
    print("提示：Windows 串口形如 COM3；Linux 形如 /dev/ttyUSB0\n")
    serial_port = input("DUT 串口：").strip()
    args.pc_ip = input("PC 的 IP（与路由器同网段）：").strip()
    ssid = input(f"路由器 SSID [{SSID}]：").strip()
    if ssid:
        args.ssid = ssid
    pwd = input("路由器密码（开放网络直接回车）：").strip()
    args.password = pwd if pwd else None

    dut = Dut(serial_port)

    def with_time(fn, key, prompt, default):
        """执行测试前先询问本项时长"""
        setattr(args, key, ask_num(prompt, default))
        return fn()

    menu = [
        ("TCP 上行吞吐",
         lambda: with_time(lambda: test_throughput(dut, args, "tcp", "up"),
                           "time", "测试时长(秒)", args.time)),
        ("TCP 下行吞吐",
         lambda: with_time(lambda: test_throughput(dut, args, "tcp", "down"),
                           "time", "测试时长(秒)", args.time)),
        ("UDP 上行吞吐",
         lambda: with_time(lambda: test_throughput(dut, args, "udp", "up"),
                           "time", "测试时长(秒)", args.time)),
        ("UDP 下行吞吐",
         lambda: with_time(lambda: test_throughput(dut, args, "udp", "down"),
                           "time", "测试时长(秒)", args.time)),
        ("时延测试",
         lambda: with_time(lambda: test_latency(dut, args),
                           "duration", "测试时长(秒)", args.duration)),
        ("连接耗时",
         lambda: test_conn(dut, args)),  # 单次连接，无时长概念
        ("连接稳定性",
         lambda: with_time(lambda: test_stable(dut, args),
                           "hours", "测试时长(小时)", args.hours)),
    ]
    try:
        while True:
            print("\n" + "-" * 56)
            for i, (n, _) in enumerate(menu, 1):
                print(f"  {i}) {n}")
            print("  8) 全部（吞吐4项+时延+连接耗时）")
            print("  0) 退出")
            c = input("选择：").strip()
            if c == "0":
                break
            if c == "8":
                # 全部测试：统一先设各项时长，再依次执行
                args.time = ask_num("吞吐每项测试时长(秒)", args.time)
                args.duration = ask_num("时延测试时长(秒)", args.duration)
                test_throughput(dut, args, "tcp", "up")
                test_throughput(dut, args, "tcp", "down")
                test_throughput(dut, args, "udp", "up")
                test_throughput(dut, args, "udp", "down")
                test_latency(dut, args)
                test_conn(dut, args)
                continue
            if c.isdigit() and 1 <= int(c) <= 7:
                menu[int(c) - 1][1]()
            else:
                print("无效选择")
    finally:
        dut.close()
        print("\n已退出。")


# ============================================================================
# main
# ============================================================================
def main():
    if len(sys.argv) == 1:
        ns = argparse.Namespace(
            port=5005, pc_ip=None, ssid=SSID, password=None, time=60,
            bw="50M", duration=600, interval=200, size=300, hours=1.0,
            report_interval=1)
        interactive(ns)
        return

    p = argparse.ArgumentParser(description="WiFi 性能测试工具")
    p.add_argument("test", choices=["tcp-up", "tcp-down", "udp-up", "udp-down",
                                    "latency", "conn", "stable", "all"])
    p.add_argument("-P", "--serial", required=True,
                   help="DUT 串口，如 COM3 或 /dev/ttyUSB0")
    p.add_argument("--pc-ip", help="PC 的 IP（吞吐/稳定性测试需要）")
    p.add_argument("--ssid", default=SSID, help=f"路由器 SSID（默认 {SSID}）")
    p.add_argument("--password", default=None,
                   help="路由器密码（开放网络不填）")
    p.add_argument("-t", "--time", type=int, default=60, help="吞吐测试时长秒（默认60）")
    p.add_argument("-p", "--port", type=int, default=5005, help="iperf 端口（默认5005）")
    p.add_argument("-i", "--report-interval", type=int, default=1,
                   help="iperf 报告打印间隔秒（默认1；模组高负载时建议调大如5）")
    p.add_argument("-b", "--bw", default="50M", help="UDP 带宽（默认50M）")
    p.add_argument("--duration", type=int, default=600, help="时延测试时长秒（默认600）")
    p.add_argument("--interval", type=int, default=200, help="时延 ping 间隔ms（默认200）")
    p.add_argument("--size", type=int, default=300, help="时延 ping 包大小B（默认300）")
    p.add_argument("--hours", type=float, default=1, help="稳定性测试小时数（默认1）")
    args = p.parse_args()

    need_ip = args.test in ("tcp-up", "tcp-down", "udp-up", "udp-down",
                            "stable", "all")
    if need_ip and not args.pc_ip:
        print("[错误] 该测试需要 --pc-ip 指定 PC 的 IP")
        sys.exit(1)

    dut = Dut(args.serial)
    try:
        if args.test == "tcp-up":
            test_throughput(dut, args, "tcp", "up")
        elif args.test == "tcp-down":
            test_throughput(dut, args, "tcp", "down")
        elif args.test == "udp-up":
            test_throughput(dut, args, "udp", "up")
        elif args.test == "udp-down":
            test_throughput(dut, args, "udp", "down")
        elif args.test == "latency":
            test_latency(dut, args)
        elif args.test == "conn":
            test_conn(dut, args)
        elif args.test == "stable":
            test_stable(dut, args)
        elif args.test == "all":
            test_throughput(dut, args, "tcp", "up")
            test_throughput(dut, args, "tcp", "down")
            test_throughput(dut, args, "udp", "up")
            test_throughput(dut, args, "udp", "down")
            test_latency(dut, args)
            test_conn(dut, args)
    finally:
        dut.close()


if __name__ == "__main__":
    main()
