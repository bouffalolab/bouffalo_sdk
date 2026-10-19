# lwip_emac

[English](README.md) | [中文](README_zh.md)

This example provides EMAC network interfaces on one lwIP stack for TCP
loopback, UDP echo, HTTP, and iperf services. BL618DG enables EMAC0 and EMAC1;
single-EMAC chips keep one network interface. Each active port owns its PHY,
DMA buffers, queues, RX task, MAC address, netif, DHCP client, link state, and
statistics.

## Services and Shell commands

| Service | Default | Start | Stop |
|:-------:|:-------:|:------|:-----|
| TCP loopback server | TCP 3365 | `tcp_server start [port]` | `tcp_server stop` |
| TCP loopback client | TCP 3365 | `tcp_client start <ip> [port]` | `tcp_client stop` |
| UDP echo server | UDP 3365 | `udp_echo start [port]` | `udp_echo stop` |
| Built-in HTTP server | TCP 80 | Started automatically | Always running |

TCP and UDP can use the same port number because they are different transport
protocols. The TCP and UDP services are controlled by the Shell and do not
start automatically. Omitting a port selects the corresponding default port.
The built-in lwIP HTTP server starts automatically after network initialization
and remains available on TCP port 80; it has no Shell start or stop command.

Services remain usable across normal PHY link down/up transitions. DHCP is
enabled by default on every active interface. Logs use the physical port name,
for example `[EMAC0] IPv4 address: ...`. A linked interface that has no lease
after 30 seconds prints one `[EMACx] DHCP timeout` message while lwIP continues
retrying. On BL618DG, EMAC1 keeps the factory MAC while EMAC0 uses the stable
derived MAC from the earlier dual-port implementation.

The iperf Shell commands and `lwip_emac_info` statistics command are enabled for
throughput and per-port diagnostics.

## Implementation overview

### Shared EMAC and lwIP initialization

`main.c` initializes the Shell and then calls `lwip_emac_start()`. The shared
startup module configures the RMII/MDIO pins, starts the lwIP TCP/IP thread,
adds one netif and DHCP client for every configured EMAC, and prints the
assigned IPv4 address with its physical port number. BL618DG initializes the
configured preferred port first (EMAC0 by default) and selects it as the
default interface. If that port fails during startup, the first surviving
interface becomes the fallback default; the other port is not stopped. Runtime
link loss does not change the default interface.

`lwip_emac_start()` is a task-context, one-time startup API and must only be
called once. The example does not provide runtime EMAC teardown or restart. If
a port fails during startup, resources created for that port are rolled back
while the other configured port is still attempted. A fatal startup error
rolls back initialized ports in reverse order.

One background task permanently polls every active PHY and updates the
corresponding lwIP link state. The EMAC TX/RX queues and hardware buffer
descriptors are created during startup. A normal link down/up event only
updates link state; it does not stop the EMAC or rebuild descriptors.

### Service lifecycle and Shell control

TCP and UDP are registered as Shell commands and are not started during boot.
Each `start` command dynamically creates its service task with `xTaskCreate()`.
Each `stop` command sets a stop flag; short socket timeouts let the task observe
that flag, close its network resources, clear its task handle, and delete
itself. HTTP uses lwIP's callback-driven Raw API server, runs in the TCP/IP
thread context, and is managed as a permanently active service.

### TCP loopback server

The TCP server uses the lwIP BSD Socket API. It creates a stream socket, binds
the requested local port, listens, and accepts clients. Received byte streams
are written back completely to the connected client. Send and receive
timeouts periodically expose the stop request. Connections are processed one
at a time; after one client disconnects, the server accepts the next client.

The server is implemented independently in `tcp/tcp_server.c` and does not
share task state, buffers, or lifecycle control with the TCP client.

### TCP loopback client

The TCP client validates the IPv4 address supplied by the Shell and performs a
nonblocking `connect()`. It waits once with `select()` for up to one second,
then validates the connection result. Once connected, it waits for data from
the remote peer and writes every received byte back completely, matching the
loopback behavior of the TCP server and UDP echo service. The destination port
defaults to 3365; for example,
`tcp_client start 192.168.1.220 5000`.

The client is implemented independently in `tcp/tcp_client.c` and can run
without starting the local TCP server.

### UDP echo server

The UDP server uses a datagram socket bound to the selected local port. Each
datagram returned by `recvfrom()` is sent back to the original peer with
`sendto()`. A short receive timeout lets the task check the stop flag even when
no datagrams arrive.

### HTTP server

The project enables lwIP's built-in callback-driven `httpd`. Static files are
generated into `http_server/web_demo/fsdata_custom.c`; the lwIP `fs.c` module
includes this file through `HTTPD_FSDATA_FILE`, and `httpd` resolves request
URIs directly from fsdata. It handles request parsing, static responses, 404
selection, and concurrent TCP connections in the lwIP TCP/IP thread.

The document page loads the corresponding text file through an HTTP GET request
and displays the response in a `<pre>` element. The text resources are embedded
with `INCBIN` and exposed through lwIP's Custom Files API. The server starts
once on TCP port 80 and runs permanently.

lwIP provides `makefsdata` as a script and as an extended C console utility for
converting a directory of web resources into an `fsdata.c` file. Refer to the
[official makefsdata usage introduction](https://github.com/lwip-tcpip/lwip/blob/master/src/apps/http/makefsdata/readme.txt)
and the [official makefsdata source directory](https://github.com/lwip-tcpip/lwip/tree/master/src/apps/http/makefsdata)
when regenerating `http_server/web_demo/fsdata_custom.c` from the static files
under `http_server/web_demo/pages/`.

### iperf throughput test

The project enables the SDK iperf component and Shell integration. The iperf
Shell commands use the shared lwIP stack and selected EMAC route for testing.

## Source layout

- `lwip_emac_start()` / `lwip_emac_port()`: board pins, lwIP/netif init, DHCP,
	PHY link monitoring and port management (in `bsp/common/eth_phy/lwip_emac_start/`).
- `tcp/tcp_server.c`: independent socket-based TCP loopback server.
- `tcp/tcp_client.c`: independent socket-based TCP loopback client.
- `udp/`: socket-based UDP echo server.
- `http_server/`: built-in lwIP httpd integration, INCBIN README assets, and
	fsdata web resources.
- `config_file/`: shared FreeRTOS and lwIP configuration.

## Support CHIP

| CHIP              | Remark |
|:-----------------:|:------:|
| BL702/BL704/BL706 |        |
| BL702L/BL704L     |        |
| BL616/BL618       |        |
| BL618DG           | AP core |
| BL616CL           |        |

## Compile

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

## Test

The HTTP server is already running after network initialization. First inspect
the per-port state from the board Shell:

```
lwip_emac_info
```

On BL618DG this prints the independent EMAC0 and EMAC1 TX/RX summaries and
available DMA descriptor counts.

After DHCP assigns an address, start the TCP and UDP services:

```
tcp_server start
udp_echo start
```

Custom ports can be selected when required:

```
tcp_server start 5000
udp_echo start 5001
tcp_client start 192.168.1.220 5000
```

Replace `<board-ip>` below with the address shown on the serial console, then
run the host-side checks:

```
nc <board-ip> 3365
nc -u <board-ip> 3365
curl http://<board-ip>/
curl http://<board-ip>/docs/readme-en.md
curl http://<board-ip>/docs/readme-zh.md
```

To test the board as a TCP client, first run a TCP listener on the host, then
use `tcp_client start <host-ip> [port]`. Data sent by the host listener is
returned unchanged by the board. Stop services independently with their
corresponding `stop` commands.

## Flash

```
make flash CHIP=chip_name COMX=xxx
```
