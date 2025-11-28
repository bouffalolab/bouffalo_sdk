# SDIO Ethernet 网络设备驱动技术文档 (AI)

## 1. 概述

`hd_eth_netdev` 是一个基于 SDIO 传输的虚拟以太网网络设备驱动程序，构建在 `msg_ctrl` 消息控制框架之上。该驱动实现了标准的 Linux 网络设备接口，允许系统通过 SDIO 通道进行网络通信。

### 1.1 核心设计理念

驱动采用 **控制与数据分离** 的设计架构：

* **控制平面**: 专用内核线程处理设备状态协商、握手和错误恢复
* **数据平面**: 高性能的数据包收发路径，针对低延迟优化

### 1.2 主要特性

* 标准 Linux 网络设备接口 (netdev)
* 基于消息的双向通信协议
* 信用流控机制防止缓冲区溢出
* 双路径接收处理 (快速/慢速路径)
* 精确的统计和错误处理
* 线程安全的并发控制

## 2. 架构设计

### 2.1 统一的回调命名规范

驱动采用清晰的命名约定来表达数据流向：

* **`hd_eth_dnld_send_cb`**: 下行数据发送完成回调 (Host → Device)
* **`hd_eth_upld_recv_cb`**: 上行数据接收处理回调 (Device → Host)

### 2.2 状态机管理

#### 2.2.1 Host 端状态机

```text
    开机/ifconfig up
         |
         v
+-------------------+
|  HD_ETH_HSTA_RESET |
|  发送 HOST_READY   |
+-------------------+
         |
         | 收到 DEVICE_READY (含MAC)
         v
+-------------------------+
| HD_ETH_HSTA_DEVICE_READY |
| 设置 MAC 地址           |
+-------------------------+
         |
         | 收到 DEVICE_START (含初始信用)
         v
+----------------------+
| HD_ETH_HSTA_DEVICE_RUN |
| netif_carrier_on     |
| 开始数据传输          |
+----------------------+
   ^                  |
   | 收到 DEVICE_STOP | 正常运行
   +------------------+
         |
         | 错误/异常
         v
+-------------------+
| HD_ETH_HSTA_ERROR  |
| 错误恢复状态       |
+-------------------+
```

**状态说明**:

* **RESET**: 初始状态，发送 `HOST_READY` 握手
* **DEVICE_READY**: 收到设备就绪，解析 MAC 地址
* **DEVICE_RUN**: 链路激活，可进行数据传输
* **ERROR**: 异常状态，自动恢复机制

#### 2.2.2 Device 端状态机（建议实现）

```text
+-------------------+
|  DEV_STA_RESET    |
|  等待 HOST_READY  |
+-------------------+
         |
         | 收到 HOST_READY
         v
+----------------------+
|  DEV_STA_HANDSHAKE   |
|  发送 DEVICE_READY   |
|  发送 DEVICE_START   |
+----------------------+
         |
         v
+-------------------+
|   DEV_STA_RUN     |
|   正常数据传输    |
+-------------------+
```

## 3. 数据通路设计

### 3.1 发送路径 (TX Path)

```text
Linux Network Stack
       |
       | skb
       v
+------------------------+
| ndo_start_xmit         |
| ·状态检查              |
| ·SKB 验证              |
| ·流控检查 (原子)       |
| ·信用消耗              |
+------------------------+
       |
       | hd_msg_ctrl_send()
       v
+------------------------+
| SDIO Hardware Layer    |
+------------------------+
       |
       | 异步完成
       v
+------------------------+
| hd_eth_dnld_send_cb    |
| ·更新统计              |
| ·处理发送结果          |
| ·流控信用管理          |
+------------------------+
```

#### 3.1.1 发送流程

1. **状态检查**: 验证设备是否处于 `DEVICE_RUN` 状态
2. **SKB 验证**: 检查数据包长度和头部空间
3. **流控检查**: 原子操作检查和消耗信用额度
4. **封装发送**: 添加消息头部，调用底层传输
5. **异步完成**: 通过回调处理发送结果

#### 3.1.2 发送完成回调的改进

```c
static int hd_eth_dnld_send_cb(struct sk_buff *skb, bool success, void *arg)
{
    if (success) {
        // 成功: 更新统计计数器
        hd_eth_update_tx_stats(priv, 1, data_len);
    } else {
        // 失败: 返还信用，记录错误
        spin_lock(&priv->credit_lock);
        priv->credit_consumed_cnt--;  // 关键：返还信用
        spin_unlock(&priv->credit_lock);
        hd_eth_update_tx_stats(priv, -1, 0);
    }
    dev_kfree_skb_any(skb);
    return 0;
}
```

**关键特性**:
* 精确的成功/失败统计
* 智能信用管理：失败时自动返还信用
* 统一的 SKB 生命周期管理

### 3.2 接收路径 (RX Path)

```text
SDIO Hardware Layer
       |
       | skb + header
       v
+------------------------+
| msg_ctrl workqueue     |
+------------------------+
       |
       v
+------------------------+
| hd_eth_upld_recv_cb    |
| ·快速路径判断          |
| ·流控更新              |
+------------------------+
    |              |
    |Fast Path     |Slow Path
    v              v
+----------+  +-------------+
|数据包处理|  |状态机处理   |
|netif_rx  |  |daemon thread|
+----------+  +-------------+
```

#### 3.2.1 双路径接收处理

**快速路径 (Fast Path)**:
* 适用于 `DEVICE_RUN` 状态下的数据包和信用更新
* 在工作队列上下文中直接处理
* 避免唤醒守护线程的开销
* 实现最低接收延迟

**慢速路径 (Slow Path)**:
* 处理控制消息和状态转换
* 通过队列传递给守护线程
* 在进程上下文中安全处理复杂逻辑

### 3.3 线程管理

#### 3.3.1 守护线程设计

```c
while (!kthread_should_stop()) {
    wait_event_interruptible(priv->waitq,
                             !skb_queue_empty(&priv->thread_skb_head) ||
                             kthread_should_stop() ||
                             priv->eth_status == HD_ETH_HSTA_RESET);

    // 处理接收到的控制消息
    // 执行状态机转换
}
```

#### 3.3.2 线程停止机制

```c
// 停止序列
hd_msg_cb_unregister(priv->msg_ctrl, priv->msg_tag);  // 1. 停止新消息
wake_up_interruptible(&priv->waitq);                  // 2. 唤醒线程
kthread_stop(priv->daemon_thread);                    // 3. 等待退出
```

**关键设计**:
* `wake_up_interruptible()` 确保线程被唤醒
* `kthread_should_stop()` 作为退出条件
* 优雅的资源清理

## 4. 流控机制

### 4.1 基于信用的流控设计

采用滚动计数器的信用流控机制，借鉴 PCIe 流控原理：

```c
// 发送前检查
spin_lock(&priv->credit_lock);
if ((s32)(priv->credit_limit_cnt - priv->credit_consumed_cnt) <= 0) {
    netif_stop_queue(netdev);
    return NETDEV_TX_BUSY;
}
priv->credit_consumed_cnt++;  // 消耗信用
spin_unlock(&priv->credit_lock);
```

### 4.2 信用计数器

* **`credit_consumed_cnt`**: Host 端维护，每发送一包 +1
* **`credit_limit_cnt`**: Device 端通知，表示处理能力上限

### 4.3 防溢出设计

使用 `(s32)` 强制转换处理计数器回绕问题：

```c
// 安全的信用检查
if ((s32)(credit_limit_cnt - credit_consumed_cnt) > 0) {
    // 允许发送
} else {
    // 信用耗尽，暂停发送
}
```

### 4.4 队列管理

* **暂停队列**: `netif_stop_queue()` - 信用耗尽时
* **唤醒队列**: `netif_wake_queue()` - 收到信用更新且从 0 变为可用时

## 5. 统计和调试

### 5.1 统计信息

驱动提供详细的网络统计：

* TX: packets, bytes, errors, dropped
* RX: packets, bytes, errors
* 流控: credit_consumed_cnt, credit_limit_cnt

### 5.2 错误处理

* 传输失败自动返还信用
* SKB 内存管理
* 状态异常自动恢复

## 6. 性能优化

### 6.1 并发优化

* 细粒度锁设计 (`stats_lock`, `credit_lock`)
* 原子操作减少锁竞争
* 快速路径避免线程切换

### 6.2 内存优化

* SKB 头部空间预留
* 统一的内存生命周期管理
* 避免不必要的内存拷贝

## 7. 使用示例

### 7.1 设备启动

```bash
# 加载驱动模块
insmod hd_sdio_drv.ko

# 启动网络接口
ifconfig hd_eth0 192.168.1.100 netmask 255.255.255.0 up

# 查看统计信息
cat /proc/net/dev | grep hd_eth
```

### 7.2 调试信息

```bash
# 查看设备状态
cat /sys/class/net/hd_eth0/operstate

# 查看链路信息
ethtool hd_eth0
```

## 8. 开发指南

### 8.1 添加新功能

1. 确保遵循统一的回调命名规范
2. 在适当的路径中处理（快速/慢速）
3. 正确管理锁和并发
4. 更新相应的统计信息

### 8.2 调试建议

1. 使用 `ETH_NETDEV_INFO/ERR` 宏输出调试信息
2. 监控信用流控状态
3. 检查状态机转换逻辑
4. 验证 SKB 生命周期管理

---

*文档版本: v2.0*
*最后更新: 2025-01-20*
*作者: mlwang*
