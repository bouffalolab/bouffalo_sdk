# CherryUSB CLI Demo

[English](README.md) | [中文](README_zh.md)

This example provides CherryUSB device and host demonstrations controlled by
Shell commands. It supports CDC ACM, CDC ECM, CDC RNDIS, HID, MSC, UAC, and
UVC device functions, as well as Serial (including CDC ACM), CDC ECM, HID, and
MSC host functions.

## Features and Shell commands

### Device mode

| Function | Start command | Additional command |
|:---------|:--------------|:-------------------|
| CDC ACM virtual serial port and loopback | `usbd_cdc_acm_test` | `usbd_cdc_acm_stats` |
| CDC ECM Ethernet bridge | `usbd_cdc_ecm_test` | |
| CDC RNDIS Ethernet bridge | `usbd_cdc_rndis_test` | |
| HID keyboard | `usbd_hid_keyboard_test` | `usbd_hid_keyboard_pause` |
| MSC RAM disk | `usbd_msc_test` | |
| UAC 1.0 audio device | `usbd_uac_v1_test` | |
| UVC MJPEG camera | `usbd_uvc_mjpeg_test` | |
| CDC ACM + MSC composite device | `usbd_cdc_acm_msc_test` | |

Use `usbd_stop` to stop the active device test. Only one device test can run
at a time.

### Host mode

| Function | Test behavior after enumeration |
|:---------|:--------------------------------|
| CDC ACM | Runs a bidirectional throughput test |
| CDC ECM | Registers a lwIP network interface |
| HID | Receives and prints input reports |
| MSC | Mounts the device and performs write, read, and data verification tests |

Use `usbh_start` to start host mode, `lsusb` to list connected USB devices,
and `usbh_stop` to stop host mode. Device mode and host mode are mutually
exclusive.

> **Limitation:** Each Serial, CDC ECM, HID, and MSC host demo supports only
> one active instance. If multiple devices of the same class are connected, or
> one device exposes multiple interfaces of the same class, later instances do
> not start the corresponding demo function.

## Supported chips

| CHIP | BOARD | Remark |
|:----:|:-----:|:-------|
| BL702 | `bl702dk` | Device mode only; MSC device is disabled because of RAM limitations |
| BL616 | `bl616dk` | Device and host modes |
| BL616CL | `bl616cldk` | Device and host modes |
| BL618DG | `bl618dgdk` | AP core; device CDC ECM/RNDIS are not enabled |

The available Shell commands depend on the selected chip and configuration.
Enter `help` in the board Shell to view the commands included in the current
build.

## Compile

- BL702

```
make CHIP=bl702 BOARD=bl702dk
```

- BL616

```
make CHIP=bl616 BOARD=bl616dk
```

- BL616CL

```
make CHIP=bl616cl BOARD=bl616cldk
```

- BL618DG AP core

```
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap
```

## Flash

```
make flash CHIP=chip_name COMX=xxx
```

Replace `chip_name` and `xxx` with the target chip and serial port name.

## Test setup

1. Connect the board debug UART to a serial terminal.
2. Build and flash the example, then reset the board.
3. Enter `help` to confirm the available USB commands.
4. Connect the USB data port to the PC or USB peripheral required by the test.
5. Stop the active device or host mode before switching modes.

The CDC ECM and RNDIS device tests additionally require an Ethernet-capable
board and an Ethernet cable.

## USB device mode

### CDC ACM

**Command:** `usbd_cdc_acm_test`

**Hardware:** Windows or Linux PC and USB data cable

After enumeration, the PC creates a virtual serial port. Open the port with a
serial terminal and assert DTR to enter loopback mode. Data transmitted from
the PC is returned on the same virtual serial port. Run `usbd_cdc_acm_stats`
to inspect transfer statistics, and run `usbd_stop` when the test is complete.

Device log:

```
[I][ACM] Create USB device cdc_acm data send task...
[I][ACM] usbd cdc_acm machine start/restart
[I/USB] CDC_ACM USBD_EVENT_RESET
[I/USB] CDC_ACM configured done
[I][ACM] USBD ACM Ready!
[I][ACM] DTR set, Enter the loopback test mode
[I][ACM] DTR clear, Exit the loopback test mode
```

Loopback-mode log:

```
IACM DTR set, Enter the loopback test mode
```

Windows Device Manager:

<div align="center">
	<img src="./picture/windows_cdc_acm.png" alt="Windows CDC ACM virtual serial port" width="420" height="420">
</div>

Linux enumeration log:

```
$ sudo dmesg -w
[ 2354.648190]usb 3-5:new high-speed USB device number 5 using xhci hcd
[ 2354.797016] usb 3-5: New USB device found, idVendor=ffff, idProduct=ffff, bcdDevice= 1.00
[ 2354.797024] usb 3-5: New USB device strings: Mfr=1, Product=2, SerialNumber=3
[ 2354.797027] usb 3-5: Product: CherryUSB_CDC_DEMO
[ 2354.797029] usb 3-5: Manufacturer: CherryUSB
[ 2354.797031] usb 3-5: SerialNumber: 2022123456
[ 2354.816798] cdc_acm 3-5:1.0: ttyACM0: USB ACM device
[ 2354.816822] usbcore: registered new interface driver cdc_acm
[ 2354.816825] cdc_acm: USB Abstract Control Model driver for USB modems and ISDN adapters
```

### CDC ECM

**Command:** `usbd_cdc_ecm_test`

**Hardware:** Linux PC, Ethernet-capable board, USB data cable, and Ethernet cable

This test bridges the USB CDC ECM interface to the board EMAC. Use `usbd_stop`
to stop the test.

Device log:

```
[I][EPHY] eth phy scan success, phy_addr: 1, phy_id: 0x02430C54
[W][EPHY] drv_match falied, use General driver
[I][ECM] Create USB device cdc_ecm <-> emac task...
[I][ECM] USB device cdc_ecm <-> emac task start...
[I][ECM] usbd cdc_ecm machine start/restart
[W][ETH_EMAC] Eth Emac ReStart (LinkDown) !!!
[I/USB] CDC_ECM USBD_EVENT_CONFIGURED
[I][ECM] USBD ECM EMAC Ready!
[W][ETH_EMAC] Eth Emac LinkUp !!!
[I][ETH_EMAC] TX_BUF_CNT:0, RX_BUF_CNT:10
[I][ETH_EMAC] eth_phy speed: 100M_FULL_DUPLEX
[I][ECM] USBD ECM IN/RX start
[I][ECM] USBD ECM OUT/TX start

[I][ETH_EMAC] TX: success cnt:0, error cnt:0, total size:0Byte
[I][ETH_EMAC]     push_cnt:0, tx_db waiting:0, tx_bd_ptr:0
[I][ETH_EMAC] RX: success cnt:0, error cnt:0, total size:0Byte
[I][ETH_EMAC]     push_cnt:10, rx_db waiting:10, rx_bd_ptr:0, busy cnt:0
```

Linux enumeration and network-interface information:

```
$ sudo dmesg -w
[664826.119339] usb 1-3: new high-speed USB device number 10 using xhci_hcd
[664826.514987] usb 1-3: New USB device found, idVendor=ffff, idProduct=ffff, bcdDevice= 1.00
[664826.514991] usb 1-3: New USB device strings: Mfr=1, Product=2, SerialNumber=3
[664826.514992] usb 1-3: Product: CherryUSB_CDC_ECM_DEMO
[664826.514993] usb 1-3: Manufacturer: CherryUSB
[664826.514994] usb 1-3: SerialNumber: 2022123456
[664826.558607] cdc_ether 1-3:1.0 eth0: register 'cdc_ether' at usb-0000:00:14.0-3, CDC Ethernet Device, 18:b9:05:12:34:56
[664826.558639] usbcore: registered new interface driver cdc_ether
[664826.565021] cdc_ether 1-3:1.0 enx18b905123456: renamed from eth0

$ ifconfig
enx18b905123456: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
		inet 10.28.30.186  netmask 255.255.255.0  broadcast 10.28.30.255
		inet6 fe80::7b6e:119c:c26:e462  prefixlen 64  scopeid 0x20<link>
		ether 18:b9:05:12:34:56  txqueuelen 1000  (Ethernet)
		RX packets 2382  bytes 414519 (414.5 KB)
		RX errors 0  dropped 215  overruns 0  frame 0
		TX packets 208  bytes 30106 (30.1 KB)
		TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0
```

### CDC RNDIS

**Command:** `usbd_cdc_rndis_test`

**Hardware:** Windows PC, Ethernet-capable board, USB data cable, and Ethernet cable

This test bridges the Windows RNDIS network interface to the board EMAC. Use
`usbd_stop` to stop the test.

Device log:

```
[I][EPHY] eth phy scan success, phy_addr: 1, phy_id: 0x02430C54
[W][EPHY] drv_match falied, use General driver
[I][RNDIS] Create USB device cdc_rndis <-> emac task...
[I][RNDIS] USB device cdc_rndis <-> emac task start...
[I][RNDIS] usbd rndis machine start/restart
[W][ETH_EMAC] Eth Emac ReStart (LinkDown) !!!
[I/USB] CDC_RNDIS USBD_EVENT_CONFIGURED
[I][RNDIS] USBD RNDIS EMAC Ready!
[W][ETH_EMAC] Eth Emac LinkUp !!!
[I][ETH_EMAC] TX_BUF_CNT:0, RX_BUF_CNT:10
[I][ETH_EMAC] eth_phy speed: 100M_FULL_DUPLEX
[I][RNDIS] USBD RNDIS IN/RX start
[I][RNDIS] USBD RNDIS OUT/TX start
[I][ETH_EMAC] TX: success cnt:0, error cnt:0, total size:0Byte
[I][ETH_EMAC]     push_cnt:0, tx_db waiting:0, tx_bd_ptr:0
[I][ETH_EMAC] RX: success cnt:0, error cnt:0, total size:0Byte
[I][ETH_EMAC]     push_cnt:10, rx_db waiting:10, rx_bd_ptr:0, busy cnt:0

[I][ETH_EMAC] TX: success cnt:0, error cnt:0, total size:0Byte
[I][ETH_EMAC]     push_cnt:0, tx_db waiting:0, tx_bd_ptr:0
[I][ETH_EMAC] RX: success cnt:0, error cnt:0, total size:0Byte
[I][ETH_EMAC]     push_cnt:10, rx_db waiting:10, rx_bd_ptr:0, busy cnt:0
```

Windows network settings:

<div align="center">
	<img src="./picture/windows_rndis.png" alt="Windows RNDIS network interface" width="650" height="550">
</div>

### HID keyboard

**Command:** `usbd_hid_keyboard_test`

**Hardware:** Windows or Linux PC and USB data cable

The device sends keyboard reports continuously. Select an appropriate input
field before testing. Use `usbd_hid_keyboard_pause` to pause or resume report
transmission, and use `usbd_stop` to stop the test.

Device log:

```
[I][USBD_CLI] Create USB device usbd_hid_keyboard task...
[I][USBD_CLI] usbd_hid_keyboard_task run
[I][USBD_CLI] hid_keyboard_init done
[I/USB] HID KeyBoard configured done
```

### MSC

**Command:** `usbd_msc_test`

**Hardware:** Windows or Linux PC and USB data cable

After enumeration, the PC detects a virtual mass-storage drive. Use
`usbd_stop` to stop the test. This command is not enabled for BL702.

Device log:

```
[I/USB] MSC configured done
```

### UAC 1.0

**Command:** `usbd_uac_v1_test`

**Hardware:** Windows PC and USB data cable

After enumeration, open an audio application and select the USB audio device.
Use `usbd_stop` to stop the test.

Device log:

```
[I][USBD_CLI] Create USB device usbd_uac_v1 task...
[I][USBD_CLI] usbd_uac_v1_task run
[I][USBD_CLI] uac_v1_init done
[I/USB] UAC configured done
[I/USB] UAC OPEN SPK
```

Windows audio device:

<div align="center">
	<img src="./picture/windows_uac.png" alt="Windows UAC device" width="520" height="300">
</div>

### UVC MJPEG

**Command:** `usbd_uvc_mjpeg_test`

**Hardware:** Windows PC and USB data cable

After enumeration, open the Windows Camera application and select the USB
camera. Use `usbd_stop` to stop the test.

Device log:

```
[I][USBD_CLI] Create USB device usbd_uvc_mjpeg task...
[I][USBD_CLI] usbd_uvc_mjpeg_task run
[I][USBD_CLI] uvc_mjpeg_init done
[I/USB] UVC configured done
[I/USB] UVC OPEN
```

Windows Camera application:

<div align="center">
	<img src="./picture/windows_uvc.png" alt="Windows UVC camera output" width="500" height="390">
</div>

### CDC ACM + MSC composite device

**Command:** `usbd_cdc_acm_msc_test`

**Hardware:** Windows or Linux PC and USB data cable

The PC detects both a virtual serial port and a virtual mass-storage drive.
Use `usbd_stop` to stop the test. This command is not enabled for BL702.

Device log:

```
[I][ACM_MSC] Create USB device cdc_acm data send task...
[I][ACM] usbd cdc_acm machine start/restart
[I/USB] MSC CDC_ACM configured done
[I][ACM] USBD ACM Ready!
[I][ACM] DTR set, Enter the loopback test mode
[I][ACM] DTR clear, Exit the loopback test mode
```

## USB host mode

Run `usbh_start` before connecting the USB peripheral. After successful
enumeration, the corresponding class test starts automatically. Run `lsusb`
to inspect enumerated devices and `usbh_stop` when testing is complete.

### CDC ECM host

**Hardware:** CDC ECM device and USB data cable

The host prints the device MAC address and maximum segment size, registers a
network device, and creates a lwIP interface.

```
[I/usbh_hub] New high-speed device on Bus 0, Hub 1, Port 1 connected
[I/usbh_core] New device found,idVendor:ffff,idProduct:ffff,bcdDevice:0100
[I/usbh_core] The device has 1 bNumConfigurations
[I/usbh_core] The device has 2 interfaces
[I/usbh_core] Enumeration success, start loading class driver
[I/usbh_core] Loading cdc_ecm class driver
[I/usbh_cdc_ecm] CDC ECM MAC address 18:b9:05:12:34:56
[I/usbh_cdc_ecm] CDC ECM Max Segment Size:1514
[I/usbh_cdc_ecm] Ep=83 Attr=03 Mps=16 Interval=05 Mult=00
[I/usbh_cdc_ecm] Ep=02 Attr=02 Mps=512 Interval=00 Mult=00
[I/usbh_cdc_ecm] Ep=81 Attr=02 Mps=512 Interval=00 Mult=00
[I/usbh_cdc_ecm] Set CDC ECM packet filter:000c
[I/usbh_cdc_ecm] Register CDC ECM Class:/dev/cdc_ether
[I/USB] USBH CDC ECM run
USBH CDC ECM LWIP test start
[I/usbh_core] Loading cdc_data class driver
[I/USB] CDC ECM link down
```

### MSC host

**Hardware:** USB mass-storage device and USB data cable

After enumeration, the example performs write, read, and data verification
tests on the connected storage device.

> **Warning:** The test writes data to the connected storage device. Back up
> important data before running it.

```
[I/USB] New high-speed device on Hub 2, Port 1 connected
[I/USB] New device found,idVendor:04e8,idProduct:61fd,bcdDevice:0005
[I/USB] The device has 1 interfaces
[I/USB] Enumeration success, start loading class driver
[I/USB] Loading msc class driver
[I/USB] Get max LUN:1
[I/USB] Ep=81 Attr=02 Mps=512 Interval=00 Mult=00
[I/USB] Ep=02 Attr=02 Mps=512 Interval=00 Mult=00
[E/USB] csw bStatus 1
[I/USB] Capacity info:
[I/USB] Block num:15486976,block size:512
[I/USB] Register MSC Class:/dev/sda

[I/USB] ******************** be about to write test... **********************
[I/USB] Write Test Succeed!
[I/USB] Single data size:32768 Byte, Write the number:1024, Total size:32768 KB
[I/USB] Time:1766ms, Write Speed:18554 KB/s

[I/USB] ******************** be about to read test... **********************
[I/USB] Read Test Succeed!
[I/USB] Single data size:32768Byte, Read the number:1024, Total size:32768 KB
[I/USB] Time:1496ms, Read Speed:21903 KB/s

[I/USB] ******************** be about to check test... **********************
[I/USB] Check Test Succeed!
[I/USB] All Data Is Good!
```

### Serial host (CDC ACM)

**Hardware:** CDC ACM device and USB data cable

After enumeration, the CDC ACM device is registered as a unified Serial Class
instance. The example opens it with `usbh_serial_open`, asserts DTR through
`USBH_SERIAL_CMD_TIOCMSET`, and closes it with `usbh_serial_close`. It
deliberately does not call `USBH_SERIAL_CMD_SET_ATTR`: that command starts the
managed Serial RingBuffer receiver, whereas the Serial CDC raw asynchronous
APIs require `dwDTERate == 0` and directly keep one TX URB and one RX URB
active concurrently for a bidirectional throughput test.

```
[I/usbh_hub] New high-speed device on Bus 0, Hub 1, Port 1 connected
[I/usbh_core] New device found,idVendor:ffff,idProduct:ffff,bcdDevice:0100
[I/usbh_core] The device has 1 bNumConfigurations
[I/usbh_core] The device has 2 interfaces
[I/usbh_core] Enumeration success, start loading class driver
[I/usbh_core] Loading cdc_acm class driver
[I/usbh_cdc_acm] Ep=04 Attr=02 Mps=512 Interval=00 Mult=00
[I/usbh_cdc_acm] Ep=83 Attr=02 Mps=512 Interval=00 Mult=00
[I/usbh_serial] Register Serial Class: /dev/ttyACM0 (cdc_acm)
USBH serial speed test start
[I/usbh_core] Loading cdc_data class driver
USBH serial test time: 2000
USBH serial tx size: 23160832
USBH serial rx size: 23160907
USBH serial tx speed: 11309 KB/S
USBH serial rx speed: 11309 KB/S
USBH serial total speed: 22618 KB/S
USBH serial speed test end
```

### HID host

**Hardware:** USB mouse or keyboard and USB data cable

After enumeration, the device is registered as an input device and the example
receives HID input reports.

```
[I/usbh_hub] New low-speed device on Bus 0, Hub 1, Port 1 connected
[I/usbh_core] New device found,idVendor:413c,idProduct:2113,bcdDevice:0110
[I/usbh_core] The device has 1 bNumConfigurations
[I/usbh_core] The device has 2 interfaces
[I/usbh_core] Enumeration success, start loading class driver
[I/usbh_core] Loading hid class driver
[I/usbh_hid] Ep=81 Attr=03 Mps=8 Interval=10 Mult=00
[I/usbh_hid] Register HID Class:/dev/input0
USBH HID test start, hid_interval: 10ms
[I/usbh_core] Loading hid class driver
[W/usbh_hid] Do not support set idle
[I/usbh_hid] Ep=82 Attr=03 Mps=3 Interval=10 Mult=00
[I/usbh_hid] Register HID Class:/dev/input1
[E/USB] USBH HID test already running
USBH HID test end
```
