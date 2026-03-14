# bflbwifictrl / libbflbwifi 说明

本文只描述 `bflbwifictrl` 子目录自身的职责。

如果要看 host 侧完整 bringup 和整体结构，请先读：

- [../README.md](../README.md)
- [../HOST_ARCHITECTURE_CN.md](../HOST_ARCHITECTURE_CN.md)

## 1. 子目录职责

`bflbwifictrl/` 下包含三部分：

- `app/`
  - `bflbwifid` daemon
  - `bflbwifictrl` CLI
  - 本地 IPC 与命令规格表
- `include/`
  - `libbflbwifi` 对外 public header
- `src/`
  - WiFi 控制库内部实现

目录结构：

```text
bflbwifictrl/
├── app/
│   ├── bflbwifid.c
│   ├── bflbwifid_config.*
│   ├── bflbwifid_instance.*
│   ├── bflbwifid_log.*
│   ├── bflbwifid_ota.*
│   ├── bflbwifid_request.*
│   ├── bflbwifid_runtime.*
│   ├── bflbwifid_service.*
│   ├── bflbwifi_command.*
│   ├── bflbwifi_ipc.*
│   └── bflbwifictrl.c
├── include/
│   ├── bflbwifi.h
│   ├── bflbwifi_log.h
│   └── bflbwifi_netif.h
└── src/
    ├── bflbwifi_wifi.c
    ├── bflbwifi_state.c
    ├── bflbwifi_parser.c
    ├── bflbwifi_tty.c
    ├── bflbwifi_internal.h
    └── channel/
```

## 2. 对外能力面

### 2.1 CLI / daemon 公共命令

固定为 9 条：

- `connect_ap`
- `disconnect`
- `scan`
- `status`
- `version`
- `reboot`
- `ota`
- `start_ap`
- `stop_ap`

详细命令语义见 [COMMANDS_CN.md](./COMMANDS_CN.md)。

### 2.2 C API

头文件：

- `include/bflbwifi.h`

推荐优先使用：

```c
bflbwifi_ctrl_config_t cfg;

bflbwifi_ctrl_config_init(&cfg);
bflbwifi_ctrl_config_use_vchan(&cfg);
bflbwifi_init_ex(&cfg);
```

当前 public API 包括：

- 初始化 / 运行态
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
- 设备管理
  - `bflbwifi_get_version()`
  - `bflbwifi_restart()`
  - `bflbwifi_ota_upgrade()`
- SoftAP
  - `bflbwifi_ap_start()`
  - `bflbwifi_ap_stop()`

## 3. 本子模块的内部边界

- CLI 不感知 `tty` / `vchan`
- daemon 负责 runtime backend 选择与命令调度
- `bflbwifid_ota.*` 负责隔离 daemon 与 OTA 私有状态
- `libbflbwifi` 负责 WiFi 语义与状态管理
- `channel/` 负责 control backend 抽象
- 外部业务代码只应依赖 `include/bflbwifi.h`，不应直接引用 `src/` 或 `channel/` 内部头

当前保留的一点架构债务：

- `bflbwifi_init()` 仍是 legacy TTY 包装
- `bflbwifid_ota.*` 仍承接了少量非 public helper，后续还可以继续收敛

## 4. 构建

```bash
make ENABLE_NETIF_AUTO_CONFIG=1
```

产物：

- `libbflbwifi.a`
- `app/bflbwifid`
- `app/bflbwifictrl`

当前不再维护独立 example 程序，推荐统一通过 daemon / CLI 或直接链接 `libbflbwifi.a` 进行集成。
