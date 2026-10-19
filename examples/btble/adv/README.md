# BLE Beacon Demo

> **Chip support: BL616CL only.**
> This demo requires `CONFIG_BLE_BEACON_ONLY=y`, which is only supported on the BL616CL (BLE-only variant). It does **not** run on BL616/BL618.

---

## Overview

A BLE iBeacon demo that demonstrates the full BLE beacon lifecycle using the `CONFIG_BLE_BEACON_ONLY` direct controller API. This API bypasses the HCI transport layer and calls controller handlers directly, significantly reducing memory footprint compared to the standard BLE host stack.

The demo runs **3 lifecycle cycles** to validate init/deinit symmetry and memory recoverability:

```
[ctrl_init] → [adv_init] → [set_params] → [set_data]
    → [adv_start] → (ADV 3s) → [adv_stop]
    → [adv_deinit] → [ctrl_deinit]
```

Each cycle broadcasts an **iBeacon-format non-connectable undirected advertisement** (ADV_NONCONN_IND) for 3 seconds, then tears down the controller completely. Heap usage and EM remaining space are reported at each step.

---

## Features

- **Non-connectable beacon** (ADV_NONCONN_IND, iBeacon format)
- **Direct controller API** — no HCI transport, no GATT/GAP host stack
- **Full lifecycle cycle test** — 3 cycles of init → advertise → deinit
- **Memory reporting** — heap delta and EM remaining after each step
- **Zero cyclic heap leak** — init/deinit fully reversible (244 + 88 bytes recovered each cycle)

---

## Memory Footprint (BL616CL)

| Dimension | Value | Status |
|-----------|-------|--------|
| Static RAM (btblecontroller) | 661 bytes | Very low |
| Dynamic Heap (ctrl + adv, per cycle) | 332 bytes | Very low |
| One-time init extra alloc | 32 bytes | Acceptable |
| EM available (after fixed structures) | 8,956 bytes | Ample |
| EM used by BEACON_ONLY allocations | 4,960 bytes | Reasonable |
| EM remaining after ctrl_init | 3,996 bytes | Ample |
| Cyclic memory leak | 0 bytes | No leak |

> EM (Exchange Memory) is a dedicated 32 KB SRAM region used for BLE hardware DMA structures.
> In BEACON_ONLY mode, FreeRTOS task stack (2 KB), TCB, and key controller structs are placed
> in EM instead of the system heap, achieving near-zero heap consumption.

---

## Building

**Prerequisite:** Ensure the RISC-V cross-compiler is installed and `CROSS_COMPILE` is set, or use the toolchain provided with the SDK. Refer to the SDK setup guide for details.

```bash
make CHIP=bl616cl BOARD=bl616cldk
```

Clean build:
```bash
make clean && make CHIP=bl616cl BOARD=bl616cldk
```

---

## Flashing

```bash
make flash CHIP=bl616cl COMX=/dev/ttyACM1
```

---

## Beacon Configuration

Edit `main.c`:

```c
#define BEACON_COMPANY_ID    0x004C  /* Apple, Inc. */
#define BEACON_TYPE          0x0215  /* iBeacon type */
#define BEACON_MAJOR         0x0001
#define BEACON_MINOR         0x0002
#define BEACON_TX_POWER      0xC5    /* -59 dBm */

static const uint8_t beacon_uuid[16] = {
    0x01, 0x12, 0x23, 0x34, 0x45, 0x56, 0x67, 0x78,
    0x89, 0x9A, 0xAB, 0xBC, 0xCD, 0xDE, 0xEF, 0xF0
};
```

Advertising interval (in `beacon_cycle_task`):
```c
params.adv_interval_min = 0x00A0;  /* 100 ms (unit: 0.625 ms) */
params.adv_interval_max = 0x00F0;  /* 150 ms */
```

---

## Expected Output

```
========================================
  BLE Beacon Lifecycle Cycle Test
  Sequence: ctrl_init -> adv_init -> set_params
            -> set_data -> adv_start -> adv_stop
            -> adv_deinit -> ctrl_deinit
  Cycles: 3, ADV duration: 3000ms
========================================

[BASELINE] heap free: 331412 bytes

--- Cycle 1/3 ---
  [ctrl_init]  heap used: 276  free: 331136
  [EM]         remaining: 3996 bytes
  [adv_init]   err=0  heap used: 88  free: 331048
  [set_params] err=0
  [set_data]   err=0  len=31
  [adv_start]  err=0  -> OK
  [adv_stop]   err=0
  [adv_deinit] heap recovered: 88  free: 331136
  [ctrl_deinit] heap recovered: 244  free: 331380
  [CHECK] heap leak: 32 bytes (baseline=331412, now=331380)

--- Cycle 2/3 ---
  [ctrl_init]  heap used: 244  free: 331136
  [EM]         remaining: 3996 bytes
  ...
  [CHECK] heap leak: 32 bytes (baseline=331412, now=331380)

--- Cycle 3/3 ---
  ...
========================================
  Beacon lifecycle test DONE (3 cycles)
  Final heap free: 331380 bytes
========================================
```

The 32-byte difference shown in `[CHECK]` is a one-time allocation on the first `ctrl_init`. It does not grow across cycles — heap usage between Cycle 2 and Cycle 3 is identical, confirming **zero cyclic memory leak**.

---

## Scanning

The beacon can be detected by any BLE scanner app:

- **Android**: nRF Connect, LightBlue
- **iOS**: nRF Connect, LightBlue, Beacon Scope

Advertisement appearance:
- **Type**: Non-connectable (ADV_NONCONN_IND)
- **Manufacturer Data**: 0x004C (Apple iBeacon format)
- **No device name** (non-connectable advertisers do not respond to scan requests)

