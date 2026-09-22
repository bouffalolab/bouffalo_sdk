# Wi-Fi/BLE 共存性能测试说明

本目录提供 Wi-Fi/BLE 共存性能测试的脚本和操作文档，覆盖三个测试项：

| 测试项 | 说明 | 对应角色 |
|--------|------|----------|
| **共存吞吐** | WiFi 连接 + coex + BLE 100% 扫描下测 WiFi 吞吐 | DUT(A) + 对端(B) |
| **共存独立性** | 分别在 BLE/WiFi 背景下触发对方，验证互不影响 | DUT(A) + 对端(B) |
| **共存下 ping 时延** | WiFi + BLE 扫描同时工作时的 ping 时延 | DUT(A) + 对端(B) |

> 1Mbps 低速 ping 是路由器限速场景，脚本层面和普通 ping 无区别，
> 由测试人员在路由器管理界面限速后直接用 `latency` 项测试即可。

---

## 一、准备工作

### 1. 硬件
- **两块 BL616CL 开发板**，都已烧录 `wfa` 测试固件。
- 两根 USB 线接到电脑。
- 路由器（PC 建议有线接入；DUT 走 WiFi）。

### 2. 确认串口号
- **Windows**：设备管理器 → 端口(COM)，如 `COM3`、`COM4`。
- **Linux**：`ls /dev/ttyUSB*`，如 `/dev/ttyUSB0`、`/dev/ttyUSB2`。

### 3. 串口参数
- 波特率 **2000000**，8N1。

### 4. 安装依赖
```
pip install pyserial
```
PC 端还需 iperf **2.x**（不能用 iperf3）和系统自带 ping。

> **术语**：「**板A**」= 被测板（DUT，同时跑 WiFi + BLE），「**板B**」= BLE 对端板。

---

## 二、方式一：用自动化脚本（推荐）

### 交互式
```
python coex_test.py
```
按提示输入串口号、SSID、密码、环境干扰标记，然后选测试项。
每次选择后会询问本次参数（时长、duty 等），直接回车用默认值。

### 命令行
```
# 共存吞吐（必填 --pc-ip）
python coex_test.py throughput -a /dev/ttyUSB0 -b /dev/ttyUSB2 \
    --pc-ip 192.168.5.200 --password 12345678 --environment "无干扰"

# 共存独立性
python coex_test.py independ -a /dev/ttyUSB0 -b /dev/ttyUSB2 \
    --password 12345678 --environment "轻度干扰"

# 共存下 ping 时延
python coex_test.py latency  -a /dev/ttyUSB0 -b /dev/ttyUSB2 \
    --pc-ip 192.168.5.200 --password 12345678 --duration 600 --environment "无干扰"
```

### 参数说明
| 参数 | 含义 | 默认 |
|------|------|------|
| `-a` | 板A(DUT) 串口 | 必填 |
| `-b` | 板B(BLE 对端) 串口 | 必填 |
| `--pc-ip` | PC 的 IP（吞吐/时延需要） | - |
| `--ssid` | 路由器 SSID | `nx30` |
| `--password` | 路由器密码，开放网络不填 | 无 |
| `--environment` | 环境干扰标记（无干扰/轻度/重度），**用于结果记录** | 无 |
| `-t` / `--time` | 吞吐测试时长（秒） | 30 |
| `-p` / `--port` | iperf 端口 | 5005 |
| `-i` / `--report-interval` | iperf 报告间隔秒 | 1 |
| `--coex-duty` | 共存 WiFi active 时间 ms（10-90） | 50 |
| `--scan-interval` | BLE 扫描 interval 毫秒 | 40 |
| `--scan-window` | BLE 扫描 window 毫秒（占空比 = window/interval） | 20 |
| `--throughput-scan-interval` | 吞吐测试 BLE 100% 扫描 interval/window 毫秒 | 10 |
| `--ble-cover-extra` | BLE 背景扫描额外覆盖时间秒 | 5 |
| `--independ-duration` | 独立性背景测试时长秒 | 20 |
| `--independ-warmup` | 独立性背景切换前预热秒 | 3 |
| `--ping-pass-ratio` | 独立性 ping 通过收包比例 | 0.8 |
| `--duration` | 时延测试时长（秒） | 60 |
| `--interval` | 时延 ping 间隔（ms） | 200 |
| `--size` | 时延 ping 包大小（B） | 300 |

---

## 三、方式二：手动敲命令

### 共存相关命令速查
| 命令 | 作用 |
|------|------|
| `wifi_coex_start` | 显式开启 PS-PTA 共存（需先连 WiFi） |
| `wifi_coex_stop` | 关闭共存 |
| `wifi_coex_duty_set <10-90>` | 设置 WiFi active 时间（ms） |
| `wifi_coex_status` | 查看 activation、runtime、band 和 duty |

### 关键前置：每次连 WiFi 后都要 coex_enable
```
wifi_sta_connect nx30 12345678
[看到 CODE_WIFI_ON_GOT_IP 后]
wifi_coex_start
wifi_coex_duty_set 50
wifi_coex_status   # 应看到 active=1 runtime=ps_pta
```

### 测试1：共存吞吐
1. 板A 连 WiFi + coex_enable（见上）
2. 板B：`ble_init` → `ble_enable` → `ble_test_tx 10000`（持续发包覆盖 iperf）
3. 板A：`ble_init` → `ble_enable` → `ble_start_scan 1 0 0010 0010`（100% 占空比）
4. PC：`iperf -s -i 1 -p 5005`
5. 板A：`iperf -c <PC_IP> -i 1 -t 30 -p 5005`
6. 记录 Mbps（结果在 PC server 输出的汇总行）
7. 清理：`ble_stop_scan` → `wifi_coex_stop`

### 测试2：共存独立性

**场景 A：ping 过程中开 BLE 扫描**
1. 板A 连 WiFi + coex_enable
2. PC 后台启动 ping：`ping <DUT_IP> -i 0.2 -s 300`（持续 20 秒以上）
3. ping 跑几秒后，板A 开 BLE 扫描：`ble_start_scan 1 0 0028 0014`
4. 板B 同时发包：`ble_test_tx 1000`
5. 等 ping 结束 → 检查收包率（应 ≥ 80%）、BLE 扫描启动成功

**场景 B：BLE 扫描过程中连 WiFi + coex**
1. 板A 先开 BLE 扫描：`ble_start_scan 1 0 0028 0014`
2. 板B 发包：`ble_test_tx 1000`
3. BLE 扫描跑几秒后，板A 连 WiFi + `wifi_coex_start`
4. 验证 BLE 扫描没中断（仍有 DEVICE 事件）
5. 验证 WiFi 连通（PC ping 板A）

### 测试3：共存下 ping 时延
1. 板A 连 WiFi + coex_enable
2. 板B：`ble_test_tx 10000`（持续发包）
3. 板A：`ble_start_scan 1 0 0028 0014`（50% 占空比）
4. PC：`ping <DUT_IP> -i 0.2 -s 300`（600 秒）
5. 记录 P50/P95/IQR/丢包率

---

## 四、测试流程核心原则

1. **每次连 WiFi 后必须 coex_enable**——`wifi_connect_with_coex` 函数已封装
2. **BLE 扫描时长必须覆盖完整 iperf/ping 测试**——脚本用 `ble_cover_extra` 额外多给几秒
3. **独立性测试：背景任务先稳定跑，再触发前台操作**——用 `independ_warmup` 预热
4. **BLE 100% 扫描用于吞吐测试**（最大化干扰），50% 占空比用于延迟测试（模拟真实场景）

---

## 五、结果汇总表

| 测试项 | 环境 | 结果 |
|--------|------|------|
| 共存吞吐 | 无干扰 | ___ Mbps |
| 共存吞吐 | 轻度干扰 | ___ Mbps |
| 共存吞吐 | 重度干扰 | ___ Mbps |
| 共存独立性 场景A | ___ | 通过/失败 |
| 共存独立性 场景B | ___ | 通过/失败 |
| 共存下 ping P50 | 无干扰 | ___ ms |
| 共存下 ping P50 | 轻度干扰 | ___ ms |
| 共存下 ping P95 | 重度干扰 | ___ ms |

---

## 六、常见问题

**Q：共存开不起来？**
A：必须先 `wifi_sta_connect` 连上 WiFi，再执行
   `wifi_coex_start`。用 `wifi_coex_status` 确认
   `active=1 runtime=ps_pta`。

**Q：吞吐测试中 BLE 扫描占用大量射频时隙，吞吐下降很明显？**
A：这是真实共存表现。`--coex-duty` 调大（WiFi 拿更多时隙）吞吐回升；
   `--scan-window` 调小（BLE 占空比降低）WiFi 折损减小。

**Q：BLE 扫描和板B 的发包怎么协调？**
A：脚本里板B 用 `ble_test_tx` 持续发包（按 25ms 发送节奏估算总包数）；
   板A 用 `ble_start_scan` 开扫描（不阻塞）。扫描时长 = 测试时长 + 额外覆盖。

**Q：环境干扰怎么标？**
A：测试人员按实际判断。常见经验：
- 无干扰：屏蔽房 / 深夜闲置的住宅
- 轻度：白天正常办公环境
- 重度：办公室密集设备、WiFi 满载

---

## 七、与 BLE/WiFi 测试的关系

| 复用 | 来源 |
|------|------|
| Board 串口类 | `测试/WiFi测试/wifi_test.py` 改名 |
| iperf/ping 解析 | `测试/WiFi测试/wifi_test.py` |
| BLE 命令 | `测试/BLE测试/` 同名 |
| 互不重复实现 | - |

本脚本是**自包含**的（不跨目录 import），可单独拷走使用。
