# lwIP 与 TCP/IP 网络基础

先确认目标工程的 `CONFIG_LWIP`、实际 `lwipopts_user.h` / `lwipopts.h`、Wi-Fi 栈和网络接口；配置选项与对端条件不同，不能假定所有示例的 DNS、IPv6、socket 超时或 DHCP 行为一致。当前 lwIP 源码的版本宏是 2.1.2，但 SDK 源码可能带本地改动。

## 组件入口

- lwIP 与网络服务：`components/net/lwip/`、`components/net/Kconfig`；ping 实现在 `components/net/lwip/lwip_apps/ping/`。DHCP 服务端与 STA 的 DHCP client 是不同功能。
- 吞吐测试：`components/iperf/README_cn.md`、`components/iperf/`。这是 Classic iPerf2，不能与 iPerf3 互测；与其他注册同名 `iperf` 命令的实现可能冲突。
- 跨核 socket：`components/net/socket_offload/README.md`，示例 `examples/wifi/sta/dualcore/`；它通过 RPC 使用另一核心上的网络栈，区别于 NetHub 主机联网。见 [18-ipc-offload.md](18-ipc-offload.md)。

## 初始化与配置

`components/net/Kconfig` 的 `LWIP` 控制组件编译，`DHCPD` 控制 DHCP **服务端**，与 STA DHCP client 不同。`opt.h` 在存在 `lwipopts_user.h` 时优先包含它，否则使用 `lwipopts.h`；未定义项取 lwIP 默认值。检查具体工程的 include 路径及预处理结果，不要只看另一个示例的选项。

所查通用和 Wi-Fi 6 配置使用 `NO_SYS=0`。此模式下 `tcpip_init()` 初始化 lwIP、创建 mailbox 和 TCP/IP 线程；完成回调在线程中执行。Wi-Fi 6 FHOST 例子 `examples/wifi/sta/wifi_tcp/` 与 wl80211 的 `examples/wifi/macsw_bare/` 都初始化 lwIP，但各自 netif 和启动路径不同。线程栈、优先级、pbuf 和 TCP 缓冲以目标配置为准；raw API 和 netif 操作应遵循所选 lwIP 线程上下文规则。

## 连接、地址和 DNS

Wi-Fi 关联成功不等于已经拿到 IP。检查选中栈的连接、DHCP 和 IP 事件，再启动需要网络的业务。FHOST 与 wl80211 事件细节不相同：当前 FHOST 的 IP 获取回调发送 `CODE_WIFI_ON_GOT_IP`，value 为 0；wl80211 兼容路径用同一事件的 value 1 表示非零地址、value 0 表示地址为零。不要跨栈复用对 value 的假设。

wl80211 通常由 `use_dhcp` 控制连接后是否启动 DHCP client；启用 P2P 的 CLIENT 路径会强制使用 DHCP。在 `COMPAT_WIFI_MGMR` 路径，`wifi_mgmr_sta_ip_set` 可以设置 netif 地址并在 DNS 非零时更新 DNS，尽管 public 头文件的注释称该接口未实现。FHOST 的 STA 静态地址路径会先停止相应 DHCP session。静态 IP 使用的字节序和调用时机应按选中头文件确认；DNS 服务器属于 lwIP 全局配置，修改时需考虑其他接口。

`CONFIG_DHCPD` 开启 AP DHCP 服务端不代表具备可用 DNS、路由或互联网转发。当前 DHCPD 例程默认地址是 `192.168.169.1`、地址池末字节 2–254，具体应以实际接口配置为准；启动通过 `tcpip_callback` 异步调度。STA/AP 同时使用时避免子网重叠，核对默认 netif 和路由；在普通 `ip4_route()` 路径中，当前实现按接口链表寻找匹配网络，再考虑 hook/default，不可假定是最长前缀匹配；源路由 hook 和指定组播接口等条件可改变选择次序。

## TCP 与 UDP Socket

TCP 是字节流，收发可能部分完成；UDP 保留数据报边界。接口及实际用法见 `components/net/lwip/` 和目标 TCP/UDP 示例。

调用方负责创建、绑定、退出和关闭 socket；二进制数据不能直接当 NUL 结尾字符串打印。`examples/wifi/sta/wifi_tcp/`、`examples/wifi/sta/wifi_udp/` 和 `examples/wifi/macsw_bare/` 可定位工程配置及 API，但其示例代码也需审查部分发送、错误和资源释放，不应直接当完整生产模板。

## DNS 与 IPv6

确认目标的 `LWIP_DNS`、`LWIP_SOCKET` 及 DNS 服务器地址；DNS 启用与 AP 提供可用 DNS 是不同事情。`getaddrinfo` 成功后应遍历/选择适用地址并 `freeaddrinfo`，处理解析和连接错误；示例入口 `examples/wifi/sta/wifi_http/`。如需 socket 发送/接收超时，核对 `LWIP_SO_RCVTIMEO`、`LWIP_SO_SNDTIMEO` 及 `LWIP_SO_SNDRCVTIMEO_NONSTANDARD`。

IPv6 取决于目标 `LWIP_IPV6`、地址自动配置和适配器；部分工程启用它。头文件中标注有状态 DHCPv6 尚不支持，启用 DHCP6 不能单独证明有状态分配可用。

## 分层排查

依次记录无线关联/认证、netif link、IP/网关/DNS、目标解析、TCP 连接或 UDP 路由、应用收发；同时确认监听端口、socket 返回值与 `errno`、接收端及实际路由。构建通过、获得 IP、连通对端和持续流量分别是不同验证阶段。多网卡、IPv6、TLS 或跨核 socket 需结合实际协议和适配层进一步验证。
