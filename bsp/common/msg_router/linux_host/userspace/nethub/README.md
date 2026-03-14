# NetHub Host 用户指南

本文档面向 host 使用者，说明如何在 Linux host 上 bringup NetHub。

当前 host 侧生效目录：

- `bsp/common/msg_router/linux_host/kernel`
- `bsp/common/msg_router/linux_host/userspace/nethub`

当前主路径是 `SDIO`。
`tty / vchan / USER virtual channel` 都承载在这条 host link 上，其中 `tty / vchan` 用于控制面 backend 选择，`USER virtual channel` 用于客户私有数据。

## 1. 组成

host 侧主要组件：

- `mr_sdio.ko`
  - host kernel 模块
- `bflbwifid`
  - 控制面守护进程
- `bflbwifictrl`
  - 命令行工具
- `libbflbwifi.a`
  - Wi-Fi 控制库
- `libnethub_vchan.a`
  - virtual channel 用户态库
- `nethub_vchan_app`
  - 可选调试工具

控制 backend 当前支持：

- `tty`
- `vchan`

说明：

- host 侧会同时编入 `tty + vchan`
- 实际使用哪一种，在 `bflbwifid` 启动时选择

## 2. 快速 bringup

### 2.1 编译

```bash
cd bsp/common/msg_router/linux_host/userspace/nethub
./build.sh build
```

产物位于：

- `output/mr_sdio.ko`
- `output/bflbwifid`
- `output/bflbwifictrl`
- `output/libnethub_vchan.a`
- `output/nethub_vchan_app`

### 2.2 加载内核模块

```bash
sudo ./build.sh load
```

如需卸载：

```bash
sudo ./build.sh unload
```

### 2.3 启动 daemon

#### 默认 TTY 模式

```bash
sudo ./output/bflbwifid
```

#### 指定 TTY 设备

```bash
sudo ./output/bflbwifid -c tty -p /dev/ttyAT0
```

#### VCHAN 模式

```bash
sudo ./output/bflbwifid -c vchan
```

可选路径参数：

- `-s <socket_path>`
- `-P <pid_file>`
- `-l <log_file>`

### 2.4 使用 CLI

查看状态：

```bash
sudo ./output/bflbwifictrl status
```

扫描 AP：

```bash
sudo ./output/bflbwifictrl scan
```

连接 AP：

```bash
sudo ./output/bflbwifictrl connect_ap "<ssid>" "<password>"
```

断开连接：

```bash
sudo ./output/bflbwifictrl disconnect
```

## 3. 控制 backend 选择

host 侧 backend 需要和 device 配置匹配：

| device 配置 | host 启动方式 |
| --- | --- |
| `CONFIG_NETHUB_AT_USE_VCHAN=n` | `./output/bflbwifid` 或 `./output/bflbwifid -c tty -p /dev/ttyAT0` |
| `CONFIG_NETHUB_AT_USE_VCHAN=y` | `./output/bflbwifid -c vchan` |

默认情况下，device 侧控制通道走 `TTY`，因此 host 侧通常直接启动：

```bash
sudo ./output/bflbwifid
```

## 4. 常用命令

当前公共命令固定为 9 条：

- `connect_ap`
- `disconnect`
- `scan`
- `status`
- `version`
- `reboot`
- `ota`
- `start_ap`
- `stop_ap`

示例：

```bash
sudo ./output/bflbwifictrl version
sudo ./output/bflbwifictrl reboot
sudo ./output/bflbwifictrl start_ap "<ssid>" "<password>"
sudo ./output/bflbwifictrl stop_ap
```

详细命令说明见：

- `bflbwifictrl/COMMANDS_CN.md`

## 5. 自动配网

`build.sh` 默认会以 `ENABLE_NETIF_AUTO_CONFIG=1` 编译 userspace。

因此当 device 侧上报 `GOTIP` 后，host 侧会自动尝试配置：

- `mr_eth0` 的 IP
- 默认网关
- DNS

如果 `connect_ap` 成功但 host 仍无法联网，请检查：

- `status` 是否已经进入 `GOTIP`
- `mr_eth0` 是否已获取地址
- 系统里的 NetworkManager、dhcpcd 等服务是否覆盖了 NetHub 配置

## 6. USER Virtual Channel

如果客户需要在 host 和 device 之间传输私有数据，可使用 USER Virtual Channel。

头文件：

- `virtualchan/nethub_vchan.h`

常用接口：

```c
int nethub_vchan_init(void);
int nethub_vchan_deinit(void);

int nethub_vchan_user_send(const void *data, size_t len);
int nethub_vchan_user_register_callback(nethub_vchan_recv_callback_t callback);

int nethub_vchan_get_state_snapshot(nethub_vchan_state_snapshot_t *snapshot);
```

更多说明见：

- `virtualchan/README_CN.md`

如果应用需要在发送前判断当前 virtual channel 是否已经 ready，可选读取：

- `link_state == NETHUB_VCHAN_LINK_UP`
  - 表示链路已可收发
- `host_state == NETHUB_VCHAN_HOST_STATE_DEVICE_RUN`
  - 表示 host 侧已经和 device 完成握手

## 7. 常见问题

### 7.1 `bflbwifid` 启动失败

优先检查：

- backend 选择是否和 device 匹配
- `/dev/ttyAT0` 是否存在
- `mr_sdio.ko` 是否已成功加载
- 硬件链路是否正常

### 7.2 `build.sh unload` 提示模块被占用

优先确认：

- 是否还有 `bflbwifid` 正在运行
- 是否还有 `nethub_vchan_app` 正在运行

通常直接执行：

```bash
sudo ./build.sh unload
```

脚本会先尝试停止已知持有者，再执行卸载。

## 8. 相关文档

- `HOST_ARCHITECTURE_CN.md`
- `bflbwifictrl/README_CN.md`
- `bflbwifictrl/COMMANDS_CN.md`
- `virtualchan/README_CN.md`
