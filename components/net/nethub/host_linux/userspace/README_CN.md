# NetHub 用户态工具

博流智能 NetHub 网络控制模块用户态工具集。

## 模块介绍

### bflbwifictrl - BFLB WiFi AT命令控制模块

博流智能 WiFi Soc AT 命令控制模块（Linux 用户态）。

**功能**：
- 守护进程架构，支持多进程安全访问
- CLI 工具，提供命令行控制接口
- C 语言 API，支持二次开发

**快速开始**：
```bash
cd bflbwifictrl
make

# 启动守护进程(/dev/ttyHD*或者 /dev/ttyACM*)
sudo ./app/bflbwifid -p /dev/ttyACM1

# 使用 CLI 工具
./app/bflbwifictrl connect_ap ssid password
./app/bflbwifictrl status
```

**详细文档**：[bflbwifictrl/README_CN.md](bflbwifictrl/README_CN.md)

### bflbvirtualchannel - BFLB虚拟通道

基于 Netlink 的虚拟通道控制工具。

**功能**：
- 网络接口状态管理
- 路由配置
- 网络事件监听

**快速开始**：
```bash
cd bflbvirtualchannel
make

# 运行示例
sudo ./bflbvchan_app
```

**详细文档**：[bflbvirtualchannel/README_CN.md](bflbvirtualchannel/README_CN.md)

## 编译

### 编译所有模块

```bash
make
```

### 编译单个模块

```bash
make bflbwifictrl      # 编译 bflbwifictrl
make bflbvirtualchannel # 编译 bflbvirtualchannel
```

### 清理

```bash
make clean
```

## 目录结构

```
userspace/
├── bflbwifictrl/          # BFLB WiFi AT命令控制模块
│   ├── app/                # 应用程序（守护进程+CLI）
│   ├── src/                # 核心库源代码
│   ├── include/            # 头文件
│   └── Makefile
├── bflbvirtualchannel/     # BFLB虚拟通道控制工具
│   ├── bflbvchan.c         # 核心控制逻辑
│   ├── bflbvchan_app.c     # 应用示例
│   └── Makefile
└── Makefile                # 统一构建入口
```

## 依赖

- Linux 系统
- gcc 编译器
- pthread 库
- libnl-3 库（bflbvirtualchannel 模块）
- TTY 串口设备（bflbwifictrl 模块）

## 注意事项

1. **权限**：网络和 TTY 设备操作需要 root 权限
2. **设备路径**：根据硬件配置选择正确的 TTY 设备
3. **库依赖**：确保系统已安装必要的开发库
