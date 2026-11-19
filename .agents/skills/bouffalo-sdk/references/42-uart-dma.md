# UART DMA 入口

路径相对 SDK 根目录：

- 示例：`examples/peripherals/uart/uart_dma/`；板级引脚、UART 和 DMA 请求由目标板配置决定。
- 接口与实现：`drivers/lhal/include/bflb_uart.h`、`drivers/lhal/include/bflb_dma.h`、`drivers/lhal/src/bflb_dma.c`。
- 通用设备表及外设选择见 [03-lhal-drivers.md](03-lhal-drivers.md) 和 [08-peripherals.md](08-peripherals.md)。

示例分别配置 UART TX、RX DMA，具体顺序和参数看示例源码。使用前核对 UART 与 console 是否冲突、DMA 请求和方向、描述符池容量、缓冲区生命周期及目标缓存规则；当前非 ROM 的 LLI 实现先写描述符才检查池容量，不能仅依靠返回错误避免容量不足。DMA 启动和完成是不同事件。
