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
make CHIP=bl616d BOARD=bl616ddk CPU_ID=ap CONFIG_ROMAPI=n
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

## 系统架构

nethub 方案包含 host 和 device 两部分：
- **Device 端**：运行在 Bouffalo 芯片上，处理 WiFi 连接和数据转发
- **Host 端**：运行在主机上，通过驱动程序与 Device 通信，命名为host driver，简称hd

## 注意事项和常见问题

### 问题 1：硬件连接并加载 host 驱动后，host 会多出哪些 interface？

加载后，host 会多出至少 2 个 interface：

1. **hd_eth0** [必须] - 网卡接口，相当于 STA 转以太网功能
2. **/dev/ttyhd0** [必须] - TTY 控制通道，连接、断开等命令通过此通道进行发送和接收，协议为标准 AT 命令
3. **/dev/ttyhd1** [选配] - 专为用户提供的数据通道，用户可以借助此接口传输任意数据（当前还未支持）

### 问题 2：host 和 device 的 DHCP client 运行在哪一侧？

运行在 **device** 侧。host 通过查询机制获取 IP 地址。连接 WiFi 后会收到 GOTIP 事件，然后 host 通过查询更新 IP 地址。

可以参考 host 中的 `connect_and_setip` 脚本实现。

### 问题 3：device 侧的数据包 filter 用户可以自定义吗？默认规则是怎样的？

**可以自定义**。用户在device侧重新实现 `eth_input_filter` 函数即可完成 filter 的自定义。

**默认过滤规则：**
1. **ARP 包**：双向传输
2. **DHCPv4 和 ICMPv4 包**：默认交给 device 侧处理，不会交付到host侧
3. **端口过滤**：用户可通过定义 `CONFIG_NETHUB_FILTER_LOCAL_PORT_MIN` 和 `CONFIG_NETHUB_FILTER_LOCAL_PORT_MAX` 完成地址空间的过滤
4. **其他数据包**：默认都给 host 侧处理

### 问题 4: nethub、hd_sdio、easyat、connect_and_setip 有什么关系和区别

**nethub**: 运行在设备端的固件软件，实现WiFi Station到以太网协议转换，支持数据包过滤和转发规则管理。

**hd_sdio**: 内核驱动模块(ko文件)，负责主机与设备间的SDIO通信。

**easyat**: 用户控制设备的AT命令封装库，提供便捷的设备操作接口。开发者可直接收发AT协议绕过此库。

**connect_and_setip**: 网络配置示例工具，实现连接指定AP、获取IP信息并设置静态IP的完整流程。

**架构关系**:
```
用户应用层
    ↓ connect_and_setip (示例工具)
    ↓ easyat (AT命令库)
控制通道层 (/dev/ttyhd0)
    ↓ hd_sdio (内核驱动)
物理层 (SDIO接口)
    ↓ nethub (设备固件)
```

**数据流向**:
- `nethub`是设备端固件，处理WiFi协议转换和数据转发
- `hd_sdio`是底层通信驱动，负责主机与设备的物理连接
- `easyat`是用户空间控制库，封装AT命令操作
- `connect_and_setip`是应用层示例，展示完整的使用流程


### 问题 5：host 开发有什么特别的注意事项

**网络配置要求：**

由于当前方案 DHCP client 设计在 device 侧，因此 host 侧需要遵循以下配置要求：

- **必须关闭 hd_eth0 的 DHCP 服务**：采用静态 IP 配置方式
- **静态 IP 配置步骤**：
  1. 使用 easyat 命令查询链路状态：`./userspace/easyat/src/easyat get_link_status`
  2. 根据查询结果配置 host 侧的静态 IP 地址（与 device 侧使用相同的 IP）
  3. 确保 host 侧和 device 侧使用相同的 IP 地址

**配置示例：**
```bash
# 1. 查询链路状态获取 IP 信息
./userspace/easyat/src/easyat get_link_status

# 2. 根据查询结果配置静态 IP（示例）
sudo ip addr add 192.168.1.100/24 dev hd_eth0
sudo ip link set hd_eth0 up
```

⚠️ **重要提醒**：如果 host 侧启用了 DHCP 服务，将会与 device 侧的 DHCP client 产生冲突，导致网络连接异常。

## 待实现功能

- [ ] 热插拔 SDIO 功能
- [ ] hd_sdio.ko 动态加载/卸载功能
- [ ] 用户私有通道数据传输（/dev/ttyhd1）
- [ ] 性能优化和稳定性提升
- [ ] 增加 USB、SPI 相关 interface 支持

## 许可证

MIT License

