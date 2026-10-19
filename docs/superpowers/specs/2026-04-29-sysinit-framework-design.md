# Bouffalo SDK SYSINIT Framework Design

> **Date**: 2026-04-29  
> **Status**: Approved  
> **Scope**: BL616CL first, other chips follow  

## 1. Overview

Implement a FreeBSD-style SYSINIT declarative initialization framework for the Bouffalo SDK. Each component registers its init function with a subsystem and order priority via a linker section, and the framework executes all items in sorted order before `main()` without any runtime sorting or RAM overhead.

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
      → sysinit_run_all()     ← NEW: iterate sorted .sysinit.array
      → [existing BOD/xtal code]
  → main()
      → board_init()          (coexists, to be gradually migrated)
      → task creation / scheduler start
```

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
  .sysinit.70.80.wifi_controller → SI_SUB_WIRELESS, SI_ORDER_MIDDLE
```

ASCII lexicographic sort of fixed-width hex equals numerical sort, so `SORT_BY_NAME(.sysinit.*)` in the linker produces the correct execution order.

## 3. Files

| File | Purpose |
|------|---------|
| `components/sysinit/sysinit.h` | SYSINIT macro, struct, subsystem/order defines, runtime API |
| `components/sysinit/sysinit.c` | `sysinit_run_all()` implementation |
| `components/sysinit/CMakeLists.txt` | Build integration |
| `bsp/board/bl616cldk/bl616cl_common.ld.in` | Linker script: `.sysinit.array` section |
| `drivers/soc/bl616cl/std/startup/system_bl616cl.c` | Call `sysinit_run_all()` in `System_Post_Init()` |

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
#define SI_SUB_DRIVERS        "40"   // Peripheral drivers (UART, GPIO, SPI...)
#define SI_SUB_FS             "50"   // Filesystem (Flash MTD, LittleFS, FATFS)
#define SI_SUB_NET            "60"   // Network stack (lwIP)
#define SI_SUB_WIRELESS       "70"   // WiFi/BLE controller init
#define SI_SUB_APP            "80"   // Application-level init
#define SI_SUB_HIGHEST        "ff"
```

### Order IDs (si_order)
```c
#define SI_ORDER_FIRST        "00"
#define SI_ORDER_SECOND       "01"
#define SI_ORDER_THIRD        "02"
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
void sysinit_run_all(void);  // called once in System_Post_Init()
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

Add to `bl616cl_common.ld.in`, in the FLASH region, after `.text`:

```ld
    .sysinit.array :
    {
        PROVIDE(__sysinit_array_start = .);
        KEEP(*(SORT_BY_NAME(.sysinit.*)))
        PROVIDE(__sysinit_array_stop = .);
    } > FLASH
```

Boundary symbols `__sysinit_array_start` / `__sysinit_array_stop` provide iteration bounds.

## 6. Runtime Implementation

```c
// sysinit.c
#include "sysinit.h"

extern const struct sysinit_item __sysinit_array_start[];
extern const struct sysinit_item __sysinit_array_stop[];

void sysinit_run_all(void)
{
    const struct sysinit_item *p;
    for (p = __sysinit_array_start; p < __sysinit_array_stop; p++) {
        if (p->func) {
            p->func(p->arg);
        }
    }
}
```

## 7. Migration Strategy

- **Phase 1 (this implementation)**: SYSINIT framework in place. `board_init()` coexists.
- **Phase 2 (future)**: Gradually convert `board_init()` internals to individual `SYSINIT()` registrations.
- **Phase 3 (future)**: Remove `board_init()` when fully migrated. Deploy to all chip families.

## 8. Non-Goals
- No runtime sorting (zero RAM overhead)
- No dynamic module loading (MCU has no kldload equivalent)
- No SYSUNINIT (no module unloading on MCU)
- No multi-core support in v1
- Other chip families deferred (BL616CL only)
