# NetHub Host Linux 架构设计文档

## 系统架构图

```
┌─────────────────────────────────────────────────────────────────┐
│                        用户空间 (Userspace)                      │
├─────────────────┬───────────────────┬───────────────────────────┤
│ bflbwifictrl    │ bflbvirtualchannel│ 应用程序                   │
│ (CLI 工具)      │ (虚拟通道)        │ (自定义)                   │
│                 │                   │                           │
│ bflbwifid       │                   │                           │
│ (守护进程)      │                   │                           │
└────────┬────────┴─────────┬─────────┴───────────┬───────────────┘
         │                  │                       │
         ▼                  ▼                       ▼
┌─────────────────────────────────────────────────────────────────┐
│              内核空间 (Kernel Space)                             │
├─────────────────┬───────────┬───────────────┬───────────────────┤
│ TTY 驱动        │ 网卡驱动   │ Netlink驱动   │ SDIO 驱动         │
│ (/dev/ttyHD0)   │ (hd_eth0)  │              │                   │
└────────┬────────┴─────┬─────┴───────┬───────┴────────┬───────────┘
         │              │            │                 │
         ▼              ▼            ▼                 ▼
┌─────────────────────────────────────────────────────────────────┐
│                消息控制层 (hd_msg_ctrl)                          │
│           - 消息路由 (tag-based)                                 │
│           - 回调管理                                             │
│           - 统计信息                                             │
└───────────────────────────────┬─────────────────────────────────┘
                                │
                                ▼
┌─────────────────────────────────────────────────────────────────┐
│                SDIO 传输层 (hd_sdio_manage)                     │
│           - SDIO2/SDIO3 硬件抽象                                │
│           - 队列管理                                             │
│           - 中断处理                                             │
└───────────────────────────────┬─────────────────────────────────┘
                                │
                                ▼
┌─────────────────────────────────────────────────────────────────┐
│                    设备端 (Device Side)                          │
│            (BL616/BL808 WiFi SoC)                               │
│            - AT 命令解析                                        │
│            - WiFi 协议栈                                        │
│            - 以太网协议转换                                     │
└─────────────────────────────────────────────────────────────────┘
```

## 数据流向

### 1. WiFi 控制流程 (AT 命令)

```
用户命令
  ↓
bflbwifictrl (CLI)
  ↓ Unix Socket (/tmp/bflbwifi.sock)
bflbwifid (守护进程)
  ↓ 调用 API
libbflbwifi.a
  ↓ TTY 写入
/dev/ttyHD0
  ↓ 内核 TTY 驱动
hd_msg_ctrl (msg_tag=0x08)
  ↓ SDIO 传输
WiFi 设备 (AT 命令执行)
```

### 2. 网络数据流程 (以太网)

```
网络应用
  ↓
Linux 网络栈
  ↓ skb
hd_eth_netdev (网卡驱动)
  ↓ 消息封装 (msg_tag=0x04)
hd_msg_ctrl (消息路由)
  ↓ SDIO 传输
WiFi 设备 (WiFi 发送)
```

### 3. TTY 数据流向

**发送路径 (用户 → 设备)**:
```
用户进程 write()
  ↓
TTY 核心
  ↓
hd_tty_write()
  ↓
构建消息包 (msg_tag=0x08)
  ↓
hd_msg_ctrl_send()
  ↓
SDIO 硬件传输
```

**接收路径 (设备 → 用户)**:
```
SDIO 中断
  ↓
hd_sdio_manage (工作队列)
  ↓
hd_msg_ctrl (消息路由)
  ↓
hd_tty_upld_recv_cb()
  ↓
TTY flip buffer
  ↓
用户进程 read()
```

## 关键组件说明

| 组件 | 位置 | 功能 |
|------|------|------|
| **hd_sdio** | kernel/sdio_drv/ | SDIO 硬件抽象层，支持 SDIO2/SDIO3 |
| **hd_msg_ctrl** | kernel/msg_ctrl/ | 消息路由中心，基于 tag 进行消息分发 |
| **hd_eth_netdev** | kernel/eth_netdev/ | 虚拟网卡驱动，提供 hd_eth0 接口 |
| **hd_tty** | kernel/tty/ | TTY 字符设备驱动，提供 /dev/ttyHD0 |
| **bflbwifictrl** | userspace/bflbwifictrl/ | WiFi 控制模块（守护进程+CLI） |
| **bflbvirtualchannel** | userspace/bflbvirtualchannel/ | 用户私有数据通信通道 |

## 消息标签 (msg_tag) 定义

```c
enum {
    HD_MSG_TAG_NONE = 0,      // 无特定标签
    HD_MSG_TAG_TEST_1,        // 测试消息标签 1
    HD_MSG_TAG_TEST_2,        // 测试消息标签 2
    HD_MSG_TAG_TEST_3,        // 测试消息标签 3
    HD_MSG_TAG_SYS,           // 系统消息
    HD_MSG_TAG_WIFI,          // WiFi 子系统消息
    HD_MSG_TAG_ETH = 0x04,    // 以太网子系统消息
    HD_MSG_TAG_TTY = 0x08,    // TTY/UART 子系统消息
    HD_MSG_TAG_NETLINK = 0x09,// Netlink 子系统消息
    HD_MSG_TAG_MAX            // 最大标签值（边界标记）
};
```

## 状态机

### Host 端状态机

```
     开机/ifconfig up
          │
          ▼
+-------------------+
│  HD_ETH_HSTA_RESET │
│  发送 HOST_READY   |
+-------------------+
         │
         │ 收到 DEVICE_READY (含MAC)
         ▼
+-------------------------+
│ HD_ETH_HSTA_DEVICE_READY │
│ 设置 MAC 地址           |
+-------------------------+
         │
         │ 收到 DEVICE_START (含初始信用)
         ▼
+----------------------+
│ HD_ETH_HSTA_DEVICE_RUN │
│ netif_carrier_on     |
│ 开始数据传输          |
+----------------------+
   │                  │
   │ 收到 DEVICE_STOP │ 正常运行
   +------------------+
         │
         │ 错误/异常
         ▼
+-------------------+
│ HD_ETH_HSTA_ERROR  │
│ 错误恢复状态       │
+-------------------+
```

### TTY 状态机

```
     ┌─────────────┐
     │    RESET    │ ◄─── 初始状态/错误恢复
     └──────┬──────┘
            │ HOST_READY
            ▼
     ┌─────────────┐
     │ HOST_READY  │ ◄─── 等待设备响应
     └──────┬──────┘
            │ DEVICE_START
            ▼
     ┌─────────────┐
     │ DEVICE_RUN  │ ◄─── 正常数据传输
     └──────┬──────┘
            │ DEVICE_RESET/STOP
            ▼
     ┌─────────────┐
     │   ERROR     │ ◄─── 错误状态
     └─────────────┘
```

### Device 端状态机

```
+-------------------+
│  DEV_STA_RESET    │
│  等待 HOST_READY  |
+-------------------+
         │
         │ 收到 HOST_READY
         ▼
+----------------------+
│  DEV_STA_HANDSHAKE   │
│  发送 DEVICE_READY   │
│  发送 DEVICE_START   │
+----------------------+
         │
         ▼
+-------------------+
│   DEV_STA_RUN     │
│   正常数据传输    │
+-------------------+
```

## 流量控制机制

采用基于信用的流控机制：

1. **初始化**：设备在 `DEVICE_START` 消息中告知初始信用限制
2. **信用消费**：每发送一个数据包消费一个信用
3. **信用更新**：设备定期发送 `CREDIT_UPDATE` 消息更新可用信用
4. **流量控制**：当信用耗尽时，暂停发送直到收到更新

```c
// 信用管理
struct {
    uint32_t credit_consumed_cnt;  // 已消费信用计数
    uint32_t credit_limit_cnt;     // 信用限制上限
};

// 发送前检查
if ((s32)(credit_limit_cnt - credit_consumed_cnt) > 0) {
    // 允许发送
    credit_consumed_cnt++;
} else {
    // 信用耗尽，暂停发送
    netif_stop_queue(netdev);
}
```

## AT 命令协议

### 基础命令

| 命令 | 说明 | 示例 |
|------|------|------|
| `AT` | 测试 AT 启动 | `AT` |
| `AT+GMR` | 查看版本信息 | `AT+GMR` |
| `AT+RST` | 重启模块 | `AT+RST` |
| `AT+CWMODE` | 设置 Wi-Fi 模式 | `AT+CWMODE=1` |
| `AT+CWJAP` | 连接到 AP | `AT+CWJAP="ssid","password"` |
| `AT+CWQAP` | 断开与 AP 的连接 | `AT+CWQAP` |
| `AT+CWLAP` | 扫描 AP | `AT+CWLAP` |
| `AT+CWSAP` | 配置 SoftAP | `AT+CWSAP="ssid","pwd",ch,enc` |

## 编译和使用

### 编译内核驱动
```bash
cd kernel
make
```

### 编译用户空间应用
```bash
cd userspace/bflbwifictrl
make
```

### 加载驱动
```bash
sudo insmod kernel/sdio/hd_sdio.ko
```

### 启动守护进程
```bash
cd userspace/bflbwifictrl
sudo ./app/bflbwifid -p /dev/ttyHD0
```

### 使用 CLI 工具
```bash
# 连接 WiFi
./app/bflbwifictrl connect_ap ssid password

# 查询状态
./app/bflbwifictrl status

# 扫描 AP
./app/bflbwifictrl scan

# 断开连接
./app/bflbwifictrl disconnect
```

## 设备节点

加载驱动后，系统会创建以下设备：

1. **hd_eth0** - 网卡接口（STA 转以太网功能）
2. **/dev/ttyHD0** - TTY 控制通道（AT 命令通信）

## 网络配置注意事项

由于 DHCP client 运行在 device 侧，host 侧需要：

1. **关闭 hd_eth0 的 DHCP 服务**
2. **使用静态 IP 配置**
3. **确保 host 和 device 使用相同的 IP 地址**

配置示例：
```bash
# 查询 device 侧 IP
./app/bflbwifictrl status

# 配置 host 侧静态 IP（与 device 侧相同）
sudo ip addr add 192.168.1.100/24 dev hd_eth0
sudo ip link set hd_eth0 up
```

## 版本历史

- v1.0 - 初始架构设计
- 更新日期: 2025-01-30
