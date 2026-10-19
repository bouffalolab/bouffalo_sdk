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

const struct nhif_ops nhspi_ops = {
    .type = NETHUB_CHANNEL_SPI,
    .init = transport_spi_init,
    .deinit = NULL,
    .input = transport_spi_input,
    .output = transport_spi_output,
};
