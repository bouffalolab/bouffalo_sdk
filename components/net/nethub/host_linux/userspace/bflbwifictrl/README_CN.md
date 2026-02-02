# BFLB WiFi Control Module

博流智能 WiFi Soc AT 命令控制模块（Linux 用户态）

## 架构

```
┌──────────────────────────────────────────────────────────────┐
│                        应用层                                 │
│  ┌────────────────────────┐  ┌──────────────────────────┐    │
│  │  bflbwifictrl (CLI)   │  │                          │    │
│  │            ↓            │  │                          │    │
│  │        Unix Socket      │  │      examples            │    │
│  │            ↓            │  │      (直接使用API)        │    │
│  │       bflbwifid        │  │                          │    │
│  │       (守护进程)         │  │                          │    │
│  └────────────────────────┘  └──────────────────────────┘    │
└──────────────────────────────┬───────────────────────────────┘
                               ↓
┌──────────────────────────────────────────────────────────────┐
│                    核心库 (libbflbwifi.a)                   │
│                                                               │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐    │
│  │  API层   │→│ 状态管理 │→│ 协议解析 │→│ TTY通信  │    │
│  └──────────┘  └──────────┘  └──────────┘  └──────────┘    │
└──────────────────────────────────────────────────────────────┘
                                ↓
┌──────────────────────────────────────────────────────────────┐
│                    TTY设备 (/dev/ttyUSBx)                     │
└──────────────────────────────────────────────────────────────┘
                                ↓
┌──────────────────────────────────────────────────────────────┐
│                    WiFi Soc (AT命令固件)                      │
└──────────────────────────────────────────────────────────────┘
```

**说明**：
- 应用层：bflbwifictrl + bflbwifid 组成守护进程方案（本模块提供）
- 您也可以直接链接 libbflbwifi.a 开发自己的应用程序，例如 examples
- 核心库：统一提供 WiFi 控制能力

**数据流（连接WiFi）**：
```
用户输入命令
    ↓
bflbwifictrl (CLI)
    ↓ Unix Socket (/tmp/bflbwifi.sock)
bflbwifid (守护进程)
    ↓ 调用 API
libbflbwifi.a (状态机→解析→发送AT命令)
    ↓ 串口通信
WiFi Soc (执行连接)
    ↑ 返回URC事件
libbflbwifi.a (解析响应)
    ↑
bflbwifid (封装结果)
    ↑ Unix Socket
bflbwifictrl (显示结果)
```

## 快速开始

### 1. 编译

```bash
make
```

生成：
- `libbflbwifi.a` - 核心静态库
- `app/bflbwifid` - 守护进程
- `app/bflbwifictrl` - CLI工具

### 2. 运行

```bash
# 启动守护进程（管理TTY设备）
sudo ./app/bflbwifid -p /dev/ttyACM1

# 另一个终端：使用CLI工具
./app/bflbwifictrl connect_ap ssid password
./app/bflbwifictrl disconnect
./app/bflbwifictrl scan
./app/bflbwifictrl status
```

### 3. 安装到系统

```bash
sudo make install
# 安装后可从任意位置使用：bflbwifictrl / bflbwifid
```

## CLI 工具命令

| 命令 | 说明 | 示例 |
|------|------|------|
| `connect_ap <ssid> [pwd]` | 连接WiFi（密码可选） | `./app/bflbwifictrl connect_ap ssid password` |
| `disconnect` | 断开连接 | `./app/bflbwifictrl disconnect` |
| `scan` | 扫描AP | `./app/bflbwifictrl scan` |
| `status` | 查询状态 | `./app/bflbwifictrl status` |
| `version` | 查看版本 | `./app/bflbwifictrl version` |
| `reboot` | 重启模块 | `./app/bflbwifictrl reboot` |
| `ota <file.bin>` | OTA固件升级 | `./app/bflbwifictrl ota firmware.bin` |
| `start_ap <ssid> [pwd]` | 启动SoftAP | `./app/bflbwifictrl start_ap myap password` |
| `stop_ap` | 停止SoftAP | `./app/bflbwifictrl stop_ap` |

守护进程选项：
- `-p <dev>` - TTY设备（默认：/dev/ttyUSB0）
- `-d` - 后台运行

**OTA注意事项**：
- OTA期间接收线程暂停，独占TTY访问
- 其他命令在OTA期间会被拒绝（返回错误码 -12）
- OTA完成后模块会自动重启
- 每发送512字节打印一次进度

## 编程接口（C API）

```c
#include "bflbwifi.h"

// 初始化
bflbwifi_init("/dev/ttyUSB0", 115200);

// 连接WiFi
bflbwifi_connect("ssid", "password", 30000);

// 查询信息
wifi_sta_info_t info;
bflbwifi_get_sta_info(&info);
printf("SSID: %s, RSSI: %d\n", info.ssid, info.rssi);

// 清理
bflbwifi_deinit();
```

完整API列表：`include/bflbwifi.h`

## 目录结构

```
bflbwifictrl/
├── include/          # 头文件
├── src/              # 核心库源代码
│   ├── bflb_tty.c    # TTY通信
│   ├── bflb_parser.c # AT协议解析
│   ├── bflb_state.c  # 状态管理
│   └── bflbwifi.c   # 公共API
├── app/              # 应用程序
│   ├── bflbwifid.c  # 守护进程
│   └── bflbwifictrl.c # CLI工具
└── Makefile          # 统一构建
```

## 特性

- ✅ **多进程安全**：守护进程架构，支持多客户端同时访问
- ✅ **线程安全**：核心库使用互斥锁保护共享资源
- ✅ **零警告编译**：严格遵循 `-Wall -Wextra` 标准
- ✅ **模块化设计**：分层架构，职责清晰
- ✅ **完整错误处理**：统一错误码，便于调试
- ✅ **OTA固件升级**：支持固件OTA升级，采用原子操作确保可靠性
- ✅ **版本查询**：支持查询固件版本信息
- ✅ **模块重启**：支持远程重启WiFi模块

## 依赖

- Linux 系统
- gcc 编译器
- pthread 库
- TTY串口设备（USB转串口或UART）

## 注意事项

1. **权限**：TTY设备需要读写权限（建议使用 sudo）
2. **设备路径**：根据硬件选择 `/dev/ttyUSBx` 或 `/dev/ttyACMx`
3. **守护进程**：使用守护进程方式可支持多客户端，单进程场景直接调用API即可
4. **OTA升级**：
   - OTA文件必须是绝对路径或相对于守护进程工作目录的路径
   - OTA文件大小限制：最大 16MB
   - OTA期间不要中断守护进程
   - OTA失败会自动恢复，不会影响正常使用
