#ifndef _BFLB_SDIO3_H
#define _BFLB_SDIO3_H

#include "bflb_core.h"

/** @addtogroup LHAL
  * @{
  */

/** @addtogroup SDIO3
  * @{
  */

/** @defgroup SDIO3_FUNC
  * @{
  */
#ifndef SDIO3_FUNC_NUM_MAX
#define SDIO3_FUNC_NUM_MAX (1)
#endif
#ifndef SDIO3_FUNC_QUEUE_NUM_MAX
#define SDIO3_FUNC_QUEUE_NUM_MAX (8)
#endif
#ifndef SDIO3_DMA1_MODE_ENABLE
#define SDIO3_DMA1_MODE_ENABLE (0)
#endif
#ifndef SDIO3_DMA1_PAGA_SIZE
#define SDIO3_DMA1_PAGA_SIZE (32 * 1024) /* min:4K */
#endif
/**
  * @}
  */

/** @defgroup SDIO3_CUSTOM_REG sdio3 custom reg def
  * @{
  */
#define SDIO3_CUSTOM_REG_SIZE                 (512)
/* func queue ctrl area */
#define SDIO3_CUSTOM_REG_FUNC_OFFSET(fn)      (128 * fn) /* func1:128~255, func2:256~383, 128 Byte */
/* func dnld/upld queue info def */
#define SDIO3_CUSTOM_REG_FUNC_DNLD_MAX_SIZE   (0)  /* dnld max size, 2 Byte */
#define SDIO3_CUSTOM_REG_FUNC_DNLD_QUEUE_IN   (2)  /* dnld queue in(push), 1 Byte */
#define SDIO3_CUSTOM_REG_FUNC_DNLD_QUEUE_OUT  (4)  /* dnld queue out(pop), 1 Byte */
#define SDIO3_CUSTOM_REG_FUNC_UPLD_MAX_SIZE   (8)  /* upld max size, 2 Byte */
#define SDIO3_CUSTOM_REG_FUNC_UPLD_QUEUE_IN   (10) /* upld queue in(push), 1 Byte */
#define SDIO3_CUSTOM_REG_FUNC_UPLD_QUEUE_OUT  (12) /* upld queue out(pop), 1 Byte */
#define SDIO3_CUSTOM_REG_FUNC_QUEUE_MAX_DEPTH (14) /* queue max depth, 1 Byte */
/* func other info def */
#define SDIO3_CUSTOM_REG_FUNC_BLOCK_SIZE      (32) /* block size, 2 Byte */
#define SDIO3_CUSTOM_REG_FUNC_CARD_READY      (34) /* card ready, 1 Byte */
#define SDIO3_CUSTOM_REG_FUNC_HOST_READY      (35) /* host ready, 1 Byte */
#define SDIO3_CUSTOM_REG_FUNC_STA_FLAG        (36) /* sdio system status, 1 Byte */
/* func queue elem def */
#define SDIO3_CUSTOM_REG_FUNC_DNLD_QUEUE      (64) /* 64~95, 2*16 Byte */
#define SDIO3_CUSTOM_REG_FUNC_UPLD_QUEUE      (96) /* 96~127, 2*16 Byte */
/* user usage area */
#define SDIO3_CUSTOM_REG_USER_OFFSET          (384) /* user,  384~511, 128 Byte */
#define SDIO3_CUSTOM_REG_USER_SIZE            (128)
/**
  * @}
  */

/** @defgroup SDIO2_STA_FLAG sdio system status and mode type.
  * @{
  */
#define SDIO3_STA_FLAG_SDIO_BOOT              (1 << 7)
#define SDIO3_STA_FLAG_APP_RUN                (1 << 6)
#define SDIO3_STA_FLAG_RD_LEN_COMPRESS_SUP    (1 << 5)
/**
  * @}
  */

/** @defgroup SDIO3_TRAN_STA sdio3 status
  * @{
  */
#define SDIO3_TRAN_STA_IDLE                   (0)
#define SDIO3_TRAN_STA_READY                  (1)
#define SDIO3_TRAN_STA_BUSY_DNLD              (2)
#define SDIO3_TRAN_STA_BUSY_UPLD              (3)
/**
  * @}
  */

/** @defgroup SDIO3_CAP_FLAG sdio3 capability type.
  * @{
  */
#define SDIO3_CAP_FLAG_SIG1V8                 (1 << 0) /*!< 1.8v sig */
#define SDIO3_CAP_FLAG_SAI                    (1 << 1) /*!< Asynchronous Interrupt  */
#define SDIO3_CAP_FLAG_DDR50                  (1 << 2) /*!< DDR50 supported */
#define SDIO3_CAP_FLAG_SDR104                 (1 << 3) /*!< SDR104 supported */
#define SDIO3_CAP_FLAG_SDR50                  (1 << 4) /*!< SDR50 supported*/
#define SDIO3_CAP_FLAG_GAP_INT                (1 << 5) /*!< Block Gap Interrupt */
#define SDIO3_CAP_FLAG_RD_WAIT                (1 << 6) /*!< Read wait */
/**
  * @}
  */

/** @defgroup SDIO3_IRQ_EVENT sdio3 irq callback event type.
  * @{
  */
#define SDIO3_IRQ_EVENT_SOFT_RESET            (1) /*!< soft reset  */
#define SDIO3_IRQ_EVENT_FUNC1_RESET           (2) /*!< Func1 reset  */
#define SDIO3_IRQ_EVENT_FUNC2_RESET           (3) /*!< Func1 reset  */
#define SDIO3_IRQ_EVENT_DNLD_CPL              (4) /*!< Dnld complete  */
#define SDIO3_IRQ_EVENT_UPLD_CPL              (5) /*!< Upld complete  */
#define SDIO3_IRQ_EVENT_SIG1V8_SWITCH         (6) /*!< Switch to 1.8v  */

#define SDIO3_IRQ_EVENT_ERR_CRC               (7)  /*!< CRC error  */
#define SDIO3_IRQ_EVENT_ERR_DNLD_ABORT        (8)  /*!< Host aborts the FunctionX DNLD Operation */
#define SDIO3_IRQ_EVENT_ERR_UPLD_ABORT        (9)  /*!< Host aborts the FunctionX UPLD Operation */
#define SDIO3_IRQ_EVENT_ERR_ADMA              (10) /*!< ADMA error  */
#define SDIO3_IRQ_EVENT_ERR_DNLD_QUEUE        (11) /*!< dnld queue error  */
#define SDIO3_IRQ_EVENT_ERR_UPLD_QUEUE        (12) /*!< upld queue error  */
#define SDIO3_IRQ_EVENT_ERR_UNKNOW            (13) /*!< other error */
/**
  * @}
  */

/** @defgroup SDIO3_CMD sdio3 feature_control cmd.
  * @{
  */
#define SDIO3_CMD_INIT_READY                  (0)  /* card init ready */
#define SDIO3_CMD_GET_CARD_STA                (1)  /* get card status (Resp_1) */
#define SDIO3_CMD_GET_BUS_WIDTH               (2)  /* get bus width */
#define SDIO3_CMD_SET_FUNC_CARD_READY         (3)  /* set func1/func2 card ready sta */
#define SDIO3_CMD_GET_FUNC_HOST_READY         (4)  /* get func1/func2 host ready sta */
#define SDIO3_CMD_GET_FUNC_BLK_SIZE           (5)  /* get func1/func2 block size */
#define SDIO3_CMD_GET_DNLD_MAX_SIZE           (6)  /* get func1/func2 dnld max size */
#define SDIO3_CMD_GET_UPLD_MAX_SIZE           (7)  /* get func1/func2 upld max size */
#define SDIO3_CMD_GET_DNLD_QUEUE_WAIT_NUM     (8)  /*  */
#define SDIO3_CMD_GET_DNLD_QUEUE_AVAILABLE    (9)  /*  */
#define SDIO3_CMD_GET_UPLD_QUEUE_WAIT_NUM     (10)  /*  */
#define SDIO3_CMD_GET_UPLD_QUEUE_AVAILABLE    (11) /*  */
/**
  * @}
  */

/* ADMA descriptor table:
 * |----------------|---------------|-------------|--------------------------|
 * | Address field  |     Length    | Reserved    |         Attribute        |
 * |----------------|---------------|-------------|--------------------------|
 * |63            32|31           16|15         06|05  |04  |03|02 |01 |00   |
 * |----------------|---------------|-------------|----|----|--|---|---|-----|
 * | 32-bit address | 16-bit length | 0000000000  |Act2|Act1| 0|Int|End|Valid|
 * |----------------|---------------|-------------|----|----|--|---|---|-----|
 *
 * ADMA action table:
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
/** @defgroup SDIO3_ADMA_ATTR ADMA2 attribute.
  * @{
  */
#define SDIO2_ADMA_ATTR_VALID                 (1 << 0) /*!< descriptor valid. */
#define SDIO2_ADMA_ATTR_END                   (1 << 1) /*!< descriptor end, complete. */
#define SDIO2_ADMA_ATTR_INT                   (1 << 2) /*!< DMA int. */
#define SDIO2_ADMA_ATTR_ACT_NOP               (0 << 4) /*!< No operation, go to next line */
#define SDIO2_ADMA_ATTR_ACT_RSV               (1 << 4) /*!< No operation, go to next line */
#define SDIO2_ADMA_ATTR_ACT_TRAN              (2 << 4) /*!< Transfer data of one descriptor line. */
#define SDIO2_ADMA_ATTR_ACT_LINK              (3 << 4) /*!< Link to another descriptor. */
/**
  * @}
  */

/** @defgroup SDIO3_ADMA_DESC_LEN_MAX ADMA max len.
  * @{
  */
#define SDIO3_ADMA_DESC_LEN_MAX               (0xffff + 1)
#define SDIO3_ADMA_DESC_LEN_MASK              (0xffff)
/**
  * @}
  */

/*! @brief Defines the ADMA hardware descriptor structure. */
struct bflb_sdio3_adma_hw_desc_s {
    uint16_t attribute;
    uint16_t length;  /*!< The control and status field. */
    uint32_t address; /*!< The address field. */
};

/*! @brief Data structure to initialize the sdio3. */
struct bflb_sdio3_config_s {
    uint8_t func_num;             /*!< function num: 1~2. */
    uint32_t ocr;                 /*!< OCR, [14:0]:res, [23:15]:2.7v~3.6v */
    uint32_t cap_flag;            /*!< capability flag */
    uint32_t func1_dnld_size_max; /* dnld max size */
    uint32_t func2_dnld_size_max; /* dnld max size */
};

/*! @brief Data transfer desc */
typedef struct
{
    uint8_t func;
    uint16_t buff_len;
    uint16_t data_len;
    void *buff;
    void *user_arg;
} bflb_sdio3_trans_desc_t;

typedef void (*bflb_sdio3_irq_cb_t)(void *arg, uint32_t irq_event, bflb_sdio3_trans_desc_t *trans_desc);

#ifdef __cplusplus
extern "C" {
#endif

/* init */
int bflb_sdio3_init(struct bflb_device_s *dev, struct bflb_sdio3_config_s *cfg);
int bflb_sdio3_deinit(struct bflb_device_s *dev);

/* custom reg */
int bflb_sdio3_custom_reg_read(struct bflb_device_s *dev, uint16_t reg_offset, void *buff, uint16_t len);
int bflb_sdio3_custom_reg_write(struct bflb_device_s *dev, uint16_t reg_offset, void *buff, uint16_t len);

/* attach dnld/upld buff */
int bflb_sdio3_dnld_push(struct bflb_device_s *dev, bflb_sdio3_trans_desc_t *trans_desc);
int bflb_sdio3_upld_push(struct bflb_device_s *dev, bflb_sdio3_trans_desc_t *trans_desc);

/* pop dnld/upld queue, can only be used after reset, and before CMD_INIT_READY. */
int bflb_sdio3_dnld_pop(struct bflb_device_s *dev, bflb_sdio3_trans_desc_t *trans_desc, uint8_t func);
int bflb_sdio3_upld_pop(struct bflb_device_s *dev, bflb_sdio3_trans_desc_t *trans_desc, uint8_t func);

/* feature control */
int bflb_sdio3_feature_control(struct bflb_device_s *dev, int cmd, uintptr_t arg);

/* isr callback attach */
int bflb_sdio3_irq_attach(struct bflb_device_s *dev, bflb_sdio3_irq_cb_t irq_event_cb, void *arg);

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
