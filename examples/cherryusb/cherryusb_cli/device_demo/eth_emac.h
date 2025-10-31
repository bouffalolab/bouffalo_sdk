#ifndef _ETH_EMAC_H_
#define _ETH_EMAC_H_

#include "bflb_core.h"
#include "bflb_emac.h"

#define EAMC_BUF_HEAD_SIZE (64)

#define EMAC_TX_BUFF_SIZE  (14 + 1500 + 4)
#define EMAC_TX_BUFF_CNT   (6)

#define EMAC_RX_BUFF_SIZE  (14 + 1500 + 4)
#define EMAC_RX_BUFF_CNT   (10)

typedef void (*eth_emac_event_cb_t)(uint32_t irq_event);

struct emac_debug_info_s {
    /* tx debug info */
    volatile uint32_t tx_push_cnt;
    volatile uint32_t tx_success_cnt;
    volatile uint32_t tx_error_cnt;
    volatile uint64_t tx_total_size;

    /* rx debug info */
    volatile uint32_t rx_push_cnt;
    volatile uint32_t rx_success_cnt;
    volatile uint32_t rx_error_cnt;
    volatile uint32_t rx_busy_cnt;
    volatile uint64_t rx_total_size;
};

extern int eth_emac_init(void);
extern int eth_emac_deinit(void);
extern void eth_emac_restart(void);
extern void eth_emac_event_cb_register(eth_emac_event_cb_t cb);
extern bool eth_link_state_update(void);

extern int eth_emac_tx_buff_get(struct bflb_emac_trans_desc_s *trans_desc, uint32_t timeout);
extern int eth_emac_tx_buff_push(struct bflb_emac_trans_desc_s *trans_desc);
extern int eth_emac_rx_data_get(struct bflb_emac_trans_desc_s *trans_desc, uint32_t timeout);
extern int eth_emac_rx_data_free(struct bflb_emac_trans_desc_s *trans_desc);

void eth_eamc_info_dump(void);

extern struct emac_debug_info_s emac_debug_info;

#endif