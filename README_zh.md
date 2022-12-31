[![License](https://img.shields.io/badge/License-Apache--2.0-brightgreen)](LICENSE)
[![Release](https://img.shields.io/github/v/tag/bouffalolab/bl_mcu_sdk?color=s&label=release)]()

[English Version](README.md)

# 简介

**bl mcu sdk** 是 Bouffalo Lab 提供的 MCU 软件开发包，支持博流智能所有系列芯片。

# SDK 版本与芯片支持情况

注意1：**v1.4.5 版本之前的驱动库使用 1.0 版本（hal + std），之后的版本都将使用 2.0 版本 (lhal + soc)**。如果使用 **v1.4.5** ，请切换分支到 [release-v1.4.5](https://github.com/bouffalolab/bl_mcu_sdk/tree/release_v1.4.5)。

注意2：**soc** 由于是非通用的外设，代码风格和接口名还是延续之前版本，但是后续会更新到新的代码风格。

|   芯片        |  v1.4.5  |  latest |
|:-------------:|:--------:|:-------:|
|BL602/BL604    |  √       |   √     |
|BL702/BL704/BL706 |  √    |   √     |
|BL616/BL618     |  ×      |   √     |
|BL808     |  ×            |   √     |

## LHAL 支持情况

**LHAL** 是博流为统一通用外设接口而设计的驱动库，代码精炼并且支持博流所有系列芯片，方便用户使用和移植到其他平台。

备注：**√** 表示已支持；**×** 表示未支持；**○** 表示已支持但未测试；**-** 表示没有该外设。

|   外设       |    BL602/BL604 |    BL702/BL704/BL706 | BL616/BL618 |   BL808  |
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

# 代码框架

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

- **bsp/board** : 包含时钟、引脚、内存管理和 console 的板级初始化
- **bsp/common** : 存放一些板级相关的常用外设驱动代码
- **components** : 存放第三方库公共组件
- **docs** : 存放教程文档以及其他帮助信息
- **drivers/lhal** : 存放博流智能系列芯片通用外设驱动，支持所有系列芯片
- **drivers/soc** : 存放博流智能系列芯片非通用外设驱动，各个芯片独有的部分
- **examples** : 存放官方提供的示例代码
- **tools** : 存放编译下载相关的工具包

# 环境搭建

## 工具链

- windows 下使用 [riscv64-unknown-elf-gcc](https://gitee.com/bouffalolab/toolchain_gcc_t-head_windows)
- Linux 下使用 [riscv64-unknown-elf-gcc](https://gitee.com/bouffalolab/toolchain_gcc_t-head_linux)
- 编译 linux 时使用 [linux-riscv64-linux-gcc](https://gitee.com/bouffalolab/linuxtoolchain_gcc_t-head)

## 命令行编译

在进行命令行编译之前，需要根据你的操作系统，选择对应的工具链，并配置到系统环境变量，并安装了 **make** 或者 **ninja** 工具，然后才能进行下面操作。更详细的搭建过程，参考 [bl mcu sdk 环境搭建](https://bl-mcu-sdk.readthedocs.io/zh_CN/latest/get_started/index.html).

- 进入要编译的 demo 目录，且该目录下有 `main.c` 和 `Makefile` 文件
- 执行下面命令即可,以 **BL616** 为例

```
cd examples/helloworld
make CHIP=bl616 BOARD=bl616dk
```

- 如果使用 **BL808** 或者 **BL606P** ,需要添加 **CPU_ID**

```
cd examples/helloworld
make CHIP=bl808 BOARD=bl808dk CPU_ID=m0
```

如果你想使用 **ninja** 编译，你可以尝试：

```
cd examples/helloworld
make ninja CHIP=bl616 BOARD=bl616dk
```

## CDK 编译

TODO

## 调试

当前仅支持使用 CKLink 调试。详细参考 [bl mcu sdk 调试指南](https://bl-mcu-sdk.readthedocs.io/zh_CN/latest/get_started/debug.html)。

## 固件烧录

推荐使用 **BL DevCube** 并通过串口进行固件的烧录。

- [Bouffalo Lab Dev Cube](https://dev.bouffalolab.com/download)
- [Bouffalo Lab Dev Cube User Guide](https://bl-mcu-sdk.readthedocs.io/zh_CN/latest/get_started/devcube.html)

# 芯片手册

芯片数据手册和参考手册见 [文档](https://github.com/bouffalolab/bl_docs)。

# 文档教程

获取更多 bl mcu sdk 开发相关的教程，如环境搭建、 api 手册、外设 demo 等，请参考：

- [bl mcu sdk v2.0 文档教程](https://bl-mcu-sdk.readthedocs.io/zh_CN/latest/)
- [bl mcu sdk v1.4.5 文档教程](https://dev.bouffalolab.com/media/doc/sdk/bl_mcu_sdk_zh/index.html)

# 视频教程

- [BL706 MCU 开发系列视频教程](https://www.bilibili.com/video/BV1xK4y1P7ur)

# 论坛

博流开发者交流论坛: [https://bbs.bouffalolab.com/](https://bbs.bouffalolab.com/)

# 许可协议

**bl mcu sdk** 遵循 Apache License 2.0 开源许可协议，可以免费在商业产品中使用，并且不需要公开私有代码。

```
/*
 * Copyright (c) 2022 Bouffalolab team
 *
 * SPDX-License-Identifier: Apache-2.0
 */

```