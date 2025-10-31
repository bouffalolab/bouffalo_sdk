#include "bflb_mtimer.h"
#include "bflb_mjdec_v2.h"
#include "board.h"
#include "format.h"

#define MJDEC_FORMAT     MJDEC_FORMAT_RGB888
#define MJDEC_BUFFER_ROW 16

static __attribute__((aligned(16))) ATTR_NOCACHE_RAM_SECTION uint8_t jpeg_buffer[] = {
#include "picture/picture.h"
};
static __attribute__((aligned(16))) ATTR_NOCACHE_NOINIT_RAM_SECTION uint8_t out_buffer[48 * 1024];
static struct bflb_mjdec_config_s config = {
    .format = MJDEC_FORMAT,
    .alpha = 0x0,
    .has_header = true,
    .parse_header = true,
    .input_bufaddr = (uint32_t)(uintptr_t *)jpeg_buffer,
    .output_bufaddr0 = (uint32_t)(uintptr_t *)out_buffer,
    .output_bufaddr1 = 0,
    .row_of_interrupt = MJDEC_BUFFER_ROW,
    .row_of_pause = MJDEC_BUFFER_ROW,
    .row_of_addr_loop = MJDEC_BUFFER_ROW * 2,
};
static struct bflb_device_s *mjdec;
static struct bflb_mjdec_parse_s parse;
static volatile int flag_complete = 0;
static volatile int flag_parse = 0;

void mjdec_isr(int irq, void *arg)
{
    uint8_t *p;
    uint32_t length;
    uint32_t intstatus;
    
    intstatus = bflb_mjdec_get_intstatus(mjdec);
    bflb_mjdec_int_clear(mjdec, intstatus);
    if (intstatus & MJDEC_INT_ROW_COMPLETE) {
        if (flag_parse == 0) {
            flag_parse = 1;
            bflb_mjdec_get_parse_result(mjdec, &parse);
            printf("header length = %u\r\n", parse.header_length);
            printf("width  = %u\r\n", parse.resolution_x);
            printf("height = %u\r\n", parse.resolution_y);
            printf("yy_horizon_sampling  = %u\r\n", parse.samp_yy_horizon);
            printf("yy_vertical_sampling = %u\r\n", parse.samp_yy_vertical);
            printf("cb_horizon_sampling  = %u\r\n", parse.samp_cb_horizon);
            printf("cb_vertical_sampling = %u\r\n", parse.samp_cb_vertical);
            printf("cr_horizon_sampling  = %u\r\n", parse.samp_cr_horizon);
            printf("cr_vertical_sampling = %u\r\n", parse.samp_cr_vertical);
        }
        if ((MJDEC_FORMAT == MJDEC_FORMAT_NRGB8888) || (MJDEC_FORMAT == MJDEC_FORMAT_NBGR8888)) {
            length = parse.resolution_x * 4 * MJDEC_BUFFER_ROW;
        } else if ((MJDEC_FORMAT == MJDEC_FORMAT_RGB888) || (MJDEC_FORMAT == MJDEC_FORMAT_BGR888)) {
            length = parse.resolution_x * 3 * MJDEC_BUFFER_ROW;
        } else if (MJDEC_FORMAT == MJDEC_FORMAT_GRAY) {
            length = parse.resolution_x * 1 * MJDEC_BUFFER_ROW;
        } else {
            length = parse.resolution_x * 2 * MJDEC_BUFFER_ROW;
        }
        if (bflb_mjdec_get_row_count_pause(mjdec) % (MJDEC_BUFFER_ROW * 2)) {
            p = out_buffer;
        } else {
            p = out_buffer + length;
        }
        format_print_data(p, length);
        
        if (bflb_mjdec_get_row_count_pause(mjdec) >= parse.resolution_y) {
            printf("mjdec interrupt row finnal.\r\n");
        } else {
            bflb_mjdec_set_row_count_pause(mjdec, bflb_mjdec_get_row_count_pause(mjdec) + MJDEC_BUFFER_ROW);
            // printf("mjdec interrupt row.\r\n");
        }
    }
    if (intstatus & MJDEC_INT_ONE_FRAME) {
        printf("mjdec interrupt one frame.\r\n");
        flag_complete = 1;
    }
    if (intstatus & MJDEC_INT_BUS_ERROR) {
        printf("mjdec interrupt bus error.\r\n");
    }
    if (intstatus & MJDEC_INT_PARSE_ERROR) {
        printf("mjdec interrupt parse error.\r\n");
    }
    if (intstatus & MJDEC_INT_DHT_ERROR) {
        printf("mjdec interrupt dht error.\r\n");
    }
}

int main(void)
{
    board_init();

    mjdec = bflb_device_get_by_name(BFLB_NAME_MJDEC);
    printf("mjdec = 0x%08lX\r\n", mjdec);

    bflb_mjdec_deinit(mjdec);
    bflb_mjdec_init(mjdec, &config);
    bflb_mjdec_int_enable(mjdec, MJDEC_INT_ALL, true);
    bflb_irq_attach(mjdec->irq_num, mjdec_isr, NULL);
    bflb_irq_enable(mjdec->irq_num);
    bflb_mjdec_start(mjdec);

    while (flag_complete == 0);
    printf("mjdec decode complete.\r\n");

    // format_convert(&config, &parse);

    return 0;
}