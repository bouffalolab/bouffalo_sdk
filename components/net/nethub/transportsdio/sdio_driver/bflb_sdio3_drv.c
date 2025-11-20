/**
 * @file bflb_sdio2_drv.c
 * @brief SDIO2 driver wrapper implementation
 * @author mlwang
 * @date 2025-07-17
 */

#include "bflb_mtimer.h"
#include "bflb_gpio.h"
#include "bflb_sdio3.h"

#if defined(BL616)
#include "bl616_glb.h"
#elif defined(BL616L)
#include "bl616l_glb.h"
#elif defined(BL616D)
#include "bl616d_glb.h"
#endif

#include "board.h"

#include "bflb_frame_buff_ctrl.h"
#include "bflb_msg_ctrl.h"

#define DBG_TAG "MAIN"
#include "log.h"

#define SDIO3_FN1_TEST_SIZE (512 * 3)
#define SDIO3_FN2_TEST_SIZE (512 * 3)

#ifndef SDIO3_FUNC_NUM_MAX
#define SDIO3_FUNC_NUM_MAX (1)
#endif

#define SDIO_TEST_FUNC_NUM (SDIO3_FUNC_NUM_MAX)

/*****************************************************************************
 * Macro Definitions
 *****************************************************************************/

/* #define SDIO3_DBG(a, ...) LOG_I(a, ##__VA_ARGS__) */
#define SDIO3_DBG(a, ...)

/** SDIO3 test size in bytes */
#define SDIO3_TEST_SIZE (512 * 3)

extern bflb_msg_ctrl_ops_t msg_sdio3_ops;

/*****************************************************************************
 * Global Variables
 *****************************************************************************/

/** Download push and completion counters */
volatile uint32_t dnld_push_cnt, dnld_cpl_cnt;
/** Upload push and completion counters */
volatile uint32_t upld_push_cnt, upld_cpl_cnt;
/** SDIO2 device handle */
struct bflb_device_s *sdio3_hd;

volatile bool sdio3_ready_flag = false;

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

/**
 * @brief SDIO3 download completion interrupt callback
 * @param arg Callback argument
 * @param trans_desc Transaction descriptor
 * @retval None
 */
void sdio3_dnld_irq_callback(void *arg, bflb_sdio3_trans_desc_t *trans_desc)
{
    bflb_msg_ctrl_t *msg_ctrl = (bflb_msg_ctrl_t *)trans_desc->user_arg;
    frame_elem_t *frame_elem = (frame_elem_t *)((uintptr_t)trans_desc->buff - FRAME_BUFF_MSGSTRUCT_OFFSET);

    frame_elem->data_size = trans_desc->data_len;
    bflb_msg_dnld_recv_done_cb(msg_ctrl, frame_elem, true);

    dnld_cpl_cnt++;
}

/**
 * @brief SDIO3 upload completion interrupt callback
 * @param arg Callback argument
 * @param trans_desc Transaction descriptor
 * @retval None
 */
void sdio3_upld_irq_callback(void *arg, bflb_sdio3_trans_desc_t *trans_desc)
{
    extern uint32_t transportsdio_get_payload_offset(uint8_t *payload);
    bflb_msg_ctrl_t *msg_ctrl;
    int offset;
    frame_elem_t *frame_elem;

    msg_ctrl = (bflb_msg_ctrl_t *)trans_desc->user_arg;
    offset = transportsdio_get_payload_offset((uint8_t *)trans_desc->buff+FRAME_BUFF_MSGSTRUCT);

    // offset is payload not msg addr, and trans_desc->buff is msg, so need add FRAME_BUFF_MSGSTRUCT after - offset
    frame_elem = (frame_elem_t *)((uintptr_t)trans_desc->buff - offset + FRAME_BUFF_MSGSTRUCT);

    //printf("%s elem:%p offset:%d, msg:%p\r\n",
    //        __func__,
    //        frame_elem,
    //        offset,
    //        trans_desc->buff);

    bflb_msg_upld_send_done_cb(msg_ctrl, frame_elem, true);

    upld_cpl_cnt++;
}

#ifdef SDIO3_SOFT_RST_INT_SUP
/**
 * @brief SDIO3 soft reset interrupt callback
 * @param arg Callback argument
 * @retval None
 */
void sdio3_reset_irq_callback(void *arg)
{
    bflb_msg_ctrl_t *msg_ctrl;
    frame_elem_t *frame_elem;

    LOG_W("\r\n***** sdio3 reset! *****\r\n");

    sdio3_ready_flag = false;
    bflb_msg_host_reset_cb();

    /*  */
    {
        bflb_sdio3_trans_desc_t trans_desc;

        while (bflb_sdio3_dnld_port_pop(sdio3_hd, &trans_desc) == 0) {
            msg_ctrl = (bflb_msg_ctrl_t *)trans_desc.user_arg;
            frame_elem = (frame_elem_t *)((uintptr_t)trans_desc.buff - FRAME_BUFF_MSGSTRUCT_OFFSET);

            frame_elem->data_size = 0;
            bflb_msg_dnld_recv_done_cb(msg_ctrl, frame_elem, false);
        }

        while (bflb_sdio3_upld_port_pop(sdio3_hd, &trans_desc) == 0) {
            msg_ctrl = (bflb_msg_ctrl_t *)trans_desc.user_arg;
            frame_elem = (frame_elem_t *)((uintptr_t)trans_desc.buff - FRAME_BUFF_MSGSTRUCT_OFFSET);

            bflb_msg_upld_send_done_cb(msg_ctrl, frame_elem, false);
        }
    }

    /* Reset SDIO3 queue control */
    bflb_sdio3_deinit(sdio3_hd);

    /* Re-initialize SDIO3 */
    bflb_sdio3_init(sdio3_hd, SDIO3_TEST_SIZE);
}
#endif

/**
 * @brief SDIO3 interrupt callback dispatcher
 * @param arg Callback argument
 * @param irq_event Interrupt event type
 * @param trans_desc Transaction descriptor
 * @retval None
 */
void sdio3_irq_cb(void *arg, uint32_t irq_event, bflb_sdio3_trans_desc_t *trans_desc)
{
    switch (irq_event) {
        case SDIO3_IRQ_EVENT_DNLD_CPL:
            sdio3_dnld_irq_callback(arg, trans_desc);
            break;

        case SDIO3_IRQ_EVENT_UPLD_CPL:
            sdio3_upld_irq_callback(arg, trans_desc);
            break;

#ifdef SDIO3_SOFT_RST_INT_SUP
        case SDIO3_IRQ_EVENT_SOFT_RESET:
            sdio3_reset_irq_callback(arg);
            break;
#endif

        case SDIO3_IRQ_EVENT_ERR_CRC:
            LOG_E("sdio3 error: CRC ERROR!\r\n");
            break;
        case SDIO3_IRQ_EVENT_ERR_DNLD_ABORT:
            LOG_E("sdio3 error: DNLD_ABORT!\r\n");
            break;
        case SDIO3_IRQ_EVENT_ERR_UPLD_ABORT:
            LOG_E("sdio3 error: UPLD_ABORT!\r\n");
            break;
        case SDIO3_IRQ_EVENT_ERR_UNKNOWN:
            LOG_E("sdio3 error: UNKNOW!\r\n");
            break;
        default:
            LOG_E("irq_event %d\r\n", irq_event);
            break;
    }
}

/*****************************************************************************
 * Public Functions
 *****************************************************************************/

/**
 * @brief Initialize SDIO3 driver
 * @retval 0 Success
 * @retval -1 Error
 */
bflb_msg_ctrl_ops_t *sdio3_driver_init(void)
{
    LOG_I("sdio3_driver_init\r\n");

    /* Enable SDIO clock */
#if defined(BL628)
    bflb_glb_per_clock_ungate(GLB_AHB_CLOCK_SDU);
    bflb_glb_per_clock_ungate(GLB_AHB_CLOCK_USB20_SDU);
    bflb_glb_per_clock_ungate(GLB_AHB_CLOCK_USB_SDIO);
#endif

    /* Initialize SDIO GPIO */
    board_sdio_gpio_init();

    /* Get SDIO3 device handle */
    sdio3_hd = bflb_device_get_by_name("sdio3");
    if (sdio3_hd == NULL) {
        LOG_E("get device failed\r\n");
        return NULL;
    }

    sdio3_ready_flag = false;
    struct bflb_sdio3_config_s cfg = {
        .func_num = SDIO_TEST_FUNC_NUM,                            /*!< function num: 1~2. */
        .ocr = 0xff0000,                                           /*!< OCR, [14:0]:res, [23:15]:2.7v~3.6v */
        .cap_flag = SDIO3_CAP_FLAG_SDR50 | SDIO3_CAP_FLAG_RD_WAIT, /*!< capability flag */
        .func1_dnld_size_max = SDIO3_FN1_TEST_SIZE,                /* dnld max size */
        .func2_dnld_size_max = SDIO3_FN2_TEST_SIZE,                /* dnld max size */
    };

    /* Initialize SDIO3 */
    bflb_sdio3_init(sdio3_hd, &cfg);

    /* Attach interrupt callback */
    bflb_sdio3_irq_attach(sdio3_hd, sdio3_irq_cb, NULL);

    bflb_sdio3_feature_control(sdio3_hd, SDIO3_CMD_INIT_READY, 0);

    /* card func ready */
    bflb_sdio3_feature_control(sdio3_hd, SDIO3_CMD_SET_FUNC_CARD_READY, 1);

    LOG_I("sdio3 init done, max queue: %d, wait host ready...\r\n", SDIO3_FUNC_QUEUE_NUM_MAX);

    return &msg_sdio3_ops;
}

/**
 * @brief Check if SDIO3 is ready
 * @retval 0 SDIO3 is ready
 * @retval -1 SDIO3 is not ready
 */
int sdio3_is_ready(void)
{
    if (bflb_sdio3_feature_control(sdio3_hd, SDIO3_CMD_GET_FUNC_HOST_READY, 1) == 0) {
        sdio3_ready_flag = false;
        return -1;
    }

    LOG_W("sdio3 host ready!\r\n");

    /* Display bus configuration */
    LOG_I("sdio3 bus width: %d\r\n", bflb_sdio3_feature_control(sdio3_hd, SDIO3_CMD_GET_BUS_WIDTH, 1));
    LOG_I("sdio3 block size: %d\r\n", bflb_sdio3_feature_control(sdio3_hd, SDIO3_CMD_GET_FUNC_BLK_SIZE, 1));
    LOG_I("sdio dnld max size %d\r\n", bflb_sdio3_feature_control(sdio3_hd, SDIO3_CMD_GET_DNLD_MAX_SIZE, 1));
    LOG_I("sdio upld max size %d\r\n", bflb_sdio3_feature_control(sdio3_hd, SDIO3_CMD_GET_UPLD_MAX_SIZE, 1));

    sdio3_ready_flag = true;
    return 0;
}

/**
 * @brief Check if SDIO3 download is busy
 * @retval 0 Download is not busy
 * @retval 1 Download is busy
 */
int sdio3_dnld_is_busy(void)
{
    if (sdio3_ready_flag == false) {
        LOG_E("sdio3 not ready\r\n");
        return -1;
    }

    if (bflb_sdio3_feature_control(sdio3_hd, SDIO3_CMD_GET_DNLD_QUEUE_AVAILABLE, 1) > 0) {
        return 0;
    } else {
        return 1;
    }
}

/**
 * @brief Start SDIO3 download operation
 * @param frame_elem Frame element to download
 * @param arg User argument
 * @retval 0 Success
 * @retval -1 Error
 */
int sdio3_dnld_start(frame_elem_t *frame_elem, void *arg)
{
    int ret;

    if (sdio3_ready_flag == false) {
        LOG_E("sdio3 not ready\r\n");
        return -1;
    }

    bflb_sdio3_trans_desc_t dnld_desc = {
        .func = 1,
        .buff = frame_elem->buff_addr,
        .buff_len = frame_elem->buff_size,
        .data_len = 0,
        .user_arg = arg,
    };

    ret = bflb_sdio3_dnld_push(sdio3_hd, &dnld_desc);
    if (ret != 0) {
        LOG_E("sdio3 dnld push failed, ret: %d\r\n", ret);
        return ret;
    }

    dnld_push_cnt++;
    SDIO3_DBG("sdio3 dnld push, buff: %p, size: %d\r\n", frame_elem->buff_addr, frame_elem->data_size);

    return 0;
}

/**
 * @brief Check if SDIO3 upload is busy
 * @retval 0 Upload is not busy
 * @retval 1 Upload is busy
 */
int sdio3_upld_is_busy(void)
{
    if (sdio3_ready_flag == false) {
        LOG_E("sdio3 not ready\r\n");
        return -1;
    }

    if (bflb_sdio3_feature_control(sdio3_hd, SDIO3_CMD_GET_UPLD_QUEUE_AVAILABLE, 1) > 0) {
        return 0;
    } else {
        return 1;
    }
}

/**
 * @brief Start SDIO3 upload operation
 * @param frame_elem Frame element to upload
 * @param arg User argument
 * @retval 0 Success
 * @retval -1 Error
 */
int sdio3_upld_start(frame_elem_t *frame_elem, void *arg)
{
    int ret;
    uint16_t blk_size;

    if (sdio3_ready_flag == false) {
        LOG_E("sdio3 not ready\r\n");
        return -1;
    }

    bflb_sdio3_trans_desc_t upld_desc = {
        .func = 1,
        .buff = frame_elem->buff_addr,
        .buff_len = frame_elem->buff_size,
        .data_len = frame_elem->data_size,
        .user_arg = arg,
    };

    blk_size = (uint16_t)bflb_sdio3_feature_control(sdio3_hd, SDIO3_CMD_GET_FUNC_BLK_SIZE, 1);
    if (blk_size == 0) {
        blk_size = 512; /* Default block size */
        LOG_E("sdio3 upld blk_size is 0, set to default 512\r\n");
    }

    if (upld_desc.data_len >= 64) {
        upld_desc.data_len = (upld_desc.data_len + blk_size - 1) & ~(blk_size - 1); /* Align up to blk_size */
    }

    ret = bflb_sdio3_upld_push(sdio3_hd, &upld_desc);
    if (ret != 0) {
        LOG_E("sdio3 upld push failed, ret: %d\r\n", ret);
        return ret;
    }

    upld_push_cnt++;
    SDIO3_DBG("sdio3 upld push, buff: %p, size: %d\r\n", frame_elem->buff_addr, frame_elem->data_size);

    return 0;
}

/* SDIO3 ops for message controller */
bflb_msg_ctrl_ops_t msg_sdio3_ops = {
    .sdio_is_ready = sdio3_is_ready,
    .sdio_dnld_is_busy = sdio3_dnld_is_busy,
    .sdio_upld_is_busy = sdio3_upld_is_busy,
    .sdio_dnld_start = sdio3_dnld_start,
    .sdio_upld_start = sdio3_upld_start,
    .sdio_feature_control = bflb_sdio3_feature_control,
    .sdio_name = "SDIO3",
};
