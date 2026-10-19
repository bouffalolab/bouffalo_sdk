#ifndef SDIO_MSGS_HANDLERS_H_GZSO2U1M
#define SDIO_MSGS_HANDLERS_H_GZSO2U1M

#include "bl_sdio_eth.h"
#include "msg_handlers.h"

int send_hello(struct bl_eth_device *dev);
int send_eth_frame(struct bl_eth_device *dev, struct sk_buff *skb);
void sdio_read_callback(void *cb_arg, const void *data_ptr, uint8_t num_of_pdu, const uint16_t data_len);

/**
 * @brief Send tty data. len should be limited around <= 2000.
 */
int send_tty_frame(struct bl_eth_device *dev, uint16_t tty_id, const void *data, size_t len);

#endif /* end of include guard: SDIO_MSGS_HANDLERS_H_GZSO2U1M */
