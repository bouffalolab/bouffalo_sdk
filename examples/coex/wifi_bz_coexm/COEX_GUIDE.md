# Wi-Fi/Bluetooth Coexistence Guide

本文介绍 BL616、BL616CL 和 BL618DG 的共存配置。编译、测速命令和 PSRAM 参考数据见
[README.md](README.md)。

## 1. 使用原则

| 功能 | 作用 | 使用方式 |
|---|---|---|
| 基础硬件共存 | 根据板级拓扑和 Wi-Fi 状态配置 PTA/SPDT | SDK 自动维护，不需要 start |
| PS-PTA | 利用 STA 省电机制分配软件时间窗口 | `wifi_coex_start` 开启，`wifi_coex_stop` 关闭 |
| Connection Protection | 在支持的 scan/connect/key/DHCP 阶段临时提高 Wi-Fi 优先级 | `wifi_coex_protection 1/0` 独立控制，默认关闭 |

`hardware_only` 表示未启用 PS-PTA，不表示关闭硬件 PTA。`wifi_coex_start` 不接收模式参数，
也不负责创建 Bluetooth/BLE 服务或连接。

## 2. 拓扑与适用场景

| 芯片与拓扑 | Wi-Fi 频段 | 自动基础配置 | 可选 PS-PTA |
|---|---|---|---|
| BL616 / BL616CL combo | 2.4 GHz | 硬件 PTA | 已连接的单 STA |
| BL618DG combo | 2.4 GHz | 硬件 PTA | 已连接的单 STA |
| BL618DG standalone 单天线/SPDT | 2.4 GHz | 动态 PTA/SPDT | 已连接的单 STA |
| BL618DG standalone 单天线/SPDT | 5 GHz | SPDT 固定 BT，5G channel-aware 配置 | 不使用 |
| BL618DG standalone 双天线 | 2.4 / 5 GHz | 并行配置，仅双天线验证构建 | 不使用 |

双天线配置需显式启用 `COEX_DUAL_ANT_VALIDATION=ON`，并在实际双天线硬件上完成集成验证；
默认构建不启用。BL618DG combo 的当前共存配置用于 2.4 GHz，不用于 5 GHz。

基础硬件共存适用于上述配置的 STA、AP，以及使用相同完整信道配置的 STA+AP；不同信道的
STA+AP 不支持。PS-PTA 只用于受支持的 2.4 GHz 单 STA，不用于 AP、STA+AP 或 5 GHz。
Wi-Fi 同一时刻只使用一个频段，不支持 2.4 GHz 与 5 GHz 双频同时工作。

### BL618DG 的三种硬件连接

| 拓扑 | RF 与天线连接 | 声明硬件 | 准备模式 |
|---|---|---|---|
| Combo | 内部 combo/2G path，无外部 SPDT | `wifi_coex_board_config combo` | `wifi_coex_init combo` |
| Standalone 单天线 | BZ 与 Wi-Fi 2.4G 经 SPDT，5G 经 diplexer，合用一根天线 | `wifi_coex_board_config standalone_single_ant <gpio>` | `wifi_coex_init standalone` |
| Standalone 双天线 | BZ 独立天线；Wi-Fi 使用另一根双频天线，无 SPDT | `wifi_coex_board_config standalone_dual_ant` | `wifi_coex_init standalone` |

按原理图选择拓扑，不需要提前选择 Wi-Fi band，也不能把 SPDT 单天线板配置成双天线。
硬件声明不写 GPIO、不校准 RF；模式准备根据声明校验能力，不支持的组合返回 `NOT_SUPPORTED`。
当前使用上表对应组合，不把单天线硬件声明成双天线，也不自动切换为其他 RF 模式。

SPDT 只使用一个 GPIO，**没有默认引脚**。
当前 BSP 要求偶数 GPIO 的高电平选 BT、奇数 GPIO 的低电平选 BT，必须与外部开关真值表及 RF
接线一致；其他接线需在 BSP 中适配。GPIO、pinmux 和极性属于板级配置。

## 3. 上电初始化

BL618DG 先声明硬件，再准备模式，两步均成功后启动业务。例如已确认 GPIO18 的单天线板：

```text
wifi_coex_board_config standalone_single_ant 18
wifi_coex_init standalone
wifi_bt_init
```

`wifi_coex_board_config show` 可分别查看硬件、允许模式、当前构建可用模式、已选模式和实际 RF path。

BL616/BL616CL 在上电时初始化 RF，直接执行 `wifi_bt_init`。

等待 `CODE_WIFI_ON_MGMR_DONE` 和 Bluetooth 初始化成功，再启动 Wi-Fi/Bluetooth 业务。
每次启动只初始化一次；改变拓扑需重启。RF 初始化失败后不要继续启动协议栈，应检查板级配置后重启。
`Run WifiMgmr init first...` 表示 Wi-Fi MGMR 尚未完成初始化。

单天线/SPDT 的自动切换如下，无需手动操作 GPIO 或寄存器：

| 阶段 | SPDT / 共存配置 |
|---|---|
| 板级 RF 准备完成，尚未启动协议栈 | 固定 BT 启动位置 |
| Wi-Fi 初始化完成、首次连接前 | 动态 PTA 默认配置（D） |
| 2.4 GHz STA 连接或 AP 启动 | 保持动态 PTA（D） |
| 5 GHz STA 连接或 AP 启动 | 固定 BT，Wi-Fi 5G 走 diplexer（F） |
| 扫描 | 临时使用 D，完成后恢复有效 home；继续连接时保留 D |
| 所有 Wi-Fi 连接/AP 均停止 | 回到默认 D |

若 STA 断开或连接失败，但仍有有效 AP，恢复 AP 所需配置，而不是一律回到 D。
F 不表示所有 Coex 寄存器清零，也不代表并发吞吐必然与单负载相同。

## 4. 典型场景

先完成第 3 节初始化，再选择一个场景。将 `<...>` 替换为实际参数；BSSID、信道必须与目标路由器
一致，信道也必须符合当地法规及板级配置。STA 等待连接和 IP 获取完成，AP 等待
`CODE_WIFI_ON_AP_STARTED` 后，再查询状态或开始打流。

### 2.4 GHz Combo：STA / AP + BLE

BL618DG 先 `wifi_coex_board_config combo`，再 `wifi_coex_init combo`；BL616/BL616CL 不执行这两步。

STA：

```text
wifi_sta_connect -b <BSSID_2G> -c 6 <SSID_2G> <password>
```

或从独立启动流程选择 AP：

```text
wifi_ap_start -s COEX_2G_AP -k 12345678 -c 6 -I 192.168.169.1 -S 2 -L 100
```

BLE 广播、GATT 服务和对端连接由应用启动。两种 Wi-Fi 角色均不需要硬件 start；单 STA 可按
第 5 节选择 PS-PTA。

### Standalone 单天线：5 GHz STA / AP + BT/BLE

完成第 3 节的单天线声明、`wifi_coex_init standalone` 和 `wifi_bt_init` 后，选择 STA：

```text
wifi_sta_connect -b <BSSID_5G> -c 36 <SSID_5G> <password>
```

或 AP：

```text
wifi_ap_start -s COEX_5G_AP -k 12345678 -c 36 -I 192.168.169.1 -S 2 -L 100
```

SDK 自动使用 F，不执行 `wifi_coex_start`。若改用 2.4 GHz，只需更换连接参数或 AP 信道，
板级初始化不变，SDK 自动使用 D。BR/EDR SPP 示例见 [README.md](README.md#bluetooth-spp)。

### Standalone 双天线：STA / AP + BT/BLE

使用 README 中的双天线验证构建，在真实双天线板上执行：

```text
wifi_coex_board_config standalone_dual_ant
wifi_coex_init standalone
wifi_bt_init
```

初始化成功后使用上面的 STA 或 AP 命令，可选 2.4 GHz 或 5 GHz。基础配置允许双 path 并行，
不配置 SPDT，不执行 PS-PTA start。

## 5. PS-PTA 与 Protection

### PS-PTA

受支持的 2.4 GHz 单 STA 获得 IP 后，按需执行：

```text
wifi_coex_duty_set 50
wifi_coex_start
wifi_coex_status
```

Duty 范围为 `10～90 ms`，表示每个 TBTT 周期内的 Wi-Fi active window，常见周期约 100 ms。
可以提前设置或运行中调整；未开启 PS-PTA 时仅保存该值。

启动前由应用保证普通 STA PS 关闭；PS-PTA 运行时不要同时调用普通 STA PS 控制接口。
返回 hardware-only：

```text
wifi_coex_stop
wifi_coex_status
```

正常 stop 关闭 PS-PTA 和其使用的普通 STA PS，保留基础硬件共存、Wi-Fi 链路和 Protection 设置。
同一连接重复 start、重复 stop 均幂等；断开重连后需显式重新开启 PS-PTA。

### Connection Protection

在协议栈初始化完成后、Wi-Fi scan/connect 前按需执行：

```text
wifi_coex_protection 1
```

它不负责建立基础天线路由。BL618DG combo 的 Protection 用于 2.4 GHz 操作及纯 2.4 GHz
扫描；单天线 standalone 对 2.4 GHz 操作及包含 2.4 GHz 的扫描提高 Wi-Fi 优先级，5 GHz
非扫描操作和纯 5 GHz 扫描跳过 priority 保护。双天线并行配置跳过该保护。

不再需要时执行 `wifi_coex_protection 0`，不必联动 PS-PTA start/stop。

## 6. API 与状态

头文件为 `wifi_mgmr_coex.h`：

```c
int wifi_mgmr_coex_start(void);
int wifi_mgmr_coex_stop(void);
int wifi_mgmr_coex_status_get(struct wifi_mgmr_coex_status *status);
int wifi_mgmr_coex_duty_set(uint8_t active_ms);
int wifi_mgmr_coex_protection_set(bool enable);
```

BL618DG 应用先调用 `wifi_mgmr_coex_board_configure(topology, spdt_gpio)`，无 SPDT 时传 `-1`；
再调用 `wifi_mgmr_coex_rf_prepare(WIFI_MGMR_COEX_RF_COMBO)` 或
`wifi_mgmr_coex_rf_prepare(WIFI_MGMR_COEX_RF_STANDALONE)`。
创建 Wi-Fi/BT 任务前检查 `wifi_mgmr_coex_rf_status_get()`。这与两层 CLI 使用同一流程，
须与 Board Config 配置和任务创建串行执行。准备成功不等于 MACSW baseline 已经应用。

自定义 BSP 只实现 RF/GPIO 操作，经 `platform_bouffalo_sdk.c` 的静态操作表适配；
运行时 SPDT callback 不阻塞、不查询 MGMR。Board overlay 只有实现相同硬件合同后才声明
`BOARD_RF_COEX_OPS_SUPPORTED`，包括无硬件写入的 `board_rf_modes_get()`。缺少绑定时初始化返回 `NOT_SUPPORTED`。
底层 `board_rf_*_init()` 不再登记共存配置，应用不应绕过统一入口直接启动协议栈。
`wifi_mgmr_coex_board_configure()` 仍仅保存事实，不代替 RF 准备。

`wifi_coex_status` 与 `status_get()` 对应。仅在返回成功时使用状态：

| 字段 | 含义 |
|---|---|
| `hardware_configured` | 基础硬件配置已就绪且与当前 radio 状态一致 |
| `active` | 当前有有效的 Wi-Fi 共存运行配置，不等同于 PS-PTA 开启 |
| `effective_runtime` / CLI `runtime` | `hardware_only` 或 `ps_pta` |
| `ps_pta_running` | PS-PTA 已进入运行阶段；start 请求成功不代替此项确认 |
| `band` | active 配置的 Wi-Fi band；CLI 在 inactive 时显示 `none` |
| `duty_active_ms` / CLI `duty` | 保存的 Wi-Fi active window |

正常未连接时可见 `hardware_configured=1 active=0`；有有效 Wi-Fi 链路时，stop 后可见
`active=1 runtime=hardware_only ps_pta_running=0`。

所有控制 API 均须检查返回值，不能把错误当作已生效或继续沿用旧状态：

| 返回值 | 处理方式 |
|---|---|
| `OK` | 操作成功 |
| `INVALID_ARGUMENT` | 检查参数及 duty 范围 |
| `NOT_SUPPORTED` | 检查拓扑、频段、Wi-Fi 角色及构建配置 |
| `NOT_READY` | 检查初始化和当前连接状态 |
| `BUSY` | 等待扫描或当前操作完成后重试 |
| `APPLY_FAILED` | 停止共存业务，保存日志并检查硬件配置；故障未恢复前不继续打流 |

表中错误名对应 `WIFI_MGMR_COEX_ERR_*`，成功值为 `WIFI_MGMR_COEX_OK`。
