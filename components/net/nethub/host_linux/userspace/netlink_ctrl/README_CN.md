# Netlink 控制库

用于与内核 Netlink 模块通信的 Netlink 用户空间库和应用程序。

## 功能特性

- **基于类型的回调**：为不同数据类型注册独立的回调函数
- **多进程支持**：多个进程可以同时接收数据
- **数据类型支持**：SYSTEM、WIFI、BLE、USER、LOG
- **流控支持**：内置基于信用的流控机制
- **易于使用**：简单的 API 和交互式命令行界面

## 编译

```bash
cd netlink_ctrl
make
```

## 快速开始

### 启动应用程序

```bash
./netlink_app
```

### 交互式命令

```
netlink> send user 12345678
Sent 8 bytes (type=USER)

netlink> send wifi "wifitestdata"
Sent 14 bytes (type=WIFI)

netlink> send ble "bletestdata"
Sent 13 bytes (type=BLE)

netlink> test flow 1000 512
[FLOW_TEST] Starting flow control test: count=1000, size=512
[FLOW_TEST] Received 100 packets, 51200 bytes, 0 lost
[FLOW_TEST] Received 200 packets, 102400 bytes, 0 lost
...
[FLOW_TEST] Received 1000 packets, 512000 bytes, 0 lost

netlink> test mtu 1024
[MTU_TEST] Testing MTU: size=1024 bytes
[MTU_TEST] Successfully sent 1024 bytes

[Received] 7 bytes (type=USER): Hello from device

netlink> status
Connection: Active
Sent: 1003 packets
Received: 1008 packets

netlink> help
Available commands:
  send [type] <string>   Send data with optional type
                         Types: system, wifi, ble, user, log
  test flow <cnt> <sz>   Flow control test (send count packets)
  test mtu <size>        MTU test (send data with specified size)
  status                 Show connection status
  help                   Show this help message
  quit/exit              Exit program

netlink> quit
Bye!
```

## 前置条件

使用前，请确保内核 Netlink 模块已加载：

```bash
# 加载模块
sudo insmod hd_netlink.ko

# 检查 dmesg
dmesg | grep Netlink
```

预期输出：
```
Netlink: Netlink module initialized successfully
Netlink: Device Ready! Credit Limit=3
```

## 安装

```bash
# 安装到系统
sudo make install

# 卸载
sudo make uninstall
```

## 作为库使用

### API 概述

```c
#include "netlink_ctrl.h"
#include "netlink_protocol.h"

/* 初始化 */
netlink_handle_t handle;
netlink_init(&handle);

/* 为特定数据类型注册回调函数 */
netlink_register_callback(handle, NETLINK_DATA_TYPE_WIFI, wifi_callback);
netlink_register_callback(handle, NETLINK_DATA_TYPE_BLE, ble_callback);

/* 发送数据 */
const char *data = "Hello";
netlink_send(handle, NETLINK_DATA_TYPE_USER, data, strlen(data));

/* 接收数据（会调用回调函数） */
void wifi_callback(const void *data, size_t len) {
    // 处理 WiFi 数据
}

/* 清理 */
netlink_deinit(handle);
```

### 示例程序

```c
#include "netlink_ctrl.h"
#include "netlink_protocol.h"

static void wifi_handler(const void *data, size_t len) {
    printf("WiFi: Received %zu bytes\n", len);
    // 处理 WiFi 数据
}

static void ble_handler(const void *data, size_t len) {
    printf("BLE: Received %zu bytes\n", len);
    // 处理 BLE 数据
}

int main() {
    netlink_handle_t handle;

    /* 初始化 */
    netlink_init(&handle);

    /* 注册回调函数 */
    netlink_register_callback(handle, NETLINK_DATA_TYPE_WIFI, wifi_handler);
    netlink_register_callback(handle, NETLINK_DATA_TYPE_BLE, ble_handler);

    /* 发送数据 */
    netlink_send(handle, NETLINK_DATA_TYPE_WIFI, "scan", 4);

    /* 在主循环或线程中接收数据 */
    // ...

    /* 清理 */
    netlink_deinit(handle);
    return 0;
}
```

## 数据类型

| 类型 | 值 | 描述 |
|------|-------|-------------|
| **SYSTEM** | 0x01 | 系统消息 |
| **WIFI** | 0x02 | WiFi 相关数据 |
| **BLE** | 0x03 | BLE 相关数据 |
| **USER** | 0x04 | 用户数据（默认） |
| **LOG** | 0x05 | 日志数据 |

## 多进程支持

多个进程可以同时运行并接收数据：

```bash
# 终端 1
./netlink_app

# 终端 2
./netlink_app

# 两个终端都会接收到从设备发送的数据
```

## 文件结构

```
netlink_ctrl/
├── netlink_protocol.h    # 协议定义（数据类型、消息头）
├── netlink_ctrl.h        # API 头文件
├── netlink_ctrl.c        # 核心 Netlink 通信库
├── netlink_app.c         # 交互式命令行应用
├── Makefile               # 构建脚本
└── README.md             # 英文文档
```

## 数据格式

所有通过 Netlink 发送的数据都包含一个类型头部：

```
+----------+------------+--------+
| data_type| reserved   | data[] |
| 1 byte   | 3 bytes    | N bytes|
+----------+------------+--------+
```

- `data_type`：指定数据类型（SYSTEM/WIFI/BLE/USER/LOG）
- `reserved[3]`：预留字段，用于未来扩展
- `data[]`：实际用户数据

## 所需权限

### 接收多播消息
要从内核接收多播消息，应用程序需要：

```bash
# 方式 1：使用 sudo 运行
sudo ./netlink_app

# 方式 2：授予 CAP_NET_RAW 权限
sudo setcap cap_net_raw+ep ./netlink_app
```

### 发送数据
无需特殊权限。任何用户空间进程都可以发送数据。

## 故障排除

### 初始化 Netlink 失败
```
Error: Failed to initialize netlink: -1
```
**解决方案**：确保内核模块已加载：
```bash
sudo insmod hd_netlink.ko
```

### 权限拒绝
```
bind: Permission denied
```
**解决方案**：使用 sudo 运行或授予 CAP_NET_RAW 权限（见上文）

### 没有接收到数据
**检查**：
1. 内核模块是否已加载？(`lsmod | grep hd_netlink`)
2. 回调函数是否已注册？（应用程序应显示 "Registered" 消息）
3. 设备是否在发送数据？（检查内核日志：`dmesg | tail -20`）

## 许可证

请参阅各个文件中的许可证信息。

## 联系方式

如有问题或建议，请联系开发团队。
