# HTTP、MQTT 与云连接

应用协议之前需确认网络接口已取得 IP、DNS 可用，并根据目标平台配置选择本地或跨核 socket；基础网络和 Socket Offload 分别见 [17-tcpip-network.md](17-tcpip-network.md)、[18-ipc-offload.md](18-ipc-offload.md)。

## MQTT-C

`CONFIG_MQTT` 启用 `components/net/lib/mqtt/` 的 MQTT-C；`examples/wifi/sta/wifi_mqtt/` 提供普通 TCP 和 TLS 发布、订阅示例。调用前先建立到 broker 的 socket/TLS 连接，再按 `mqtt.h` 的真实签名向 `mqtt_init` 提供 client、socket handle、发送和接收缓冲区及发布回调。client 持有这些对象的指针，它们须在使用期间保持有效，缓冲不足有对应错误，发送缓冲区还应按指针访问需求对齐。

`mqtt_connect`、`mqtt_publish` 等先将报文入发送队列，应用必须周期调用 `mqtt_sync()` 执行收发、ACK 和回调；CONNECT 入队成功不代表 broker 已接受。`mqtt_publish` 的长度参数是实际消息字节数，接收 topic 以长度表示，不能假定 NUL 结尾。重连模式由 `mqtt_init_reconnect` 提供回调入口，应用仍需清理旧连接、`mqtt_reinit`、重新连接和恢复订阅，不要对同一 client 重复初始化。

TLS 示例的固定 hostname、随机数和连接错误处理限制见 [11-security.md](11-security.md)；其客户端证书配置在示例中被注释，不能当作双向认证已启用。`examples/wifi/sta/aws_iot/` 是基于 MQTT 的云应用示例；实际 endpoint、凭据、证书和云端策略由部署环境决定。 当前 BL616 默认 mbedTLS v3 构建在 `mbedtls_pk_parse_key` 参数数量处失败；显式 `CONFIG_MBEDTLS_V2=y` 的隔离构建通过。此结果不证明云端认证或收发成功。

## HTTP / HTTPS

`components/net/lib/http/` 的高层 `https_client_request()` 接受完整 URL、方法、可选响应/上传回调和证书，内部建立连接、分配缓冲、请求后释放；默认接收缓冲为 1024 字节。底层 `http_client_req()` 使用调用方已建立的连接，timeout 单位为毫秒。响应可能分片，通过 `HTTP_DATA_MORE` / `HTTP_DATA_FINAL` 和长度处理；不设回调时缓冲不足可能截断。上传可以用 `payload_cb` 分段，具体分块约定见头文件。`examples/wifi/sta/wifi_http` 和 `wifi_https` 展示请求调用。

HTTPS 传 CA 时启用服务端证书验证；未传 CA 则尝试配置的证书 bundle，bundle 未启用或附加失败时当前实现降级到 `MBEDTLS_SSL_VERIFY_NONE`。使用 HTTPS URL 本身不证明服务端身份已验证；需要检查 CA/bundle、服务器名称和握手结果，并用错误 CA/名称进行负面验证。

HTTP server 的服务与资源通过 `HTTP_SERVICE_DEFINE` / `HTTP_RESOURCE_DEFINE` 等注册；动态响应通过回调及 final 状态处理。升级 WebSocket 后应用负责 socket 读写和关闭。lwIP 也有独立的 `httpd` / `httpc`，不要混同两组 API；具体项目是否编入以构建配置为准。

## WebSocket 和其他入口

`components/multimedia/xwebsocket` 是独立客户端，`examples/wifi/sta/websocket/` 启用 `CONFIG_XWEBSOCKET` 和 multimedia。检查连接、断开资源释放、回调数据是否分片以及 TLS 和主机名验证配置；不要假定与 MQTT-C 自定义 socket 支持可互换。

OpenThread CLI 的项目配置启用 CoAP、CoAP Secure 和 SNTP 客户端，这是 Thread 路径；lwIP 虽有 SNTP 源文件，当前 SDK lwIP CMake 的 apps 列表未直接加入 `sntp.c`，默认时间设置钩子为空。使用前核实工程是否实际编译并设置系统时间。协议工程构建成功不能证明 broker、云权限、证书、协议互操作或持续重连正常。
