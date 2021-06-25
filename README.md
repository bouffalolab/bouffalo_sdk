![License](https://img.shields.io/badge/License-Apache--2.0-brightgreen)

[中文版](README_zh.md)

# Introduction

**bl mcu sdk** is an MCU software development kit provided by the Bouffalo Lab Team for BL602/BL604, BL702/BL704/BL706 and other series of chips in the future.

## Code Framework

```

bl_mcu_sdk
├── bsp
│ ├── board
│ │ ├── bl706_avb
│ │ ├── bl706_iot
│ │ └── bl602_xxx
│ └── bsp_common
├── common
│ ├── device
│ ├── libc
│ ├── list
│ ├── memheap
│ ├── misc
│ ├── partition
│ ├── ring_buffer
│ └── soft_crc
├── components
│ ├── fatfs
│ ├── freertos
│ ├── shell
│ └── usb_stack
├── docs
│ ├── bl70x_docs
│ ├── development_guide
├── drivers
│ ├── bl602_driver
│ └── bl702_driver
├── examples
│ ├── adc
│ ├── audio
│ ├── camera
│ ├── coremark
│ ├── dac
│ ├── dhrystone
│ ├── dma
│ ├── gpio
│ ├── hellowd
│ ├── i2c
│ ├── i2s
│ ├── memheap
│ ├── spi
│ ├── uart
│ └── usb
├── out
└── tools
    ├── bflb_flash_tool
    ├── cdk_flashloader
    ├── cmake
    └── openocd

```

- bsp/bsp_common : stores some common peripheral driver codes related to the board
- bsp/board : stores the board-level description file such as `clock_config.h` (describes the clock configuration file) `pinmux_config.h` (describes the io function file) `peripheral_config.h` (describes the default configuration file of the peripheral) , These files together describe the board hardware information.
- common : stores some common algorithm function libraries that have nothing to do with hardware
- components : store the third-party library public component library
- drivers store the public chip underlying driver library provided by Boliu Intelligence
- examples : store the official sample code
- out : intermediate file, used to store the bin file generated after compiling and linking
- tools : store toolkits related to compiling and downloading
- docs : store tutorial documents and other help information

For more file directory reference information, please refer to [bl mcu sdk documentation tutorial](http://bouffalolab.gitee.io/bl_mcu_sdk)

## Hierarchy

[![RpUVoj.png](https://z3.ax1x.com/2021/06/18/RpUVoj.png)](https://imgtu.com/i/RpUVoj)

# Resources

## Hardware Resources

- BL706 IOT Development board
[![RpUI0g.png](https://z3.ax1x.com/2021/06/18/RpUI0g.png)](https://imgtu.com/i/RpUI0g)

- BL706 AVB Development Board
[![Rpa7DO.jpg](https://z3.ax1x.com/2021/06/18/Rpa7DO.jpg)](https://imgtu.com/i/Rpa7DO)


There is currently no official purchase channel. If you want to get the above development board, you can apply for it in any open source community.

## Chip Manual

[Chip Reference Manual](https://dev.bouffalolab.com/media/upload/doc/BL702_BL704_706_RM_zh_CN_1.1.pdf) | [Chip Data Manual](https://dev.bouffalolab.com/media/upload/doc/BL702_BL704_BL706_DS_zh_CN_Combo_2.0.pdf)

For more information, please visit the BouffaloLab developer website: [https://dev.bouffalolab.com/](https://dev.bouffalolab.com/)


## Documentation Tutorial

Contains a quick start tutorial for bl mcu sdk development, api manual and a detailed introduction to the driver framework, etc.

- [bl mcu sdk documentation tutorial](http://bouffalolab.gitee.io/bl_mcu_sdk)

## Video tutorial

- [BL706 MCU Development Series Video Tutorial](https://www.bilibili.com/video/BV1xK4y1P7ur)

## development tools

### Command line development

For the tools needed for command line development, please refer to [linux development guide](http://bouffalolab.gitee.io/bl_mcu_sdk/get_started/Linux_quick_start_ubuntu.html)

- [cmake 3.19](https://cmake.org/files/v3.19/), cmake compilation tool, it is recommended to use cmake v3.19 or above

### Eclipse development

For the tools needed for Eclipse development, please refer to [Eclipse Development Guide](http://bouffalolab.gitee.io/bl_mcu_sdk/get_started/Windows_quick_start_eclipse.html)

- [Eclipse](https://dev.bouffalolab.com/media/upload/download/BouffaloLab_eclipse_x86_64_win.zip) eclipse development free installation package under Windows
- [J-Link v10](https://www.segger.com/downloads/jlink), J-Link debugger, used to debug the chip online, it is recommended to use the hardware of J-Link V10 or above, and the software driver is recommended to use V6 .98 version

### CDK Development

For tools needed for CDK development, please refer to  [CDK Development Guide](http://bouffalolab.gitee.io/bl_mcu_sdk/get_started/Windows_quick_start_cdk.html)

- [CDK](https://occ.t-head.cn/development/activities/cdk), CDK integrated development environment of Pingtou Gejianchi, it is recommended to use CDK v2.8.4 or above

### Burning tool

In addition to using CK-link, J-link and command line programming, it also supports graphical programming tools.
Graphical programming tools provided by Boliu:

- [Bouffalo Lab Dev Cube For Windows](https://dev.bouffalolab.com/media/upload/download/BouffaloLabDevCube-1.5.3-win32.zip)
- [Bouffalo Lab Dev Cube For Ubuntu](https://dev.bouffalolab.com/media/upload/download/BouffaloLabDevCube-1.5.3-linux-x86.tar.gz)

## Forum

Bouffalolab Developer Forum: [https://bbs.bouffalolab.com/](https://bbs.bouffalolab.com/)

# License

**bl mcu sdk** is completely open source and follows the Apache License 2.0 open source license agreement. It can be used in commercial products for free and does not require public private code.

```
/*
 * Copyright (c) 2021 Bouffalolab team
 *
 * SPDX-License-Identifier: Apache-2.0
 */
 ```