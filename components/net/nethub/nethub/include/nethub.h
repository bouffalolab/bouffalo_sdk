/**
 * @file nethub.h
 * @brief nethub framework main interface
 */

#ifndef _NETHUB_H
#define _NETHUB_H

#include "nethub_types.h"
#include "nhif_ops.h"
#include "nh_forward.h"

/**
 * @brief nethub framework initialization
 * @param config Forwarding rule configuration, use default when NULL
 * @return 0 for success, negative number for error code
 */
int nethub_init(const nh_forward_config_t *config);

/**
 * @brief nethub framework deinitialization
 * @return 0 for success, negative number for error code
 */
int nethub_deinit(void);

/**
 * @brief Start nethub framework
 * @return 0 for success, negative number for error code
 */
int nethub_start(void);

/**
 * @brief Stop nethub framework
 * @return 0 for success, negative number for error code
 */
int nethub_stop(void);

/**
 * @brief Process interface input data
 * @param skb Packet buffer
 * @param src_type Source interface type
 * @return Forwarding decision result
 */
nh_forward_result_t nethub_process_input(nh_skb_t *skb, nhif_type_t src_type);

/**
 * @brief Send data to specified interface
 * @param skb Packet buffer
 * @param dst_type Target interface type
 * @return Forwarding decision result
 */
nh_forward_result_t nethub_send_output(nh_skb_t *skb, nhif_type_t dst_type);

/**
 * @brief Get nethub framework status
 * @return true if initialized, false if not initialized
 */
bool nethub_is_initialized(void);

#endif /* _NETHUB_H */