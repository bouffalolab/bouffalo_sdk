# 内存管理

## 内存与地址

Flash、内部 SRAM 和可选 PSRAM 是不同的存储区域，并非固定的逐级分配层次。实际容量和可用地址取决于芯片、封装、板载物料、链接脚本与配置。`CONFIG_PSRAM_LENGTH` 是链接器使用的区域长度，不等于已实测物理容量；不要直接沿用其他板子的容量或地址。

排查时分别查看 `bsp/board/<BOARD>/` 的板级代码和链接脚本、工程生成的 `autoconfig.*`、ELF 和 map 文件；Flash 物理偏移与 XIP 映射地址不能混用。

## 内存分配与堆

`components/mm/mm.h` 是当前 SDK 的内存管理入口，板级初始化通过 `mm_register_heap` 注册 OCRAM、WRAM、PSRAM 等区域及 allocator。可用堆和次序取决于板级代码、链接区域和配置；不要假定所有板都有 PSRAM。

分配 API 及使用条件见 `components/mm/mm.h` 和目标板的堆初始化。

`MM_FLAG_HEAP_ANY` 按自动分配顺序表遍历其中的活动堆，直到分配成功；默认按 heap ID 排序，板级代码可用 `mm_heap_set_any_alloc_order` 改序或排除堆。指定 `MM_FLAG_HEAP_OCRAM_0`、`MM_FLAG_HEAP_PSRAM_0` 等只尝试指定堆。`krealloc` 对非空指针按原堆处理。`MM_FLAG_PROP_ZERO` 在当前 TLSF 和 HEAP5 allocator 中实现；不能仅凭头文件声明就假定其他属性标志均已实现。

`CONFIG_MEM_TLSF_EN` 和 `CONFIG_MEM_HEAP5_EN` 控制 allocator 源文件编译，兼容项 `CONFIG_TLSF` 不等同于源码选择开关。当前 `mm.c` 的 allocator 注册使用另一组 `CONFIG_MM_TLSF_EN` / `CONFIG_MM_ENABLE_FREERTOS_HEAP5` 宏，默认值见 `mm_config.h`；如调整配置，须检查最终编译定义和实际注册结果，不能假设两个命名自动关联。

当前 FreeRTOS CMake 选择 `portable/MemMang/heap_3.c`，SDK 的 `pvPortMalloc` 调用 `kmalloc(size, MM_FLAG_HEAP_OCRAM_0)`，`vPortFree` 调 `kfree`；实际 allocator 由该堆注册配置决定，FreeRTOS heap 与 TLSF 并非互斥选项。不要仅因仓库含其他 heap 文件就假设当前构建使用它们，也不要假设 `xPortGetFreeHeapSize()` 在该端口有实现。

Picolibc 的 `malloc/calloc/realloc/memalign/free` 经兼容层进入 SDK 内存管理；任务局部 TLS 从任务栈划分，具体启动和 libc 限制见 [09-components.md](09-components.md) 和 `docs/standard_riscv_toolchain.md`。外部 NuttX 集成应使用其自身的分配 API 和配置，不与本 SDK 当前 `components/mm` 混为一谈。

## Flash 管理

### 物理地址、读写与擦除

`drivers/lhal/include/bflb_flash.h` 的 `bflb_flash_read/write/erase` 使用 Flash **物理地址**和长度，成功返回 0，失败返回负错误码。先按当前板的分区表选择明确保留的数据区域，确保整个操作范围及受影响的擦除单元不覆盖固件、分区表或其他数据；检查每次调用结果并按需求读回验证。`examples/peripherals/flash/flash_read_write/` 使用的 `0x10000` 在 BL616 DK 默认布局中是 FW 起始地址，不能当作通用安全测试区。

非 ROM 软件实现会依据 Flash 配置向下对齐 sector 并擦除整块，某些条件下使用更大的块；写入由底层按设备页大小拆分。不要假设任意地址和长度只影响请求的字节，也不要假设所有芯片固定使用 4 KiB sector 或写入必须按 256 字节对齐。ROM 路径和实际器件应分别核实。

XIP 映射随芯片和型号变化，常量见 `bflb_flash.h`；如 BL616 为 `0xA0000000`，BL616CL 为 `0x80000000`，BL618DG 还区分 A0 与其他型号。操作函数的物理偏移不是这些映射地址；转换还应核对 `bflb_flash_get_image_offset()` 和实际映射，参考加密示例。容量检查也不能替代分区边界检查。

默认 Flash 操作保存/恢复中断。启用 `CONFIG_DISABLE_FLASH_OP_IRQ_SAVE_RESTORE` 时，需按头文件要求实现资源锁，并确保 XIP 不可访问期间所需代码、数据及中断处理可以安全访问；涉及 XIP 的 DMA 需遵守同一资源锁约定。安全寄存器和 OTP 是独立接口，尤其应先确认设备支持与锁定语义。

### 分区表与打包

当前表采用 TOML 的 `[pt_table]` 和 `[[pt_entry]]`，例如 `bsp/board/bl616dk/config/partition_cfg_4M.toml`。它的表地址为 `0xE000`、`0xF000`，FW 的 `address0` 为 `0x10000`；这只是该配置示例，不能推广为所有自定义布局。该表的 FW 第二区域与 `mfg` 区域重合，不应假定每个条目都独占空间或将它直接视为通用 OTA 策略。

`bsp/Kconfig` 按 CHIP 选择默认表，`CONFIG_OVERRIDE_PTFILE` 可指定工程目录下的替代表；在实际生成的配置和 `cmake/bflb_flash.cmake` 中确认最终选用文件。standard 打包包含 Boot2、分区表及 RF 等数据；direct boot 从地址 0 启动。完整工具路径即使 direct boot 也会读取所选 TOML 生成配置；`CONFIG_SKIP_COMBINE` 可跳过合包。配置生成不证明镜像适合分区或可运行。

`CONFIG_PARTITION` 控制 `components/utils/partition/` 的运行时 API 编译，与打包模式是不同开关。`examples/partition/` 注册 Flash 擦写读回调，用 `pt_table_get_active_partition_need_lock` 获取有效表，再按名称或 ID 查询。运行时表选择涉及 magic、CRC 和 age；更新条目 API 会擦写分区表，不能当作只读查询。头文件中的 `BFLB_PT_TABLE0_ADDRESS/1_ADDRESS` 为该实现的表地址，使用自定义布局时应检查工具与运行时实现是否一致。

只读查询示例见 `examples/partition/`，使用前确认已初始化 Flash 并启用相应分区配置。

查询结果不能单独证明分区可擦写或未被其他用途占用；检查返回值、索引和范围。

### 双分区表与镜像槽

分区表副本和各条目的镜像槽是不同概念；表选择、Flash 读写回调及 IAP 槽切换见 `components/utils/partition/` 和 `examples/partition/`。更新表会擦写 Flash，确认目标分区、读写错误、并发和实际镜像后再操作；完整升级流程见 [29-ota-fota.md](29-ota-fota.md)。

### Flash 加密

支持的密钥类型和访问行为以 `bflb_flash.h` 与目标芯片为准。`examples/peripherals/flash/flash_secure_read_write/` 演示先用 AES 加密数据后写入，通过配置的 Flash AES 和 XIP 访问读取明文；普通 `bflb_flash_read` 读取的数据还需要相应解密。该示例同样擦写 `0x10000`，运行前必须核对分区及返回值。不能把所有写入和所有读取路径笼统称为透明加密；密钥、启动链及安全 OTA 需按具体方案审查。

## PSRAM 管理

`bsp/Kconfig` 中 `CONFIG_PSRAM` 默认关闭，`CONFIG_PSRAM_LENGTH` 是以字节表示的十六进制链接区域长度；另有 `CONFIG_PSRAM_XIP`、`CONFIG_FLASH_XIP_PSRAM`、`CONFIG_PSRAM_COPY_CODE`，其意义和组合应以当前构建为准。不同板可提供不同默认值。不要把运行时识别到的物料容量和配置的链接区域大小视为自动相同。

BL616/BL616CL DK 的 `ram_heap_init()` 在相关配置下调用 `board_psram_x8_init()`；BL618DG DK 的相关调用受核心条件限制。BL702DK 也有片内或外接 PSRAM 分支，BL702LDK 分支的支持范围不同。芯片识别、时钟、电压、DQS 校准及初始化失败处理应查看对应板的 `board.c` 和 `board_flash_psram.c`，不可用无参数 `bflb_psram_init()` 或假想的 map/unmap 接口替代板级初始化。

BL616、BL616CL、BL618DG 的普通 PSRAM 模式由 `bsp/common/psram_heap_init.c` 按链接符号注册 TLSF 堆；BL702DK 在板级代码自行注册。上述三芯片的 XIP 模式的 `psram_xip_heap_init.c` 基于 multi-bin 计算并检查剩余堆空间。启用对应堆后，可使用 `components/mm/mm.h` 提供的 `kmalloc(size, MM_FLAG_HEAP_PSRAM_0)` 和 `kfree()`，并检查分配结果。具体初始化顺序和内存分配策略取决于板和配置。

`examples/peripherals/psram/` 包含配置和读写测试；该测试直接覆盖 PSRAM 起始区域，不能直接作为与现有堆和数据共存的无损应用代码。BL618DG 多核 PSRAM XIP 还涉及 AP/NP 链接区域、multi-bin 装载和描述符校验；其他芯片不要照搬其地址和流程。

## 对齐、统计和调试

`kmemalign(size, align)` 和 `MM_FLAG_ALIGN_*` 的实现受 allocator 约束：当前 TLSF 路径显式处理到 64 字节，而 128/256 字节标志在该实现中回落到指针对齐；`kmemalign` 对超过 256 的请求也不会提供更大的对齐保证。对齐请求应使用 2 的幂，最终保证取决于被选中的 allocator；需要更大对齐时，检查当前实现并验证返回地址，不要仅凭标志名假定满足要求。DMA 还需依据目标芯片、内存区域和 cache 策略处理缓存一致性。

统计 API 见 `components/mm/mm.h` 和当前 allocator 实现。

`kfree_size(0)` 汇总所有活动堆的剩余字节，`kmax_free_size(0)` 取各堆最大连续块中的最大值，不会跨堆合并。`kmalloc_size(ptr)` 查询分配块可用大小；最小剩余量追踪受 `CONFIG_MM_ENABLE_MIN_FREE_TRACKING` 控制。Shell `mm_info` 的可用性取决于构建配置。

FreeRTOS 的栈溢出检测由工程 `FreeRTOSConfig.h` 中 `configCHECK_FOR_STACK_OVERFLOW` 控制，端口提供弱 `vApplicationStackOverflowHook`。任务栈高水位和 `vTaskList` 需要对应配置，返回单位和缓冲大小见 `task.h`。排查内存损坏应结合堆和栈检查；单独在指针容器里放 magic 字段不能检查它指向的缓冲区边界。

低功耗前后的内存保留、Flash/PSRAM 状态和恢复顺序取决于芯片与睡眠模式，查对应 `examples/pmu/`；不要调用未验证的通用 `mm_clean_cache`、`mm_init` 或 `pmu_enter_sleep` 伪接口。

## 内存映射与检查

映射以目标链接脚本和实际生成的 ELF 为准。使用 `riscv64-unknown-elf-objdump -h <elf>` 或 `riscv64-unknown-elf-readelf -S <elf>` 查看节，结合构建输出的 map 文件核对地址、大小和 PSRAM 段；不要使用固定示意地址替代目标芯片映射。

## 使用建议

明确对象生命周期、堆来源、失败处理和并发约束；按需复用缓冲区并监控剩余量及最大连续块。静态数据所在段由链接脚本决定，修改 section 或启用 XIP 后核对 map 和目标启动配置。不要根据源码存在就认定某分配器或内存区域已在当前工程启用。
