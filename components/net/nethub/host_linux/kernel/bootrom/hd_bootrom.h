/**
 * @file hd_bootrom.h
 * @brief Host Driver SDIO bootrom firmware download header definitions
 * @author mlwang
 * @date 2025-07-17
 * @details This header defines the complete bootrom firmware download protocol
 *          for Host Driver SDIO devices, including command structures, data
 *          types, configuration macros, and API function declarations for
 *          secure firmware loading and execution.
 */

#ifndef __HD_BOOTROM_H__
#define __HD_BOOTROM_H__
#include <linux/types.h>
#include <linux/firmware.h>

/** @defgroup BOOTROM_CONFIG Bootrom Configuration Macros
 * @{
 */
/** Default firmware filename for bootrom */
#define HD_BOOTROM_FW_NAME                       ("sdio_bootrom_test.bin")

/** Maximum command buffer size */
#define HD_BOOTROM_SDIO_CMD_BUFF_MAX             (2 * 1024)
/** Maximum acknowledgment buffer size */
#define HD_BOOTROM_SDIO_ACK_BUFF_MAX             (2 * 1024)

/** SDIO CPU group identifier */
#define HD_BOOTROM_SDIO_CPU_GROUP                (1)

/** SDIO transfer timeout (in 10us units) */
#define HD_BOOTROM_SDIO_TRANS_TIMEOUT            (1000 * 1000 / 10)

/** Maximum payload size for bootrom commands */
#define HD_BOOTROM_CMD_PAYLOAD_MAX               ((HD_BOOTROM_SDIO_CMD_BUFF_MAX - sizeof(bootrom_cmd_t)) & 0xfffffff0)
/** @} */

/** @defgroup BOOTROM_COMMANDS Bootrom Command Definitions
 * @brief Command codes for bootrom operations
 * @{
 */
#define HD_BOOTROM_CMD_LEN_HEADER                (0x04) /**< Length header command */
#define HD_BOOTROM_CMD_BOOTINFO_GET              (0x10) /**< Get boot information */
#define HD_BOOTROM_CMD_BOOTHEADER_LOAD           (0x11) /**< Load boot header */
#define HD_BOOTROM_CMD_PK1_LOAD                  (0x12) /**< Load public key 1 */
#define HD_BOOTROM_CMD_PK2_LOAD                  (0x13) /**< Load public key 2 */
#define HD_BOOTROM_CMD_SIGNATURE1_LOAD           (0x14) /**< Load signature 1 */
#define HD_BOOTROM_CMD_SIGNATURE2_LOAD           (0x15) /**< Load signature 2 */
#define HD_BOOTROM_CMD_AESIV_LOAD                (0x16) /**< Load AES IV */
#define HD_BOOTROM_CMD_SECTIONHEADER_LOAD        (0x17) /**< Load section header */
#define HD_BOOTROM_CMD_SECTIONDATA_LOAD          (0x18) /**< Load section data */
#define HD_BOOTROM_CMD_CHECK_IMAGE               (0x19) /**< Check image integrity */
#define HD_BOOTROM_CMD_RUN                       (0x1A) /**< Run firmware */
#define HD_BOOTROM_CMD_LOAD_BOOTHEADER_SHA384_P2 (0x1B) /**< Load boot header SHA384 P2 */
#define HD_BOOTROM_CMD_CHANGE_RATE               (0x20) /**< Change transfer rate */
#define HD_BOOTROM_CMD_RESET                     (0x21) /**< Reset command */
#define HD_BOOTROM_CMD_FLASH_ERASE               (0x30) /**< Flash erase */
#define HD_BOOTROM_CMD_FLASH_WRITE               (0x31) /**< Flash write */
#define HD_BOOTROM_CMD_FLASH_READ                (0x32) /**< Flash read */
#define HD_BOOTROM_CMD_FLASH_BOOT                (0x33) /**< Flash boot */
#define HD_BOOTROM_CMD_EFUSE_WRITE               (0x40) /**< eFuse write */
#define HD_BOOTROM_CMD_EFUSE_READ                (0x41) /**< eFuse read */
/** @} */

/** @defgroup BOOTROM_ACK Bootrom Acknowledgment Definitions
 * @brief Acknowledgment status codes
 * @{
 */
#define HD_BOOTOM_ACK_STA_OK                     (0x4b4f) /**< Operation successful */
/** @} */

/** @defgroup BOOTROM_STRUCTURES Bootrom Data Structures
 * @{
 */

/***************************** boot_header ********************************/
#pragma pack(push, 1)

/**
 * @struct bootheader_flash_cfg
 * @brief Boot header flash configuration
 */
typedef struct bootheader_flash_cfg {
    uint32_t magic_code;   /**< Magic code identifier */
    uint8_t flash_cfg[84]; /**< Flash configuration data */
    uint32_t crc32;        /**< CRC32 checksum */
} boot_flash_cfg_t;

/**
 * @struct bootheader_clock_cfg
 * @brief Boot header clock configuration
 */
typedef struct bootheader_clock_cfg {
    uint32_t magic_code;   /**< Magic code identifier */
    uint8_t flash_cfg[12]; /**< Clock configuration data */
    uint32_t crc32;        /**< CRC32 checksum */
} boot_clock_cfg_t;

/**
 * @struct bootheader_basic_cfg
 * @brief Boot header basic configuration with bit fields
 */
typedef struct bootheader_basic_cfg {
    uint32_t sign_type          : 2; /**< [1: 0] Signature type */
    uint32_t encrypt_type       : 2; /**< [3: 2] Encryption type */
    uint32_t key_sel            : 2; /**< [5: 4] Key slot selection */
    uint32_t xts_mode           : 1; /**< [6] XTS mode enable */
    uint32_t aes_region_lock    : 1; /**< [7] AES region lock (reserved) */
    uint32_t no_segment         : 1; /**< [8] No segment info flag */
    uint32_t boot2_enable       : 1; /**< [9] Boot2 enable flag */
    uint32_t boot2_rollback     : 1; /**< [10] Boot2 rollback flag */
    uint32_t cpu_master_id      : 4; /**< [14: 11] CPU master ID */
    uint32_t notload_in_bootrom : 1; /**< [15] Not load in bootrom flag */
    uint32_t crc_ignore         : 1; /**< [16] Ignore CRC flag */
    uint32_t hash_ignore        : 1; /**< [17] Ignore hash flag */
    uint32_t power_on_mm        : 1; /**< [18] Power on MM flag */
    uint32_t em_sel             : 3; /**< [21: 19] EM selection */
    uint32_t cmds_en            : 1; /**< [22] Command splitter enable */
    uint32_t cmds_wrap_mode     : 2; /**< [24: 23] Command wrap mode */
    uint32_t cmds_wrap_len      : 4; /**< [28: 25] Command wrap length */
    uint32_t icache_invalid     : 1; /**< [29] ICache invalid flag */
    uint32_t dcache_invalid     : 1; /**< [30] DCache invalid flag */
    uint32_t fpga_halt_release  : 1; /**< [31] FPGA halt release function */

    uint32_t group_image_offset; /**< Flash controller offset */
    uint32_t aes_region_len;     /**< AES region length */

    uint32_t img_len_cnt; /**< Image length or segment count */
    uint32_t hash256[8];  /**< SHA256 hash of the image */
} boot_basic_cfg_t;

/**
 * @struct bootheader_cpu_cfg
 * @brief Boot header CPU configuration
 */
typedef struct bootheader_cpu_cfg {
    uint8_t cpu_cfg[16]; /**< CPU configuration data */
} boot_cpu_cfg_t;

/**
 * @struct patch_cfg
 * @brief Patch configuration for bootrom
 */
typedef struct patch_cfg {
    uint32_t addr;  /**< Patch address */
    uint32_t value; /**< Patch value */
} boot_patch_cfg_t;

/**
 * @struct bootheader
 * @brief Complete boot header structure
 */
typedef struct bootheader {
    uint32_t magic_code; /**< Magic code identifier (4 bytes) */
    uint32_t version;    /**< Version information (4 bytes) */

    boot_flash_cfg_t flash_cfg; /**< Flash configuration (92 bytes) */
    boot_clock_cfg_t clock_cfg; /**< Clock configuration (20 bytes) */

    boot_basic_cfg_t basic_cfg; /**< Basic configuration (48 bytes) */

    boot_cpu_cfg_t cpu_cfg[1]; /**< CPU configuration (16 bytes) */

    uint32_t boot2_pt_addr_0; /**< Boot2 partition address 0 (4 bytes) */
    uint32_t boot2_pt_addr_1; /**< Boot2 partition address 1 (4 bytes) */

    uint32_t flash_cfg_table_addr; /**< Flash config table address (4 bytes) */
    uint32_t flash_cfg_table_len;  /**< Flash config table length (4 bytes) */

    boot_patch_cfg_t patch_on_read[3]; /**< Patches on read (24 bytes) */
    boot_patch_cfg_t patch_on_jump[3]; /**< Patches on jump (24 bytes) */

    uint32_t rsvd;  /**< Reserved (4 bytes) */
    uint32_t crc32; /**< CRC32 checksum (4 bytes) */
} bootheader_t;

/***************************** boot_info (ack data) ********************************/

/**
 * @struct boot_info_data
 * @brief Boot information data from device
 */
typedef struct boot_info_data {
    uint32_t version; /**< Device version */

    /** @name eFuse Hardware Configuration
     * @{
     */
    uint8_t sign_type[HD_BOOTROM_SDIO_CPU_GROUP];  /**< Signature type from eFuse */
    uint8_t encry_type[HD_BOOTROM_SDIO_CPU_GROUP]; /**< Encryption type from eFuse */
    uint8_t bus_remap;                             /**< Bus remap configuration */
    uint8_t flash_power_delay_level;               /**< Flash power delay level */
    /** @} */

    uint32_t sw_cfg0; /**< Software configuration 0 */
    uint64_t chip_id; /**< Unique chip identifier */
    uint32_t sw_cfg1; /**< Software configuration 1 */

    // uint32_t crc32;                                  /**< CRC32 (commented out) */
} boot_info_data_t;

/********************* signature ecckey/data, encryption iv  *********************/

/**
 * @struct sign256_public_key
 * @brief 256-bit ECC public key structure
 */
typedef struct sign256_public_key {
    uint32_t eckey_x[8]; /**< ECC public key X coordinate */
    uint32_t eckey_y[8]; /**< ECC public key Y coordinate */
    uint32_t crc32;      /**< CRC32 checksum */
} sign256_public_key_t;

/**
 * @struct sign256_data
 * @brief 256-bit signature data structure
 */
typedef struct sign256_data {
    uint32_t sign_len;      /**< Signature length */
    uint32_t sign_data[16]; /**< Signature data */
    uint32_t crc32;         /**< CRC32 checksum */
} sign256_data_t;

/**
 * @struct sign384_public_key
 * @brief 384-bit ECC public key structure
 */
typedef struct sign384_public_key {
    uint32_t eckey_x[12]; /**< ECC public key X coordinate */
    uint32_t eckey_y[12]; /**< ECC public key Y coordinate */
    uint32_t crc32;       /**< CRC32 checksum */
} sign384_public_key_t;

/**
 * @struct sign384_data
 * @brief 384-bit signature data structure
 */
typedef struct sign384_data {
    uint32_t sign_len;      /**< Signature length */
    uint32_t sign_data[24]; /**< Signature data */
    uint32_t crc32;         /**< CRC32 checksum */
} sign384_data_t;

/**
 * @struct encry_iv
 * @brief Encryption initialization vector
 */
typedef struct encry_iv {
    uint32_t encry_data[4]; /**< Encryption IV data */
    uint32_t crc32;         /**< CRC32 checksum */
} encry_iv_t;

/***************************** segment_header ********************************/

/**
 * @struct segment_header
 * @brief Firmware segment header structure
 */
typedef struct segment_header {
    uint32_t dest_addr; /**< Destination address for segment */
    uint32_t len;       /**< Segment length */
    uint32_t rsv;       /**< Reserved field */
    uint32_t crc32;     /**< CRC32 checksum */
} segment_header_t;

/***************************** bootrom cmd/ack ********************************/

/**
 * @struct bootrom_cmd
 * @brief Bootrom command structure
 */
typedef struct bootrom_cmd {
    uint8_t cmd;     /**< Command code */
    uint8_t sum;     /**< Checksum */
    uint16_t len;    /**< Data length */
    uint8_t data[0]; /**< Variable length data */
} bootrom_cmd_t;

/**
 * @struct bootrom_ack
 * @brief Bootrom acknowledgment structure
 */
typedef struct bootrom_ack {
    uint16_t status; /**< Status code */
    uint16_t len;    /**< Data length */
    uint8_t data[0]; /**< Variable length data */
} bootrom_ack_t;

#pragma pack(pop)
/***************************** End ******************************* */

/**
 * @struct hd_sdio_bootrom
 * @brief SDIO bootrom management structure
 * @details Structure for managing bootrom firmware download and boot process
 */
typedef struct hd_sdio_bootrom {
    /** @name Firmware Management
     * @{
     */
    const struct firmware *hd_fw; /**< Firmware binary data */
    uint32_t fw_curr_offset;      /**< Current firmware processing offset */
    /** @} */

    /** @name Firmware Properties
     * @{
     */
    uint32_t fw_version;     /**< Firmware version */
    uint8_t fw_sign_type;    /**< Firmware signature type */
    uint8_t fw_encry_type;   /**< Firmware encryption type */
    uint32_t fw_segment_num; /**< Number of firmware segments */
    /** @} */

    /** @name Chip Properties
     * @{
     */
    uint32_t chip_version;   /**< Chip version */
    uint8_t chip_sign_type;  /**< Chip signature type */
    uint8_t chip_encry_type; /**< Chip encryption type */
    uint64_t chip_id;        /**< Unique chip identifier */
    /** @} */

    /** @name Communication Buffers
     * @{
     */
    uint8_t sdio_cmd_buff[HD_BOOTROM_SDIO_CMD_BUFF_MAX]; /**< Command buffer */
    uint8_t sdio_ack_buff[HD_BOOTROM_SDIO_ACK_BUFF_MAX]; /**< Acknowledgment buffer */
    /** @} */
} hd_sdio_bootrom_t;

/** @} */

/** @defgroup BOOTROM_API Bootrom API Functions
 * @{
 */

/**
 * @brief Download firmware to device through bootrom
 * @param[in] card SDIO card structure
 * @param[in] fw_name Firmware filename to download (NULL for default)
 * @retval 0 Success - firmware downloaded and verified successfully
 * @retval -ENOMEM Memory allocation failure
 * @retval <0 Other error codes from download process
 * @details This function orchestrates the complete firmware download process
 *          including boot info retrieval, header download, signature/encryption
 *          handling, segment transfer, and verification for secure boot.
 */
int hd_bootrom_download_firmware(struct hd_sdio_card *card, char *fw_name);

/**
 * @brief Run downloaded firmware
 * @param[in] card SDIO card structure
 * @retval 0 Success - firmware execution started
 * @retval -ENOMEM Memory allocation failure
 * @retval <0 Other error codes from run command
 * @details This function sends the run command to start firmware execution
 *          after successful download and verification, transitioning the
 *          device from bootrom state to firmware execution state.
 */
int hd_bootrom_firmware_run(struct hd_sdio_card *card);

/** @} */

#endif
