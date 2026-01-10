# Bouffalo SDK 项目概述

## 简介

Bouffalo SDK 是 Bouffalo Lab 提供的 IoT 和 MCU 软件开发套件，支持所有 Bouffalo 芯片系列。该项目整合了 `bl_mcu_sdk` 和 `bl_iot_sdk`，提供统一的开发平台。

## 芯片系列

### BL602/BL604
- **特点**: WiFi 4, BLE, 低功耗
- **应用**: 智能家居传感器、IoT设备
- **核心**: 单核 RISC-V

### BL702/BL704/BL706
- **特点**: BLE, 丰富外设
- **应用**: 可穿戴设备、便携设备
- **核心**: 单核 RISC-V

### BL616/BL618/BL616D
- **特点**: WiFi 6, BLE, 双核
- **应用**: 高性能IoT网关、智能摄像头
- **核心**: 双核 (M0 + LP)

### BL808
- **特点**: WiFi, 丰富外设, 高性能
- **应用**: 智能家电、工业物联网
- **核心**: 多核 (M0 + D0)

### BL606P
- **特点**: 类似BL808
- **核心**: 多核 (M0 + D0)

## 目录结构

```
bouffalo_sdk/
├── bsp/                  # 板级支持包
│   ├── board/            # 板级配置（时钟、引脚、内存）
│   └── common/           # 通用BSP驱动
│
├── components/          # 软件组件
│   ├── ai/               # AI相关组件
│   ├── crypto/           # 加密组件（mbedTLS）
│   ├── fota/             # 固件OTA升级
│   ├── fs/               # 文件系统（FatFS、LittleFS、ROMFS）
│   ├── graphics/         # 图形库（LVGL v8/v9）
│   ├── ipc/              # 核间通信
│   ├── net/              # 网络组件（LwIP）
│   ├── os/               # 操作系统（FreeRTOS、NuttX）
│   ├── shell/            # 命令行shell
│   ├── usb/              # USB（CherryUSB）
│   ├── utils/            # 工具库
│   └── wireless/         # 无线协议栈
│       ├── wifi4/        # WiFi 4
│       ├── wifi6/        # WiFi 6
│       ├── bluetooth/    # BLE（Zephyr stack）
│       ├── zigbee/       # Zigbee
│       └── thread/       # Thread
│
├── drivers/              # 驱动层
│   ├── lhal/             # 低级别硬件抽象层（统一API）
│   ├── soc/              # 芯片特定驱动
│   └── rfparam/          # RF参数
│
├── examples/             # 官方示例
│   ├── helloworld/       # 基础示例
│   ├── peripherals/      # 外设示例
│   ├── wifi/             # WiFi示例
│   ├── btble/            # BLE示例
│   ├── freertos/         # FreeRTOS示例
│   ├── fatfs/            # FatFS示例
│   ├── lvgl/             # LVGL示例
│   └── [更多...]
│
├── tools/                # 开发工具
│   ├── bflb_tools/       # 烧录工具
│   ├── cmake/            # CMake工具
│   ├── gdbinit/          # GDB初始化脚本
│   ├── eclipse_gdb_launch/  # Eclipse调试配置
│   └── crash_tools/      # 崩溃分析工具
│
├── docs/                 # 文档
│   └── doxygen/          # API文档生成配置
│
├── cmake/                # CMake配置
├── CMakeLists.txt        # 主CMake文件
├── Kconfig               # Kconfig配置
├── project.build         # Makefile包装器
├── board.build           # 板级构建配置
├── README.md             # 英文README
└── CLAUDE.md             # AI开发指南
```

## 核心设计原则

### 1. 模块化设计
- **分层架构**: 应用层 → LHAL → SOC驱动
- **组件复用**: 可重用的软件组件库
- **板级抽象**: BSP层隔离硬件差异

### 2. 跨平台兼容
- **统一API**: LHAL提供跨芯片的统一接口
- **配置管理**: Kconfig系统支持灵活配置
- **工具链统一**: 使用统一的RISC-V工具链

### 3. 易于移植
- **标准接口**: 遵循标准的HAL接口设计
- **最小依赖**: 组件间解耦
- **文档完善**: 提供详细的API文档和示例

## 关键特性

### 无线连接
- WiFi 4 (802.11n) - BL602
- WiFi 6 (802.11ax) - BL616
- Bluetooth Low Energy (BLE) - 多芯片支持
- Zigbee - 部分芯片支持
- Thread - 部分芯片支持

### 外设支持
- **通信接口**: UART, SPI, I2C, I2S, CAN
- **定时器**: 多路定时器、PWM
- **ADC/DAC**: 模数/数模转换
- **存储**: Flash, PSRAM, SDIO
- **安全**: AES, SHA, TRNG, PKA
- **多媒体**: 摄像头、音频编解码

### 开发工具
- **构建系统**: Make + CMake + Ninja
- **配置工具**: Kconfig + menuconfig
- **调试支持**: CKLink + GDB
- **烧录工具**: Bouffalo Flash Cube

## 依赖的第三方库

| 领域 | 组件 | 版本 | 用途 |
|-----|------|------|------|
| 操作系统 | FreeRTOS | V10.6.2 | 实时操作系统 |
| 操作系统 | NuttX | Unknown | 实时操作系统（可选） |
| 网络协议栈 | LwIP | 2.1.2 | TCP/IP协议栈 |
| 加密 | mbedTLS | 2.28.2 | 加密库 |
| 无线 | wpa_supplicant | v2.10 | WiFi认证 |
| 无线 | Zephyr Bluetooth | 2.1.0 | BLE协议栈 |
| 文件系统 | FatFS | R0.15 | FAT文件系统 |
| 文件系统 | LittleFS | v2.8.2 | 轻量级文件系统 |
| 图形 | LVGL v8 | 8.4.0 | 嵌入式图形库 |
| 图形 | LVGL v9 | 9.2.0 | 嵌入式图形库 |
| JSON | cJSON | 1.7.18 | JSON解析 |
| USB | CherryUSB | v1.5.2 | USB协议栈 |

## 开发环境要求

### 工具链
- **RISC-V工具链**: riscv64-unknown-elf-gcc
- **构建工具**: make 或 ninja
- **Python**: 3.x（用于配置工具）

### 操作系统
- Linux (推荐)
- Windows
- macOS (需要自建工具链)

### 硬件
- **调试器**: CKLink（当前唯一支持）
- **烧录器**: USB转串口或专用烧录器

## 常用资源

- **项目主页**: https://github.com/bouffalolab/bouffalo_sdk
- **官方文档**: https://bl-mcu-sdk.readthedocs.io/
- **开发者论坛**: https://bbs.bouffalolab.com/
- **芯片手册**: https://github.com/bouffalolab/bl_docs
