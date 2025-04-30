#ifndef _BFLB_SDH_H_
#define _BFLB_SDH_H_

#include "bflb_core.h"

/** @addtogroup LHAL
  * @{
  */

/** @addtogroup SDH
  * @{
  */

#if defined(BL616) || defined(BL808)
#define SDH_STD_V3
#endif

/** @defgroup SDH_TRANSFER_DIR Data transfer direction.
  * @{
  */
#define SDH_TRANSFER_DIR_WR           0 /*!< USDHC transfer direction send. */
#define SDH_TRANSFER_DIR_RD           1 /*!< USDHC transfer direction receive. */

/**
  * @}
  */

/** @defgroup SDH_DATA_BUS_WIDTH Data transfer width.
  * @{
  */
#define SDH_DATA_BUS_WIDTH_1          0 /*!< 1-bit mode */
#define SDH_DATA_BUS_WIDTH_4          1 /*!< 4-bit mode */
#define SDH_DATA_BUS_WIDTH_8          2 /*!< 8-bit mode */
/**
  * @}
  */

/** @defgroup SDH_ENDIAN_MODE Endian mode.
  * @{
  */
#define SDH_ENDIAN_MODE_LITTLE        0 /*!< Little endian mode. */
#define SDH_ENDIAN_MODE_HALF_WORD_BIG 1 /*!< Half word big endian mode. */
#define SDH_ENDIAN_MODE_BIG           2 /*!< Big endian mode. */
/**
  * @}
  */

/** @defgroup SDH_DMA_FIFO_THRESHOLD FIFO threshold.
  * @{
  */
#define SDH_DMA_FIFO_THRESHOLD_64     0
#define SDH_DMA_FIFO_THRESHOLD_128    1
#define SDH_DMA_FIFO_THRESHOLD_192    2
#define SDH_DMA_FIFO_THRESHOLD_256    3
/**
  * @}
  */

/** @defgroup SDH_DMA_BURST DMA burst size.
  * @{
  */
#define SDH_DMA_BURST_32              0
#define SDH_DMA_BURST_64              1
#define SDH_DMA_BURST_128             2
#define SDH_DMA_BURST_256             3
/**
  * @}
  */

/* ADMA2 descriptor table:
 * |----------------|---------------|-------------|--------------------------|
 * | Address field  |     Length    | Reserved    |         Attribute        |
 * |----------------|---------------|-------------|--------------------------|
 * |63            32|31           16|15         06|05  |04  |03|02 |01 |00   |
 * |----------------|---------------|-------------|----|----|--|---|---|-----|
 * | 32-bit address | 16-bit length | 0000000000  |Act2|Act1| 0|Int|End|Valid|
 * |----------------|---------------|-------------|----|----|--|---|---|-----|
 *
 * ADMA2 action table:
 * | Act2 | Act1 |     Comment     | Operation                                                         |
 * |------|------|-----------------|-------------------------------------------------------------------|
 * |   0  |   0  | No op           | Don't care                                                        |
 * |------|------|-----------------|-------------------------------------------------------------------|
 * |   0  |   1  | Reserved        | Read this line and go to next one                                 |
 * |------|------|-----------------|-------------------------------------------------------------------|
 * |   1  |   0  | Transfer data   | Transfer data with address and length set in this descriptor line |
 * |------|------|-----------------|-------------------------------------------------------------------|
 * |   1  |   1  | Link descriptor | Link to another descriptor                                        |
 * |------|------|-----------------|-------------------------------------------------------------------|
 */
#if defined(BL808)
#define USDHC_ADMA2_ADDRESS_ALIGN (8)
#define USDHC_ADMA2_LENGTH_ALIGN  (4)
#else
#define USDHC_ADMA2_ADDRESS_ALIGN (1)
#define USDHC_ADMA2_LENGTH_ALIGN  (1)
#endif

#define SDH_ADMA2_DESC_LEN_MAX        (0xffff + 1)
#define SDH_ADMA2_DESC_LEN_MASK       (0xffff)

/** @defgroup SDH_DMA_TYPE DMA mode.
  * @{
  */
#define SDH_DMA_TYPE_SDMA             0 /*!< Simple DMA. */
#define SDH_DMA_TYPE_ADMA1            1 /*!< Advanced DMA 1. */
#define SDH_DMA_TYPE_ADMA2            2 /*!< Advanced DMA 2. */
/**
  * @}
  */

/** @defgroup SDH_ADMA2_ATTR ADMA2 attribute.
  * @{
  */
#define SDH_ADMA2_ATTR_VALID          (1 << 0) /*!< descriptor valid. */
#define SDH_ADMA2_ATTR_END            (1 << 1) /*!< descriptor end, complete. */
#define SDH_ADMA2_ATTR_INT            (1 << 2) /*!< DMA int. */
#define SDH_ADMA2_ATTR_ACT_NOP        (0 << 4) /*!< No operation, go to next line */
#define SDH_ADMA2_ATTR_ACT_RSV        (1 << 4) /*!< No operation, go to next line */
#define SDH_ADMA2_ATTR_ACT_TRAN       (2 << 4) /*!< Transfer data of one descriptor line. */
#define SDH_ADMA2_ATTR_ACT_LINK       (3 << 4) /*!< Link to another descriptor. */
/**
  * @}
  */

/** @defgroup SDH_CMD_TPYE Command type.
  * @{
  */
#define SDH_CMD_TPYE_NORMAL           0 /*!< Normal command */
#define SDH_CMD_TPYE_SUPEND           1 /*!< Suspend command */
#define SDH_CMD_TPYE_RESUME           2 /*!< Resume command */
#define SDH_CMD_TPYE_ABORT            3 /*!< Abort command */
#define SDH_CMD_TPYE_EMPTY            4 /*!< Empty command */
/**
  * @}
  */

/** @defgroup SDH_RESP_TYPE Response type.
  * @{
  */
#define SDH_RESP_TPYE_NONE            0 /*!< Response type: No Response */
#define SDH_RESP_TPYE_R1              1 /*!< Response type: R1 */
#define SDH_RESP_TPYE_R1b             2 /*!< Response type: R1b */
#define SDH_RESP_TPYE_R2              3 /*!< Response type: R2 */
#define SDH_RESP_TPYE_R3              4 /*!< Response type: R3 */
#define SDH_RESP_TPYE_R4              5 /*!< Response type: R4 */
#define SDH_RESP_TPYE_R5              6 /*!< Response type: R5 */
#define SDH_RESP_TPYE_R5b             7 /*!< Response type: R5b */
#define SDH_RESP_TPYE_R6              8 /*!< Response type: R6 */
#define SDH_RESP_TPYE_R7              9 /*!< Response type: R7 */
/**
  * @}
  */

/** @defgroup SDH_DATA_TPYE Data type.
  * @{
  */
#define SDH_DATA_TPYE_NORMAL          0 /*!< Transfer normal read/write data. */
#define SDH_DATA_TPYE_TUNING          1 /*!< Transfer tuning data. */
#define SDH_DATA_TPYE_BOOT            2 /*!< Transfer boot data. */
#define SDH_DATA_TPYE_BOOT_CONTINU    3 /*!< Transfer boot data continuously. */
/**
  * @}
  */

/** @defgroup SDH_AUTO_CMD Auto CMD mode.
  * @{
  */
#define SDH_AUTO_CMD_DISABLE          0 /*!< Auto Command Disable. */
#define SDH_AUTO_CMD_CMD12            1 /*!< Auto CMD_12 Enable. */
#define SDH_AUTO_CMD_CMD23            2 /*!< Auto CMD_23 Enable. */
/**
  * @}
  */

/** @defgroup SDH_NORMAL_STA Normal interrupt status.
  * @{
  */
#define SDH_NORMAL_STA_CMD_COMP       (1 << 0)  /*!< Command complete, but it may still be in a busy state. */
#define SDH_NORMAL_STA_TRAN_COMP      (1 << 1)  /*!< Transfer complete, data complete and busy end. */
#define SDH_NORMAL_STA_BLK_GAP_EVENT  (1 << 2)  /*!< Transfer stopped at block gap,  */
#define SDH_NORMAL_STA_DMA_INT        (1 << 3)  /*!< DMA interrupt */
#define SDH_NORMAL_STA_BUFF_WR_RDY    (1 << 4)  /*!< Buffer write ready. (Non-DMA mode) */
#define SDH_NORMAL_STA_BUFF_RD_RDY    (1 << 5)  /*!< Buffer read ready. (Non-DMA mode) */
#define SDH_NORMAL_STA_CARD_INSERT    (1 << 6)  /*!< Card insertion */
#define SDH_NORMAL_STA_CARD_REMOVE    (1 << 7)  /*!< Card removal */
#define SDH_NORMAL_STA_CARD_INT       (1 << 8)  /*!< Card interrupt (SDIO mode DAT[1]) */
#define SDH_NORMAL_STA_INT_A          (1 << 9)  /*!< Card interrupt INT_A */
#define SDH_NORMAL_STA_INT_B          (1 << 10) /*!< Card interrupt INT_B */
#define SDH_NORMAL_STA_INT_C          (1 << 11) /*!< Card interrupt INT_C */
#define SDH_NORMAL_STA_RETUNING_EVENT (1 << 12) /*!< Re-tuning event */
#define SDH_NORMAL_STA_ERROR          (1 << 15) /*!< Error interrupt */
/**
  * @}
  */

/** @defgroup SDH_ERROR_STA Normal interrupt status.
  * @{
  */
#define SDH_ERROR_ALL_MASK            (0xFFFF << 16)
#define SDH_ERROR_STA_CMD_TIMEOUT     (1 << (16 + 0))   /*!< No response received within 64 clock cycles after sending the CMD.. */
#define SDH_ERROR_STA_CMD_CRC_ERR     (1 << (16 + 1))   /*!< Response CRC error or CMD line conflict. */
#define SDH_ERROR_STA_CMD_END_BIT     (1 << (16 + 2))   /*!< The end bit of response is 0  */
#define SDH_ERROR_STA_CMD_IDX_ERR     (1 << (16 + 3))   /*!< Response index error */
#define SDH_ERROR_STA_DATA_TIMEOUT    (1 << (16 + 4))   /*!< Data timeout, 1.busy timeout, 2.crc status timeout, 3.read data timeout */
#define SDH_ERROR_STA_DATA_CRC_ERR    (1 << (16 + 5))   /*!< Data CRC error or CRC status error */
#define SDH_ERROR_STA_DATA_END_BIT    (1 << (16 + 6))   /*!< The end bit of data/crc is 0 */
#define SDH_ERROR_STA_CURR_LIMIT      (1 << (16 + 7))   /*!< Card power protection. */
#define SDH_ERROR_STA_AUTO_CMD_ERR    (1 << (16 + 8))   /*!< Auto send cmd12 or cmd23 error */
#define SDH_ERROR_STA_ADMA_ERR        (1 << (16 + 9))   /*!< ADMA error */
#define SDH_ERROR_STA_TUNING_ERROR    (1 << (16 + 10))  /*!< Tuning error */
#ifdef SDH_STD_V3                                       /*!< Vendor specific error */
#define SDH_ERROR_STA_SPI_MODE_ERR     (1 << (16 + 12)) /*!< SPI token error */
#define SDH_ERROR_STA_AXI_BUS_ERR      (1 << (16 + 13)) /*!< AXI bus response error */
#define SDH_ERROR_STA_CMD_COMP_TIMEOUT (1 << (16 + 14)) /*!< Command completion signal timeout */
#define SDH_ERROR_STA_CRC_STA_ERR      (1 << (16 + 15)) /*!< CRC status or MMC_BOOT_ACK error */
#endif
/**
  * @}
  */

/** @defgroup SDH_CMD feature cmd.
  * @{
  */
enum {
    SDH_CMD_GET_PRESENT_STA_RD_BUFF_RDY,
    SDH_CMD_GET_PRESENT_STA_WD_BUFF_RDY,
    SDH_CMD_GET_PRESENT_STA_RX_ACTIVE,
    SDH_CMD_GET_PRESENT_STA_TX_ACTIVE,
    SDH_CMD_GET_PRESENT_STA_RE_TUNING,
    SDH_CMD_GET_PRESENT_STA_DATA_LINE_ACTIVE,
    SDH_CMD_GET_PRESENT_STA_DATA_INHIBIT,
    SDH_CMD_GET_PRESENT_STA_CMD_INHIBIT,
    SDH_CMD_GET_PRESENT_STA_CMD_SIG,
    SDH_CMD_GET_PRESENT_STA_DATA_SIG,
    SDH_CMD_GET_PRESENT_STA_WR_PROTECT,
    SDH_CMD_GET_PRESENT_STA_CARD_DETECT,
    SDH_CMD_GET_PRESENT_STA_CARD_STABLE,
    SDH_CMD_GET_PRESENT_STA_CARD_INSERTED,

    SDH_CMD_SET_BUS_WIDTH,
    SDH_CMD_SET_HS_MODE_EN,
    SDH_CMD_SET_SD_BUS_POWER,
    SDH_CMD_SET_INT_AT_BLK_GAP_EN,

    SDH_CMD_SET_RD_WAIT_EN,
    SDH_CMD_SET_CONTINUE_REQ,
    SDH_CMD_SET_STOP_AT_BLK_GAP_REQ,

    SDH_CMD_SET_BUS_CLK_DIV,
    SDH_CMD_SET_BUS_CLK_EN,
    SDH_CMD_GET_INTERNAL_CLK_STABLE,
    SDH_CMD_SET_INTERNAL_CLK_EN,
    SDH_CMD_SET_DATA_TIMEOUT_CNT_VAL, /*!< Data timeout_s = (2^val) / root_clk_freq */

    SDH_CMD_SOFT_RESET_ALL,
    SDH_CMD_SOFT_RESET_CMD_LINE,
    SDH_CMD_SOFT_RESET_DATA_LINE,

    SDH_CMD_SET_DRIVER_TYPE,
    SDH_CMD_SET_SIG_VOL_1V8_EN,
    SDH_CMD_SET_UHS_MODE,
    SDH_CMD_SET_ASYNC_INT_EN,

#ifdef SDH_STD_V3
    SDH_CMD_ACTIVE_CLK_OUT,
    SDH_CMD_FORCE_CLK_OUTPUT,
#endif
};
/**
  * @}
  */

/*! @brief Data structure to initialize the USDHC. */
struct bflb_sdh_config_s {
    uint8_t dma_fifo_th; /*!< FIFO threshold config. */
    uint8_t dma_burst;   /*!< Burst len config. */
    uint8_t power_vol;
};

/*! @brief Defines the data descriptor structure. */
struct bflb_sdh_data_tranfer_s {
    void *address;
    uint32_t length;
    bool int_en;
};

/*! @brief Defines the ADMA2 hardware descriptor structure. */
struct bflb_sdh_adma2_hw_desc_s {
    uint16_t attribute;
    uint16_t length;  /*!< The control and status field. */
    uint32_t address; /*!< The address field. */
};

/*!
 * @brief Card command descriptor.
 *
 * Defines card command-related attribute.
 */
struct bflb_sdh_cmd_cfg_s {
    uint8_t index;     /*!< Command index. */
    uint8_t cmd_type;  /*!< Command type. */
    uint8_t resp_type; /*!< Command response type. */
    uint32_t argument; /*!< Command argument. */
    uint32_t resp[4];  /*!< Response for this command. */
};

/*!
 * @brief Card data descriptor.
 *
 * Defines a structure to contain data-related attribute. The 'enableIgnoreError' is used when upper card
 * driver wants to ignore the error event to read/write all the data and not to stop read/write immediately when an
 * error event happens. For example, bus testing procedure for MMC card.
 */
struct bflb_sdh_data_cfg_s {
    uint8_t data_dir;      /*!< Data transfer direction. */
    uint8_t data_type;     /*!< this is used to distinguish the normal/tuning/boot data. */
    uint8_t auto_cmd_mode; /*!< Enable auto CMD12 or CMD23. */
    uint16_t block_size;   /*!< Block size. */
    uint16_t block_count;  /*!< Block count. */

    /* adma2 cfg */
    bool adma2_hw_desc_raw_mode; /*!< Use ADMA2 hardware descriptors directly instead of converting them from adma_tranfer descriptors. */
    /* Description of the data segment. */
    struct bflb_sdh_data_tranfer_s *adma_tranfer; /*!< The address of adma_tranfer descriptors. */
    uint32_t adma_tranfer_cnt;                    /*!< The number of adma_tranfer descriptors. */
    /* Only ADMA2 is required, Must be accessed using a static non-cacheable address. */
    struct bflb_sdh_adma2_hw_desc_s *adma2_hw_desc; /*!< The address of hardware descriptors. */
    uint32_t adma2_hw_desc_cnt;                     /*!< The number of hardware descriptors. */
};

/*!
 * @brief USDHC capability information.
 *
 * Defines a structure to save the capability information of USDHC.
 */
struct bflb_sdh_capability_s {
    uint32_t sd_version;      /*!< Support SD card/sdio version. */
    uint32_t mmc_version;     /*!< Support EMMC card version. */
    uint32_t block_size_max;  /*!< Maximum block length united as byte. */
    uint32_t block_count_max; /*!< Maximum block count can be set one time. */
    uint64_t capability;      /*!< Capability flags to indicate the support information(@ref _usdhc_capability_flag). */
};

#ifdef __cplusplus
extern "C" {
#endif

/* Internal functions */
int bflb_sdh_adma2_desc_init(struct bflb_device_s *dev, struct bflb_sdh_data_cfg_s *data_cfg);
int bflb_sdh_cmd_cfg(struct bflb_device_s *dev, struct bflb_sdh_cmd_cfg_s *cmd_cfg);
int bflb_sdh_data_cfg(struct bflb_device_s *dev, struct bflb_sdh_data_cfg_s *data_cfg);

/* init */
int bflb_sdh_init(struct bflb_device_s *dev, struct bflb_sdh_config_s *cfg);
/* request transfer */
int bflb_sdh_tranfer_start(struct bflb_device_s *dev, struct bflb_sdh_cmd_cfg_s *cmd_cfg, struct bflb_sdh_data_cfg_s *data_cfg);
/* read resp */
int bflb_sdh_get_resp(struct bflb_device_s *dev, struct bflb_sdh_cmd_cfg_s *cmd_cfg);

/* status enable cfg */
void bflb_sdh_sta_en(struct bflb_device_s *dev, uint32_t sta_bit, bool en);
uint32_t bflb_sdh_sta_en_get(struct bflb_device_s *dev);

/* status interrupt cfg */
void bflb_sdh_sta_int_en(struct bflb_device_s *dev, uint32_t sta_bit, bool en);
uint32_t bflb_sdh_sta_int_en_get(struct bflb_device_s *dev);

/* get/clear status */
uint32_t bflb_sdh_sta_get(struct bflb_device_s *dev);
void bflb_sdh_sta_clr(struct bflb_device_s *dev, uint32_t sta_bit);

/*  */
int bflb_sdh_feature_control(struct bflb_device_s *dev, int cmd, uintptr_t arg);

#ifdef __cplusplus
}
#endif

/**
  * @}
  */

/**
  * @}
  */

#endif
