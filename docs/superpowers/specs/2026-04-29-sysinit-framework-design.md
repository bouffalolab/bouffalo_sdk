# Bouffalo SDK SYSINIT Framework Design

> **Date**: 2026-04-29 (framework v1); scheduler barrier added 2026-11-06, post-scheduler order finalized 2026-11-09, sync with the current implementation 2026-11-11
> **Status**: Approved (framework v1). Current behavior is defined by the [migration spec](2026-11-04-sysinit-migration.md) and the [access guide](sysinit-migration-guide.md).
> **Scope**: all supported chips (BL602 / BL702 / BL702L / BL616 / BL616CL / BL618DG)

> **文档状态（2026-11-11 更新）**：本文最初是 v1 框架设计；调度器分隔符、post-scheduler 阶段、
> APP/LAST 兼容事件、续跑任务优先级窗口和 vTaskStartScheduler 兼容入口已按当前实现修正（见 §9）。
> 已知未修复的代码问题见 [SYSINIT 迁移合规性审计报告](../../../SYSINIT_MIGRATION_AUDIT_2026-11-11.md)。

## 1. Overview

Implement a FreeBSD-style SYSINIT declarative initialization framework for the Bouffalo SDK. Each component registers its init function with a subsystem and order priority via a linker section. A pre-scheduler iteration runs the fixed SDK initialization before the scheduler starts; a scheduler barrier then creates a continuation task that runs the remaining records in task context, including the `APP/MIDDLE` record that enters the application `main()`. There is no runtime sorting and no RAM overhead for ordering.


### Core Principles
- **Declarative**: Any `.c` file registers init via `SYSINIT()` macro — no central scheduler changes needed
- **Zero-RAM sorting**: Linker `SORT_BY_NAME` on section names with embedded priority digits handles ordering at link time
- **Zero runtime overhead**: Simple array iteration, no dynamic allocation, no sorting
- **Two-level priority**: 8-bit subsystem (0x00-0xFF) + 8-bit order (0x00-0xFF), 256 × 256 levels

## 2. Architecture

### Boot Flow
```
__start
  → SystemInit()              (caches, PMP, CLIC)
  → start_load()              (copy .data, zero .bss)
  → System_Post_Init()
      → SoC trim/DCDC setup          (before the table: board clock/RTC records depend on it)
      → sysinit_run_pre_schedule()   ← iterate sorted .sysinit.array
          → records 00..52           (board, services, SoC global IRQ enable)
          → SI_SUB_SCHED barrier: create continuation task, start FreeRTOS, never returns
      (continuation task, post-scheduler records)
          → POST_SCHED/EVENTS/WIRELESS   (async event loop, RF/WiFi bring-up)
          → APP/MIDDLE: enter application main()
          → APP/LAST: application-facing INIT_DONE/MGMR_DONE compat events
  → application tasks run
```
Without a scheduler barrier (`CONFIG_SYSINIT=n` or `CONFIG_FREERTOS=n`),
`sysinit_run_pre_schedule()` returns and the startup assembly calls `main()`
directly, as before.

### Section Name Scheme
```
.sysinit.<sub_hex2>.<ord_hex2>.<name>
         ^^         ^^
    2-digit hex   2-digit hex
    zero-padded   zero-padded

Examples:
  .sysinit.10.00.rf_calib        → SI_SUB_HW, SI_ORDER_FIRST
  .sysinit.20.00.sys_clock       → SI_SUB_CLOCK, SI_ORDER_FIRST
  .sysinit.40.01.uart_early      → SI_SUB_DRIVERS, SI_ORDER_SECOND
  .sysinit.70.00.wifi_start      → SI_SUB_WIRELESS, SI_ORDER_FIRST
  .sysinit.90.ff.wifi_app_events → SI_SUB_APP, SI_ORDER_LAST
```

ASCII lexicographic sort of fixed-width hex equals numerical sort, so `SORT_BY_NAME(.sysinit.*)` in the linker produces the correct execution order.

## 3. Files

| File | Purpose |
|------|---------|
| `components/sysinit/sysinit.h` | SYSINIT macro, struct, subsystem/order defines, runtime API |
| `components/sysinit/sysinit.c` | pre-scheduler iteration and the resume position used by the continuation task |
| `components/sysinit/sysinit_freertos.c` | scheduler barrier, continuation task, `APP/MIDDLE` main() record, `vTaskStartScheduler()` compatibility wrapper |
| `components/sysinit/CMakeLists.txt` | build integration; adds `-Wl,--wrap,vTaskStartScheduler` when `CONFIG_SYSINIT && CONFIG_COMPAT_V2` |
| `components/sysinit/Kconfig` | `CONFIG_SYSINIT_TASK_STACK_DEPTH` (continuation task stack) |
| `bsp/board/*/*.ld` and `*.ld.in` | `.sysinit.array` section with `KEEP(*(SORT_BY_NAME(.sysinit.*)))` |
| `drivers/soc/*/std/startup/system_*.c` | call `sysinit_run_pre_schedule()` from `System_Post_Init()` |

## 4. API Design

### Data Structure
```c
struct sysinit_item {
    void       (*func)(const void *arg);
    const void  *arg;
    const char  *name;        // debug identifier
};
```

Subsystem and order values are **not** stored in the struct — they exist only in the section name, saving 2+ bytes per item.

### Subsystem IDs (si_sub)
```c
#define SI_SUB_LOWEST         "00"
#define SI_SUB_HW             "10"   // CPU/memory/security init
#define SI_SUB_CLOCK          "20"   // System & peripheral clocks
#define SI_SUB_INTR           "30"   // Interrupt controller init
#define SI_SUB_DRIVERS        "40"   // Peripheral drivers (UART, GPIO, SPI...) and board steps
#define SI_SUB_SERVICES       "50"   // Service subsystems (Flash MTD, LittleFS, FATFS, KV store)
#define SI_SUB_PRE_SCHED      "52"   // Pre-scheduler records (global IRQ enable, low power)
#define SI_SUB_SCHED          "55"   // Scheduler barrier (FreeRTOS integration only)
#define SI_SUB_POST_SCHED     "56"   // First post-scheduler phase (reserved)
#define SI_SUB_EVENTS         "58"   // Async event loop, before WIRELESS/APP
#define SI_SUB_WIRELESS       "70"   // RF/WiFi backend init, synchronous, before APP
#define SI_SUB_NET            "80"   // Reserved TCP/IP hand-off phase (no records today)
#define SI_SUB_APP            "90"   // Application main() (MIDDLE) and APP/LAST compat events
#define SI_SUB_HIGHEST        "ff"
```

### Order IDs (si_order)
```c
#define SI_ORDER_FIRST        "00"
#define SI_ORDER_SECOND       "01"
#define SI_ORDER_THIRD        "02"
#define SI_ORDER_FOURTH       "03"
#define SI_ORDER_FIFTH        "04"
#define SI_ORDER_MIDDLE       "80"
#define SI_ORDER_LAST         "ff"
```

### Registration Macro
```c
#define SYSINIT(_name, _sub, _ord, _fn, _arg)                              \
    static const struct sysinit_item _sysinit_##_name                      \
    __attribute__((section(".sysinit." _sub "." _ord "." #_name), used)) = {\
        .func = (_fn), .arg = (const void *)(_arg), .name = #_name,       \
    }
```

### Runtime API
```c
void sysinit_run_pre_schedule(void);   /* pre-scheduler part; does not return when a barrier exists */
void sysinit_run_post_schedule(void);  /* continuation task only: run entries after the barrier */
```

### Usage Example
```c
// In any .c file:
static void uart_early_init(const void *arg) {
    (void)arg;
    // init UART hardware...
}
SYSINIT(uart_early, SI_SUB_DRIVERS, SI_ORDER_FIRST, uart_early_init, NULL);
```

## 5. Linker Script

Add to the board linker script (or `*.ld.in` template), in the FLASH region, after `.text`:

```ld
    .sysinit.array :
    {
        __sysinit_array_start = .;
        KEEP(*(SORT_BY_NAME(.sysinit.*)))
        __sysinit_array_stop = .;
    } > FLASH
```

Boundary symbols `__sysinit_array_start` / `__sysinit_array_stop` provide iteration bounds.

## 6. Runtime Implementation

```c
// sysinit.c (simplified)
#include "sysinit.h"

extern const struct sysinit_item __sysinit_array_start[];
extern const struct sysinit_item __sysinit_array_stop[];

static const struct sysinit_item *sysinit_resume;

static void sysinit_run_items(const struct sysinit_item *p)
{
    for (; p < __sysinit_array_stop; p++) {
        if (p->func) {
            sysinit_resume = p + 1;   /* resume point for the continuation task */
            p->func(p->arg);
        }
    }
}

void sysinit_run_pre_schedule(void)
{
#ifdef CONFIG_SYSINIT
    sysinit_run_items(__sysinit_array_start);
#endif
}

void sysinit_run_post_schedule(void)
{
#ifdef CONFIG_SYSINIT
    const struct sysinit_item *p = sysinit_resume;

    if (p == NULL || p >= __sysinit_array_stop) {
        return;
    }
    sysinit_resume = NULL;
    sysinit_run_items(p);
#endif
}
```

## 7. Migration Strategy

- **Phase 1 (done)**: SYSINIT framework introduced; `board_init()` coexisted with the framework.
- **Phase 2 (done)**: board and fixed component initialization registered as SYSINIT records, including the scheduler barrier and the post-scheduler phases.
- **Phase 3 (in progress)**: old public initialization entries are kept as deprecated compatibility wrappers until Bouffalo SDK v3; applications/examples are cleaned up gradually. `CONFIG_COMPAT_V2=n` removes the wrappers for v3 builds.

## 8. Non-Goals
- No runtime sorting (zero RAM overhead)
- No dynamic module loading (MCU has no kldload equivalent)
- No SYSUNINIT (no module unloading on MCU)
- No multi-core-specific SYSINIT mechanism: BL618DG AP/NP/LP each build their own record set and rely on the existing board/SoC startup ownership rules
- No application-level SYSINIT registration: applications create business tasks and subscribe to events from `main()`; the only exception is `CONFIG_SYSINIT_CUSTOM_BOARD_INIT` project board code

## 9. Scheduler barrier (2026-11-06; corrected 2026-11-11 to match the current implementation)

The table is split by a scheduler barrier. `sysinit_run_pre_schedule()` still iterates the
sorted records from `__sysinit_array_start`, but when it reaches the record in
`SI_SUB_SCHED` the FreeRTOS integration
(`components/sysinit/sysinit_freertos.c`) creates the continuation task and calls
`vTaskStartScheduler()`, which does not return. The continuation task executes
the remaining records in task context through `sysinit_run_post_schedule()`.
The application `main()` is one of those records (`SI_SUB_APP`), so it is no
longer called directly after the table walk.

### Added priorities

| subsystem | content |
|---|---|
| `SI_SUB_PRE_SCHED` (`52`) | Pre-scheduler records that must run after the service records (global IRQ enable; RC32K/RC32M trim and DCDC setup run earlier in `System_Post_Init()` so the board clock/RTC records see trimmed clocks) |
| `SI_SUB_SCHED` (`55`) | Scheduler separator, registered only by the FreeRTOS integration |
| `SI_SUB_POST_SCHED` (`56`) | First post-scheduler phase (continuation task); reserved for common records before the component phases |
| `SI_SUB_EVENTS` (`58`) | Async event loop initialization, before WIRELESS/APP |
| `SI_SUB_WIRELESS` (`70`) | RF / Wi-Fi backend bring-up, synchronous, before APP |
| `SI_SUB_NET` (`80`) | Reserved TCP/IP hand-off phase; no records today. TCP/IP is initialized by the `WIRELESS` backend records, and the application-facing compatibility events are posted by `APP/LAST` after `main()` returns |
| `SI_SUB_APP` (`90`) | Application `main()` at `MIDDLE`; `APP/LAST` records post the application-facing `INIT_DONE`/`MGMR_DONE` compatibility events after `main()` returns |

Entries sorted after `55` run in the continuation task in the order
`POST_SCHED -> EVENTS -> WIRELESS -> APP (main)`. The async event loop is ready before
the bring-up records; the Wi-Fi backend completes synchronously, including its
internal firmware/manager readiness waits (`*_SYS` events), so `main()` starts
with a usable network/Wi-Fi stack. `SI_SUB_NET` is reserved and has no records.
Each backend registers an `APP/LAST` record that posts the application-facing
`INIT_DONE`/`MGMR_DONE` compatibility events exactly once after `main()` returns,
isolated from the internal `*_SYS` event codes.

### Runtime API
```c
void sysinit_run_pre_schedule(void);       /* pre-scheduler part; does not return when a barrier exists */
void sysinit_run_post_schedule(void); /* continuation task only: run entries after the barrier */
```

### Configuration
`CONFIG_SYSINIT_TASK_STACK_DEPTH` (default 2048 words) selects the continuation
task stack. The task runs at `configMAX_PRIORITIES - 1` so that tasks created by
SYSINIT entries and by `main()` do not preempt the initialization flow; the
Wi-Fi backends temporarily lower its priority (priority window) while the
firmware task is created, then restore it.

### Fallbacks
- `CONFIG_SYSINIT=n`: the table is empty, no barrier is registered,
  `sysinit_run_pre_schedule()` returns and the startup assembly calls `main()` as before.
- `CONFIG_FREERTOS=n`: the barrier integration is not compiled, all records run
  inline in `System_Post_Init()` and the startup assembly calls `main()`.

### Startup assembly and main()
`start.S` still calls `main` after `System_Post_Init()`. With a barrier that call
is unreachable; the continuation task calls `main()` through the `SI_SUB_APP`
record instead. The call is kept so non-barrier images are unchanged.
There is no `-Wl,--wrap,main`: the shared C++ runtime component registers a
`cxx_runtime` record (`SI_SUB_DRIVERS`/`SI_ORDER_MIDDLE`) that runs
`__libc_init_array()` once, after the board console/heap records and before the
scheduler barrier. The old public `bflb_cxx_runtime_init()` is a deprecated
compatibility wrapper under `CONFIG_COMPAT_V2`; applications must not call it.

### Consequences
- Post-scheduler SYSINIT entries may block and call `vTaskDelay()`; pre-barrier
  entries only run the synchronous hardware setup.
- `main()` now runs in a task and must not call `vTaskStartScheduler()` again.
  The call is not compile-time deprecated; it is intercepted at link time via
  `-Wl,--wrap,vTaskStartScheduler` (wrapper in `sysinit_freertos.c` prints a
  warning, then `setjmp`/`longjmp` back to the `SI_SUB_APP` record, skipping the
  rest of `main()`) so SYSINIT can continue. FreeRTOS examples still carry the
  old call and need a follow-up cleanup.
- The Wi-Fi backend runs synchronously in the continuation task before
  `main()`. The final implementation does not create a private startup task:
  it temporarily lowers the continuation task priority before the firmware
  task is created (priority window) and restores it afterwards, so
  `INCLUDE_vTaskPrioritySet` must be 1 (the platform files have `#error`
  checks).
- Internal readiness uses isolated `CODE_WIFI_ON_*_SYS` event codes; the
  application-facing `CODE_WIFI_ON_INIT_DONE`/`CODE_WIFI_ON_MGMR_DONE` events
  are posted once from the `APP/LAST` record after `main()` returns and are
  dispatched after the application `main()` has registered its filters.
- `APP/LAST` records only run when `main()` returns. Legacy `main()`
  implementations that end with `vTaskStartScheduler()` are intercepted by the
  link-time wrapper (`longjmp` back to the `APP` record), so `APP/LAST` still
  runs; a `main()` with an unconditional `while (1)` would prevent the
  application-facing compatibility events.
