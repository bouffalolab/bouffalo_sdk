# NetHub Quick Bring-Up

This page gives the shortest customer bring-up flow for NetHub.

For the current chip and interface support state, see
[NetHub.md](NetHub.md), "Current Support Matrix".

## 1. Select Chip and Interface

Choose one host interface per firmware build.

| Interface | Required options |
| --- | --- |
| `SDIO` | `CONFIG_NETHUB_PROFILE_SDIO=y`, `CONFIG_NETHUB_PROFILE_USB=n` |
| `USB` | `CONFIG_NETHUB_PROFILE_USB=y`, `CONFIG_NETHUB_PROFILE_SDIO=n` |

Low power is supported for `BL616` NetHub builds. Keep
`CONFIG_NETHUB_LOWPOWER_ENABLE=n` for `BL616CL` and `BL618DG`.

## 2. Build Firmware

Build from the NetHub example directory:

```bash
cd examples/wifi/nethub
```

Build examples:

```bash
# BL616
make CHIP=bl616 BOARD=bl616dk

# BL616CL
make CHIP=bl616cl BOARD=bl616cldk

# BL618DG
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap
```

To force a specific interface, pass the interface options on the make command
line. For example:

```bash
# SDIO profile
make CHIP=bl616 BOARD=bl616dk \
  CONFIG_NETHUB_PROFILE_SDIO=y CONFIG_NETHUB_PROFILE_USB=n

# USB profile
make CHIP=bl616 BOARD=bl616dk \
  CONFIG_NETHUB_PROFILE_USB=y CONFIG_NETHUB_PROFILE_SDIO=n
```

## 3. Flash Firmware

Use the board's download UART:

```bash
cd examples/wifi/nethub
make flash CHIP=bl616 BOARD=bl616dk COMX=/dev/ttyUSB0
```

Replace `CHIP`, `BOARD`, and `COMX` according to the target board.

## 4. Host Bring-Up

For `SDIO`:

1. Connect the host SDIO pins to the device.
2. Build and load the Linux host SDIO driver and NetHub userspace tools.
3. Use the host control tool to scan, connect to an AP, and confirm that the
   host network interface has an IP address.

For `USB`:

1. Connect the device USB port to the host.
2. Confirm that the host enumerates a USB ECM network interface.
3. If the example AT control channel is enabled, confirm that the host also
   sees the USB ACM control interface.
4. Use the host control flow to connect Wi-Fi and verify network traffic.

## 5. Default SDIO Pins

| Pin Function | BL616 (`bl616dk`) | BL616CL (`bl616cldk`) | BL618DG (`bl618dgdk`) |
| --- | --- | --- | --- |
| `SDIO_DAT2` | `GPIO10` | `GPIO6` | `GPIO43` |
| `SDIO_DAT3` | `GPIO11` | `GPIO7` | `GPIO44` |
| `SDIO_CMD` | `GPIO12` | `GPIO8` | `GPIO45` |
| `SDIO_CLK` | `GPIO13` | `GPIO9` | `GPIO46` |
| `SDIO_DAT0` | `GPIO14` | `GPIO10` | `GPIO47` |
| `SDIO_DAT1` | `GPIO15` | `GPIO11` | `GPIO48` |

## 6. Default USB Pins

| Chip / Board | Default USB pins |
| --- | --- |
| BL616 (`bl616dk`) | board default USB port |
| BL616CL (`bl616cldk`) | `GPIO32`, `GPIO33` |
| BL618DG (`bl618dgdk`) | `GPIO40`, `GPIO41` |

For custom boards, confirm USB `D+` and `D-` polarity from the board schematic.

## 7. Common Questions

### 7.1 Which Interface Should I Bring Up First?

Use `SDIO` for the default bring-up path. Use `USB` when the host connection is
USB. `SPI` is not supported in the current customer bring-up flow.

For chip-specific status and low-power support, see
[NetHub.md](NetHub.md), "Current Support Matrix".

### 7.2 The Host Control Tool Cannot Start

This applies only when the example AT control channel is enabled.

Check the following:

- the device and host are using the same interface profile, `SDIO` or `USB`
- the selected host interface is detected by Linux
- for `USB`, the ECM network interface and ACM control interface are both
  enumerated when the example AT control channel is enabled
- for `SDIO`, the host SDIO driver and userspace tools are built and loaded
  for the same SDK version

If `CONFIG_NETHUB_CTRLCHANNEL_USE_ATMODULE=n`, the example AT control channel
is not included. In that case, product software must provide its own control
flow.

### 7.3 Wi-Fi Connect Succeeds but the Host Has No Network Access

Check the following:

- the Wi-Fi connection has reached the got-IP state on the device side
- the host network interface exists and is up
- the host network interface has the expected IP address, netmask, gateway, and
  DNS settings
- host network managers are not overriding the NetHub interface configuration

For `USB`, check the ECM network interface. For `SDIO`, check the virtual
network interface created by the host SDIO stack.

### 7.4 The Host Interface Does Not Appear in `ifconfig`

Use `ifconfig -a` or `ip link` to list interfaces that are currently down.

For `USB`, first confirm that the USB device enumerated successfully. For
`SDIO`, confirm that the host SDIO driver is loaded and the SDIO hardware link
is detected.

### 7.5 Can the Host Expose AP and STA at the Same Time?

No. The current NetHub flow exposes one active Wi-Fi endpoint to the host at a
time.

The default active endpoint is `STA`. Product software can switch the active
endpoint with `nethub_set_active_wifi_channel()` when an `AP` flow is required.

### 7.6 What Changes When `CONFIG_NETHUB_CTRLCHANNEL_USE_ATMODULE=n`?

The NetHub data path remains available, but the example AT control channel is
not built.

Product software must then provide any required private control flow, including
Wi-Fi control commands and host network-interface configuration.

### 7.7 Is Low Power Supported on Every Platform?

No. Low power is currently supported for `BL616` NetHub builds.

Keep `CONFIG_NETHUB_LOWPOWER_ENABLE=n` for `BL616CL` and `BL618DG` unless their
low-power support is updated in [NetHub.md](NetHub.md), "Current Support
Matrix".

### 7.8 Can I Customize the Wi-Fi RX Policy?

Yes. The built-in policy is shared by `SDIO` and `USB`; there is no separate
USB-specific or SDIO-specific policy split.

If the default policy does not match the product requirement, register a custom
filter with `nethub_set_wifi_rx_filter()` before NetHub bootstrap.

### 7.9 Why Can Some LAN Traffic Behave Differently from a Native Host Wi-Fi Interface?

NetHub is a Wi-Fi bridge flow running through device-side Wi-Fi software, not a
native host Wi-Fi driver. Some local packets may be handled on the device side
by the default Wi-Fi RX policy.

If the product needs different packet ownership between device and host, use a
custom Wi-Fi RX filter policy.

## 8. Next Pages

- [NetHub.md](NetHub.md)
- [NetHubArchitecture.md](NetHubArchitecture.md)
- [NetHubVirtualChannel.md](NetHubVirtualChannel.md)
