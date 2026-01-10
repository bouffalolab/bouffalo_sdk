---
name: bouffalo-sdk
description: Bouffalo SDK开发指南，提供完整的Bouffalo芯片系列开发文档和参考资料。当需要使用bouffalo_sdk开发IoT/MCU应用、了解SDK架构、配置构建系统、使用外设驱动、开发无线功能、管理电源、调试固件时使用此skill。
---

# Bouffalo SDK 开发指南

本skill提供Bouffalo SDK的完整开发指南，涵盖从基础配置到高级应用的所有方面。

## 项目概述

Bouffalo SDK是Bouffalo Lab提供的IoT和MCU软件开发套件，支持所有Bouffalo芯片系列，包括BL602/BL702/BL616/BL808等。

开始前，阅读[01-overview.md](references/01-overview.md)了解：
- SDK简介和芯片系列
- 目录结构和核心设计原则
- 关键特性和依赖的第三方库
- 开发环境要求和常用资源

## SDK架构

理解SDK架构是高效开发的基础。

阅读[02-architecture.md](references/02-architecture.md)了解：
- 整体架构和分层设计
- LHAL（低级别硬件抽象层）设计理念
- 组件化系统和板级抽象（BSP）
- 构建系统和配置系统
- 无线协议栈架构
- 多核架构（BL808、BL606P、BL616D）
- 内存管理、安全架构、电源管理架构

## LHAL驱动

LHAL提供跨芯片的统一外设API。

阅读[03-lhal-drivers.md](references/03-lhal-drivers.md)了解：
- LHAL设计和API模式
- 核心外设驱动（GPIO、UART、SPI、I2C、I2S、ADC、DAC、DMA、定时器、PWM、RTC等）
- 中断管理
- 跨芯片兼容性（设备表机制）
- 性能优化（ROM API、DMA加速）
- 最佳实践

## 芯片差异

不同芯片系列在外设、无线协议、内存、功耗等方面有差异。

阅读[04-chips-diff.md](references/04-chips-diff.md)了解：
- 各芯片系列特性和应用场景
- 外设支持矩阵
- 核心差异（单核、双核、多核）
- 内存差异（Flash、SRAM、PSRAM）
- 电源管理差异
- 编译差异（CPU_ID参数）
- 选择建议和移植指南

## 无线开发

SDK支持多种无线协议：WiFi 4/6、BLE、Zigbee、Thread。

阅读[05-wireless.md](references/05-wireless.md)了解：
- 无线协议支持矩阵
- WiFi架构和使用（Station、AP模式、Socket通信）
- BLE架构和使用（Peripheral、Central、GATT）
- WiFi/BLE共存（Coex）
- 无线协议选择建议（功耗、吞吐量、连接数量对比）
- 调试和测试
- 常见问题

## 构建系统

Bouffalo SDK使用混合构建系统：Makefile（入口）+ CMake（构建引擎）+ Kconfig（配置系统）。

阅读[06-build-system.md](references/06-build-system.md)了解：
- 构建流程和Makefile详解
- CMakeLists.txt和Kconfig语法
- 配置系统（menuconfig使用）
- 工具链配置和编译器标志
- 组件构建
- 常见构建问题
- 最佳实践

## 开发工作流

从环境设置到编译、烧录、调试的完整开发流程。

阅读[07-development-workflow.md](references/07-development-workflow.md)了解：
- 工具链安装（Windows/Linux/macOS）
- 安装构建工具和Python依赖
- 配置串口访问（Linux）
- 项目创建（基于示例或从零创建）
- 编译流程（单核/多核芯片）
- 配置管理（menuconfig）
- 烧录固件（基本烧录、多核芯片、波特率设置）
- 调试（CKLink、Eclipse、GDB初始化脚本、崩溃分析）
- 日志和调试（printf、Shell、串口查看器）
- 常见开发任务（添加自定义组件、修改启动代码、添加自定义外设驱动）
- 性能分析（Puncover、Flash使用、内存使用）
- 常见问题排查
- 版本管理和最佳实践

## 外设使用

通过LHAL API使用各种外设。

阅读[08-peripherals.md](references/08-peripherals.md)了解：
- GPIO（中断、常用应用）
- UART（轮询、中断、DMA模式）
- SPI（主机/从机模式）
- I2C（主机模式、常用应用）
- I2S（音频接口）
- ADC（模数转换）
- DAC（数模转换）
- 定时器（中断模式）
- PWM（PWM v1/v2）
- RTC（实时时钟、闹钟）
- DMA（直接内存访问）
- 外设最佳实践（初始化顺序、错误处理、资源释放、RTOS使用、低功耗）

## 组件系统

SDK提供丰富的软件组件：操作系统、网络协议栈、文件系统、加密库、图形库、多媒体组件等。

阅读[09-components.md](references/09-components.md)了解：
- 组件目录结构
- 操作系统组件（FreeRTOS、NuttX）
- 网络组件（LwIP、MQTT-C）
- 文件系统组件（FatFS、LittleFS、ROMFS）
- 加密组件（mbedTLS）
- 图形库组件（LVGL v8/v9）
- USB组件（CherryUSB）
- 工具库组件（cJSON、partition）
- 核间通信组件（OpenAMP + RPMsg）
- Shell组件（命令行界面）
- 组件依赖管理
- 自定义组件
- 组件最佳实践

## 内存管理

SDK提供多种内存管理方案。

阅读[10-memory-management.md](references/10-memory-management.md)了解：
- 内存层次（Flash、SRAM、PSRAM）
- 芯片内存配置
- 内存管理方案（TLSF、FreeRTOS Heap、NuttX MM）
- Flash管理（读/写/擦除、分区、加密）
- PSRAM管理（初始化、配置、使用、分配）
- 内存对齐
- 内存优化（减少堆使用、复用内存、使用const、编译器优化）
- 内存调试（内存泄漏检测、内存破坏检测、堆栈溢出检测）
- 低功耗内存管理
- 内存映射
- 最佳实践

## 安全特性

SDK提供多层安全机制。

阅读[11-security.md](references/11-security.md)了解：
- 安全架构和硬件安全引擎
- AES加密（ECB、CBC、CTR、GCM模式）
- SHA哈希（SHA-1、SHA-224、SHA-256）
- TRNG（真随机数生成）
- PKA（公钥算法）
- 安全启动
- 安全OTA（HTTPS FOTA）
- Flash加密
- 密钥管理（EFUSE、硬件密钥存储）
- 抗回滚保护
- mbedTLS集成
- 安全最佳实践
- 安全检查清单

## 电源管理

SDK提供完整的电源管理功能。

阅读[12-power-management.md](references/12-power-management.md)了解：
- 电源模式（运行、浅睡眠、深度睡眠、休眠、关机）
- 各模式特性和对比
- 运行模式、浅睡眠、深度睡眠、休眠模式的使用
- 唤醒机制（GPIO、定时器、RTC）
- 低功耗优化（动态频率调整、关闭未使用的外设、使用低功耗模式、WiFi/BLE低功耗）
- 电源管理配置（Kconfig、动态配置）
- 状态保存和恢复
- 低功耗测量
- 多核电源管理（BL808、BL616）
- 低功耗最佳实践
- 常见问题

## 示例代码

SDK提供丰富的示例代码，涵盖从基础功能到复杂应用。

阅读[13-examples.md](references/13-examples.md)了解：
- 示例目录结构
- 基础示例（helloworld）
- 外设示例（UART、SPI、I2C、ADC、PWM等）
- WiFi示例（Station、AP、HTTP请求）
- BLE示例（Peripheral、Central）
- 文件系统示例（FatFS、LittleFS）
- FreeRTOS示例
- LVGL示例
- 电源管理示例
- 使用示例的步骤
- 基于示例创建项目
- 示例最佳实践

## 快速开始

### 编译示例

```bash
# 进入示例目录
cd examples/helloworld

# 编译BL616
make CHIP=bl616 BOARD=bl616dk
```

### 烧录固件

```bash
# Linux
make flash CHIP=bl616 COMX=/dev/ttyUSB0

# Windows
make flash CHIP=bl616 COMX=COM5
```

### 多核芯片编译

```bash
# BL808 M0核心
make CHIP=bl808 BOARD=bl808dk CPU_ID=m0
```

## 常用资源

- **项目主页**: https://github.com/bouffalolab/bouffalo_sdk
- **官方文档**: https://bl-mcu-sdk.readthedocs.io/
- **开发者论坛**: https://bbs.bouffalolab.com/
- **芯片手册**: https://github.com/bouffalolab/bl_docs

## 使用建议

1. **新手入门**：从01-overview.md开始，然后阅读13-examples.md，运行helloworld示例
2. **了解架构**：阅读02-architecture.md理解SDK设计
3. **开发应用**：根据需求阅读对应的references（外设→08-peripherals.md，无线→05-wireless.md等）
4. **配置构建**：阅读06-build-system.md和07-development-workflow.md
5. **优化性能**：阅读10-memory-management.md、11-security.md、12-power-management.md
6. **故障排查**：在对应的references中查找常见问题部分
