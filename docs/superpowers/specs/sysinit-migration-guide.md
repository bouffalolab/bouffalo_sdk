# SYSINIT 迁移与接入指南

> 适用对象：SDK 组件、板级、专用固件（Boot2 / LPFW / MFG）维护者。
> 普通应用开发者只需阅读[第 6 章](#6-应用侧规则强制)，不得在应用代码中注册 SYSINIT。
>
> 本指南是**操作手册**，用于把 SDK 固定初始化迁移到 SYSINIT、选择 subsystem/order、
> 为被接管的旧入口增加弃用兼容层。发生冲突时以
> [迁移规范](2026-11-04-sysinit-migration.md) 和
> [框架设计](2026-04-29-sysinit-framework-design.md) 为准。
> FreeBSD 原理参考见 [SYSINIT_ARCHITECTURE.md](SYSINIT_ARCHITECTURE.md)，它不是 SDK 接入指南。

> **文档状态（2026-11-11 更新）**：本文已按当前实现与合规审计同步（NET 阶段预留、
> APP/LAST 兼容事件、续跑任务优先级窗口、`CONFIG_COMPAT_V2` 验收口径、专用固件开关归属）。
> 已知但尚未修复的代码问题见
> [SYSINIT 迁移合规性审计报告](../../../SYSINIT_MIGRATION_AUDIT_2026-11-11.md)；
> 在对应代码修复前，相关验收项不计为通过。

---

## 1. 一分钟判断

对每一段固定初始化依次回答：

1. **它属于谁？**
   - SDK 组件 / 板级固定初始化 → 可以迁移；
   - 应用业务初始化、事件订阅、媒体/镜像挂载、业务任务 → **不要迁移**，保留在 `main()` 或任务入口；
   - 可选应用模块（AT、业务协议栈等）→ 在其自己的 `*_start()` 中初始化，不注册 SYSINIT。
2. **它需要 RTOS 调度吗？**
   - 需要任务、信号量、`vTaskDelay()`、等待事件/其它任务 → 必须在 `SI_SUB_SCHED` **之后**；
   - 只做同步硬件/内存/Flash 操作 → 可以在 `SI_SUB_SCHED` **之前**。
3. **它依赖哪些已有初始化？**
   - console → `DRIVERS/FIRST`；heap → `DRIVERS/SECOND`；
   - Flash/MTD/分区表 → `SERVICES/FIRST`；LittleFS/KV → `SERVICES/SECOND`；
   - SoC 收尾：RC32K/RC32M trim/DCDC 在表前由 `System_Post_Init()` 完成；全局开中断 → `PRE_SCHED`；
   - async_event → `EVENTS/FIRST` 之后；
   - RF/WiFi 后端 → `WIRELESS`；
   - TCP/IP 交接 → `NET`（当前预留）；
   - 应用 `main()` / 面向应用的事件 → `APP`。

4. **有没有旧公开入口需要兼容？**
   - 有 → 按[第 5 章](#5-带兼容入口的接入模板)处理：私有 `_func` + deprecated 公开入口 +
     `CONFIG_COMPAT_V2` 守卫；
   - 无 → 只加 SYSINIT 记录并删除旧调用点。
5. **MFG / LPFW / Boot2 这三类特殊 app（以及自带独立启动流程的 Eflash Loader）是否依赖它？**
   - 否 → 不需要考虑该函数在 `CONFIG_SYSINIT=n` 下的调用路径，不要为此增加 fallback、
     条件分支或额外兼容入口；
   - 是 → 明确它们调用公开兼容入口还是 `_func()`，并只在对应特殊 app 的启动流程中
     显式初始化；常规 app 仍必须保持 `CONFIG_SYSINIT=y`。

---

## 2. 当前启动时序

### 2.1 分隔符之前（pre-scheduler）

```text
复位 / SystemInit
  → start_load（加载 .data、清 .bss）
  → libc TLS 初始化（picolibc）
  → System_Post_Init
      → SoC BOD / cache / XTAL 处理
      → sysinit_run_pre_schedule()    // 单次遍历链接表
          → LOWEST / HW / CLOCK / INTR / DRIVERS（含 C++ 构造函数 cxx_runtime）
          → SERVICES（分区表 boot2、LittleFS、KV、低功耗服务）
          → PRE_SCHED（全局开中断；RC32K/RC32M trim 已在表前完成）
          → SCHED 分隔符：创建续跑任务、启动调度器，永不返回
```

**约束：**

- pre-scheduler 记录运行在启动上下文，**不能创建依赖调度的任务、不能等待、不能 `vTaskDelay()`**；
- 必需初始化失败直接 `abort()` 或早期致命入口，不做框架级重试/回滚；
- 板级 IRQ 状态、RAM/TCM 属性、原有芯片条件编译必须保持。

### 2.2 分隔符之后（post-scheduler，续跑任务）

```text
sysinit 续跑任务（configMAX_PRIORITIES - 1）
  → SI_SUB_POST_SCHED（56，当前预留）
  → SI_SUB_EVENTS（58）：async_event 初始化 + 平台 EV_WIFI filter
  → SI_SUB_WIRELESS（70）：RF / WiFi 后端同步初始化
  → SI_SUB_NET（80，当前预留）：TCP/IP 交接
  → SI_SUB_APP/MIDDLE：进入应用 main()
  → SI_SUB_APP/LAST：发布面向应用的 INIT_DONE/MGMR_DONE 兼容事件
  → 续跑任务删除；应用任务运行
```

**约束：**

- 分隔符之后的记录允许阻塞、等待 `_SYS` 就绪事件、创建任务；
- `main()` 已经是任务上下文，应用**不得再调用 `vTaskStartScheduler()`**；
  SYSINIT 构建中的旧调用由 `-Wl,--wrap,vTaskStartScheduler` 兼容入口拦截
  （打印弃用提示 + `longjmp` 回 `APP` 记录，跳过其后代码），示例仍应删除该调用；
- `APP/LAST` 的兼容事件记录会临时把续跑任务优先级降到 `1`，让 `main()` 创建的应用任务
  先完成事件注册，再收到 `INIT_DONE`/`MGMR_DONE`。

---

## 3. `SI_SUB_*` 选择

### 3.1 子系统表

定义位置：`components/sysinit/sysinit.h`。值为两位十六进制字符串，链接脚本使用
`KEEP(*(SORT_BY_NAME(.sysinit.*)))`，因此**字典序即执行序**，新增阶段必须保持两位补零。

| 值 | 常量 | 阶段 | 可做 | 不可做 | 典型注册 |
|---|---|---|---|---|---|
| `00` | `SI_SUB_LOWEST` | 公共 board_early | 最早板级回调 | 依赖任何资源 | 公共 `board_early` 弱入口 |
| `10` | `SI_SUB_HW` | CPU/内存/安全 | 早期硬件、Flash 前缀 | 依赖 console/heap | 板级 IRQ 保存、Flash 前缀 |
| `20` | `SI_SUB_CLOCK` | 时钟 | 系统/RTC/外设时钟 | 依赖 RTOS | 时钟树、RTC |
| `30` | `SI_SUB_INTR` | 中断 | IRQ 表、中断控制器 | 依赖线程 | IRQ 初始化 |
| `40` | `SI_SUB_DRIVERS` | 基础驱动/服务 | UART/console、heap、PSRAM 堆、C++ 运行时构造函数 | 依赖调度 | console、`board_heap`、`cxx_runtime`（C++ 构造函数，依赖 heap） |
| `50` | `SI_SUB_SERVICES` | 服务层 | Flash 分区表（boot2）、LittleFS、KV | 依赖 WiFi 后端 | `bflb_boot2`、`lfs_kv` |
| `52` | `SI_SUB_PRE_SCHED` | 调度器前收尾 | 全局开中断（BL602/BL702/BL702L/DG LP）、LP 前置；RC32K/RC32M trim/DCDC 在表前 `System_Post_Init` 完成 | 等待/延时/任务 | `soc_post`、`pm_rc32k_auto_cal`、`pm_sys`、`bl_lp` |
| `55` | `SI_SUB_SCHED` | 分隔符 | 仅 FreeRTOS 集成注册一条 | 其他组件注册 | `sysinit_sched` |
| `56` | `SI_SUB_POST_SCHED` | post-sched 早期 | 通用 post-sched 记录 | 跳过 EVENTS 依赖 | 当前预留 |
| `58` | `SI_SUB_EVENTS` | 异步事件 | async_event 初始化；WiFi4/FHOST 同时注册 `EV_WIFI` 系统 filter（WL80211 不注册） | 依赖 WiFi 后端 | `wifi_events` |
| `70` | `SI_SUB_WIRELESS` | 无线后端 | RF、WiFi firmware/manager | 应用/AT/业务组件 | `rfparam`、`wifi_start`、`board_rf_btble`（BL618DG 仅蓝牙） |
| `80` | `SI_SUB_NET` | TCP/IP 交接 | TCP/IP hand-off | 应用业务 | 当前预留 |
| `90` | `SI_SUB_APP` | 应用阶段 | `main()`、APP 级事件记录 | SDK 业务初始化 | `app_main`、`wifi_app_events` |
| `ff` | `SI_SUB_HIGHEST` | 保留 | — | — | — |

### 3.2 决策树

```text
需要 RTOS 调度/等待/任务/延时？
├─ 是 → 必须放在 SI_SUB_SCHED 之后
│   ├─ 依赖 async_event？            → SI_SUB_EVENTS 或之后
│   ├─ RF/WiFi 后端？                → SI_SUB_WIRELESS
│   ├─ TCP/IP 交接/设备事件？        → SI_SUB_NET（当前预留，不要直接注册）
│   ├─ 通用 post-sched 早期记录？    → SI_SUB_POST_SCHED
│   └─ 应用入口/APP 级事件？        → SI_SUB_APP（MIDDLE/LAST，见第 4 章）
└─ 否 → 放在 SI_SUB_SCHED 之前
    ├─ 只依赖 CPU/内存？             → HW
    ├─ 依赖系统/外设时钟？           → CLOCK
    ├─ 中断控制器？                  → INTR
    ├─ 依赖 console/heap/基础驱动？  → DRIVERS
    ├─ 依赖 Flash/MTD/文件系统？     → SERVICES
    ├─ 全局开中断/服务之后、调度器之前的收尾？ → PRE_SCHED（RC trim/DCDC 在表前）
    └─ 更早的基础步骤？              → LOWEST/HW
```

### 3.3 选择规则

1. **按依赖/上下文选阶段，不按组件归属选。** 名字只是语义标签，真正决定安全性的是
   “分隔符前/后”和依赖顺序。
2. **优先复用现有阶段，不新增 subsystem。** 新增会引入全局顺序约定和文档维护成本；
   只有出现现有阶段无法表达的全局阶段需求时才新增，并同步更新 `sysinit.h`、本指南和框架设计。
3. **`SI_SUB_NET`、`SI_SUB_POST_SCHED` 当前预留**，不要直接注册实现；确需使用时先更新
   设计文档并验证记录顺序。
4. **应用层禁止注册 SYSINIT。** 唯一例外是 `CONFIG_SYSINIT_CUSTOM_BOARD_INIT=y` 的工程
   自定义板级源码，且只能注册自己的板级硬件流程，不能注册 FS/RF/WiFi/事件/业务初始化。
5. **AT 等可选应用模块** 在其 `*_start()` 里初始化/注册事件，不占 `SI_SUB_WIRELESS`；
   事件订阅属于应用行为，不属于 WiFi 后端。
6. **pre-scheduler 记录禁止阻塞。** 需要 `vTaskDelay()`、信号量、等待事件时，必须移到
   `SI_SUB_EVENTS` 或之后。
7. **事件顺序依赖记录在 SYSINIT 和事件总线之间保持一致。** 面向应用的事件在
   `SI_SUB_APP/LAST` 发布；应用在 `main()` 注册 filter，`APP/LAST` 的优先级窗口保证
   `main()` 创建的应用任务先完成注册。

---

## 4. `SI_ORDER_*` 选择

| 值 | 常量 | 使用场景 |
|---|---|---|
| `00` | `SI_ORDER_FIRST` | 阶段内第一个，或独立初始化 |
| `01` | `SI_ORDER_SECOND` | 明确排在 FIRST 之后 |
| `02` | `SI_ORDER_THIRD` | 继续显式排序 |
| `03` | `SI_ORDER_FOURTH` | 继续显式排序 |
| `04` | `SI_ORDER_FIFTH` | 继续显式排序 |
| `80` | `SI_ORDER_MIDDLE` | 应用入口/阶段中部 |
| `ff` | `SI_ORDER_LAST` | 阶段最后、兼容事件/观察者 |

> 只使用 `sysinit.h` 中已定义的 order，不私写十六进制字符串；确需新常量时先更新
> `components/sysinit/sysinit.h` 和本指南。

规则：

1. **有真实先后依赖时，分配不同的显式 order**（FIRST/SECOND/THIRD/FOURTH…），
   并在记录上方注释依赖原因。
2. **无依赖的普通初始化** 使用 FIRST/MIDDLE 的既有约定即可。
3. **同一 `sub + order` 的记录按 section 名（包含 `_name`）字典序执行**；
   不要依赖源码出现顺序或链接顺序。需要固定顺序就换用不同 order。
4. **`APP` 阶段：** `main()` 由框架注册为 `MIDDLE`，面向应用的事件记录使用 `LAST`。
   组件的 APP 级记录不要注册在 `FIRST`（会早于 `main()`）或 `LAST` 之后。
5. **记录名唯一、简短，使用 C 标识符。** 名字会影响同 order 的排序，避免 `foo`/`foo2` 这类
   看不出依赖的名字；新增阶段/顺序时同步更新 `sysinit.h` 注释。

当前典型顺序：

```text
40/MIDDLE cxx_runtime（C++ 全局构造函数；console/heap/board 服务之后、分隔符之前）
50/FIRST  boot2（MTD 组件已编译时；BL618DG AP 例外为 40/FOURTH，早于释放从核）
52/FIRST  soc_post（全局开中断；BL602/702/702L/DG LP。BL616/CL/DG AP 的 trim/DCDC 在表前完成）
52/SECOND pm_rc32k_auto_cal（BL616）
52/THIRD  pm_sys（LPAPP）
52/FOURTH bl_lp（LPAPP）
58/FIRST  wifi_events（WiFi4/FHOST 另注册 EV_WIFI 系统 filter）
70/FIRST  rfparam、wifi_start（同 order 按名字排序）
70/SECOND board_rf_btble（BL618DG 仅蓝牙镜像：板级 RF 路径选择；standalone 时第二次 _rfparam_init）
90/MIDDLE app_main
90/LAST   wifi_app_events（main() 返回后发布兼容事件）
```

---

## 5. 带兼容入口的接入模板

### 5.1 纯新增（无旧公开入口）

```c
#include <stddef.h>
#include <stdlib.h>
#include "sysinit.h"

static void foo_sysinit(const void *arg)
{
    (void)arg;

    if (foo_init_real() != 0) {
        abort();
    }
}
SYSINIT(foo, SI_SUB_DRIVERS, SI_ORDER_LAST, foo_sysinit, NULL);
```

要点：

- `SYSINIT` **紧跟回调定义**，不集中放到文件尾；
- 回调签名固定 `void (*)(const void *)`，首行 `(void)arg;`；
- 只注册确实需要的记录；条件编译用 `#if IS_ENABLED(CONFIG_FOO)` 包住整个回调和记录；
- 必需初始化失败 `abort()`；可选能力失败记录日志并继续；
- 删除原调用点，不能保留“会再次初始化”的调用。

### 5.2 有旧公开入口（deprecated 兼容层）

真实实现改名 `_<原函数名>()`，公开入口只做兼容。模板：

```c
/* foo.h */
/* 外部子构建也包含该头文件时，用 defined 守卫而不是 IS_ENABLED。 */
#if defined(CONFIG_COMPAT_V2) && CONFIG_COMPAT_V2
int foo_init(void) __attribute__((deprecated("handled by SYSINIT; will be removed in Bouffalo SDK v3")));
#endif /* defined(CONFIG_COMPAT_V2) && CONFIG_COMPAT_V2 */

/* 仅当 Boot2 / LPFW / MFG 等专用镜像需要显式初始化时才声明；普通应用不需要。 */
int _foo_init(void);
```

```c
/* foo.c */
int _foo_init(void)
{
    /* 真实初始化，保留原有返回值/副作用 */
    return 0;
}

#if IS_ENABLED(CONFIG_COMPAT_V2)
int foo_init(void)
{
    printf("[W] deprecated: %s() is handled by SYSINIT, will be removed in Bouffalo SDK v3\r\n",
           __func__);
    return 0; /* 幂等/兼容返回；不能重复执行初始化 */
}
#endif /* IS_ENABLED(CONFIG_COMPAT_V2) */

static void foo_sysinit(const void *arg)
{
    (void)arg;

    if (_foo_init() != 0) {
        abort();
    }
}
SYSINIT(foo, SI_SUB_DRIVERS, SI_ORDER_LAST, foo_sysinit, NULL);
```

### 5.3 兼容层规则

1. **命名**：真实实现使用 `_<原公开函数名>`（如 `_tcpip_init`、`_lfs_kv_init`）；
   不再新增 `*_internal` 后缀。公开旧入口只保留兼容语义。
2. **编译期属性**：公开头文件声明加
   `__attribute__((deprecated("handled by SYSINIT; will be removed in Bouffalo SDK v3")))`；
   其它组件已有特殊消息时保持同风格。
3. **运行时打印**：统一
   `[W] deprecated: <func>() is handled by SYSINIT, will be removed in Bouffalo SDK v3`，
   使用 `__func__`，`printf` 结尾 `\r\n`。
4. **行为**：
   - SYSINIT 已接管时，公开入口不得再次初始化，只返回兼容结果/保留旧回调语义；
   - 需要“未初始化时补做”的接口（如 `tcpip_init()`）在内部判断状态后复用真实实现；
   - 不能靠幂等标志掩盖重复调用，必须保证只有一个初始化发起位置。
5. **`CONFIG_COMPAT_V2` 守卫**：兼容声明、定义、打印、事件码、EasyFlash 兼容层全部用
   `#if IS_ENABLED(CONFIG_COMPAT_V2)`（SDK 内部源文件）或
   `#if defined(CONFIG_COMPAT_V2) && CONFIG_COMPAT_V2`（可能被外部子构建包含的头文件）包住。
6. **strict 构建**：弃用告警不是错误；`cmake/toolchain.cmake` 已加
   `-Wno-error=deprecated-declarations`，不需要在各文件里再 `#pragma` 抑制。
7. **事件 tag**：不能用将被删除的 deprecated 符号作为 `EV_*` tag；
   WL80211 的 `EV_WIFI` 使用稳定的 `wifi_mgmr_sta_connect` 地址。
8. **`vTaskStartScheduler()` 特殊处理**：不加编译期 deprecated；使用
   `-Wl,--wrap,vTaskStartScheduler` + `setjmp`/`longjmp`（`components/sysinit/sysinit_freertos.c`），
   内核 `tasks.c`/`task.h` 不改。
9. **外部独立子构建**（不 include `autoconfig.h`，例如 `wl80211/src`、`wifi6/fhost`）：
   `library.mk` 把 `CONFIG_COMPAT_V2` 的 y/n 值透传给它们的 CMake，嵌套 CMake 仅在 y 时
   定义 `-DCONFIG_COMPAT_V2=1`；wl80211 公共头使用
   `#if defined(CONFIG_COMPAT_V2) && CONFIG_COMPAT_V2` 形式，wifi6/fhost 依赖 `macsw.h`
   中已有的 `IS_ENABLED`。

### 5.4 `CONFIG_SYSINIT=n` 规则

- **关闭 SYSINIT 框架在常规 app 下不被允许。** 保留 `CONFIG_SYSINIT` 开关仅用于兼容
  MFG、LPFW、Boot2 三类特殊 app（它们拥有独立启动流程），以及自带独立 port 启动流程的
  Eflash Loader RAM 镜像（`examples/eflash_loader` defconfig 显式 `CONFIG_SYSINIT=n`；
  该镜像自带 clock/NP-LP halt/flash 探测且不使用 SDK 堆）。Boot2 由 `CONFIG_BOOT2`
  默认 `n`，MFG/LPFW 当前按 examples 恢复策略保持默认 `y`。普通应用必须保持
  `CONFIG_SYSINIT=y`，不得通过关闭 SYSINIT 绕过初始化。
- **迁移前先判断这些专用固件是否依赖目标函数：** 没有依赖时，不需要考虑该函数在
  `CONFIG_SYSINIT=n` 下的调用路径，不要为此增加 fallback、条件初始化或额外兼容分支；
  有依赖时，只在对应特殊 app 中显式调用真实初始化入口。
- `CONFIG_SYSINIT=n` 时记录不注册/不执行；已确认特殊 app 依赖该函数时，专用镜像显式调用
  `_foo_init()`（全局私有入口），公开 deprecated 入口保持纯兼容，不“偷偷初始化”。
- 不要在普通应用兼容入口里加“检测到 SYSINIT 关闭就补做初始化”的 fallback；
  这是专用固件的责任。
- `CONFIG_SYSINIT=n` 构建仍需能链接：`_foo_init()` 的定义不能依赖 `CONFIG_SYSINIT` 条件编译，
  除非同文件内没有引用者。

### 5.5 自定义板级例外

只有 `CONFIG_SYSINIT_CUSTOM_BOARD_INIT=y` 时，工程 `board*.c` 可以注册**替代默认板级所需**
的硬件流程；不能注册应用文件系统、RF、WiFi、事件订阅或业务启动。模板见迁移规范
「SDK 组件与板级接入模板」。

自定义板级仍必须让系统 heap 先于共享 `cxx_runtime` 记录（默认 BSP 的 `board_heap` 在
`SI_SUB_DRIVERS`/`SI_ORDER_SECOND`，`cxx_runtime` 在 `SI_ORDER_MIDDLE`）：C++ 全局构造函数
里的 `new`/`malloc` 依赖已注册的 heap。若自定义板级把 heap 注册放到更晚的 order，必须同时
把该记录移到 heap 之后，或按默认约定保持 heap 在 `SI_ORDER_SECOND`。

---

## 6. 应用侧规则（强制）

1. 普通应用、`examples`、AT 内置示例和应用辅助文件**禁止** `#include "sysinit.h"`、
   禁止 `SYSINIT(...)`，禁止把注册搬到别的应用文件或用宏包装。
2. 事件订阅在 `main()` 中、调度器启动前完成；业务任务从普通 `main()`/任务入口创建。
3. 应用不得调用 SDK 已接管的固定初始化入口（`board_init()`、`bflb_mtd_init()`、
   `bflb_boot2_init()`、`rfparam_init()`、`lfs_kv_init()`、`wifi_task_create()`、
   `fhost_init()`、`bflb_cxx_runtime_init()` 等），
   也不得手工调用 `sysinit_run_pre_schedule()`。
4. 应用不再调用 `vTaskStartScheduler()`；SYSINIT 构建中的旧调用由 wrap 兼容入口吸收，
   示例清理时删除。
5. 应用层可选模块（AT 等）在自己的启动函数中注册事件；不占 `SI_SUB_WIRELESS`。
   案例：v1.0 wl80211 AT 的 `at_wifi_start()` 注册 `EV_WIFI` filter；`APP/LAST` 的优先级
   让出保证应用任务先完成注册，再收到兼容事件。
6. 应用自己的媒体/镜像挂载、策略、配网、业务任务保持普通调用；SYSINIT 只做 SDK 固定初始化。

> **迁移过渡期**：恢复版 examples 仍保留对 `board_init()` 等兼容入口的旧调用；这些调用只产生
> 统一弃用告警、不重新执行初始化，不属于应用初始化依赖，后续随 examples 清理删除。

---

## 7. 验收 checklist

### 代码

- [ ] 记录只出现在 SDK 组件/板级源码，应用文件无 `sysinit.h`/`SYSINIT`；
- [ ] 回调签名、`(void)arg;`、`SYSINIT` 紧跟回调、记录名唯一；
- [ ] subsystem/order 选择有注释说明依赖；pre-sched 记录无阻塞；
- [ ] 真实初始化只有一个发起位置；旧调用点已删除或改为兼容入口；
- [ ] 公开头文件有 deprecated 属性；定义处打印统一提示；兼容行为不重复初始化；
- [ ] `CONFIG_COMPAT_V2` 守卫完整（声明、定义、打印、EasyFlash 兼容层等）；
- [ ] 已确认 MFG/LPFW/Boot2（及显式 `CONFIG_SYSINIT=n` 的 Eflash Loader）是否依赖被迁移
      函数；无依赖时未新增 `CONFIG_SYSINIT=n` 分支/fallback，有依赖时只在对应特殊 app 中
      显式调用 `_foo_init()`；
- [ ] 已确认有依赖的专用固件 `CONFIG_SYSINIT=n` 路径使用 `_foo_init()`，普通应用无 fallback；
- [ ] 外部子构建若包含相关头文件，已透传 `CONFIG_COMPAT_V2`；公开头使用
      `defined() && CONFIG_COMPAT_V2` 形式，或依赖 `macsw.h` 的 `IS_ENABLED`。

### 构建

- [ ] `CONFIG_COMPAT_V2=y` 构建通过；
- [ ] **至少一个已清理工程/组件**在 `CONFIG_COMPAT_V2=n` 下构建通过（旧公开入口消失、内部
      入口仍链接）。仍调用旧入口的恢复版 examples 在 `=n` 下会如预期链接失败，需随应用清理
      删除旧调用后满足；该验收不要求恢复版 examples 在 `=n` 下可构建。
- [ ] `CONFIG_STRICT=y` 构建通过，deprecated 告警不升级为错误；
- [ ] 普通应用 `CONFIG_SYSINIT=y`；Boot2 默认 `n`，Eflash Loader RAM 镜像显式 `n`；
      MFG/LPFW 按当前恢复策略保持默认 `y`（其独立启动归属要求见迁移规范），不以“三类
      专用镜像统一为 n”作为验收条件；
- [ ] 旧调用点产生预期 `-Wdeprecated-declarations`，消息含
      `will be removed in Bouffalo SDK v3`。

### 产物检查

- [ ] ELF 中 `_sysinit_*` 记录名称/顺序符合预期：
      `nm -n` 或按 section 名排序检查 subsystem/order；
- [ ] pre-sched 记录没有依赖任务的调用（代码审查/日志）；
- [ ] `CONFIG_SYSINIT=n` 产物没有 `_sysinit_*` 记录和分隔符符号；
- [ ] `CONFIG_COMPAT_V2=n` 产物没有旧兼容入口/事件码/`__wrap_vTaskStartScheduler`
      （2026-11-11 审计：FHOST/WL80211 的 `wifi_app_events` 记录与三个后端的兼容事件码尚未加
      guard，见审计报告 V2–V4，修复前该项不通过）；
- [ ] 首次启动只有一份初始化日志/一次任务创建，没有重复初始化。

### 文档

- [ ] 新增/调整 subsystem、order 或兼容规则时，同步更新本指南、迁移规范、
      `components/sysinit/sysinit.h` 注释；
- [ ] 记录涉及的 `CONFIG_*` 开关和默认值；
- [ ] 如有特殊时序（优先级窗口、事件发布位置），在记录注释和文档中说明原因。

---

## 8. 常见错误

| 错误 | 后果 | 正确做法 |
|---|---|---|
| 把应用组件注册在 `SI_SUB_WIRELESS` | 分层错误，WiFi 启动序列包含可选应用 | 应用层在 `main()`/自己的 `*_start()` 处理 |
| 依赖源码/链接顺序 | 换链接器/改文件名后顺序变化 | 用不同 `SI_ORDER_*` 显式排序 |
| 依赖 heap 的初始化排在 `board_heap` 之前 | 构造函数的 `new`/`malloc` 拿不到内存 | 用更晚的 order（默认 `board_heap` 40.01、`cxx_runtime` 40.80） |
| pre-sched 记录里 `vTaskDelay()`/等待事件 | 调度器未启动，挂死或断言 | 移到 `SI_SUB_EVENTS` 之后 |
| 兼容入口里做真实初始化 | 双重初始化/重复创建任务 | 公开入口只打印并返回兼容结果 |
| 普通应用加 `CONFIG_SYSINIT=n` fallback | 隐蔽的补偿初始化，违反唯一发起位置 | 普通应用保持 `SYSINIT=y`；专用固件调用 `_foo_init()` |
| 为 MFG/LPFW/Boot2 都不引用的函数增加 SYSINIT=n 分支 | 不必要的兼容代码和测试负担 | 先确认特殊 app 依赖；无依赖就不设计关闭 SYSINIT 的调用路径 |
| 用 deprecated 符号做事件 tag | 每次发事件都产生弃用告警 | 使用稳定公开符号（如 `wifi_mgmr_sta_connect`） |
| 只改 `.c` 不加头文件属性/打印 | 应用调用无告警，难以清理 | 声明 + 定义 + 打印三处齐全 |
| 新 subsystem 未更新文档/`sysinit.h` | 后续维护者无法判断顺序 | 同步更新设计文档和注释 |
| 把 `APP/LAST` 事件订阅放到 `main()` 之后的任务 | 错过一次性 INIT/MGMR 事件 | 在 `main()` 注册，或依赖 `APP/LAST` 优先级窗口先让出应用任务 |

---

## 9. 新增 subsystem / order 的流程

1. 确认现有阶段无法表达需求，且有明确的全局依赖；
2. 在 `components/sysinit/sysinit.h` 增加两位十六进制常量，保持排序和间隔；
3. 更新 [框架设计](2026-04-29-sysinit-framework-design.md)、本指南和迁移规范；
4. 链接脚本使用 `SORT_BY_NAME(.sysinit.*)` 通配，一般无需按板修改；新增后检查所有
   目标板 ELF 记录顺序；
5. 验证 pre/post-scheduler 约束、`CONFIG_SYSINIT=n`、`CONFIG_COMPAT_V2=y/n` 构建。

---

## 10. 参考

- [迁移规范](2026-11-04-sysinit-migration.md)：强制约定、启动契约、历史验证记录
- [框架设计](2026-04-29-sysinit-framework-design.md)：subsystem/order 定义、注册宏、调度
- [SYSINIT_ARCHITECTURE.md](SYSINIT_ARCHITECTURE.md)：FreeBSD SYSINIT 原理（参考）
- `components/sysinit/sysinit.h`、`components/sysinit/sysinit.c`、`components/sysinit/sysinit_freertos.c`
- `cmake/toolchain.cmake`（`CONFIG_STRICT` 豁免）、`tools/kconfig/config_seed.py`（`IS_ENABLED`/`CONFIG_COMPAT_V2` 生成）
