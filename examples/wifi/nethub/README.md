# NetHub 用户指南

本文档面向首次接触 NetHub 的客户，目标是帮助你尽快把 device 和 host 跑起来。

如果只关心当前可用方案，可以直接记住下面几点：

- 当前主路径是 `SDIO`
- device 侧运行 `Wi-Fi backend（fhost / wl80211）+ lwIP`
- `nethub` 负责把 device 侧 Wi-Fi 能力桥接到 host
- 控制通道和 `USER virtual channel` 都承载在同一条 host link 上

当前生效目录：

- device 侧
  - `components/net/nethub`
  - `examples/wifi/nethub`
- host 侧
  - `bsp/common/msg_router/linux_host/kernel`
  - `bsp/common/msg_router/linux_host/userspace/nethub`

## 在线文档索引

- 在线 wiki 总入口：
  - <https://docs.bouffalolab.com/index.php?title=NetHub>
- 当前仓内主入口：
  - `examples/wifi/nethub/README.md`

说明：

- 线上 wiki 适合给客户快速查阅和分享
- 仓内文档更适合跟随 SDK 一起演进和更新
- 如果线上内容与仓内内容出现差异，建议优先以当前仓内文档为准

## 1. Bringup 前先确认两项选择

### 1.1 选择 Wi-Fi backend

默认配置：

- 不打开 `CONFIG_WL80211`
- 使用 `fhost`

如果要切到 `wl80211`，在 `examples/wifi/nethub/defconfig` 中打开：

```make
CONFIG_WL80211=y
```

说明：

- `fhost` 和 `wl80211` 是二选一关系
- 当前 `nethub` 同时支持这两种 device Wi-Fi backend

### 1.2 选择控制通道

device 与 host 需要匹配：

| device 配置 | host 启动方式 |
| --- | --- |
| `CONFIG_NETHUB_AT_USE_VCHAN=n` | `./output/bflbwifid` 或 `./output/bflbwifid -c tty -p /dev/ttyAT0` |
| `CONFIG_NETHUB_AT_USE_VCHAN=y` | `./output/bflbwifid -c vchan` |

默认配置是：

```make
CONFIG_NETHUB_AT_USE_VCHAN=n
```

即控制通道默认走 `TTY`。

补充说明：

- host 侧会同时编入 `tty + vchan`
- 实际选择发生在 `bflbwifid` 运行时

## 2. 编译并烧录 device 固件

```bash
cd examples/wifi/nethub

# BL616
make CHIP=bl616 BOARD=bl616dk

# BL616CL
make CHIP=bl616cl BOARD=bl616cldk

# BL618DG
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap
```

烧录示例：

```bash
cd examples/wifi/nethub
make flash CHIP=bl618dg COMX=/dev/ttyUSB0
```

当前 bringup 最相关的配置位于：

- `examples/wifi/nethub/defconfig`

常见关键项：

- `CONFIG_NETHUB=y`
- `CONFIG_MR_NETDEV=y`
- `CONFIG_MR_TTY=y`
- `CONFIG_MR_VIRTUALCHAN=y`
- `CONFIG_NETHUB_AT_USE_VCHAN=n`

## 3. 编译并启动 host 软件

### 3.1 编译

```bash
cd bsp/common/msg_router/linux_host/userspace/nethub
./build.sh build
```

产物：

- `output/mr_sdio.ko`
- `output/bflbwifid`
- `output/bflbwifictrl`
- `output/libnethub_vchan.a`
- `output/nethub_vchan_app`

### 3.2 加载内核模块

```bash
cd bsp/common/msg_router/linux_host/userspace/nethub
sudo ./build.sh load
```

如需卸载：

```bash
sudo ./build.sh unload
```

### 3.3 启动 host daemon

默认 TTY 模式：

```bash
sudo ./output/bflbwifid
```

显式指定 TTY：

```bash
sudo ./output/bflbwifid -c tty -p /dev/ttyAT0
```

VCHAN 模式：

```bash
sudo ./output/bflbwifid -c vchan
```

## 4. 基础验证流程

建议按下面顺序验证：

### 4.1 查看当前状态

```bash
sudo ./output/bflbwifictrl status
```

### 4.2 扫描 AP

```bash
sudo ./output/bflbwifictrl scan
```

### 4.3 连接 AP

```bash
sudo ./output/bflbwifictrl connect_ap "<ssid>" "<password>"
```

### 4.4 再次查看状态

```bash
sudo ./output/bflbwifictrl status
```

如果连接成功，通常可以看到：

- backend 已连接
- Wi-Fi 状态为 `CONNECTED` 或 `GOTIP`
- 获取到地址后显示 `IP / Gateway / Netmask / DNS`

### 4.5 断开连接

```bash
sudo ./output/bflbwifictrl disconnect
```

## 5. 常用命令

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

## 6. SDIO 硬件信息

当前客户 bringup 路径建议直接按 `SDIO` 使用。
`USB / SPI` 目前仍保留架构骨架，不作为当前推荐 bringup 路径。

### 6.1 芯片支持情况

| 接口类型 | BL616 / BL618 | BL618DG | BL616CL |
| --- | --- | --- | --- |
| SDIO | 支持 | 支持 | 支持 |
| USB | 规划中 | 规划中 | 规划中 |
| SPI | 规划中 | 规划中 | 规划中 |

### 6.2 SDIO 引脚

| 功能引脚 | BL616 / BL618 | BL618DG | BL616CL |
| --- | --- | --- | --- |
| SDIO_DAT2 | GPIO10 | GPIO43 | GPIO6 |
| SDIO_DAT3 | GPIO11 | GPIO44 | GPIO7 |
| SDIO_CMD | GPIO12 | GPIO45 | GPIO8 |
| SDIO_CLK | GPIO13 | GPIO46 | GPIO9 |
| SDIO_DATA0 | GPIO14 | GPIO47 | GPIO10 |
| SDIO_DATA1 | GPIO15 | GPIO48 | GPIO11 |

## 7. USER Virtual Channel

如果客户需要在 host 与 device 之间传输私有消息，可使用 `USER virtual channel`。

说明：

- `USER` 通道和控制通道是并列关系
- 它不是独立物理接口，而是承载在当前 host link 上的逻辑通道

### 7.1 device 侧 API

头文件：

- `components/net/nethub/include/nethub_vchan.h`

常用接口：

```c
int nethub_vchan_user_send(const void *data, uint16_t len);
int nethub_vchan_user_recv_register(nethub_vchan_recv_cb_t recv_cb, void *cb_arg);
```

### 7.2 host 侧 API

头文件：

- `bsp/common/msg_router/linux_host/userspace/nethub/virtualchan/nethub_vchan.h`

常用接口：

```c
int nethub_vchan_init(void);
int nethub_vchan_deinit(void);

int nethub_vchan_user_send(const void *data, size_t len);
int nethub_vchan_user_register_callback(nethub_vchan_recv_callback_t callback);

int nethub_vchan_get_state_snapshot(nethub_vchan_state_snapshot_t *snapshot);
```

host 侧典型使用顺序：

1. `nethub_vchan_init()`
2. `nethub_vchan_user_register_callback()`
3. `nethub_vchan_user_send()`
4. 使用完成后调用 `nethub_vchan_deinit()`

如果需要判断链路是否已经 ready，可选查询：

- `link_state == NETHUB_VCHAN_LINK_UP`
  - 表示链路已经可收发
- `host_state == NETHUB_VCHAN_HOST_STATE_DEVICE_RUN`
  - 表示 host 侧已经完成与 device 的握手

## 8. 常见问题

### 8.1 `bflbwifid` 启动失败

优先检查：

- device 和 host 的控制 backend 是否匹配
- `/dev/ttyAT0` 是否存在
- `mr_sdio.ko` 是否已经成功加载
- SDIO 硬件链路是否正常

### 8.2 `connect_ap` 成功但 host 没有网络

优先检查：

- `status` 是否已经进入 `GOTIP`
- `mr_eth0` 是否已经获取地址
- 系统里的 `NetworkManager`、`dhcpcd` 等服务是否覆盖了 NetHub 配置

说明：

- `build.sh` 默认以 `ENABLE_NETIF_AUTO_CONFIG=1` 编译 userspace
- device 上报 `GOTIP` 后，host 会自动配置 `mr_eth0` 的 IP、网关和 DNS

### 8.3 `build.sh unload` 提示模块被占用

优先确认：

- 是否还有 `bflbwifid` 正在运行
- 是否还有 `nethub_vchan_app` 正在运行

通常直接执行：

```bash
sudo ./build.sh unload
```

脚本会先尝试停止已知持有者，再执行卸载。

## 9. 更多文档

如果你已经完成 bringup，后续可按需要继续阅读：

- `components/net/nethub/README.md`
  - device 模块入口与公共头说明
- `components/net/nethub/ARCHITECTURE_CN.md`
  - device 架构说明和 API 视角框图
- `bsp/common/msg_router/linux_host/userspace/nethub/README.md`
  - host bringup 说明
- `bsp/common/msg_router/linux_host/userspace/nethub/HOST_ARCHITECTURE_CN.md`
  - host 架构说明
- `bsp/common/msg_router/linux_host/userspace/nethub/virtualchan/README_CN.md`
  - host 侧 USER virtual channel 详细说明
