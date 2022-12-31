[![License](https://img.shields.io/badge/License-Apache--2.0-brightgreen)](LICENSE)
[![Release](https://img.shields.io/github/v/tag/bouffalolab/bl_mcu_sdk?color=s&label=release)]()

[中文版](README_zh.md)

# Introduction

**bl mcu sdk** is an MCU software development kit provided by the Bouffalo Lab Team, supports all the series of Bouffalo chips.

# SDK Versions & Chip Support

Note1: **drivers before v1.4.5 use v1.0（hal + std）, the later version will use v2.0 (lhal + soc)**。If you want to use **v1.4.5**, please checkout your branch to [release-v1.4.5](https://github.com/bouffalolab/bl_mcu_sdk/tree/release_v1.4.5).

Note2: due to the non-generic peripherals, the code style and interface name in **soc** are still the previous version, but will be subsequently updated to the new code style.

|   CHIP        |  v1.4.5  |  latest |
|:-------------:|:--------:|:-------:|
|BL602/BL604    |  √       |   √     |
|BL702/BL704/BL706 |  √    |   √     |
|BL616/BL618     |  ×      |   √     |
|BL808     |  ×            |   √     |

## LHAL Support

**LHAL** is a low level hal driver for common peripherals designed by Bouffalo Lab, in order to support all the Bouffalo chips with the same api . Also it is convenient for users to use and port to other platforms.

Note：**√** means supported ; **×** means not supported; **○** means supported but not tested ; **-** means no such peripheral.

|   Peripheral |    BL602/BL604 |    BL702/BL704/BL706 | BL616/BL618 |   BL808  |
|:------------:|:--------------:|:--------------------:|:-----------:|:--------:|
|  ADC         |      ○         |      √             |   √           |   ○      |
|  CAM         |      -         |      √             |   √           |   √      |
|  CKS         |      ○         |      √             |   √           |   ○      |
|  DAC         |      ○         |      √             |   √           |   ○      |
|  DMA         |      ○         |      √             |   √           |   √      |
|  EFUSE       |      √         |      √             |   √           |   √      |
|  EMAC        |      -         |      √             |   √           |   √      |
|  FLASH       |      √         |      √             |   √           |   √      |
|  GPIO        |      ○         |      √             |   √           |   √      |
|  I2C         |      ○         |      √             |   √           |   √      |
|  IR          |      ○         |      √             |   √           |   ○      |
|  MJPEG       |      ×         |      √             |   √           |   √      |
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

## Code Framework

```

bl_mcu_sdk
├── bsp
│   ├── board
│   │   └── bl602
│   │   └── bl702
│   │   └── bl616
│   │   └── bl808
│   └── common
├── components
│   ├── bflog
│   ├── cherryusb
│   ├── fatfs
│   ├── freertos
│   ├── lua
│   ├── lvgl
│   ├── lwip
│   └── shell
├── docs
├── drivers
│   ├── lhal
│   └── soc
├── examples
│   ├── bflog
│   ├── fatfs
│   ├── freertos
│   ├── helloworld
│   ├── lua
│   ├── lvgl
│   ├── peripherals
│   └── shell
└── tools
    └── cmake
    └── kconfig
    └── make

```

- **bsp/board** : including clock, pinmux, memoryheap and console init
- **bsp/common** : including bsp common driver
- **components** : including third-party components
- **docs** : including docs
- **drivers/lhal** : including bouffalo common peripherals drivers which support all the chips
- **drivers/soc** : including bouffalo non-generic peripherals drivers
- **examples** : including samples
- **tools** : including compiler tools

# Environment Setup

## Toolchain

- [riscv64-unknown-elf-gcc](https://gitee.com/bouffalolab/toolchain_gcc_t-head_windows) toolchain used in windows.
- [riscv64-unknown-elf-gcc](https://gitee.com/bouffalolab/toolchain_gcc_t-head_linux) toolchain used in linux.
- [linux-riscv64-linux-gcc](https://gitee.com/bouffalolab/linuxtoolchain_gcc_t-head) toolchain used for compiling linux code.

## Command Line Development

Before compiling with the command line, you need to select the corresponding toolchain according to your operating system, configure it to the system environment variables, and install the **make** or **ninja**, then you can do the following. For the details, you can visit [bl mcu sdk Environment Setup](https://bl-mcu-sdk.readthedocs.io/zh_CN/latest/get_started/index.html).

- Go to the demo directory where you want to compile and there are `main.c` and `Makefile` files in that directory
- Just execute the following command, take **BL616** as an example

```
cd examples/helloworld
make CHIP=bl616 BOARD=bl616dk
```

- If you use **BL808** or **BL606P**, you need to add **CPU_ID** with m0 or d0.

If you want to use **ninja**, you can try:

```
cd examples/helloworld
make ninja CHIP=bl616 BOARD=bl616dk
```

## CDK Development

TODO

## Debug

Only supports debug with CKLink currently. See [bl mcu sdk Debug Guide](https://bl-mcu-sdk.readthedocs.io/zh_CN/latest/get_started/debug.html).

## Firmware Download

We recommend you to use this **BL DevCube** and download firmware with serial.

- [Bouffalo Lab Dev Cube](https://dev.bouffalolab.com/download)
- [Bouffalo Lab Dev Cube User Guide](https://bl-mcu-sdk.readthedocs.io/zh_CN/latest/get_started/devcube.html)

# Resources

## Chip Manual

**Chip Reference Manual** and **Chip Data Manual** are listed on [document](https://github.com/bouffalolab/bl_docs)

## Documentation Tutorial

To get more bl mcu sdk documentation tutorial, like api manual or peripheral demo and so on, please visit:

- [bl mcu sdk v2.0 documentation tutorial](https://bl-mcu-sdk.readthedocs.io/zh_CN/latest/)
- [bl mcu sdk  v1.4.5 documentation tutorial](https://dev.bouffalolab.com/media/doc/sdk/bl_mcu_sdk_en/index.html)

## Video Tutorial

- [BL706 MCU Development Series Video Tutorial](https://www.bilibili.com/video/BV1xK4y1P7ur)

## Forum

Bouffalolab Developer Forum: [https://bbs.bouffalolab.com/](https://bbs.bouffalolab.com/)

# License

**bl mcu sdk** follows the Apache License 2.0 open source license agreement. It can be used in commercial products for free and does not require public private code.

```
/*
 * Copyright (c) 2022 Bouffalolab team
 *
 * SPDX-License-Identifier: Apache-2.0
 */
 ```