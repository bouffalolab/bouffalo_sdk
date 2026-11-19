#include <stdio.h>

#include "bflb_core.h"
#include "bflb_irq.h"

#include <FreeRTOS.h>
#include <task.h>

#include "default/display_default.h"
#include "display_os.h"

#include "soln_fbq.h"

#include "soln_rgb_disp.h"
#include "soln_rgb_draw.h"

#ifdef CONFIG_SOLN_DISP_RGB_LOG_LEVEL
#undef CONFIG_LOG_LEVEL
#define CONFIG_LOG_LEVEL CONFIG_SOLN_DISP_RGB_LOG_LEVEL
#endif
#define DBG_TAG "SOLN_RGB"
#include "log.h"

#if !defined(CONFIG_DISPLAY_DEFAULT_TYPE_VIDEO) || !defined(CONFIG_DISPLAY_DEFAULT_VIDEO_FORMAT_XRGB8888)
#error "Solution RGB display needs a default VIDEO screen with XRGB8888 framebuffers"
#endif

#ifdef CONFIG_PSRAM
#define RGB_FB_ATTR __ALIGNED(64) ATTR_NOINIT_PSRAM_SECTION
#else
#define RGB_FB_ATTR __ALIGNED(64) ATTR_NOCACHE_NOINIT_RAM_SECTION
#endif

#define RGB_FB_BYTES ((size_t)DISPLAY_DEFAULT_WIDTH * DISPLAY_DEFAULT_HEIGHT * sizeof(uint32_t))
#define RGB_FB_WORDS (((RGB_FB_BYTES + 63u) / 64u) * 16u)

#ifndef RGB_DISP_3_BUFFER_EN
#define RGB_DISP_3_BUFFER_EN 1
#endif
#define RGB_DISP_BUFFER_COUNT (RGB_DISP_3_BUFFER_EN ? 3u : 2u)

static fbq_ctrl_t *img_raw_frame_ctrl_disp = NULL;
static uint16_t img_raw_queue_id_disp;
static uint16_t img_raw_queue_depth_disp;

static TaskHandle_t rgb_disp_handle;
static display_os_sem_t rgb_disp_semaphore;
static display_t *rgb_display;
static display_info_t rgb_display_info;
static bool rgb_disp_resources;
static volatile bool flip_outstanding;
static display_event_type_t expected_event;

/* Each slot, not just the array base, starts on a cache-line boundary. */
static uint32_t RGB_FB_ATTR rgb_screen_buf[RGB_DISP_BUFFER_COUNT][RGB_FB_WORDS];
static uint8_t rgb_buf_idx = 0;

static uint32_t s_rgb_disp_total_frame_count = 0;

uint32_t soln_rgb_disp_get_total_frame_count(void)
{
    uintptr_t irq_flags = bflb_irq_save();
    uint32_t frame_count = s_rgb_disp_total_frame_count;
    bflb_irq_restore(irq_flags);

    return frame_count;
}

void soln_rgb_disp_clear_total_frame_count(void)
{
    uintptr_t irq_flags = bflb_irq_save();
    s_rgb_disp_total_frame_count = 0;
    bflb_irq_restore(irq_flags);
}

static void rgb_disp_record_frame(void)
{
    uintptr_t irq_flags = bflb_irq_save();
    s_rgb_disp_total_frame_count++;
    bflb_irq_restore(irq_flags);
}

static void rgb_screen_frame_callback(display_t *display, const display_event_t *event, void *ctx)
{
    (void)display;
    (void)ctx;
    if (flip_outstanding && event->type == expected_event) {
        flip_outstanding = false;
        if (event->type == DISPLAY_EVT_FB_RETIRED) {
            rgb_disp_record_frame();
        }
        display_os_sem_give(&rgb_disp_semaphore);
    }
}

static int rgb_disp_flush_wait(void)
{
    return display_os_sem_take(&rgb_disp_semaphore, DISPLAY_OS_WAIT_FOREVER);
}

static int rgb_disp_submit(unsigned index, display_event_type_t event)
{
    display_fb_t fb = {
        .data = rgb_screen_buf[index],
        .stride = (uint32_t)rgb_display_info.width * 4u,
        .format = DISPLAY_FMT_XRGB8888,
    };
    /* Arm before flip: completion may run before display_flip() returns. */
    uintptr_t flags = bflb_irq_save();
    expected_event = event;
    flip_outstanding = true;
    bflb_irq_restore(flags);
    int ret = display_flip(rgb_display, &fb);
    if (ret) {
        flags = bflb_irq_save();
        flip_outstanding = false; /* rejection has no completion event */
        bflb_irq_restore(flags);
    #if RGB_DISP_3_BUFFER_EN
        display_os_sem_give(&rgb_disp_semaphore); /* restore the idle token */
    #endif
    } else {
        rgb_buf_idx = index; /* only accepted frames advance the buffer cycle */
    }
    return ret;
}

static ATTR_TCM_SECTION void rgb_disp_lcd_task(void *pvParameters)
{
    int ret;
    uint32_t *disp_fb;
    fbq_elem_t *disp_src_frame;

    (void)pvParameters;

    vTaskDelay(100);

    /* drop the first frame */
    ret = fbq_pop(img_raw_frame_ctrl_disp, &disp_src_frame, img_raw_queue_id_disp, 0);
    if (ret == FBQ_OK) {
        fbq_free(disp_src_frame);
    }

    while (1) {
        /* get frame */
        ret = fbq_pop(img_raw_frame_ctrl_disp, &disp_src_frame, img_raw_queue_id_disp, 1000);
        if (ret != FBQ_OK) {
            LOG_W("img_raw pop timeout: %d, continue wait...\r\n", ret);
            continue;
        }
        LOG_D("img_raw pop: id %d, addr 0x%08X,\r\n", disp_src_frame->id, disp_src_frame->data);

        unsigned next = (rgb_buf_idx + 1u) % RGB_DISP_BUFFER_COUNT;
        disp_fb = rgb_screen_buf[next];

        if (disp_src_frame->ext_type != SOLUTION_FBQ_EXT_IMG_RAW ||
            disp_src_frame->ext_size < sizeof(soln_fbq_img_raw_ext_t)) {
            LOG_E("invalid RAW frame extension\r\n");
            fbq_free(disp_src_frame);
            continue;
        }
        const soln_fbq_img_raw_ext_t *raw_ext = soln_fbq_img_raw_ext_const(disp_src_frame);
        unsigned width = (unsigned)raw_ext->x_end - raw_ext->x_start + 1u;
        unsigned height = (unsigned)raw_ext->y_end - raw_ext->y_start + 1u;
        unsigned bpp = raw_ext->format == IMG_RAW_FRAME_FORMAT_RGB888 ? 3u : 4u;
        if (!disp_src_frame->data || raw_ext->x_end < raw_ext->x_start || raw_ext->y_end < raw_ext->y_start ||
            width != 640u || height != 480u || rgb_display_info.width != 480u || rgb_display_info.height != 272u ||
            (raw_ext->format != IMG_RAW_FRAME_FORMAT_RGB888 && raw_ext->format != IMG_RAW_FRAME_FORMAT_NRGB8888) ||
            disp_src_frame->size > disp_src_frame->capacity ||
            disp_src_frame->size < (size_t)640u * 480u * bpp ||
            (bpp == 4u && (uintptr_t)disp_src_frame->data % sizeof(uint32_t))) {
            LOG_E("unsupported or invalid frame: %ux%u, format %d\r\n", width, height, raw_ext->format);
            fbq_free(disp_src_frame);
            continue;
        }
        if (raw_ext->format == IMG_RAW_FRAME_FORMAT_NRGB8888) {
            soln_nrgb888_640x480_to_nrgb8888_480x272(disp_src_frame->data, disp_fb);
        } else {
            soln_rgb888_640x480_to_nrgb8888_480x272(disp_src_frame->data, disp_fb);
        }

        /* free source frame after draw/switch handling */
        fbq_free(disp_src_frame);

        /* draw overlay on the frame buffer */
        ret = soln_rgb_draw_overlay(disp_fb, rgb_display_info.width, rgb_display_info.height);
        if (!ret) {
#if RGB_DISP_3_BUFFER_EN
            /* Three buffers: convert -> wait for previous flip -> submit. */
            ret = rgb_disp_flush_wait();
            if (!ret) {
                ret = rgb_disp_submit(next, DISPLAY_EVT_FB_RETIRED);
            }
#else
            /* Two buffers: convert -> submit -> wait before reusing the old frame. */
            ret = rgb_disp_submit(next, DISPLAY_EVT_FB_RETIRED);
            if (!ret) {
                ret = rgb_disp_flush_wait();
                if (ret) {
                    /* Do not reuse either buffer without confirmed retirement. */
                    LOG_E("frame completion wait failed: %d\r\n", ret);
                    fbq_output_close(img_raw_frame_ctrl_disp, img_raw_queue_id_disp);
                    vTaskDelete(NULL);
                    return;
                }
            }
#endif
        }
        if (ret) {
            LOG_E("frame submission failed: %d\r\n", ret);
        }
    }
}

int soln_rgb_disp_task_init(void)
{
    int ret;
    uint32_t img_raw_accept_mask;

    LOG_I("rgb_disp_init\r\n");

    if (rgb_disp_resources) {
        return -EBUSY;
    }
    ret = display_os_sem_init(&rgb_disp_semaphore);
    if (ret) {
        return ret;
    }
    /* Completion semaphore starts empty; only a matching flip completes it. */
    ret = rgb_disp_flush_wait();
    if (ret) {
        display_os_sem_deinit(&rgb_disp_semaphore);
        return ret;
    }
    rgb_disp_resources = true;
    rgb_display = display_default_get();
    if (!rgb_display) {
        ret = -ENODEV;
        goto fail_sem;
    }
    ret = display_init(rgb_display);
    if (ret) {
        goto fail_display;
    }
    ret = display_get_info(rgb_display, &rgb_display_info);
    if (ret) {
        goto fail_display;
    }
    /* Keep the legacy converter fixed; other video sizes need a separate port. */
    if (rgb_display_info.width != 480u || rgb_display_info.height != 272u) {
        ret = -ENOTSUP;
        goto fail_display;
    }
    if (rgb_display_info.state != DISPLAY_STATE_READY || rgb_display_info.kind != DISPLAY_KIND_FB ||
        rgb_display_info.format != DISPLAY_FMT_XRGB8888 ||
        rgb_display_info.width != DISPLAY_DEFAULT_WIDTH || rgb_display_info.height != DISPLAY_DEFAULT_HEIGHT ||
        !(rgb_display_info.scanout.flags & DISPLAY_SCANOUT_CAP_FLIP) ||
        !(rgb_display_info.scanout.format_mask & DISPLAY_FMT_BIT(DISPLAY_FMT_XRGB8888)) ||
        (rgb_display_info.scanout.addr_align &&
         (uintptr_t)rgb_screen_buf % rgb_display_info.scanout.addr_align) ||
        (rgb_display_info.scanout.addr_align &&
         sizeof(rgb_screen_buf[0]) % rgb_display_info.scanout.addr_align) ||
        (rgb_display_info.scanout.size_align && RGB_FB_BYTES % rgb_display_info.scanout.size_align)) {
        ret = -ENOTSUP;
        goto fail_display;
    }
    flip_outstanding = false;
    ret = display_set_callback(rgb_display, rgb_screen_frame_callback, NULL);
    if (ret) {
        goto fail_display;
    }
    for (size_t i = 0; i < RGB_FB_BYTES / sizeof(uint32_t); i++) {
        rgb_screen_buf[0][i] = 0x00ff0000; /* retain the old red startup frame */
    }
    ret = rgb_disp_submit(0, DISPLAY_EVT_FB_FIRST);
    if (ret) {
        goto fail_display;
    }
    ret = rgb_disp_flush_wait();
    if (ret) {
        goto fail_display;
    }
#if RGB_DISP_3_BUFFER_EN
    /* The first conversion has no previous flip to wait for. */
    display_os_sem_give(&rgb_disp_semaphore);
#endif
    ret = display_set_backlight(rgb_display, DISPLAY_BACKLIGHT_MAX);
    if (ret && ret != -ENOTSUP) {
        goto fail_display;
    }

#if IS_ENABLED(CONFIG_SOLN_FBQ_VID_RAW_REMOTE_EN)
    img_raw_frame_ctrl_disp = soln_fbq_vid_raw_remote();
    img_raw_queue_id_disp = CONFIG_SOLN_FBQ_VID_RAW_REMOTE_DISP_ID;
    img_raw_queue_depth_disp = CONFIG_SOLN_FBQ_VID_RAW_REMOTE_DISP_DEPTH;
    img_raw_accept_mask = SOLUTION_FBQ_TYPE_MASK_IMG_RAW_JPEG_DEC;
#elif IS_ENABLED(CONFIG_SOLN_FBQ_VID_RAW_LOCAL_EN)
    img_raw_frame_ctrl_disp = soln_fbq_vid_raw_local();
    img_raw_queue_id_disp = CONFIG_SOLN_FBQ_VID_RAW_LOCAL_DISP_ID;
    img_raw_queue_depth_disp = CONFIG_SOLN_FBQ_VID_RAW_LOCAL_DISP_DEPTH;
    img_raw_accept_mask = SOLUTION_FBQ_TYPE_MASK_IMG_RAW_CAPTURE_ALL;
#else
#error "please config img_raw queue for display"
#endif

    if (!img_raw_frame_ctrl_disp) {
        ret = -ENODEV;
        goto fail_display;
    }
    /* Create the display FBQ output subscription. */
    if (fbq_output_open(img_raw_frame_ctrl_disp, &img_raw_queue_id_disp,
                        img_raw_queue_depth_disp,
                        img_raw_accept_mask) != FBQ_OK) {
        LOG_E("img_raw frame rgb display out queue create failed\r\n");
        ret = -EIO;
        goto fail_display;
    } else {
        LOG_I("img_raw frame rgb display out queue ID: %d\r\n", img_raw_queue_id_disp);
    }

    /* rgb display task */
    if (xTaskCreate(rgb_disp_lcd_task, (char *)"disp_rgb_task", 1024, NULL, 10, &rgb_disp_handle) != pdPASS) {
        fbq_output_close(img_raw_frame_ctrl_disp, img_raw_queue_id_disp);
        ret = -ENOMEM;
        goto fail_display;
    }

    return 0;

fail_display:
    {
        int cleanup_ret = display_deinit(rgb_display);
        if (cleanup_ret) {
            /* A failed stop retains frames and callback: keep their semaphore alive. */
            LOG_E("display cleanup failed: %d\r\n", cleanup_ret);
            return cleanup_ret;
        }
        /* Successful deinit has disconnected scanout events. */
    }
fail_sem:
    display_os_sem_deinit(&rgb_disp_semaphore);
    rgb_disp_resources = false;
    return ret;
}
