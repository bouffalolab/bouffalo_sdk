# bflbwifictrl / libbflbwifi Guide

This document only describes the responsibilities of the `bflbwifictrl`
subdirectory itself.

If you want the full host-side bring-up flow and overall architecture, read:

- [../README.md](../README.md)
- [../HOST_ARCHITECTURE.md](../HOST_ARCHITECTURE.md)

If you want the overall NetHub quick start, also see:

- `examples/wifi/nethub/README.md`

## 1. Subdirectory Responsibilities

`bflbwifictrl/` contains three parts:

- `app/`
  - `bflbwifid` daemon
  - `bflbwifictrl` CLI
  - local IPC and the command specification table
- `include/`
  - public headers for `libbflbwifi`
- `src/`
  - internal implementation of the Wi-Fi control library

Directory layout:

```text
bflbwifictrl/
|- app/
|  |- bflbwifid.c
|  |- bflbwifid_config.*
|  |- bflbwifid_instance.*
|  |- bflbwifid_log.*
|  |- bflbwifid_ota.*
|  |- bflbwifid_request.*
|  |- bflbwifid_runtime.*
|  |- bflbwifid_service.*
|  |- bflbwifi_command.*
|  |- bflbwifi_ipc.*
|  `- bflbwifictrl.c
|- include/
|  |- bflbwifi.h
|  |- bflbwifi_log.h
|  `- bflbwifi_netif.h
`- src/
   |- bflbwifi_wifi.c
   |- bflbwifi_state.c
   |- bflbwifi_parser.c
   |- bflbwifi_tty.c
   |- bflbwifi_internal.h
   `- channel/
```

## 2. External Capability Surface

### 2.1 Public CLI / Daemon Commands

The command surface is fixed to 9 commands:

- `connect_ap`
- `disconnect`
- `scan`
- `status`
- `version`
- `reboot`
- `ota`
- `start_ap`
- `stop_ap`

For detailed command semantics, see [COMMANDS_CN.md](./COMMANDS_CN.md).

### 2.2 C API

Header:

- `include/bflbwifi.h`

Recommended initialization flow:

```c
bflbwifi_ctrl_config_t cfg;

bflbwifi_ctrl_config_init(&cfg);
bflbwifi_ctrl_config_use_vchan(&cfg);
bflbwifi_init_ex(&cfg);
```

The current public API includes:

- initialization / runtime state
  - `bflbwifi_ctrl_config_init()`
  - `bflbwifi_ctrl_config_use_tty()`
  - `bflbwifi_ctrl_config_use_vchan()`
  - `bflbwifi_init_ex()`
  - `bflbwifi_init()`
  - `bflbwifi_get_ctrl_status()`
- STA
  - `bflbwifi_sta_connect()`
  - `bflbwifi_sta_disconnect()`
  - `bflbwifi_sta_get_state()`
  - `bflbwifi_sta_get_connection_info()`
  - `bflbwifi_scan()`
- device management
  - `bflbwifi_get_version()`
  - `bflbwifi_restart()`
  - `bflbwifi_ota_upgrade()`
- SoftAP
  - `bflbwifi_ap_start()`
  - `bflbwifi_ap_stop()`

## 3. Internal Boundaries of This Submodule

- the CLI should not know about `tty` / `vchan`
- the daemon is responsible for runtime backend selection and command dispatch
- `bflbwifid_ota.*` isolates daemon-local OTA state
- `libbflbwifi` owns Wi-Fi semantics and state management
- `channel/` owns the control-backend abstraction
- external application code should only depend on `include/bflbwifi.h` and
  should not include internal headers from `src/` or `channel/`

One remaining piece of architecture debt:

- `bflbwifi_init()` is still a legacy TTY wrapper
- `bflbwifid_ota.*` still uses a small amount of non-public helper logic, which
  can be reduced further later

## 4. Build

```bash
make ENABLE_NETIF_AUTO_CONFIG=1
```

Artifacts:

- `libbflbwifi.a`
- `app/bflbwifid`
- `app/bflbwifictrl`

Standalone example programs are no longer maintained. Integration should go
through the daemon / CLI or by linking `libbflbwifi.a` directly.
