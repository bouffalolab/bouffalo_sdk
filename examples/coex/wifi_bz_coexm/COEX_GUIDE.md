# Wi-Fi/Bluetooth Coexistence Guide

本文介绍 BL616、BL616CL 和 BL618DG 的共存配置。编译和测速命令见 [README.md](README.md)。
BL618DG 在 5 GHz、40 MHz 下的四方向吞吐见
[AP40 实测结果](README.md#bl618dg-ap40-throughput-2026-11-20)。

## 1. 使用原则

| 功能 | 作用 | 使用方式 |
|---|---|---|
| 基础硬件共存 | 根据板级拓扑和 Wi-Fi 状态配置 PTA/SPDT | SDK 自动维护，不需要 start |
| PS-PTA | 利用 STA 省电机制分配软件时间窗口 | `wifi_coex_start` 开启，`wifi_coex_stop` 关闭 |
| Connection Protection | 在支持的 scan/connect/key/DHCP 阶段临时提高 Wi-Fi 优先级 | `wifi_coex_protection 1/0` 独立控制，默认关闭 |

`hardware_only` 表示未启用 PS-PTA，不表示关闭硬件 PTA。`wifi_coex_start` 不接收模式参数，
也不负责创建 Bluetooth/BLE 服务或连接。BL618DG 的板级拓扑和 RF 模式在编译期由 Kconfig 固定
（见第 2、3 节），运行期不可修改。

## 2. 拓扑与适用场景

| 芯片与拓扑 | Wi-Fi 频段 | 自动基础配置 | 可选 PS-PTA |
|---|---|---|---|
| BL616 / BL616CL combo | 2.4 GHz | 硬件 PTA | 已连接的单 STA |
| BL618DG combo | 2.4 GHz | 硬件 PTA | 已连接的单 STA |
| BL618DG standalone 单天线/SPDT | 2.4 GHz | 动态 PTA/SPDT | 已连接的单 STA |
| BL618DG standalone 单天线/SPDT | 5 GHz | SPDT 固定 BT，5G channel-aware 配置 | 不使用 |
| BL618DG standalone 双天线 | 2.4 / 5 GHz | 并行共存，需发布版 MACSW 库支持 | 不使用 |

应用链接预编译的发布版 MACSW 库。`CONFIG_COEX_BOARD_TOPOLOGY_STANDALONE_DUAL_ANT` 用于声明
双天线板级拓扑，仅在所用发布版 MACSW 库支持双天线并行共存时选择。
BL618DG combo 的当前共存配置限于 2.4 GHz。

基础硬件共存适用于上述配置的 STA、AP，以及使用相同完整信道配置的 STA+AP；不同信道的
STA+AP 不支持。PS-PTA 只用于受支持的 2.4 GHz 单 STA，不用于 AP、STA+AP 或 5 GHz。
Wi-Fi 同一时刻只使用一个频段，不支持 2.4 GHz 与 5 GHz 双频同时工作。

### BL618DG 的三种硬件连接

BL618DG 的板级拓扑是硬件事实，在编译期通过 `Wi-Fi/Bluetooth coexistence` 菜单选择，运行期
不可修改，也不再通过 shell 命令声明：

| 拓扑 | RF 与天线连接 | Kconfig 拓扑 | 准备模式（SDK 自动） |
|---|---|---|---|
| Combo | 内部 combo/2G path，无外部 SPDT | `CONFIG_COEX_BOARD_TOPOLOGY_COMBO` | combo |
| Standalone 单天线 | BZ 与 Wi-Fi 2.4G 经 SPDT，5G 经 diplexer，合用一根天线 | `CONFIG_COEX_BOARD_TOPOLOGY_STANDALONE_SINGLE_ANT_SPDT` | standalone |
| Standalone 双天线 | BZ 独立天线；Wi-Fi 使用另一根双频天线，无 SPDT | `CONFIG_COEX_BOARD_TOPOLOGY_STANDALONE_DUAL_ANT` | standalone |

按原理图选择拓扑，不需要提前选择 Wi-Fi band，也不能把 SPDT 单天线板配置成双天线。
拓扑决定 WIRELESS 记录准备的 RF 模式（combo 或 standalone）以及是否占用 SPDT GPIO；
未选择拓扑的构建会失败，不支持的拓扑/GPIO 组合在启动时被拒绝，SDK 不会自动切换为其他 RF 模式。

SPDT 只使用一个 GPIO，对应 `CONFIG_COEX_BOARD_SPDT_GPIO`，**默认 18**，仅单天线 SPDT 拓扑使用。
当前 BSP 要求偶数 GPIO 的高电平选 BT、奇数 GPIO 的低电平选 BT，必须与外部开关真值表及 RF
接线一致；其他接线需在 BSP 中适配。GPIO、pinmux 和极性属于板级配置。

## 3. 上电初始化

BL618DG 的拓扑和 RF 模式在编译期确定，启动顺序由 SDK 的 SYSINIT 完成，不需要 shell 命令：

1. 在 menuconfig 的 `Wi-Fi/Bluetooth coexistence` 菜单或工程 defconfig 中，选择与板卡匹配、
   且由发布版 MACSW 库支持的拓扑。
2. 编译、烧录并复位；
3. SYSINIT 的 WIRELESS 记录在 `main()`/shell 之前发布板级配置并准备 RF，校验 RF 状态后启动
   Wi-Fi 固件和 manager。失败时打印日志并中止启动。
4. BL618DG 看到 `RF is ready` 和 shell 提示符后即可使用 Wi-Fi 命令。需要 Bluetooth 时，
   执行一次 `wifi_bt_init` 并等到 `BD_ADDR` 日志。

例如示例默认的单天线 GPIO18 板，defconfig 中对应：

```make
CONFIG_COEX_BOARD_TOPOLOGY_STANDALONE_SINGLE_ANT_SPDT =y
CONFIG_COEX_BOARD_SPDT_GPIO                          =18
```

也可在 `make menuconfig CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap` 中选择相同选项，
并使用相同的 `BUILD_DIR` 编译。保存后的 menuconfig 优先于 defconfig 和命令行配置。未选择
拓扑的 `CONFIG_COEX` 构建会报错。改变拓扑或 GPIO 后需重新编译、烧录并重启。

`wifi_coex_debug_board_config show` 可查看编译进固件的拓扑、允许模式、已选模式和实际 RF path。
BL616/BL616CL 同样在上电时初始化 Wi-Fi，需要 Bluetooth 时执行 `wifi_bt_init`。应用注册事件
日志的时间可能晚于 Wi-Fi manager 就绪，`CODE_WIFI_ON_MGMR_DONE` 日志可能缺失。若 RF 初始化
失败，检查 `CONFIG_COEX_BOARD_*` 对应的板级拓扑和 GPIO 后重新启动。

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

按第 3 节完成构建配置并启动。将 `<...>` 替换为实际参数。STA 使用目标 AP 的 BSSID 和信道，
等到连接并取得 IP。AP 等到 `AP-ENABLED`，确认实际信道和带宽，再验证对端 STA 成功关联、
取得 IP 并能 ping 通 AP。使用符合当地法规及板级配置的信道。

### 2.4 GHz Combo：STA / AP + BLE

BL618DG 在编译时选择 `CONFIG_COEX_BOARD_TOPOLOGY_COMBO` 并重新构建；
BL616/BL616CL 没有板级拓扑选项，不需要这一步。

STA：

```text
wifi_sta_connect -b <BSSID_2G> -c 6 <SSID_2G> <password>
```

或选择 AP：

```text
wifi_ap_start -s COEX_2G_AP -a WPA2 -k <passphrase> -c 6 -b 0 -I 192.168.169.1
```

BLE 广播、GATT 服务和对端连接由应用启动。两种 Wi-Fi 角色均不需要硬件 start；单 STA 可按
第 5 节选择 PS-PTA。

### Standalone 单天线：5 GHz STA / AP + BT/BLE

按第 3 节选择 `CONFIG_COEX_BOARD_TOPOLOGY_STANDALONE_SINGLE_ANT_SPDT` 并完成构建、
烧录和启动后，选择 STA：

```text
wifi_sta_connect -b <BSSID_5G> -c 36 <SSID_5G> <password>
```

或 AP：

```text
wifi_ap_start -s COEX_5G_AP -a WPA2 -k <passphrase> -c 36 -b 1 -I 192.168.169.1
```

`-b 1` 请求 40 MHz。使用 40 MHz 时保持 WMM 开启，并在 AP 和对端 STA 上核对实际带宽。SDK 在 5 GHz
自动使用 F，采用 `hardware_only` 运行。切换到 2.4 GHz 时，按对应频段选择连接参数，SDK
自动使用 D。BR/EDR SPP 示例见 [README.md](README.md#bluetooth-spp)。

复测 5 GHz AP40 时关闭对端 STA 的省电，保持 Bluetooth 空闲，四个方向各测 60 秒 × 2。
板卡、构建配置和接收端数据见 [AP40 实测结果](README.md#bl618dg-ap40-throughput-2026-11-20)。

## 5. PS-PTA 与 Protection

### PS-PTA

受支持的 2.4 GHz 单 STA 获得 IP 后，先关闭普通 STA 省电，再开启 PS-PTA。

```text
wifi_sta_ps_off
wifi_coex_duty_set 50
wifi_coex_start
wifi_coex_status
```

Duty 范围为 `10～90 ms`，表示每个 TBTT 周期内的 Wi-Fi active window，常见周期约 100 ms。
可以提前设置或运行中调整；未开启 PS-PTA 时仅保存该值。

确认 `ps_pta_running=1`。PS-PTA 负责运行期间的 STA 省电控制。
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

BL618DG 构建中，SYSINIT 的 WIRELESS 记录在启动 Wi-Fi 固件/manager 前读取 Kconfig 拓扑并调用
`wifi_mgmr_coex_board_configure(topology, spdt_gpio)`（无 SPDT 时传 `-1`）与
`wifi_mgmr_coex_rf_prepare(WIFI_MGMR_COEX_RF_COMBO)` 或
`wifi_mgmr_coex_rf_prepare(WIFI_MGMR_COEX_RF_STANDALONE)`，成功后校验
`wifi_mgmr_coex_rf_status_get()`，因此应用不需要也不应重复调用。自定义板级集成若自行绑定
RF/GPIO 操作，仍按上述顺序调用这些公开接口，须与 RF 配置和任务创建串行执行；普通应用启动
流程不调用。准备成功不等于 MACSW baseline 已经应用。

自定义 BSP 提供 RF/GPIO 操作，BL618DG 在 `platform_chip_bl618dg.c` 中通过静态操作表绑定。
运行时 SPDT callback 不阻塞、不查询 MGMR。Board overlay 实现相同的板级接口后才声明
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
