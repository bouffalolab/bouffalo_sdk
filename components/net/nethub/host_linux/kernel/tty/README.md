# Host Driver TTY Driver (AI)

## 概述

Host Driver TTY 驱动程序是一个基于 Linux TTY 子系统的字符设备驱动程序，用于通过 SDIO 接口与 Host Driver 设备进行通信。该驱动程序实现了完整的 TTY 接口，支持状态机管理、流量控制和动态设备管理。

## 主要特性

- **标准 TTY 接口**：完全兼容 Linux TTY 子系统
- **状态机管理**：实现设备连接状态的自动管理
- **流量控制**：支持基于信用的流量控制机制
- **动态设备管理**：支持热插拔和设备状态变化
- **安全的引用管理**：正确处理 TTY 端口和设备的生命周期
- **错误处理**：完善的错误处理和恢复机制
- **与消息控制层集成**：通过 msg_ctrl 接口与底层 SDIO 通信

## 架构设计

### 系统架构

```
┌─────────────────────────────────────────────────────────────────┐
│                        User Space                              │
├─────────────────┬───────────────────┬───────────────────────────┤
│   Applications  │   Terminal Tools  │    System Commands        │
│   (custom apps) │   (picocom, etc.) │    (echo, cat, etc.)      │
└─────────────────┴───────────────────┴───────────────────────────┘
         │                    │                         │
         ▼                    ▼                         ▼
┌─────────────────────────────────────────────────────────────────┐
│                     Linux TTY Subsystem                        │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────────┐  │
│  │ TTY Core    │  │ Line Disc   │  │      TTY Driver         │  │
│  │ (tty_io.c)  │  │ (n_tty.c)   │  │    (hd_tty.c)           │  │
│  └─────────────┘  └─────────────┘  └─────────────────────────┘  │
└─────────────────────────────────────────────────────────────────┘
                                    │
                                    ▼
┌─────────────────────────────────────────────────────────────────┐
│                    HD TTY Driver Layer                         │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────────┐  │
│  │ TTY Ops     │  │ Port Ops    │  │    State Machine        │  │
│  │ (open,write,│  │ (activate,  │  │  (daemon thread)        │  │
│  │  close,etc.)│  │  shutdown)  │  │                         │  │
│  └─────────────┘  └─────────────┘  └─────────────────────────┘  │
│                                    │                           │
│  ┌─────────────┐  ┌─────────────┐  │  ┌─────────────────────┐  │
│  │Credit Flow  │  │ Statistics  │  │  │   Message Handling  │  │
│  │ Control     │  │ Management  │  │  │  (callbacks)        │  │
│  └─────────────┘  └─────────────┘  │  └─────────────────────┘  │
└────────────────────────────────────┼─────────────────────────────┘
                                    │
                                    ▼
┌─────────────────────────────────────────────────────────────────┐
│                   Message Control Layer                        │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────────┐  │
│  │ hd_msg_ctrl │  │ Message     │  │   Message Routing       │  │
│  │ Interface   │  │ Queue Mgmt  │  │   (tag-based)           │  │
│  └─────────────┘  └─────────────┘  └─────────────────────────┘  │
└─────────────────────────────────────────────────────────────────┘
                                    │
                                    ▼
┌─────────────────────────────────────────────────────────────────┐
│                     SDIO Transport Layer                       │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────────┐  │
│  │ SDIO Driver │  │ Buffer Mgmt │  │   Hardware Interface    │  │
│  │ (hd_sdio.c) │  │ (SKB)       │  │   (SDIO Controller)     │  │
│  └─────────────┘  └─────────────┘  └─────────────────────────┘  │
└─────────────────────────────────────────────────────────────────┘
                                    │
                                    ▼
┌─────────────────────────────────────────────────────────────────┐
│                      Host Driver Device                       │
└─────────────────────────────────────────────────────────────────┘
```

### 状态机

TTY 驱动程序使用状态机来管理与设备的连接状态：

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

**状态说明**：
- **RESET**: 初始状态，发送 HOST_RESET 命令
- **HOST_READY**: 主机就绪，等待设备启动确认
- **DEVICE_RUN**: 设备运行，可以正常收发数据
- **ERROR**: 错误状态，需要重新初始化

## 生命周期管理

### 1. 驱动程序生命周期

#### 初始化阶段 (`hd_tty_init`)

```c
struct hd_tty_priv *hd_tty_init(struct hd_msg_ctrl *msg_ctrl, uint8_t msg_tag)
```

**初始化步骤**：

1. **内存分配**：
   ```c
   priv = kzalloc(sizeof(struct hd_tty_priv), GFP_KERNEL);
   ```

2. **同步对象初始化**：
   ```c
   spin_lock_init(&priv->credit_lock);
   spin_lock_init(&priv->stats_lock);
   init_waitqueue_head(&priv->waitq);
   skb_queue_head_init(&priv->thread_skb_head);
   ```

3. **TTY 端口初始化**：
   ```c
   tty_port_init(&priv->port);
   priv->port.ops = &hd_tty_port_ops;
   ```

4. **TTY 驱动程序分配和配置**：
   ```c
   priv->tty_driver = tty_alloc_driver(1, HD_TTY_FLAGS);
   // 设置驱动程序属性
   tty_set_operations(priv->tty_driver, &hd_tty_ops);
   tty_port_link_device(&priv->port, priv->tty_driver, 0);
   ```

5. **注册 TTY 驱动程序**：
   ```c
   ret = tty_register_driver(priv->tty_driver);
   ```

6. **注册消息回调**：
   ```c
   ret = hd_msg_cb_register(priv->msg_ctrl, priv->msg_tag,
                            hd_tty_upld_recv_cb, priv,
                            hd_tty_dnld_send_cb, priv);
   ```

7. **启动守护线程**：
   ```c
   priv->daemon_thread = kthread_run(hd_tty_daemon_thread, priv, "hd_tty_daemon");
   ```

8. **注册 TTY 设备**：
   ```c
   priv->tty_dev = tty_port_register_device(&priv->port,
                                            priv->tty_driver, 0, NULL);
   ```

#### 清理阶段 (`hd_tty_deinit`)

**清理步骤（与初始化相反的顺序）**：

1. **注销消息回调**：
   ```c
   hd_msg_cb_unregister(priv->msg_ctrl, priv->msg_tag);
   ```

2. **挂断并注销设备**：
   ```c
   if (priv->is_device_registered) {
       tty_port_hangup(&priv->port);  // 通知用户空间
       tty_port_unregister_device(&priv->port, priv->tty_driver, 0);
   }
   ```

3. **停止守护线程**：
   ```c
   if (priv->daemon_thread) {
       priv->thread_condition = true;
       wake_up_interruptible(&priv->waitq);
       kthread_stop(priv->daemon_thread);
   }
   ```

4. **注销 TTY 驱动程序**：
   ```c
   tty_unregister_driver(priv->tty_driver);
   tty_driver_kref_put(priv->tty_driver);
   ```

5. **清理 TTY 端口**：
   ```c
   tty_port_destroy(&priv->port);
   ```

6. **清理待处理的 SKB**：
   ```c
   while ((skb = skb_dequeue(&priv->thread_skb_head)) != NULL) {
       dev_kfree_skb(skb);
   }
   ```

7. **释放内存**：
   ```c
   kfree(priv);
   ```

### 2. TTY 会话生命周期

#### 打开阶段 (`hd_tty_open` → `hd_tty_port_activate`)

```
用户进程调用 open("/dev/ttyHD0")
        │
        ▼
TTY 核心调用 hd_tty_open()
        │
        ▼
调用 tty_port_open() ──► hd_tty_port_activate()
        │                         │
        │                         ├─ 检查单一打开限制
        │                         ├─ 验证设备状态 (DEVICE_RUN)
        │                         ├─ 设置 is_open = true
        │                         └─ 发送 OPEN 命令到设备
        ▼
返回文件描述符给用户进程
```

#### 数据传输阶段

**写入数据流程**：
```
用户调用 write(fd, data, len)
        │
        ▼
TTY 核心调用 hd_tty_write_room() ──► 检查流量控制
        │
        ▼
TTY 核心调用 hd_tty_write()
        │
        ├─ 检查设备状态
        ├─ 获取流量控制信用
        ├─ 构建消息包
        ├─ 通过 msg_ctrl 发送
        └─ 返回写入字节数
```

**读取数据流程**：
```
设备发送数据 ──► SDIO 中断 ──► msg_ctrl 接收
        │
        ▼
hd_tty_upld_recv_cb() 被调用
        │
        ├─ 快速路径：直接处理数据包
        │   └─ hd_tty_process_data_packet()
        │       ├─ 获取 TTY 引用
        │       ├─ 插入到 flip buffer
        │       ├─ 推送到 TTY 核心
        │       └─ 释放 TTY 引用
        │
        └─ 慢速路径：队列控制包到守护线程
```

#### 关闭阶段 (`hd_tty_close` → `hd_tty_port_shutdown`)

```
用户进程调用 close(fd) 或进程退出
        │
        ▼
TTY 核心调用 hd_tty_close()
        │
        ▼
调用 tty_port_close() ──► hd_tty_port_shutdown()
        │                         │
        │                         ├─ 设置 is_open = false
        │                         └─ 发送 CLOSE 命令到设备
        ▼
释放 TTY 资源
```

### 3. 守护线程生命周期

守护线程 (`hd_tty_daemon_thread`) 管理设备状态和处理控制消息：

```c
static int hd_tty_daemon_thread(void *data)
{
    // 1. 发送初始 HOST_RESET 命令
    hd_tty_send_cmd_packet(priv, HD_TTY_FLAG_HOST_RESET);
    priv->tty_status = HD_TTY_HSTA_RESET;

    // 2. 主循环
    while (!kthread_should_stop()) {
        // 等待事件：SKB、条件变化、停止信号
        wait_event_interruptible(priv->waitq, 条件);

        // 处理设备重置（高优先级）
        if (收到 DEVICE_RESET) {
            tty_port_hangup(&priv->port);  // 立即释放用户空间引用
            priv->tty_status = HD_TTY_HSTA_RESET;
            continue;
        }

        // 状态机处理
        switch (priv->tty_status) {
            case HD_TTY_HSTA_RESET:
                发送 HOST_READY;
                转换到 HOST_READY 状态;
                break;

            case HD_TTY_HSTA_HOST_READY:
                等待 DEVICE_START;
                设置流量控制参数;
                转换到 DEVICE_RUN 状态;
                break;

            case HD_TTY_HSTA_DEVICE_RUN:
                处理数据包和流量控制更新;
                break;
        }
    }

    // 3. 清理退出
    发送 HOST_STOP 或 HOST_RESET;
    等待 kthread_stop();
}
```

## 与 msg_ctrl 的集成

### 1. msg_ctrl 接口

TTY 驱动程序通过 `msg_ctrl` 接口与底层 SDIO 传输层通信：

```c
struct hd_msg_ctrl {
    // 发送函数指针
    int (*send)(struct hd_msg_ctrl *ctrl, struct sk_buff *skb);

    // 回调注册/注销
    int (*register_callback)(struct hd_msg_ctrl *ctrl, uint8_t tag,
                           msg_recv_cb_t recv_cb, void *recv_arg,
                           msg_send_cb_t send_cb, void *send_arg);
    int (*unregister_callback)(struct hd_msg_ctrl *ctrl, uint8_t tag);

    // 其他控制接口...
};
```

### 2. 消息标签 (msg_tag)

每个 TTY 实例使用唯一的消息标签进行路由：

```c
#define HD_TTY_MSG_TAG  0x08  // TTY 消息标签

// 初始化时注册
hd_msg_cb_register(priv->msg_ctrl, HD_TTY_MSG_TAG,
                   hd_tty_upld_recv_cb, priv,      // 接收回调
                   hd_tty_dnld_send_cb, priv);     // 发送回调
```

### 3. 消息包格式

```c
struct hd_tty_msg_packt {
    struct hd_msg_packt msg_packt;      // 基础消息包头
    // ├─ uint8_t tag;                  // 消息标签 (HD_TTY_MSG_TAG)
    // ├─ uint8_t sub_tag;              // 子标签 (通常为 0)
    // └─ uint16_t len;                 // 载荷长度

    uint8_t reseved[1];                 // 保留字段
    uint8_t flag;                       // 命令/数据标志
    uint8_t credit_update_flag;         // 流量控制更新标志
    uint8_t credit_limit_cnt;           // 流量控制限制计数
    uint8_t data[];                     // 数据载荷（可变长度）
};
```

### 4. 发送路径

```c
static ssize_t hd_tty_write(struct tty_struct *tty, const unsigned char *buf, size_t count)
{
    // 1. 分配 SKB
    skb = dev_alloc_skb(sizeof(struct hd_tty_msg_packt) + data_len);

    // 2. 构建消息包
    tty_msg_packt = (struct hd_tty_msg_packt *)skb_put(skb, total_len);
    tty_msg_packt->msg_packt.tag = priv->msg_tag;     // 设置路由标签
    tty_msg_packt->msg_packt.len = payload_len;
    tty_msg_packt->flag = HD_TTY_FLAG_DNLD_DATA;
    memcpy(tty_msg_packt->data, buf, data_len);

    // 3. 通过 msg_ctrl 发送
    ret = hd_msg_ctrl_send(priv->msg_ctrl, skb);

    // 4. 错误处理
    if (ret < 0) {
        hd_tty_credit_put(priv);  // 归还流量控制信用
        dev_kfree_skb(skb);       // 释放 SKB
        return ret;
    }

    return data_len;
}
```

### 5. 接收路径

```c
static int hd_tty_upld_recv_cb(struct sk_buff *skb, bool success, void *arg)
{
    struct hd_tty_priv *priv = (struct hd_tty_priv *)arg;
    struct hd_tty_msg_packt *tty_msg_packt;

    // 1. 验证包大小
    if (skb->len < sizeof(struct hd_tty_msg_packt)) {
        // 错误处理
        return 0;
    }

    tty_msg_packt = (struct hd_tty_msg_packt *)skb->data;

    // 2. 快速路径：运行状态下的数据包
    if (priv->tty_status == HD_TTY_HSTA_DEVICE_RUN &&
        (tty_msg_packt->flag == HD_TTY_FLAG_UPLD_DATA ||
         tty_msg_packt->flag == HD_TTY_FLAG_CREDIT_UPDATE)) {

        // 更新流量控制
        if (tty_msg_packt->credit_update_flag) {
            hd_tty_credit_limit_update(priv, tty_msg_packt->credit_limit_cnt);
        }

        // 处理数据
        if (tty_msg_packt->flag == HD_TTY_FLAG_UPLD_DATA) {
            hd_tty_process_data_packet(priv, tty_msg_packt);
        }

        dev_kfree_skb_any(skb);
        return 0;
    }

    // 3. 慢速路径：控制包队列到守护线程
    skb_queue_tail(&priv->thread_skb_head, skb);
    wake_up_interruptible(&priv->waitq);

    return 0;
}
```

### 6. 发送完成回调

```c
static int hd_tty_dnld_send_cb(struct sk_buff *skb, bool success, void *arg)
{
    struct hd_tty_priv *priv = (struct hd_tty_priv *)arg;

    // 1. 解析消息包
    if (skb->len >= sizeof(struct hd_tty_msg_packt)) {
        tty_msg_packt = (struct hd_tty_msg_packt *)skb->data;
        if (tty_msg_packt->flag == HD_TTY_FLAG_DNLD_DATA) {
            data_len = HD_TTY_GET_DATA_LEN(tty_msg_packt->msg_packt.len);
        }
    }

    // 2. 更新统计信息
    if (success) {
        if (data_len > 0) {
            hd_tty_update_tx_stats(priv, 1, data_len);
        }
    } else {
        if (data_len > 0) {
            hd_tty_update_tx_stats(priv, -1, 0);
            hd_tty_credit_put(priv);  // 发送失败，归还信用
        }
    }

    return 0;
}
```

## 消息协议

### 消息类型定义

| 标志值 | 名称 | 方向 | 描述 | 载荷 |
|--------|------|------|------|------|
| 0x00 | HOST_RESET | H→D | 主机重置命令 | 无 |
| 0x01 | HOST_READY | H→D | 主机就绪 | 无 |
| 0x02 | HOST_STOP | H→D | 主机停止 | 无 |
| 0x03 | DEVICE_START | D→H | 设备启动 | 流量控制参数 |
| 0x04 | DEVICE_STOP | D→H | 设备停止 | 无 |
| 0x05 | DEVICE_RESET | D→H | 设备重置 | 无 |
| 0x08 | UPLD_DATA | D→H | 上行数据 | 用户数据 |
| 0x09 | DNLD_DATA | H→D | 下行数据 | 用户数据 |
| 0x0A | OPEN | H→D | 打开连接 | 无 |
| 0x0B | CLOSE | H→D | 关闭连接 | 无 |
| 0x0C | HANGUP | H→D | 挂断连接 | 无 |
| 0x0D | CREDIT_UPDATE | D→H | 流量控制更新 | 信用参数 |

### 流量控制协议

流量控制使用基于信用的机制：

1. **初始化**：设备在 `DEVICE_START` 消息中告知初始信用限制
2. **信用消费**：每发送一个数据包消费一个信用
3. **信用更新**：设备定期发送 `CREDIT_UPDATE` 消息更新可用信用
4. **流量控制**：当信用耗尽时，暂停发送直到收到更新

```c
// 信用管理函数
static int hd_tty_credit_get(struct hd_tty_priv *priv);     // 获取一个信用
static int hd_tty_credit_put(struct hd_tty_priv *priv);     // 释放一个信用
static int hd_tty_credit_limit_update(struct hd_tty_priv *priv, uint8_t limit);
```

## 文件结构

```
tty/
├── hd_tty.c          # 主要驱动程序实现
├── hd_tty.h          # 头文件定义和数据结构
└── README.md         # 本文档
```

## 使用方法

### 基本用法

1. **加载模块**：
   ```bash
   sudo insmod hd_sdio.ko
   ```

2. **检查设备**：
   ```bash
   ls -l /dev/ttyHD*
   # 应该显示: crw-rw-rw- 1 root tty 236, 0 ... /dev/ttyHD0
   ```

3. **发送数据**：
   ```bash
   # 方法1：直接重定向（需要适当权限）
   echo "Hello,Device" > /dev/ttyHD0

   # 方法2：使用 sudo（正确方式）
   sudo sh -c 'echo "Hello,Device" > /dev/ttyHD0'

   # 方法3：使用 tee
   echo "Hello,Device" | sudo tee /dev/ttyHD0 > /dev/null
   ```

4. **交互式通信**：
   ```bash
   # 使用 picocom
   picocom -b 115200 /dev/ttyHD0

   # 使用 minicom
   minicom -D /dev/ttyHD0

   # 使用 screen
   screen /dev/ttyHD0 115200
   ```

### 权限说明

⚠️ **重要提示**：

- `sudo echo "data" > /dev/ttyHD0` **不会工作**，因为重定向由当前用户的 shell 处理
- 使用 `sudo sh -c 'echo "data" > /dev/ttyHD0'` 或 `echo "data" | sudo tee /dev/ttyHD0` 代替

## 调试和监控

### 内核日志

查看驱动程序日志：
```bash
# 实时查看日志
dmesg -w | grep hd_tty

# 查看最近日志
dmesg | tail -20

# 查看特定类型日志
dmesg | grep -E "(hd_tty|ttyHD|msg_ctrl)"
```

### 统计信息

驱动程序维护以下统计信息：
- `tx_packets`: 发送包数量
- `tx_bytes`: 发送字节数
- `tx_errors`: 发送错误数
- `rx_packets`: 接收包数量
- `rx_bytes`: 接收字节数
- `rx_errors`: 接收错误数

### 调试宏

代码中使用了以下调试宏：
- `TTY_ERR()`: 错误信息
- `TTY_INFO()`: 一般信息
- `TTY_DBG()`: 调试信息

### 状态监控

```bash
# 检查模块状态
lsmod | grep hd_sdio

# 检查 TTY 驱动程序
cat /proc/tty/drivers | grep ttyHD

# 检查设备节点
ls -la /dev/ttyHD*

# 通过 debugfs 查看详细状态（如果可用）
cat /sys/kernel/debug/hd_sdio/tty_status
```

## 错误处理

### 常见错误

1. **设备不可用** (`-ENODEV`)：
   - 设备未连接或已断开
   - 检查硬件连接和设备状态

2. **设备忙** (`-EBUSY`)：
   - TTY 设备已被其他进程打开
   - 确保没有其他程序占用设备

3. **输入/输出错误** (`-EIO`)：
   - 设备状态不正确
   - 等待设备进入运行状态

4. **再试一次** (`-EAGAIN`)：
   - 设备暂时不可用
   - 稍后重试操作

### 错误恢复机制

驱动程序具有多层错误恢复机制：

1. **自动重连**：检测到设备重置时自动重新初始化状态机
2. **资源清理**：错误发生时自动清理分配的资源
3. **用户通知**：通过 hangup 机制通知用户空间程序
4. **统计记录**：记录错误统计信息用于问题诊断

## 性能考虑

### 优化特性

1. **快速路径处理**：
   - 数据包在运行状态下直接在接收回调中处理
   - 避免不必要的线程切换和队列操作

2. **异步消息处理**：
   - 使用专用守护线程处理控制消息
   - 避免阻塞数据传输路径

3. **高效内存管理**：
   - 使用 SKB 缓冲区进行零拷贝传输
   - 及时释放不需要的内存

4. **锁优化**：
   - 使用自旋锁保护关键数据结构
   - 最小化锁持有时间

### 性能参数

- **最大数据长度**：`HD_TTY_MAX_DATA_LEN` (1520 字节)
- **信用限制**：由设备动态设置，通常为 8-16
- **缓冲区策略**：无本地缓冲，直接传输到 TTY 核心

## 故障排除

### 问题诊断步骤

1. **检查模块状态**：
   ```bash
   lsmod | grep hd_sdio
   cat /proc/modules | grep hd_sdio
   ```

2. **检查设备节点**：
   ```bash
   ls -l /dev/ttyHD*
   cat /proc/tty/drivers | grep ttyHD
   ```

3. **检查内核日志**：
   ```bash
   dmesg | grep -E "(hd_tty|ttyHD|hd_sdio|msg_ctrl)"
   ```

4. **检查设备状态**：
   ```bash
   # 通过 debugfs 检查（如果可用）
   cat /sys/kernel/debug/hd_sdio/status
   cat /sys/kernel/debug/hd_sdio/msg_ctrl_status
   ```

### 常见问题解决

**Q: 无法发送数据**
- A: 检查设备状态，确保处于 `DEVICE_RUN` 状态
- A: 检查权限和命令语法
- A: 验证 msg_ctrl 连接状态

**Q: 设备节点不存在**
- A: 检查模块是否正确加载
- A: 检查硬件连接
- A: 验证 SDIO 控制器状态

**Q: 模块无法卸载**
- A: 确保所有 TTY 连接已关闭
- A: 检查是否有进程占用设备
- A: 强制关闭相关进程后重试

**Q: 数据传输中断**
- A: 检查 SDIO 连接稳定性
- A: 查看是否有设备重置事件
- A: 验证流量控制状态

## 注意事项

1. **单实例限制**：每个 TTY 设备只支持一个打开的连接
2. **状态依赖**：只有在 `DEVICE_RUN` 状态下才能进行数据传输
3. **内存管理**：确保正确释放 SKB 缓冲区
4. **线程安全**：所有接口都是线程安全的
5. **msg_ctrl 依赖**：需要底层 msg_ctrl 接口正常工作
6. **硬件依赖**：依赖 SDIO 硬件接口的稳定性

## 版本历史

- **v1.0**: 初始版本，基本 TTY 功能
- **v1.1**: 添加流量控制支持
- **v1.2**: 改进错误处理和状态管理
- **v1.3**: 优化性能和内存管理
- **v1.4**: 完善生命周期管理和文档

## 许可证

本驱动程序遵循 GPL v2 许可证。

## 作者

- mlwang
- 创建日期：2025-08-07

## 技术支持

如有问题或建议，请联系开发团队或查看项目文档。
