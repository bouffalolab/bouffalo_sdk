#ifndef _ETH_EMAC_H_
#define _ETH_EMAC_H_

#include "bflb_core.h"
#include "bflb_emac.h"

#define EAMC_BUF_HEAD_SIZE (64)

#define EMAC_TX_BUFF_SIZE  (14 + 1500 + 4)
#define EMAC_TX_BUFF_CNT   (6)

#define EMAC_RX_BUFF_SIZE  (14 + 1500 + 4)
#define EMAC_RX_BUFF_CNT   (10)

/* MAC ADDRESS NUM */
#define MAC_ADDR_NUM_0     (0x18)
#define MAC_ADDR_NUM_1     (0xB9)
#define MAC_ADDR_NUM_2     (0x05)
#define MAC_ADDR_NUM_3     (0x12)
#define MAC_ADDR_NUM_4     (0x34)
#define MAC_ADDR_NUM_5     (0x56)
/* MAC ADDRESS ASCII */
#define MAC_ADDR_ASCII_00  ('1')
#define MAC_ADDR_ASCII_01  ('8')
#define MAC_ADDR_ASCII_10  ('B')
#define MAC_ADDR_ASCII_11  ('9')
#define MAC_ADDR_ASCII_20  ('0')
#define MAC_ADDR_ASCII_21  ('5')
#define MAC_ADDR_ASCII_30  ('1')
#define MAC_ADDR_ASCII_31  ('2')
#define MAC_ADDR_ASCII_40  ('3')
#define MAC_ADDR_ASCII_41  ('4')
#define MAC_ADDR_ASCII_50  ('5')
#define MAC_ADDR_ASCII_51  ('6')

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