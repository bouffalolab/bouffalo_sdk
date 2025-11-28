## nethub_host

本项目实现了kernel和userspace，kernel 会编译生成 ko 文件，完成将多种interface虚拟成以太网、tty通道。
userspace 做了一个基于tty通道，做的一个AT的应用程序，主要完成设备的连接、扫描、查询等功能.

## 功能特性

- **主机驱动**: 将多种interface虚拟成以太网、tty通道
- **WiFi连接**: 连接到指定WiFi网络、查询Wifi状态、扫描可用WiFi
- **两种实现**: C语言版本和Expect脚本版本

## 项目结构

- kernel: kernel 部分，会编译成*.ko文件
- userspace: 应用程序，会编译生成可执行文件，或者直接执行相关expect脚本

## 快速开始

### 编译 hd_sdio.ko和应用固件
./build.sh build

### 加载 hd_sdio.ko
sudo ./build.sh load

### 联网并配置静态IP(该脚本仅供测试使用，详细请参考下文)
./userspace/connect_and_setip <ssid> [password]

### WiFi连接
```bash
# 连接WiFi
./userspace/easyat/src/easyat connect MyWiFi password123

# 连接开放网络
./userspace/easyat/src/easyat connect OpenWiFi
```

### WiFi断开
```bash
./userspace/easyat/src/easyat disconnect_ap
```

### 查询状态
```bash
./userspace/easyat/src/easyat get_link_status
```

### 扫描网络
```bash
./userspace/easyat/src/easyat wifi_scan
```

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