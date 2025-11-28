# EasyAT Expect 脚本实现详细说明

## 概述

EasyAT 的 Expect 脚本实现是基于 Tcl/Expect 语言的自动化串口通信工具。它利用 Expect 强大的模式匹配和交互控制能力，提供了简洁易用的 WiFi 连接管理方案。

## 技术特点

- **快速开发**: 脚本语言，开发周期短
- **易于调试**: 内置调试支持，便于问题排查
- **灵活控制**: 强大的模式匹配和条件控制
- **跨平台**: 支持所有安装了 Expect 的系统
- **易于维护**: 代码简洁，逻辑清晰

## 依赖要求

### 系统要求
- **Expect 解释器**: 5.45+ 版本
- **Tcl/Tk**: 8.6+ 版本
- **串口设备**: `/dev/ttyHD0`
- **系统权限**: 串口读写权限

### 安装 Expect

```bash
# Ubuntu/Debian
sudo apt-get install expect

# CentOS/RHEL
sudo yum install expect

# Fedora
sudo dnf install expect

# Arch Linux
sudo pacman -S expect
```

## 脚本架构

### 核心组件

#### 1. `connect` - WiFi 连接脚本
负责 WiFi 网络连接的完整流程：

```bash
#!/usr/bin/expect
set timeout 20
spawn -open [open /dev/ttyHD0 "r+"]

# 设置工作模式
send -i $serial_id "AT+CWMODE=1\r\n"
expect -i $serial_id "OK"

# 连接 WiFi
send -i $serial_id "AT+CWJAP=\"$ssid\",\"$password\"\r\n"
expect -i $serial_id "GOTIP"
```

**功能特点**：
- 支持 WPA/WPA2 加密网络
- 自动处理开放网络连接
- 20 秒超时保护
- 错误状态检测

#### 2. `get_link_status` - 状态查询脚本
获取当前网络连接状态信息：

```bash
#!/usr/bin/expect
set timeout 5
spawn -open [open /dev/ttyHD0 "r+"]

send -i $serial_id "AT+CIPSTA?\r\n"
expect -i $serial_id "+CIPSTA:"
```

**输出信息**：
- IP 地址分配状态
- 网关地址信息
- 子网掩码配置
- DNS 服务器地址

#### 3. `wifi_scan` - 网络扫描脚本
扫描周围的可用 WiFi 网络：

```bash
#!/usr/bin/expect
set timeout 15
spawn -open [open /dev/ttyHD0 "r+"]

send -i $serial_id "AT+CWLAP\r\n"
expect -i $serial_id "+CWLAP:"
```

**扫描信息**：
- SSID（网络名称）
- 信号强度（RSSI）
- 加密类型
- 频段信息

## 使用方法

### 基本语法
```bash
./script_name [arguments]
```

### 具体命令

#### WiFi 连接
```bash
# 连接加密网络
./connect "MyWiFi" "password123"

# 连接开放网络
./connect "OpenWiFi"
```

#### 状态查询
```bash
./get_link_status
```

#### 网络扫描
```bash
./wifi_scan
```

## 技术实现细节

### Expect 语法要点

#### 1. Spawn 控制
```tcl
# 打开串口设备
spawn -open [open /dev/ttyHD0 "r+"]
set serial_id $spawn_id

# 向串口发送数据
send -i $serial_id "AT\r\n"

# 等待响应
expect -i $serial_id "OK"
```

#### 2. 模式匹配
```tcl
expect {
    -i $serial_id "GOTIP" {
        puts "✓ WiFi 连接成功"
    }
    -i $serial_id "ERROR" {
        puts "✗ 连接失败"
        exit 1
    }
    timeout {
        puts "✗ 连接超时"
        exit 1
    }
    eof {
        puts "✗ 设备断开连接"
        exit 1
    }
}
```

#### 3. 变量处理
```tcl
# 命令行参数
set ssid [lindex $argv 0]
set password [expr {$argc >= 2 ? [lindex $argv 1] : ""}]

# 条件判断
if {$argc < 1} {
    puts "用法: $argv0 <ssid> [password]"
    exit 1
}
```

### 错误处理机制

#### 1. 超时处理
```tcl
set timeout 20  # 设置全局超时

# 局部超时覆盖
expect -timeout 10 "OK"
```

#### 2. 异常捕获
```tcl
expect {
    "OK" { puts "✓ 命令执行成功" }
    "ERROR" {
        puts "✗ 命令执行失败"
        exit 1
    }
    timeout {
        puts "✗ 响应超时"
        exit 1
    }
    eof {
        puts "✗ 设备连接断开"
        exit 1
    }
}
```

#### 3. 资源清理
```tcl
# 确保关闭串口连接
trap {
    close -i $serial_id
    exit
} SIGINT SIGTERM

# 正常关闭
close -i $serial_id
```

## 调试支持

### 调试模式

#### 1. 启用内部调试
```bash
# 在脚本开头添加
exp_internal 1

# 或者通过环境变量
export Expect_debug=1
./connect "SSID" "password"
```

#### 2. 日志输出
```bash
# 保存调试日志到文件
./connect "SSID" "password" 2>&1 | tee debug.log
```

#### 3. 交互式调试
```bash
# 使用 strace 跟踪系统调用
strace -o trace.log ./connect "SSID" "password"

# 使用 ltrace 跟踪库函数调用
ltrace -o ltrace.log ./connect "SSID" "password"
```

### 调试技巧

#### 1. 分步执行
```tcl
# 添加调试输出
puts "发送命令: AT+CWMODE=1"
send -i $serial_id "AT+CWMODE=1\r\n"
puts "等待响应..."
expect -i $serial_id "OK"
puts "收到响应: OK"
```

#### 2. 变量检查
```tcl
# 显示变量值
puts "SSID: $ssid"
puts "Password length: [string length $password]"
```

## 性能优化

### 1. 超时调优
```tcl
# 根据操作类型设置合适的超时
set timeout 2     # 状态查询
set timeout 10    # 网络扫描
set timeout 20    # WiFi 连接
```

### 2. 模式优化
```tcl
# 使用精确的模式匹配
expect -re "\\+CIPSTA:ip:\"([^\"]+)\"" {
    set ip $expect_out(1,string)
    puts "IP 地址: $ip"
}
```

### 3. 并发控制
```tcl
# 避免同时运行多个实例
if {[file exists /tmp/easyat.lock]} {
    puts "错误: 已有实例在运行"
    exit 1
}
set lock_file [open /tmp/easyat.lock w]
puts $lock_file [pid]
close $lock_file
```

## 扩展开发

### 添加新功能脚本

#### 1. 创建模板
```bash
#!/usr/bin/expect
set timeout 10

if {$argc < 1} {
    puts "用法: $argv0 <arguments>"
    exit 1
}

# 参数解析
set arg1 [lindex $argv 0]

# 主要逻辑
spawn -open [open /dev/ttyHD0 "r+"]
set serial_id $spawn_id

# AT 命令处理
send -i $serial_id "AT+COMMAND\r\n"
expect -i $serial_id "OK"

# 清理资源
close -i $serial_id
```

#### 2. 函数库创建
```tcl
# common.exp - 公共函数库
proc serial_connect {device} {
    global serial_id
    spawn -open [open $device "r+"]
    set serial_id $spawn_id
    return $serial_id
}

proc send_at {command} {
    global serial_id
    send -i $serial_id "$command\r\n"
}

proc wait_for {pattern timeout} {
    global serial_id
    expect -timeout $timeout -i $serial_id $pattern
}
```

#### 3. 配置文件支持
```tcl
# config.tcl - 配置文件
set ::serial_device "/dev/ttyHD0"
set ::default_timeout 10
set ::baud_rate 115200

# 读取配置
source config.tcl
serial_connect $::serial_device
```

## 部署建议

### 1. 权限设置
```bash
# 设置执行权限
chmod +x connect get_link_status wifi_scan

# 设置用户组权限
chown :dialout connect get_link_status wifi_scan
chmod g+s connect get_link_status wifi_scan
```

### 2. 环境检查
```bash
#!/bin/bash
# check_env.sh - 环境检查脚本

# 检查 Expect 安装
if ! command -v expect &> /dev/null; then
    echo "错误: 未安装 Expect"
    exit 1
fi

# 检查串口设备
if [ ! -e "/dev/ttyHD0" ]; then
    echo "错误: 串口设备不存在"
    exit 1
fi

# 检查用户权限
if [ ! -r "/dev/ttyHD0" ] || [ ! -w "/dev/ttyHD0" ]; then
    echo "错误: 串口设备权限不足"
    exit 1
fi

echo "环境检查通过"
```

### 3. 包装脚本
```bash
#!/bin/bash
# easyat_expect.sh - 统一入口脚本

SCRIPT_DIR="$(dirname "$0")"
case "$1" in
    "connect")
        shift
        exec "$SCRIPT_DIR/scripts/connect" "$@"
        ;;
    "status")
        exec "$SCRIPT_DIR/scripts/get_link_status"
        ;;
    "scan")
        exec "$SCRIPT_DIR/scripts/wifi_scan"
        ;;
    *)
        echo "用法: $0 {connect|status|scan} [args...]"
        exit 1
        ;;
esac
```

## 故障排除

### 常见问题

#### 1. Expect 未找到
```bash
# 检查 Expect 安装
expect -v

# 查找 Expect 路径
which expect
whereis expect
```

#### 2. 串口权限问题
```bash
# 检查设备权限
ls -l /dev/ttyHD0

# 修复权限
sudo chmod 666 /dev/ttyHD0
# 或添加用户到 dialout 组
sudo usermod -aG dialout $USER
```

#### 3. 脚本执行失败
```bash
# 检查脚本语法
expect -d script_name.exp

# 跟踪执行过程
expect -D 1000 script_name.exp
```

## 版本历史

### v1.0.0
- 初始版本
- 实现基本 WiFi 连接功能
- 支持状态查询和网络扫描

### 计划中的功能
- 配置文件支持
- 多网络自动切换
- 连接质量监控
- 详细的日志记录