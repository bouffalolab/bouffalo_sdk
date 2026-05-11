#ifndef __LMAC154_DGB_H
#define __LMAC154_DGB_H

#if CONFIG_LMAC154_DBG
#define LMAC154_TX_STATUS_TX_ENH_ACK LMAC154_TX_STATUS_MAX

void lmac154_dbg_trace_init(void);
void lmac154_dbg_output(void);
int lmac154_dbg_cli(int argc, char **argv);

void lmac154_dbg_trace(const char *format, ...);
void lmac154_dbg_trace_tx(uint32_t * DataPtr, uint32_t length, uint32_t base_time, uint32_t delay_time, uint32_t plat_entry_sym, uint32_t chk_time, int ret);
void lmac154_dbg_trace_rxing_ack(char * tag, int is_rx_doing, int rx_acc_symb, int ack_frame_length, int result);
void lmac154_dbg_trace_tx_ack(uint32_t base_time, uint32_t delay_time, int ret);
void lmac154_dbg_trace_write_ack(uint32_t * DataPtr, uint32_t length, int timeout, int ret);
void lmac154_dbg_trace_isr(uint32_t intStatus, uint32_t events, uint32_t filter_error, uint8_t len, uint32_t mpdu[]);
void lmac154_dbg_trace_monitor(char * tag);
uint32_t lmac154_getRxAccSymCnt(void);

uint32_t lmac154_dbg_enter_critical(void);
void lmac154_dbg_exit_critical(uint32_t tag);
#endif

#if CONFIG_LMAC154_DBG_ALL
int lmac154_dbg_triggerTx(uint32_t *DataPtr, uint32_t length, uint32_t csma,
    uint64_t baseTimeUs, uint32_t delayUs);
void lmac154_dbg_txDoneEvent(lmac154_tx_status_t tx_status);
void lmac154_dbg_ackFrameEvent(uint8_t ack_received, uint8_t *rx_buf, uint8_t rx_len);
void lmac154_dbg_rxStartEvent(void);
void lmac154_dbg_rxMhrEvent(uint8_t *rx_buf, uint8_t rx_len, uint8_t pkt_len);
void lmac154_dbg_rxDoneEvent(uint8_t *rx_buf, uint8_t rx_len, uint32_t filter_error);
void lmac154_dbg_reqEnhAckEvent(uint32_t * mpdu);
void lmac154_dbg_hwAutoTxAckDoneEvent(void);
void lmac154_dbg_irq_enable(lmac154_isr_t irq_callback);
#endif

#endif