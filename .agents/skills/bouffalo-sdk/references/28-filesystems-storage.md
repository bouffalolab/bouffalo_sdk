# 文件系统与存储服务

## 组件入口

- FatFs、LittleFS 和 ROMFS：`components/fs/`；示例 `examples/fatfs/`、`examples/littlefs/`。
- SPI NAND 与 Dhara FTL：`components/spi_nand/`；FatFs 适配见 `components/fs/fatfs/port/fatfs_spi_nand.c`，示例及板级连接见 `examples/peripherals/spi/spi_nand/README.md`。运行前检查格式化或测试文件覆盖的影响。
- 分区和 MTD：见 [10-memory-management.md](10-memory-management.md)。

## LittleFS 和 KV

`CONFIG_LITTLEFS` 启用 LittleFS；SDK 的 Flash port 通过 MTD 查找分区，通常使用 `PSM`。检查当前分区表、介质几何及实际挂载对象；`lfs_config` 除 read/prog/erase/sync 回调外还需正确设置块和缓存参数。文件写入返回字节数或负错误，数据提交涉及 `lfs_file_sync` / `lfs_file_close`，两者也要检查结果；不要把掉电保护描述为任何时刻都保证最新写入。

当前 `lfs_xip_init()` 对**任意挂载失败**都会尝试 `lfs_format()`，libc LittleFS 的相应挂载路径也有类似处理；挂载失败可能是 I/O 等故障，不能等同于首次启动。对已有设备数据，先确认介质和恢复策略，不能盲目运行初始化或示例。

`lfs_kv` 默认使用 `PSM` 和 `/_ef4_kvs_`，源码注明命名空间不能随意更改，否则影响旧数据访问。初始化前配置 MTD 与分区；写入检查完整长度、truncate、close，并对满盘映射错误。读取接口返回 0 的情形可能包括 key 不存在、失败或空值，应结合输出长度和应用语义处理；不要假定 0 必然代表某一种原因。`components/fs/littlefs/tests/run_host_tests.py` 使用模拟 Flash 和故障注入，可验证部分错误和重挂载，不能证明真实断电行为或覆盖 Flash port 自动格式化。

## FatFs 和 ROMFS

`CONFIG_FATFS` 及 SD、USB、SPI NAND 后端按工程配置选择。默认 FatFs 卷名中 `0:` 是 RAM 卷，SD 示例先初始化板级 SDH、注册驱动，再挂载 `/sd`；工程可通过 `fatfs_conf_user.h` 覆盖默认卷、长文件名和线程安全等设置。检查每次调用的 `FRESULT`、读写的实际字节数以及 `f_close` 结果。`examples/fatfs` 在 `FR_NO_FILESYSTEM` 时执行 `f_mkfs`，还以 `FA_CREATE_ALWAYS` 覆盖测试文件；SPI NAND 示例也会覆盖其测试文件。运行前确认数据影响。

ROMFS 的接口为 `romfs_mount(uint32_t addr)`，实现把 Flash 地址转为 XIP 并校验 magic；挂载点和读取 API 见 `components/fs/romfs/bflb_romfs.h`。镜像生成与分区放置要结合工程的 `CONFIG_POST_BUILDS_GENERATE_ROMFS`、注册分区和实际烧录布局核对，不要把普通路径名当成该 API 的地址参数。

## MTD 与 libc 适配

MTD 提供 `bflb_mtd_init/open/info/read/write/erase/close`；默认打开 active 分区，`BFLB_MTD_OPEN_FLAG_BACKUP` 选择 inactive 分区。当前 `bflb_mtd.c` 的 read/write/erase 包装忽略底层 Flash 返回值并直接返回 0，也没有在这些函数中检查访问是否越过分区；不能把返回 0 当作底层读写或擦除成功的完整证据。调用方须依据实际分区大小和底层行为做边界与结果核对。

libc 文件后端在构建时按 `CONFIG_NEWLIB_LITTLEFS` / `CONFIG_NEWLIB_FATFS` 选择，否则使用 nosys；FatFs、ROMFS 的直接 API 不应假设经过同一个通用 VFS。分区表、Flash 地址和擦除约束另见 [10-memory-management.md](10-memory-management.md)。

## 示例和验证边界

`examples/fatfs`、`examples/easyflash` 是可核对的 API 入口；使用 `examples/littlefs/` 前核对其配置与当前 Kconfig 是否匹配。构建、主机模拟、真实 Flash 读写、重启恢复和断电故障注入应分别记录，主机测试与编译均不能证明真实介质数据安全。
