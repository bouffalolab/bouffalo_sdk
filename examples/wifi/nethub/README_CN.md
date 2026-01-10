# Nethub

## 概述

nethub 是一个带有 host 的方案支持双栈方案示例，支持多种 interface（默认使用 SDIO interface）。该方案实现了 WiFi 网络到以太网的转换功能，为主机提供网络连接能力。

## 功能特性

- 支持 WiFi 到以太网转换
- 支持双栈网络协议（IPv4）,IPv6默认会forward给host
- 提供 AT 命令控制通道
- 支持自定义数据包过滤规则
- 多种接口支持（SDIO、USB、SPI）

## 芯片支持情况

说明：**√** 表示支持；**×** 表示不支持；**○ ** 表示支持但未测试。

|   接口类型   | bl616/bl618 | bl616d/bl618d/bl619d | bl616L  |
|:------------:|:-----------:|:--------------------:|:-------:|
|  SDIO        |    √(sdio2) |        √(sdio3)      |    √(sdio2)   |
|  USB         |     开发中  |           开发中     |  开发中 |
|  SPI         |     开发中  |           开发中     |  开发中 |

## 硬件GPIO相关

| 功能引脚     | bl616/bl618 | bl616d/bl618d/bl619d | bl616L  |
|:------------:|:-----------:|:--------------------:|:-------:|
| SDIO_DAT2    |   GPIO10    | GPIO43               | GPIO6  |
| SDIO_DAT3    |   GPIO11    | GPIO44               | GPIO7  |
| SDIO_CMD     |   GPIO12    | GPIO45               | GPIO8  |
| SDIO_CLK     |   GPIO13    | GPIO46               | GPIO9  |
| SDIO_DATA0   |   GPIO14    | GPIO47               | GPIO10 |
| SDIO_DATA1   |   GPIO15    | GPIO48               | GPIO11 |

## 编译指南

### bl616/bl618 系列

```bash
make CHIP=<芯片名称> BOARD=<开发板名称>
```

示例：
```bash
make CHIP=bl616 BOARD=bl616dk
```

### bl616d 系列

```bash
make CHIP=bl616d BOARD=bl616ddk CPU_ID=ap
```

## 烧录指南

### 所有支持的芯片

```bash
make flash CHIP=<芯片名称> COMX=<串口名称>
```

示例：
```bash
make flash CHIP=bl616 COMX=/dev/ttyUSB0
```

## 系统架构和 host 使用手册

系统架构说明和 host 端使用手册请参考：[components/net/nethub/host_linux/README_CN.md](../../components/net/nethub/host_linux/README_CN.md)

#### 重要注意事项

⚠️ **DHCP 配置**：当前方案 DHCP client 运行在 device 侧，host 侧必须采用静态 IP 配置，且需确保 host 侧和 device 侧使用相同的 IP 地址。


