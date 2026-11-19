# 多核 IPC 与 Socket Offload

适用范围：以下以当前 BL618DG 示例和 `components/ipc/`、`components/net/socket_offload/` 为依据。AP、NP、LP 的实际职责由具体工程决定；硬件 IPC 也可以不用 RPMsg（见 `examples/peripherals/ipc/ipc_thrcore/`）。

## RPMsg 数据路径和配置

`examples/openamp/` 的 AP 是 virtio driver / RPMsg master，NP 是 device / remote。消息及队列放在共享内存，IPM 通道触发对端中断，接收任务再处理 virtqueue；`ipm_send` 在当前移植里只用作通知，不能把其参数当作传输的数据。两侧须协调共享内存布局、角色和 `CONFIG_OPENAMP_RPMSG_BUFFER_SIZE`；后者包含 RPMsg 头，示例为 1600，示例两端还将 `VRING_SIZE` 设为 4。默认共享区和 vring 常量在 `components/ipc/libmetal/CMakeLists.txt`，不要将其地址直接移植到其他芯片或版本。核查目标内存映射、容量、缓存属性与双方构建参数；源码中的缓存维护宏是否启用需要结合实际编译配置判断。

`CONFIG_IPC` 启用 IPC，`CONFIG_RPMSG_SERVICE` 启用服务；master 侧 `CONFIG_RPMSG_SERVICE_MODE_MASTER=y`。在**双方启动服务前**用相同名称调用 `rpmsg_service_register_endpoint(name, callback)`，成功返回非负 endpoint ID；之后 `rpmsg_service_start()`，收到数据进入 callback，`rpmsg_service_send(id, data, len)` 返回发送字节数或负错误码。服务默认保留三个注册槽；名称字符串需在注册后持续有效。master 可用 `rpmsg_service_endpoint_is_bound(id)` 等待远端公告；当前 remote 分支不设置该 bound 标志，不要以它在 remote 为真作为就绪条件。API 签名和回调类型见 `components/ipc/rpmsg_service/rpmsg_service.h` 与 OpenAMP `rpmsg.h`，完整注册和任务调度参考 `examples/openamp/ipc_throughput_test.c`。

## Socket Offload

`examples/wifi/sta/dualcore/` 中 AP 为 client，NP 为 service，NP 运行 lwIP 和网络接口。两侧均配置 FreeRTOS、IPC、RPMsg service、lwIP 和 `CONFIG_SOCKET_OFFLOAD`，分别选 `CONFIG_SOCKET_OFFLOAD_CLIENT` / `CONFIG_SOCKET_OFFLOAD_SERVICE`。两核 RPMsg buffer 配置须一致；service 要求 `LWIP_NETCONN_FULLDUPLEX=1` 和 `LWIP_NETCONN_SEM_PER_THREAD=1`，两侧须支持 FreeRTOS 静态分配。例程 AP 先 `rpmsg_master_init()`，再 `rpmsg_and_rpc_init()`、`socket_offload_init()`；NP 初始化网络栈，在任务中依次 `rpmsg_slave_init()`、`rpc_service_init()`。各初始化返回值都应检查；开始网络通信前还须等待网络就绪。

client 包含 `socket_offload.h` 后，配置会将 `socket`、`send`、`recv` 等宏映射到 RPC，也会映射 `close`；混用本地文件描述符时要注意作用域。调用要求任务上下文，只支持 IPv4 TCP/UDP。公开 socket 接口失败返回 -1 并设置 `errno`；初始化接口返回负 errno。调用和停用顺序、协议限制以组件 README 为准。

- TCP 大发送按 RPMsg 帧分段并可能部分发送，应检查返回字节数。UDP 发送如果跨多个 RPMsg 帧，会变成多个独立数据报，不能把这种拆分当成原数据报的透明分片。
- 默认配置下 UDP 接收数据上限为 1552 字节；超限和 `MSG_PEEK`、调用方缓冲区不足的具体处理见 `components/net/socket_offload/README.md`。同一 socket 同时只允许一个待处理 receive，另一调用返回 `EBUSY`。
- `getsockname` / `getpeername` 对有效参数返回 `ENOTSUP`，`accept` 不提供对端地址，`ioctl` 仅支持 `FIONBIO`。不要假定完整 POSIX 行为。
- 默认控制 RPC 超时 5000 ms，socket RPC 默认无限等待；RPC 超时不会取消远端操作。发送缓冲不足可能返回 `ENOBUFS`；设置 socket 收发超时与设置 RPC 超时是不同机制。关闭服务前停止 socket 调用。

## 验证与源码入口

- `examples/openamp/`：RPMsg endpoint 双核运行及吞吐测试。
- `examples/wifi/sta/dualcore/`：Socket Offload 与无线网络。`wifi_ap` 和 `wifi_np` 可分别构建；顶层 `dualcore.mk` 包装还负责组合镜像，并会写顶层 `defconfig`，隔离验证时检查其副作用。
- `components/ipc/rpmsg_service/`、`components/ipc/ipm/`：service、共享内存 backend 与硬件通知。
- `components/net/socket_offload/README.md`、`src/ipc_socket.c`、`src/socket_rpc_service.c`：接口范围、协议、错误和 worker。

单核构建成功不证明双核镜像组合、端点绑定、数据完整性或实际联网成功；这些需要目标设备的双核运行验证。
