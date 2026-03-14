#include "transport_spi.h"

#define DBG_TAG "NETHUB_SPI"
#include "log.h"

static int transport_spi_not_supported(const char *what)
{
    LOG_E("%s not implemented\r\n", what);
    return NETHUB_ERR_NOT_SUPPORTED;
}

static int transport_spi_init(void)
{
    return transport_spi_not_supported("spi host backend");
}

static nethub_route_result_t transport_spi_input(nethub_frame_t *frame, void *arg)
{
    NETHUB_UNUSED(frame);
    NETHUB_UNUSED(arg);
    return NETHUB_ROUTE_ERROR;
}

static nethub_route_result_t transport_spi_output(nethub_frame_t *frame, void *arg)
{
    NETHUB_UNUSED(frame);
    NETHUB_UNUSED(arg);
    return NETHUB_ROUTE_ERROR;
}

static int transport_spi_ctrlpath_upld_send(uint8_t *data_buff, uint32_t data_size)
{
    NETHUB_UNUSED(data_buff);
    NETHUB_UNUSED(data_size);
    return transport_spi_not_supported("spi ctrl upld");
}

static int transport_spi_ctrlpath_dnld_register(nethub_ctrl_rx_cb_t dnld_cb, void *cbpri_arg)
{
    NETHUB_UNUSED(dnld_cb);
    NETHUB_UNUSED(cbpri_arg);
    return transport_spi_not_supported("spi ctrl dnld");
}

const struct nhif_ops nhspi_ops = {
    .type = NETHUB_CHANNEL_SPI,
    .init = transport_spi_init,
    .deinit = NULL,
    .input = transport_spi_input,
    .output = transport_spi_output,
};

const nh_ctrlpath_ops_t nhspi_ctrlpath_ops = {
    .upld_send = transport_spi_ctrlpath_upld_send,
    .dnld_register = transport_spi_ctrlpath_dnld_register,
};
