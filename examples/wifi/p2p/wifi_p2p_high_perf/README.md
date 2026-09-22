# Wi-Fi Direct (P2P) high performance

Throughput-oriented P2P example. Application code is reused from
`examples/wifi/p2p/wifi_p2p`. TCP/lwIP settings follow
`examples/wifi/sta/wifi_tcp_high_perf`: no-copy TCP TX pbufs, large TCP
window/send buffer, and batched RX window updates.

MAC software stays on the P2P profile (`CONFIG_MACSW_SELECT=p2p`). The
`tcp_bench` profile used by `wifi_tcp_high_perf` cannot be selected here
because `CONFIG_WIFI_P2P` requires the P2P MACSW library.

Supported chips: BL616, BL616CL, BL618DG.

BL618DG uses the same TCP window, send buffer, and lwIP heap as
`wifi_tcp_high_perf`. BL616 and BL616CL keep no-copy TX pbufs and RX ACK
batching, but use the smaller `wifi_tcp` window (`TCP_SND_BUF=96*TCP_MSS`,
`LWIP_HEAP_SIZE=64KB`). P2P cannot select `tcp_bench`, and the 160KB Wi-Fi
RAM holds the lwIP heap, so the STA/SoftAP 106KB heap does not link.

P2P command syntax, events, and troubleshooting are documented in
`examples/wifi/p2p/wifi_p2p/README.md`. Use this example when measuring
TCP/UDP throughput after the group is up.

## Compile

Run the following commands from the directory containing this README.

```bash
make CHIP=bl616 BOARD=bl616dk
make CHIP=bl616cl BOARD=bl616cldk
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap \
  CONFIG_PSRAM=y CONFIG_PSRAM_COPY_CODE=y
```

When changing CHIP, BOARD, or CPU_ID, clean the previous build first:

```bash
make clean
```

### BL618DG PSRAM

BL618DG throughput tests must enable both PSRAM initialization and
code/rodata copying. Pass the flags on every rebuild; `CONFIG_PSRAM=y`
alone does not move ordinary code out of Flash. The example `defconfig`
also sets these flags for BL618DG.

This demo defaults to `CONFIG_ROMAPI=n` on BL618DG and `-Os` optimization.

## Flash

The SDK generates the flash configuration in the build directory during
configuration. The flash target uses that generated configuration; no
example-local INI file is required.

Flash the firmware in the default `build` directory; compilation flags are
not needed:

```bash
make flash CHIP=bl616 BOARD=bl616dk COMX=<board-serial-port>
make flash CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap COMX=<board-serial-port>
```

`make flash` does not build firmware. If previously using a custom output
directory, rebuild with the commands above before flashing.

After flashing and resetting the board, wait for both events before running
P2P commands:

```text
CODE_WIFI_ON_INIT_DONE
CODE_WIFI_ON_MGMR_DONE
```

## Form a group

P2P uses fixed interface `wl2` (FHOST VIF 1). A local GO uses
`192.168.49.1/24`. For a local GC, use the address from `GOT_IP`.

Autonomous GO on 5 GHz channel 36, then open WPS PBC:

```text
p2p_init 36
p2p_name bl-p2p-hp
p2p_group_add freq=5180 ht40 vht he passphrase=12345678
p2p_wps_pbc
```

Before measuring throughput, `p2p_status` should report `GROUP_ACTIVE` and
`COMPLETED`.

## Throughput test

P2P traffic uses VIF 1. Add `-I 1` to DUT iperf commands so they bind to
`wl2` instead of STA (`-I 0`).

Show the available iperf arguments:

```text
iperf -h
```

TCP RX (DUT as server; TX/RX are relative to the DUT):

```text
iperf -s -i 1 -I 1
```

TCP TX (DUT as client):

```text
iperf -c <peer-ip> -t 30 -i 1 -I 1
```

UDP RX:

```text
iperf -s -u -i 1 -I 1
```

UDP TX:

```text
iperf -c <peer-ip> -u -b 100M -t 30 -i 1 -I 1
```

A local GO uses `192.168.49.1`. When the DUT is GC, replace `<peer-ip>` with
the peer GO address.

TCP echo commands from `wifi_p2p` remain available (`wifi_tcp_echo_test`,
`wifi_tcp_test`) but are not the throughput measurement path.
