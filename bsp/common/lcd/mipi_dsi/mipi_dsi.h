#ifndef __MIPI_DSI_H__
#define __MIPI_DSI_H__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "bflb_dsi.h"

#ifdef __cplusplus
extern "C" {
#endif

/* request ACK and LPM flags */
#define MIPI_DSI_MSG_REQ_ACK (1U << 0)
#define MIPI_DSI_MSG_USE_LPM (1U << 1)
#define MIPI_DSI_DCS_TEAR_MODE_VBLANK   0
#define MIPI_DSI_DCS_TEAR_MODE_VHBLANK  1
#define MIPI_DSI_DATA_FMT_YUV422 0x2C /*!< DSI data format:YUV 422 8bits */
#define MIPI_DSI_DATA_FMT_RGB565 0x0E /*!< DSI data format:RGB565 */
#define MIPI_DSI_DATA_FMT_RGB666 0x2E /*!< DSI data format:RGB666 */
#define MIPI_DSI_DATA_FMT_RGB888 0x3E /*!< DSI data format:RGB888 */

/* Operating mode flags (BSP-visible) */
#define MIPI_DSI_MODE_LPM (1U << 0)

/* Minimal MIPI DSI device struct used by HAL-style callers */
struct mipi_dsi_device {
    void *dev;
    void *host;
    char name[20];
    unsigned int channel;
    unsigned int lanes;
    int format;
    unsigned long mode_flags;
    unsigned long hs_rate;
    unsigned long lp_rate;
};

struct drm_dsc_picture_parameter_set; /* forward-declare (PPS is 128 bytes) */

struct mipi_dsi_msg {
    uint8_t channel;
    uint8_t type;
    uint16_t flags;
    size_t tx_len;
    const void *tx_buf;
    size_t rx_len;
    void *rx_buf;
};

struct mipi_dsi_packet {
    size_t size;
    uint8_t header[4];
    size_t payload_length;
    const uint8_t *payload;
};

/* Prototypes for HAL-style APIs (implemented in mipi_dsi.c) */

bool mipi_dsi_packet_format_is_short(uint8_t type);
bool mipi_dsi_packet_format_is_long(uint8_t type);
int mipi_dsi_create_packet(struct mipi_dsi_packet *packet,
                           const struct mipi_dsi_msg *msg);

int mipi_dsi_controller_init(const struct mipi_dsi_device *dsi);
int mipi_dsi_shutdown_peripheral(const struct mipi_dsi_device *dsi);
int mipi_dsi_turn_on_peripheral(const struct mipi_dsi_device *dsi);
int mipi_dsi_set_maximum_return_packet_size(const struct mipi_dsi_device *dsi,
                                            uint16_t value);
int mipi_dsi_compression_mode(const struct mipi_dsi_device *dsi, bool enable);
int mipi_dsi_picture_parameter_set(const struct mipi_dsi_device *dsi,
                                      const struct drm_dsc_picture_parameter_set *pps);

int mipi_dsi_generic_write(const struct mipi_dsi_device *dsi, const void *payload,
                              size_t size);
int mipi_dsi_generic_read(const struct mipi_dsi_device *dsi, const void *params,
                             size_t num_params, void *data, size_t size);
int mipi_dsi_dcs_write_buffer(const struct mipi_dsi_device *dsi,
                                 const void *data, size_t len);
int mipi_dsi_dcs_write(const struct mipi_dsi_device *dsi, uint8_t cmd,
                          const void *data, size_t len);
int mipi_dsi_dcs_read(const struct mipi_dsi_device *dsi, uint8_t cmd, void *data,
                         size_t len);
int mipi_dsi_dcs_nop(const struct mipi_dsi_device *dsi);
int mipi_dsi_dcs_soft_reset(const struct mipi_dsi_device *dsi);
int mipi_dsi_dcs_get_power_mode(const struct mipi_dsi_device *dsi, uint8_t *mode);
int mipi_dsi_dcs_get_pixel_format(const struct mipi_dsi_device *dsi, uint8_t *format);
int mipi_dsi_dcs_enter_sleep_mode(const struct mipi_dsi_device *dsi);
int mipi_dsi_dcs_exit_sleep_mode(const struct mipi_dsi_device *dsi);
int mipi_dsi_dcs_set_display_off(const struct mipi_dsi_device *dsi);
int mipi_dsi_dcs_set_display_on(const struct mipi_dsi_device *dsi);
int mipi_dsi_dcs_set_column_address(const struct mipi_dsi_device *dsi, uint16_t start,
                                    uint16_t end);
int mipi_dsi_dcs_set_page_address(const struct mipi_dsi_device *dsi, uint16_t start,
                                  uint16_t end);
int mipi_dsi_dcs_set_tear_off(const struct mipi_dsi_device *dsi);
int mipi_dsi_dcs_set_tear_on(const struct mipi_dsi_device *dsi,
                             int mode);
int mipi_dsi_dcs_set_pixel_format(const struct mipi_dsi_device *dsi, uint8_t format);
int mipi_dsi_dcs_set_tear_scanline(const struct mipi_dsi_device *dsi, uint16_t scanline);
int mipi_dsi_dcs_set_display_brightness(const struct mipi_dsi_device *dsi,
                                        uint16_t brightness);
int mipi_dsi_dcs_get_display_brightness(const struct mipi_dsi_device *dsi,
                                        uint16_t *brightness);

#ifdef __cplusplus
}
#endif

#endif /* __MIPI_DSI_H__ */
