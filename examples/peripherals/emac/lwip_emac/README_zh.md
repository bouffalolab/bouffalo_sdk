# lwip_emac

[English](README.md) | [中文](README_zh.md)

本示例通过一套 lwIP 协议栈上的 EMAC 网络接口提供 TCP Loopback、UDP Echo、HTTP 和 iperf 服务。BL618DG 同时启用 EMAC0 和 EMAC1，单 EMAC 芯片保持一个网络接口。每个活动端口独立维护 PHY、DMA Buffer、Queue、RX Task、MAC 地址、netif、DHCP Client、Link 状态和统计信息。

## 服务和 Shell 命令

| 服务 | 默认配置 | 启动命令 | 停止命令 |
|:----:|:--------:|:---------|:---------|
| TCP Loopback Server | TCP 3365 | `tcp_server start [port]` | `tcp_server stop` |
| TCP Loopback Client | TCP 3365 | `tcp_client start <ip> [port]` | `tcp_client stop` |
| UDP Echo Server | UDP 3365 | `udp_echo start [port]` | `udp_echo stop` |
| 内置 HTTP Server | TCP 80 | 自动启动 | 长期运行 |

由于 TCP 和 UDP 是不同的传输层协议，因此二者可以使用相同的端口号。TCP 和 UDP 服务由 Shell 控制，不会自动启动；未指定端口时使用相应的默认端口。lwIP 内置 HTTP Server 在网络初始化后自动启动，并长期运行于 TCP 80 端口，不提供 Shell 启停命令。

普通 PHY Link Down/Up 期间服务仍可继续使用。工程默认为每个活动接口启用 DHCP，日志使用 `[EMAC0]`、`[EMAC1]` 标识物理端口。接口 Link Up 后 30 秒仍未获得租约时输出一次 `[EMACx] DHCP timeout`，lwIP 会继续重试。BL618DG 固定由 EMAC1 使用工厂 MAC，EMAC0 使用兼容旧双口版本的派生 MAC。

工程启用了用于吞吐量测试的 iperf Shell 命令，以及用于查看每口统计的 `lwip_emac_info` 命令。

## 实现概述

### 共享 EMAC 和 lwIP 初始化

`main.c` 初始化 Shell，然后调用 `lwip_emac_start()`。共享启动模块配置 RMII/MDIO 引脚、启动 lwIP TCP/IP 线程，并为每个已配置 EMAC 添加独立 netif 和 DHCP Client。BL618DG 先初始化配置的首选口（默认为 EMAC0）并将其作为默认接口；只有该口在启动阶段初始化失败时，才使用第一个存活接口作为默认接口，不停止另一个端口。运行中的 Link Down 不会切换默认接口。

`lwip_emac_start()` 是任务上下文的一次性启动 API，只能调用一次；示例不提供运行期 EMAC 停止或重启。某个端口在启动阶段失败时，只回滚该端口已经创建的资源并继续尝试另一端口；致命启动失败则按逆序回滚已经初始化的端口。

一个常驻后台任务周期性查询所有活动 PHY 的 Link 状态，并同步更新对应的 lwIP Link 状态。EMAC TX/RX 队列和硬件 Buffer Descriptor 在启动阶段创建；普通 Link Down/Up 只更新 Link 状态，不会停止 EMAC 或重建 Descriptor。

### 服务生命周期和 Shell 控制

TCP 和 UDP 注册为 Shell 命令，启动阶段不会自动运行。每个 `start` 命令通过 `xTaskCreate()` 动态创建对应的服务任务。每个 `stop` 命令设置停止标志；较短的 Socket 超时使任务能够及时检查该标志，关闭网络资源、清空任务句柄并自行删除。HTTP 使用 lwIP 基于 Raw API 回调驱动的内置服务，运行在 TCP/IP 线程上下文中，并作为长期服务统一管理。

### TCP Loopback Server

TCP Server 使用 lwIP BSD Socket API。它创建 Stream Socket，绑定指定的本地端口，进入监听状态并接受客户端连接。收到字节流后，服务端确保将全部数据回送给当前客户端。收发超时使任务可以周期性响应停止请求。服务端每次处理一个连接；当前客户端断开后，再接受下一个客户端。

Server 在 `tcp/tcp_server.c` 中独立实现，不与 TCP Client 共享任务状态、缓冲区或生命周期控制。

### TCP Loopback Client

TCP Client 首先校验 Shell 指定的 IPv4 地址，然后执行非阻塞 `connect()`，使用一次 `select()` 等待最多 1 秒，再检查连接结果。连接成功后，Client 等待远端发送数据，并确保将每次收到的全部字节原样回送，其 Loopback 行为与 TCP Server 和 UDP Echo 服务一致。目标端口默认为 3365，例如：`tcp_client start 192.168.1.220 5000`。

Client 在 `tcp/tcp_client.c` 中独立实现，无需启动本地 TCP Server 即可工作。

### UDP Echo Server

UDP Server 使用绑定到指定本地端口的 Datagram Socket。每次通过 `recvfrom()` 接收到一个 Datagram 后，使用 `sendto()` 将其原样回送给发送方。短接收超时保证即使没有数据到达，任务也能检查停止标志。

### HTTP Server

工程启用 lwIP 基于回调驱动的内置 `httpd`。静态文件生成在 `http_server/web_demo/fsdata_custom.c` 中；lwIP `fs.c` 模块通过 `HTTPD_FSDATA_FILE` 包含该文件，`httpd` 直接根据请求 URI 查询 fsdata。请求解析、静态响应、404 选择和并发 TCP 连接均在 lwIP TCP/IP 线程中处理。

文档页面通过 HTTP GET 请求加载对应的文本文件，并在 `<pre>` 元素中显示响应内容。文本资源通过 `INCBIN` 嵌入固件，再由 lwIP Custom Files API 提供访问。HTTP Server 在 TCP 80 端口启动一次并长期运行。

lwIP 官方同时提供 `makefsdata` 脚本和功能更完整的 C 控制台工具，用于将网页资源目录转换为 `fsdata.c`。需要根据 `http_server/web_demo/pages/` 下的静态文件重新生成 `http_server/web_demo/fsdata_custom.c` 时，请参考 [lwIP 官方 makefsdata 使用介绍](https://github.com/lwip-tcpip/lwip/blob/master/src/apps/http/makefsdata/readme.txt) 和 [lwIP 官方 makefsdata 源码目录](https://github.com/lwip-tcpip/lwip/tree/master/src/apps/http/makefsdata)。

### iperf 吞吐量测试

工程启用了 SDK iperf 组件及其 Shell 集成。iperf Shell 命令使用共享的 lwIP 协议栈和 EMAC netif 进行吞吐量测试。

## 源码目录

- `lwip_emac_start()` / `lwip_emac_port()`：板级引脚、lwIP/netif 初始化、DHCP、PHY Link 监控与端口管理，源码位于 `bsp/common/eth_phy/lwip_emac_start/`。
- `tcp/tcp_server.c`：独立的、基于 Socket 的 TCP Loopback Server。
- `tcp/tcp_client.c`：独立的、基于 Socket 的 TCP Loopback Client。
- `udp/`：基于 Socket 的 UDP Echo Server。
- `http_server/`：lwIP 内置 httpd 集成、INCBIN README 资源和 fsdata 网页资源。
- `config_file/`：共享的 FreeRTOS 和 lwIP 配置。

## 支持的芯片

| 芯片 | 备注 |
|:----:|:----:|
| BL702/BL704/BL706 | |
| BL702L/BL704L | |
| BL616/BL618 | |
| BL618DG | AP Core |
| BL616CL | |

## 编译

- BL702/BL704/BL706

```
make CHIP=bl702 BOARD=bl702dk
```

- BL702L/BL704L

```
make CHIP=bl702l BOARD=bl702ldk
```

- BL616/BL618

```
make CHIP=bl616 BOARD=bl616dk
```

- BL618DG

```
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap
```

- BL616CL

```
make CHIP=bl616cl BOARD=bl616cldk
```

## 测试

HTTP Server 在网络初始化后已经运行。先在开发板 Shell 中查看每口状态：

```
lwip_emac_info
```

BL618DG 会分别输出 EMAC0 和 EMAC1 的 TX/RX 摘要及可用 DMA Descriptor 数量。

DHCP 获取地址后，再启动 TCP 和 UDP 服务：

```
tcp_server start
udp_echo start
```

需要时可以指定自定义端口：

```
tcp_server start 5000
udp_echo start 5001
tcp_client start 192.168.1.220 5000
```

将以下命令中的 `<board-ip>` 替换为串口输出的开发板 IPv4 地址，然后在主机侧执行测试：

```
nc <board-ip> 3365
nc -u <board-ip> 3365
curl http://<board-ip>/
curl http://<board-ip>/docs/readme-en.md
curl http://<board-ip>/docs/readme-zh.md
```

测试开发板 TCP Client 时，先在主机侧启动 TCP Listener，然后执行 `tcp_client start <host-ip> [port]`。主机通过该连接发送的数据会由开发板原样返回。各服务可通过相应的 `stop` 命令独立停止。

## 烧录

```
make flash CHIP=chip_name COMX=xxx
```
