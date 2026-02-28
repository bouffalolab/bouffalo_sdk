# 电源管理

## 电源管理概览

Bouffalo SDK 提供完整的电源管理功能，包括运行模式、睡眠模式、低功耗优化、唤醒机制等。

## 电源模式

### 模式层级

```
运行模式 (RUN)
   ↓
浅睡眠 (SHALLOW_SLEEP)
   ↓
深度睡眠 (DEEP_SLEEP)
   ↓
休眠 (HIBERNATE)
   ↓
关机 (SHUTDOWN)
```

### 模式对比

| 模式 | 功耗 | RAM保持 | Flash访问 | 唤醒时间 |
|-----|------|---------|----------|---------|
| RUN | ~200mA | ✓ | ✓ | 0 |
| SHALLOW_SLEEP | ~20mA | ✓ | ✓ | ~10μs |
| DEEP_SLEEP | ~100μA | 部分 | X | ~1ms |
| HIBERNATE | ~10μA | 部分 | X | ~10ms |
| SHUTDOWN | ~5μA | ✗ | X | - |

## 运行模式

### 特性

- 所有外设可用
- CPU全速运行
- 最高性能

### 使用

```c
void normal_mode(void)
{
    // 默认运行模式
    // 无需特殊配置

    // 运行应用代码
    while(1) {
        // 任务代码
        bflb_mtimer_delay_ms(100);
    }
}
```

## 浅睡眠模式

### 特性

- 低功耗
- 快速唤醒
- RAM保持
- 支持定时器唤醒

### 进入浅睡眠

```c
#include "bflb_pm.h"

void enter_shallow_sleep(void)
{
    // 配置睡眠时间
    uint32_t sleep_time_ms = 1000;

    // 进入浅睡眠
    bflb_pm_shallow_sleep(sleep_time_ms);

    // 唤醒后继续执行
    printf("Woke up from shallow sleep\n");
}
```

### 配置唤醒源

```c
// 定时器唤醒
void setup_timer_wakeup(void)
{
    // 配置定时器
    bflb_timer_config cfg = {
        .period = 1000,
        .reload = 1000
    };
    bflb_device_t *timer = bflb_timer_init(0, &cfg);
    bflb_timer_start(timer);

    // 配置唤醒
    bflb_pm_wakeup_source_set(PM_WAKEUP_SOURCE_TIMER, timer);
}
```

## 深度睡眠模式

### 特性

- 超低功耗
- 部分RAM保持
- Flash XIP关闭
- 支持GPIO唤醒

### 进入深度睡眠

```c
#include "bflb_pm.h"

void enter_deep_sleep(void)
{
    // 保存关键数据
    save_critical_data();

    // 配置唤醒源
    bflb_pm_wakeup_source_set(PM_WAKEUP_SOURCE_GPIO, GPIO_PIN_0);

    // 进入深度睡眠
    bflb_pm_deep_sleep();

    // 唤醒后恢复
    printf("Woke up from deep sleep\n");
}
```

### RAM保持配置

```c
// 配置保持的RAM区域
void config_ram_retention(void)
{
    // 保持特定SRAM块
    bflb_pm_ram_retention_set(SRAM_BLOCK_0, true);

    // 或保持整个SRAM
    bflb_pm_ram_retention_set_all(true);
}
```

## 休眠模式

### 特性

- 极低功耗
- 大部分SRAM关闭
- 需要更长时间唤醒

### 进入休眠

```c
#include "bflb_pm.h"

void enter_hibernate(void)
{
    // 保存状态到Flash
    save_state_to_flash();

    // 配置唤醒源（RTC）
    bflb_pm_wakeup_source_set(PM_WAKEUP_SOURCE_RTC, &alarm);

    // 进入休眠
    bflb_pm_hibernate();

    // 唤醒后从Flash恢复
    restore_state_from_flash();
}
```

### RTC唤醒

```c
#include "bflb_rtc.h"

void setup_rtc_wakeup(void)
{
    struct bflb_rtc_alarm alarm = {
        .hour = 12,
        .minute = 30
    };

    // 设置闹钟
    bflb_rtc_set_alarm(&alarm, rtc_alarm_callback);

    // 配置唤醒
    bflb_pm_wakeup_source_set(PM_WAKEUP_SOURCE_RTC, &alarm);
}
```

## 低功耗优化

### 1. 动态频率调整

```c
#include "bflb_clk.h"

void adjust_clock(void)
{
    // 降频以降低功耗
    bflb_clk_set_sysclk(CLK_160MHz);  // 从320MHz降到160MHz

    // 需要高性能时提升频率
    bflb_clk_set_sysclk(CLK_320MHz);
}
```

### 2. 关闭未使用的外设

```c
void power_down_unused_peripherals(void)
{
    // 关闭UART
    bflb_uart_deinit(uart);

    // 关闭SPI
    bflb_spi_deinit(spi);

    // 关闭ADC
    bflb_adc_deinit(adc);
}
```

### 3. 使用低功耗模式

```c
void enter_low_power_mode(void)
{
    // 任务空闲时进入睡眠
    while(1) {
        if (no_pending_work()) {
            bflb_pm_shallow_sleep(100);  // 睡眠100ms
        }

        // 处理任务
        process_tasks();
    }
}
```

### 4. WiFi低功耗

```c
#include "wifi_mgmr.h"

void wifi_low_power(void)
{
    // 关闭WiFi
    wifi_mgmr_disable();

    // 或使用WiFi低功耗模式
    wifi_mgmr_set_power_save(true);

    // 需要时重新启用
    wifi_mgmr_enable();
}
```

### 5. BLE低功耗

```c
#include "btble_api.h"

void ble_low_power(void)
{
    // 配置BLE低功耗参数
    struct bt_le_conn_param conn_param = {
        .interval_min = 0x200,  // 较大间隔
        .interval_max = 0x200,
        .latency = 10,
        .timeout = 0x200
    };

    // 请求连接参数更新
    bt_conn_le_param_update(conn, &conn_param);
}
```

## 唤醒机制

### GPIO唤醒

```c
void gpio_wakeup_setup(void)
{
    // 配置GPIO为唤醒源
    bflb_gpio_init(0, GPIO_PIN_0, GPIO_INPUT | GPIO_WAKEUP_EDGE_RISING);

    // 设置唤醒回调
    bflb_pm_gpio_wakeup_attach(0, GPIO_PIN_0, gpio_wakeup_callback, NULL);

    // 使能唤醒
    bflb_pm_gpio_wakeup_enable(0, GPIO_PIN_0, true);
}
```

### 定时器唤醒

```c
void timer_wakeup_setup(void)
{
    struct bflb_timer_config cfg = {
        .period = 5000,
        .reload = 5000
    };

    bflb_device_t *timer = bflb_timer_init(0, &cfg);
    bflb_timer_start(timer);

    // 设置唤醒源
    bflb_pm_timer_wakeup_attach(timer, timer_callback);
}
```

### RTC唤醒

```c
void rtc_wakeup_setup(void)
{
    struct bflb_rtc_alarm alarm = {
        .hour = 6,
        .minute = 0,
        .second = 0
    };

    // 设置RTC闹钟
    bflb_rtc_set_alarm(&alarm, rtc_alarm_callback);

    // 设置唤醒源
    bflb_pm_rtc_wakeup_attach(&alarm);
}
```

## 电源管理配置

### Kconfig配置

```kconfig
# 电源管理选项
config CONFIG_PM
    bool "Enable power management"
    default y

config CONFIG_PM_DEEP_SLEEP
    bool "Enable deep sleep"
    depends on CONFIG_PM
    default y

config CONFIG_PM_HIBERNATE
    bool "Enable hibernate"
    depends on CONFIG_PM
    default n

# 低功耗选项
config CONFIG_WIFI_LOW_POWER
    bool "Enable WiFi low power"
    default n

config CONFIG_BLE_LOW_POWER
    bool "Enable BLE low power"
    default y
```

### 动态配置

```c
void dynamic_pm_config(void)
{
    // 运行时配置
    bflb_pm_config_t pm_cfg = {
        .enable_shallow_sleep = true,
        .enable_deep_sleep = true,
        .auto_sleep_threshold = 100,  // 空闲100ms后自动睡眠
    };

    bflb_pm_config(&pm_cfg);
}
```

## 状态保存和恢复

### 保存状态

```c
void save_state(void)
{
    struct system_state state = {
        .counter = counter,
        .config = config,
        // ... 其他状态
    };

    // 保存到Flash
    bflb_flash_write(STATE_ADDR, &state, sizeof(state));
}
```

### 恢复状态

```c
void restore_state(void)
{
    struct system_state state;

    // 从Flash读取
    bflb_flash_read(STATE_ADDR, &state, sizeof(state));

    // 恢复状态
    counter = state.counter;
    config = state.config;
}
```

## 低功耗测量

### 测量功耗

```c
void measure_power(void)
{
    // 使用ADC测量电压
    uint16_t vbat_adc = bflb_adc_read(adc, ADC_CHANNEL_VBAT);
    float vbat = bflb_adc_to_voltage(vbat_adc);

    printf("Battery: %.2fV\n", vbat);
}
```

### 功耗优化验证

```c
void power_test(void)
{
    // 记录当前时间
    uint32_t start = bflb_mtimer_get_time_ms();

    // 进入睡眠
    bflb_pm_deep_sleep();

    // 唤醒后记录时间
    uint32_t elapsed = bflb_mtimer_get_time_ms() - start;
    printf("Slept for %d ms\n", elapsed);
}
```

## 多核电源管理

### BL618dg双核管理

```c
void bl618dg_pm(void)
{
    // LP核心处理低功耗任务
    if (get_core_id() == CORE_LP) {
        bflb_pm_deep_sleep();
    }

    // ap核心正常运行
}
```

## 低功耗最佳实践

### 1. 合理选择睡眠模式

```c
// 短时间空闲：浅睡眠
if (idle_time < 100) {
    bflb_pm_shallow_sleep(idle_time);
}
// 长时间空闲：深度睡眠
else {
    bflb_pm_deep_sleep();
}
```

### 2. 及时关闭外设

```c
// 使用完外设后立即关闭
uart_send(uart, data, len);
bflb_uart_deinit(uart);  // 关闭UART
```

### 3. 使用动态频率

```c
// 低负载时降频
if (load < 30%) {
    bflb_clk_set_sysclk(CLK_160MHz);
}
// 高负载时升频
else if (load > 80%) {
    bflb_clk_set_sysclk(CLK_320MHz);
}
```

### 4. WiFi/BLE低功耗

```c
// 不需要时关闭WiFi
if (!wifi_needed) {
    wifi_mgmr_disable();
}

// 使用BLE低功耗连接
bt_conn_le_param_update(conn, &low_power_params);
```

### 5. 状态保存和恢复

```c
// 进入低功耗前保存状态
save_state();
bflb_pm_deep_sleep();

// 唤醒后恢复状态
restore_state();
```

## 低功耗检查清单

- [ ] 选择合适的睡眠模式
- [ ] 关闭未使用的外设
- [ ] 使用动态频率调整
- [ ] 配置WiFi/BLE低功耗
- [ ] 保存和恢复状态
- [ ] 设置合适的唤醒源
- [ ] 测量和优化功耗
- [ ] 使用电池监控
- [ ] 优化任务调度
- [ ] 避免频繁唤醒

## 常见问题

### 1. 唤醒后程序不执行

**原因：** RAM未保持

**解决：**
```c
// 配置RAM保持
bflb_pm_ram_retention_set(SRAM_BLOCK_0, true);
```

### 2. WiFi连接后无法睡眠

**原因：** WiFi阻止睡眠

**解决：**
```c
// 断开WiFi连接
wifi_mgmr_disconnect();

// 或使用WiFi低功耗模式
wifi_mgmr_set_power_save(true);
```

### 3. 功耗不降低

**原因：** 未关闭外设

**解决：**
```c
// 关闭所有未使用的外设
bflb_uart_deinit(uart);
bflb_spi_deinit(spi);
// ...
```

## 总结

Bouffalo SDK提供完整的电源管理功能，支持多种低功耗模式和唤醒机制。合理选择睡眠模式，关闭未使用的外设，使用动态频率，可以显著降低功耗。遵循低功耗最佳实践，延长电池寿命。
