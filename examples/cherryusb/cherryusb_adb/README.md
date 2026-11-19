# CherryUSB ADB Device Demo

This example turns a Bouffalo SDK board into a USB ADB device using the
CherryUSB `adb` class. After flashing, the board enumerates with the standard
ADB interface (`class 0xff / subclass 0x42 / protocol 0x01`) and can be used
from a host with the normal `adb` command line tool.

Supported features:

- `adb devices` enumeration (no USB driver install on Linux, WinUSB on Windows)
- Interactive shell: `adb shell`
- One-shot commands: `adb shell help`, `adb shell echo hello`, ...
- Built-in commands: `help`, `echo`, `free`, `uptime`, `version`, `exit`

Not implemented in this demo:

- ADB RSA authentication (`A_AUTH`). The device does not ask the host for
  authentication, so any host can connect. Do not use this in production.
- File sync (`adb push` / `adb pull` / `adb ls`). The `sync:` service is
  rejected on purpose so that `adb push/pull` fail immediately instead of
  hanging.

## Hardware

Use a board with a USB device port, for example:

| Chip | Notes |
| --- | --- |
| BL616 / BL618DG / BL616CL | High-speed USB |
| BL702 | Full-speed USB only (no `CONFIG_USB_HS`) |

The default build target is the BL616DK board.

## Build and flash

```bash
cd examples/cherryusb/cherryusb_adb

# build
make CHIP=bl616 BOARD=bl616dk

# flash (adjust COMX)
make flash CHIP=bl616 COMX=/dev/ttyUSB0
```

The firmware is generated in `build/build_out/cherryusb_adb_bl616.bin`.

## Host setup

### Linux

Install adb (`sudo apt install adb android-sdk-platform-tools` on Debian /
Ubuntu) and add a udev rule so that the device node is accessible without
root. The demo uses VID:PID `18d1:4ee7`:

```bash
echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="18d1", MODE="0666", GROUP="plugdev"' \
    | sudo tee /etc/udev/rules.d/51-cherryusb-adb.rules
sudo udevadm control --reload-rules
sudo udevadm trigger
```

Then, after plugging the board:

```bash
adb kill-server
adb devices
# List of devices attached
# 2024111701      device

adb shell
# cherryadb:/ $ help
# ...
```

### Windows

Install the Android platform-tools and a driver for VID:PID `18D1:4EE7`.
The Google USB driver bundled with the Android SDK works, otherwise use
[Zadig](https://zadig.akeo.ie/) to bind the device to WinUSB.

## Example session

```text
$ adb devices
List of devices attached
2024111701      device

$ adb shell version
CherryUSB v1.6.1 (0x010601)

$ adb shell free
heap: 12345 bytes free, largest block: 9000 bytes

$ adb shell
cherryadb:/ $ echo hello
hello
cherryadb:/ $ uptime
uptime: 12.345 s
cherryadb:/ $ exit
bye
```

## How it works

- `main.c` starts the board and the FreeRTOS scheduler.
- `adb_demo.c` registers the USB descriptors, calls `usbd_adb_init_intf()`,
  and provides the hooks the ADB class expects:
  - `usbd_adb_notify_open()`: an `A_OPEN` request was accepted. An empty
    service means an interactive shell, otherwise the text after `shell:` is
    executed as a one-shot command.
  - `usbd_adb_notify_shell_read()`: shell input arrived; the bytes are pushed
    into a FreeRTOS stream buffer.
  - `usbd_adb_notify_write_done()`: the previous IN transfer completed.
- `adb_shell.c` implements the line based shell and its built-in commands.
- The endpoint callbacks run in USB interrupt context, so they only touch
  FreeRTOS primitives; all transfers and command execution happen in the ADB
  worker task.

`components/usb/Kconfig` exposes the class as `CONFIG_CHERRYUSB_DEVICE_ADB`.
Enable it together with `CONFIG_CHERRYUSB` and `CONFIG_CHERRYUSB_DEVICE` when
adding ADB to another application.

## Files

| File | Purpose |
| --- | --- |
| `main.c` | Board init and scheduler startup |
| `adb_demo.c` | USB descriptors, ADB hooks, RX/TX worker task |
| `adb_shell.c` | Minimal shell served over ADB |
| `config_file/usb_config.h` | CherryUSB configuration |
| `config_file/FreeRTOSConfig.h` | FreeRTOS configuration |
| `defconfig` | SDK configuration (`CONFIG_CHERRYUSB_DEVICE_ADB=y`) |
