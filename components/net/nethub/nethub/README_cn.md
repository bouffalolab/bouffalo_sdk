# nethub框架

nethub是一个高效的多接口网络数据转发框架，支持zerocopy数据传输和静态配置的转发规则。

## 特性

- **多接口支持**：支持STA、AP、TCPIPSTA、TCPIPAP、TCPIPNAT、SDIO、USB、SPI、BRIDGE等多种接口类型
- **Zerocopy传输**：通过回调函数机制实现高效数据传输，避免数据拷贝
- **静态转发规则**：编译时定义转发规则，减少运行时开销，提升性能
- **简洁API**：最小化的接口设计，易于集成和使用
- **统一日志系统**：Linux内核风格的4级日志控制（nh_err, nh_warn, nh_info, nh_debug）
- **一键测试**：内置make test命令，快速验证所有功能

## 架构设计

### 核心组件

1. **接口管理器** (`nhif_ops.h`, `nhif_manager.c`)
   - 管理所有注册的网络接口
   - 提供标准化的接口操作抽象

2. **转发引擎** (`nh_forward.h`, `nh_forward.c`)
   - 实现数据包转发逻辑
   - 支持多种转发规则类型：FORWARD、CONDITIONAL、BYPASS

3. **核心框架** (`nethub.h`, `nethub.c`)
   - 协调各个组件工作
   - 提供统一的初始化和控制接口

4. **日志系统** (`nh_log.h`)
   - Linux内核风格的4级日志控制
   - 统一的日志宏：nh_err, nh_warn, nh_info, nh_debug
   - 编译时可选择性包含，优化代码大小

5. **转发规则配置** (`forwardrules/`)
   - 预定义的静态转发规则配置
   - SDIO WiFi规则：STA → TCPIPSTA → SDIO
   - Hostless规则：STA ↔ TCPIPSTA, AP ↔ TCPIPAP

### 数据结构

#### nh_skb_t - 数据包缓冲区
```c
typedef struct nh_skb {
    uint8_t *data;              // 数据缓冲区指针
    uint32_t len;               // 数据长度
    void (*free_cb)(void *priv, void *frame_elem); // 释放回调
    void *cb_priv;              // 回调私有参数
    void *cb_arg;               // 回调参数
    struct nh_skb *next;        // 链表指针
} nh_skb_t;
```

#### nhif_ops - 接口操作函数
```c
struct nhif_ops {
    nhif_type_t type;                      // 接口类型
    int (*init)(void);                     // 初始化
    int (*deinit)(void);                   // 反初始化
    nhforward_callback_t input;            // 输入回调
    nhforward_callback_t output;           // 输出回调
};
```

### 转发规则类型

- **NH_RULE_TYPE_FORWARD**：直接转发到指定接口
- **NH_RULE_TYPE_CONDITIONAL**：根据上一个接口的返回值决定是否转发
- **NH_RULE_TYPE_BYPASS**：绕过此接口，不转发

## 配置系统

nethub框架采用静态配置系统，转发规则在编译时定义，减少运行时开销。

### 转发规则配置

框架提供预定义的转发规则配置，位于`forwardrules/`目录：

- **`forwardrules/sdiowifi_rules.c`** - SDIO WiFi应用转发规则
  - 转发规则：STA → TCPIPSTA → SDIO 链式转发
  - 使用方式：`nethub_init(sdiowifi_get_forward_config())`
  - 适用于需要SDIO通信的WiFi设备

- **`forwardrules/hostless_rules.c`** - Hostless应用转发规则
  - 转发规则：STA ↔ TCPIPSTA, AP ↔ TCPIPAP 双向转发
  - 使用方式：`nethub_init(hostless_get_forward_config())`
  - 适用于独立运行的嵌入式设备

每个配置文件提供统一的接口：`*_get_forward_config()`

### 日志级别配置

```c
/* 日志级别配置 */
#define CONFIG_NETHUB_LOG_LEVEL      4        /**< 日志级别：DEBUG级别 */

/* 日志级别说明 */
/* 0: 无日志输出 */
/* 1: ERROR级别 (nh_err) */
/* 2: WARNING级别 (nh_err, nh_warn) */
/* 3: INFO级别 (nh_err, nh_warn, nh_info) */
/* 4: DEBUG级别 (nh_err, nh_warn, nh_info, nh_debug) */
```

## 使用方法

### 1. 包含头文件
```c
#include "nethub.h"
```

### 2. 实现接口操作函数
```c
static nh_forward_result_t my_if_input(nh_skb_t *skb, void *arg)
{
    NH_UNUSED(arg);  // 避免未使用参数警告
    // 处理接收到的数据
    printf("接收到数据：长度 %u\n", skb->len);
    return NH_FORWARD_CONTINUE;
}

static nh_forward_result_t my_if_output(nh_skb_t *skb, void *arg)
{
    NH_UNUSED(arg);  // 避免未使用参数警告
    // 处理要发送的数据
    printf("发送数据：长度 %u\n", skb->len);
    return NH_FORWARD_CONTINUE;
}

static const struct nhif_ops my_if_ops = {
    .type = NHIF_TYPE_STA,
    .init = my_if_init,
    .deinit = my_if_deinit,
    .input = my_if_input,
    .output = my_if_output
};
```

### 3. 注册接口
```c
int ret = nhif_register(&my_if_ops, private_data);
if (ret != NH_OK) {
    printf("接口注册失败：%d\n", ret);
    return -1;
}
```

### 4. 初始化框架
```c
ret = nethub_init(sdiowifi_get_forward_config());  // 使用预定义配置
if (ret != NH_OK) {
    printf("nethub初始化失败：%d\n", ret);
    return -1;
}
```

### 5. 启动框架
```c
ret = nethub_start();
if (ret != NH_OK) {
    printf("nethub启动失败：%d\n", ret);
    return -1;
}
```

### 6. 处理数据
```c
// 当接口接收到数据时
nh_skb_t *skb = create_skb(...);
nethub_process_input(skb, NHIF_TYPE_STA);
```

### 7. 清理资源
```c
nethub_stop();
nethub_deinit();
```

## 自定义转发规则

可以通过定义自己的转发规则表来定制转发行为：

```c
static const nh_forward_rule_t custom_sta_rules[] = {
    {NHIF_TYPE_TCPIPSTA, NH_RULE_TYPE_FORWARD, false},
    {NHIF_TYPE_BRIDGE, NH_RULE_TYPE_CONDITIONAL, true}
};

static const nh_if_forward_table_t custom_if_tables[] = {
    [NHIF_TYPE_STA] = {custom_sta_rules, 2},
    [NHIF_TYPE_SDIO] = {custom_sdio_rules, 1},
};

static const nh_forward_config_t custom_config = {
    .if_tables = custom_if_tables,
    .if_count = NHIF_TYPE_MAX
};

// 使用自定义配置初始化
nethub_init(&custom_config);
```

## 构建示例

```bash
mkdir build
cd build
cmake ..
make

# 一键运行所有示例测试
make test

# 或者单独运行示例
./examples/sdiowifi/sdiowifi_demo
./examples/hostless/hostless_demo
```

### make test命令

`make test`命令会自动：
- 检查并构建项目（如果需要）
- 逐个运行所有示例程序
- 为每个测试提供清晰的输出标题
- 自动处理超时和错误情况
- 提供测试完成总结

这是验证nethub框架功能的推荐方式。

## 快速集成

nethub框架可以作为子仓库集成到其他项目中，提供预定义的转发规则配置。

### 支持的应用配置

1. **SDIO WiFi配置** (`forwardrules/sdiowifi_rules.c`)
   - 支持SDIO传输接口
   - 转发规则：STA → TCPIPSTA → SDIO 链式转发
   - 适用于需要SDIO通信的WiFi设备

2. **Hostless配置** (`forwardrules/hostless_rules.c`)
   - 无SDIO依赖的独立场景
   - 转发规则：STA ↔ TCPIPSTA, AP ↔ TCPIPAP 双向转发
   - 适用于独立运行的嵌入式设备

### 集成步骤

#### 1. 添加子仓库
```bash
git submodule add https://github.com/your-repo/nethub.git libs/nethub
```

#### 2. 配置CMakeLists.txt
```cmake
cmake_minimum_required(VERSION 3.15)
project(my_app)

# 添加nethub头文件路径
include_directories(libs/nethub/include)
include_directories(libs/nethub/forwardrules)

# 收集nethub源文件
set(NETHUB_SOURCES
    libs/nethub/src/nethub.c
    libs/nethub/src/nh_forward.c
    libs/nethub/src/nhif_manager.c
    libs/nethub/forwardrules/sdiowifi_rules.c  # 或 hostless_rules.c
)

# 创建可执行文件
add_executable(my_app
    ${NETHUB_SOURCES}
    main.c
)
```

#### 3. 应用代码示例
```c
#include <stdio.h>
#include "nethub.h"

/* 声明转发规则配置函数 */
extern const nh_forward_config_t* sdiowifi_get_forward_config(void);
// 或
// extern const nh_forward_config_t* hostless_get_forward_config(void);

int main(void)
{
    // 初始化nethub框架
    int ret = nethub_init(sdiowifi_get_forward_config());
    if (ret != NH_OK) {
        printf("nethub初始化失败：%d\n", ret);
        return -1;
    }

    // 启动nethub框架
    ret = nethub_start();
    if (ret != NH_OK) {
        printf("nethub启动失败：%d\n", ret);
        return -1;
    }

    // 你的应用逻辑...

    // 清理资源
    nethub_stop();
    nethub_deinit();

    return 0;
}
```

## 目录结构

```
nethub_proj/
├── include/               # 头文件
│   ├── nethub_types.h    # 核心数据类型
│   ├── nhif_ops.h        # 接口操作定义
│   ├── nh_forward.h      # 转发规则配置
│   └── nethub.h          # 主接口
├── src/                  # 源文件
│   ├── nethub.c          # 核心框架实现
│   ├── nhif_manager.c    # 接口管理器
│   └── nh_forward.c      # 转发引擎
├── forwardrules/         # 转发规则配置
│   ├── sdiowifi_rules.c  # SDIO WiFi应用转发规则
│   └── hostless_rules.c  # Hostless应用转发规则
├── examples/             # 应用示例
│   ├── sdiowifi/         # SDIO WiFi应用示例
│   └── hostless/         # Hostless应用示例
├── Kconfig               # 配置文件
├── Makefile              # 构建和测试脚本
├── CMakeLists.txt        # CMake构建配置
└── README.md             # 本文档
```

## API参考

### nethub核心API
- `nethub_init()` - 初始化框架
- `nethub_start()` - 启动框架
- `nethub_stop()` - 停止框架
- `nethub_deinit()` - 反初始化框架
- `nethub_process_input()` - 处理输入数据

### 接口管理API
- `nhif_register()` - 注册接口
- `nhif_unregister()` - 注销接口
- `nhif_find_by_type()` - 查找接口

### 转发配置API
- `sdiowifi_get_forward_config()` - 获取SDIO WiFi转发配置
- `hostless_get_forward_config()` - 获取Hostless转发配置
- `nh_forward_print_rules()` - 打印当前转发规则

## 错误码

- `NH_OK` (0) - 操作成功
- `NH_ERR_INVALID_PARAM` (-1) - 无效参数
- `NH_ERR_NOT_FOUND` (-2) - 接口未找到
- `NH_ERR_ALREADY_EXISTS` (-3) - 接口已存在
- `NH_ERR_NO_MEMORY` (-4) - 内存不足
- `NH_ERR_NOT_INITIALIZED` (-5) - 未初始化
- `NH_ERR_INVALID_STATE` (-6) - 无效状态
- `NH_ERR_INTERNAL` (-7) - 内部错误

## 注意事项

1. **内存管理**：nh_skb_t的内存管理由调用方负责
2. **线程安全**：当前实现不是线程安全的，需要在单线程环境中使用
3. **回调函数**：input/output回调函数应该尽快返回，避免阻塞
4. **规则配置**：转发规则表在运行时不能修改，需要在初始化时设置

## 许可证

本项目采用MIT许可证，详见LICENSE文件。

## 贡献

欢迎提交Issue和Pull Request来改进nethub框架。