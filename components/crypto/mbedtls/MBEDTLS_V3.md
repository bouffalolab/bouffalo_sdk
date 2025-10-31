# Mbed TLS

Mbed TLS 是一个 C 代码库，用于实现加密基元、X.509 证书操作以及 SSL/TLS 和 DTLS 协议。该库代码占用空间小，适合嵌入式系统使用。

Mbed TLS 通过提供以下内容支持 TLS 1.2、TLS 1.3 和 DTLS 1.2 通信：

- TCP/IP 通信功能：监听、连接、接收、读/写。
- SSL/TLS 通信功能：初始化、握手、读/写。
- X.509 功能：CRT、CRL 和密钥处理
- 随机数生成
- 哈希
- 加密/解密

## 版本说明

Bouffalo SDK 提供了两个 Mbed TLS 版本：`mbedtls` 目录下是 v2.28 版本，而 `mbedtls_v3` 目录下则是 v3.6.3 版本。其中，位于 `mbedtls_v3` 目录下的版本将获得长期维护与更新，支持 Kconfig 进行配置。

### v3.0版本重大更新（概述）

#### 增加私有结构体字段数量

- 不再支持直接访问公共头文件中声明的结构体（ `struct` 类型）字段。
- 当前版本下，访问公共头文件中声明的结构体字段需要使用特定的访问函数 (getter/setter)。另外，也可以用 `MBEDTLS_PRIVATE` 宏暂时代替，但不建议使用此种方法。
- 更多详细信息，请参考 [官方指南](https://github.com/Mbed-TLS/mbedtls/blob/development/docs/3.0-migration-guide.md#most-structure-fields-are-now-private)。

#### SSL

- 不再支持 TLS 1.0、TLS 1.1 和 DTLS 1.0
- 不再支持 SSL 3.0

#### 移除密码模块中的废弃函数

- 更新了与 MD、SHA、RIPEMD、RNG、HMAC 模块相关的函数 `mbedtls_*_ret()` 的返回值，并将其重新命名，以取代未附加 `_ret` 的相应函数。
- 更多详细信息，请参考 [官方指南](https://github.com/Mbed-TLS/mbedtls/blob/development/docs/3.0-migration-guide.md#deprecated-functions-were-removed-from-hashing-modules)。

更多有关 Mbed TLS 从 v2.x 版本迁移到 v3.0 或更高版本的详细信息，请参考 [官方指南](https://github.com/Mbed-TLS/mbedtls/blob/development/docs/3.0-migration-guide.md)。

## 配置说明

### CONFIG_MBEDTLS_HAVE_ASM

**功能**：启用汇编优化。
​**​详细说明​**​：

- 允许使用CPU专用指令加速，如AES-NI/ARMv8加密扩展。
- 暂未实现该功能。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration`

------

### CONFIG_MBEDTLS_LMS

**功能**：启用LMS签名方案。
**详细说明**：

- LMS 是一种基于哈希函数的**后量子加密签名方案**，由 Robert J. Leighton 和 Silvio Micali 设计，旨在抵御量子计算机攻击。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration`

------

### CONFIG_MBEDTLS_HAVE_TIME

**功能**：启用基本时间支持。
**详细说明**：

- 系统需要提供`time.h`和`time()`函数。
- 时间值不需要非常精确，只需要能够计算时间差（例如用于超时控制）。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration`

------

### CONFIG_MBEDTLS_HAVE_TIME_DATE

**功能**：启用完整时间功能。
​**​详细说明​**​：

- 此功能用于验证X.509证书的有效期。
- 需要系统时钟支持，SNTP同步。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration`

------

### CONFIG_MBEDTLS_TEST

**功能**：启用自测试模式。
​**​详细说明​**​：

- 包含算法实现的自检测试。
- 上电时自动验证密码学原语。

- 可以在开发阶段启用，生产环境禁用（增加启动时间，同时会增加flash占用）。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support`

------

### CONFIG_MBEDTLS_TLS_VERSION_1_2

**功能**：启用TLS 1.2协议支持。
​**​详细说明​**​：

- 支持TLS 1.2协议（RFC 5246）。
- 提供广泛兼容的加密通信能力，必须启用（互联网主流协议）。

**默认值**：是（启用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > TLS`

------

### CONFIG_MBEDTLS_TLS_VERSION_1_3

**功能**：启用TLS 1.3协议支持。
​**​详细说明​**​：

- 支持最新TLS 1.3协议（RFC 8446）。
- 关键特性：1-RTT握手、0-RTT数据、增强安全性。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > TLS`

------

### CONFIG_MBEDTLS_X509_TRUSTED_CERT_CALLBACK

**功能**：启用用于控制 X.509 证书验证的自定义回调函数。
**详细说明**：

- TLS 握手时，客户端需验证服务器证书的有效性（如签名、有效期、信任链），该宏允许开发者替换默认的验证逻辑。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > TLS > Support for TLS 1.3`

------

### CONFIG_MBEDTLS_SSL_CONTEXT_SERIALIZATION

**功能**：启用控制 SSL/TLS 上下文（Context）序列化为可存储或传输的格式。
**详细说明**：

- 提供上下文序列化/反序列化的接口（如 `mbedtls_ssl_context_save()` 和 `mbedtls_ssl_context_load()`），允许用户将上下文状态保存到内存或存储设备。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > TLS > Support for TLS 1.3`

------

### CONFIG_MBEDTLS_SSL_CID_PADDING_GRANULARITY

**功能**：启用控制 TLS 1.3 连接标识符（Connection ID, CID）的填充粒度。
**详细说明**：

- 当值为 `0` 时，CID 有效载荷按实际长度填充至 16 字节（可能包含填充字节 `0x00`）。
- 当值为 `1` 时，CID 有效载荷按加密块大小对齐填充（如 AES 块大小 16 字节时，直接填充至 16 字节）。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > TLS > Support for TLS 1.3`

------

### CONFIG_MBEDTLS_ECDH_LEGACY_CONTEXT

**功能**：启用控制 ECDH（椭圆曲线 Diffie-Hellman）密钥交换的上下文兼容性模式。
**详细说明**：

- **启用（定义为 `1`）**：mbed TLS 使用旧版 ECDH 上下文（如基于 `mbedtls_ecdh_context_legacy` 结构体），支持传统曲线参数和计算方式，需要与旧版本 TLS 实现（如 TLS 1.2）兼容的场景。
- **禁用（未定义或 `0`）**：使用新版 ECDH 上下文（如 `mbedtls_ecdh_context`），支持更安全的曲线（如 X25519）和优化的计算逻辑。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > TLS > Support for TLS 1.3`

------

### CONFIG_MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH

**功能**：启用控制 SSL/TLS 消息缓冲区的长度是否动态可变。
**详细说明**：

- **启用（定义为 `1`）**：mbed TLS 使用动态缓冲区（如通过 `mbedtls_ssl_buffer` 结构体管理），根据实际消息长度调整缓冲区大小，避免内存浪费或截断。
- **禁用（未定义或 `0`）**：使用固定长度缓冲区（由 `MBEDTLS_SSL_MAX_CONTENT_LEN` 定义），可能导致长消息被截断或短消息内存浪费。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > TLS > Support for TLS 1.3`

------

### CONFIG_MBEDTLS_TLS_SESSION_TICKETS

**功能**：启用TLS会话票据机制，支持无状态会话恢复。
**详细说明**：

- 实现了TLS协议的会话票据扩展（RFC 5077），允许服务器在不需要存储会话状态的情况下恢复之前的TLS会话。
- 当客户端再次连接到服务器时，通过提交之前获得的会话票据，可以跳过完整的握手过程，从而减少延迟和计算开销。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > TLS > Support for TLS 1.3`

------

### CONFIG_MBEDTLS_DTLS

**功能**：启用DTLS协议支持（Datagram Transport Layer Security）。
**详细说明**：

- 基于UDP的TLS协议（DTLS 1.2）。
- 适用于实时媒体传输/IoT通信场景。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > TLS > Support for TLS 1.2 (DTLS 1.2)`

------

### CONFIG_MBEDTLS_KEY_EXCHANGE_PSK_ENABLED

**功能**：启用基于预共享密钥（PSK）的密钥交换机制。
**详细说明**：

- 仅使用预共享密钥建立安全连接，无需证书或公钥基础设施。

**默认值**：是（启用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration`

------

### CONFIG_MBEDTLS_KEY_EXCHANGE_DHE_PSK_ENABLED

**功能**：启用DHE-PSK（Diffie-Hellman Ephemeral with Pre-Shared Key）密钥交换算法。
**详细说明**：

- DHE-PSK是一种混合密钥交换机制，结合了预共享密钥（PSK）和前向保密的Diffie-Hellman临时密钥交换（DHE）。其核心作用原理如下：
  1. **预共享密钥（PSK）**：客户端和服务器预先共享一个密钥（通常是一个字符串或十六进制数据），用于身份验证。
  2. **临时Diffie-Hellman（DHE）**：双方在每次会话中生成临时的DH密钥对，通过交换公钥计算出共享密钥，实现前向保密。
  3. **密钥派生**：将PSK和DHE计算出的共享密钥通过密钥派生函数（如TLS-PRF）组合生成最终会话密钥。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration`

------

### CONFIG_MBEDTLS_KEY_EXCHANGE_ECDHE_PSK_ENABLED

**功能**：启用ECDHE-PSK（Elliptic Curve Diffie-Hellman Ephemeral with Pre-Shared Key）密钥交换算法。
**详细说明**：

- 结合预共享密钥（PSK）认证与椭圆曲线临时密钥交换（ECDHE）。
- 前向保密（PFS）：每次会话生成临时的ECC私钥。
- PSK身份验证：验证通信双方身份。
- 相比RSA/DH：更小的密钥尺寸（256位ECC≈3072位RSA）。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration`

------

### CONFIG_MBEDTLS_KEY_EXCHANGE_RSA_PSK_ENABLED

**功能**：启用RSA-PSK混合密钥交换机制，结合RSA证书与预共享密钥进行身份验证。
**详细说明**：

- 同时使用RCA证书（服务器身份）与PSK（客户端身份）进行双因素认证。
- 结合RSA证书和预共享密钥。
- 服务器用RSA证书验证，客户端用PSK。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration`

------

### CONFIG_MBEDTLS_PSK_MAX_LEN

**功能**：设置预共享密钥（PSK）的最大允许长度。
**详细说明**：

- 限制预共享密钥字节长度，防止缓冲区溢出攻击。

**默认值**：32字节

------

### CONFIG_MBEDTLS_KEY_EXCHANGE_RSA_ENABLED

**功能**：启用传统的RSA密钥交换算法。
**详细说明**：

- 传统RSA加密预主密钥，不提供前向保密性。

**默认值**：是（启用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration`

------

### CONFIG_MBEDTLS_KEY_EXCHANGE_DHE_RSA_ENABLED

**功能**：启用DHE-RSA（Diffie-Hellman Ephemeral with RSA）密钥交换算法，提供前向保密特性。
**详细说明**：

- RSA证书认证 + 临时Diffie-Hellman。
- 提供前向保密性。

**默认值**：是（启用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration`

------

### CONFIG_MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED

**功能**：启用ECDHE-RSA密钥交换算法，提供前向保密和服务器身份验证。
**详细说明**：

- RSA证书认证 + 临时ECDH。

**默认值**：是（启用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration`

------

### CONFIG_MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED

**功能**：启用ECDHE-ECDSA密钥交换
​**​详细说明​**​：

- 使用临时椭圆曲线Diffie-Hellman（ECDHE）实现前向保密。
- 使用椭圆曲线数字签名算法（ECDSA）进行身份验证。
- 相比RSA，具有更快的运算速度和更短的密钥长度。
- 需要服务器持有ECDSA证书。

**默认值**：是（启用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration`

### CONFIG_MBEDTLS_KEY_EXCHANGE_ECDH_ECDSA_ENABLED

**功能**：启用静态ECDH-ECDSA密钥交换算法，基于永久椭圆曲线密钥进行身份验证。
**详细说明**：

- 淘汰的安全机制，永久密钥一旦泄露，所有历史通信可被解密。

**默认值**：是（启用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration`

------

### CONFIG_MBEDTLS_ECDSA_DETERMINISTIC

**功能**：启用确定性ECDSA签名生成，根据RFC 6979标准实现。
**详细说明**：

- 基于RFC 6979的确定性签名，消除随机数生成失败风险。
- 防止随机数问题导致私钥泄露，相同输入产生相同签名。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration`

### CONFIG_MBEDTLS_KEY_EXCHANGE_ECDH_RSA_ENABLED

**功能**：启用静态ECDH-RSA密钥交换算法，使用固定椭圆曲线密钥进行RSA身份验证。
**详细说明**：

- 使用预先生成的固定ECC密钥进行密钥交换。
- 长期密钥泄露导致历史通信可被解密。

**默认值**：是（启用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration`

### CONFIG_MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED

**功能**：启用ECJPAKE密钥交换算法，用于支持基于密码的认证密钥交换。
**详细说明**：

- 使用共享的密码（口令）进行身份认证，不需要预置证书或密钥。
- **前向保密**：每次会话使用临时密钥，即使密码泄露也不会影响之前会话的安全性。
- **抵抗离线字典攻击**：协议设计使得攻击者无法通过截获的数据进行离线密码猜测。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration`

------

### CONFIG_MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED

**功能**：启用TLS 1.3协议中基于预共享密钥（PSK）的密钥交换模式。
**详细说明**：

- 完全依赖预置密钥验证身份，无需证书交换或公钥运算。
- 客户端首次消息可携带加密应用数据，消除握手延迟。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration`

------

### CONFIG_MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED

**功能**：启用TLS 1.3协议中基于临时密钥的密钥交换模式
**详细说明**：

- 使用椭圆曲线临时Diffie-Hellman（ECDHE）或有限域Diffie-Hellman（DHE）进行密钥协商。
- 每次会话生成唯一的临时密钥，长期密钥泄露不影响历史会话安全。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration`

------

### CONFIG_MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED

**功能**：启用TLS 1.3中PSK与临时密钥(EC)DHE的混合模式，兼具性能和安全性优势
**详细说明**：

- 结合预共享密钥(PSK)的效率和临时密钥的前向保密性

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration`

------

### CONFIG_MBEDTLS_HKDF_C

**功能**：启用HMAC-based Key Derivation Function (HKDF)算法，实现安全密钥派生功能。
**详细说明**：

- 基于RFC 5869的HMAC密钥派生函数，从初始密钥材料派生安全密钥。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration`

------

### CONFIG_MBEDTLS_CIPHER_AES_ENABLED

**功能**：启用AES加密算法。
​**​详细说明​**​：

- 支持128/192/256位密钥。
- 提供ECB/CBC/CTR/GCM/XTS等模式。

**默认值**：是（启用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration`

------

### CONFIG_MBEDTLS_AES_ROM_TABLES

**功能**：启用AES算法使用预先存储在ROM中的S盒表，替代运行时计算生成。
**详细说明**：

**默认值**：是（启用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration > AES block cipher`

------

### CONFIG_MBEDTLS_AES_FEWER_TABLES

**功能**：启用AES查表优化，以减少内存使用（ROM或RAM）为代价，降低AES算法的计算速度。
**详细说明**：

- 牺牲速度换取空间

**默认值**：是（启用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration > AES block cipher > Use precomputed AES tables stored in ROM.`

------

### CONFIG_MBEDTLS_CIPHER_CAMELLIA_ENABLED

**功能**：启用Camellia加密算法
​**​详细说明​**​：

- 日本NEC开发的分组密码，与AES同安全强度（128/192/256位），可作为AES替代方案。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration`

------

### CONFIG_MBEDTLS_CIPHER_DES_ENABLED

**功能**：启用DES（Data Encryption Standard）对称加密算法支持。
**详细说明**：

- 遗留的56位分组密码，提供3DES增强版本。
- 不推荐使用（已破解），仅用于兼容传统系统。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration`

------

### CONFIG_MBEDTLS_CIPHER_CHACHA20_ENABLED

**功能**：启用ChaCha20流密码算法，提供高性能对称加密功能
**详细说明**：

- 高性能流式加密算法，它对于没有AES硬件加速的设备尤为重要，且性能优于软件实现的AES。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration`

------

### CONFIG_MBEDTLS_CIPHER_CCM_ENABLED

**功能**：启用CCM（Counter with CBC-MAC）认证加密模式，提供机密性和完整性保护。
**详细说明**：

- CCM是无线通信和物联网安全的核心加密模式，结合CTR加密和CBC-MAC认证。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration`

------

### CONFIG_MBEDTLS_CIPHER_GCM_ENABLED

**功能**：启用AES-GCM（Galois/Counter Mode）认证加密算法，提供高速的机密性和完整性保护
**详细说明**：

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration`

------

### CONFIG_MBEDTLS_CIPHER_MODE_XTS_ENABLED

**功能**：启用XTS（XEX-based Tweaked CodeBook mode with ciphertext Stealing）分组密码工作模式，专为**随机访问加密存储**设计，提供机密性和完整性保护。
**详细说明**：

**​默认值​**​：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration > AES block cipher`

------

### CONFIG_MBEDTLS_CIPHER_MODE_CBC_ENABLED

**功能**：启用CBC（Cipher Block Chaining，密码块链接）加密模式，一种经典的分组密码工作模式，通过**块间异或链接**实现机密性保护
**详细说明**：

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration`

------

### CONFIG_MBEDTLS_CIPHER_MODE_CTR_ENABLED

**功能**：启用CTR（Counter，计数器）加密模式，将分组密码（如AES）转换为**流密码**，支持对数据的**随机访问**和**并行加密**
**详细说明**：

**​默认值​**​：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration`

------

### CONFIG_MBEDTLS_ECDH_C

**功能**：启用椭圆曲线Diffie-Hellman（ECDH）密钥交换算法。
**详细说明**：

- 支持在不安全信道上安全协商共享密钥，是构建TLS/SSL、IPSec等安全协议的核心组件。

**​默认值​**​：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration`

------

### CONFIG_MBEDTLS_ECDSA_C

**功能**：启用椭圆曲线数字签名算法（ECDSA，Elliptic Curve Digital Signature Algorithm）。
**详细说明**：

- ECDSA-xxx的密码套件需要开启此选项。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration`

------

### CONFIG_MBEDTLS_ECJPAKE_C

**功能**：启用椭圆曲线密码认证密钥交换协议（ECJPAKE，Elliptic Curve Password-Authenticated Key Exchange）
**详细说明**：

- ECJPAKE-xxx 的密码套件需要开启此选项。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration`

------

### CONFIG_MBEDTLS_ECP_C

**功能**：启用椭圆曲线密码学（ECC，Elliptic Curve Cryptography）的核心原语模块，提供椭圆曲线点运算、标量乘法、密钥对生成等基础功能。
**详细说明**：

- 提供椭圆曲线数学运算基础
- 所有ECC功能的依赖项，启用任何ECC功能必须先开启

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration`

------

### CONFIG_MBEDTLS_ECP_DP_SECP192R1_ENABLED

**功能**：启用NIST标准椭圆曲线**secp192r1**（Prime Field Curve P-192）的支持。
**详细说明**：

- 96位安全强度，曾广泛用于早期TLS协议（如TLS 1.0/1.1），但因安全性不足已被NIST弃用

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration > Elliptic curve over GF(p) library`

------

### CONFIG_MBEDTLS_ECP_DP_SECP224R1_ENABLED

**功能**：启用NIST标准椭圆曲线**secp224r1**（Prime Field Curve P-224）的支持。
**详细说明**：

- 该曲线属于素数域（GF(p)）上的椭圆曲线，曾广泛用于TLS协议（如TLS 1.0/1.1）和早期安全通信场景，但因安全性不足已被NIST弃用。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration > Elliptic curve over GF(p) library`

------

### CONFIG_MBEDTLS_ECP_DP_SECP256R1_ENABLED

**功能**：启用NIST标准椭圆曲线**secp256r1**（Prime Field Curve P-256）的支持。
**详细说明**：

- secp256r1是目前最广泛使用的椭圆曲线之一，平衡了安全强度与计算性能，是构建现代安全通信（如TLS 1.3、IoT设备配对）的核心密码学原语。

**默认值**：是（启用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration > Elliptic curve over GF(p) library`

------

### CONFIG_MBEDTLS_ECP_DP_SECP384R1_ENABLED

**功能**：启用NIST标准椭圆曲线**secp384r1**（Prime Field Curve P-384）的支持。
**详细说明**：

- secp384r1是NIST SP 800-56A标准中定义的高安全椭圆曲线，提供**192位安全强度**（等效RSA 7680位密钥），是当前主流高安全场景的首选密码学原语。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration > Elliptic curve over GF(p) library`

------

### CONFIG_MBEDTLS_ECP_DP_SECP521R1_ENABLED

**功能**：启用NIST标准椭圆曲线**secp521r1**（Prime Field Curve P-521）的支持。
**详细说明**：

- secp521r1是目前NIST定义的最高安全强度的椭圆曲线之一，提供**256位安全强度**（等效RSA 15360位密钥），专为极端高安全场景设计。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration > Elliptic curve over GF(p) library`

------

### CONFIG_MBEDTLS_ECP_DP_SECP192K1_ENABLED

**功能**：启用NIST标准椭圆曲线**secp192k1**（Prime Field Curve K-192）的支持。
**详细说明**：

- secp192k1是NIST SP 800-56A标准中定义的椭圆曲线之一，属于**Koblitz曲线家族**（基于二进制域优化的素数域曲线），曾用于早期TLS协议（如TLS 1.0/1.1），但因安全性不足已被逐步弃用。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration > Elliptic curve over GF(p) library`

------

### CONFIG_MBEDTLS_ECP_DP_SECP224K1_ENABLED

**功能**：启用NIST标准椭圆曲线**secp224k1**（Prime Field Curve K-224）的支持。
**详细说明**：

- secp224k1是NIST SP 800-56A标准中定义的**Koblitz曲线家族**成员（基于二进制域优化的素数域曲线），曾用于早期TLS协议（如TLS 1.0/1.1），但因安全性不足已被逐步弃用。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration > Elliptic curve over GF(p) library`

------

### CONFIG_MBEDTLS_ECP_DP_SECP256K1_ENABLED

**功能**：启用NIST标准椭圆曲线**secp256k1**（Prime Field Curve K-256）的支持。
**详细说明**：

- secp256k1是Koblitz曲线家族（基于二进制域优化的椭圆曲线）的成员，以**计算高效性**著称，广泛应用于区块链（如比特币、莱特币）和低功耗物联网设备中。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration > Elliptic curve over GF(p) library`

------

### CONFIG_MBEDTLS_ECP_DP_BP256R1_ENABLED

**功能**：启用NIST标准椭圆曲线**BP256R1**（Brainpool P-256）的支持。
**详细说明**：

- BP256R1是德国密码学家设计的**Brainpool曲线家族**成员，旨在提供更高的安全性和性能，是NIST SP 800-56A标准中推荐的椭圆曲线之一，适用于高安全场景。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration > Elliptic curve over GF(p) library`

------

### CONFIG_MBEDTLS_ECP_DP_BP384R1_ENABLED

**功能**：启用NIST标准椭圆曲线**BP384R1**（Brainpool P-384）的支持。
**详细说明**：

- BP384R1是德国密码学家设计的**Brainpool曲线家族**成员，提供**192位安全强度**（等效RSA 7680位密钥），是高安全场景下的优选密码学原语，旨在替代传统高安全曲线（如secp384r1）并增强抗攻击能力。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration > Elliptic curve over GF(p) library`

------

### CONFIG_MBEDTLS_ECP_DP_BP512R1_ENABLED

**功能**：启用NIST标准椭圆曲线**BP512R1**（Brainpool P-512）的支持。
**详细说明**：

- BP512R1是德国密码学家设计的**Brainpool曲线家族**中512位安全强度的椭圆曲线，提供**256位安全强度**（等效RSA 15360位密钥），是当前已知最高安全强度的椭圆曲线之一，专为极端高安全场景设计。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration > Elliptic curve over GF(p) library`

------

### CONFIG_MBEDTLS_ECP_DP_CURVE25519_ENABLED

**功能**：启用**Curve25519**椭圆曲线的支持。
**详细说明**：

- Curve25519是由密码学家Daniel J. Bernstein设计的现代椭圆曲线，专为高效、安全的密钥交换和数字签名设计，是当前主流密码学协议（如TLS 1.3、WireGuard、SSH）的首选曲线之一。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration > Elliptic curve over GF(p) library`

------

### CONFIG_MBEDTLS_ECP_DP_CURVE448_ENABLED

**功能**：启用**Curve448**（X448）椭圆曲线的支持。
**详细说明**：

- Curve448是由密码学家Daniel J. Bernstein设计的现代椭圆曲线，专为高效、安全的密钥交换设计，提供**224位安全强度**（等效RSA 5096位密钥），是当前已知高安全场景下的优选密码学原语，广泛应用于TLS 1.3扩展、量子抗性协议等前沿领域。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration > Elliptic curve over GF(p) library`

## 支持的哈希算法

### CONFIG_MBEDTLS_MD5

**功能**：启用**MD5（Message Digest Algorithm 5）**哈希算法的支持。
**详细说明**：

- MD5是一种经典的密码学哈希函数，由Ronald Rivest设计，广泛用于数据完整性校验、密码存储（尽管不安全）等场景。广泛用于TLS 1.0/1.1协议。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration`

------

### CONFIG_MBEDTLS_SHA1

**功能**：启用**SHA-1（Secure Hash Algorithm 1）**哈希算法的支持。
**详细说明**：

- SHA-1是一种经典的密码学哈希函数，由美国国家安全局（NSA）设计，广泛用于数据完整性校验、数字签名（如SSL/TLS早期版本）、密码存储（已弃用）等场景。

**默认值**：是（启用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration`

------

### CONFIG_MBEDTLS_SHA224

**功能**：启用**SHA-224（Secure Hash Algorithm 224）**哈希算法的支持。
**详细说明**：

- SHA-224是NIST（美国国家标准与技术研究院）设计的**SHA-2家族**成员之一，提供**224位（28字节）的安全强度**，广泛用于数据完整性校验、数字签名（如TLS 1.2/1.3）等场景。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration`

------

### CONFIG_MBEDTLS_SHA256

**功能**：启用**SHA-256（Secure Hash Algorithm 256）**哈希算法的支持。
**详细说明**：

- SHA-256是NIST（美国国家标准与技术研究院）设计的**SHA-2家族**核心成员之一，提供**256位（32字节）的安全强度**，是目前全球最广泛使用的安全哈希算法之一。

**默认值**：是（启用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration`

------

### CONFIG_MBEDTLS_SHA256_SMALLER

**功能**：启用**SHA-256的小内存/小代码体积实现**（`mbedtls_sha256_small`）。
**详细说明**：

- 该宏用于在资源受限的设备（如低功耗IoT终端、微控制器）中优化SHA-256的计算效率，通过减少内存占用和代码体积，平衡安全性与资源消耗。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration`

------

### CONFIG_MBEDTLS_SHA384

**功能**：启用**SHA-384（Secure Hash Algorithm 384）**哈希算法的支持。
**详细说明**：

- SHA-384是NIST（美国国家标准与技术研究院）设计的**SHA-2家族**成员之一，提供**384位（48字节）的安全强度**，是目前全球广泛使用的高安全哈希算法之一。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration`

------

### CONFIG_MBEDTLS_SHA512

**功能**：启用**SHA-512（Secure Hash Algorithm 512）**哈希算法的支持。
**详细说明**：

- SHA-512是NIST（美国国家标准与技术研究院）设计的**SHA-2家族**核心成员之一，提供**512位（64字节）的安全强度**，是目前全球最高安全强度的通用哈希算法之一。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration`

------

### CONFIG_MBEDTLS_POLY1305

**功能**：启用**Poly1305**密码学消息认证码（MAC）算法的支持。
**详细说明**：

- Poly1305是由密码学家Daniel J. Bernstein设计的轻量级、高安全性的MAC算法，主要用于验证数据的完整性和真实性。它通常与ChaCha20流密码结合使用（形成ChaCha20-Poly1305），是TLS 1.3、WireGuard等现代安全协议的核心组件。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration`

------

### CONFIG_MBEDTLS_CMAC

**功能**：启用**CMAC（Cipher-based Message Authentication Code，基于密码的消息认证码）**算法的支持。
**详细说明**：

- CMAC是一种基于分组密码（如AES）的高效消息认证码，用于验证数据的完整性和真实性。它通过结合分组密码的加密特性与MAC的认证功能，提供比传统HMAC更安全、更灵活的认证机制。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration`

------

### CONFIG_MBEDTLS_CTR_DRBG_ENABLED

**功能**：启用**CTR_DRBG（Counter Mode Deterministic Random Bit Generator，基于计数器模式的确定性随机数生成器）**的支持。
**详细说明**：

- CTR_DRBG是一种基于块密码（如AES）的伪随机数生成器（PRNG），通过计数器模式（CTR）生成确定性的随机比特流。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration`

------

### CONFIG_MBEDTLS_HMAC_DRBG_ENABLED

**功能**：启用**HMAC_DRBG（HMAC-based Deterministic Random Bit Generator，基于HMAC的确定性随机数生成器）**的支持。
**详细说明**：

- HMAC_DRBG是一种基于哈希算法（如SHA-256、SHA-512）的伪随机数生成器（PRNG），通过HMAC（基于哈希的消息认证码）生成确定性的随机比特流。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration`

------

### CONFIG_MBEDTLS_DEBUG

**功能**：启用**mbed TLS调试模块**的支持。调试模块提供丰富的调试工具（如日志记录、错误检查、断言、内存调试等），帮助开发者在开发、测试阶段定位密码学算法、协议实现中的潜在问题。
**详细说明**：

- **调试日志**：输出详细的运行时信息（如算法执行步骤、参数值、错误码）。
- **内存调试**：检测内存泄漏、越界访问等常见问题（如`mbedtls_debug_mem`）。
- **断言检查**：在关键逻辑中插入断言（如`MBEDTLS_ASSERT`），验证前置条件。
- **算法追踪**：记录加密/解密、签名/验签等操作的中间结果（如AES的轮密钥、RSA的中间值）。**​**

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support`

------

### CONFIG_MBEDTLS_MEMORY_DEBUG

**功能**：启用**mbed TLS内存调试模块**的支持。
**详细说明**：

- 跟踪内存分配/释放操作，检测内存泄漏和越界。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support`

------

### CONFIG_MBEDTLS_CHACHAPOLY_AEAD_ENABLED

**功能**：启用**ChaCha20-Poly1305认证加密模式（AEAD）**的支持。
**详细说明**：

- ChaCha20-Poly1305是由密码学家Daniel J. Bernstein设计的轻量级、高安全性的认证加密算法，结合了ChaCha20流密码（提供加密）和Poly1305消息认证码（提供完整性验证），广泛用于现代安全协议（如TLS 1.3、WireGuard）。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration`

------

### CONFIG_MBEDTLS_GENPRIME_ENABLED

**功能**：启用**mbed TLS素数生成模块**的支持。
**详细说明**：

- 该模块提供大素数生成算法（如米勒-拉宾素性测试），用于生成密码学中所需的大素数（如RSA模数的素因子、ECC素数域参数等）。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration > RSA base support`

------

### CONFIG_MBEDTLS_ENTROPY_C

**功能**：启用**mbed TLS熵源模块**的支持。
**详细说明**：

- 熵源模块是mbed TLS的核心组件之一，负责生成密码学安全的随机数（熵），为加密算法（如RSA、AES）、密钥交换协议（如TLS）、数字签名（如ECDSA）等提供不可预测的随机输入。

**默认值**：是（启用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration`

------

### CONFIG_MBEDTLS_SSL_ALPN

**功能**：启用**mbed TLS SSL/TLS层的ALPN（Application-Layer Protocol Negotiation，应用层协议协商）**支持。
**详细说明**：

- ALPN是TLS 1.2及更高版本的标准扩展（RFC 7301），允许客户端和服务器在TLS握手过程中协商应用层协议（如HTTP/2、HTTP/1.1、WebSocket等），避免在应用层单独处理协议版本协商。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > TLS`

------

### CONFIG_MBEDTLS_CIPHER

**功能**：启用**mbed TLS对称加密模块**的支持。
**详细说明**：

- 该模块是mbed TLS的核心密码学组件之一，提供对称加密算法（如AES、ChaCha20、DES）及其模式（如CBC、CTR、GCM）的实现，用于保护数据的机密性。

**默认值**：是（启用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration`

------

### CONFIG_MBEDTLS_MD

**功能**：启用**mbed TLS消息摘要（Message Digest, MD）模块**的支持。
**详细说明**：

- 该模块是mbed TLS的核心密码学组件之一，提供通用消息摘要（哈希）算法的实现接口，支持SHA-1、SHA-256、SHA-512等多种哈希算法。

**默认值**：是（启用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration`

------

### CONFIG_MBEDTLS_PEM_CERTIFICATE_FORMAT

**功能**：启用**mbed TLS PEM格式证书支持**。
**详细说明**：

- PEM（Privacy-Enhanced Mail）是一种基于Base64编码的证书/密钥存储格式，广泛用于X.509证书、私钥、CSR（证书签名请求）等密码学对象的存储与传输。

**默认值**：是（启用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Ciphersuite configuration`

------

### CONFIG_MBEDTLS_PK_WRITE_C

**功能**：启用**mbed TLS公钥（Public Key, PK）写入功能**。
**详细说明**：

- 该宏控制mbed TLS是否支持将公钥对象（如RSA、ECC公钥）序列化为标准格式（如PEM、DER）并写入文件或内存缓冲区。

**默认值**：是（启用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration`

------

### CONFIG_MBEDTLS_PKCS5_C

**功能**：启用**mbed TLS PKCS#5密码学标准支持**。
**详细说明**：

- PKCS#5（Public-Key Cryptography Standards #5）是密码学中关于密钥派生、填充和密码验证的核心标准，定义了PBKDF2（基于密码的密钥派生函数2）和PKCS#7填充（用于块密码的填充方案）等关键算法。

**默认值**：是（启用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration`

------

### CONFIG_MBEDTLS_SERVER_NAME_INDICATION

**功能**：启用**mbed TLS服务器名称指示（Server Name Indication, SNI）支持**。
**详细说明**：

- SNI是TLS 1.2及更高版本的标准扩展（RFC 6066），允许客户端在TLS握手阶段向服务器明确指定目标服务器的名称（如域名或IP地址）。服务器可根据该名称选择对应的数字证书，从而支持多域名托管（如同一IP地址托管多个HTTPS站点）。

**默认值**：是（启用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration`

------

### CONFIG_MBEDTLS_SSL_CACHE_C

**功能**：启用**mbed TLS SSL会话缓存支持**。
**详细说明**：

- SSL会话缓存（SSL Session Caching）是TLS协议的一项优化机制，用于存储已建立的SSL会话信息（如会话ID、密钥参数、扩展信息等），以便在后续连接中快速恢复会话，避免重复执行完整的TLS握手流程。

**默认值**：是（启用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration`

------

### CONFIG_MBEDTLS_SSL_EXTENDED_MASTER_SECRET

**功能**：启用**mbed TLS SSL扩展主密钥（Extended Master Secret, EMS）支持**。
**详细说明**：

- EMS是TLS 1.3标准中引入的增强型密钥派生机制（RFC 8446），用于替代传统TLS中的预主密钥（Pre-Master Secret），通过多次哈希迭代生成更安全的“扩展主密钥”，从而提升密钥的抗攻击能力。

**默认值**：是（启用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration`

------

### CONFIG_MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG

**功能**：启用**mbed TLS与PSA Crypto（ARM安全架构）集成时使用外部随机数生成器（RNG）**的支持。
**详细说明**：

- PSA Crypto是ARM提出的安全计算架构，要求加密操作使用符合严格安全标准的随机数生成器（如硬件RNG或高熵软件RNG）。

**默认值**：是（启用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration > Select random source for built-in PSA crypto`

------

### CONFIG_MBEDTLS_PSA_CRYPTO_C

**功能**：启用**mbed TLS与ARM PSA Crypto（Platform Security Architecture Crypto）框架的集成支持**。
**详细说明**：

- PSA Crypto是ARM提出的标准化安全计算架构，提供统一的加密原语、密钥管理和安全服务接口，旨在提升嵌入式设备的安全性和互操作性。

**默认值**：是（启用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration`

------

### CONFIG_MBEDTLS_PSA_P256M_DRIVER_ENABLED

**功能**：启用**mbed TLS与ARM PSA Crypto框架集成时，针对P-256椭圆曲线（secp256r1）的优化驱动支持**。
**详细说明**：

- P-256（NIST SECG P-256，又称secp256r1）是广泛使用的椭圆曲线密码学（ECC）标准，适用于密钥交换（如ECDHE）、数字签名（如ECDSA）等场景。

**默认值**：是（启用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration`

------

### CONFIG_MBEDTLS_PSA_STATIC_KEY_SLOTS

**功能**：启用**mbed TLS对ARM PSA Crypto静态密钥槽（Static Key Slots）的支持**。
**详细说明**：

- 静态密钥槽是PSA Crypto框架中预分配的固定密钥存储位置，用于长期保存高敏感度的静态密钥（如设备根密钥、预共享密钥、硬件绑定密钥等）。（当前不支持）

- **默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration`

------

### CONFIG_MBEDTLS_PSA_KEY_SLOT_COUNT

**功能**：启用**mbed TLS对ARM PSA Crypto密钥槽数量的显式配置支持**。
**详细说明**：

- PSA Crypto的密钥槽是预分配的固定存储位置，用于安全保存高敏感度密钥（如根密钥、预共享密钥）。

**默认值**：32个

**Menu路径**：`mbed TLS Support > Mbed TLS configuration`

------

### CONFIG_MBEDTLS_USE_PSA_CRYPTO

**功能**：启用**mbed TLS使用ARM PSA Crypto框架作为底层密码学实现**。
**详细说明**：

- 将TLS/DTLS底层操作迁移到PSA API
- 统一硬件/软件加密接口

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration`

------

### CONFIG_MBEDTLS_SSL_DTLS_CONNECTION_ID

**功能**：启用**mbed TLS在DTLS（数据报传输层安全）协议中使用连接ID（Connection ID）**。
**详细说明**：

- DTLS（Datagram Transport Layer Security）是TLS在UDP之上的扩展，用于为不可靠数据报提供安全传输。连接ID是DTLS连接的核心标识符，用于在数据报中唯一标识属于某个DTLS连接的数据包，避免不同连接间的数据混淆。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration`

------

### CONFIG_MBEDTLS_NIST_KW_C

**功能**：启用**mbed TLS对NIST标准密钥封装机制（NIST Key Wrapping, NIST KW）的支持**。
**详细说明**：

- NIST KW是一种基于格的密钥封装算法（如NIST SP 800-56A修订版定义的Kyber算法），用于安全地封装对称密钥（如AES密钥），确保只有持有对应私钥的一方才能解封装得到明文密钥。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration`

------

### CONFIG_MBEDTLS_DHM_C

**功能**：启用**mbed TLS对Diffie-Hellman密钥交换（Diffie-Hellman Key Exchange, DHM）的支持**。
**详细说明**：

- DHM是一种经典的密钥协商协议，用于在不安全信道上建立共享密钥，其核心优势是支持**前向保密**（Perfect Forward Secrecy, PFS）——即使长期私钥泄露，历史会话的密钥也无法被破解。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration`

------

### CONFIG_MBEDTLS_X509_CRL_PARSE_C

**功能**：启用**mbed TLS对X.509证书撤销列表（X.509 Certificate Revocation List, CRL）的解析支持**。
**详细说明**：

- X.509 CRL的核心作用是**标记已撤销的证书**，确保只有未被撤销的证书可用于安全通信。

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration`

------

### CONFIG_MBEDTLS_X509_CSR_WRITE_C

**功能**：启用**mbed TLS对X.509证书签名请求（X.509 Certificate Signing Request, CSR）的写入支持**。
**详细说明**：

- X.509 CSR的核心作用是**向CA提交证书申请**，其内容需包含申请者的身份信息和公钥，并通过申请者的私钥签名以确保真实性。`MBEDTLS_X509_CSR_WRITE_C`的核心功能是**将生成的CSR编码为标准格式（如PEM/DER）并写入存储介质**

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration`

------

### CONFIG_MBEDTLS_X509_CSR_PARSE_C

**功能**：启用**mbed TLS对X.509证书签名请求（X.509 Certificate Signing Request, CSR）的解析支持**。
**详细说明**：

- **解析CSR的ASN.1编码数据，验证其结构合法性与签名有效性**

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration`

------

### CONFIG_MBEDTLS_X509_CRT_WRITE_C

**功能**：启用**mbed TLS对X.509数字证书的写入支持**。
**详细说明**：

- 创建X.509证书
- 自签名或CA签发

**默认值**：否（禁用）

**Menu路径**：`mbed TLS Support > Mbed TLS configuration`

