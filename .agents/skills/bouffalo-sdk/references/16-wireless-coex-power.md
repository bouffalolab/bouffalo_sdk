# Wi-Fi / Bluetooth 共存与无线省电

先区分 **基础硬件共存**、可选 **PS-PTA** 时间窗口、**Connection Protection** 和普通 **STA PS**。它们有不同的启停条件和状态，不应把开启 Wi-Fi PS 等同于共存成功。以下以当前 FHOST + MACSW 及 `examples/coex/wifi_bz_coexm/` 为依据；其他 host、芯片和固件应核对自身实现。

## 芯片、拓扑与初始化

| 目标 | 共存硬件路径 | 注意 |
| --- | --- | --- |
| BL616 / BL616CL | combo 共享路径，当前 plan 为 2.4 GHz | 该示例在启动时准备 RF，再执行 `wifi_bt_init` |
| BL618DG combo | 共享路径，当前 plan 为 2.4 GHz | 先声明 `wifi_coex_board_config combo`，再 `wifi_coex_init combo` |
| BL618DG standalone 单天线 | SPDT + diplexer，支持 2.4/5 GHz 的硬件配置 | 必须按真实接线声明 SPDT GPIO 和极性；没有默认引脚 |
| BL618DG standalone 双天线 | 双路径并行，2.4/5 GHz hardware-only | 需 `COEX_DUAL_ANT_VALIDATION=ON` 和实际板级验证，默认关闭 |

BL618DG 在板级声明、RF 准备成功后执行 `wifi_bt_init`，等待 manager 与 Bluetooth 初始化完成，再建立 Wi-Fi 连接或启动 AP/蓝牙业务。准备成功不等于已应用当前连接的硬件 baseline。物理拓扑由板卡决定，不能为了让命令通过而选择与接线不符的路径；控制命令和完整步骤见 `examples/coex/wifi_bz_coexm/COEX_GUIDE.md`。

基础硬件共存在支持的拓扑和信道下自动维护，`hardware_only` 不表示 PTA 关闭。BL618DG 单天线在 2.4 GHz 和扫描使用相应动态 PTA/SPDT，稳定 5 GHz 使用固定 BT 路径；扫描结束后根据有效 home 状态恢复。具体频段、STA/AP 并存及通道约束以该工程指南和当前实现为准。

## PS-PTA 与状态

示例中 PS-PTA 仅用于支持的 **已连接 2.4 GHz 单 STA**；开始前关闭普通 STA PS，不用于 AP、STA+AP、5 GHz 或双天线并行。应用在合适条件下调用 `wifi_mgmr_coex_start()` / `stop()`，或在支持的 FHOST CLI 使用无参数 `wifi_coex_start` / `wifi_coex_stop`。重连后不会自动重新开启；stop 保留硬件 baseline 和 Protection 配置。

`wifi_coex_duty_set <10-90>` 的值是 Wi-Fi active window 的**毫秒数**，不是无条件的百分比。未运行 PS-PTA 时它只是保存的设置。`wifi_coex_status` 只有成功返回的快照有效：

- `hardware_configured`：硬件基线是否就绪并与 radio 状态一致。
- `active`：是否存在有效共存运行配置；**不等于** PS-PTA 正在运行。
- `runtime` 和 `ps_pta_running`：区分 hardware-only 与时间窗口是否实际运行。
- `band`、`duty`：当前频段和保存的 active window；hardware-only 下的 duty 不能证明时间分片在运行。

MACSW 的 PS-PTA coordinator 以 TBTT 为锚，在 Wi-Fi 窗口内恢复 host TX，结束窗口时能否暂停 host TX 由平台决定；窗口长度取配置的 beacon guard 和 `wifi_active_us` 的较大值；未覆盖宏时 guard 默认 4000 µs，不是窗口外额外增加的时长。radio 状态同步丢失会暂停 runtime。BL616CL 的平台不提供 host TX gate，不能从 BL616 的现象推断其门控行为。

## 连接阶段保护

Connection Protection 默认关闭，按需启用后在支持的 scan、connect、DHCP、disconnect、key management 等阶段临时保护 Wi-Fi；stop PS-PTA 不等于关闭 Protection。保护条件、频段和路径依平台而不同，例如 BL618DG 双天线并行、单天线 5 GHz 与 2.4 GHz 使用不同规则。低层按事件类型计数并在关键阶段阻止让出 RF；保护释放后硬件优先级也可能由后续 idle reconcile 或 sleep 路径恢复，不能假定每次释放立即写硬件。

共存控制 API 及状态结构见 `wifi6/fhost/include/wifi_mgmr_coex.h`；检查所有返回值。`NOT_READY`、`NOT_SUPPORTED`、`BUSY` 和硬件应用失败各有不同处理，不应把失败当成生效或盲目继续打流。部分 recipe 写入失败会锁存 fault；保存实际状态和日志，并按目标指南恢复。

## 普通 STA PS、DTIM 与系统 tickless

FHOST 的 `wifi_mgmr_sta_ps_enter` 请求动态 STA PS；wl80211 对应路径在 `MACSW_POWERSAVE` 条件下发送 `PS_MODE_ON_DYN`，退出请求关闭省电。具体生效由目标栈和配置决定。系统 PM 的 `pm_enable_tickless()` 还负责低功耗准备及 tickless 入口，与单独开启 STA PS 不同。

`components/os/power_mgmt/pm_manager.c` 的 `set_dtim_config(dtim)` 将 0 映射为 1，其他值赋给 LPFW 的 `dtim_origin`，目标字段类型会发生转换且该函数未做范围校验；休眠前复制至 `dtim_num`。关联参数中的 listen interval 是另一项配置，并非同一个变量；不要从 `PS_MODE_ON_DYN` 名称推断 DTIM 自动调整。可用范围、重连后的状态、广播接收与实际电流需按芯片、AP 和目标固件验证。GPIO 唤醒是独立的芯片低功耗唤醒源，见 [12-power-management.md](12-power-management.md)。

## 芯片差异与排查

共用 `coexm` 调度通过 `hal_machw_coex_*` 下沉平台行为；不要在共用逻辑硬编码芯片特有寄存器或 PTI。BL616CL 的 `0x24920028` 是 `STAT_COEX`，不能按 BL616 的 PTA `CONFIG2` 写入。BL616CL 的 PTI 策略和 BL618DG 的 RF topology/5 GHz 路径也不能混用。

排查按实际产品记录芯片/板、天线接线、构建配置、栈和 controller、频段/角色、连接及 IP、基础硬件状态、PS-PTA 和 Protection 状态，再进行单无线与并发对照。`examples/wifi/coex/wifi_ble/` 和 `examples/coex/wifi_bz_coexm/` 是不同工程；旧测试脚本可能使用不同固件或 CLI 格式，不能直接当成当前工程通过的证据。吞吐应记录方向、实际发送速率、接收结果、丢包、窗口和对端；低功耗还需记录板级供电、仪表、时钟及连接状态。构建、连接、收发和测电流分别验证。
