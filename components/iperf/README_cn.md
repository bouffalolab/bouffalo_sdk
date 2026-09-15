# 精简版 iPerf（Classic iPerf2）

[English](README.md)

`components/iperf` 是面向嵌入式 SDK 的精简 IPv4 吞吐量测试组件。它支持
TCP/UDP client 和 server，并可选择 lwIP Socket API 或 lwIP Raw API 后端。

组件和 Shell 命令名为 `iperf`，但实现的是 Classic iPerf2 normal mode。
本组件不支持 iPerf3，也不能与 iPerf3 对端互测。

## 快速上手

### 1. 启用组件

在应用配置中启用 FreeRTOS、lwIP、iPerf2 和 Shell：

```text
CONFIG_FREERTOS=y
CONFIG_LWIP=y
CONFIG_IPERF=y
CONFIG_SHELL=y
```

也可以在 `proj.conf` 中设置：

```cmake
set(CONFIG_FREERTOS 1)
set(CONFIG_LWIP 1)
set(CONFIG_IPERF 1)
set(CONFIG_SHELL 1)
```

仅使用 C API 时可以不启用 `CONFIG_SHELL`。Raw 后端还要求 lwIP 配置启用
`LWIP_TCPIP_CORE_LOCKING=1` 和 `SYS_LIGHTWEIGHT_PROT=1`。同一应用中不要再启用
其他也会注册 `iperf` Shell 命令的实现，例如 `CONFIG_WIFI_IPERF`。

### 2. 准备 PC 端工具

在 PC 上安装 Classic iPerf2。虽然协议版本是 iPerf2，但桌面端程序通常仍叫
`iperf`：

```bash
iperf --version
```

请勿使用 `iperf3` 与本组件互测。

### 3. 运行测试

以下示例假设设备地址是 `192.168.1.100`。

#### 设备作为 TCP Server

设备端：

```text
iperf -s
```

PC 端：

```bash
iperf -c 192.168.1.100 -t 10 -i 1
```

#### 设备作为 TCP Client

PC 端先启动 server：

```bash
iperf -s
```

设备端使用 PC 的地址连接：

```text
iperf -c 192.168.1.10 -t 10 -i 1
```

#### 设备作为 UDP Server

设备端：

```text
iperf -s -u
```

PC 端发送 20 Mbit/s UDP 流量：

```bash
iperf -c 192.168.1.100 -u -b 20M -l 1470 -t 10
```

#### 设备作为 UDP Client

PC 端：

```bash
iperf -s -u
```

设备端：

```text
iperf -c 192.168.1.10 -u -b 20M -l 1470 -t 10
```

`-b` 可以使用整数 bit/s，也可以使用不区分大小写的十进制 `K`/`M` 后缀。
例如，`100K` 表示 100000 bit/s，`100M` 表示 100000000 bit/s。

#### 选择 Socket 后端

默认使用 Raw 后端。在设备端命令中加入 `-A socket` 即可选择 Socket 后端：

```text
iperf -s -A socket
iperf -c 192.168.1.10 -A socket -t 10 -N
iperf -s -u -A socket
iperf -c 192.168.1.10 -u -A socket -b 50M -l 1470 -t 10
```

Socket 和 Raw 后端使用相同的线上数据格式，可以与同一个桌面 iPerf2 工具互测。

### 命令说明

```text
iperf -s|-c <IPv4-address> [-u] [-A socket|raw] [-p port]
  [-l bytes] [-t sec|-n bytes] [-i sec] [-b bit/s[K|M]]
  [-S tos] [-N] [-B IPv4-address]
iperf -a
iperf
iperf -h
```

| 参数 | 说明 |
|---|---|
| `-s` | 作为 server 运行。 |
| `-c <IPv4>` | 作为 client 连接指定 server。 |
| `-u` | 使用 UDP；默认使用 TCP。 |
| `-A socket\|raw` | 选择后端；默认使用 Raw。 |
| `-p <port>` | Server 端口；默认是 `5001`。 |
| `-l <bytes>` | TCP buffer 或 UDP datagram 长度。 |
| `-t <sec>` | Client 运行时间；默认 10 秒。 |
| `-n <bytes>` | 按总字节数运行，替代运行时间。 |
| `-i <sec>` | 报告间隔；`0` 表示关闭周期报告。 |
| `-b <rate>` | UDP client 发送速率，单位为 bit/s；支持不区分大小写的 `K`/`M` 后缀。默认 1 Mbit/s。 |
| `-S <tos>` | IPv4 TOS 值。 |
| `-N` | TCP client 关闭 Nagle。 |
| `-B <IPv4>` | 绑定本地 IPv4 地址。 |
| `-a` | 同步停止并销毁 Shell 当前管理的测试。 |
| `-h` | 显示命令帮助。 |

使用时注意：

- `iperf` 和 `iperf -h` 都会显示命令帮助。
- 同一应用只能由一个实现注册 `iperf` Shell 命令。
- `-s` 和 `-c` 必须且只能选择一个。
- `-t` 和 `-n` 不能同时使用。
- `-b` 只适用于 UDP client。
- 只有 `-b` 支持后缀：`K/k` 表示 1000，`M/m` 表示 1000000；其他数值
  参数仍只接受整数。
- Shell 同一时间只管理一个测试；C API 支持多个独立实例。
- CLI 首次命令创建永久 mutex，初始化依赖 SDK 单 Shell 命令线程。槽位在锁内
  按 EMPTY / ACTIVE / FINISHED / CLOSING 管理。自然结束安排一个临时 cleanup
  task；`-a` 获取同一份独占回收权，均解锁后同步 destroy。CLOSING 拒绝新建、
  查询和重复销毁。不使用 timer 或常驻任务。
- Cleanup 按值持有 generation，不持有实例指针；即使地址复用，旧任务也不会
  误删新实例。代次永不回绕，耗尽后拒绝新建。临时任务创建失败保留 FINISHED，
  可由 `-a` 或下一条命令回收；意外 destroy 错误保留句柄和可重试状态。
- 配置的 UDP buffer/datagram 长度必须为 80～1470 字节；接收 setup/data 的
  最小长度由下文所述线上格式决定。TCP Socket buffer 最大为
  16384 字节，TCP Raw buffer 最大为 4096 字节。

## C API

使用时包含 `bflb_iperf.h`。一个测试实例的基本生命周期是：

```text
config_init -> create（启动 worker）-> get_state/get_result -> destroy（停止 + 等待 + 释放）
```

以下任务上下文示例运行 TCP Raw client，等待自然结束、复制最终快照，再同步销毁：

```c
#include <lwip/ip4_addr.h>
#include <bflb_iperf.h>
#include <FreeRTOS.h>
#include <semphr.h>

typedef struct {
  SemaphoreHandle_t finished;
  bflb_iperf_result_t result;
} app_iperf_result_t;

static void app_iperf_event(bflb_iperf_t *iperf, bflb_iperf_event_t event,
              const bflb_iperf_result_t *result, void *user_data)
{
  app_iperf_result_t *app = user_data;

  (void)iperf;
  if (event == BFLB_IPERF_EVENT_FINISHED) {
    app->result = *result;
    xSemaphoreGive(app->finished);
  }
  /* 此处不能销毁本实例，也不能等待其他任务销毁它。 */
}

int app_run_iperf(void)
{
  bflb_iperf_config_t config;
  bflb_iperf_t *iperf = NULL;
  app_iperf_result_t app;
  ip4_addr_t server;
  int ret;

  app.finished = xSemaphoreCreateBinary(); /* 初始为空。 */
  if (app.finished == NULL) {
    return BFLB_IPERF_ERR_INVALID;
  }
  bflb_iperf_config_init(&config);
  ip4addr_aton("192.168.1.10", &server);
  config.role = BFLB_IPERF_ROLE_CLIENT; /* 默认 TCP Raw。 */
  config.remote_ip4 = server.addr;
  config.duration_s = 10;
  config.event_cb = app_iperf_event;
  config.user_data = &app;

  ret = bflb_iperf_create(&config, &iperf);
  if (ret == BFLB_IPERF_OK) {
    xSemaphoreTake(app.finished, portMAX_DELAY);
    ret = bflb_iperf_destroy(iperf); /* 同时等待 callback 返回。 */
    iperf = NULL;
    if (ret == BFLB_IPERF_OK) {
      ret = app.result.error; /* 销毁后副本仍有效。 */
    }
    }
  vSemaphoreDelete(app.finished);
  return ret;
}
```

主要接口：

| 接口 | 作用 |
|---|---|
| `bflb_iperf_config_init()` | 使用默认值初始化配置。 |
| `bflb_iperf_create()` | 校验复制配置、分配资源并启动 worker。 |
| `bflb_iperf_get_state()` | 获取当前生命周期状态。 |
| `bflb_iperf_get_result()` | 获取一致的统计快照。 |
| `bflb_iperf_destroy()` | 请求停止，等待 worker/回调完成，然后释放实例。 |

接口使用注意事项：

- `remote_ip4` 和 `local_ip4` 使用网络字节序。
- Create 即启动一次测试，不再提供 public start/stop。实例永不自动释放，
  自然结束或失败后也必须由 owner 销毁。
- 配置会复制，但 `user_data` 仅借用指针，其对象须有效至 FINISHED 返回。
  输出句柄、同步对象在 worker 可见前初始化；callback 可能早于 create 返回。
  同步 create 失败时输出 NULL、完整释放资源且不发 callback。
- `event_cb` 从 worker 发一次 STARTED（不表示已连接），随后在运行资源清理、
  结果冻结后发一次 FINISHED，包含提前取消和异步失败。事件 snapshot 指针只在
  callback 内有效，需复制才能保留；`get_result` 复制所有字段、不清零计数。
  DONE/ERROR 结果直到销毁前保持不变；提前取消时 STARTED 快照可为 STOPPING。
- 唯一外部 owner 必须串行化 create/destroy 和其他外部 API；没有引用计数替
  失效句柄兜底。Destroy 不持实例 mutex 等待，故 callback 可在 owner 销毁期间
  查询状态/结果。自身 worker 调用 destroy 返回 `BFLB_IPERF_ERR_SELF` 且不修改
  实例；callback 也不能等待其他线程销毁本实例，必须最终返回。
- 要提前取消，owner 直接 destroy，不必像示例一样等自然结束。Destroy 设置
  stop、唤醒 TCP Raw 无限通知等待，再等待初始为空的 worker_done 二值信号量。
  FINISHED 返回后 worker 撤销句柄，give 是最后一次实例访问，随后仅自删任务。
  Owner 再释放 context、同步对象和实例，不需要 BUSY 轮询。
- 各后端保持独立等待方式：Socket send/recv/accept 使用检查成功的超时选项；
  TCP 建连使用非阻塞 connect + 200 ms select，因为 lwIP 阻塞 connect 不受
  SO_SNDTIMEO 限制。UDP Raw 队列等待至多 10 ms，UDP Socket 节拍等待至多
  20 ms（AckFIN 接收至多 1 秒）。Destroy 实际延迟还取决于调度、TCP/IP 服务、
  Socket close 和 callback 耗时，不承诺硬实时上限。
- API 仅可从任务调用，须开启调度/中断且不持 TCP/IP core lock。统计继续使用
  短 IRQ 临界区防止单核 32 位平台的 64 位撕裂读写，不支持跨 SMP CPU 共享实例。
- `local_port` 和 `task_priority` 可以通过 C API 配置，但没有对应 Shell 参数。

完整配置字段、结果字段和返回值请查看
[include/bflb_iperf.h](include/bflb_iperf.h)。

## 实现原理

`include/bflb_iperf.h` 是公开 API，仅暴露不透明实例句柄。
私有 `iperf/iperf_internal.h` 管理实例结构、后端操作、worker 生命周期和 client
预算契约，核心管理逻辑由 `iperf/bflb_iperf.c` 实现。它包含 `iperf/iperf_common.h`，
后者声明协议常量、UDP 格式与 tracker、统计类型、共享 Raw 负载及报告辅助接口，
对应实现位于 `iperf/iperf_common.c`。Common 头只依赖标准类型和公开 API；
common 实现包含 internal 头以访问实例字段，头文件不存在反向依赖。

组件根据协议和后端选择四条相互独立的数据路径：

| 模式 | 实现方式 |
|---|---|
| TCP Socket | Worker task 中执行阻塞式 Socket `send`/`recv` 循环。 |
| UDP Socket | Worker task 负责 datagram、发送节拍、丢包/jitter 和 FIN/AckFIN。 |
| TCP Raw | lwIP TCP callback 处理接收数据，worker 管理连接事件和发送进度。 |
| UDP Raw | 接收 callback 将 pbuf 所有权交给实例 RX queue，worker 解析并统计 datagram。 |

Raw 后端是默认选择，直接使用 lwIP Raw PCB callback，并只在短时间内持有
TCP/IP Core Lock，以减少 API 和线程切换开销。对于倾向使用 Socket API 的集成，
仍可显式选择 Socket 后端。每个后端实例拥有独立的 worker 和私有状态，不共享
可变测试状态。

UDP 使用 Classic iPerf2 normal-mode 数据格式。两个接收后端都通过合法首包
选择会话格式，后续不再重新探测：

| PC UDP client | 前缀及配置 | 数据序号基准 | AckFIN 长度 |
|---|---|---|---|
| 2.0.5 normal 单流 | 12 字节 SEQ32 + 24 字节配置，flags=0 | 0 | 52 字节 |
| 2.0.13 SEQ64，无 EXTEND | 16 字节 SEQ64 + 4 字节 flags，其余为负载 | 1 | 56 字节 |
| 2.2.1 SEQ64，带 EXTEND | 16 字节 SEQ64 + 24 字节配置 + 40 字节扩展 | 1 | 56 字节 |

优先识别 SEQ64，但必须先满足至少 20 字节、完整 signed64 ID 非负、合法时间戳
和 SEQ flag（不含 VERSION1）。无 EXTEND 时最小为 20 字节：2.0.13 normal
模式的 flags 之后是 ASCII 负载，不读取或校验 base 配置字段。EXTEND 仍要求
完整 80 字节及合法 base 配置。声明 SEQ64 后若校验失败，不会回退为 SEQ32。
SEQ32 要求 signed32 ID 非负、至少 36 字节，并满足 normal flags、单线程、合法端口、合理 buffer
长度（允许上游默认值 0）、非零带宽/amount 及合法时间戳，不能仅凭短 UDP 或
flags=0 占用会话。三种格式均允许在 ID0/1 丢失后，由后续合法 ID 建立会话；
负序号 FIN 不能建立会话。64 位 ID 允许高字非零、低字最高位为 1，只要完整
signed64 值非负。建连后数据和 FIN 只需满足所选格式的 12/16 字节前缀长度。

接收计时从首个接受的报文开始，但初段丢包仍按协议序号基准（SEQ32 为 0，
两种 SEQ64 为 1）统计，不以首个收到的 ID 重设基准。peer 绑定逻辑不变。
限制：旧测试滞留的数据若带有合法 setup 前缀，可能占用新的 server 会话，
接收端无法据此区分旧数据和新测试。

2.0.5 的数据 ID 为 0～N-1，FIN 为 -N：ID0 计入数据，报告总包数为 N，
不是 N-1。SEQ64 保持已有从 1 开始的统计方式（忽略 ID0）。字节数只统计数据，
不统计 FIN；首次 FIN 冻结统计，重复 FIN 仅触发相同格式的报告回复。丢包和乱序
沿用缺口抵消估算，并非精确去重。Client 发送负序号 FIN 后，server 返回 AckFIN。

设备 UDP TX 仍使用 SEQ64 扩展配置；此次新增的是 2.0.5 **RX 支持**，不代表支持
向 2.0.5 server 发送，也未新增 32 位 TX 模式。原有 12/16 字节前缀的报告读取仍保留。

### 实机验证

使用 [examples/wifi/macsw_bare](../../examples/wifi/macsw_bare) 工程验证 iPerf。
分别使用 iPerf2 2.0.5、2.0.13 和 2.2.1 对端验证设备 UDP RX，覆盖 Raw 和 Socket
两种后端，检查吞吐、丢包统计及 FIN/AckFIN 正常结束。

## 功能范围与限制

支持：

- IPv4 TCP/UDP client 和 server
- Socket/Raw 两种后端
- Client 按时间或字节数运行
- UDP 发送节拍、丢包、乱序和 jitter 报告
- Classic iPerf2 normal-mode UDP setup 和 FIN/AckFIN
- C API 多实例；Shell 单实例

暂不支持：

- IPv6
- iPerf3
- Parallel streams
- Reverse、dual、tradeoff 和 full-duplex 模式
- Enhanced mode 和扩展统计
- 无控制头的 UDP compatibility（`-C`）模式
- 单个 server 实例同时服务多个 client
- Classic TCP V1 控制头交互

实际吞吐量会受到目标芯片时钟、lwIP 内存池、worker task 优先级、网卡队列和后端
类型影响。验证高速 UDP 时，应同时观察 iPerf 丢包统计和网卡接口计数。