# BL616CL 测试工具说明

统一测试工具 `test_tool.py`，合并自原 `wifi_test.py` / `ble_test.py` / `coex_test.py`
三个脚本。被测对象是运行 **wfa 固件**的 BL616CL 开发板，WiFi / BLE / 共存命令都在
同一固件里，故合并为一个工具，共享串口封装、iperf/ping 解析、日志与结果表格。

- **WiFi 测试**：1 块板(板A/DUT) + 1 台 PC（PC 与板A 同路由器，建议 PC 有线接入）
- **BLE 测试**：板A(DUT) + 板B(对端)，可选先让板A 连 WiFi（测试要求空载连接）
- **共存测试**：板A(DUT，同时 WiFi+BLE) + 板B(BLE 对端) + PC

> 术语：**板A** = 被测板(DUT)，**板B** = 配合对端板。

---

## 一、准备工作

### 1. 硬件
- BL616CL 开发板，烧好 `wfa` 测试固件，USB 接电脑。
- WiFi/共存吞吐：PC 与板A 连同一路由器（板A 走 WiFi，PC 建议**有线**）。
- BLE/共存：需要第二块板(板B) 也烧 `wfa` 固件。
- 测试条件（需求）：屏蔽房、Wi-Fi 6 路由器、2.4GHz 20MHz。

### 2. 串口
- 波特率 **2000000**，8N1。
- **Windows**：设备管理器 → 端口(COM)，如 `COM3`、`COM4`。
- **Linux**：`ls /dev/ttyUSB*`，如 `/dev/ttyUSB0`、`/dev/ttyUSB2`。

> ⚠️ 脚本运行时会**独占串口**，请先关闭 picocom / SecureCRT 等串口工具。

### 3. PC 的 IP（吞吐/共存吞吐需要）
- Windows：`ipconfig`；Linux：`ip addr`，取与路由器同网段的地址，如 `192.168.5.200`。

### 4. 依赖
```
pip install pyserial
```
- **iperf 必须是 2.0.9**（与固件版本严格一致，详见§七）
  - ⚠️ **不能用 2.1.x**：PC 端 iperf 2.1.x 收固件 UDP 流会段错误崩溃（UDP 上行丢包拿不到），必须降到 **2.0.9**
  - apt 装的通常是 2.1.x，需源码编译 2.0.9：`wget https://downloads.sourceforge.net/project/iperf2/iperf-2.0.9.tar.gz && tar xf iperf-2.0.9.tar.gz && cd iperf-2.0.9 && ./configure && make && sudo make install`
  - 验证：`iperf --version` 应显示 `iperf version 2.0.9`
- **ping**：系统自带，普通用户即可（Linux `-i` 最小 0.2s，故 ping 间隔默认 200ms）。

---

## 二、快速开始

### 交互式（推荐，照提示操作）
```
python test_tool.py
```
依次输入板A 串口、PC 的 IP、路由器 SSID、密码、环境干扰标记，然后选测试项。
选 BLE/共存项时会提示输入板B 串口；每次单项前会询问本次参数（时长/次数等），
直接回车用默认值。**退出时自动打印本次所有测试的汇总表格。**

### 命令行
子命令 `wifi` / `ble` / `coex`，公共参数可在子命令前或后。

```
# WiFi（只需板A）
python test_tool.py wifi all      -a /dev/ttyUSB0 --pc-ip 192.168.5.200 --password 12345678
python test_tool.py wifi tcp-up   -a COM3 --pc-ip 192.168.5.200 -t 60
python test_tool.py wifi udp-up   -a COM3 --pc-ip 192.168.5.200 -t 60 --bw 80M
python test_tool.py wifi latency  -a COM3 --ping-count 100 --interval 200
python test_tool.py wifi conn     -a COM3
python test_tool.py wifi stable   -a COM3 --pc-ip 192.168.5.200 --hours 1

# BLE（板A + 板B）
python test_tool.py ble rx     -a COM3 -b COM4 --count 100 --distance 1m --csv
python test_tool.py ble master -a COM3 -b COM4 --count 100
python test_tool.py ble slave  -a COM3 -b COM4 --count 100

# 共存（板A + 板B + PC）
python test_tool.py coex throughput -a /dev/ttyUSB0 -b /dev/ttyUSB2 --pc-ip 192.168.5.200 --password 12345678
python test_tool.py coex independ   -a /dev/ttyUSB0 -b /dev/ttyUSB2 --password 12345678
python test_tool.py coex latency    -a /dev/ttyUSB0 -b /dev/ttyUSB2 --password 12345678 --ping-count 100

# 模组高负载时，用 -i 5 让 iperf 每 5 秒报告一次（间隔更稳）
python test_tool.py wifi tcp-up -a COM3 --pc-ip 192.168.5.200 -t 60 -i 5
```

---

## 三、参数说明

| 参数 | 含义 | 默认 |
|------|------|------|
| `-a` | 板A(DUT) 串口 | 必填 |
| `-b` | 板B(对端) 串口，BLE/共存必填 | - |
| `--pc-ip` | PC 的 IP（吞吐/共存吞吐需要） | - |
| `--ssid` | 路由器 SSID | `nx30` |
| `--password` | 路由器密码（开放网络不填） | 无 |
| `--environment` | 环境干扰标记（无干扰/轻度/重度，用于记录） | 无 |
| `-t` / `--time` | 吞吐测试时长（秒） | 60 |
| `-p` / `--port` | iperf 端口 | 5005 |
| `-i` / `--report-interval` | iperf 报告间隔（秒） | 1 |
| `--bw` | UDP 发送带宽 | **80M** |
| `--ping-count` | ping 次数 | **100** |
| `--interval` | ping 间隔（ms） | **200** |
| `--size` | ping 包大小（字节） | 300 |
| `--hours` | 稳定性测试时长（小时） | 1 |
| `--count` | BLE 次数/包数 | 100 |
| `--distance` | BLE 距离标记（如 1m） | - |
| `--rx-interval` / `--rx-window` | BLE rx 扫描 interval/window（ms） | 40 / 20 |
| `--csv` | BLE 结果追加到 `测试结果.csv` | 关 |
| `--coex-duty` | 共存 WiFi active 时间（ms，10-90） | 50 |
| `--scan-interval` / `--scan-window` | BLE 扫描 interval/window（ms） | 40 / 20 |
| `--throughput-scan-interval` | 共存吞吐 BLE 100% 扫描 interval/window（ms） | 10 |
| `--ble-cover-extra` | BLE 背景扫描额外覆盖（秒） | 5 |
| `--independ-duration` | 独立性背景测试时长（秒） | 20 |
| `--independ-warmup` | 独立性背景切换前预热（秒） | 3 |
| `--ping-pass-ratio` | 独立性 ping 通过收包比例 | 0.6 |
| `--reboot` | 测试前重启板子清状态残留（ble_init 不完全复位，见§七） | 关 |

> 所有日志自动保存到 `logs/`（脚本所在目录下），文件名带时间戳；
> BLE 结果可用 `--csv` 追加到 `测试结果.csv`。

---

## 四、测试项一览

| 类别 | 测试项 | 测什么 | 关键结果 |
|------|--------|--------|----------|
| WiFi | `tcp-up` | DUT→PC TCP 吞吐 | Mbps |
| WiFi | `tcp-down` | PC→DUT TCP 吞吐 | Mbps |
| WiFi | `udp-up` | DUT→PC UDP 吞吐 | Mbps + 丢包率 + 抖动 |
| WiFi | `udp-down` | PC→DUT UDP 吞吐 | Mbps + 丢包率 + 抖动 |
| WiFi | `latency` | PC ping DUT | P50/P95/IQR + 丢包率 |
| WiFi | `conn` | 连接→拿到IP 耗时 | 秒 |
| WiFi | `stable` | 长时间 iperf 跑机 | 断连次数 + 最低/中位吞吐 |
| BLE | `rx` | 板B发 N 包，板A扫描统计 | 接收成功率 |
| BLE | `master` | 板A主动连板B N 次 | 连接成功率 |
| BLE | `slave` | 板A被板B连 N 次 | 连接成功率 |
| 共存 | `throughput` | coex+BLE 扫描下 TCP/UDP×上下行 4 方向 | Mbps + BLE扫描事件数 |
| 共存 | `independ` | WiFi/BLE 背景下触发对方 | 通过 x/2 |
| 共存 | `latency` | 共存下 ping 时延 | P50/P95/IQR + 丢包率 |

---

## 五、关键设计

### 1. PC 侧 iperf 实时打印
PC 侧 iperf（server 或 client）的输出经后台线程逐行**实时打印**（`[PC]` 前缀），
并写入日志。既便于观察（尤其 UDP 上行时 PC server 的分段丢包/抖动），也避免
iperf stdout 管道写满导致进程阻塞（长时间稳定性测试的关键修复）。

### 2. UDP 上行丢包/抖动（需 PC 端 iperf 2.0.9）
UDP 的丢包/抖动由**接收端**统计，上行时接收端是 PC server。脚本用后台线程
实时打印 PC 输出，结束时发 **SIGINT**（非 SIGTERM）触发 iperf2 server 报告
函数输出丢包/抖动（iperf2 平时不打印分段，靠 SIGINT 出汇总）。

**前提：PC 端 iperf 必须是 2.0.9**（与固件一致）。若用 2.1.x，server 收固件
UDP 流会 SIGSEGV 崩溃，丢包拿不到（脚本会检测到 server 异常退出并提示）。
装好 2.0.9 后 UDP 上行丢包/抖动正常可得（实测 72Mbps 丢包0% 抖动0.14ms）。

### 3. ping 次数驱动
`latency` 测试由 `--ping-count`（默认 100）和 `--interval`（默认 200ms）控制，
不再依赖时长推导。

### 4. 结果表格
单项测试结束打印结论；交互式退出 / 命令行批量结束时打印汇总表格（中文对齐）。

### 5. 共存吞吐 4 方向 + BLE 扫描事件数
`coex throughput` 复用 `test_throughput` 跑完整 TCP/UDP×上下行 4 方向，参数与
纯 WiFi 一致（`-t` / `--bw` 等）。通过 `ble_ctl` 控制器让 **BLE 扫描在打流期间
保持**（真共存），仅发 iperf 命令的瞬间暂停清 buffer（避免 `[DEVICE]` 干扰）。
每方向额外统计打流期间板A 扫描收到板B 的 `[DEVICE]` 事件数，反映共存对 BLE
的影响面（WiFi 打流对 BLE 扫描的抑制通常远大于反向）。

---

## 六、结果解读

单项结论示例：
```
UDP 下行吞吐：  吞吐 = 23.30 Mbps | 丢包 0.0% | 抖动 0.15 ms
时延测试：      P50 = 2.28 / P95 = 27.60 / IQR = 2.69 ms | 丢包 2.0%
连接耗时：      4.02 秒
```

汇总表格示例（退出时自动打印）：
```
WiFi 测试汇总
+----------+------------------------------------------+
| 测试项   | 结果                                     |
+----------+------------------------------------------+
| TCP 上行 | 45.30 Mbps                               |
| TCP 下行 | 42.10 Mbps                               |
| UDP 上行 | 72.00 Mbps | 丢包 0.0% | 抖动 0.14 ms     |
| UDP 下行 | 23.30 Mbps | 丢包 0.0% | 抖动 0.15 ms    |
| 时延     | P50 2.28 / P95 27.60 / IQR 2.69 ms | 丢包 2.0% |
| 连接耗时 | 4.02 秒                                  |
+----------+------------------------------------------+
```

---

## 七、已知限制

### 打流时模组 CPU 负载高，`-i` 报告间隔可能不准
满速打流时 iperf 的 `-i` 周期报告**实际间隔可能不是设定值**。影响：
- **总吞吐不受影响**（看 `0.0-总时长` 汇总行）。
- **稳定性测试的“最低吞吐”可能虚低**：被压短的分段瞬时吞吐看起来很低，但不代表
  链路真的差。脚本已自动过滤时长明显偏短的分段（< 设定间隔 60%），并额外给出
  **吞吐中位数**，请结合中位数判断稳定性。
- 可用 `-i 5`（5 秒报告一次）减小间隔抖动的相对影响，长时间测试推荐。

### PC 端 iperf 必须 2.0.9（不能用 2.1.x）
固件 iperf 版本是 **2.0.9**（`net_iperf_al.c` 顶部注释）。PC 端 iperf 也必须
是 **2.0.9**——若用 2.1.x（apt 默认），其 UDP server 收固件 UDP client 流时
会 SIGSEGV 段错误崩溃（UDP 上行丢包/抖动拿不到）。这是 iperf 协议版本不匹配
所致，**降级到 2.0.9 即解决**，无需改固件。TCP 不受影响。脚本检测到 server
异常退出会打印提示。

### BLE 测试：对端板卡死会让 ble_test_tx "空发"
持续发包的对端板（板B）可能进入异常状态：`ble_test_tx` 软件层报告 `sent=N
status=ok`，但**空口 0 包**（板 A 扫描/接收都收不到）。表现就是 rx 测试 0%。
`ble_start_adv`/`ble_test_conn_master` 等标准命令不受影响。排查：手动打开
板 B 串口看有无输出，无输出则 `reboot` 板 B 恢复。

### ble_init 不完全复位，连续测试需 reboot
`ble_init` 不能完全复位模组 BLE 状态：连续跑多项测试（尤其 `ble_test_tx` /
`ble_test_rx` 这类 DTM 命令后再跑 `master`）后，BLE 可能进入异常状态导致连接
失败或结果解析异常。脚本已内置两处 reboot 策略：
- **交互式**：切换测试大类（wifi↔ble↔coex）时**自动 reboot** 两板；
- **命令行**：加 `--reboot` 标志，单项测试前重启，如
  `python test_tool.py ble master --reboot -a ... -b ...`。

> 历史 bug（已修复）：`master` 测试在 count 较大（如 10）时曾报 `?/10`——
> `grep_result` 把串口分片读到一半的 RESULT 行（截断在 `success=` 之前）当
> 完整行匹配。现已要求 RESULT 行以换行结尾才算数，count=10 正确出 10/10。

---

## 八、固件命令速查（手动验证用）

| 命令 | 作用 |
|------|------|
| `wifi_sta_connect <ssid> [password]` | 连接路由器 |
| `wifi_sta_disconnect` | 断开连接 |
| `wifi_state` | 查看连接状态 |
| `wifi_sta_dhcp` | 触发/查看 DHCP 获取的 IP |
| `wifi_coex_start` / `wifi_coex_stop` | 显式开启/关闭 PS-PTA 共存 |
| `wifi_coex_duty_set <10-90>` | 设置 WiFi active 时间（ms） |
| `wifi_coex_status` | 查看 activation、runtime、band 和 duty |
| `iperf -c <IP> -i 1 -t 60` | 做 client（TCP），UDP 加 `-u --bw 80M` |
| `iperf -s -i 1` | 做 server，UDP 加 `-u` |
| `ble_init` / `ble_enable` | 初始化/使能 BLE |
| `ble_start_scan 1 0 <interval> <window>` | 启动扫描（0.625ms 单位，如 40ms=0x28） |
| `ble_stop_scan` | 停止扫描 |
| `ble_start_adv 0 0` / `ble_stop_adv` | 启动/停止广播 |
| `ble_test_tx <count>` | 发包测试 |
| `ble_test_rx <ms> <count> <interval> <window>` | 接收统计（参数 `strtoul base=0`：`0040` 当十进制 40，十六进制须写 `0x40`） |
| `ble_read_local_address` | 读本地地址 |
| `reboot` | 重启设备 |

---

## 九、与旧脚本的关系

本工具由 `WiFi/wifi_test.py`、`BLE/ble_test.py`、`coex/coex_test.py` 合并而来，
消除了三个脚本间重复的串口类、`iperf_summary`/`open_log`/`ping_command`/
`ms_to_units`/WiFi 连接逻辑等。原三个子目录脚本如不再需要可删除。
