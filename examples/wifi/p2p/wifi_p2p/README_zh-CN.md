# Wi-Fi Direct (P2P) 示例

## 功能概述

本示例提供 Wi-Fi Direct（P2P）Shell 命令，可用于设备发现、连接、组管理和数据传输测试。主要功能包括：

- 使用 `wl2` 接口完成 P2P 初始化、发现、监听和组网。
- 支持通过协商成为 Group Owner（GO）或 Group Client（GC）。
- 支持 PBC、由模组生成 PIN、使用固定 PIN 和输入对端 PIN。
- 支持自主 GO、持久组、配置方法发现（Provision Discovery）和组邀请（Invitation）。
- 未显式配置 `p2p_ssid_postfix` 时，自主 GO 的 SSID 会包含已设置的 P2P 设备名。
- 通过 P2P 事件报告设备发现、角色协商、组创建、组删除和获取 IP 等结果。
- GO 自动配置 `192.168.49.1/24` 并启动 DHCP Server；GC 自动启动 DHCP Client。
- GO 或 GC 组网成功时自动执行 Keyram dump，便于测试期间分析加密数据帧。
- 提供 TCP 和 iperf 命令用于组网后的数据面验证。

执行发现、监听、连接或组管理命令前，需要先执行 `p2p_init`。`p2p_join` 是例外：它会自动完成初始化、发现、选择对端、自动连接和等待连接完成。通过 `peer` 或 `name` 指定目标时，只要目标出现在已发现设备缓存中，命令就会立即停止发现。该命令使用 `P2P_CONNECT ... pbc auto`：对端已经是 GO 时模组作为 GC 加入；对端未运行 GO 时自动进行 GO Negotiation，最终角色可能是 GO 或 GC。

## 支持平台

| CHIP | BOARD | 说明 |
| --- | --- | --- |
| BL616 | `bl616dk` | 已验证构建 |
| BL618DG | `bl618dgdk` | AP 核，启用 PSRAM，已验证构建和自主 GO 启动 |

## 编译

以下命令均在本 README 所在的示例目录执行。

编译 BL616：

```bash
make CHIP=bl616 BOARD=bl616dk
```

编译 BL618DG AP 核：

```bash
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap
```

切换 CHIP、BOARD 或 CPU_ID 后，建议先清理原有构建结果：

```bash
make clean
```

## 烧录

SDK 在配置阶段自动生成构建目录中的烧录配置，烧录目标使用该生成文件，
无需在示例根目录维护 INI 文件。

将 `/dev/ttyUSB0` 替换为实际串口。

BL616：

```bash
make flash CHIP=bl616 BOARD=bl616dk COMX=/dev/ttyUSB0 BAUDRATE=2000000
```

BL618DG AP 核：

```bash
make flash CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap COMX=/dev/ttyUSB0 BAUDRATE=2000000
```

烧录并复位后，等待串口出现以下事件再执行 P2P 命令：

```text
CODE_WIFI_ON_INIT_DONE
CODE_WIFI_ON_MGMR_DONE
```

## 使用约定

- `channel` 使用信道号，例如 `6` 或 `36`；`freq` 使用 MHz，例如 `2437` 或 `5180`。
- 命令中的 MAC 参数是 P2P Device Address，格式为 `xx:xx:xx:xx:xx:xx`。它用于发现和连接，不一定等于组建立后的 P2P Interface Address。
- CLI 返回 `0` 仅表示 supplicant 已受理请求，不表示异步组网已经成功。
- 最终结果应结合 `[APP] [EVT] [P2P]` 事件和 `p2p_status` 判断。
- `p2p_init` 不指定信道时优先复用已连接 STA 的信道，否则使用信道 1。
- 使用 5 GHz 前应先确认国家码、对端能力和监管限制。DFS/NO_IR 信道即使出现在国家信道列表中，也可能不允许直接发起 P2P，例如信道 52。
- 固件内置帮助是当前代码的命令参数权威来源：

```text
p2p_help
p2p_help p2p_connect
```

## 快速开始

以下流程以信道 6 和 PBC 为例。

### 1. 初始化并发现对端

```text
p2p_init 6
p2p_name bl-p2p-demo
p2p_find 15 freq=2437
p2p_peers discovered
p2p_peer <peer-device-address>
```

`p2p_find` 的第一个参数是发现超时时间，单位为秒。`freq` 限制发现频率，`dev_id` 指定要发现的 P2P Device Address。例如：

```text
p2p_find 30 freq=5180 dev_id=02:11:22:33:44:55
```

`p2p_peers` 显示全部缓存设备；增加 `discovered` 参数后，只显示通过 P2P 发现流程获得的设备。

### 2. 由模组发起角色协商

让手机进入 Wi-Fi Direct 可发现状态，取得手机的 P2P Device Address 后执行：

```text
p2p_connect <peer-device-address> pbc
```

不指定 `join` 时，双方通过 GO 角色协商决定各自角色。需要影响角色选择时可设置 `go_intent=0..15`，值越大越倾向成为 GO：

```text
p2p_connect <peer-device-address> pbc go_intent=7 freq=2437
```

等待 `GO_NEG_SUCCESS`、`GROUP_FORMATION_SUCCESS` 和 `GROUP_STARTED`。模组作为 GC 时还应等待 `GOT_IP`。最后检查：

```text
p2p_status
```

成功状态通常包含：

```text
p2p_state=GROUP_ACTIVE
wpa_state=COMPLETED
mode=P2P GO
```

或：

```text
p2p_state=GROUP_ACTIVE
wpa_state=COMPLETED
mode=P2P CLIENT
```

### 3. 由手机发起连接

模组进入监听状态：

```text
p2p_init 6
p2p_listen 120 6
```

手机发起连接后，模组会报告 `GO_NEG_REQUEST`，其中包含对端地址、GO intent 和 WPS method。示例不会自动接受连接；应用确认允许该设备连接后，应使用与请求一致的 WPS 方法授权。例如，PBC 请求使用：

```text
p2p_connect <peer-device-address> pbc auth
```

如不接受请求，则不要执行授权。需要取消正在进行的协商时执行：

```text
p2p_cancel
```

### 4. 模组创建自主 GO

```text
p2p_init 36
p2p_name bl618-p2p
p2p_group_add freq=5180 ht40 vht he passphrase=12345678
```

新建自主 GO 时，`p2p_name` 设置的设备名会追加到标准随机前缀之后。上述命令会生成
类似 `DIRECT-QQ-bl618-p2p` 的 SSID。显式配置的 `p2p_ssid_postfix` 优先级更高；
使用 `persistent=<id>` 重新调用持久组时仍保留已保存的 SSID。

等待 `GROUP_STARTED` 后执行：

```text
p2p_status
p2p_wps_pbc
```

`p2p_wps_pbc` 会开启 PBC，手机需要在 PBC 有效期内加入。

`passphrase=` 可直接接收 8 到 63 个可打印字符。对于需要以十六进制安全传输的值，
仍可使用 `passphrase_hex=`。例如，`P9XKZRjI` 对应：

```text
p2p_group_add freq=2437 passphrase_hex=5039584b5a526a49
```

BL618DG AP（`bl618dgdk`）运行验证确认：明文命令可以正常解析，`GROUP_STARTED`
报告 GO 工作在 5180 MHz，`p2p_status` 返回 `ssid=DIRECT-QQ-bl618-p2p`、
`mode=P2P GO` 和 `wpa_state=COMPLETED`。

创建成功后模组 GO 地址固定为 `192.168.49.1/24`，DHCP 地址池为 `192.168.49.2` 到 `192.168.49.100`。

### 5. 创建并重新调用持久组

持久组 profile 保存在 PSM 分区中。每次 `p2p_init` 重建 P2P 接口时，
supplicant 都会恢复这些 profile。先创建持久 GO，并等待 `GROUP_STARTED`：

```text
p2p_init 6
p2p_group_add persistent freq=2437
```

列出已保存的 profile，并记录 network ID：

```text
list_networks
```

删除当前活动组后，可按 ID 重新调用该 profile，不会生成新的凭据：

```text
p2p_group_remove
p2p_group_add persistent=<id> freq=2437
```

执行 `p2p_stop`、重建 P2P 接口或设备重启后，该 profile 仍然保留。设备重启后，
需要先执行 `p2p_init`，再执行 `list_networks` 或重新调用。

当本机是该持久组中保存的 GO 时，必须指定对端 P2P Device Address，并且该对端
必须已经进入 supplicant peer cache。发送 Invitation 前先发现对端：

```text
p2p_find 15 freq=2437 dev_id=<peer-device-address>
p2p_stop_find
p2p_invite persistent=<id> peer=<peer-device-address> freq=2437
```

当本机是持久组中保存的 GC、已保存的对端是 GO 时，可以省略 `peer`。持久组成功
启动后，`GROUP_STARTED` 事件会显示 `[PERSISTENT]`。标准
`P2P-INVITATION-RESULT` 只包含 `status` 和可选 BSSID，不包含 peer、频率或
network ID。

删除不再使用的 profile：

```text
remove_network <id>
```

删除结果会立即写入 PSM。擦除或替换 PSM 分区会清除所有持久组 profile。
该功能由 `CONFIG_WIFI_P2P_PERSISTENT_GROUP` 控制，并已在本示例 defconfig 中启用。

### 6. 自动选择角色连接对端

当对端已经是 GO 且正在开放 PBC 时：

```text
p2p_init 6
p2p_find 10 freq=2437
p2p_connect <go-device-address> pbc join freq=2437
p2p_status
```

也可以使用 `p2p_join` 一次完成上述操作。建议通过 `peer` 明确指定 GO，避免连接到其他已发现设备：

```text
p2p_join peer=<peer-device-address> freq=2437 go_intent=7 find=60 wait=35
```

`find` 和 `wait` 的单位均为秒，默认值分别为 60 秒和 35 秒。指定 `peer` 或 `name` 时，`p2p_join` 会在发现期间检查已发现设备缓存，匹配后立即发送 `P2P_STOP_FIND`；未指定这两类目标时，会等待完整发现周期后再选择设备。`freq` 和 `channel` 可任选一个；同时指定时，两者必须表示同一信道。对端必须在连接期间保持 WPS PBC 开启。如果对端未运行 GO，supplicant 会产生 `P2P-FALLBACK-TO-GO-NEG` 事件并自动开始 GO Negotiation。

`go_intent` 为可选参数，取值范围为 0 到 15。它只在自动连接回退到 GO Negotiation 后生效；对端已经是 GO、模组直接加入时不生效。省略该参数时使用 supplicant 的配置值，当前默认值为 7。数值越大，模组成为 GO 的倾向越强；取值 15 表示模组必须成为 GO，如果双方都设置为 15，协商将失败。

指定 `peer` 时直接选择该地址，`name` 不再参与选择。只指定 `name=<substring>` 时，命令选择第一个名称匹配项；同时省略 `peer` 和 `name` 时，命令选择发现列表中的第一个设备。设备名称可能重复，且列表中的第一个设备不一定是目标 GO，因此优先使用 `peer=<go-device-address>`。`no_find` 会跳过发现，仅适用于对端信息已经在缓存中的情况。

## PIN 连接

### 由模组生成 PIN，对端输入

模组生成并打印 PIN，在对端输入该 PIN：

```text
p2p_connect <peer-device-address> pin display
```

命令成功受理后会打印：

```text
p2p: generated_pin=<PIN>
```

### 模组使用固定 PIN，对端输入

模组侧预先指定 PIN，并在对端输入：

```text
p2p_connect <peer-device-address> 12345670 display
```

### 对端显示 PIN，模组输入

对端显示 PIN 后，在模组侧输入：

```text
p2p_connect <peer-device-address> 12345670 keypad
```

`display` 表示 PIN 由模组提供、在对端输入；`keypad` 表示 PIN 由对端提供、在模组命令中输入。PIN 必须为 4 位或 8 位数字。是否接受 4 位 PIN 还取决于对端实现；互操作测试优先使用有效的 8 位 WPS PIN。

## 命令参考

| 命令 | 参数 | 作用 |
| --- | --- | --- |
| `p2p_help` | `[command]` | 显示全部命令或指定命令的详细帮助。 |
| `p2p_init` | `[channel]` | 初始化 `wl2` P2P 接口；省略信道时复用活动 STA 信道，否则使用信道 1。 |
| `p2p_channel` | `<channel>` | 同时修改工作信道、监听信道和首选信道；不能在协商或活动组期间执行。 |
| `p2p_config` | `<channel> [no_group_iface]` | 更新已初始化 P2P 的信道配置；当前 `no_group_iface` 必须为 `1`。 |
| `p2p_name` | `<device-name>` | 设置 1 到 32 字节的 P2P/WPS 设备名称；未显式配置 SSID 后缀时，新建自主 GO 的 SSID 会包含该名称。 |
| `p2p_find` | `[timeout] [freq=<MHz>] [dev_id=<MAC>]` | 启动发现，可限制时长、频率和目标设备。 |
| `p2p_stop_find` | 无 | 停止当前发现或监听，保留 P2P 接口和设备缓存。 |
| `p2p_listen` | `[seconds] [channel]` | 在指定信道监听连接请求；默认监听 30 秒。 |
| `p2p_peers` | `[discovered]` | 列出缓存的 P2P Device Address；`discovered` 仅显示通过 P2P 发现流程获得的设备。 |
| `p2p_flush` | 无 | 停止发现并清除设备及 Provision Discovery 缓存，不删除活动组。 |
| `p2p_peer` | `<peer-mac>` | 显示一个缓存对端的解析信息。 |
| `p2p_peer_raw` | `<peer-mac>` | 输出 supplicant 返回的原始 `P2P_PEER` 信息，用于诊断。 |
| `p2p_connect` | `<peer-mac> {pbc \| pin [display] \| <PIN> [display\|keypad]} [join\|auto\|auth] [go_intent=<0..15>] [freq=<MHz>]` | 发起角色协商、加入现有组或授权对端请求。`auto` 会在对端为 GO 时加入，否则回退到角色协商。`join`、`auto` 与 `auth` 互斥。 |
| `p2p_cancel` | 无 | 取消正在进行的角色协商、加入或邀请操作，不删除活动组。 |
| `p2p_group_add` | `[persistent\|persistent=<id>] [freq=<MHz>] [max_oper_chwidth=<20\|40\|80>] [ht40] [vht] [he] [passphrase=<8..63 chars>] [passphrase_hex=<16..126 hex chars>]` | 创建自主或持久 GO，或按 ID 重新调用已保存的 profile；明文或解码后的 passphrase 必须为 8 到 63 个可打印字符。 |
| `list_networks` | 无 | 列出已保存的持久组 profile 及 network ID；执行 `p2p_init` 后可看到从 PSM 恢复的 profile。 |
| `remove_network` | `<id>` | 删除一个已保存的持久组 profile，并立即更新 PSM。 |
| `p2p_group_remove` | 无 | 删除活动 `wl2` 组，保留已初始化的 P2P 接口。 |
| `p2p_wps_pbc` | `[peer-mac]` | 在本地 GO 上打开 WPS PBC，可限制指定对端。 |
| `p2p_prov_disc` | `<peer-mac> [pbc\|pin\|display\|keypad] [join\|auto]` | 发起配置方法发现（Provision Discovery）；`join` 与 `auto` 互斥。 |
| `p2p_invite` | `group=wl2 peer=<MAC> [go_dev_addr=<MAC>]` | 邀请已缓存的对端加入当前活动组。 |
| `p2p_invite` | `persistent=<id> [peer=<MAC>] [freq=<MHz>] [pref=<MHz>] [max_oper_chwidth=<20\|40\|80>] [ht40] [vht] [he]` | 重新调用已保存的持久组。本机为持久 GO 时必须指定 `peer`，已保存对端为 GO 时可省略；指定的 peer 必须已进入缓存。不受硬件支持的信道能力参数会被 supplicant 拒绝。 |
| `p2p_join` | `[peer=<MAC>\|<MAC>] [name=<text>] [freq=<MHz>] [channel=<channel>] [go_intent=<0..15>] [find=<seconds>] [wait=<seconds>] [no_find]` | 通过 PBC 自动加入现有 GO，或与非 GO 对端协商 GO/GC 角色；发现指定的 `peer` 或 `name` 后立即停止发现；`go_intent` 仅控制角色协商期间的本地角色倾向。 |
| `p2p_stop` | 无 | 完全停止 P2P，删除活动组、清除缓存并释放 `wl2` 接口。 |
| `p2p_status` | 无 | 显示本地 P2P 状态和 supplicant 连接状态。 |

## 生命周期和停止命令

P2P 生命周期的正常状态变化为：

```text
STOPPED -> IDLE -> FINDING/LISTENING -> NEGOTIATING -> GROUP_ACTIVE
```

不同停止命令的范围不同：

| 命令 | 停止发现/监听 | 清除设备缓存 | 删除活动组 | 释放 `wl2` 接口 |
| --- | --- | --- | --- | --- |
| `p2p_stop_find` | 是 | 否 | 否 | 否 |
| `p2p_flush` | 是 | 是 | 否 | 否 |
| `p2p_cancel` | 否 | 否 | 否 | 否 |
| `p2p_group_remove` | 否 | 否 | 是 | 否 |
| `p2p_stop` | 是 | 是 | 是 | 是 |

## 数据面验证

组网后先确认 `p2p_status` 显示 `GROUP_ACTIVE` 和 `COMPLETED`。
本示例使用偏保守的 lwIP 配置，用于协议和连通性验证。若要测 TCP/UDP 峰值吞吐，请改用
`examples/wifi/p2p/wifi_p2p_high_perf`：应用代码与本示例相同，TCP/lwIP 配置对齐
`wifi_tcp_high_perf`。

模组运行 TCP echo server：

```text
wifi_tcp_echo_test 3365
```

对端连接模组的 P2P IP 和端口 `3365`，发送数据并检查 echo。模组作为 GO 时 IP 为 `192.168.49.1`；模组作为 GC 时，应从 `GOT_IP` 日志或对端 DHCP lease 获取其地址。

模组作为 TCP client：

```text
wifi_tcp_test <peer-ip> 3365
```

iperf 参数可先通过以下命令查看：

```text
iperf -h
```

UDP server 示例：

```text
iperf -s -u -i 1
```

UDP client 示例：

```text
iperf -c <peer-ip> -u -b 50M -t 30 -i 1
```

## 事件和日志

重点关注以下事件：

| 事件 | 含义 |
| --- | --- |
| `DEVICE_FOUND` / `DEVICE_LOST` | 对端进入或离开发现缓存。 |
| `FIND_STOPPED` | 发现或监听操作停止，包括自然超时。 |
| `GO_NEG_REQUEST` | 收到对端协商请求；应用需要决定是否显式授权。 |
| `GO_NEG_SUCCESS` / `GO_NEG_FAILURE` | GO Negotiation 结果。 |
| `GROUP_FORMATION_SUCCESS` / `GROUP_FORMATION_FAILURE` | Group Formation 结果。 |
| `GROUP_STARTED` | 组已建立，日志包含接口、角色、SSID 和频率等信息。 |
| `GOT_IP` | GC DHCP 成功。 |
| `GROUP_REMOVED` | 组已删除，生命周期应回到 `IDLE`。 |
| `PROV_DISC_*` | Provision Discovery 的 PIN、PBC 或失败事件。 |
| `INVITATION_*` | Invitation 接收、接受或结果事件。 |

组建立时的 Keyram dump 包含会话密钥，仅用于受控测试和问题分析。不要在公开日志或缺少访问控制的环境中保存、传输这些内容。

## 常见问题

### `p2p_init` 返回 `not supported`

先确认已经收到 `CODE_WIFI_ON_MGMR_DONE`。然后检查指定信道是否同时满足国家码、硬件能力、对端能力以及 P2P 发起限制。DFS/NO_IR 信道不能仅根据国家信道列表判断可用性；日常互操作测试优先使用 1、6、11、36、40、44 或 48 等非 DFS 信道。

### `p2p_find` 找不到手机

- 确认手机 Wi-Fi Direct 页面保持在可发现状态。
- 确认双方信道/频段一致，必要时用 `freq=<MHz>` 限制扫描。
- 执行 `p2p_stop_find` 后重新发现；缓存异常时执行 `p2p_flush`。
- 使用 `p2p_peers` 和 `p2p_peers discovered` 对比主动发现与被动学习的条目。

### `p2p_connect` 返回 0，但没有建立组

返回 0 只表示命令已被 supplicant 接收。继续观察 `GO_NEG_*`、`GROUP_FORMATION_*` 和 `GROUP_STARTED`；使用 `p2p_status` 检查本地 P2P 状态和 `wpa_state`。对端未在 WPS 窗口内确认、PIN 方法不匹配或频率不一致都会导致异步失败。

### 状态停留在 `NEGOTIATING`

先执行 `p2p_status` 对比本地 P2P 状态与 supplicant 状态。若 supplicant 未完成组网，执行 `p2p_cancel` 取消当前协商；仍无法恢复时使用 `p2p_stop` 完整清理后重新初始化。未授权的 `GO_NEG_REQUEST` 和协商失败应使状态自动恢复到 `IDLE`；如未恢复，请保留完整串口事件日志。

### 手机主动连接没有自动继续

这是预期策略。示例只上报 `GO_NEG_REQUEST`，不会自动授权未知对端。上层确认用户授权后，调用带 `auth` 的 `p2p_connect`；PBC、display 或 keypad 方法必须与事件中的 WPS method 一致。

### STA 与 P2P 并发时信道冲突

单射频并发场景优先复用活动 STA 信道。省略 `p2p_init` 的 `channel` 和 `p2p_connect` 的 `freq` 时，示例会自动尝试复用 STA 信道；显式指定其他信道可能导致信道驻留失败或组网失败。

## 清理

仅删除活动组并保留 P2P 接口：

```text
p2p_group_remove
```

完整停止并释放 P2P 资源：

```text
p2p_stop
```
