#pragma once

#include <lwip/pbuf.h>
#include <utils_list.h>

// Default high performance off
#ifndef CONFIG_SDIO_HIGH_PERFORMANCE
#define CONFIG_SDIO_HIGH_PERFORMANCE    (0)
#endif

struct sdiowifi_tx_buf {
    struct utils_list buf_list;
    uint32_t window_size;       // capacity
    uint32_t free_size;
    uint32_t una_seq;
};

int sdiowifi_tx_buf_init(struct sdiowifi_tx_buf *tb);
int sdiowifi_tx_buf_lpmem_register(struct sdiowifi_tx_buf *tb);
int sdiowifi_tx_buf_mem_register(struct sdiowifi_tx_buf *tb, void *mem, size_t size);
struct pbuf *sdiowifi_tx_buf_alloc(struct sdiowifi_tx_buf *tb);
void sdiowifi_tx_buf_update_credit(struct sdiowifi_tx_buf *tb);
void sdiowifi_tx_buf_init_done(struct sdiowifi_tx_buf *tb);
bool sdiowifi_tx_buf_is_all_free(struct sdiowifi_tx_buf *tb);

