#ifndef __HOST_OPS_H__
#define __HOST_OPS_H__

#include "bflb_core.h"
#include "bflb_sdh.h"

/* host cfg */
#define SDH_HOST_SUP_BUS_WIDTH_4     (1)
#define SDH_HOST_SUP_BUS_WIDTH_8     (0)
#define SDH_HOST_SUP_SIGNAL_1V8      (0)
#define SDH_HOST_SUP_UHS_I           (0)
#define SDH_HOST_SUP_OCR_3V3         (1)

#define SDH_HOST_ADMA2_HW_DESC_NUM   (16)
#define SDH_HOST_INTER_BLK_BUFF_SEZE (512)

struct sdh_host_transfer_s {
    struct bflb_sdh_cmd_cfg_s *cmd_cfg;
    struct bflb_sdh_data_cfg_s *data_cfg;
};

struct sdh_host_s {
    /* lhal dev */
    struct bflb_device_s *sdh_dev;

    void *osal_semb; /* Binary Semaphore */

#if 0
    bool sbc_cmd_en; /* SET_BLOCK_COUNT for multiblock */
    struct bflb_sdh_cmd_cfg_s *sbc_cmd_cfg;
    bool stop_cmd_en; /* STOP_TRANSMISSION for multiblock */
    struct bflb_sdh_cmd_cfg_s *stop_cmd_cfg;
#endif
    void *internal_buff; /* Internal buffer (nocache) 512-byte */
    uint32_t internal_buff_size;

    /* adma descriptors buff */
    struct bflb_sdh_adma2_hw_desc_s *adma2_hw_desc; /*!< The address of adma2 hardware descriptors. */
    uint32_t adma2_hw_desc_num;

    uint32_t clk_src_hz;
    uint32_t sdh_div;
};

int sdh_host_init(struct sdh_host_s *host);
int sdh_host_deinit(struct sdh_host_s *host);

int sdh_set_bus_width(struct sdh_host_s *host, uint8_t bus_width);

int sdh_set_bus_clock(struct sdh_host_s *host, uint32_t freq, uint32_t *actual_freq);

int sdh_send_active_clk(struct sdh_host_s *host);

int sdh_get_signal_status(struct sdh_host_s *host);

int sdh_force_clock(struct sdh_host_s *host, bool en);

int sdh_set_data_timeout_ms(struct sdh_host_s *host, uint16_t timeout_ms, uint32_t *actual_ms);

int sdh_switch_voltage(struct sdh_host_s *host, uint32_t voltage);

// int sdh_card_detect_init(struct sdh_host_s* host);

int sdh_host_transfer(struct sdh_host_s *host, struct sdh_host_transfer_s *transfer);
int sdh_host_wait_done(struct sdh_host_s *host, struct sdh_host_transfer_s *transfer);

int sdh_mmc_boot_start(struct sdh_host_s *host);
int sdh_mmc_boot_data_read(struct sdh_host_s *host);
int sdh_mmc_boot_exit(struct sdh_host_s *host);

#endif
