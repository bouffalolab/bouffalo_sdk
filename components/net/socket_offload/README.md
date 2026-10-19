# Socket Offload over RPMsg

Socket offload forwards IPv4 TCP and UDP socket calls over RPMsg to a core
running lwIP. The client and service must use compatible RPC protocol and
RPMsg buffer configurations.

## Configuration

Enable `CONFIG_FREERTOS`, `CONFIG_IPC`, `CONFIG_RPMSG_SERVICE`, `CONFIG_LWIP`,
and `CONFIG_SOCKET_OFFLOAD` on both cores. Select one role on each core:

| Role | Configuration | Function |
| --- | --- | --- |
| Client | `CONFIG_SOCKET_OFFLOAD_CLIENT=y` | Forward socket calls over RPMsg. |
| Service | `CONFIG_SOCKET_OFFLOAD_SERVICE=y` | Execute socket calls using the local lwIP stack. |

The service core provides the network interface and lwIP stack. The client
does not require a local network interface. A BL618DG dual-core example is
available in `examples/wifi/sta/dualcore`.

The component uses 1600-byte RPMsg buffers on both cores. The service requires
`LWIP_NETCONN_FULLDUPLEX=1` and `LWIP_NETCONN_SEM_PER_THREAD=1`. Both cores
require `configSUPPORT_STATIC_ALLOCATION=1`.

Timeout defaults are defined in `src/ipc_socket.h` and can be overridden with
compiler definitions:

| Setting | Default | Scope |
| --- | --- | --- |
| `RPC_TIMEOUT_MS` | `5000` | Control RPC responses; must be positive. |
| `RPC_SOCKET_TIMEOUT_MS` | `0` | `connect`, `accept`, `send`, `recv`, and `close` RPC responses; `0` waits indefinitely. |

Values are in milliseconds. A response timeout returns `ETIMEDOUT` without
cancelling the operation on the service core. Configure `SO_RCVTIMEO` and
`SO_SNDTIMEO` to limit the underlying socket's receive and send waits.
RPMsg frames are sent once. A client transmit buffer shortage returns
`ENOBUFS`. If a service response cannot be transmitted, the client waits for
its configured RPC response timeout; a zero socket RPC timeout has no limit.

## Initialization

Initialize the SDK RPMsg device before the socket RPC endpoint. On the client,
call `rpmsg_and_rpc_init()` followed by `socket_offload_init()`. On the service,
call `rpc_service_init()`. Initialize the service's network stack before starting
socket traffic. Initialization functions return zero on success or a negative
errno on failure.

Include `<socket_offload.h>` in client source files that use the offload. With
`CONFIG_SOCKET_OFFLOAD_CLIENT=y`, the header maps supported socket functions
such as `socket`, `connect`, `send`, `recv`, and `select` to the offload API.
Socket functions return `-1` and set `errno` on failure.

Stop socket calls before shutting down the client or service.

## iPerf2 examples

The following commands use the dual-core example, with `192.168.1.100` as the
service core's device address and `192.168.1.10` as the PC address. Replace
them with the addresses on your network. Use Classic iPerf2 on the PC and
select the Socket backend with `-A socket` on the client core. Start the server
before the client.

### TCP transmit

On the PC:

```sh
iperf -s -p 5001 -i 1
```

On the device:

```text
iperf -c 192.168.1.10 -p 5001 -t 10 -i 1 -A socket
```

### TCP receive

On the device:

```text
iperf -s -p 5001 -i 1 -A socket
```

On the PC:

```sh
iperf -c 192.168.1.100 -p 5001 -t 10 -i 1
```

### UDP transmit

On the PC:

```sh
iperf -s -u -p 5001 -i 1
```

On the device:

```text
iperf -c 192.168.1.10 -u -p 5001 -t 10 -i 1 -b 200M -A socket
```

### UDP receive

On the device:

```text
iperf -s -u -p 5001 -i 1 -A socket
```

On the PC:

```sh
iperf -c 192.168.1.100 -u -p 5001 -t 10 -i 1 -b 200M
```

The examples use iPerf's default buffer sizes.

## API behavior and limits

- Only IPv4 `SOCK_STREAM` and `SOCK_DGRAM` sockets are supported.
- Blocking and nonblocking sockets are supported. Configure nonblocking mode
  with `FIONBIO`, use `MSG_DONTWAIT` for an individual call, or set a receive
  timeout with `SO_RCVTIMEO`.
- RPC requests and responses use task-managed queues. Calls on different
  sockets wait independently. Service tasks are allocated on demand and reused
  until component shutdown. Allocation failures return `ENOMEM`.
- Each socket supports one pending receive call. Concurrent receive calls on
  the same socket return `EBUSY`.
- TCP sends support data larger than one RPMsg frame. UDP sends spanning
  multiple RPMsg frames produce separate datagrams.
- Socket control requests must fit one RPMsg frame (1556 bytes with the default
  configuration). Oversized requests return `EMSGSIZE`.
- UDP receive payloads are limited to 1552 bytes with the default configuration.
  Oversized datagrams return `EMSGSIZE` when the caller requests more than this
  limit; they are discarded unless `MSG_PEEK` is set. Smaller caller buffers
  follow the underlying socket's truncation behavior.
- RPC waits use the configured timeouts. Socket timeouts apply to send and
  receive independently of the RPC response timeout.
- `getsockname()` and `getpeername()` return `ENOTSUP`. `accept()` does not
  return the peer address.
