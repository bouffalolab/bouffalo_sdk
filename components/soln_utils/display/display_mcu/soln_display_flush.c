#include <string.h>

#include "bflb_dbi.h"
#include "default/display_default.h"
#include "display_os.h"
#include "simple_font/display_simple_font.h"
#include "soln_display_convert.h"
#include "soln_display_flush.h"

#if !defined(CONFIG_DISPLAY_DEFAULT_TYPE_GRAM) || !defined(CONFIG_DISPLAY_BUS_DBI) || \
    !defined(CONFIG_DISPLAY_DEFAULT_FORMAT_RGB888)
#error "Solution DBI display needs a default DBI GRAM display with packed RGB888 input"
#endif

#define BAND_WIDTH ((DISPLAY_DEFAULT_WIDTH > DISPLAY_DEFAULT_HEIGHT) ? DISPLAY_DEFAULT_WIDTH : DISPLAY_DEFAULT_HEIGHT)
/* Each buffer starts on its own cache line, with readable padding for word-rounded DBI reads. */
#define BAND_BYTES (((BAND_WIDTH * SOLN_DISPLAY_BAND_ROWS * 3u + 3u + 63u) / 64u) * 64u)
#if defined(CONFIG_PSRAM)
#define BAND_ATTR __ALIGNED(64) ATTR_NOINIT_PSRAM_SECTION
#else
#define BAND_ATTR __ALIGNED(64) ATTR_NOCACHE_NOINIT_RAM_SECTION
#endif

static uint8_t BAND_ATTR bands[2][BAND_BYTES];
static display_t *screen;
static display_io_t *screen_io;
static display_info_t screen_info;
static struct bflb_device_s *dbi;
/* Available means the previous transfer is complete. Initialized once to 1. */
static display_os_sem_t transfer_sem;
static volatile int transfer_result;

static void flush_done(display_t *display, const display_event_t *event, void *ctx)
{
    (void)display;
    (void)ctx;
    if (event->type == DISPLAY_EVT_DRAW_DONE) {
        transfer_result = event->result;
        display_os_sem_give(&transfer_sem); /* ISR with DMA, task context without DMA. */
    }
}

static void restore_rgb(void)
{
    bflb_dbi_feature_control(dbi, DBI_CMD_INPUT_PIXEL_FORMAT, DBI_PIXEL_INPUT_FORMAT_RGB_888);
    bflb_dbi_feature_control(dbi, DBI_CMD_CLEAR_TX_FIFO, 0);
}

static int submit_band(const display_area_t *area, const void *pixels)
{
    int ret = display_os_sem_take(&transfer_sem, DISPLAY_OS_WAIT_FOREVER);
    if (ret) {
        return ret;
    }
    ret = transfer_result;
    if (!ret) {
        /* Take before submit, NOT before conversion: CPU prepares the other buffer
         * while DBI is reading the previous one. Completion may precede return. */
        ret = display_draw_async(screen, area, pixels, (size_t)area->w * area->h * 3u);
    }
    if (ret) {
        /* A rejected draw has no completion callback. Return the idle token ourselves. */
        display_os_sem_give(&transfer_sem);
    }
    return ret;
}

const display_info_t *soln_disp_dbi_info(void)
{
    return &screen_info;
}

int soln_disp_dbi_init(void)
{
    int ret;

    screen = display_default_get();
    screen_io = display_panel_get_io(display_get_panel(screen));
    dbi = bflb_device_get_by_name(CONFIG_DISPLAY_DEFAULT_DEV);
    if (!dbi || !screen_io) {
        return -ENODEV;
    }
    ret = display_os_sem_init(&transfer_sem);
    if (ret) {
        return ret;
    }
    ret = display_init(screen);
    if (!ret) {
        ret = display_set_rotation(screen, DISPLAY_ROTATION_90);
    }
    if (!ret) {
        ret = display_get_info(screen, &screen_info);
    }
    if (!ret && (screen_info.kind != DISPLAY_KIND_GRAM || screen_info.format != DISPLAY_FMT_RGB888 ||
                 screen_info.width > BAND_WIDTH || screen_info.buf_align > 64u ||
                 (size_t)screen_info.width * SOLN_DISPLAY_BAND_ROWS * 3u > screen_info.max_draw_bytes)) {
        ret = -ENOTSUP;
    }
    if (!ret) {
        ret = display_set_callback(screen, flush_done, NULL);
    }
    if (!ret) {
        memset(bands[0], 0, BAND_BYTES);
        for (unsigned y = 0; y < screen_info.height && !ret; y += SOLN_DISPLAY_BAND_ROWS) {
            unsigned rows = screen_info.height - y;
            if (rows > SOLN_DISPLAY_BAND_ROWS) {
                rows = SOLN_DISPLAY_BAND_ROWS;
            }
            display_area_t area = { .x = 0, .y = y, .w = screen_info.width, .h = rows };
            ret = display_draw(screen, &area, bands[0], (size_t)area.w * area.h * 3u);
        }
    }
    if (!ret) {
        ret = display_set_backlight(screen, DISPLAY_BACKLIGHT_MAX);
        if (ret == -ENOTSUP) {
            ret = 0;
        }
    }
    if (ret) {
        display_deinit(screen);
        display_os_sem_deinit(&transfer_sem);
    }
    return ret;
}

/* RGB888 and packed YUV444 already have the endpoint's three-byte layout.
 * Direct blocks use whole rows with aligned starts/lengths; only unaligned
 * data or the final sub-word tail needs a staging copy. */
static int flush_packed_frame(const uint8_t *src, unsigned width, unsigned height)
{
    const size_t stride = (size_t)width * 3u;
    const unsigned alignment = screen_info.buf_align > 4u ? screen_info.buf_align : 4u;
    unsigned a = alignment, b = stride % alignment;
    while (b) {
        unsigned remainder = a % b;
        a = b;
        b = remainder;
    }
    const unsigned row_quantum = alignment / a;
    const unsigned max_rows = screen_info.max_draw_bytes / stride;
    unsigned index = 0;

    for (unsigned y = 0; y < height;) {
        unsigned rows = height - y;
        if (rows > max_rows) {
            rows = max_rows;
        }
        const uint8_t *pixels = src + (size_t)y * stride;
        unsigned direct_rows = !((uintptr_t)pixels % alignment) ? rows - rows % row_quantum : 0;
        if (direct_rows) {
            rows = direct_rows;
        } else {
            if (rows > SOLN_DISPLAY_BAND_ROWS) {
                rows = SOLN_DISPLAY_BAND_ROWS;
            }
            size_t bytes = rows * stride;
            memcpy(bands[index], pixels, bytes);
            memset(bands[index] + bytes, 0, (4u - bytes % 4u) % 4u);
            pixels = bands[index];
            index ^= 1u;
        }
        display_area_t area = { .x = 0, .y = y, .w = width, .h = rows };
        int ret = submit_band(&area, pixels);
        if (ret) {
            return ret;
        }
        y += rows;
    }
    return 0;
}

static int flush_yuyv422_frame(const uint8_t *src, unsigned width, unsigned height)
{
    unsigned index = 0;
    for (unsigned y = 0; y < height; y += SOLN_DISPLAY_BAND_ROWS) {
        unsigned rows = height - y;
        if (rows > SOLN_DISPLAY_BAND_ROWS) {
            rows = SOLN_DISPLAY_BAND_ROWS;
        }
        /* Convert before waiting: the other band may still be read by DMA. */
        soln_yuyv422_to_yuv444_band(bands[index], src, width, y, rows);
        size_t bytes = (size_t)width * rows * 3u;
        memset(bands[index] + bytes, 0, (4u - bytes % 4u) % 4u);
        display_area_t area = { .x = 0, .y = y, .w = width, .h = rows };
        int ret = submit_band(&area, bands[index]);
        if (ret) {
            return ret;
        }
        index ^= 1u;
    }
    return 0;
}

static int flush_yuyv422_640x480_to_480x320(const uint8_t *src)
{
    unsigned index = 0;
    for (unsigned y = 0; y < 320u; y += SOLN_DISPLAY_BAND_ROWS) {
        unsigned rows = 320u - y;
        if (rows > SOLN_DISPLAY_BAND_ROWS) {
            rows = SOLN_DISPLAY_BAND_ROWS;
        }
        soln_yuyv422_640x480_to_yuv444_480x320_band(bands[index], src, y, rows);
        display_area_t area = { .x = 0, .y = y, .w = 480, .h = rows };
        int ret = submit_band(&area, bands[index]);
        if (ret) {
            return ret;
        }
        index ^= 1u;
    }
    return 0;
}

int soln_disp_flush_frame(const fbq_elem_t *frame)
{
    if (!frame) {
        return -EINVAL;
    }
    const soln_fbq_img_raw_ext_t *raw = soln_fbq_img_raw_ext_const(frame);
    unsigned width, height, bpp;
    bool scaled = false;

    if (!frame->data || raw->x_end < raw->x_start || raw->y_end < raw->y_start) {
        return -EINVAL;
    }
    width = (unsigned)raw->x_end - raw->x_start + 1u;
    height = (unsigned)raw->y_end - raw->y_start + 1u;
    switch (raw->format) {
        case IMG_RAW_FRAME_FORMAT_YUYV:
            bpp = 2;
            break;
        case IMG_RAW_FRAME_FORMAT_RGB888:
        case IMG_RAW_FRAME_FORMAT_YUV:
            bpp = 3;
            break;
        default:
            return -ENOTSUP;
    }
    if ((size_t)width * height > frame->size / bpp ||
        (raw->format == IMG_RAW_FRAME_FORMAT_YUYV && (width & 1u))) {
        return -EINVAL;
    }
    if (width > screen_info.width || height > screen_info.height) {
        if (width != 640 || height != 480 || raw->format != IMG_RAW_FRAME_FORMAT_YUYV ||
            screen_info.width < 480 || screen_info.height < 320) {
            return -ENOTSUP;
        }
        scaled = true;
    }
    int ret = display_os_sem_take(&transfer_sem, DISPLAY_OS_WAIT_FOREVER);
    if (ret) {
        return ret;
    }
    transfer_result = 0;
    /* Apply the RGB888 endpoint before the temporary override. Single-screen exclusive
     * ownership keeps it applied throughout this frame; on any error we stop the frame. */
    ret = display_io_write(screen_io, 0x00, NULL, 0);
    if (!ret && raw->format != IMG_RAW_FRAME_FORMAT_RGB888) {
        ret = bflb_dbi_feature_control(dbi, DBI_CMD_INPUT_PIXEL_FORMAT, DBI_PIXEL_INPUT_FORMAT_YUV444);
    }
    display_os_sem_give(&transfer_sem);

    if (!ret) {
        if (raw->format == IMG_RAW_FRAME_FORMAT_RGB888 || raw->format == IMG_RAW_FRAME_FORMAT_YUV) {
            ret = flush_packed_frame(frame->data, width, height);
        } else if (!scaled) {
            ret = flush_yuyv422_frame(frame->data, width, height);
        } else {
            ret = flush_yuyv422_640x480_to_480x320(frame->data);
        }
    }

    /* The final band has no following submit to wait for it. Drain before restoring
     * the input format, drawing text, reusing buffers, or releasing a direct source. */
    int wait_ret = display_os_sem_take(&transfer_sem, DISPLAY_OS_WAIT_FOREVER);
    if (wait_ret) {
        return wait_ret;
    }
    if (!ret) {
        ret = transfer_result;
    }
    restore_rgb();
    display_os_sem_give(&transfer_sem);
    return ret;
}

int soln_disp_draw_text(uint16_t x, uint16_t y, uint32_t fg, uint32_t bg, const char *text)
{
    if (x >= screen_info.width || y >= screen_info.height) {
        return -EINVAL;
    }
    /* Text is drawn only between completed frames, in normal RGB888 mode. */
    for (; *text && x + 8u <= screen_info.width; text++, x += 8u) {
        unsigned rows = screen_info.height - y;
        if (rows > 16u) {
            rows = 16u;
        }
        display_simple_font_cfg_t cfg = {
            .format = DISPLAY_FMT_RGB888, .width = 8, .height = rows,
            .font = DISPLAY_SIMPLE_FONT_ASCII_16X8, .fg_color = fg, .bg_color = bg,
        };
        char glyph[2] = { *text, '\0' };
        int ret = display_simple_font_draw(&cfg, bands[0], 0, 0, glyph);
        if (!ret) {
            display_area_t area = { .x = x, .y = y, .w = 8, .h = rows };
            ret = display_draw(screen, &area, bands[0], (size_t)8u * rows * 3u);
        }
        if (ret) {
            return ret;
        }
    }
    return 0;
}