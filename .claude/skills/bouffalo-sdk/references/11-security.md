# 安全特性

## 安全架构概览

Bouffalo SDK 提供多层安全机制，包括硬件安全引擎、安全启动、安全OTA、Flash加密、密钥保护等。

## 硬件安全引擎

### 安全外设

| 外设 | 功能 | 芯片支持 |
|-----|------|---------|
| SEC_AES | AES加密/解密 | 所有芯片 |
| SEC_SHA | SHA哈希 | 所有芯片 |
| SEC_TRNG | 真随机数生成 | 所有芯片 |
| SEC_PKA | 公钥算法 | 所有芯片 |
| SEC_DSA | 数字签名 | 部分芯片 |
| SEC_GMAC | 消息认证码 | 部分芯片 |

## AES 加密

### 特性

- 支持128/192/256位密钥
- ECB、CBC、CTR、GCM模式
- 硬件加速
- DMA传输

### AES 加密示例

```c
#include "bflb_sec_aes.h"

void aes_encrypt_example(void)
{
    uint8_t key[16] = {0x00, 0x01, ..., 0x0F};  // 128位密钥
    uint8_t plaintext[16] = {0xAA, 0xBB, ...};
    uint8_t ciphertext[16];

    // 初始化AES
    bflb_sec_aes_init();

    // 设置密钥
    bflb_sec_aes_set_key(key, 128);

    // 加密（ECB模式）
    bflb_sec_aes_encrypt_ecb(plaintext, ciphertext, 1);
}
```

### AES CBC模式

```c
void aes_cbc_example(void)
{
    uint8_t key[16] = {...};
    uint8_t iv[16] = {...};  // 初始化向量
    uint8_t plaintext[32] = {...};
    uint8_t ciphertext[32];

    bflb_sec_aes_init();
    bflb_sec_aes_set_key(key, 128);

    // CBC加密
    bflb_sec_aes_encrypt_cbc(plaintext, ciphertext, 2, iv);
}
```

## SHA 哈希

### 特性

- SHA-1、SHA-224、SHA-256
- 硬件加速
- 支持流式哈希

### SHA-256示例

```c
#include "bflb_sec_sha.h"

void sha256_example(void)
{
    uint8_t data[100] = {...};
    uint8_t hash[32];

    // 初始化SHA
    bflb_sec_sha_init();

    // 计算哈希
    bflb_sec_sha256(data, sizeof(data), hash);

    printf("SHA256: ");
    for(int i = 0; i < 32; i++) {
        printf("%02X", hash[i]);
    }
    printf("\n");
}
```

### 流式哈希

```c
void sha256_stream_example(void)
{
    uint8_t data1[50] = {...};
    uint8_t data2[50] = {...};
    uint8_t hash[32];

    bflb_sec_sha256_context ctx;

    // 开始
    bflb_sec_sha256_starts(&ctx);

    // 更新
    bflb_sec_sha256_update(&ctx, data1, sizeof(data1));
    bflb_sec_sha256_update(&ctx, data2, sizeof(data2));

    // 完成
    bflb_sec_sha256_finish(&ctx, hash);
}
```

## TRNG (真随机数生成器)

### 特性

- 硬件真随机数
- FIPS 140-2兼容
- 连续输出

### TRNG示例

```c
#include "bflb_sec_trng.h"

void trng_example(void)
{
    bflb_sec_trng_init();

    // 读取随机数
    uint32_t rand = bflb_sec_trng_read();
    printf("Random: 0x%08X\n", rand);

    // 读取多个字节
    uint8_t rand_bytes[16];
    bflb_sec_trng_read_bytes(rand_bytes, 16);
}
```

## PKA (公钥算法)

### 特性

- RSA、ECDSA
- 硬件加速
- 支持密钥生成、签名、验证

### RSA示例

```c
#include "bflb_sec_pka.h"

void rsa_example(void)
{
    // RSA签名
    uint8_t modulus[256] = {...};  // RSA-2048
    uint8_t private_key[256] = {...};
    uint8_t message[32] = {...};
    uint8_t signature[256];

    bflb_sec_pka_rsa_sign(private_key, modulus, message, signature);
}
```

## 安全启动

### 特性

- 验证固件签名
- 防止未授权固件运行
- 支持RSA/ECDSA签名

### 配置

```kconfig
config CONFIG_SECURE_BOOT
    bool "Enable secure boot"
    default n

config CONFIG_SECURE_BOOT_KEY
    hex "Public key"
    default 0x00000000000000000000000000000000
```

### 流程

```
Boot2 → 验证签名 → 签名有效 → 跳转到固件
                        ↓
                     签名无效 → 拒绝启动
```

## 安全OTA

### 特性

- HTTPS下载
- 固件签名验证
- 版本控制
- 抗回滚保护

### HTTPS FOTA

```c
#include "https_fota.h"

void https_fota_example(void)
{
    struct https_fota_config cfg = {
        .url = "https://server.com/firmware.bin",
        .server_cert = server_cert,
        .pub_key = pub_key,
        .version = 2,
    };

    // 开始OTA
    https_fota_start(&cfg);
}
```

### FOTA回调

```c
void fota_status_callback(void *arg, https_fota_status_t event)
{
    switch(event) {
        case HTTPS_FOTA_START:
            printf("FOTA start\n");
            break;
        case HTTPS_FOTA_DOWNLOADING:
            printf("Downloading: %d%%\n", progress);
            break;
        case HTTPS_FOTA_VERIFYING:
            printf("Verifying\n");
            break;
        case HTTPS_FOTA_SUCCESS:
            printf("FOTA success\n");
            break;
        case HTTPS_FOTA_FAILED:
            printf("FOTA failed\n");
            break;
    }
}
```

## Flash 加密

### 特性

- AES-256加密
- 透明加密
- 硬件解密

### 配置

```kconfig
config CONFIG_FLASH_ENCRYPTION
    bool "Enable Flash encryption"
    default n

config CONFIG_FLASH_ENCRYPTION_KEY
    hex "Flash encryption key"
    default 0x00000000000000000000000000000000
```

### 使用

```c
// Flash加密是透明的，应用层无需修改
// 读取加密的Flash时自动解密
bflb_flash_read(addr, buf, len);  // 自动解密
```

## 密钥管理

### EFUSE (电子熔丝)

**特性：**
- 一次性编程
- 硬件保护
- 密钥存储

**使用：**
```c
#include "bflb_efuse.h"

// 写入EFUSE（一次性）
bflb_efuse_write(0x10, 0xAABBCCDD);

// 读取EFUSE
uint32_t value = bflb_efuse_read(0x10);
```

### 密钥保护

**特性：**
- 硬件密钥存储
- 无法读取密钥
- 仅用于加密/解密

**使用：**
```c
// 设置密钥（存储在硬件）
bflb_sec_set_hardware_key(key, key_len);

// 使用密钥（无法读取）
bflb_sec_use_hardware_key();
```

## 抗回滚保护

### 特性

- 版本验证
- 防止降级攻击
- 支持多版本固件

### 配置

```kconfig
config CONFIG_ANTI_ROLLBACK
    bool "Enable anti-rollback"
    default n

config CONFIG_APP_ANTI_ROLLBACK_VER
    int "Application version"
    default 0
```

### 使用

```c
// 在固件中设置版本
void app_set_version(uint8_t version)
{
    // 存储版本号到Flash
    bflb_flash_write(VERSION_ADDR, &version, 1);
}

// Boot2验证版本
if (app_version < stored_version) {
    printf("Version rollback detected!\n");
    boot2_reject();
}
```

## mbedTLS 集成

### mbedTLS 配置

```kconfig
config CONFIG_MBEDTLS
    bool "Enable mbedTLS"
    default y

config CONFIG_MBEDTLS_AES
    bool "Enable AES"
    default y

config CONFIG_MBEDTLS_SHA256
    bool "Enable SHA256"
    default y
```

### mbedTLS AES

```c
#include "mbedtls/aes.h"

void mbedtls_aes_example(void)
{
    mbedtls_aes_context aes;
    uint8_t key[16] = {...};
    uint8_t plaintext[16] = {...};
    uint8_t ciphertext[16];

    mbedtls_aes_init(&aes);
    mbedtls_aes_setkey_enc(&aes, key, 128);
    mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_ENCRYPT, plaintext, ciphertext);
    mbedtls_aes_free(&aes);
}
```

### mbedTLS TLS

```c
#include "mbedtls/net_sockets.h"
#include "mbedtls/ssl.h"

void mbedtls_tls_example(void)
{
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;

    mbedtls_ssl_init(&ssl);
    mbedtls_ssl_config_init(&conf);
    mbedtls_ssl_setup(&ssl, &conf);

    // 配置TLS
    mbedtls_ssl_set_hostname(&ssl, "example.com");

    // 连接
    mbedtls_ssl_handshake(&ssl);

    // 发送/接收
    mbedtls_ssl_write(&ssl, data, len);
    mbedtls_ssl_read(&ssl, buf, len);

    mbedtls_ssl_free(&ssl);
    mbedtls_ssl_config_free(&conf);
}
```

## 安全最佳实践

### 1. 使用硬件安全引擎

```c
// 推荐：使用硬件AES
bflb_sec_aes_encrypt(data, ciphertext);

// 不推荐：使用软件AES
software_aes_encrypt(data, ciphertext);
```

### 2. 保护密钥

```c
// 推荐：存储在EFUSE或硬件密钥区
bflb_sec_set_hardware_key(key, key_len);

// 不推荐：存储在Flash中
bflb_flash_write(KEY_ADDR, key, key_len);
```

### 3. 验证签名

```c
// 推荐：验证固件签名
if (!verify_signature(firmware, signature, pubkey)) {
    printf("Invalid signature\n");
    return;
}

// 不推荐：不验证直接运行
run_firmware(firmware);
```

### 4. 使用安全OTA

```c
// 推荐：使用HTTPS + 签名验证
https_fota_start(&cfg);

// 不推荐：使用HTTP无验证
http_download(firmware_url);
```

### 5. 启用加密

```c
// 推荐：启用Flash加密
menuconfig → Security Options → Enable Flash encryption

// 不推荐：不加密敏感数据
```

## 安全检查清单

- [ ] 启用安全启动
- [ ] 启用Flash加密
- [ ] 使用硬件安全引擎
- [ ] 保护密钥（EFUSE/硬件密钥区）
- [ ] 验证固件签名
- [ ] 使用安全OTA
- [ ] 启用抗回滚保护
- [ ] 使用HTTPS/TLS
- [ ] 定期更新固件
- [ ] 进行安全审计

## 总结

Bouffalo SDK提供完整的安全机制，包括硬件安全引擎、安全启动、安全OTA、Flash加密、密钥保护。启用这些特性可以保护设备免受攻击。遵循安全最佳实践，定期更新固件。
