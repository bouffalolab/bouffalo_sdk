# BFLB 虚拟通道 (bflbvchan)

基于 Netlink 的博流虚拟通道用户空间库和应用程序，用于与内核模块进行数据通信。

## 目录

- [1. 功能特性](#1-功能特性)
- [2. 快速开始](#2-快速开始)
- [3. API 文档](#3-api-文档)
- [4. 数据类型](#4-数据类型)
- [5. 文件结构](#5-文件结构)
- [6. 故障排除](#6-故障排除)

## 1. 功能特性

- **多数据类型支持**：SYSTEM、WIFI、BLE、USER、AT
- **基于类型的回调**：为不同数据类型注册独立的接收回调函数
- **便捷 API**：提供 USER 和 AT 数据类型的便捷封装接口
- **线程安全**：内置接收线程，自动处理异步消息
- **流控支持**：内置流控机制，防止数据丢失

## 2. 快速开始

### 前置条件

确保内核 Netlink 模块已加载：

```bash
sudo modprobe hd_netlink
# 或
sudo insmod hd_netlink.ko
```

### 编译和运行

```bash
# 编译
make

# 运行测试程序（需要 root 权限）
sudo ./bflbvchan_app
```

### 交互式命令示例

```
netlink> send user 12345678
Sent 8 bytes (type=USER)

netlink> send wifi "wifitestdata"
Sent 14 bytes (type=WIFI)

netlink> send at "AT+GMR"
Sent 8 bytes (type=AT)

netlink> test flow 1000 512
[FLOW_TEST] Starting flow control test: count=1000, size=512
[FLOW_TEST] Progress: 100/1000 packets sent (10.0%)
[FLOW_TEST] Progress: 200/1000 packets sent (20.0%)
...
[FLOW_TEST] Send test completed
[FLOW_TEST] Total packets:   1000
[FLOW_TEST] Sent successfully: 1000 packets (100.00%)
[FLOW_TEST] Failed:          0 packets (0.00%)

netlink> status
Connection: Active

[Kernel ↔ Device via SDIO/USB]
  TX: 1523 packets (dropped: 0)
  RX: 1245 packets (dropped: 0)
  Pending Queue: 0 packets

[Kernel ↔ Userspace via Netlink]
  RX from userspace: 1008 packets
  TX to userspace: 1523 packets (dropped: 0)

netlink> help
Available commands:
  send [type] <string>   Send data with optional type
                         Types: system, wifi, ble, user, at
  test flow <cnt> <sz>   Flow control test (send count packets)
  test mtu <size>        MTU test (send data with specified size)
  status                 Show connection status
  help                   Show this help message
  quit/exit              Exit program

netlink> quit
Bye!
```

## 3. API 文档

### 核心 API

```c
#include "bflbvchan.h"

/* 初始化和清理 */
int bflbvchan_init(void);
int bflbvchan_deinit(void);

/* 数据发送 */
int bflbvchan_send(uint8_t type, const uint8_t *data, size_t len);

/* 回调注册 */
int bflbvchan_register_callback(uint8_t data_type, bflbvchan_recv_callback_t callback);
int bflbvchan_unregister_callback(uint8_t data_type);
```

### USER 类型便捷 API

```c
/* 发送 USER 数据 */
int bflbvchan_user_send(const uint8_t *data, size_t len);

/* 注册 USER 回调 */
int bflbvchan_user_register_callback(bflbvchan_recv_callback_t callback);
```

### AT 类型便捷 API

```c
/* 发送 AT 数据 */
int bflbvchan_at_send(const uint8_t *data, size_t len);

/* 注册 AT 回调 */
int bflbvchan_at_register_callback(bflbvchan_recv_callback_t callback);
```

### 代码示例

```c
#include "bflbvchan.h"

/* USER 数据接收回调 */
static void user_handler(const void *data, size_t len) {
    printf("USER: Received %zu bytes: %.*s\n", len, (int)len, (const char *)data);
}

/* AT 数据接收回调 */
static void at_handler(const void *data, size_t len) {
    printf("AT: Received %zu bytes\n", len);
    // 处理 AT 命令响应
}

int main() {
    /* 初始化虚拟通道 */
    if (bflbvchan_init() < 0) {
        fprintf(stderr, "Failed to initialize bflbvchan\n");
        return 1;
    }

    /* 注册回调函数 */
    bflbvchan_register_callback(BFLBVCHAN_DATA_TYPE_USER, user_handler);
    bflbvchan_at_register_callback(at_handler);

    /* 发送数据 */
    bflbvchan_user_send((const uint8_t *)"Hello", 5);
    bflbvchan_at_send((const uint8_t *)"AT+GMR", 6);

    /* 主循环 */
    while (1) {
        sleep(1);
    }

    /* 清理 */
    bflbvchan_deinit();
    return 0;
}
```

## 4. 数据类型

| 类型 | 值 | 说明 |
|------|-----|------|
| `BFLBVCHAN_DATA_TYPE_SYSTEM` | 0x01 | 系统消息 |
| `BFLBVCHAN_DATA_TYPE_WIFI` | 0x02 | WiFi 相关数据 |
| `BFLBVCHAN_DATA_TYPE_BLE` | 0x03 | BLE 相关数据 |
| `BFLBVCHAN_DATA_TYPE_USER` | 0x04 | 用户数据（默认） |
| `BFLBVCHAN_DATA_TYPE_AT` | 0x05 | AT 命令数据 |

## 5. 文件结构

```
bflbvchan/
├── bflbvchan_protocol.h    # 协议定义（数据类型、消息头结构）
├── bflbvchan.h             # API 头文件
├── bflbvchan.c             # 核心 Netlink 通信库实现
├── bflbvchan_app.c         # 交互式命令行测试应用
├── Makefile                # 构建脚本
├── README_CN.md            # 中文文档
└── README.md               # 英文文档
```

## 6. 故障排除

### 初始化失败

```
Failed to initialize netlink: -1
Make sure kernel module is loaded (sudo modprobe hd_netlink)
```

**解决方案**：确保内核模块已加载
```bash
sudo modprobe hd_netlink
# 验证模块已加载
lsmod | grep hd_netlink
```

### 权限拒绝

```
bind: Permission denied
```

**解决方案**：使用 sudo 运行程序，或授予 CAP_NET_RAW 权限
```bash
sudo ./bflbvchan_app
# 或
sudo setcap cap_net_raw+ep ./bflbvchan_app
./bflbvchan_app
```

### 数据发送失败

```
sendto: Operation not permitted
```

**解决方案**：确保以 root 权限运行，Netlink 通信需要特权操作。

### 编译警告

编译时启用 `-Wall -Wextra` 选项，确保无警告编译。

```bash
make clean && make
```
