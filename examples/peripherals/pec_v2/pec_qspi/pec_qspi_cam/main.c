#include "board.h"
#include "bflb_pec_v2_instance.h"
#include "image_sensor.h"
#include "bflb_dma.h"
#include "sensor/common.h"

struct bflb_device_s *i2c0;
struct bflb_device_s *dma0_ch0;
struct bflb_device_s *pec_cam;
struct bflb_pec_qspi_cam_s *cam_cfg;

__attribute((aligned(BFLB_CACHE_LINE_SIZE))) ATTR_NOINIT_PSRAM_SECTION uint8_t framebuffer[640 * 480 * 4];
struct bflb_dma_channel_lli_transfer_s transfers[1];
struct bflb_dma_channel_lli_pool_s lli[200];

void dma0_ch0_isr(void *arg)
{
    static uint32_t dma_tc_flag0 = 0;
    printf("tc done, flag = %u\r\n", dma_tc_flag0++);
    if ((dma_tc_flag0 % 50) == 0) {
        bflb_pec_qspi_cam_stop(pec_cam);
        bflb_l1c_dcache_invalidate_range(framebuffer, sizeof(framebuffer));
        for (int i = 0; i < cam_cfg->resolution_x * cam_cfg->resolution_y * cam_cfg->pixel_bits / 8; i++) {
            if (i % 16 == 0) {
                printf("\r\n");
            }
            printf("%02X ", framebuffer[i]);
        }
        bflb_pec_qspi_cam_start(pec_cam, cam_cfg);
    }
}

int main(void)
{
    struct image_sensor_config_s *sensor_config;
    struct bflb_dma_channel_config_s config;

    board_init();
    board_pec_qspi_cam_gpio_init();

    i2c0 = bflb_device_get_by_name("i2c0");
    printf("i2c0 = 0x%08lX\r\n", i2c0);
    dma0_ch0 = bflb_device_get_by_name("dma0_ch0");
    printf("dma0_ch0 = 0x%08lX\r\n", dma0_ch0);
    pec_cam = bflb_device_get_by_name("pec_sm0");
    printf("pec_cam = 0x%08lX\r\n", pec_cam);

    if (image_sensor_scan(i2c0, &sensor_config)) {
        printf("\r\nSensor name: %s\r\n", sensor_config->name);
        printf("Sensor resolution: %d x %d\r\n", sensor_config->resolution_x, sensor_config->resolution_y);
    } else {
        printf("\r\nError! Can't identify sensor!\r\n");
        while (1) {}
    }
    cam_cfg = bflb_pec_qspi_cam_get_cfg(sensor_config->name);
    if (cam_cfg == NULL) {
        printf("Error! No matched config for this sensor!\r\n");
        while (1) {}
    }

    config.direction = DMA_PERIPH_TO_MEMORY;
    config.src_req = DMA_REQUEST_PEC_SM0_RX;
    config.dst_req = DMA_REQUEST_NONE;
    config.src_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
    config.dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    config.src_burst_count = DMA_BURST_INCR8;
    config.dst_burst_count = DMA_BURST_INCR8;
    config.src_width = DMA_DATA_WIDTH_16BIT;
    config.dst_width = DMA_DATA_WIDTH_16BIT;
    bflb_dma_channel_init(dma0_ch0, &config);
    bflb_dma_channel_irq_attach(dma0_ch0, dma0_ch0_isr, NULL);
    transfers[0].src_addr = (uint32_t)DMA_ADDR_PEC_SM0_RDR;
    transfers[0].dst_addr = (uint32_t)framebuffer;
    transfers[0].nbytes = sensor_config->resolution_x * sensor_config->resolution_y * cam_cfg->pixel_bits / 8;
    int used_count = bflb_dma_channel_lli_reload(dma0_ch0, lli, sizeof(lli) / sizeof(lli[0]), transfers, 1);
    bflb_dma_channel_lli_link_head(dma0_ch0, lli, used_count);
    bflb_dma_channel_start(dma0_ch0);

    bflb_mtimer_delay_us(cam_cfg->delay_first_us);
    cam_cfg->resolution_x = sensor_config->resolution_x;
    cam_cfg->resolution_y = sensor_config->resolution_y;
    bflb_pec_qspi_cam_init(pec_cam, cam_cfg);
    bflb_pec_qspi_cam_start(pec_cam, cam_cfg);

    printf("pec_qspi_cam start\r\n");
    while (1) {
        bflb_mtimer_delay_ms(1000);
    }
}
