# NetHub Linux 主机方案

基于BL616C/BL618M/BL618DG/BL616CL 的 Linux 主机通信方案，提供内核模块和用户态控制工具。

## 目录

- [1. 系统架构](#1-系统架构)
- [2. 快速开始](#2-快速开始)
- [3. 开发指南](#3-开发指南)
- [4. 常见问题](#4-常见问题)

## 1. 系统架构

![主机-设备架构图](./doc/images/arch_host_device.png)

**说明**：当前控制通道通过 tty 传输，后续将支持通过 VirtualChannel 传输 AT 命令。

### 1.1 组件说明

| 组件 | 类型 | 说明 |
|------|------|------|
| bflbwifictrl | 用户态 | 命令行工具，通过 Unix Socket 与守护进程通信 |
| bflbwifid | 用户态 | 守护进程，负责设备通信和 WiFi 连接管理 |
| libbflbwifi | 用户态 | 静态库，提供 WiFi 控制 API |
| nethub_vchan_app | 用户态 | VirtualChannel 通信程序，提供私有数据通道（消息包传输，非流式） |
| mr_sdio.ko | 内核态 | 内核驱动模块，支持 SDIO 接口 |

### 1.2 数据流向

```
用户命令：
bflbwifictrl → bflbwifid → libbflbwifi → /dev/ttyAT → 内核驱动 → 设备

URC事件：
设备 → 内核驱动 → /dev/ttyAT → bflbwifid → 解析处理 → 网卡配置

工作流程：
加载内核模块 → 启动守护进程 → TTY 通信建立 → 准备就绪
```

## 2. 快速开始

![NetHub SDIO 快速入门演示](./doc/images/nethub_sdio_quickstart_demo.gif)

本演示展示了完整的操作流程：编译 → 加载内核模块 → 启动守护进程 → WiFi 连接 → 网络通信 → 消息收发。

### 2.1 编译和加载内核模块

```bash
cd nethub/
./build.sh build
sudo ./build.sh load
```

### 2.2 运行 WiFi 守护进程

```bash
# 启动守护进程（默认使用 /dev/ttyAT0）
sudo ./output/bflbwifid -p /dev/ttyAT0

# 或查看帮助
sudo ./output/bflbwifid -h
```

### 2.3 使用 WiFi 控制工具

```bash
# 扫描 AP
sudo ./output/bflbwifictrl scan

# 连接 AP（无密码）
sudo ./output/bflbwifictrl connect_ap "SSID"

# 连接 AP（有密码）
sudo ./output/bflbwifictrl connect_ap "SSID" "password"

# 查看状态
sudo ./output/bflbwifictrl status

# 断开连接
sudo ./output/bflbwifictrl disconnect
```

## 3. 开发指南

### 3.1 系统要求

- **内核版本**：Linux 3.7+（内核模块需要）
- **架构**：x86_64、ARMv7（树莓派）、ARMv8（树莓派 4/5）
- **依赖**：gcc、make、libc、pthread

**兼容性说明**：
- ✅ 用户空间程序（bflbwifid/bflbwifictrl）：无内核版本限制
- ✅ 内核模块（mr_sdio.ko）：支持 Linux 3.7+
- ✅ 已测试：Linux raspberrypi 6.12.25+rpt-rpi-v8

### 3.2 命令参考

#### build.sh 命令

| 命令 | 说明 |
|------|------|
| `build` | 编译内核模块、virtualchan 和 bflbwifictrl |
| `clean` | 清理所有编译产物 |
| `load` | 加载内核模块 `mr_sdio.ko` |
| `unload` | 卸载内核模块 |

#### bflbwifictrl 命令

| 命令 | 说明 |
|------|------|
| `scan` | 扫描附近 AP |
| `connect_ap <SSID> [密码]` | 连接到 AP |
| `disconnect` | 断开连接 |
| `status` | 查看连接状态 |
| `version` | 查看固件版本 |

### 3.3 功能特性

#### bflbwifid（守护进程）

- **TTY 通信**：与 WiFi 模块通过串口通信
- **AT 协议**：解析和封装 AT 指令
- **状态管理**：维护 WiFi 连接状态
- **Unix Socket**：提供 IPC 通信接口
- **GOTIP 自动配置**（可选）：收到 IP 后自动配置 Linux 网卡

#### bflbwifictrl（命令行工具）

- **CLI 接口**：通过 Unix Socket 与守护进程通信
- **简洁易用**：提供常用 WiFi 操作命令
- **状态查询**：实时显示连接信息

#### GOTIP 自动配置功能（默认启用）

当 WiFi 模块获取 IP 后，会自动：
1. 解析 URC：`+CW:GOTIP,IP:xxx,gw:xxx,mask:xxx,dns:xxx`
2. 配置网卡：`ip addr add`、`ip route add default`
3. 配置 DNS：写入 `/etc/resolv.conf`

**注意事项**：
- 部分host需要禁用 NetworkManager：`sudo systemctl disable --now NetworkManager`
- 需要禁用 dhcpcd：`sudo systemctl disable --now dhcpcd`
- 默认网卡名为 `mr_eth0`（可在代码中修改）

### 3.4 代码修改与编译

#### 修改代码后重新编译

```bash
# 只编译 bflbwifictrl
cd bflbwifictrl
make clean && make

# 或使用 build.sh 编译所有
cd ..
./build.sh build
```

#### 重新加载内核模块

```bash
sudo ./build.sh unload
sudo ./build.sh load
```

## 4. 常见问题

### 4.1 找不到串口设备

```bash
# 查看可用串口
ls /dev/ttyACM* /dev/ttyUSB*

# 添加用户到 dialout 组（避免每次都用 sudo）
sudo usermod -aG dialout $USER
# 然后重新登录
```

### 4.2 IP 配置后自动消失

**原因**：NetworkManager 或 dhcpcd 覆盖了手动配置。

**解决**：
```bash
# 禁用 NetworkManager
sudo systemctl stop NetworkManager
sudo systemctl disable NetworkManager

# 禁用 dhcpcd
sudo systemctl stop dhcpcd
sudo systemctl disable dhcpcd
```

### 4.3 查看调试日志

```bash
# 查看守护进程日志
tail -f /var/log/bflbwifi.log

# 查看内核日志
dmesg -w

# 守护进程前台运行（查看实时输出）
sudo ./output/bflbwifid -p /dev/ttyAT0 --foreground
```

### 4.4 编译错误

```bash
# 确保安装必要的开发包
sudo apt-get install build-essential libc-dev-i386

# 树莓派需要安装 32 位兼容库
sudo apt-get install libc6:i386 libstdc++6:i386
```
