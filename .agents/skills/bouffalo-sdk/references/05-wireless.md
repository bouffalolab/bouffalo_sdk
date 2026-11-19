# 无线协议入口

协议可用性取决于芯片、板级、SDK 版本和配置；以目标组件 Kconfig、示例及芯片资料核对。路径相对 SDK 根目录。

| 需求 | 专题与 SDK 入口 |
| --- | --- |
| Wi-Fi 6，选择 FHOST 或 wl80211 | [Wi-Fi 6](14-wifi6-wl80211.md)；`examples/wifi/sta/`、`examples/wifi/macsw_bare/` |
| Wi-Fi 4、BL602 | [Wi-Fi 4](21-wifi4.md)；`components/wireless/wifi4/` |
| 蓝牙 controller、host、GAP/GATT | [BLE](15-ble-basics.md)；`components/wireless/bluetooth/`、`examples/btble/` |
| BLE Mesh、Nearby | [BLE Mesh 与 Nearby](36-ble-mesh-nearby.md) |
| Wi-Fi/Bluetooth 共存与无线省电 | [共存与省电](16-wireless-coex-power.md) |
| IEEE 802.15.4、Thread | [802.15.4 与 Thread](23-ieee802154-thread.md) |
| Zigbee | [Zigbee](24-zigbee.md) |
| Matter | [Matter](37-matter.md) |
| RF 参数、PHY | [RF 与 PHY](22-rf-phy.md) |
| Sub-G、BL3141 | [Sub-G](38-subg-bl3141.md) |
| Linux 外部主机 WLAN 驱动 | [外部主机 WLAN 驱动](39-wlan-external-host.md) |

## 常见 Wi-Fi 方案

- P2P 发现和建组：`examples/wifi/p2p/wifi_p2p/README.md`；高性能示例为 `examples/wifi/p2p/wifi_p2p_high_perf/`。
- 配网：BLE 配网见 `examples/wifi/sta/smartconfig_ble/README.md`；SoftAP 网页配网见 `examples/wifi/sta/smartconfig_softap/README.md`，该文档介绍 BL602。
- 桥接：AP–STA 见 `examples/wifi/ap_sta_bridge/README.md`；Wi-Fi–Ethernet 见 `examples/wifi/sta/wifi_eth_bridge/README.md`。它们与 NetHub 主机联网是不同方案。
- 中文 Wi-Fi API 与示例文档入口：`docs/zh/api_reference/wifi6.rst`、`docs/zh/samples/wifi6/index.rst`。各方案适用芯片以示例配置为准。

Wi-Fi 6 的 FHOST 和 wl80211、蓝牙 blestack 和 NimBLE 使用不同接口，先确认工程选用的栈。连接、获取 IP、业务通信和性能是不同验证阶段。
