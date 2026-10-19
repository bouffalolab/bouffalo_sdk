# BLE 性能测试说明

本目录提供 BLE 性能测试的脚本和操作文档，覆盖三个测试项：

| 测试项 | 说明 | 对应角色 |
|--------|------|----------|
| **包接收成功率** | 一块板发约定的广播包，另一块板扫描统计收到多少（即「扫描成功率」的量化版） | 收 / 发 |
| **连接成功率 (Master)** | 被测板做**主机**，反复主动连接对端 | 主机 |
| **连接成功率 (Slave)** | 被测板做**从机**，反复被对端连接 | 从机 |

每项都在 **1m / 5m / 10m** 三个距离各测一次，记录成功率。

---

## 一、准备工作

### 1. 硬件
- **两块 BL616CL 开发板**，都已烧录 `wfa` 测试固件。
- 两根 USB 线接到电脑。

### 2. 确认串口号
- **Windows**：设备管理器 → 端口(COM)，看到两个口，如 `COM3`、`COM4`。
- **Linux**：执行 `ls /dev/ttyUSB*`，如 `/dev/ttyUSB0`、`/dev/ttyUSB2`。

### 3. 串口参数
- **波特率：2000000**（200万），8N1。
- 串口工具任选：Windows 用 SecureCRT / Xshell / 串口助手；Linux 用 picocom / minicom。

### 4.（用脚本时）安装 Python 依赖
```
pip install pyserial
```

> **术语约定**：下文「**板A**」= 被测板(DUT)，「**板B**」= 配合用的对端板。

---

## 二、方式一：用自动化脚本（推荐）

脚本会自动初始化两块板、跑测试、给出成功率，并可记录到 CSV。

### 交互式（最简单，照提示操作）
```
python ble_test.py
```
然后按提示输入两个串口号。脚本会先询问**是否让板A(DUT)连接 WiFi 路由器**——
测试用例要求在「Wi-Fi 连接空载状态」下测试，正式测试请选 `y` 并输入
SSID/密码（开放网络密码直接回车）。之后选测试项、输入距离和次数即可。

### 命令行（适合批量）
```
# Windows 示例（--wifi-ssid 满足「Wi-Fi 连接空载」前提，开放网络可不加 --wifi-password）
python ble_test.py rx     -a COM3 -b COM4 --count 100 --distance 1m --csv --wifi-ssid nx30 --wifi-password 12345678
python ble_test.py master -a COM3 -b COM4 --count 100 --distance 1m --csv --wifi-ssid nx30 --wifi-password 12345678
python ble_test.py slave  -a COM3 -b COM4 --count 100 --distance 1m --csv --wifi-ssid nx30 --wifi-password 12345678

# Linux 示例
python3 ble_test.py rx -a /dev/ttyUSB0 -b /dev/ttyUSB2 --count 100 --distance 5m --csv --wifi-ssid nx30 --wifi-password 12345678
```

参数说明：
| 参数 | 含义 |
|------|------|
| `rx/master/slave` | 测试项：包接收 / 主机连接 / 从机连接 |
| `-a` | 板A(被测板) 串口 |
| `-b` | 板B(对端) 串口 |
| `--count` | 次数/包数，默认 100 |
| `--distance` | 距离标记（1m/5m/10m），仅用于记录 |
| `--csv` | 把结果追加到 `测试结果.csv` |
| `--wifi-ssid` | 测试前先让板A连接此 WiFi（测试要求 Wi-Fi 连接空载） |
| `--wifi-password` | WiFi 密码，开放网络不填 |
| `--scan-interval` | rx 扫描 interval（**毫秒**），默认 40 |
| `--scan-window` | rx 扫描 window（**毫秒**），默认 20（40/20 = **占空比 50%**） |

> **占空比说明**：测试用例要求「BLE 占空比设置不可高于 50%」。脚本默认
> interval=40ms / window=20ms 即 50%，直接满足要求；交互模式选包接收测试时
> 也会提示输入这两个值（十进制毫秒，脚本自动换算成 0.625ms 协议单位）。
> window 不能大于 interval。

> 加 `--csv` 后，结果会存到本目录的 `测试结果.csv`，可用 Excel 打开汇总。

---

## 三、方式二：手动敲命令（不装脚本也能测）

用串口工具分别打开两块板，按下面步骤敲命令。每条命令回车执行。

> **测试前提**：用例要求「Wi-Fi 连接空载状态」。先在**板A**输入
> `wifi_sta_connect <SSID> [密码]`（如 `wifi_sta_connect nx30 12345678`），
> 看到 `CODE_WIFI_ON_GOT_IP` 即连接成功，之后保持空载（不打流）再开始 BLE 测试。

### 测试1：包接收成功率

**板A（接收）依次输入：**
```
ble_init
ble_enable
ble_test_rx 6000 100
```
> `ble_test_rx <扫描时长ms> <期望包数> [interval] [window]`。上面是扫描 6 秒、
> 期望 100 个包。interval/window 可选，单位 0.625ms，**不填默认 0x40/0x20
> （40ms/20ms，占空比 50%，满足用例要求）**；如 `ble_test_rx 6000 100 0x20 0x10`
> 表示 20ms/10ms。window 不能大于 interval。
> **发完这条后，立刻去板B 发包**（6 秒内）。

**板B（发送）依次输入：**
```
ble_init
ble_enable
ble_test_tx 100
```
> `ble_test_tx <包数>`，按 500ms/20 包的节奏发 100 个包（约 2.5 秒发完）。

**看板A 输出的结果行：**
```
[BLE_TEST] RESULT type=rx status=ok total=97 unique=97 expect=100 rate=97
```
- `unique` = 收到的不重复包数
- `rate` = **接收成功率(%)** = unique / expect × 100

---

### 测试2：连接成功率 (Master) —— 被测板做主机

**板B（从机）依次输入：**
```
ble_init
ble_enable
ble_read_local_address      ← 记下打印的 public 地址，如 C8:E7:13:7E:0C:E8
ble_start_adv 0 0           ← 开可连接广播
```

**板A（主机）输入：**（地址去掉冒号）
```
ble_init
ble_enable
ble_test_conn_master 0 C8E7137E0CE8 100
```
> `ble_test_conn_master <地址类型> <对端MAC> <次数>`，地址类型 0=public。
> 板A 会自动连接→断开→再连，循环 100 次。**耗时几分钟，请耐心等**。

**看板A 输出的结果行：**
```
[BLE_TEST] RESULT type=conn_master status=ok attempt=100 success=98 target=100 rate=98
```
- `success` = 成功连接次数
- `rate` = **连接成功率(%)**

---

### 测试3：连接成功率 (Slave) —— 被测板做从机

**板A（从机=被测板）依次输入：**
```
ble_init
ble_enable
ble_read_local_address      ← 记下 public 地址
ble_start_adv 0 0           ← 先配好可连接广播
ble_test_conn_slave 100     ← 进入从机测试模式，目标被连 100 次
```

**板B（主机）输入：**（用板A 的地址，去冒号）
```
ble_init
ble_enable
ble_test_conn_master 0 <板A的MAC> 100
```
> 板B 反复连板A 100 次，**耗时几分钟**。

**板B 跑完后，回到板A 查看结果：**
```
ble_test_conn_stop
```
输出：
```
[BLE_TEST] RESULT type=conn_slave status=ok success=99 target=100 rate=99
```
- `success` = 被成功连接的次数
- `rate` = **从机连接成功率(%)**

> 测试中途也可在板A 敲 `ble_test_conn_stats` 查看实时进度。

---

## 四、距离测试与记录

每个测试项在 **1m / 5m / 10m** 各做一遍。建议用下表记录：

| 测试项 | 1m | 5m | 10m |
|--------|----|----|-----|
| 包接收成功率 | ___% | ___% | ___% |
| 连接成功率(Master) | ___% | ___% | ___% |
| 连接成功率(Slave) | ___% | ___% | ___% |

> 测试条件（来自需求）：家居无线环境、Wi-Fi 空载、BLE 占空比不高于 50%。
> 用脚本加 `--csv` 会自动生成 `测试结果.csv`，方便汇总。

---

## 五、命令速查

| 命令 | 作用 |
|------|------|
| `ble_init` | 初始化 BLE（每次上电后先执行） |
| `ble_enable` | 使能 BLE |
| `ble_read_local_address` | 读本机蓝牙地址 |
| `ble_start_adv 0 0` | 开可连接广播 |
| `ble_test_tx <包数>` | 发送端：发约定测试包 |
| `ble_test_rx <时长ms> <期望包数> [interval] [window]` | 接收端：扫描统计接收成功率（interval/window 单位 0.625ms，默认 0x40/0x20=50% 占空比） |
| `ble_test_conn_master <类型> <MAC> <次数>` | 主机连接成功率（类型 0=public,1=random） |
| `ble_test_conn_slave <次数>` | 从机连接成功率（需先 `ble_start_adv 0 0`） |
| `ble_test_conn_stats` | 查看连接测试实时统计 |
| `ble_test_conn_stop` | 结束从机测试并打印结果 |

---

## 六、结果怎么看

所有测试结果都是一行，格式统一：
```
[BLE_TEST] RESULT type=<测试项> status=ok ... rate=<成功率>
```
**`rate` 就是成功率百分比**，例如 `rate=98` 表示 98%。

---

## 七、常见问题

**Q：板子上电后敲命令没反应？**
A：先确认波特率是 2000000；再敲一次回车看是否有 `bouffalolab />` 提示符；命令前要先 `ble_init` 和 `ble_enable`。

**Q：包接收率偏低（如只有 50%）？**
A：正常受距离/遮挡/干扰影响。确认两板距离、Wi-Fi 空载、周围无强干扰。包接收率反映真实射频质量，不是 bug。

**Q：连接测试卡住很久？**
A：连接 100 次本身就要几分钟（每次连+断约 1~3 秒）。建议先用 `--count 10` 或手动 `... 10` 小批量验证流程，再跑 100 次。

**Q：`ble_test_conn_slave` 报 `status=error`？**
A：从机测试必须**先** `ble_start_adv 0 0` 配好可连接广播，再执行 `ble_test_conn_slave`。

**Q：脚本提示缺少 pyserial？**
A：执行 `pip install pyserial`（或 `pip3 install pyserial`）。

**Q：脚本里串口名怎么写？**
A：Windows 直接写 `COM3`；Linux 写完整路径 `/dev/ttyUSB0`。
