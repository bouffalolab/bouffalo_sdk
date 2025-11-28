# NetHub 用户空间工具集

这是 NetHub Linux 主机驱动的用户空间工具集，提供了通过 TTY 通道与 WiFi 模块通信的完整解决方案。包含 C 语言和 Expect 脚本两种实现方式，支持 WiFi 连接管理、状态查询和网络扫描等功能。

## 功能特性

- **WiFi 连接管理**: 连接到指定 WiFi 网络（支持加密和开放网络）
- **AP 断开连接**: 断开当前连接的 WiFi 热点
- **状态查询**: 查询当前 WiFi 连接状态和 IP 信息
- **网络扫描**: 扫描周围可用的 WiFi 网络
- **双实现支持**: C 语言版本和 Expect 脚本版本
- **一键联网**: 自动化 WiFi 连接和静态 IP 配置
- **硬件抽象**: 基于 `/dev/ttyHD0` TTY 接口的统一通信

## 快速开始

### 一键联网并配置静态IP（仅供测试使用，用户最好是通过easyat命令自行连接和配置静态ip）
```bash
./connect_and_setip <ssid> [password]
```

### C 语言版本使用
```bash
cd easyat/src
make
./easyat/src/easyat connect MyWiFi password123
./easyat/src/easyat connect MyOpenWifi
./easyat/src/easyat disconnect_ap
./easyat/src/easyat get_link_status
./easyat/src/easyat wifi_scan
```

### Expect 脚本版本使用
```bash
cd easyat/scripts
./connect MyWiFi password123
./disconnect_ap
./get_link_status
./wifi_scan
```

## 项目结构

```
userspace/
├── README.md                    # 本文档，项目总览和快速开始
├── connect_and_setip            # 一键联网和配置静态ip的脚本（仅供测试使用）
├── easyat/                      # 主项目目录
│   ├── src/                     # C 语言实现
│   │   ├── easyat.c            # 主程序源码
│   │   └── Makefile            # C 版本构建脚本
│   ├── scripts/                # Expect 脚本实现
│   │   ├── connect             # WiFi 连接脚本
│   │   ├── disconnect_ap       # AP 断开连接脚本
│   │   ├── get_link_status     # 状态查询脚本
│   │   └── wifi_scan           # WiFi 扫描脚本
│   └── docs/                   # 详细文档
│       ├── c-implementation.md     # C 实现详细说明
│       ├── expect-implementation.md # Expect 实现详细说明
│       └── api-reference.md         # API 和命令参考
└── tools/                      # 其他辅助工具
```

## 环境要求

### 基础要求
- **操作系统**: Linux
- **串口设备**: `/dev/ttyHD0`
- **权限**: 串口设备读写权限

### C 版本额外要求
- GCC 编译器
- Make 工具

### Expect 版本额外要求
- Expect 解释器
```bash
# Ubuntu/Debian 安装
sudo apt-get install expect
```

## 权限设置

```bash
# 添加用户到 dialout 组获取串口权限
sudo usermod -aG dialout $USER
newgrp dialout
```

## 两种实现对比

| 特性 | C 语言版本 | Expect 脚本版本 |
|------|------------|-----------------|
| 性能 | 高性能，编译后执行 | 解释执行，性能一般 |
| 依赖 | 仅需标准C库 | 需要 Expect 解释器 |
| 调试 | 适合调试和开发 | 适合快速测试 |
| 部署 | 需要编译 | 直接执行 |
| 维护 | 代码结构清晰 | 脚本简洁易懂 |

**推荐场景**：
- **生产环境**: 使用 C 语言版本
- **开发调试**: 使用 Expect 脚本版本
- **快速验证**: 使用 Expect 脚本版本

## 技术规格

- **串口配置**: `/dev/ttyHD0`, 115200 8N1
- **超时设置**:
  - WiFi 连接: 20 秒
  - 状态查询: 2 秒
  - WiFi 扫描: 10 秒
- **响应检测**: 检测 "GOTIP"（成功）和 "ERROR"（失败）

## 返回值

- `0`: 命令执行成功
- `1`: 命令执行失败（串口错误、超时、参数错误等）

## 构建和安装

### 构建 C 版本
```bash
cd easyat/src
make
```

### 清理构建文件
```bash
cd easyat/src
make clean
```

### 安装到系统（可选）
```bash
cd easyat/src
sudo make install
```

### 卸载（可选）
```bash
cd easyat/src
sudo make uninstall
```

## 使用示例

### 基本操作
```bash
# 连接加密 WiFi (用户连接后，需要自行配置静态ip）
./easyat/src/easyat connect MyWiFi password123

# 连接开放网络 (用户连接后，需要自行配置静态ip）
./easyat/src/easyat OpenWiFi

# 断开AP连接
./easyat/src/easyat disconnect_ap

# 查询连接状态
./easyat/src/easyat get_link_status

# 扫描网络
./easyat/scripts/wifi_scan
```


## 故障排除

### 常见问题

1. **权限被拒绝**
   ```bash
   sudo usermod -aG dialout $USER
   newgrp dialout
   ```

2. **设备不存在**
   - 检查内核模块是否加载：`lsmod | grep hd_`
   - 检查设备节点：`ls -l /dev/ttyHD0`

3. **连接超时**
   - 检查 WiFi 名称和密码是否正确
   - 确认设备在信号覆盖范围内

4. **编译失败**
   - 检查 GCC 工具链是否安装
   - 确认 Makefile 路径正确

### 调试技巧

- **查看串口输出**: 使用 `screen /dev/ttyHD0 115200` 监控通信
- **详细日志**: C 版本会显示详细的发送/接收信息
- **Expect 调试**: 在脚本开头添加 `exp_internal 1` 显示交互过程

## 相关文档

- [C 实现详细说明](easyat/docs/c-implementation.md)
- [Expect 实现详细说明](easyat/docs/expect-implementation.md)
- [API 和命令参考](easyat/docs/api-reference.md)
- [内核驱动文档](../kernel/README.md)

## 许可证

MIT License - 详见项目根目录 LICENSE 文件