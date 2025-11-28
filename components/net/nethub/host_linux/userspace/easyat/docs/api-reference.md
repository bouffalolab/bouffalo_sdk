# EasyAT API 和命令参考

## 概述

本文档提供了 EasyAT 工具的完整 API 参考和命令行接口说明，包括 C 语言版本和 Expect 脚本版本的详细使用方法。

## 命令行接口

### C 语言版本 (easyat)

#### 基本语法
```bash
./easyat <command> [arguments]
```

#### 返回值
- **0**: 命令执行成功
- **1**: 命令执行失败（串口错误、超时、参数错误等）

#### 命令详细说明

##### 1. WiFi 连接 (connect)
```bash
./easyat connect <ssid> [password]
```

**参数**：
- `ssid` (必需): WiFi 网络名称，支持中文和特殊字符
- `password` (可选): WiFi 密码，如未提供则尝试连接开放网络

**示例**：
```bash
# 连接加密网络
./easyat connect "MyHomeWiFi" "mypassword123"

# 连接开放网络
./easyat connect "FreeWiFi"

# 连接包含空格的 SSID
./easyat connect "My WiFi Network" "network password"
```

**超时**: 20 秒

**返回**: 连接成功时显示 "✓ WiFi connection successful"，失败时显示具体错误信息

##### 2. 状态查询 (get_link_status)
```bash
./easyat get_link_status
```

**输出格式**：
```
← +CIPSTA:ip:"192.168.1.100"
← +CIPSTA:gateway:"192.168.1.1"
← +CIPSTA:netmask:"255.255.255.0"
← +CIPSTA:dns1:"8.8.8.8"
← +CIPSTA:dns2:"8.8.4.4"
← OK

✓ Status query completed
IP: 192.168.1.100, Gateway: 192.168.1.1, Netmask: 255.255.255.0
```

**超时**: 2 秒

##### 3. WiFi 扫描 (wifi_scan)
```bash
./easyat wifi_scan
```

**输出格式**：
```
← +CWLAP:(3,"MyWiFi",-68,"00:11:22:33:44:55",1)
← +CWLAP:(4,"GuestWiFi",-72,"66:77:88:99:aa:bb",1)
← OK

✓ WiFi scan completed
Found 2 networks
```

**超时**: 10 秒

##### 4. 帮助信息 (help)
```bash
./easyat
# 或
./easyat help
```

**输出**：
```
EasyAT - WiFi串口通信工具 v1.0.0

用法: ./easyat <command> [arguments]

支持的命令:
  connect <ssid> [password]    连接到WiFi网络
  get_link_status              获取连接状态和IP信息
  wifi_scan                    扫描WiFi网络
  help                         显示此帮助信息

示例:
  ./easyat connect "MyWiFi" "password"
  ./easyat get_link_status
  ./easyat wifi_scan

技术规格:
  串口设备: /dev/ttyHD0
  波特率: 115200 8N1
  超时设置: 连接20s, 查询2s, 扫描10s
```

### Expect 脚本版本

#### 1. 连接脚本 (connect)
```bash
./connect <ssid> [password]
```

**参数**: 与 C 版本相同

**输出示例**：
```
→ AT+CWMODE=1
← OK
→ AT+CWJAP="MyWiFi","password123"
← WIFI CONNECTED
← WIFI GOT IP
← +CIFSR:STAIP,"192.168.1.100"
← +CIFSR:STAMAC,"dc:f6:11:22:33:44"

✓ WiFi 连接成功
SSID: MyWiFi, IP: 192.168.1.100
```

**超时**: 20 秒

#### 2. 状态查询脚本 (get_link_status)
```bash
./get_link_status
```

**输出格式**：
```
→ AT+CIPSTA?
← +CIPSTA:ip:"192.168.1.100"
← +CIPSTA:gateway:"192.168.1.1"
← +CIPSTA:netmask:"255.255.255.0"
← OK

连接状态: 已连接
IP 地址: 192.168.1.100
网关地址: 192.168.1.1
子网掩码: 255.255.255.0
```

**超时**: 2 秒

#### 3. 网络扫描脚本 (wifi_scan)
```bash
./wifi_scan
```

**输出格式**：
```
→ AT+CWLAP
← +CWLAP:(3,"MyWiFi",-68,"00:11:22:33:44:55",1,3)
← +CWLAP:(4,"GuestWiFi",-72,"66:77:88:99:aa:bb",3,4)
← OK

扫描完成，发现 2 个 WiFi 网络:

1) MyWiFi
   信号强度: -68 dBm
   加密类型: WPA2-PSK
   频段: 2.4GHz
   MAC地址: 00:11:22:33:44:55

2) GuestWiFi
   信号强度: -72 dBm
   加密类型: WPA/WPA2-PSK
   频段: 5GHz
   MAC地址: 66:77:88:99:aa:bb
```

**超时**: 15 秒

## 一键联网和配置静态ip工具 (connect_and_setip)

### 语法
```bash
./connect_and_setip <ssid> [password]
```

### 功能流程
1. **检查 easyat 工具**: 自动编译 C 版本（如果不存在）
2. **连接 WiFi**: 使用 easyat 连接到指定网络
3. **获取 IP 信息**: 从设备获取 IP、网关、子网掩码
4. **配置网络接口**: 设置静态 IP 配置
5. **显示结果**: 验证配置并显示当前网络状态

### 输出示例
```
---> Step 0: Checking easyat tool...
✓ easyat tool found

---> Step 1: Connect AP
Connecting to SSID: MyWiFi
Using password: [hidden]
→ AT+CWMODE=1
← OK
→ AT+CWJAP="MyWiFi","password123"
← WIFI CONNECTED
← WIFI GOT IP
✓ WiFi connection successful

---> Step 2: Getting IP information from AT module...
✓ Extracted info: IP:192.168.1.100  gw:192.168.1.1  mask:255.255.255.0

---> Step 3: Configuring hd_eth0...
✓ Static IP config done! Inf:hd_eth0 IP:192.168.1.100 gw:192.168.1.1 mask: 255.255.255.0

---> Step 4 Current config:
3: hd_eth0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UP group default qlen 1000
    link/ether 00:11:22:33:44:55 brd ff:ff:ff:ff:ff:ff
    inet 192.168.1.100/24 brd 192.168.1.255 scope global hd_eth0
       valid_lft forever preferred_lft forever

Default gateway:
default via 192.168.1.1 dev hd_eth0
```

## AT 命令参考

### 设备配置命令

#### 设置工作模式
```at
AT+CWMODE=<mode>
```
- `mode=1`: Station 模式（客户端）
- `mode=2`: AP 模式（热点）
- `mode=3`: Station + AP 模式

**响应**: `OK`

### WiFi 连接命令

#### 连接 AP
```at
AT+CWJAP="<ssid>","<password>"
```
- `ssid`: 目标网络名称
- `password`: WiFi 密码（开放网络可留空）

**响应**:
- 成功: `WIFI CONNECTED`, `WIFI GOT IP`
- 失败: `+CWJAP:<error_code>`

#### 断开连接
```at
AT+CWQAP
```
**响应**: `OK`

#### 查询连接状态
```at
AT+CWJAP?
```
**响应**: `+CWJAP:"<ssid>",<bssid>,<channel>,<rssi>`

### 网络信息命令

#### 查询 IP 配置
```at
AT+CIPSTA?
```
**响应格式**:
```
+CIPSTA:ip:"<ip_address>"
+CIPSTA:gateway:"<gateway>"
+CIPSTA:netmask:"<netmask>"
+CIPSTA:dns1:"<dns1>"
+CIPSTA:dns2:"<dns2>"
```

#### 设置静态 IP
```at
AT+CIPSTA="<ip>","<gateway>","<netmask>"
```

### 网络扫描命令

#### 扫描 WiFi 网络
```at
AT+CWLAP
```
**响应格式**:
```
+CWLAP:<ecn>,"<ssid>",<rssi>,"<mac>",<channel>,<freq>
```
- `ecn`: 加密类型 (0=开放, 2=WPA, 3=WPA2, 4=WPA/WPA2)
- `ssid`: 网络名称
- `rssi`: 信号强度 (dBm)
- `mac`: MAC 地址
- `channel`: 信道
- `freq`: 频段

## 错误代码参考

### WiFi 连接错误

| 错误代码 | 说明 | 解决方案 |
|----------|------|----------|
| 1 | 连接超时 | 检查 WiFi 信号和密码 |
| 2 | 密码错误 | 确认 WiFi 密码正确 |
| 3 | 找不到网络 | 检查 SSID 拼写 |
| 4 | 连接失败 | 重启设备或更换网络 |

### 通用错误

| 错误类型 | 说明 | 解决方案 |
|----------|------|----------|
| `ERROR` | 命令执行失败 | 检查命令格式和参数 |
| `FAIL` | 操作失败 | 确认设备状态正常 |
| `SEND FAIL` | 数据发送失败 | 检查串口连接 |

## 性能参数

### 通信参数
- **串口设备**: `/dev/ttyHD0`
- **波特率**: 115200 bps
- **数据格式**: 8 数据位, 无校验, 1 停止位 (8N1)
- **流控制**: 无

### 超时设置
| 操作 | 默认超时 | 建议范围 |
|------|----------|----------|
| 设备响应 | 2 秒 | 1-5 秒 |
| WiFi 连接 | 20 秒 | 15-30 秒 |
| 网络扫描 | 10-15 秒 | 10-30 秒 |
| IP 查询 | 2 秒 | 1-5 秒 |

### 性能指标
- **连接成功率**: >95%
- **扫描响应时间**: <15 秒
- **命令响应时间**: <2 秒
- **并发连接**: 1 个设备

## 配置选项

### 环境变量
```bash
# 串口设备路径
export EASYAT_DEVICE="/dev/ttyHD0"

# 默认超时时间
export EASYAT_TIMEOUT="10"

# 启用调试模式
export EASYAT_DEBUG="1"
```

### 配置文件
用户可以创建 `~/.easyatrc` 配置文件：

```ini
[general]
device = /dev/ttyHD0
baud_rate = 115200
timeout = 10
debug = false

[wifi]
connect_timeout = 20
scan_timeout = 15
auto_reconnect = true

[network]
interface = hd_eth0
configure_static_ip = true
```

## 示例脚本

### 批量网络测试
```bash
#!/bin/bash
# test_networks.sh - 批量测试多个网络

networks=(
    "HomeWiFi:home_password123"
    "OfficeWiFi:office_pwd"
    "GuestWiFi:"
)

for network in "${networks[@]}"; do
    IFS=':' read -r ssid password <<< "$network"
    echo "测试连接: $ssid"

    if ./easyat/src/easyat connect "$ssid" "$password"; then
        echo "✓ $ssid 连接成功"
        ./easyat/src/easyat get_link_status
    else
        echo "✗ $ssid 连接失败"
    fi

    echo "---"
    sleep 2
done
```

### 网络监控脚本
```bash
#!/bin/bash
# monitor_connection.sh - 网络连接监控

while true; do
    if ! ./easyat/src/easyat get_link_status > /dev/null 2>&1; then
        echo "$(date): 连接断开，尝试重连"
        ./easyat/src/easyat connect "MyWiFi" "password123"
    fi

    sleep 30
done
```

这个 API 参考文档为开发者提供了完整的使用指南，包括所有命令的详细说明、参数格式、输出示例和错误处理。
