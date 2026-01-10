# 开发工作流

## 环境设置

### 工具链安装

#### 下载工具链

**Windows:**
```bash
# 下载地址
https://github.com/bouffalolab/toolchain_gcc_t-head_windows
```

**Linux:**
```bash
# 下载地址
https://github.com/bouffalolab/toolchain_gcc_t-head_linux

# 解压
tar -xzf riscv64-unknown-elf-gcc.tar.gz
```

**macOS:**
```bash
# macOS需要自建工具链
# 参考：https://github.com/p4ddy1/pine_ox64/blob/main/build_toolchain_macos.md
```

#### 配置工具链

**Windows:**
```bash
# 添加到PATH（系统环境变量）
C:\path\to\toolchain\bin
```

**Linux:**
```bash
# 添加到PATH
export PATH=/path/to/toolchain/bin:$PATH

# 添加到~/.bashrc或~/.zshrc
echo 'export PATH=/path/to/toolchain/bin:$PATH' >> ~/.bashrc
source ~/.bashrc
```

**验证工具链：**
```bash
riscv64-unknown-elf-gcc --version
```

### 安装构建工具

**Linux:**
```bash
# Ubuntu/Debian
sudo apt-get install build-essential cmake ninja-build python3

# CentOS/RHEL
sudo yum install make cmake python3
```

**Windows:**
- 安装 [CMake](https://cmake.org/download/)
- 安装 [Ninja](https://github.com/ninja-build/ninja/releases)

**macOS:**
```bash
brew install cmake ninja python3
```

### 安装Python依赖

```bash
# 进入SDK目录
cd bouffalo_sdk

# 安装依赖（可选）
pip3 install -r docs/requirements.txt
```

### 配置串口访问（Linux）

```bash
# 添加用户到dialout组
sudo usermod -aG dialout $USER

# 重新登录或执行
newgrp dialout

# 验证权限
ls -l /dev/ttyUSB0
```

## 项目创建

### 基于示例创建

1. **复制示例：**
```bash
cp -r examples/helloworld my_project
cd my_project
```

2. **修改代码：**
```c
// 编辑main.c
#include "bflb_mtimer.h"
#include "bflb_uart.h"

int main(void)
{
    printf("Hello, Bouffalo SDK!\n");
    while(1) {
        bflb_mtimer_delay_ms(1000);
    }
}
```

3. **配置项目：**
```bash
# 编辑Kconfig
vim Kconfig

# 编辑defconfig
vim defconfig
```

### 从零创建

1. **创建目录：**
```bash
mkdir my_project
cd my_project
```

2. **创建Makefile：**
```makefile
include $(BL_SDK_BASE)/project.build
```

3. **创建CMakeLists.txt：**
```cmake
cmake_minimum_required(VERSION 3.15)

add_executable(${PROJECT_NAME}
    main.c
)
```

4. **创建Kconfig：**
```kconfig
config CONFIG_MY_PROJECT
    bool "My Project"
    default y
```

5. **创建defconfig：**
```
CONFIG_DEBUG=y
CONFIG_FREERTOS=y
```

6. **创建main.c：**
```c
#include "bflb_mtimer.h"

int main(void)
{
    printf("Hello, World!\n");
    while(1) {
        bflb_mtimer_delay_ms(1000);
    }
}
```

## 编译流程

### 单核芯片（BL616、BL602、BL702）

```bash
# 1. 进入项目目录
cd my_project

# 2. 配置项目
make menuconfig

# 3. 编译
make CHIP=bl616 BOARD=bl616dk

# 4. 或使用Ninja
make ninja CHIP=bl616 BOARD=bl616dk
```

### 多核芯片（BL808、BL606P）

```bash
# M0核心
make CHIP=bl808 BOARD=bl808dk CPU_ID=m0

# D0核心
make CHIP=bl808 BOARD=bl808dk CPU_ID=d0
```

## 配置管理

### menuconfig 使用

```bash
# 启动图形化配置
make menuconfig
```

**常见配置项：**
- `System Type → SoC Selection`: 选择芯片
- `Board Selection`: 选择开发板
- `RTOS Selection`: 选择RTOS（FreeRTOS/NuttX）
- `Wireless Options`: 启用WiFi/BLE
- `File Systems`: 启用文件系统
- `Debug Options`: 启用调试

### 保存配置

```bash
# 保存配置为.defconfig
make savedefconfig

# 使用defconfig
make load-defconfig CONFIG=my_defconfig
```

### 配置对比

```bash
# 对比两个配置
make diffconfig config1.defconfig config2.defconfig
```

## 烧录固件

### 基本烧录

```bash
# Windows
make flash CHIP=bl616 COMX=COM5

# Linux
make flash CHIP=bl616 COMX=/dev/ttyUSB0
```

### 多核芯片烧录

```bash
# M0核心
make flash CHIP=bl808 CPU_ID=m0 COMX=/dev/ttyUSB0

# D0核心
make flash CHIP=bl808 CPU_ID=d0 COMX=/dev/ttyUSB0
```

### 波特率设置

**默认波特率：2Mbps**

```bash
# 设置波特率
make flash CHIP=bl616 COMX=/dev/ttyUSB0 BAUDRATE=2000000

# 不同USB转串口芯片支持的最大波特率：
# ch340: ≤500K
# cp2102: ≤2M
# ft232: ≤2M
# bl702: ≤8M
# bl616: ≤10M
```

### Flash Cube 烧录工具

```bash
# 使用Bouffalo Flash Cube
tools/bflb_tools/bouffalo_flash_cube/BLFlashCommand[.exe|-linux|-macos] \
    --interface=uart \
    --baudrate=2000000 \
    --port=/dev/ttyUSB0 \
    --chipname=bl616 \
    --flash
```

## 调试

### CKLink 连接

1. **连接CKLink：**
   - SWCLK: SWD时钟
   - SWDIO: SWD数据
   - GND: 地
   - VCC: 可选（3.3V）

2. **配置GDB：**
```bash
# 启动GDB服务器
openocd -f interface/cklink.cfg -f target/bouffalo.cfg

# 启动GDB
riscv64-unknown-elf-gdb build/build_out/my_project.elf

# 在GDB中连接
target remote :3333
load
continue
```

### Eclipse 调试

1. **导入项目：**
   - File → Import → C/C++ → Existing Code as Makefile Project

2. **配置调试器：**
   - Run → Debug Configurations
   - 选择GDB硬件调试
   - 设置CKLink参数

3. **使用预配置：**
```bash
# 使用SDK提供的Eclipse配置
cp -r tools/eclipse_gdb_launch/ ~/workspace/.metadata/.plugins/org.eclipse.debug.core/.launches/
```

### GDB 初始化脚本

SDK提供GDB初始化脚本：

```bash
# 使用gdbinit脚本
riscv64-unknown-elf-gdb -x tools/gdbinit/bl616.gdbinit build/build_out/my_project.elf
```

### 崩溃分析

```bash
# 使用崩溃分析工具
tools/crash_tools/crash_analyzer.py \
    --elf=build/build_out/my_project.elf \
    --core=build/build_out/coredump.bin
```

## 日志和调试

### printf 调试

```c
#include "bflb_uart.h"

void app_main(void)
{
    printf("Debug info: %d\n", value);
}
```

### Shell 调试

```c
#include "shell.h"

void cmd_test(int argc, char **argv)
{
    printf("Test command\n");
}

// 注册命令
SHELL_CMD_EXPORT(cmd_test, "Test command");
```

### 串口查看器

```bash
# Linux (minicom)
minicom -D /dev/ttyUSB0 -b 2000000

# Linux (screen)
screen /dev/ttyUSB0 2000000

# Windows (PuTTY)
# 配置串口、波特率
```

## 常见开发任务

### 添加自定义组件

1. **创建组件目录：**
```bash
mkdir -p components/my_component/include
mkdir -p components/my_component/src
```

2. **创建CMakeLists.txt：**
```cmake
cmake_minimum_required(VERSION 3.15)

add_library(my_component STATIC
    src/my_component.c
)

target_include_directories(my_component PUBLIC
    include
)
```

3. **创建Kconfig：**
```kconfig
config CONFIG_MY_COMPONENT
    bool "Enable my component"
    default y
```

4. **在主Kconfig中添加：**
```kconfig
source "components/my_component/Kconfig"
```

5. **在主CMakeLists.txt中添加：**
```cmake
add_subdirectory(components/my_component)
```

### 修改启动代码

1. **找到启动代码：**
```bash
# FreeRTOS启动
components/os/freertos/portable/port.c

# NuttX启动
components/os/nuttx/nuttx/
```

2. **修改初始化顺序：**
```c
// 在main()之前添加初始化
void system_init(void)
{
    // 自定义初始化
}

// 调用
board_init();
system_init();
```

### 添加自定义外设驱动

1. **创建LHAL驱动：**
```bash
mkdir drivers/lhal/src/my_peripheral.c
mkdir drivers/lhal/include/bflb_my_peripheral.h
```

2. **实现驱动：**
```c
#include "bflb_my_peripheral.h"

bflb_device_t* bflb_my_peripheral_init(uint8_t id, const struct bflb_my_peripheral_config *cfg)
{
    // 初始化代码
}
```

3. **添加设备表：**
```c
struct bflb_device my_peripheral_devs[] = {
    [0] = {
        .name = "my_peripheral0",
        .priv = &my_peripheral0_config,
        .ops = &my_peripheral_ops
    }
};
```

## 性能分析

### Puncover 分析

```bash
# 生成大小报告
make size

# 使用Puncover分析
make puncover

# 打开浏览器
# http://localhost:5001
```

### Flash 使用分析

```bash
# 查看Flash使用
riscv64-unknown-elf-size build/build_out/my_project.elf

# 生成详细报告
riscv64-unknown-elf-objdump -h build/build_out/my_project.elf
```

### 内存使用分析

```bash
# 查看内存使用
riscv64-unknown-elf-nm -S build/build_out/my_project.elf | grep ' B '

# 使用heap分析
# 启用CONFIG_TLSF_DEBUG=y
```

## 常见问题排查

### 编译错误

**问题：** 找不到头文件

**解决：**
1. 检查 `CMakeLists.txt` 中的 `target_include_directories`
2. 检查 Kconfig 配置
3. 清理后重新编译 `make clean && make`

### 链接错误

**问题：** undefined reference

**解决：**
1. 检查是否链接了正确的库
2. 检查Kconfig中是否启用了相关组件
3. 查看构建日志中的库路径

### 烧录失败

**问题：** 烧录超时或失败

**解决：**
1. 降低波特率
2. 检查串口权限（Linux）
3. 检查串口连接
4. 尝试不同的USB转串口芯片

### 运行时崩溃

**问题：** 程序运行后崩溃

**解决：**
1. 启用coredump
2. 使用崩溃分析工具
3. 使用GDB调试
4. 检查堆栈溢出

### WiFi/BLE 连接问题

**问题：** 无法连接到WiFi或BLE

**解决：**
1. 检查Kconfig配置
2. 检查天线连接
3. 检查RF配置
4. 启用调试日志

## 版本管理

### SDK版本

```bash
# 查看SDK版本
cat VERSION

# 查看版本信息
cat bsp/sdk_version.c
```

### 组件版本

```bash
# 查看组件版本
# - LwIP: components/net/lwip/
# - mbedTLS: components/crypto/mbedtls/
# - FreeRTOS: components/os/freertos/
```

## 最佳实践

### 1. 使用版本控制

```bash
# 使用Git管理项目
git init
git add .
git commit -m "Initial commit"

# 创建.gitignore
echo "build/" > .gitignore
echo "*.o" >> .gitignore
```

### 2. 使用menuconfig配置

```bash
# 总是使用menuconfig，而不是手动修改Kconfig
make menuconfig
```

### 3. 清理构建

```bash
# 配置改变后总是清理
make clean
```

### 4. 使用调试日志

```bash
# 启用调试信息
menuconfig → Debug Options → Enable debug output
```

### 5. 文档和注释

```c
// 添加文档注释
/**
 * @brief 函数描述
 * @param param1 参数1说明
 * @return 返回值说明
 */
int my_function(int param1)
{
    // 实现代码
}
```

## 总结

Bouffalo SDK提供了完整的开发工作流，从环境设置、项目创建、编译、烧录到调试。遵循最佳实践可以有效提高开发效率。遇到问题时，参考examples目录中的示例代码。
