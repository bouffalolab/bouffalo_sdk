#ifndef __SDH_SD_H__
#define __SDH_SD_H__

#include "bflb_core.h"
#include "sdh_host.h"

#define SD_DEFAULT_BLOCK_SIZE             (512)
#define SD_DATA_TIMEOUT_MS                (1000)
#define SD_LP_CLK_HZ                      (400 * 1000)
#define SD_SDR12_CLK_HZ                   (25 * 1000 * 1000)
#define SD_SDR25_CLK_HZ                   (50 * 1000 * 1000)

/* OCR flag, ACMD41/R3 */
#define SD_OCR_FLAG_BUSY                  (1 << 31) /*!< Power up busy status */
#define SD_OCR_FLAG_HCS                   (1 << 30) /*!< Card high capacity status */
#define SD_OCR_FLAG_XPC                   (1 << 28) /*!< maximum performance mode */
#define SD_OCR_FLAG_S18R                  (1 << 24) /*!< Switch to 1.8V */
#define SD_OCR_VDD_27                     (1 << 15) /*!< VDD 2.7-2.8 */
#define SD_OCR_VDD_28                     (1 << 16) /*!< VDD 2.8-2.9 */
#define SD_OCR_VDD_29                     (1 << 17) /*!< VDD 2.9-3.0 */
#define SD_OCR_VDD_30                     (1 << 18) /*!< VDD 3.0-3.1 */
#define SD_OCR_VDD_31                     (1 << 19) /*!< VDD 3.1-3.2 */
#define SD_OCR_VDD_32                     (1 << 20) /*!< VDD 3.2-3.3 */
#define SD_OCR_VDD_33                     (1 << 21) /*!< VDD 3.3-3.4 */
#define SD_OCR_VDD_34                     (1 << 22) /*!< VDD 3.4-3.5 */
#define SD_OCR_VDD_35                     (1 << 23) /*!< VDD 3.5-3.6 */

/* CMD6 switch_function group */
#define SD_SWITCH_FUNC_GRUP_ACCESS_MODE   (0)
#define SD_SWITCH_FUNC_GRUP_CMD_SYSTEM    (1)
#define SD_SWITCH_FUNC_GRUP_DRV_STRENGTH  (2)
#define SD_SWITCH_FUNC_GRUP_CURR_LIMIT    (3)
/* CMD6 switch_function function */
/* access mode */
#define SD_SWITCH_FUNC_ACCESS_MODE_SDR12  (0x00)
#define SD_SWITCH_FUNC_ACCESS_MODE_SDR25  (0x01)
#define SD_SWITCH_FUNC_ACCESS_MODE_SDR50  (0x02)
#define SD_SWITCH_FUNC_ACCESS_MODE_SDR104 (0x03)
#define SD_SWITCH_FUNC_ACCESS_MODE_DDR50  (0x04)
/* cmd system */
#define SD_SWITCH_FUNC_CMD_SYSTEM_DEFAULT (0x01)
#define SD_SWITCH_FUNC_CMD_SYSTEM_EC      (0x01)
#define SD_SWITCH_FUNC_CMD_SYSTEM_OTP     (0x01)
#define SD_SWITCH_FUNC_CMD_SYSTEM_ASSD    (0x01)
#define SD_SWITCH_FUNC_CMD_SYSTEM_ESD     (0x0C)
#define SD_SWITCH_FUNC_CMD_SYSTEM_VENDOR  (0x0E)
/* driver strength */
#define SD_SWITCH_FUNC_DRV_STRENGTH_TYPEB (0x00)
#define SD_SWITCH_FUNC_DRV_STRENGTH_TYPEA (0x01)
#define SD_SWITCH_FUNC_DRV_STRENGTH_TYPEC (0x02)
#define SD_SWITCH_FUNC_DRV_STRENGTH_TYPED (0x03)
/* current limit */
#define SD_SWITCH_FUNC_CURR_LIMIT_200MA   (0x01)
#define SD_SWITCH_FUNC_CURR_LIMIT_400MA   (0x01)
#define SD_SWITCH_FUNC_CURR_LIMIT_600MA   (0x01)
#define SD_SWITCH_FUNC_CURR_LIMIT_800MA   (0x01)

struct sd_cid_s {
    uint8_t mid;        /*!< Manufacturer ID [127:120] */
    uint16_t oid;       /*!< OEM/Application ID [119:104] */
    uint8_t pnm[5 + 1]; /*!< Product name [103:64] */
    uint8_t prv;        /*!< Product revision [63:56] */
    uint32_t psn;       /*!< Product serial number [55:24] */
    uint16_t mdt;       /*!< Manufacturing date [19:8] */
};

struct sd_csd_s {
    uint8_t csd_structure; /*!< CSD structure [127:126] */
    uint8_t taac;          /*!< Data read access-time-1 [119:112] */
    uint8_t nsac;          /*!< Data read access-time-2 in clock cycles (NSAC*100) [111:104] */
    uint8_t tran_speed;    /*!< Maximum data transfer rate [103:96] */
    uint16_t ccc;          /*!< Card command classes [95:84] */
    uint8_t read_bl_len;   /* Maximum read data block length [83:80] */

    uint8_t read_bl_partial : 1; /*!< Partial blocks for read allowed [79:79] */
    uint8_t wr_blk_misalign : 1; /*!< Write block misalignment [78:78] */
    uint8_t rd_blk_misalign : 1; /*!< Read block misalignment [77:77] */
    uint8_t dsr_imp         : 1; /*!< DSR implemented [76:76] */

    uint32_t c_size; /*!< Device size [73:62] */

    /* Following fields from 'readCurrentVddMin' to 'deviceSizeMultiplier' exist in CSD version 1 */
    uint8_t vdd_r_curr_min : 3; /*!< Maximum read current at VDD min [61:59] */
    uint8_t vdd_r_curr_max : 3; /*!< Maximum read current at VDD max [58:56] */
    uint8_t vdd_w_curr_min : 3; /*!< Maximum write current at VDD min [55:53] */
    uint8_t vdd_w_curr_max : 3; /*!< Maximum write current at VDD max [52:50] */
    uint8_t c_size_mult    : 3; /*!< Device size multiplier [49:47] */

    uint8_t erase_blk_en       : 1; /*!< Erase single block enabled [46:46] */
    uint8_t sector_size        : 7; /*!< Erase sector size [45:39] */
    uint8_t wp_grp_size        : 7; /*!< Write protect group size [38:32] */
    uint8_t wp_grp_enable      : 1; /*!< Write protect group enabled [31:31] */
    uint8_t r2w_factor         : 3; /*!< Write speed factor [28:26] */
    uint8_t write_bl_len       : 4; /*!< Maximum write data block length [25:22] */
    uint8_t write_bl_partial   : 1; /*!< Partial blocks for write allowed [21:21] */
    uint8_t file_format_grp    : 1; /*!< File format group [15:15] */
    uint8_t copy               : 1; /*!< Copy flag [14:14] */
    uint8_t perm_write_protect : 1; /*!< Permanent write protection [13:13] */
    uint8_t tmp_write_protect  : 1; /*!< Temporary write protection [12:12] */
    uint8_t file_format        : 2; /*!< File format [11:10] */
};

struct sd_scr_s {
    uint8_t scr_structure         : 4; /*!< SCR Structure [63:60] */
    uint8_t sd_spec               : 4; /*!< SD memory card specification version [59:56] */
    uint8_t data_stat_after_erase : 1; /*!< Data status after erases [55:55] */
    uint8_t sd_security           : 3; /*!< Security specification supported [54:52] */
    uint8_t sd_bus_widths         : 4; /*!< Data bus widths supported [51:48] */
    uint8_t sd_spec3              : 1; /*!< Specification version 3.00 or higher [47:47]*/
    uint8_t cmd_support           : 2; /*!< Command support bits [33:32] 33-support CMD23, 32-support cmd20*/
};

struct sd_status_s {
    uint8_t dat_bus_width           : 2; /*!< current buswidth */
    uint8_t secure_mode             : 1; /*!< secured mode */
    uint16_t sd_card_type           : 6; /*!< sdcard type */
    uint32_t size_of_protected_area : 2; /*!< size of protected area */
    uint8_t speed_class;                 /*!< speed class of card */
    uint8_t performance_move;            /*!< Performance of move indicated by 1[MB/S]step */
    uint8_t au_size;                     /*!< size of AU */
    uint16_t erase_size;                 /*!< number of AUs to be erased at a time */
    uint8_t erase_timeout : 6;           /*!< timeout value for erasing areas specified by UNIT OF ERASE AU */
    uint8_t erase_offset  : 2;           /*!< fixed offset value added to erase time */
};

struct sd_card_s {
    struct sdh_host_s *host;
    uint32_t bus_clock_hz; /*!< SD bus clock frequency united in Hz */

    uint32_t card_spec_version; /*!< Card version (scr) */
    uint32_t sd_rca;            /*!< Relative address of the card */
    uint32_t ocr;               /*!< Raw OCR content */

    struct sd_cid_s sd_cid; /*!< Card IDentification */
    struct sd_csd_s sd_csd; /*!< CARD Specific Data register */
    struct sd_scr_s sd_scr; /*!< SD CARD Configuration Register */

    struct sd_status_s sd_status; /*!< sd 512 bit status */

    uint32_t card_flag;
#define SDH_SD_FLAG_SUP_HIGH_CAPAC  (1 << 0) /*!< Support high capacity SDHC/SDXC */
#define SDH_SD_FLAG_SUP_4BIT        (1 << 1) /*!< Support 4-bit data width */
#define SDH_SD_FLAG_SDHC            (1 << 2) /*!< Card is SDHC */
#define SDH_SD_FLAG_SDXC            (1 << 3) /*!< Card is SDXC */
#define SDH_SD_FLAG_SUP_SIGNAL_1V8  (1 << 4) /*!< card support 1.8v signal voltage*/
#define SDH_SD_FLAG_SUP_CMD23       (1 << 5) /*!< card support cmd23 flag*/
#define SDH_SD_FLAG_SUP_SPEED_CLASS (1 << 6) /*!< card support speed class control flag (cmd20) */

    uint32_t block_size;  /*!< Card block size */
    uint32_t block_count; /*!< Card total block number */
};

int sdh_sd_deinit(struct sd_card_s *sd_card);
int sdh_sd_card_init(struct sd_card_s *sd_card, struct sdh_host_s *sdh_host);

int sdh_sd_read_status(struct sd_card_s *sd_card);

int sdh_sd_read_blocks(struct sd_card_s *sd_card, void *rd_buff, uint32_t blk_addr, uint32_t blk_cnt);

int sdh_sd_write_blocks(struct sd_card_s *sd_card, void *wr_buff, uint32_t blk_addr, uint32_t blk_cnt);

int sdh_sd_erase_blocks(struct sd_card_s *sd_card, uint32_t blk_addr, uint32_t blk_cnt);

#endif
