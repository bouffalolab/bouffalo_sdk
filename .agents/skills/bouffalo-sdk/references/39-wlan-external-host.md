# WLAN 外部 Linux 主机驱动

`components/wireless/wlan/linux_driver/` 是独立的 Linux cfg80211 WLAN 主机驱动；`examples/wlan/wlan_fw/` 生成相应设备端固件。它与 [26-nethub.md](26-nethub.md) 的网络接口桥接方案不同，不能混用主机驱动、固件和配置。

主机 Makefile 选择一组 `CONFIG_BL_USB`、`CONFIG_BL_SDIO`、`CONFIG_BL_BTUSB`、`CONFIG_BL_BTSDU` 或 `CONFIG_BL_BTUART`；未显式选择时默认 USB。USB 和 SDIO 分别构建 `bl_usb_drv.ko`、`bl_sdio_drv.ko`。指定 CHIP、目标内核构建树 `KERNELDIR`/`KBUILDDIR`、架构和工具链；编译输出放隔离副本或构建目录，不能用当前主机内核版本推断目标板支持。现有 Makefile 主要设置 BL616 和 BL616CL 宏，BL618 系列的具体支持需核实 ID 与固件，不可泛化。

驱动默认通过 Linux `request_firmware()` 请求对应芯片、总线及 WLAN/combo/MP 的文件，实际名称见 `linux_driver/bl_platform.h`。设备固件 Makefile 使用 `CONFIG_WLAN_IF=usb|sdu`；BLE/BTBLE 非 UART 时应与 WLAN 使用相同总线，UART 标注为开发调试。核对目标固件实际文件名及部署映射，不应假设 `sdu` 构建名自动等于主机请求的 `sd` 名称。驱动 A-MSDU TX 布局和固件 `CFG_AMSDU` 必须匹配。

USB 有 boot 和固件两种枚举阶段；不同芯片和状态的 USB/SDIO ID 以驱动源码为准。源码有 PCIe 条件片段，但当前 Makefile 和固件示例提供 USB/SDIO 路径，不能据片段认定 PCIe 可用。检查模块编译、版本和固件名称只是离线验证；加载模块、枚举、下载、扫描、连接和实际流量都要在目标内核与设备上分别验证。
