# NetHub Device 模块说明

本文档说明 `components/net/nethub` 这个 device 侧模块的定位和入口。

如果你是第一次 bringup NetHub，请优先阅读：

- `examples/wifi/nethub/README.md`

## 1. 模块定位

`components/net/nethub` 是当前生效的 device 侧 NetHub 实现。

它的职责是：

- 把 device 侧 Wi-Fi 数据桥接到当前 host link
- 对 Wi-Fi RX 报文做本地/host 分流
- 在同一 host link 上提供逻辑控制通道
- 提供可选的 USER virtual channel

它不负责：

- Wi-Fi 协议栈本身
- host 侧 daemon / CLI
- 多个 host link 同时工作的产品级仲裁

## 2. 当前架构

当前数据和控制模型可以概括为：

```text
Wi-Fi + lwIP
    |
    v
  nethub
    |- data bridge
    |- packet filter
    |- ctrlpath
    |- virtual channel
    |
    v
Host Link (SDIO / USB / SPI)
```

当前主路径是 `SDIO`。

控制面和 USER 通道都不是独立物理接口，而是承载在当前 host link 上的逻辑通道。

## 3. 公共头文件

对外稳定公共头如下：

- `include/nethub.h`
- `include/nethub_vchan.h`
- `include/nethub_filter.h`

补充说明：

- 常规业务集成通常只需要 `include/nethub.h`
- `include/nethub_vchan.h`
  - 只有当你需要 host-link 上的逻辑 virtual channel 能力时再直接包含
- `include/nethub_filter.h`
  - 只有当你需要完全替换内建 Wi-Fi RX filter 时再直接包含
- `include/nethub_defs.h`
  - 仅在需要共享公共类型时再直接包含，普通业务代码通常不需要单独引用

### 3.1 核心 API

`nethub.h`：

- `nethub_bootstrap()`
- `nethub_shutdown()`
- `nethub_get_status()`
- `nethub_set_active_wifi_channel()`
- `nethub_ctrl_upld_send()`
- `nethub_ctrl_dnld_register()`

`nethub_vchan.h`：

- `nethub_vchan_user_send()`
- `nethub_vchan_user_recv_register()`
- `nethub_vchan_at_send()`
- `nethub_vchan_at_recv_register()`

`nethub_filter.h`：

- `nethub_set_wifi_rx_filter()`

说明：

- `nethub_ctrl_upld_send()` / `nethub_ctrl_dnld_register()`
  - 对应 host link 上的逻辑控制通道
- `nethub_vchan_user_*`
  - 对应 host link 上的逻辑 USER 通道
- `nethub_set_wifi_rx_filter()`
  - 用于替换内建 Wi-Fi RX filter
  - 必须在 `nethub_bootstrap()` 之前调用

## 4. 目录说明

- `include/`
  - 公共头文件
- `core/`
  - hub 生命周期、endpoint、filter、runtime、ctrlpath
- `profile/`
  - 当前编译配置对应的固定拓扑选择
- `bootstrap/`
  - 模块启动装配入口
- `backend/wifi/`
  - Wi-Fi backend 适配
- `backend/host/sdio/`
  - 当前主路径 backend
- `backend/host/usb/`
  - USB skeleton
- `backend/host/spi/`
  - SPI skeleton
- `diag/`
  - 内部诊断代码

## 5. 关键配置

当前 NetHub bringup 最相关的配置通常位于：

- `examples/wifi/nethub/defconfig`

常见关键项：

- `CONFIG_NETHUB=y`
- `CONFIG_MR_NETDEV=y`
- `CONFIG_MR_TTY=y`
- `CONFIG_MR_VIRTUALCHAN=y`
- `CONFIG_NETHUB_AT_USE_VCHAN=n`

Wi-Fi backend 选择：

- 打开 `CONFIG_WL80211=y`
  - 使用 `wl80211`
- 不打开 `CONFIG_WL80211`
  - 默认走 `fhost`

控制通道选择：

- `CONFIG_NETHUB_AT_USE_VCHAN=n`
  - 控制面走 `TTY`
- `CONFIG_NETHUB_AT_USE_VCHAN=y`
  - 控制面走 `AT virtual channel`

## 6. 初始化入口

当前示例工程中，NetHub 初始化在：

- `examples/wifi/nethub/app/src/app_user.c`

主流程是：

1. RF 参数初始化
2. `tcpip_init()`
3. `easyflash_init()`
4. `nethub_bootstrap()`
5. `app_wifi_init()`

如果启用了 `CONFIG_MR_VIRTUALCHAN`，示例还会继续初始化 USER virtual channel 演示代码。

## 7. 相关文档

- `examples/wifi/nethub/README.md`
- `components/net/nethub/ARCHITECTURE_CN.md`
- `bsp/common/msg_router/linux_host/userspace/nethub/README.md`
