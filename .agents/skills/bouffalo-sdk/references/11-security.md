# 安全、加密与 TLS

安全能力取决于芯片、CPU 核、配置、镜像格式和部署流程。先核对目标芯片的头文件、示例、构建配置和启动链；不要把某一个开发板示例当作全部芯片的支持矩阵。

## 硬件安全引擎

`drivers/lhal/include/bflb_sec_*.h` 是当前 LHAL 接口，`examples/peripherals/sec_eng/` 有 AES、SHA、TRNG、ECDSA 等示例。示例对 BL702L、BL602、BL702 的部分算法和密钥长度有编译条件；PKA 库选择还受 `CPU_ID` 和 `CPU_MODEL` 影响，见 `drivers/lhal/CMakeLists.txt`。确认设备存在，并按目标工程要求获得 group 访问权限；不要假定所有核心都能直接访问。

| 功能 | 当前接口和限制 |
| --- | --- |
| AES | 先获取 `BFLB_NAME_SEC_AES` 设备，调用 `bflb_aes_init`、`bflb_aes_set_mode`、`bflb_aes_setkey`，再调用 `bflb_aes_encrypt` / `bflb_aes_decrypt`；数据长度为 16 字节整数倍，检查返回值。硬件密钥和 IV 为 NULL 的含义见头文件；使用前需明确密钥源和 IV 状态。模式常量有 ECB、CBC、CTR、XTS；不要把 mbedTLS GCM 等同于 LHAL AES 的模式常量。 |
| SHA | `bflb_sha_init(dev, SHA_MODE_SHA256)` 后依次调用 `bflb_sha256_start`、`bflb_sha256_update`、`bflb_sha256_finish`，上下文类型是 `struct bflb_sha256_ctx_s`。检查 update 返回值；SHA 算法可用性随目标不同。 |
| TRNG | 获取 `BFLB_NAME_SEC_TRNG` 设备后，`bflb_trng_read(dev, data)` 要求 32 字节缓冲区；也有 `bflb_trng_readlen(data, len)`。检查返回值，不要将 API 存在当作安全认证证明。 |
| 公钥运算 | `bflb_sec_pka.h` 暴露 PKA 底层算术操作；ECDSA 接口见 `bflb_sec_ecdsa.h` 和 `sec_eng_ecdsa` 示例。不要调用不存在的 `bflb_sec_pka_rsa_sign`。 |

安全协议应选择适合需求的认证加密和密钥管理方案；裸 ECB 测试仅用于核对算法，不应直接当作完整数据保护方案。实际操作参考 `sec_eng_aes_sw_key/main.c` 和 `sec_eng_sha/main.c`；涉及共享硬件引擎时还需遵循目标平台的访问和并发约束。

## mbedTLS 和 TLS 客户端

`components/crypto/Kconfig` 的符号是 `MBEDTLS`、`MBEDTLS_V2`、`MBEDTLS_CFG_FILE`；defconfig 使用 `CONFIG_` 前缀。默认选 v3 源码，选择 `CONFIG_MBEDTLS_V2=y` 时构建 v2；`components/crypto/mbedtls/CMakeLists.txt` 是实际选源依据。AES、SHA、ECC 等硬件加速分别由 `CONFIG_MBEDTLS_*_USE_HW` 控制，还要检查目标芯片和其他依赖。GCM 有额外配置分支，不宜从 AES 硬件加速开关推导所有模式的实现。

TLS 客户端至少要：

1. 初始化 SSL 配置、上下文、CA、网络及随机数状态；根据目标版本和配置初始化所需的 PSA 等组件。
2. 加载可信 CA，调用 `mbedtls_ssl_config_defaults`，设 `MBEDTLS_SSL_VERIFY_REQUIRED`、CA 链和可靠的 RNG；检查各个返回值。
3. 建立传输连接，`mbedtls_ssl_setup` 后以 `mbedtls_ssl_set_hostname` 设置**实际服务器证书对应的名称**，并绑定 BIO。信任链和主机名匹配需要分别成立。
4. 握手时处理 `MBEDTLS_ERR_SSL_WANT_READ/WRITE`，检查握手结果和 `mbedtls_ssl_get_verify_result`；读写处理部分完成及错误，失败时关闭并释放各对象。

`examples/wifi/sta/wifi_mqtt/wifi_mqtts_sub.c` 展示了配置次序和 CA 验证，但固定 hostname 为 `server.local`，其随机数包装忽略底层返回值、TCP connect 失败后仍返回描述符，而且 hostname 返回值未检查；使用前须修正并验证。TLS 是否真正拒绝不可信证书、错误主机名、过期证书，必须通过目标设备和服务器做负面测试；编译成功不能证明连接安全。

## 启动签名、Flash 与 eFuse

`examples/boot2_isp/bflb_sp_boot_parser.c` 包含公钥哈希核对及 ECDSA 验签逻辑；`bsp/board/bl616dk/builtin_imgs/README.rst` 给出特定板的镜像加密和 eFuse 公钥哈希示例，但使用旧配置文件格式。具体信任链、签名算法、镜像及密钥流程应核对所用芯片和工具，不能推断所有目标支持 RSA 启动签名。

Flash 控制器提供 AES 配置及启停接口；`examples/peripherals/flash/flash_secure_read_write_hw_key_ctr_128/` 则是应用显式 AES-CTR 加解密数据的示例，并说明应用镜像本身未加密。不要假设任意 `bflb_flash_read()` 都会透明解密。

`bflb_efuse.h` 声明了 secure boot 状态读取、AES 使能与密钥读写锁等具体接口；是否可读取取决于实际保护配置。eFuse 和抗回滚版本涉及不可逆状态，不要把未经目标核实的烧写命令作为普通示例执行。`Kconfig` 中 `ANTI_ROLLBACK`、`APP_ANTI_ROLLBACK_VER` 配合目标启动链使用；示例文档说明版本范围 0–128，具体 eFuse 启用和版本单向递增流程见 `docs/zh/samples/peripherals/anti_rollback/anti_rollback.rst` 与目标板源码。

## HTTPS FOTA 边界

当前 API 定义在 `components/fota/https/client/https_fota.h`：`https_fota_init(url, &config)` 返回 handle，`https_fota_start(handle)` 启动，也提供 `https_fota(url, &config)` 便捷接口。`struct https_fota_config` 使用 `ca_pem`、`ca_len` 和可选客户端证书/私钥等字段，具体资源生命周期和结束流程以头文件及实现为准。HTTPS 自身不证明服务端身份已验证，也不证明镜像签名或抗回滚已启用；分别检查 CA、名称、镜像验证和启动链。FOTA 完整生命周期、分区和回滚边界见 [29-ota-fota.md](29-ota-fota.md)。
