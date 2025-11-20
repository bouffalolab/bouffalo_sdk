/**
 * @file bflb_sdio2_drv.c
 * @brief SDIO2 driver wrapper implementation
 * @author mlwang
 * @date 2025-07-17
 */

#include "bflb_mtimer.h"
#include "bflb_gpio.h"
#include "bflb_sdio2.h"

#if defined(BL616)
#include "bl616_glb.h"
#elif defined(BL616L)
#include "bl616l_glb.h"
#endif

#include "board.h"

#include "bflb_frame_buff_ctrl.h"
#include "bflb_msg_ctrl.h"

#define DBG_TAG "MAIN"
#include "log.h"

/*****************************************************************************
 * Macro Definitions
 *****************************************************************************/

/* #define SDIO2_DBG(a, ...) LOG_I(a, ##__VA_ARGS__) */
#define SDIO2_DBG(a, ...)

/** SDIO2 test size in bytes */
#define SDIO2_TEST_SIZE (512 * 3)

extern bflb_msg_ctrl_ops_t msg_sdio2_ops;
/*****************************************************************************
 * Global Variables
 *****************************************************************************/

/** Download push and completion counters */
volatile uint32_t dnld_push_cnt, dnld_cpl_cnt;
/** Upload push and completion counters */
volatile uint32_t upld_push_cnt, upld_cpl_cnt;
/** SDIO2 device handle */
struct bflb_device_s *sdio2_hd;

volatile bool sdio2_ready_flag = false;

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

/**
 * @brief SDIO2 download completion interrupt callback
 * @param arg Callback argument
 * @param trans_desc Transaction descriptor
 * @retval None
 */
void sdio2_dnld_irq_callback(void *arg, bflb_sdio2_trans_desc_t *trans_desc)
{
    bflb_msg_ctrl_t *msg_ctrl = (bflb_msg_ctrl_t *)trans_desc->user_arg;
    frame_elem_t *frame_elem = (frame_elem_t *)((uintptr_t)trans_desc->buff - FRAME_BUFF_MSGSTRUCT_OFFSET);

    frame_elem->data_size = trans_desc->data_len;
    bflb_msg_dnld_recv_done_cb(msg_ctrl, frame_elem, true);

    dnld_cpl_cnt++;
}

/**
 * @brief SDIO2 upload completion interrupt callback
 * @param arg Callback argument
 * @param trans_desc Transaction descriptor
 * @retval None
 */
void sdio2_upld_irq_callback(void *arg, bflb_sdio2_trans_desc_t *trans_desc)
{
    bflb_msg_ctrl_t *msg_ctrl = (bflb_msg_ctrl_t *)trans_desc->user_arg;
    frame_elem_t *frame_elem = (frame_elem_t *)((uintptr_t)trans_desc->buff - FRAME_BUFF_MSGSTRUCT_OFFSET);

    bflb_msg_upld_send_done_cb(msg_ctrl, frame_elem, true);

    upld_cpl_cnt++;
}

#ifdef SDIO2_SOFT_RST_INT_SUP
/**
 * @brief SDIO2 soft reset interrupt callback
 * @param arg Callback argument
 * @retval None
 */
void sdio2_reset_irq_callback(void *arg)
{
    bflb_msg_ctrl_t *msg_ctrl;
    frame_elem_t *frame_elem;

    LOG_W("\r\n***** sdio2 reset! *****\r\n");

    sdio2_ready_flag = false;
    bflb_msg_host_reset_cb();

    /*  */
    {
        bflb_sdio2_trans_desc_t trans_desc;

        while (bflb_sdio2_dnld_port_pop(sdio2_hd, &trans_desc) == 0) {
            msg_ctrl = (bflb_msg_ctrl_t *)trans_desc.user_arg;
            frame_elem = (frame_elem_t *)((uintptr_t)trans_desc.buff - FRAME_BUFF_MSGSTRUCT_OFFSET);

            frame_elem->data_size = 0;
            bflb_msg_dnld_recv_done_cb(msg_ctrl, frame_elem, false);
        }

        while (bflb_sdio2_upld_port_pop(sdio2_hd, &trans_desc) == 0) {
            msg_ctrl = (bflb_msg_ctrl_t *)trans_desc.user_arg;
            frame_elem = (frame_elem_t *)((uintptr_t)trans_desc.buff - FRAME_BUFF_MSGSTRUCT_OFFSET);

            bflb_msg_upld_send_done_cb(msg_ctrl, frame_elem, false);
        }
    }

    /* Reset SDIO2 queue control */
    bflb_sdio2_deinit(sdio2_hd);

    /* Re-initialize SDIO2 */
    bflb_sdio2_init(sdio2_hd, SDIO2_TEST_SIZE);
}
#endif

/**
 * @brief SDIO2 interrupt callback dispatcher
 * @param arg Callback argument
 * @param irq_event Interrupt event type
 * @param trans_desc Transaction descriptor
 * @retval None
 */
void sdio2_irq_cb(void *arg, uint32_t irq_event, bflb_sdio2_trans_desc_t *trans_desc)
{
    switch (irq_event) {
        case SDIO2_IRQ_EVENT_DNLD_CPL:
            sdio2_dnld_irq_callback(arg, trans_desc);
            break;

        case SDIO2_IRQ_EVENT_UPLD_CPL:
            sdio2_upld_irq_callback(arg, trans_desc);
            break;

#ifdef SDIO2_SOFT_RST_INT_SUP
        case SDIO2_IRQ_EVENT_SOFT_RESET:
            sdio2_reset_irq_callback(arg);
            break;
#endif

        case SDIO2_IRQ_EVENT_ERR_CRC:
            LOG_E("sdio2 error: CRC ERROR!\r\n");
            break;
        case SDIO2_IRQ_EVENT_ERR_ABORT:
            LOG_E("sdio2 error: ABORT!\r\n");
            break;
        case SDIO2_IRQ_EVENT_ERR_UNKNOW:
            LOG_E("sdio2 error: UNKNOW!\r\n");
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
 * @brief Initialize SDIO2 driver
 * @retval 0 Success
 * @retval -1 Error
 */
bflb_msg_ctrl_ops_t *sdio2_driver_init(void)
{
    LOG_I("sdio2_driver_init\r\n");

    /* Enable SDIO clock */
#if defined(BL616)
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_USB20_SDU);
#elif defined(BL616L)
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_SDU);
#elif defined(BL602)
    /* No clock gating for BL602 */
#endif

    /* Initialize SDIO GPIO */
    board_sdio_gpio_init();

    /* Get SDIO2 device handle */
    sdio2_hd = bflb_device_get_by_name("sdio2");
    if (sdio2_hd == NULL) {
        LOG_E("get device failed\r\n");
        return NULL;
    }

    sdio2_ready_flag = false;

    /* Initialize SDIO2 */
    bflb_sdio2_init(sdio2_hd, SDIO2_TEST_SIZE);

    /* Attach interrupt callback */
    bflb_sdio2_irq_attach(sdio2_hd, sdio2_irq_cb, NULL);

    LOG_I("sdio2 init done, wait host ready...\r\n");

    return &msg_sdio2_ops;
}

/**
 * @brief Check if SDIO2 is ready
 * @retval 0 SDIO2 is ready
 * @retval -1 SDIO2 is not ready
 */
int sdio2_is_ready(void)
{
    if (bflb_sdio2_feature_control(sdio2_hd, SDIO2_CMD_GET_FUNC_HOST_READY, 0) == 0) {
        sdio2_ready_flag = false;
        return -1;
    }

    LOG_W("sdio2 host ready!\r\n");

    /* Display bus configuration */
    LOG_I("sdio2 bus width: %d\r\n", bflb_sdio2_feature_control(sdio2_hd, SDIO2_CMD_GET_BUS_WIDTH, 0));
    LOG_I("sdio2 block size: %d\r\n", bflb_sdio2_feature_control(sdio2_hd, SDIO2_CMD_GET_FUNC_BLK_SIZE, 0));
    LOG_I("sdio dnld max size %d\r\n", bflb_sdio2_feature_control(sdio2_hd, SDIO2_CMD_GET_DNLD_MAX_SIZE, 0));
    LOG_I("sdio upld max size %d\r\n", bflb_sdio2_feature_control(sdio2_hd, SDIO2_CMD_GET_UPLD_MAX_SIZE, 0));

    sdio2_ready_flag = true;
    return 0;
}

/**
 * @brief Check if SDIO2 download is busy
 * @retval 0 Download is not busy
 * @retval 1 Download is busy
 */
int sdio2_dnld_is_busy(void)
{
    if (sdio2_ready_flag == false) {
        LOG_E("sdio2 not ready\r\n");
        return -1;
    }

    if (bflb_sdio2_feature_control(sdio2_hd, SDIO2_CMD_GET_DNLD_QUEUE_AVAILABLE, 0) > 0) {
        return 0;
    } else {
        return 1;
    }
}

/**
 * @brief Start SDIO2 download operation
 * @param frame_elem Frame element to download
 * @param arg User argument
 * @retval 0 Success
 * @retval -1 Error
 */
int sdio2_dnld_start(frame_elem_t *frame_elem, void *arg)
{
    int ret;

    if (sdio2_ready_flag == false) {
        LOG_E("sdio2 not ready\r\n");
        return -1;
    }

    bflb_sdio2_trans_desc_t dnld_desc = {
        .buff = frame_elem->buff_addr,
        .buff_len = frame_elem->buff_size,
        .data_len = 0,
        .user_arg = arg,
    };

    ret = bflb_sdio2_dnld_port_push(sdio2_hd, &dnld_desc);
    if (ret != 0) {
        LOG_E("sdio2 dnld push failed, ret: %d\r\n", ret);
        return ret;
    }

    dnld_push_cnt++;
    SDIO2_DBG("sdio2 dnld push, buff: %p, size: %d\r\n", frame_elem->buff_addr, frame_elem->data_size);

    return 0;
}

/**
 * @brief Check if SDIO2 upload is busy
 * @retval 0 Upload is not busy
 * @retval 1 Upload is busy
 */
int sdio2_upld_is_busy(void)
{
    if (sdio2_ready_flag == false) {
        LOG_E("sdio2 not ready\r\n");
        return -1;
    }

    if (bflb_sdio2_feature_control(sdio2_hd, SDIO2_CMD_GET_UPLD_QUEUE_AVAILABLE, 0) > 0) {
        return 0;
    } else {
        return 1;
    }
}

/**
 * @brief Start SDIO2 upload operation
 * @param frame_elem Frame element to upload
 * @param arg User argument
 * @retval 0 Success
 * @retval -1 Error
 */
int sdio2_upld_start(frame_elem_t *frame_elem, void *arg)
{
    int ret;
    uint16_t blk_size;

    if (sdio2_ready_flag == false) {
        LOG_E("sdio2 not ready\r\n");
        return -1;
    }

    bflb_sdio2_trans_desc_t upld_desc = {
        .buff = frame_elem->buff_addr,
        .buff_len = frame_elem->buff_size,
        .data_len = frame_elem->data_size,
        .user_arg = arg,
    };

    blk_size = (uint16_t)bflb_sdio2_feature_control(sdio2_hd, SDIO2_CMD_GET_FUNC_BLK_SIZE, 0);
    if (blk_size == 0) {
        blk_size = 512; /* Default block size */
        LOG_E("sdio2 upld blk_size is 0, set to default 512\r\n");
    }

    if (upld_desc.data_len >= 64) {
        upld_desc.data_len = (upld_desc.data_len + blk_size - 1) & ~(blk_size - 1); /* Align up to blk_size */
    }

    ret = bflb_sdio2_upld_port_push(sdio2_hd, &upld_desc);
    if (ret != 0) {
        LOG_E("sdio2 upld push failed, ret: %d\r\n", ret);
        return ret;
    }

    upld_push_cnt++;
    SDIO2_DBG("sdio2 upld push, buff: %p, size: %d\r\n", frame_elem->buff_addr, frame_elem->data_size);

    return 0;
}

/* SDIO2 ops for message controller */
bflb_msg_ctrl_ops_t msg_sdio2_ops = {
    .sdio_is_ready = sdio2_is_ready,
    .sdio_dnld_is_busy = sdio2_dnld_is_busy,
    .sdio_upld_is_busy = sdio2_upld_is_busy,
    .sdio_dnld_start = sdio2_dnld_start,
    .sdio_upld_start = sdio2_upld_start,
    .sdio_feature_control = bflb_sdio2_feature_control,
    .sdio_name = "SDIO2",
};
