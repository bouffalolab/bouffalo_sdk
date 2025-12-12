[![License](https://img.shields.io/badge/License-Apache--2.0-brightgreen)](LICENSE)

[中文版](README_zh.md)

# Introduction

**BouffaloSDK** is the IOT and MCU software development kit provided by the Bouffalo Lab Team, supports all the series of Bouffalo chips. Also it is the combination of **bl_mcu_sdk** and **bl_iot_sdk** 。

# SDK Architecture

![SDK Architecture](BouffaloSDK.png)

# Code Directories

| Name | Description |
|:---:|:------:|
| bsp/board |  clock, pinmux, memoryheap and console init |
| bsp/common | bsp common driver|
| components| components|
|docs | quick start、 api、demo doc|
|drivers/lhal| bouffalo common peripherals drivers which support all the chips|
|drivers/soc| bouffalo non-generic peripherals drivers|
|drivers/rfparam| rf param |
|examples| official samples|
|tools| tools |

# LHAL Support

**LHAL** is a low level hal driver for common peripherals designed by Bouffalo Lab, in order to support all the Bouffalo chips with the same api . Also it is convenient for users to use and port to other platforms.

Note：**√** means supported ; **×** means not supported; **○** means supported but not tested ; **-** means no such peripheral.

|   Peripheral |    BL602/BL604 |    BL702/BL704/BL706 | BL616/BL618 |   BL808  |
|:------------:|:--------------:|:--------------------:|:-----------:|:--------:|
|  ADC         |      ○         |      √             |   √           |   ○      |
|  CAM         |      -         |      √             |   √           |   √      |
|  CKS         |      ○         |      √             |   √           |   ○      |
|  CSI         |      -         |      -             |   -           |   √      |
|  DAC         |      ○         |      √             |   √           |   ○      |
|  DMA         |      ○         |      √             |   √           |   √      |
|  EFUSE       |      ×         |      ×             |   ×           |   ×      |
|  EMAC        |      -         |      √             |   √           |   √      |
|  FLASH       |      √         |      √             |   √           |   √      |
|  GPIO        |      ○         |      √             |   √           |   √      |
|  I2C         |      ○         |      √             |   √           |   ○      |
|  I2S         |      ○         |      ○             |   √           |   ○      |
|  IR          |      ○         |      √             |   √           |   ○      |
|  MJPEG       |      ×         |      ×             |   √           |   √      |
|  PWM_v1      |      ○         |      √             |   -           |   -      |
|  PWM_v2      |      -         |      -             |   √           |   √      |
|  RTC         |      ○         |      √             |   √           |   √      |
|  SEC_AES     |      ○         |      √             |   √           |   √      |
|  SEC_SHA     |      ○         |      √             |   √           |   √      |
|  SEC_TRNG    |      ○         |      √             |   √           |   √      |
|  SEC_PKA     |      ○         |      √             |   √           |   √      |
|  SPI         |      ○         |      √             |   √           |   √      |
|  TIMER       |      ○         |      √             |   √           |   √      |
|  UART        |      √         |      √             |   √           |   √      |
|  USB_v1      |      -         |      √             |   -           |   -      |
|  USB_v2      |      -         |      -             |   √           |   √      |
|  WDG         |      ○         |      √             |   √           |   ○      |

# Wireless Support

|   Peripheral |    BL602/BL604 |    BL702/BL704/BL706 | BL616/BL618 |   BL808  |
|:------------:|:--------------:|:--------------------:|:-----------:|:--------:|
|  WIFI4       |      √         |      -             |   -           |   ×      |
|  WIFI6       |      -         |      -             |   √           |   -      |
|  BT          |      -         |      -             |   ×           |   ×      |
|  BLE         |      √         |      ×             |   √           |   ×      |
|  ZIGBEE      |      -         |      ×             |   ×           |   ×      |

Note：**√** means supported ; **×** means not supported; **○** means supported but not tested ; **-** means no such peripheral.

# SBOM

| Area       | Component(s)            | Original Repository                                          | Version                           |
| ---------- | ----------------------- | ------------------------------------------------------------ | --------------------------------- |
| Multimedia | xwebsocket              | https://github.com/OlehKulykov/librws                        | 9-Jan-19                          |
| Networking | MQTT-C                  | https://github.com/LiamBindle/MQTT-C                         | 1.1.6                             |
| Bluetooth  | Zephyr                  | https://github.com/zephyrproject-rtos/zephyr/                | 2.1.0                             |
| Bluetooth  | sbc                     | https://android.googlesource.com/platform/external/bluetooth/bluedroid/ | UnKnown                           |
| Networking | LWIP                    | https://github.com/lwip-tcpip/lwip                           | 2.1.2                             |
| Networking | PPP                     | https://github.com/ppp-project/ppp                           | Unknown. Managed by upstream LWIP |
| Utils      | cJSON                   | https://github.com/DaveGamble/cJSON                          | 1.7.18                            |
| WLAN       | hostapd, wpa_supplicant | https://w1.fi/cgit/hostap                                    | v2.10-devel                       |
| BSP        | CherryUSB               | https://github.com/cherry-embedded/CherryUSB                 | v1.5.2                            |
| BSP        | FatFS                   | http://elm-chan.org/fsw/ff/                                  | R0.15                             |
| BSP        | FreeRTOS-Kernel         | https://github.com/FreeRTOS/FreeRTOS-Kernel/                 | V10.6.2                           |
| BSP        | LittleFS                | https://github.com/littlefs-project/littlefs                 | v2.8.2                            |
| BSP        | Nuttx                   | https://github.com/apache/nuttx                              | UnKnown                           |
| BSP        | TLSF                    | https://github.com/mattconte/tlsf                            | Mar 30, 2020                      |
| Graphics   | LVGL_v8                 | https://github.com/lvgl/lvgl                                 | 8.4.0                             |
| Graphics   | LVGL_v9                 | https://github.com/lvgl/lvgl                                 | 9.2.0                             |
| Networking | mbedtls                 | https://github.com/Mbed-TLS/mbedtls                          | 2.28.2                            |
| Utils      | musl getopt             | https://git.musl-libc.org/cgit/musl                          | UnKnown                           |
| Utils      | queue                   | https://web.mit.edu/freebsd/head/sys/sys/queue.h             | 8.5                               |
| Utils      | tree                    | https://ftp.netbsd.org/pub/NetBSD/NetBSD-current/src/sys/sys/tree.h | 1.8 (NetBSD), 1.7 (OpenBSD)       |

# Environment Setup

## Toolchain

- [riscv64-unknown-elf-gcc](https://github.com/bouffalolab/toolchain_gcc_t-head_windows) toolchain used in windows.
- [riscv64-unknown-elf-gcc](https://github.com/bouffalolab/toolchain_gcc_t-head_linux) toolchain used in linux.
- T-HEAD official does not provide macos toolchain, so you should build by yourself, refer to [build_toolchain_macos](https://github.com/p4ddy1/pine_ox64/blob/main/build_toolchain_macos.md).
- [linux-riscv64-linux-gcc](https://gitee.com/bouffalolab/linuxtoolchain_gcc_t-head) toolchain used for compiling linux code.

## Command Line Development

Before compiling with the command line, you need to select the corresponding toolchain according to your operating system, configure it to the system environment variables, and install the **make** or **ninja**, then you can do the following. For the details, you can visit [BouffaloSDK Environment Setup](https://bl-mcu-sdk.readthedocs.io/zh_CN/latest/get_started/get_started.html).

- Go to the demo directory where you want to compile and there are `main.c` and `Makefile` files in that directory
- Just execute the following command, take **BL616** as an example

```
cd examples/helloworld
make CHIP=bl616 BOARD=bl616dk
```

- If you use **BL602** , you can try

```
cd examples/helloworld
make CHIP=bl602 BOARD=bl602dk
```

- If you use **BL808** or **BL606P**, you need to add **CPU_ID** with m0 or d0.

If you want to use **ninja**, you can try:

```
cd examples/helloworld
make ninja CHIP=bl616 BOARD=bl616dk
```

```
cd examples/helloworld
make ninja CHIP=bl808 BOARD=bl808dk CPU_ID=m0
```

## CDK Development

TODO

## Debug

Only supports debug with CKLink currently. See [BouffaloSDK Debug Guide](https://bl-mcu-sdk.readthedocs.io/zh_CN/latest/get_started/eclipse.html).

## Firmware Download

Note that if you are using linux, linux will deny access to the serial device due to permission issues, so add your own username to `dialout` for easy to use, restart your linux and then take effect. Or add `sudo` before command.

```
usermod -aG dialout xxx # xxx is your own name
```

```
cd examples/helloworld
make flash CHIP=chip_name COMX=xxx # chip_name should be bl602/bl702/bl616/bl808/bl606p, COMX in Windows, /dev/ttyxxx in Linux
```

- If you use **BL808** or **BL606P**, you need to add **CPU_ID** with m0 or d0.

```
cd examples/helloworld
make flash CHIP=chip_name CPU_ID=m0 COMX=xxx # chip_name should be bl602/bl702/bl616/bl808/bl606p, COMX in Windows, /dev/ttyxxx in Linux
```

If flash using serial port rather than USB, different USB2TTL chips support different max baudrates, need to pay attention to when flashing.

| chip | baudrate|
|:---:|:------:|
| ch340 |  <= 500K |
| cp2102 |  <= 2M |
| ft232 |  <= 2M |
| bl702 |  <= 8M |
| bl616 |  <= 10M |

### Flash Cube User Guide

See [bouffalo_flash_cube doc](tools/bflb_tools/bouffalo_flash_cube/docs/FlashCube_User_Guide.pdf)

# Resources

## Chip Manual

**Chip Reference Manual** and **Chip Data Manual** are listed on [document](https://github.com/bouffalolab/bl_docs)

## Documentation Tutorial

To get more BouffaloSDK documentation tutorial, like api manual or peripheral demo and so on, please visit:

- [BouffaloSDK documentation tutorial](https://bl-mcu-sdk.readthedocs.io/zh_CN/latest/)

## Video Tutorial

TODO

## LCD Support List

[LCD Support List](bsp/common/lcd/README.md)

## Cam Sensor Support List

[Cam Sensor Support List](bsp/common/image_sensor/README.md)

## Forum

Bouffalolab Developer Forum: [https://bbs.bouffalolab.com/](https://bbs.bouffalolab.com/)
