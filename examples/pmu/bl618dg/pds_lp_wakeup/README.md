# pds_lp_wakeup 注意事项

本示例演示 BL618DG 的 LP 核唤醒 AP 核的流程（AP 核进入 PDS 低功耗，LP 核通过软件触发将 AP 核唤醒）。在使用 / 移植本示例时，需要特别留意以下时钟相关的限制，否则会出现 LP 核失活、串口乱码等问题。

## 1. RC32M 不能关闭

RC32M 是 **LP 核的工作时钟源**。

- LP 核运行的全部代码（包括 `lp/main.c` 中的 shell、`Triger_PDS_Wakeup` 命令等）都依赖 RC32M 提供时钟。
- 在 AP 核或 LP 核的任何低功耗 / 时钟配置流程里，**禁止关闭 RC32M**；一旦关闭，LP 核立即停摆，后续无法再唤醒 AP 核。
- AP 核进入 PDS 时，PDS 配置中必须保持 RC32M 处于使能状态。


## 2. UART3 的时钟与 UART0/1/2 共用，进入 PDS 时建议 XCLK 选 RC32M

BL618DG 的 UART 时钟挂在 `HBN_GLB.UART_CLK_SEL / UART_CLK_SEL2` 上，**UART3（LP 域）与 UART0/1/2（AP 域）共用同一个时钟选择**，不能单独配置。

可选的时钟源：

| `UART_CLK_SEL2` | `UART_CLK_SEL` | 时钟源       |
| --------------- | -------------- | ------------ |
| 0               | 0              | MCU_BCLK     |
| 0               | 1              | WIFIPLL_160M |
| 1               | x              | XCLK         |

进入 PDS 时硬件行为：

- AP 核进入 PDS 后，**XCLK 会被自动切换到 RC32M**（XTAL 40M 被关闭以省电）。
- 如果此时 UART 时钟源选的是 XCLK：
  - 切到 RC32M 之前 XCLK = XTAL 40M（外部晶体），波特率分频按 40M 计算。
  - 切到 RC32M 之后 XCLK = RC32M（内部 RC，频率不准），分频系数不变，**UART3 在 LP 核继续输出的字符就会乱码**。
- 如果 UART 时钟源选 WIFIPLL_160M：PDS 期间 WIFIPLL 被关闭，UART3 完全无时钟。
- 如果 UART 时钟源选 MCU_BCLK：PDS 期间 MCU_BCLK 也会受影响。

### 推荐做法

在 `bsp/board/bl618dgdk/board.c` 的板级初始化里，初始化 XCLK 时直接选 **RC32M** 作为 XCLK，避免 PDS 前后 XCLK 频率发生变化：

```c
HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_RC32M);
GLB_Set_UART_CLK(ENABLE, HBN_UART_CLK_XCLK, 0);
```

这样：

- XCLK 在进入 / 退出 PDS 前后始终是 RC32M，频率不发生跳变。
- UART3 在 LP 核运行期间和 LP 唤醒 AP 之后都不会乱码。
- 对波特率精度有要求的串口建议把波特率适当降低，并实测确认 RC32M 偏差范围。

## 3. 上手验证

启动后串口会打印当前 UART 时钟源和频率（来自 `lp/main.c` 中的 `dump_uart_clock_info()`）：

```
[I][MAIN] pds_lp_wakeup test start
[I][MAIN] UART clock source: XCLK, frequency: 32000000 Hz
```

在 shell 中输入 `Triger_PDS_Wakeup` 即可触发 LP 核唤醒 AP 核：

```
$ Triger_PDS_Wakeup
[I][MAIN] LP waking up AP from PDS...
[I][MAIN] LP wakeup AP done
```

如果观察到 UART3 输出乱码，优先检查 XCLK 是否选了 RC32M、以及 RC32M 是否被意外关闭。
