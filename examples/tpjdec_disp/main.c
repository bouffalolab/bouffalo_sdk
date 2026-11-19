
#include "bflb_mtimer.h"
#include "board.h"
#include "default/display_default.h"
#include "display_os.h"
#include "simple_font/display_simple_font.h"
#include "tjpgd.h"
#include "bflb_dbi.h"
#include <string.h>

/* fatfs */
#ifdef CONFIG_FATFS
#if !defined(CONFIG_FATFS_SDH_SDCARD) || !defined(CONFIG_BSP_SDH_SD)
#error "The SD-card path requires FATFS_SDH_SDCARD and BSP_SDH_SD"
#endif
#include "fatfs_diskio_register.h"
#include "ff.h"
#else
/* image data */
#include "test_img/img0.h"
#include "test_img/img1.h"
#include "test_img/img2.h"
#include "test_img/img3.h"
#include "test_img/img4.h"
#endif

#define DBG_TAG "MAIN"
#include "log.h"

#if !defined(CONFIG_DISPLAY_DEFAULT_TYPE_GRAM) || !defined(CONFIG_DISPLAY_BUS_DBI) || \
    defined(CONFIG_DISPLAY_DEFAULT_IF_SPI) || !defined(CONFIG_DISPLAY_DEFAULT_FORMAT_RGB888)
#error "This demo requires the hardware DBI peripheral and a default RGB888 GRAM display"
#endif

#if !DBI_YUV_SUPPORT || !JD_YUV444_MODE || JD_FORMAT != 0
#error "This demo requires packed YUV444 JPEG output and DBI YUV conversion"
#endif

#define N_BPP       3u
#define IMAGE_COUNT 5u
#define FILL_PIXELS 2048u

static struct bflb_device_s *dbi_hd;
static display_t *screen;
static display_info_t info;
static volatile bool draw_pending;
static volatile int draw_result;
static const void *pending_pixels;
/* DBI reads whole words, including the last partial word of packed RGB/YUV. */
static uint8_t draw_buff[FILL_PIXELS * N_BPP + 3] __ALIGNED(64);

#if defined(CONFIG_DISPLAY_DEFAULT_MODULE_ST7796_320X480)
/* Parameters of the ST7796 glass connected to this board. */
static const display_dcs_cmd_t st7796_inversion[] = {
    DISPLAY_DCS(0xF0, 0, 0xC3),
    DISPLAY_DCS(0xF0, 0, 0x96),
    DISPLAY_DCS(0xB4, 0, 0x01),
    DISPLAY_DCS(0xF0, 0, 0x3C),
    DISPLAY_DCS(0xF0, 0, 0x69),
};

static void adjust_default_module(void)
{
    display_dcs_module_t *module = display_default_get_module();

    module->madctl[0] = 0x48;
    module->madctl[1] = 0xE8;
    module->madctl[2] = 0x88;
    module->madctl[3] = 0x28;
    module->invert = true;
    module->init_extra = st7796_inversion;
    module->init_extra_count = DISPLAY_DCS_COUNT(st7796_inversion);
}
#endif

/* input jpeg buf */
ATTR_NOINIT_PSRAM_SECTION __ALIGNED(64) uint8_t jpeg_buff[32 * 1024];

/* output img buf */
ATTR_NOINIT_PSRAM_SECTION __ALIGNED(64) uint8_t bpp_buff[320 * 240 * N_BPP + 3];

/* Session identifier for input/output functions (Name, members and usage are as user defined) */
struct IODEV {
    uint8_t *in_jpg_buf;
    uint32_t in_jpg_buf_size;
    uint32_t in_jpg_data_size;
    uint32_t in_jpg_offset; /* */

    uint8_t *out_img_buf; /* One application-owned frame, reused only after completion. */
    uint32_t out_img_buf_size;
    uint32_t out_img_w; /* Width of the frame buffer [pix] */
    uint32_t out_img_h;
};

/*------------------------------*/
/* User defined input funciton  */
/*------------------------------*/
size_t in_func(               /* Returns number of bytes read (zero on error) */
               JDEC *jd,      /* Decompression object */
               uint8_t *buff, /* Pointer to the read buffer (null to remove data) */
               size_t nbyte   /* Number of bytes to read/remove */
)
{
    struct IODEV *dev = (struct IODEV *)jd->device; /* Session identifier (5th argument of jd_prepare function) */

    if (dev->in_jpg_offset > dev->in_jpg_data_size) {
        return 0;
    }
    if (nbyte > dev->in_jpg_data_size - dev->in_jpg_offset) {
        nbyte = dev->in_jpg_data_size - dev->in_jpg_offset;
    }
    if (buff) {
        /* Read only bytes present in the loaded JPEG. */
        memcpy(buff, (uint8_t *)dev->in_jpg_buf + dev->in_jpg_offset, nbyte);
    }
    dev->in_jpg_offset += nbyte;

    return nbyte;
}

/*------------------------------*/
/* User defined output funciton */
/*------------------------------*/
int out_func(              /* Returns 1 to continue, 0 to abort */
             JDEC *jd,     /* Decompression object */
             void *bitmap, /* Bitmap data to be output */
             JRECT *rect   /* Rectangle region of output image */
)
{
    struct IODEV *dev = (struct IODEV *)jd->device; /* Session identifier (5th argument of jd_prepare function) */

    const uint8_t *src = bitmap;

    if (rect->left > rect->right || rect->top > rect->bottom ||
        rect->right >= dev->out_img_w || rect->bottom >= dev->out_img_h ||
        (size_t)dev->out_img_w * dev->out_img_h > dev->out_img_buf_size / N_BPP) {
        return 0;
    }
    uint8_t *dst = dev->out_img_buf + N_BPP * (rect->top * dev->out_img_w + rect->left);
    size_t bws = N_BPP * (rect->right - rect->left + 1u);
    size_t bwd = N_BPP * dev->out_img_w;

    /* The decoder supplies a tightly packed, clipped Y/U/V rectangle. */
    for (unsigned y = rect->top; y <= rect->bottom; y++) {
        memcpy(dst, src, bws); /* Copy a line */
        src += bws;
        if (y < rect->bottom) {
            dst += bwd; /* Next line, but never beyond the output object. */
        }
    }

    return 1; /* Continue to decompress */
}

/*  */
int jpeg_raw_load(struct IODEV *dev, uint8_t n)
{
    dev->in_jpg_buf = (void *)jpeg_buff;
    dev->in_jpg_buf_size = sizeof(jpeg_buff);
    dev->in_jpg_offset = 0;
    dev->in_jpg_data_size = 0;
    if (n >= IMAGE_COUNT) {
        return -EINVAL;
    }

#ifdef CONFIG_FATFS
    int ret;
    FIL fnew;
    UINT fnum;
    char file_name[64];

    sprintf(file_name, "/sd/img%d.jpg", n);
    ret = f_open(&fnew, file_name, FA_OPEN_EXISTING | FA_READ);
    if (ret != FR_OK) {
        LOG_E("Fail to open files:%s err:%d\n", file_name, ret);
        return -1;
    }

    FSIZE_t file_size = f_size(&fnew);
    if (!file_size || file_size > dev->in_jpg_buf_size) {
        f_close(&fnew);
        LOG_E("Invalid JPEG file size\r\n");
        return -1;
    }

    ret = f_read(&fnew, dev->in_jpg_buf, (UINT)file_size, &fnum);
    f_close(&fnew);
    if (ret != FR_OK || fnum != file_size) {
        LOG_E("JPEG read failed: %d\r\n", ret);
        return -1;
    }
    dev->in_jpg_data_size = fnum;
    LOG_I("file:%s, size:%u\r\n", file_name, (unsigned)fnum);

#else
    const uint8_t *img_tab[] = { _acimg0, _acimg1, _acimg2, _acimg3, _acimg4 };
    uint32_t img_size_tab[] = { sizeof(_acimg0), sizeof(_acimg1), sizeof(_acimg2), sizeof(_acimg3), sizeof(_acimg4) };

    if (img_size_tab[n] > dev->in_jpg_buf_size) {
        LOG_E("img_data size over: %d\r\n", img_size_tab[n]);
        return -1;
    }

    LOG_I("jpeg_data[%d], size:%d\r\n", n, img_size_tab[n]);

    memcpy(dev->in_jpg_buf, img_tab[n], img_size_tab[n]);
    dev->in_jpg_data_size = img_size_tab[n];

#endif

    return 0;
}

int jpeg_dec(struct IODEV *dev)
{
    int ret;
    JDEC jdec;
    static uint32_t jpg_work_buff[16 * 1024 / 4];

    /* Waiting here, before any output write, protects the single frame buffer. */
    while (draw_pending) {
        display_os_delay_ms(1);
    }
    if (draw_result) {
        return draw_result;
    }
    dev->out_img_buf = bpp_buff;
    dev->out_img_buf_size = sizeof(bpp_buff) - 3;

    uint32_t start_time = bflb_mtimer_get_time_ms();

    /* prepare jpeg file */
    ret = jd_prepare(&jdec, in_func, (void *)jpg_work_buff, sizeof(jpg_work_buff), dev);
    if (ret != JDR_OK) {
        LOG_E("jd_prepare() failed (rc=%d)\n", ret);
        return -1;
    }

    /* It is ready to dcompress and image info is available here */
    LOG_I("Image size is %u x %u.\r\n", jdec.width, jdec.height);
    LOG_I("%u bytes of workspace used.\r\n", (unsigned)(sizeof(jpg_work_buff) - jdec.sz_pool));

    /* Initialize output device (Create a frame buffer) */
    if (!jdec.width || !jdec.height || jdec.width > info.width || jdec.height > info.height ||
        (size_t)jdec.width * jdec.height > dev->out_img_buf_size / N_BPP) {
        LOG_E("Image exceeds screen or output buffer\r\n");
        return -EINVAL;
    }
    dev->out_img_w = jdec.width;
    dev->out_img_h = jdec.height;

    /* Start to decompress with 1/1 scaling */
    ret = jd_decomp(&jdec, out_func, 0);
    if (ret != JDR_OK) {
        LOG_E("jd_decomp() failed (rc=%d)\n", ret);
        return -1;
    }

    LOG_I("Dec time: %dms\r\n", (uint32_t)(bflb_mtimer_get_time_ms() - start_time));

    return 0;
}

static void restore_rgb(void)
{
    bflb_dbi_feature_control(dbi_hd, DBI_CMD_INPUT_PIXEL_FORMAT, DBI_PIXEL_INPUT_FORMAT_RGB_888);
    bflb_dbi_feature_control(dbi_hd, DBI_CMD_CLEAR_TX_FIFO, 0);
}

static void flush_async_callback(display_t *display, const display_event_t *event, void *ctx)
{
    (void)ctx;

    /* DBI TC means pixels have left the FIFO. May also run inline without DMA. */
    if (display == screen && event->type == DISPLAY_EVT_DRAW_DONE &&
        draw_pending && event->buf == pending_pixels) {
        restore_rgb();
        draw_result = event->result;
        draw_pending = false; /* Publish completion only after restoring RGB. */
    }
}

static int display_image(struct IODEV *dev)
{
    const size_t row_bytes = dev->out_img_w * N_BPP;
    size_t max_rows = info.max_draw_bytes / row_bytes;
    display_io_t *io = display_panel_get_io(display_get_panel(screen));

    if (max_rows > dev->out_img_h) {
        max_rows = dev->out_img_h;
    }
    /* Every non-final stripe must leave an aligned start for the next stripe. */
    if (max_rows < dev->out_img_h) {
        while (max_rows && (max_rows * row_bytes) % info.buf_align) {
            max_rows--;
        }
    }
    if (!max_rows) {
        return -ENOTSUP;
    }
    for (unsigned row = 0; row < dev->out_img_h;) {
        unsigned rows = dev->out_img_h - row;
        int ret;

        if (rows > max_rows) {
            rows = max_rows;
        }
        /* DCS NOP applies this endpoint's normal configuration BEFORE the LHAL
         * override, including after a previous configuration invalidation.
         * This demo is the only user of the DBI peripheral; do not share it. */
        ret = display_io_write(io, 0x00, NULL, 0);
        if (ret) {
            return ret;
        }
        pending_pixels = dev->out_img_buf + row * row_bytes;
        draw_result = -EINPROGRESS;
        draw_pending = true; /* Arm before submit: completion can be inline. */
        bflb_dbi_feature_control(dbi_hd, DBI_CMD_INPUT_PIXEL_FORMAT, DBI_PIXEL_INPUT_FORMAT_YUV444);
        display_area_t area = { .x = 0, .y = row, .w = dev->out_img_w, .h = rows };

        ret = display_draw_async(screen, &area, pending_pixels, rows * row_bytes);
        if (ret) {
            restore_rgb(); /* Failed submissions do not produce a completion event. */
            draw_result = ret;
            draw_pending = false;
            return ret;
        }
        while (draw_pending) {
            display_os_delay_ms(1);
        }
        if (draw_result) {
            return draw_result;
        }
        row += rows;
    }
    return 0;
}

static int fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t rgb)
{
    size_t capacity = info.max_draw_bytes / N_BPP;

    if (capacity > FILL_PIXELS) {
        capacity = FILL_PIXELS;
    }
    if (!capacity) {
        return -ENOTSUP;
    }
    for (size_t i = 0; i < capacity; i++) {
        draw_buff[i * N_BPP] = rgb;
        draw_buff[i * N_BPP + 1] = rgb >> 8;
        draw_buff[i * N_BPP + 2] = rgb >> 16;
    }
    for (unsigned dx = 0; dx < w;) {
        unsigned cw = w - dx;

        if (cw > capacity) {
            cw = capacity;
        }
        for (unsigned dy = 0; dy < h;) {
            unsigned ch = h - dy;
            size_t rows = capacity / cw;

            if (ch > rows) {
                ch = rows;
            }
            display_area_t area = { .x = x + dx, .y = y + dy, .w = cw, .h = ch };
            int ret = display_draw(screen, &area, draw_buff, cw * ch * N_BPP);

            if (ret) {
                return ret;
            }
            dy += ch;
        }
        dx += cw;
    }
    return 0;
}

static int draw_text(uint16_t y, uint32_t color, const char *text)
{
    uint16_t width = strlen(text) * 8u;
    const display_simple_font_cfg_t cfg = {
        .format = DISPLAY_FMT_RGB888, .width = width, .height = 16,
        .font = DISPLAY_SIMPLE_FONT_ASCII_16X8,
        .fg_color = 0xff000000u | color, .bg_color = 0xff000000u,
    };

    if (!width || (size_t)width * 16 > FILL_PIXELS || (size_t)width * 16 * N_BPP > info.max_draw_bytes) {
        return -ENOTSUP;
    }
    int ret = display_simple_font_draw(&cfg, draw_buff, 0, 0, text);
    display_area_t area = { .x = 20, .y = y, .w = width, .h = 16 };

    return ret ? ret : display_draw(screen, &area, draw_buff, width * 16u * N_BPP);
}

static int draw_startup(void)
{
    static const uint32_t colors[] = { 0xff0000, 0x00ff00, 0x0000ff };
    int ret = fill_rect(0, 0, info.width, info.height, 0x101010);

    if (!ret) {
        ret = draw_text(20, 0xff0000, "Hello World !");
    }
    if (!ret) {
        ret = draw_text(40, 0x00ff00, "dbi lcd test.");
    }
    for (unsigned i = 0; !ret && i < 3; i++) {
        uint16_t inset = i * 5;
        uint16_t w = info.width - 2 * inset;
        uint16_t h = info.height - 2 * inset;

        ret = fill_rect(inset, inset, w, 1, colors[i]);
        if (!ret) {
            ret = fill_rect(inset, inset + h - 1, w, 1, colors[i]);
        }
        if (!ret) {
            ret = fill_rect(inset, inset + 1, 1, h - 2, colors[i]);
        }
        if (!ret) {
            ret = fill_rect(inset + w - 1, inset + 1, 1, h - 2, colors[i]);
        }
    }
    return ret;
}

static void fail_stop(int ret)
{
    LOG_E("Display demo stopped: %d\r\n", ret);
    while (1) {
        bflb_mtimer_delay_ms(1000);
    }
}

#ifdef CONFIG_FATFS
/* fatfs filesystem init */
int fatfs_init(void)
{
    static FATFS fs;
    FRESULT ret;

    board_sdh_gpio_init();

    fatfs_sdh_driver_register();

    ret = f_mount(&fs, "/sd", 1);
    if (ret != FR_OK) {
        LOG_F("fail to mount filesystem,error= %d\r\n", ret);
        LOG_F("SD card might fail to initialise.\r\n");
        return -1;
    }

    LOG_D("Succeed to mount filesystem\r\n");
    LOG_I("FileSystem cluster size:%d-sectors (%d-Byte)\r\n", fs.csize, fs.csize * 512);
    return 0;
}
#endif

int main(void)
{
    struct IODEV dev = { 0 };
    int ret;

    board_init();

    LOG_I("tpjdec display test\r\n");

    dbi_hd = bflb_device_get_by_name(CONFIG_DISPLAY_DEFAULT_DEV);
    screen = display_default_get();
    if (!dbi_hd) {
        fail_stop(-ENODEV);
    }
#if defined(CONFIG_DISPLAY_DEFAULT_MODULE_ST7796_320X480)
    adjust_default_module();
#endif
    ret = display_init(screen);
    if (!ret) {
        ret = display_set_rotation(screen, DISPLAY_ROTATION_90);
    }
    if (!ret) {
        ret = display_get_info(screen, &info);
    }
    if (ret) {
        fail_stop(ret);
    }
    if (info.kind != DISPLAY_KIND_GRAM || info.format != DISPLAY_FMT_RGB888 ||
        info.width < 124 || info.height < 56 || info.x_align != 1 || info.y_align != 1 ||
        !info.buf_align || (uintptr_t)bpp_buff % info.buf_align || (uintptr_t)draw_buff % info.buf_align) {
        fail_stop(-ENOTSUP);
    }
    ret = display_set_callback(screen, flush_async_callback, NULL);
    if (!ret) {
        ret = draw_startup();
    }
    if (!ret) {
        ret = display_set_backlight(screen, DISPLAY_BACKLIGHT_MAX);
        if (ret == -ENOTSUP) {
            ret = 0; /* The module may have an externally controlled backlight. */
        }
    }
    if (ret) {
        fail_stop(ret);
    }
    LOG_I("Display init done\r\n");

    bflb_mtimer_delay_ms(500);

#ifdef CONFIG_FATFS
    /* fatfs_init */
    ret = fatfs_init();
    if (ret < 0) {
        fail_stop(ret);
    }
#endif

    while (1) {
        for (unsigned i = 0; i < IMAGE_COUNT; i++) {
            ret = jpeg_raw_load(&dev, i);
            if (!ret) {
                ret = jpeg_dec(&dev);
            }
            if (ret) {
                LOG_E("Skip JPEG %u: %d\r\n", i, ret);
                continue;
            }
            ret = display_image(&dev);
            if (ret) {
                fail_stop(ret); /* Do not retry with unknown controller state. */
            }
            LOG_I("JPEG %u displayed: %ux%u, RGB888 input restored\r\n",
                  i, (unsigned)dev.out_img_w, (unsigned)dev.out_img_h);

            bflb_mtimer_delay_ms(1000);
        }
    }

    return 0;
}
