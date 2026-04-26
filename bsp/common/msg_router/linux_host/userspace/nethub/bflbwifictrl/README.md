# bflbwifictrl / libbflbwifi Guide

This document describes the responsibilities of the
`bsp/common/msg_router/linux_host/userspace/nethub/bflbwifictrl`
subdirectory.

If you want the full host-side bring-up flow and overall architecture,
read:

- `../README.md`
- `../HOST_ARCHITECTURE.md`

## 1. Scope

This subdirectory belongs to the optional AT-style host control stack.
It is not required for NetHub data-path-only products.

Important boundary:

- `nethub` core does not directly depend on `ATModule`
- this host stack is relevant when the example composition enables the
  AT control solution

## 2. Subdirectory Responsibilities

`bflbwifictrl/` contains three main parts:

- `app/`
  - `bflbwifid` daemon
  - `bflbwifictrl` CLI
  - local IPC and command dispatch helpers
- `include/`
  - public headers for `libbflbwifi`
- `src/`
  - internal implementation of the Wi-Fi control library

## 3. External Capability Surface

Public CLI and daemon commands:

- `connect_ap`
- `disconnect`
- `scan`
- `status`
- `version`
- `reboot`
- `ota`
- `start_ap`
- `stop_ap`

Public C API header:

- `include/bflbwifi.h`

Common initialization flow:

```c
bflbwifi_ctrl_config_t cfg;

bflbwifi_ctrl_config_init(&cfg);
bflbwifi_ctrl_config_use_vchan(&cfg);
bflbwifi_init_ex(&cfg);
```

Notes about backend coverage:

- `tty` and `vchan` backends are both built in this host stack
- current in-tree `vchan` backend is still aligned with the SDIO
  path
- do not describe `bflbwifi_ctrl_config_use_vchan()` as proof that USB and
  SPI already share the same finished host-control implementation

## 4. Internal Boundaries

- the CLI should not know about backend details
- the daemon owns runtime backend selection and request dispatch
- `libbflbwifi` owns Wi-Fi semantics and state management
- `channel/` owns the control-backend abstraction
- external code should depend only on `include/bflbwifi.h`

## 5. Build

```bash
make ENABLE_NETIF_AUTO_CONFIG=1
```

Artifacts:

- `libbflbwifi.a`
- `app/bflbwifid`
- `app/bflbwifictrl`

## 6. Related Documents

- `../README.md`
- `../HOST_ARCHITECTURE.md`
- `../virtualchan/README.md`
