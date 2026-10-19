# WiFi 性能测试说明

本目录提供 WiFi 性能测试的脚本和操作文档，覆盖 7 个测试项。
**只需一块 DUT（被测板）+ 一台 PC**，DUT 走 WiFi、PC 接同一路由器（建议有线）。

| 测试项 | 命令 | 测什么 | 结果 |
|--------|------|--------|------|
| TCP 上行吞吐 | `tcp-up` | DUT→PC 方向 TCP 极限吞吐 | Mbps |
| TCP 下行吞吐 | `tcp-down` | PC→DUT 方向 TCP 极限吞吐 | Mbps |
| UDP 上行吞吐 | `udp-up` | DUT→PC 方向 UDP 吞吐 | Mbps（丢包见下方说明） |
| UDP 下行吞吐 | `udp-down` | PC→DUT 方向 UDP 吞吐 | Mbps + 丢包率 + 抖动 |
| 时延测试 | `latency` | PC ping DUT 的时延与丢包 | P50/P95/IQR + 丢包率 |
| 连接耗时 | `conn` | 发起连接到拿到 IP 的耗时 | 秒 |
| 连接稳定性 | `stable` | 长时间 iperf 跑机 | 断连次数 + 最低吞吐 |

---

## 一、准备工作

### 1. 硬件与环境
- **一块 BL616CL 开发板**（DUT），烧好 `wfa` 固件，USB 接电脑。
- **PC 与 DUT 连同一路由器**：DUT 走 WiFi，PC 建议**有线**接入（避免 PC 的 WiFi 与 DUT 抢空口）。
- 测试条件（来自需求）：屏蔽房、Wi-Fi 6 路由器、2.4GHz 20MHz。

### 2. 确认串口号
- **Windows**：设备管理器 → 端口(COM)，如 `COM3`。
- **Linux**：`ls /dev/ttyUSB*`，如 `/dev/ttyUSB0`。串口波特率 **2000000**。

> ⚠️ 脚本运行时会**独占串口**，请先关闭正在看串口的工具（picocom / SecureCRT 等），否则脚本打不开串口。

### 3. 查 PC 的 IP（吞吐/稳定性测试需要）
- **Windows**：`ipconfig`，找无线/有线网卡的 IPv4，如 `192.168.5.200`。
- **Linux**：`ip addr` 或 `ifconfig`，找与路由器同网段的地址。

### 4. 软件依赖（重要）

**(1) Python + pyserial**
```
pip install pyserial
```

**(2) iperf —— 必须是 2.x 版本！**
固件用的是 **iperf2 协议**，PC 端**不能用 iperf3**（协议不兼容，连不上）。
- **Linux**：`sudo apt install iperf`（注意是 `iperf` 不是 `iperf3`）
- **Windows**：下载 iperf 2.x（如 iperf-2.x-win64），把 `iperf.exe` 放进 PATH，
  或解压后在文件夹里运行脚本（确保命令行能直接调用 `iperf`）。
- 验证：`iperf --version` 应显示 `iperf version 2.x`。

**(3) ping**：用系统自带 ping（Windows/Linux 都有，无需安装、无需 root）。

---

## 二、快速开始

### 交互式（推荐，照提示操作）
```
python wifi_test.py
```
依次输入：DUT 串口、PC 的 IP、路由器 SSID、**路由器密码**（开放网络直接回车），
然后选测试项。**每次选择测试项后会询问本次测试时长**（直接回车用默认值）；
选「全部」时会先统一询问吞吐各项时长和时延时长。

### 命令行（适合批量/脚本）
```
# 一次性跑完吞吐4项 + 时延 + 连接耗时（带密码路由器加 --password）
python wifi_test.py all -P /dev/ttyUSB0 --pc-ip 192.168.5.200 --password 12345678

# 单项（Windows 串口写 COM3）
python wifi_test.py tcp-up   -P COM3 --pc-ip 192.168.5.200 -t 60 -p 5005
python wifi_test.py tcp-down -P COM3 --pc-ip 192.168.5.200 -t 60
python wifi_test.py udp-up   -P COM3 --pc-ip 192.168.5.200 -t 60 -b 50M
python wifi_test.py udp-down -P COM3 --pc-ip 192.168.5.200 -t 60 -b 50M
python wifi_test.py latency  -P COM3 --duration 600 --interval 200 --size 300
python wifi_test.py conn     -P COM3
python wifi_test.py stable   -P COM3 --pc-ip 192.168.5.200 --hours 1

# 模组高负载时，用 -i 5 让 iperf 每 5 秒报告一次（间隔更稳）
python wifi_test.py tcp-up   -P COM3 --pc-ip 192.168.5.200 -t 60 -i 5
```

### 参数说明
| 参数 | 含义 | 默认 |
|------|------|------|
| `-P` / `--serial` | DUT 串口（COM3 或 /dev/ttyUSB0） | 必填 |
| `--pc-ip` | PC 的 IP（吞吐/稳定性需要） | - |
| `--ssid` | 路由器 SSID | `nx30` |
| `--password` | 路由器密码（开放网络不填） | 无 |
| `-t` / `--time` | 吞吐测试时长（秒） | 60 |
| `-p` / `--port` | iperf 端口 | 5005 |
| `-i` / `--report-interval` | iperf 报告打印间隔（秒） | 1 |
| `-b` / `--bw` | UDP 发送带宽 | 50M |
| `--duration` | 时延测试时长（秒） | 600 |
| `--interval` | 时延 ping 间隔（ms） | 200 |
| `--size` | 时延 ping 包大小（字节） | 300 |
| `--hours` | 稳定性测试时长（小时） | 1 |

> 所有日志自动保存到 `logs/` 目录，文件名带时间戳，可追溯每次测试的原始 iperf/ping 输出。

---

## 三、各测试项原理与手动操作

脚本已自动完成下列流程；若想手动验证，可参考。
> DUT 连接路由器命令：`wifi_sta_connect nx30`（脚本会自动连）。

### TCP/UDP 上行（DUT→PC）
- PC 做 server：`iperf -s -i 1`（UDP 加 `-u`）
- DUT 做 client：`iperf -c <PC_IP> -i 1 -t 60`（UDP 加 `-u -b 50M`）
- 吞吐从 **PC server** 端读取。

### TCP/UDP 下行（PC→DUT）
- DUT 做 server：`iperf -s -i 1`（UDP 加 `-u`）
- PC 做 client：`iperf -c <DUT_IP> -i 1 -t 60`（UDP 加 `-u -b 50M`）
- TCP 吞吐从 PC client 读；UDP 吞吐/丢包/抖动从 **DUT server** 端读。

### 时延（PC ping DUT）
- PC 执行 ping（脚本自动按 Windows/Linux 选参数）：
  - Linux：`ping -c <次数> -i 0.2 -s 300 <DUT_IP>`
  - Windows：`ping -n <次数> -l 300 <DUT_IP>`
- 脚本解析所有 RTT，算 **P50（中位数）/ P95 / IQR（四分位距）/ 丢包率**。

### 连接耗时
- DUT 先 `wifi_sta_disconnect`，再 `wifi_sta_connect nx30`。
- 计时从发起连接到串口出现 `CODE_WIFI_ON_GOT_IP`（拿到 IP）。

### 连接稳定性
- DUT 做 client 持续 `iperf -c <PC_IP> -t <总秒数>`，PC 做 server。
- 脚本每秒解析分段吞吐，记录**最低吞吐**；检测断连日志，记录**断连次数**。
- 每 60 秒打印一次进度。

---

## 四、结果解读

脚本会直接打印结论，例如：
```
TCP 上行吞吐：  吞吐 = 45.30 Mbps
UDP 下行吞吐：  吞吐 = 23.30 Mbps | 丢包率 = 0.0% | 抖动 = 0.15 ms
时延测试：      P50 = 2.28 ms | P95 = 27.60 ms | IQR = 2.69 ms ，丢包率 = 2.0%
连接耗时：      连接耗时 = 4.02 秒
连接稳定性：    断连次数 = 0 | 最低吞吐 = 12.50 Mbps
```

记录表建议：

| 测试项 | 结果 |
|--------|------|
| TCP 上行吞吐 | ___ Mbps |
| TCP 下行吞吐 | ___ Mbps |
| UDP 上行吞吐 | ___ Mbps |
| UDP 下行吞吐 | ___ Mbps / 丢包 ___% / 抖动 ___ ms |
| 时延 | P50 ___ / P95 ___ / IQR ___ ms / 丢包 ___% |
| 连接耗时 | ___ 秒 |
| 连接稳定性 | 断连 ___ 次 / 最低 ___ Mbps |

---

## 五、已知限制与注意事项

### 1. UDP 上行（udp-up）拿不到丢包率/抖动
原因：UDP 的丢包/抖动由**接收端**统计。上行时接收端是 PC，但当前固件的 iperf
client 不发送 UDP-FIN 结束包，PC server 无法生成接收报告。这是固件实现限制，
**不是脚本 bug**。脚本会明确提示此项不可得。
- 若必须测 UDP 上行丢包，可改测 **UDP 下行**（udp-down，丢包/抖动正常可得），
  或评估固件 iperf 是否支持发送 UDP-FIN。

### 2. 打流时模组 CPU 负载高，`-i` 报告间隔可能不准
模组在满速打流时 CPU 负载高，iperf 的 `-i` 周期报告**实际间隔可能不是设定值**
（例如设 1 秒，实际某段是 0.5 秒或 1.8 秒）。影响：
- **总吞吐不受影响**（看的是 `0.0-总时长` 的汇总行，按总传输量算）。
- **稳定性测试的“最低吞吐”可能虚低**：某个被压短的分段，瞬时吞吐看起来很低，
  但不代表链路真的差。
- **应对**：
  - 脚本已自动**过滤掉时长明显偏短的分段**（< 设定间隔的 60%），并额外给出
    **吞吐中位数**，请结合中位数判断稳定性，不要只看最低值。
  - 可用 `-i 5`（5 秒报告一次）减小间隔抖动的相对影响，长时间测试推荐。

---

## 六、常见问题

**Q：脚本报“打开串口失败”？**
A：串口被别的程序占用了。关掉正在看串口的工具（picocom/SecureCRT/串口助手）再跑。

**Q：iperf 连不上 / PC server 没数据？**
A：(1) 确认 PC 装的是 **iperf 2.x** 不是 iperf3；(2) 确认 PC 与 DUT 同网段、能互 ping；
(3) PC 防火墙可能挡了 5001 端口，临时关闭防火墙或放行该端口；(4) 换个端口 `-p 5002`。

**Q：吞吐很低（几 Mbps）？**
A：受距离、信号、干扰、PC 是否走 WiFi 抢空口影响。请按需求在**屏蔽房**、PC**有线**接入下测。
脚本打印的是真实测量值。

**Q：时延测试在 Linux 下提示权限？**
A：本脚本用系统 `ping`（不是 pythonping），普通用户即可，无需 sudo。
若 Windows 下 ping 参数不被识别，确认用的是系统自带 ping。

**Q：连接耗时一直失败？**
A：确认 SSID 正确（默认 `nx30`，用 `--ssid` 改）、路由器在范围内、无密码或固件已配好。

**Q：怎么改路由器？**
A：加 `--ssid 你的SSID`，带密码的加 `--password 你的密码`（交互式会提示输入）。
固件命令格式为 `wifi_sta_connect <ssid> [password]`。

---

## 七、DUT 命令速查

| 命令 | 作用 |
|------|------|
| `wifi_sta_connect nx30` | 连接开放路由器 nx30 |
| `wifi_sta_connect nx30 12345678` | 连接带密码路由器 |
| `wifi_sta_disconnect` | 断开连接 |
| `wifi_state` | 查看连接状态（Connected / channel / bssid 等） |
| `wifi_sta_dhcp` | 触发/查看 DHCP 获取的 IP |
| `iperf -c <IP> -i 1 -t 60` | 做 client（TCP），加 `-u -b 50M` 为 UDP |
| `iperf -s -i 1` | 做 server，加 `-u` 为 UDP |
| `ping <IP> -c <次数>` | DUT 侧 ping（本测试用 PC 侧 ping，此条备用） |
| `reboot` | 重启设备 |
