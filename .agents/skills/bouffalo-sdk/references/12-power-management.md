# 低功耗与唤醒

SDK 的低功耗能力由目标芯片、板级电路、固件配置和唤醒源共同决定。PDS、HBN 等模式及其等级不能统一套用另一颗芯片的入口、GPIO 范围、内存保持、恢复路径或电流数值。以下是源码和示例入口；编译成功不等于已经验证睡眠、唤醒或功耗。

## 先确定芯片、模式和恢复路径

1. 指定 `CHIP`、`BOARD`、核心、供电和目标唤醒源，在 `drivers/soc/<chip>/std/include/<chip>_pm.h`、`drivers/sys/<chip>/` 与 `examples/pmu/<chip>/` 查实际接口和级别。
2. 区分直接进入 PDS/HBN 与 `bl_lp_pds_enter_with_restore` 等保存恢复路径；是否从调用点继续、重新启动或保留 RAM/PSRAM 由目标模式、芯片和入口决定，不能根据统一的“深睡眠”名称推断。
3. 记录时钟、唤醒 IO 电平/上下拉、外设和无线状态，并按目标例子的顺序关闭或屏蔽不使用的唤醒源。进入 HBN 后的恢复可能是复位重启，例如 BL616CL `pm_demo`，运行时 RAM 配置会丢失。

以下源码接口需要各自的目标芯片头文件及构建条件：

| CHIP | 入口 | 关键区别 |
| --- | --- | --- |
| BL616 | `pm_pds_mode_enter(level, sleep_time)`、`pm_hbn_mode_enter(level, sleep_time)`，见 `bl616_pm.h` | PDS GPIO 与 AON GPIO 有不同分组及触发能力；HBN 也有 LP firmware 进入路径 |
| BL616CL | `pm_pds_mode_enter(level, sleep_time, lowpower_cfg)`、`pm_hbn_mode_enter(level, sleep_time)`，见 `bl616cl_pm.h` | PDS 多一个配置参数；GPIO 配置使用 `pm_lowpower_gpio_cfg`，不得照搬 BL616 函数签名 |
| BL618DG | `pm_pds_mode_enter(level, sleep_time)`、`pm_hbn_mode_enter(level, sleep_time)`，见 `bl618dg_pm.h` | 区分 AP、NP、LP；所查头文件的 HBN 枚举列出 level 0 |

sleep time 的单位和含义应查所用入口：例如 BL616CL/BL618DG HBN RTC 路径使用时钟周期，BL616CL `hbn_rtc` 示例以 `32768` 表示名义 1 秒；BL616CL `pm_demo` 的 shell `wake_ms` 是上层毫秒参数，先换算成计数。RC32K 误差会影响实际时间。部分 PDS 实现会在非零计数不大于 warmup 阈值时直接返回；PDS1 的入口要求先注册对应 IRQ。不要把一次 `void` 函数返回当作已进入睡眠的证明。

## GPIO 与 RTC 唤醒

- **BL616**：`examples/pmu/bl616/pds_io_wakeup/` 区分 AON pad 和 PDS pad，并分别配置唤醒与 IRQ；`hbn_io_wakeup/` 展示 LP firmware HBN 路径。`drivers/sys/bl616/bl616_lp.h` 中部分引脚共用触发配置，PDS 与 AON 支持的触发方式也不同。
- **BL616CL**：`examples/pmu/bl616cl/pds_io_wakeup/` 设置 `io_ie`、上下拉、唤醒 mask 与各引脚触发方式。唤醒检测要求输入使能，但 API 不会因输入未使能而拒绝配置；当前实现还跳过 Flash IO。示例还检查 DCDC 引脚冲突。`hbn_io_wakeup/` 示例限制 GPIO0–5 和 HBN0/1，不能推广到其他芯片。`pm_demo` 的板级默认配置、运行时 GPIO 设置、看门狗和复位后恢复规则见该目录 README。
- **BL618DG**：`examples/pmu/bl618dg/pds_io_wakeup/` 和 `hbn_io_wakeup/` 配置 `lp_gpio_cfg_type`，HBN 示例仅接受 GPIO0–7。RTC 示例位于相应芯片的 `pds_rtc/`、`hbn_rtc/`；读取唤醒源和 reset reason 时按所用示例检查寄存器和返回路径。

配置时核对实际引脚、板上外接电平、上下拉、触发方式、输入使能及电源域；避免选用 Flash、供电控制或其他被占用的引脚。示例列出的可选引脚只是相应芯片/模式和示例的范围。

## FreeRTOS 电源管理框架

`components/os/Kconfig` 的 `CONFIG_PWRMGMT` 控制 `components/os/power_mgmt/`；`CONFIG_TICKLESS_EXT` 与依赖它的 `CONFIG_TICKLESS_ACCURATE` 选择相关 tickless 实现。`drivers/sys/CMakeLists.txt` 按 CHIP 选择系统低功耗代码，支持的回调和恢复路径不完全相同。

准确 tickless 路径会检查 `pm_sleep_check_dispatch()`：使用 `pm_sleep_check_register(name, cb, priority)` 注册，回调返回 0 允许睡眠，非 0 阻止睡眠；数字越小优先级越高，名称需保持有效。默认容量为 8，重名返回 -2、无效参数或容量不足返回 -1；dispatch 遇到首个非零值停止。当前实现不应依赖自动打印拒绝原因。准确 tickless 调用该检查时已关闭中断，且已有 PM 事件时可能跳过 dispatch；回调应短小且不可阻塞。

接口声明及回调约束见 `components/os/power_mgmt/` 的 `pm_manager.h` 和目标芯片实现。

普通 tickless 路径使用 `tickless_hooks_register` 的 allow/prepare/after 回调；当前保存一组 hooks，后一次注册会覆盖先前设置，不能视为多订阅列表。`pm_enable_tickless()` / `pm_disable_tickless()` 的实际影响取决于当前配置和调用路径。

## BL618DG AP 与 LP 配合

`examples/pmu/bl618dg/pds_lp_wakeup/` 的 AP/LP 是两个独立工程：AP 配置软件唤醒源并进入 PDS，LP 通过 `PDS_Triger_SW_Wakeup_PDS()` 触发唤醒，再清除触发。该示例要求 LP 工作期间保留 RC32M；进入 PDS 前后 UART 共享时钟源可能变化，导致串口乱码。具体时钟选择与约束见示例 README，不要把此设置直接推广到其他板或模式。NP、PSRAM 保持和跨核时序需单独验证。

## 调试和测量

先分别验证构建与下载，再检查实际休眠前后日志、唤醒源、reset reason、目标任务或网络恢复；测量电流时记录芯片和板型、供电、仪表、固件版本、模式/级别、时钟、连接状态、外设与 IO 状态和采样窗口。RTC 电压不等于系统电流。功耗未下降或未唤醒时，分别排查未屏蔽的唤醒源、IO 电平、外设和无线阻塞、时钟、看门狗以及电路漏电，不能单凭一个症状断定原因。

Wi-Fi/BLE 省电和共存应结合对应无线栈、连接状态与芯片模式分析；不要把无线省电接口当成所有系统模式通用开关。Flash 保存状态的擦写边界见 [10-memory-management.md](10-memory-management.md)，外设中断入口见 [08-peripherals.md](08-peripherals.md)。
