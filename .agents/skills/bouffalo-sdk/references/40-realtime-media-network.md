# 实时音视频网络

以下路径相对 SDK 根目录。协议组件、采集/编解码、信令和真实网络互通分别验证。

## WebRTC

`components/net/lib/webrtc_peer/` 是 `libpeer` 的应用接口，底层提供 ICE、DTLS-SRTP、RTP、SCTP；启用 `CONFIG_LIBPEER` 选择 lwIP、mbedTLS、SRTP，`CONFIG_WEBRTC_PEER` 另启 facade。应用提供音视频采集、编解码和信令。文档支持 Opus/PCMA/PCMU 音频、H.264 RTP 视频；MJPEG 使用可选 DataChannel 适配，不能仅据枚举存在认定 VP8 或标准 JPEG RTP 已端到端支持。

单连接由一个 owner 任务持续调用 `webrtc_peer_main_loop()`；跨任务请求与多句柄 open/close 按 README 串行化。回调在 owner 同步执行，不要在回调中重入销毁。默认每连接队列视频 332800、音频 8096、数据 80960 字节，仅启用对应流才分配，还需计入协议、栈和 codec。内建 SCTP 默认消息上限 16384 字节、无完整拥塞控制；TURN 限 UDP/IPv4，不支持连接内重新协商或 ICE restart。

验证远端 SDP 的来源：DTLS 证书指纹依赖经认证的信令，未认证的 SDP 不能确立对端身份。AppRTC 的信任锚、URL 限制和 mbedTLS v2/v3 配置见组件 README。

## RTSP

`components/net/lib/rtsp/` 提供单 owner RTSP/1.0 server；`rtsp_server_open/poll/get_stats/close` 接收应用视频及可选音频源，单任务每 1–5 ms poll。支持单客户端、RTP/JPEG、单声道 PCMA、UDP 单播或 TCP interleaved；没有认证、TLS、H.264 packetization、组播或 ONVIF。公开接口和 JPEG 格式限制见 `rtsp/README.md`。应用必须根据部署环境控制访问；无认证或 TLS 不能用于需要保密和身份鉴别的链路。

## 示例与测试

`examples/solution/OpenAI_bl618/` 使用原生 libpeer、Opus 和远程实时语音服务，不是新的 facade 音视频示例。当前源码会打印读取的 API key 和含 Bearer key 的 HTTP 请求；其构建没有定义 `BL_VERIFY`，HTTPS 默认不验证服务端证书。避免暴露凭据并在产品中处理真实认证；源码检查不等于已连接服务。

`libpeer/tests/host/run_host_tests.py` 验证主机模拟环境下的 SCTP、ICE、缓冲和音频逻辑；DTLS v2/v3 有独立回归。`rtsp/tests/run_host_tests.py` 构造 JPEG 并使用 TCP/UDP 回归；其生产 server 会绑定所有 IPv4 接口，执行时先确认隔离和访问范围。它们不能替代真实网络、目标设备、时钟、信令认证和编解码互通。

本工作区 GCC 14.3.0 下 BL616 OpenAI 示例构建在 `oai_config.h` 的 `uint8_t` 未定义处失败；libpeer 主机逻辑回归通过。两者均不验证实际云连接。
