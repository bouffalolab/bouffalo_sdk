# Compact iPerf (Classic iPerf2)

[中文](README_cn.md)

`components/iperf` is a compact IPv4 throughput test component for embedded
SDKs. It supports TCP and UDP clients and servers through either the lwIP
Socket API or the lwIP Raw API.

The component and shell command are named `iperf`, but the implemented protocol
is Classic iPerf2 normal mode. It does not support iPerf3 and is not compatible
with an iPerf3 peer.

## Quick Start

### 1. Enable the component

Enable FreeRTOS, lwIP, iPerf2, and Shell in the application configuration:

```text
CONFIG_FREERTOS=y
CONFIG_LWIP=y
CONFIG_IPERF=y
CONFIG_SHELL=y
```

The equivalent `proj.conf` settings are:

```cmake
set(CONFIG_FREERTOS 1)
set(CONFIG_LWIP 1)
set(CONFIG_IPERF 1)
set(CONFIG_SHELL 1)
```

`CONFIG_SHELL` is optional when only the C API is used. Raw backends also
require `LWIP_TCPIP_CORE_LOCKING=1` and `SYS_LIGHTWEIGHT_PROT=1` in the lwIP
configuration. Do not enable another implementation that also exports the
`iperf` shell command, such as `CONFIG_WIFI_IPERF`, in the same application.

### 2. Prepare the desktop peer

Install Classic iPerf2 on the PC. The desktop executable is normally named
`iperf` even though the protocol version is iPerf2:

```bash
iperf --version
```

Do not use `iperf3` for these tests.

### 3. Run a test

Replace `192.168.1.100` with the device IPv4 address.

#### Device as a TCP server

On the device:

```text
iperf -s
```

On the PC:

```bash
iperf -c 192.168.1.100 -t 10 -i 1
```

#### Device as a TCP client

On the PC:

```bash
iperf -s
```

On the device, replace the address with the PC address:

```text
iperf -c 192.168.1.10 -t 10 -i 1
```

#### Device as a UDP server

On the device:

```text
iperf -s -u
```

On the PC, send 20 Mbit/s UDP traffic:

```bash
iperf -c 192.168.1.100 -u -b 20M -l 1470 -t 10
```

#### Device as a UDP client

On the PC:

```bash
iperf -s -u
```

On the device:

```text
iperf -c 192.168.1.10 -u -b 20M -l 1470 -t 10
```

The `-b` option accepts integer bit/s values or a case-insensitive decimal
`K`/`M` suffix. For example, `100K` is 100000 bit/s and `100M` is 100000000
bit/s.

#### Select the Socket backend

Raw is the default backend. Add `-A socket` to a device command to select the
Socket backend:

```text
iperf -s -A socket
iperf -c 192.168.1.10 -A socket -t 10 -N
iperf -s -u -A socket
iperf -c 192.168.1.10 -u -A socket -b 50M -l 1470 -t 10
```

The Socket and Raw backends use the same wire format and may be tested against
the same desktop iPerf2 peer.

### Command Reference

```text
iperf -s|-c <IPv4-address> [-u] [-A socket|raw] [-p port]
  [-l bytes] [-t sec|-n bytes] [-i sec] [-b bit/s[K|M]]
  [-S tos] [-N] [-B IPv4-address]
iperf -a
iperf
iperf -h
```

| Option | Description |
|---|---|
| `-s` | Run as server. |
| `-c <IPv4>` | Run as client and connect to the specified server. |
| `-u` | Use UDP; TCP is the default. |
| `-A socket\|raw` | Select the backend; Raw is the default. |
| `-p <port>` | Server port; default is `5001`. |
| `-l <bytes>` | TCP buffer or UDP datagram length. |
| `-t <sec>` | Client duration; default is 10 seconds. |
| `-n <bytes>` | Client byte limit instead of duration. |
| `-i <sec>` | Report interval; `0` disables periodic reports. |
| `-b <rate>` | UDP client transmit rate in bit/s; accepts case-insensitive `K`/`M` suffixes. Default is 1 Mbit/s. |
| `-S <tos>` | IPv4 TOS value. |
| `-N` | Disable Nagle for a TCP client. |
| `-B <IPv4>` | Bind a local IPv4 address. |
| `-a` | Synchronously stop and destroy the current shell-managed test. |
| `-h` | Print command help. |

Command notes:

- `iperf` and `iperf -h` both print command help.
- Only one implementation may register the `iperf` shell command.
- Select exactly one of `-s` and `-c`.
- `-t` and `-n` are mutually exclusive.
- `-b` is valid only for a UDP client.
- Only `-b` accepts suffixes. `K/k` means 1000 and `M/m` means 1000000;
  other numeric options remain plain integers.
- The shell command manages one test at a time. The C API supports multiple
  independent instances.
- The CLI creates one permanent mutex on its first command (initialization
  assumes the SDK's single shell command task). Its slot transitions through
  EMPTY / ACTIVE / FINISHED / CLOSING under that mutex. Natural completion
  schedules one temporary cleanup task; `-a` takes the same exclusive reclaim
  ownership. Both release the mutex before synchronous destruction. CLOSING
  rejects creation, queries and another destroy; no timer or resident task is used.
- Cleanup carries a generation by value, not an instance pointer. Stale tasks
  cannot reclaim a newer instance even if its address is reused. Generations
  never wrap (new creation is refused at exhaustion). If cleanup task allocation
  fails, FINISHED remains reclaimable by `-a` or the next command. Unexpected
  destroy errors retain the handle for retry rather than losing the slot.
- The configured UDP buffer/datagram length must be 80 to 1470 bytes; received
  setup/data minimum lengths depend on the wire format described below.
  TCP Socket buffers may be up to
  16384 bytes; TCP Raw buffers may be up to 4096 bytes.

## C API

Include `bflb_iperf.h`. A test has a simple lifecycle:

```text
config_init -> create (launches worker) -> get_state/get_result -> destroy (stop + join + free)
```

The following task-context example runs a TCP Raw client to natural completion,
copies the final snapshot, and then destroys it synchronously:

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
  /* Never destroy this instance or wait for its destruction here. */
}

int app_run_iperf(void)
{
  bflb_iperf_config_t config;
  bflb_iperf_t *iperf = NULL;
  app_iperf_result_t app;
  ip4_addr_t server;
  int ret;

  app.finished = xSemaphoreCreateBinary(); /* Initially empty. */
  if (app.finished == NULL) {
    return BFLB_IPERF_ERR_INVALID;
  }
  bflb_iperf_config_init(&config);
  ip4addr_aton("192.168.1.10", &server);
  config.role = BFLB_IPERF_ROLE_CLIENT; /* TCP Raw is the default. */
  config.remote_ip4 = server.addr;
  config.duration_s = 10;
  config.event_cb = app_iperf_event;
  config.user_data = &app;

  ret = bflb_iperf_create(&config, &iperf);
  if (ret == BFLB_IPERF_OK) {
    xSemaphoreTake(app.finished, portMAX_DELAY);
    ret = bflb_iperf_destroy(iperf); /* Also waits for callback return. */
    iperf = NULL;
    if (ret == BFLB_IPERF_OK) {
      ret = app.result.error; /* app.result remains valid after destroy. */
    }
    }
  vSemaphoreDelete(app.finished);
  return ret;
}
```

Main API functions:

| Function | Purpose |
|---|---|
| `bflb_iperf_config_init()` | Initialize a configuration with defaults. |
| `bflb_iperf_create()` | Validate/copy configuration, allocate resources and launch the worker. |
| `bflb_iperf_get_state()` | Read the current lifecycle state. |
| `bflb_iperf_get_result()` | Read a consistent statistics snapshot. |
| `bflb_iperf_destroy()` | Request stop, wait for worker/callback completion, then free the instance. |

Important API rules:

- `remote_ip4` and `local_ip4` use network byte order.
- Create immediately launches one test. There are no public start/stop APIs.
  The instance never frees itself, even after natural completion or failure.
- The configuration is copied, but `user_data` is only a borrowed pointer and
  must remain valid until FINISHED returns. Output handle and synchronization
  objects are initialized before worker visibility; callbacks may run before
  create returns. A synchronous create failure leaves a NULL handle, releases
  resources and emits no callback.
- `event_cb` receives STARTED once from the worker (not a connected indication),
  then FINISHED once after runtime cleanup and result freezing, including early
  cancellation and asynchronous failure. Snapshot pointers are callback-local:
  copy to retain. `get_result` copies without clearing counters; DONE/ERROR
  results remain unchanged until destruction. STARTED may snapshot STOPPING.
- One external owner must serialize create/destroy and all other external APIs.
  No reference-count framework protects stale handles. Callbacks may query
  results/state while the owner is in destroy, because destroy joins without
  holding the instance mutex. Calling destroy from this instance's worker
  returns `BFLB_IPERF_ERR_SELF` without changing it. A callback must not wait
  for another task to destroy the instance either; it must eventually return.
- To cancel, the owner calls destroy directly instead of waiting for natural
  completion in the example. Destroy requests stop, wakes TCP Raw's indefinite
  notification wait, and waits on an initially empty worker-done semaphore.
  FINISHED returns before the worker revokes its handle and gives that semaphore
  as its final instance access; only task self-deletion follows. The owner then
  frees context, synchronization objects and instance. No BUSY polling is needed.
- Socket waits remain backend-specific: send/receive/accept use checked timeout
  options; TCP connection setup uses nonblocking connect with 200 ms select
  slices because lwIP's blocking connect ignores SO_SNDTIMEO. UDP Raw queue
  waits are bounded to 10 ms, UDP Socket pacing to 20 ms (AckFIN receive up to
  1 s). Actual destroy latency also depends on scheduling, TCP/IP service,
  socket close and callback duration; it is not a hard real-time deadline.
- Public APIs are task-only, with scheduler/interrupts enabled and no TCP/IP
  core lock held. Statistics use short IRQ guards against torn 64-bit accesses
  on a single 32-bit CPU; sharing an instance across SMP CPUs is unsupported.
- `local_port` and `task_priority` are available through the C API but are not
  exposed as shell options.

See [include/bflb_iperf.h](include/bflb_iperf.h) for all configuration fields,
result fields, and return values.

## How It Works

`include/bflb_iperf.h` is the public API with an opaque instance handle.
Private `iperf/iperf_internal.h` owns the instance representation, backend
operations, worker lifecycle, and client budget contracts; core management is
implemented by `iperf/bflb_iperf.c`. It includes `iperf/iperf_common.h`, which
declares protocol constants, UDP layouts/tracking, statistics, shared Raw
payload, and reporting helpers implemented by `iperf/iperf_common.c`. The common
header depends only on standard types and the public API; the common
implementation includes the internal header to access instance fields.
There is no reverse header dependency.

The implementation has four independent data paths selected by protocol and
backend:

| Mode | Implementation |
|---|---|
| TCP Socket | Blocking Socket `send`/`recv` loops in a worker task. |
| UDP Socket | Socket datagrams, pacing, loss/jitter tracking, and FIN/AckFIN in a worker task. |
| TCP Raw | lwIP TCP callbacks handle receive data; a worker manages connection events and transmit progress. |
| UDP Raw | The receive callback transfers pbuf ownership to a per-instance queue; a worker parses and accounts datagrams. |

The Raw backend is the default and uses lwIP Raw PCB callbacks with short
TCP/IP Core Lock sections to reduce API and thread overhead. The Socket backend
remains available for integrations that prefer the Socket API. Each backend
instance owns its worker and private state; instances do not share mutable test
state.

UDP uses the Classic iPerf2 normal-mode wire format. Both receivers select a
fixed layout from the first supported setup datagram:

| PC UDP client | Prefix / settings | Data sequence base | AckFIN size |
|---|---|---|---|
| 2.0.5 normal, single stream | 12-byte SEQ32 + 24-byte settings, flags=0 | 0 | 52 bytes |
| 2.0.13 SEQ64, no EXTEND | 16-byte SEQ64 + 4-byte flags; remaining bytes are payload | 1 | 56 bytes |
| 2.2.1 SEQ64 with EXTEND | 16-byte SEQ64 + 24-byte settings + 40-byte extension | 1 | 56 bytes |

SEQ64 is identified first, only after at least 20 bytes are available, with
any nonnegative signed 64-bit ID, a valid timestamp and the SEQ flag (no
VERSION1). Without EXTEND, 20 bytes suffice: 2.0.13 normal mode has ASCII
payload after flags, so base configuration fields are neither read nor
validated. EXTEND requires the full 80 bytes and valid base settings.
A SEQ64 claim that fails validation never falls back to SEQ32.
SEQ32 requires a nonnegative signed 32-bit ID, at least 36 bytes, normal flags, one thread, a valid
port, a plausible buffer length (including the upstream default zero), nonzero
bandwidth/amount, and a valid timestamp. Arbitrary short UDP or flags=0 alone
cannot claim a session. All three layouts allow a later valid ID to establish
a session if IDs 0/1 were lost; negative FIN IDs cannot establish a session.
For 64-bit IDs, a nonzero high word or a set low-word sign bit is valid as long
as the complete signed 64-bit value is nonnegative.
Once selected, data/FIN require only the selected 12/16-byte
prefix; the layout is not redetected on each packet.

Receiver timing starts with the first accepted datagram, but initial loss is
still counted from the protocol sequence base (zero for SEQ32, one for SEQ64
layouts), not from that datagram's ID. Peer binding is unchanged. Delayed data
from an old test with a valid setup prefix may claim a new server session;
the receiver cannot distinguish it from a new test.

For 2.0.5, data IDs are 0 through N-1 and FIN is -N: ID0 counts as data and the
reported total is N, not N-1. SEQ64 retains its existing one-based accounting
(ID0 is ignored). Receivers account data bytes, not FIN bytes. The first FIN
freezes statistics; duplicate FINs only trigger another format-matched report.
Loss and reordering use the existing gap-cancellation estimate, not exact
duplicate detection. A negative final sequence ends the test.

Device UDP TX remains SEQ64 with extended settings; this change adds 2.0.5
**RX**, not 2.0.5 TX compatibility or a 32-bit TX mode. Existing 12-byte and
16-byte prefix server-report readers remain supported.

### Hardware validation

Use [examples/wifi/macsw_bare](../../examples/wifi/macsw_bare) to validate iPerf
on the device. Test UDP RX with iPerf2 2.0.5, 2.0.13 and 2.2.1 peers on both
Raw and Socket backends, checking throughput, packet loss and FIN/AckFIN completion.

## Scope and Limitations

Supported:

- IPv4 TCP and UDP client/server
- Socket and Raw backends
- Time and byte client limits
- UDP pacing, loss, out-of-order, and jitter reports
- Classic iPerf2 normal-mode UDP setup and FIN/AckFIN
- Multiple instances through the C API; one instance through the shell

Not supported:

- IPv6
- iPerf3
- Parallel streams
- Reverse, dual, tradeoff, or full-duplex modes
- Enhanced mode and extended statistics
- Headerless compatibility (`-C`) UDP mode
- Multiple simultaneous clients on one server instance
- Classic TCP V1 control-header exchange

Actual throughput depends on the target clock, lwIP memory pools, worker task
priority, network-interface queues, and the selected backend. Validate high-rate
UDP tests on the target board and inspect both iPerf loss statistics and the
network-interface counters.