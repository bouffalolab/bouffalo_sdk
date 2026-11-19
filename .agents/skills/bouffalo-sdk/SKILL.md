---
name: bouffalo-sdk
description: 定位 Bouffalo SDK 的模块、源码、配置、文档和示例；适用于理解 SDK 架构、选择芯片和组件，以及开发、构建或调试 Bouffalo 固件。
---

# Bouffalo SDK 导航

按需求选择下表中的专题，再从专题进入 SDK 源码、配置、文档或示例。SDK 路径相对 `bouffalo_sdk/` 根目录；示例支持的芯片、板子和功能以当前版本的源码与配置为准。参考文件可能含历史信息，使用 API 或执行操作前核对目标工程。

## 从哪里开始

| 需求 | 阅读 |
| --- | --- |
| 了解 SDK、顶层目录与模块 | [SDK 概览](references/01-overview.md)、[架构与板级支持](references/02-architecture.md)、[组件目录](references/09-components.md) |
| 选择芯片、板子和核心 | [芯片差异与查证入口](references/04-chips-diff.md)、[板级继承示例](references/43-board-integration.md) |
| 找示例、开始编译或配置工程 | [示例索引](references/13-examples.md)、[构建和配置](references/06-build-system.md)、[开发与调试流程](references/07-development-workflow.md) |
| 外设、设备表、GPIO、UART、SPI、I2C、DMA | [LHAL 驱动](references/03-lhal-drivers.md)、[外设入口](references/08-peripherals.md)、[UART DMA](references/42-uart-dma.md) |
| 内存、Flash、分区和 PSRAM | [内存与分区](references/10-memory-management.md) |
| 加密、安全启动、证书、TLS | [安全和加密](references/11-security.md) |
| 低功耗、睡眠及唤醒 | [低功耗](references/12-power-management.md)、[无线共存与省电](references/16-wireless-coex-power.md) |
| 日志、GDB、崩溃分析、测试 | [调试与 Coredump](references/19-debugging-coredump.md)、[测试与 CI](references/20-testing-ci.md) |
| 固件打包、下载、烧录、串口控制脚（RTS→CHIP_EN、DTR→BOOT） | [固件打包与烧录](references/35-firmware-pack-flash.md) |

## 无线与网络

| 需求 | 阅读 |
| --- | --- |
| 无线协议概览和芯片选择 | [无线协议](references/05-wireless.md)、[芯片差异](references/04-chips-diff.md) |
| Wi-Fi 6：FHOST 和 wl80211 | [Wi-Fi 6](references/14-wifi6-wl80211.md) |
| Wi-Fi 4、BL602 | [Wi-Fi 4](references/21-wifi4.md) |
| BLE、GAP、GATT | [BLE 基础](references/15-ble-basics.md) |
| BLE Mesh、Nearby Fast Pair | [BLE Mesh 与 Nearby](references/36-ble-mesh-nearby.md) |
| Wi-Fi/Bluetooth 共存、无线省电 | [共存与省电](references/16-wireless-coex-power.md) |
| RF 参数、PHY、产测 | [RF 与 PHY](references/22-rf-phy.md) |
| IEEE 802.15.4、Thread | [802.15.4 与 Thread](references/23-ieee802154-thread.md) |
| Zigbee、Thread 共存 | [Zigbee](references/24-zigbee.md) |
| Sub-G、BL3141 | [Sub-G](references/38-subg-bl3141.md) |
| TCP/IP、lwIP、DNS、网络测试 | [TCP/IP 网络](references/17-tcpip-network.md) |
| HTTP、MQTT、云连接 | [应用协议和云](references/25-app-protocols-cloud.md) |
| Matter、工厂数据 | [Matter](references/37-matter.md) |
| 外部主机联网、NetHub、Linux WLAN 驱动 | [NetHub](references/26-nethub.md)、[外部主机 WLAN 驱动](references/39-wlan-external-host.md) |
| Netbus、AT 命令模块 | [Netbus 与 ATModule](references/27-netbus-at.md) |

## 系统与应用组件

| 需求 | 阅读 |
| --- | --- |
| RTOS、系统服务和工具库 | [组件目录](references/09-components.md)、[运行时工具](references/34-runtime-utilities.md) |
| 多核通信、RPMsg、Socket Offload | [IPC 与 Offload](references/18-ipc-offload.md) |
| 文件系统、键值存储、SPI NAND | [文件系统与存储](references/28-filesystems-storage.md) |
| OTA、FOTA、固件更新 | [OTA 与 FOTA](references/29-ota-fota.md) |
| USB、CherryUSB、ADB 设备测试 | [USB](references/30-usb-cherryusb.md) |
| 图形、显示、触摸 | [图形和显示](references/31-graphics-display-touch.md) |
| 音频、视频、摄像头、媒体组件 | [音视频](references/32-audio-video-media.md)、[实时媒体网络](references/40-realtime-media-network.md) |
| AI 推理、语音 | [AI 与语音](references/33-ai-inference-speech.md) |
| WebAssembly、WAMR | [WAMR](references/41-wasm-wamr.md) |

配置和能力随芯片、板子、核心、SDK 版本及依赖变化。先核对相应组件的 `Kconfig`、构建文件和目标示例；编译通过不等同于板上运行验证。
