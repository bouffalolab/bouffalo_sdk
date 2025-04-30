#include "usbh_uvc_uac.h"

#include <compiler/compiler_gcc.h>
#include <compiler/compiler_ld.h>

#include "bflb_mtimer.h"

#include "frame_queue_ctrl.h"
#include "frame_queue.h"

#if (0)
#define UAC_INFO(a, ...) printf("[UAC info]:" a, ##__VA_ARGS__)
#else
#define UAC_INFO(a, ...)
#endif

#define UAC_ERR(a, ...) printf("[UAC err]:" a, ##__VA_ARGS__)

struct bflb_device_s *usbh_audio_get_dma_ch(void)
{
    return bflb_device_get_by_name("dma0_ch0");
}

static audio_in_frame_t auadc_frame = { 0 };

ATTR_TCM_SECTION void usbh_audio_one_frame_callback(uint32_t data_size)
{
    int ret;

    if (g_uac_frame_buff != NULL) {
        /* check */
        if (g_uac_frame_buff != auadc_frame.elem_base.frame_addr) {
            UAC_ERR("uac unkown pbuff\r\n");
            while (1) {
            };
        }

        if (data_size > auadc_frame.elem_base.frame_size) {
            UAC_ERR("uac size over: %d->%d\r\n", auadc_frame.elem_base.frame_size, data_size);
            while (1) {
            };
        }

        if (data_size) {
            /* update date size */
            auadc_frame.data_size = data_size;
#if (AUDIO_SAMPLING_FREQ == 48000)
            /* auadc frame push to rate_convert */
            audio_play_rate_convert_push(&auadc_frame);
#elif (AUDIO_SAMPLING_FREQ == 16000)
            /* auadc frame push to queue */
            for (uint8_t i = 0; i < OUTPUT_QUEUE_NUM_MAX; i++) {
                ret = frame_queue_push(g_audio_in_frame_ctrl, (frame_elem_t *)&auadc_frame, i, 0);
                // UAC_INFO("uav[%d] %d\r\n", i, ret);
            }

            /* unlock */
            frame_queue_free_unlock(g_audio_in_frame_ctrl, (frame_elem_t *)&auadc_frame);
#else
#error "sample rate error"
#endif
        }
    }

    /* alloc new */
    ret = frame_queue_alloc_lock(g_audio_in_frame_ctrl, (frame_elem_t *)&auadc_frame, 0);

    if (ret < 0) {
        UAC_INFO("alloc failed\r\n");
        g_uac_frame_buff = NULL;
    } else {
        UAC_INFO("alloc id %d, addr 0x%08X\r\n", auadc_frame.elem_base.frame_id, (uint32_t)auadc_frame.elem_base.frame_addr);
        g_uac_frame_buff = auadc_frame.elem_base.frame_addr;
    }
}

extern void usbh_audio_run_callback()
{
    /* uac init */
    printf("Starting UAC\r\n");
    usbh_audio_test();
}

extern void usbh_audio_stop_callback()
{
    if (g_uac_frame_buff != NULL) {
        frame_queue_free_unlock(g_audio_in_frame_ctrl, (frame_elem_t *)&auadc_frame);
        g_uac_frame_buff = NULL;
    }
    printf("Stop UAC...\r\n");
}
