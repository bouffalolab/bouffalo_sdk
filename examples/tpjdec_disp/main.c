
#include "bflb_mtimer.h"
#include "board.h"
#include "lcd.h"
#include "tjpgd.h"

#include "bflb_dbi.h"

/* fatfs */
#ifdef CONFIG_FATFS
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

#if defined(DBI_YUV_SUPPORT) && DBI_YUV_SUPPORT
#define N_BPP 3 /* yuv444 */
#else
#define N_BPP 2 /* rgb565 */
#endif

struct bflb_device_s *dbi_hd;

/* input jpeg buf */
ATTR_NOINIT_PSRAM_SECTION __ALIGNED(64) uint8_t jpeg_buff[32 * 1024];

/* output img buf */
ATTR_NOINIT_PSRAM_SECTION __ALIGNED(64) uint8_t bpp_buff[1][320 * 240 * N_BPP];

/* Session identifier for input/output functions (Name, members and usage are as user defined) */
struct IODEV {
    uint8_t *in_jpg_buf;
    uint32_t in_jpg_buf_size;
    uint32_t in_jpg_data_size;
    uint32_t in_jpg_offset; /* */

    uint8_t *out_img_buf[2]; /* Pointer to the frame buffer */
    uint8_t out_img_buf_index;
    uint32_t out_img_buf_size;
    uint32_t out_img_w; /* Width of the frame buffer [pix] */
    uint32_t out_img_h;
} jpeg_dev;

/*------------------------------*/
/* User defined input funciton  */
/*------------------------------*/
ATTR_PSRAM_CODE_SECTION
size_t in_func(               /* Returns number of bytes read (zero on error) */
               JDEC *jd,      /* Decompression object */
               uint8_t *buff, /* Pointer to the read buffer (null to remove data) */
               size_t nbyte   /* Number of bytes to read/remove */
)
{
    struct IODEV *dev = (struct IODEV *)jd->device; /* Session identifier (5th argument of jd_prepare function) */

    if (buff) {
        /* Raad data from imput stream */
        memcpy(buff, (void *)dev->in_jpg_buf + dev->in_jpg_offset, nbyte);
    }
    dev->in_jpg_offset += nbyte;

    return nbyte;
}

/*------------------------------*/
/* User defined output funciton */
/*------------------------------*/
ATTR_PSRAM_CODE_SECTION
int out_func(              /* Returns 1 to continue, 0 to abort */
             JDEC *jd,     /* Decompression object */
             void *bitmap, /* Bitmap data to be output */
             JRECT *rect   /* Rectangle region of output image */
)
{
    struct IODEV *dev = (struct IODEV *)jd->device; /* Session identifier (5th argument of jd_prepare function) */

    uint8_t *src, *dst;
    uint16_t y, bws;
    unsigned int bwd;

    /* Progress indicator */
    // if (rect->left == 0) {
    //     LOG_I("\r%lu%%", (rect->top << jd->scale) * 100UL / jd->height);
    // }

    // LOG_I("out rect w:%d, h:%d\r\n", rect->bottom - rect->top + 1, rect->right - rect->left + 1);

    /* Copy the output image rectangle to the frame buffer */
    src = (uint8_t *)bitmap;                                                                            /* Output bitmap */
    dst = dev->out_img_buf[dev->out_img_buf_index] + N_BPP * (rect->top * dev->out_img_w + rect->left); /* Left-top of rectangle in the frame buffer */
    bws = N_BPP * (rect->right - rect->left + 1);                                                       /* Width of the rectangle [byte] */
    bwd = N_BPP * dev->out_img_w;                                                                       /* Width of the frame buffer [byte] */
    for (y = rect->top; y <= rect->bottom; y++) {
        memcpy(dst, src, bws); /* Copy a line */
        src += bws;
        dst += bwd; /* Next line */
    }

    return 1; /* Continue to decompress */
}

/*  */
int jpeg_raw_load(struct IODEV *dev, uint8_t n)
{
    dev->in_jpg_buf = (void *)jpeg_buff;
    dev->in_jpg_buf_size = sizeof(jpeg_buff);
    dev->in_jpg_offset = 0;

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

    if (f_size(&fnew) > dev->in_jpg_buf_size) {
        f_close(&fnew);
        LOG_E("file size over: %d\r\n", f_size(&fnew));
        return -1;
    }

    ret = f_read(&fnew, dev->in_jpg_buf, f_size(&fnew), &fnum);
    LOG_I("file:%s, size:%d, rd_size:%d\r\n", file_name, f_size(&fnew), fnum);
    f_close(&fnew);

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

    dev->out_img_buf[0] = bpp_buff[0];
    dev->out_img_buf[1] = bpp_buff[0];
    dev->out_img_buf_size = sizeof(bpp_buff[0]);
    dev->out_img_buf_index = !dev->out_img_buf_index;

    uint32_t start_time = bflb_mtimer_get_time_ms();

    /* prepare jpeg file */
    ret = jd_prepare(&jdec, in_func, (void *)jpg_work_buff, sizeof(jpg_work_buff), dev);
    if (ret != JDR_OK) {
        LOG_E("jd_prepare() failed (rc=%d)\n", ret);
        return -1;
    }

    /* It is ready to dcompress and image info is available here */
    LOG_I("Image size is %u x %u.\r\n", jdec.width, jdec.height);
    LOG_I("%u bytes of work ares is used.\r\n", sizeof(jpg_work_buff) - jdec.sz_pool);

    /* Initialize output device (Create a frame buffer) */
    if (jdec.width > lcd_max_x + 1 || jdec.height > lcd_max_y + 1) {
        LOG_E("Image size OVER\r\n");
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

void flush_async_callback()
{
    static uint32_t cnt;
    cnt += 1;

#if defined(DBI_YUV_SUPPORT) && DBI_YUV_SUPPORT
    if (JD_YUV444_MODE) {
        /* disable YUV mode for DBI */
        bflb_dbi_feature_control(dbi_hd, DBI_CMD_YUV_TO_RGB_ENABLE, false);
        /* Switch BGR888 to RGB565 */
        bflb_dbi_feature_control(dbi_hd, DBI_CMD_INPUT_PIXEL_FORMAT, DBI_PIXEL_INPUT_FORMAT_RGB_565);
    }
    bflb_dbi_feature_control(dbi_hd, DBI_CMD_CLEAR_TX_FIFO, 0);
#endif

    /* lcd async int come */
    LOG_I("lcd async int come, cnt: %d\r\n", cnt);
}

int lcd_disp(struct IODEV *dev)
{
    /* wait flush done */
    while (lcd_draw_is_busy()) {
        LOG_E("lcd_busy\r\n");
    };

#if defined(DBI_YUV_SUPPORT) && DBI_YUV_SUPPORT
    if (JD_YUV444_MODE) {
        /* Switch RGB565 to YUV444 */
        bflb_dbi_feature_control(dbi_hd, DBI_CMD_INPUT_PIXEL_FORMAT, DBI_PIXEL_INPUT_FORMAT_BGR_888);
        bflb_dbi_feature_control(dbi_hd, DBI_CMD_YUV_TO_RGB_ENABLE, true);
    }
#endif

    LOG_I("disp addr: %p\r\n", (void *)dev->out_img_buf[dev->out_img_buf_index]);

    /* start draw lcd (async) */
    lcd_draw_picture_nonblocking(0, 0, dev->out_img_w - 1, dev->out_img_h - 1, (void *)dev->out_img_buf[dev->out_img_buf_index]);

    return 0;
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
    struct IODEV dev;

    board_init();

    LOG_I("tpjdec display test\r\n");

    dbi_hd = bflb_device_get_by_name("dbi");

    /* lcd init */
    lcd_init();
    lcd_set_dir(1, 0);

    LOG_I("LCD init done\r\n");

    /* register async callback */
    lcd_async_callback_register(flush_async_callback);

    /* clean lcd */
    lcd_clear(LCD_COLOR_RGB(0x10, 0x10, 0x10));

    /* disp font */
    lcd_draw_str_ascii16(20, 20, LCD_COLOR_RGB(0xff, 0x00, 0x00), LCD_COLOR_RGB(0x00, 0x00, 0x00), (uint8_t *)"Hello World !", 100);
    lcd_draw_str_ascii16(20, 40, LCD_COLOR_RGB(0x00, 0xff, 0x00), LCD_COLOR_RGB(0x00, 0x00, 0x00), (uint8_t *)"dbi lcd test.", 100);

    lcd_draw_rectangle(0, 0, lcd_max_x, lcd_max_y, LCD_COLOR_RGB(0xff, 0x00, 0x00));
    lcd_draw_rectangle(5, 5, lcd_max_x - 5, lcd_max_y - 5, LCD_COLOR_RGB(0x00, 0xff, 0x00));
    lcd_draw_rectangle(10, 10, lcd_max_x - 10, lcd_max_y - 10, LCD_COLOR_RGB(0x00, 0x00, 0xff));

    bflb_mtimer_delay_ms(500);

#ifdef CONFIG_FATFS
    /* fatfs_init */
    ret = fatfs_init();
    if (ret < 0) {
        while (1) {
        };
    }
#endif

    memset(&jpeg_dev, 0, sizeof(jpeg_dev));

    while (1) {
        for (int i = 0; i < 5; i++) {
            /* load jpeg data */
            jpeg_raw_load(&dev, i);

            /* dec */
            jpeg_dec(&dev);

            /* display */
            lcd_disp(&dev);

            bflb_mtimer_delay_ms(1000);
        }
    }

    return 0;
}
