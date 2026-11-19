# NetHub 主机联网

## 架构与控制

NetHub 将设备侧 Wi-Fi 的 STA/AP endpoint 与选定 host 接口连接；host 到 Wi-Fi 走当前选定 endpoint，Wi-Fi 接收由 filter 决定丢弃、交本机、交 host 或两者。入口 `examples/wifi/nethub/` 在应用中调用 `nethub_bootstrap()`，组件位于 `components/net/nethub/`。`nethub_get_status()` 查询状态和计数，`nethub_set_active_wifi_channel()` 选择 STA/AP；`nethub_shutdown()` 停止实例。

AT 控制是可选组合，控制 facade 使用 AT 类型虚拟通道；`CONFIG_NETHUB_CTRLCHANNEL_USE_ATMODULE=n` 时 `nethub_ctrl_*` 返回不支持，但数据和普通 vchan 不因此禁用。自定义 `nethub_set_wifi_rx_filter()` 须在 bootstrap 前设置；callback 位于 Wi-Fi RX 热路径，不能阻塞、保留、修改或释放传入 pbuf，也不能假设帧连续。

## 接口与配置边界

当前 `cmake/bflb_flash.cmake` 的 LittleFS 打包命令将 `./` 前缀拼到 `BUILD_DIR`，直接传绝对路径会在 post-build 创建镜像时失败。需要隔离构建时可使用从示例目录指向独立任务构建目录的相对路径，先确认解析后的目标位置，避免覆盖已有 build。

示例默认 SDIO；USB 设备端使用 CDC ECM 承载网络数据、CDC ACM 承载控制及虚拟通道，SPI 当前为未实现 stub。客户文档描述 BL616/BL618DG dual 模式在启动时探测并锁定首个有效 SDIO/USB host，不自动热切换或故障转移；但当前 Kconfig 将 SDIO 和 USB 放在单选 choice 中。本次 BL618DG AP 以 `CONFIG_NETHUB_PROFILE_DUAL=y` 构建时，输入同时请求两者，生成配置只保留 USB，SDIO 为 n，`nh_profile_select.c` 也只注册 USB。因此这次成功构建不证明 dual 功能可用；须以生成配置、链接内容和实际双接口测试确认。`nethub_set_active_host_link()` 只切换已经编译、初始化并注册的链路路由，不负责初始化和枚举新接口，不能由此推导 dual 具备自动切换。

仓库客户文档 `examples/wifi/nethub/docs/NetHub.md` 将 BL616CL SDIO 标记为 build-only，并要求 BL616CL/BL618DG 关闭低功耗；然而当前示例 defconfig 对相关芯片默认启用 `CONFIG_NETHUB_LOWPOWER_ENABLE`。实际生成配置与文档支持状态是两回事，应显式选择并核对 autoconfig，不应仅因成功构建就认为低功耗已验证。

## 虚拟通道和 Linux host

`nethub_vchan.h` 定义 USER、AT、SYSTEM 类型及最大 1500 字节**消息 payload**；该值不能直接当作网络 MTU。SDIO host 使用 `mr_sdio.ko`、netdev、netlink 及用户态工具，主机队列深度等配置须与固件相配；Linux kernel module 的构建依赖目标主机内核头文件。入口 `bsp/common/msg_router/linux_host/userspace/nethub/build.sh` 会构建模块和用户态程序，load 操作改变主机内核状态，执行前核对环境和权限。

当前 host `virtualchan/nethub_vchan.c` 已有 USB ACM 发送、接收和重连代码；部分架构文档仍称 host USB vchan 未实现，客户 USER vchan 文档仍将端到端支持列为待验证。存在代码不等同于完成端到端验证：以实际固件、host 版本及目标环境联合测试确认。设备端桥接使用 pbuf 和完成回调管理缓冲生命周期，扩展数据路径应查看 `core/nh_types.h` 与 `backend/wifi/nh_wifi_bridge.c`，不要提前释放共享对象。

排查顺序：确认固件与 host 来自兼容版本、profile 与引脚、SDIO 队列深度或 USB ECM/ACM 枚举，再观察控制事件、网络接口/IP、收发和断连恢复。构建成功不能证明链路、吞吐、低功耗、虚拟通道或故障恢复；硬件及网络验证须分项记录。
