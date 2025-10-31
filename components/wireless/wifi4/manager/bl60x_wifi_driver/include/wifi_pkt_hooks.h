#pragma once

#include <stdbool.h>

typedef void *(*bl_pkt_eth_input_hook_cb_t)(bool is_sta, void *pkt, void *arg);

extern bl_pkt_eth_input_hook_cb_t bl_wifi_pkt_eth_input_hook;
extern void *bl_wifi_pkt_eth_input_hook_arg;

/**
 * @brief  Register a callback that is called before an eth packet is passed to TCP/IP stack.
 *
 * @param cb      Callback. cb may inspect, manipulate or even duplicate the packet.
 *                The first argument passed to cb is a struct pbuf *, second being cb_arg.
 *                cb should return NULL if cb drops the packet(cb might need to free it if so).
 *                If non-NULL is returned by cb, the returned pbuf will be passed to TCP/IP stack.
 * @param cb_arg  Callback argument.
 */
void bl_pkt_eth_input_hook_register(bl_pkt_eth_input_hook_cb_t cb, void *cb_arg);

/**
 * @brief  Unregister the callback previously registered.
 */
void bl_pkt_eth_input_hook_unregister(void);

typedef void *(*bl_pkt_eth_output_hook_cb_t)(bool is_sta, void *pkt, void *arg);

extern bl_pkt_eth_output_hook_cb_t bl_wifi_pkt_eth_output_hook;
extern void *bl_wifi_pkt_eth_output_hook_arg;


/**
 * @brief  Register a callback that is called before an eth packet from TCP/IP stack is sent by Wi-Fi.
 *
 * @param cb      Callback. cb may inspect, manipulate or even duplicate the packet.
 *                cb should return NULL if it drops the packet(cb should NOT free it if so).
 *                If non-NULL is returned by cb, the returned pbuf will be sent by Wi-Fi.
 * @param cb_arg  Callback argument.
 */
void bl_pkt_eth_output_hook_register(bl_pkt_eth_output_hook_cb_t cb, void *cb_arg);

/**
 * @brief  Unregister the callback previously registered.
 */
void bl_pkt_eth_output_hook_unregister(void);
