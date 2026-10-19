#include <wifi_pkt_hooks.h>

#include <stdio.h>

bl_pkt_eth_input_hook_cb_t bl_wifi_pkt_eth_input_hook = NULL;
void *bl_wifi_pkt_eth_input_hook_arg = NULL;

void bl_pkt_eth_input_hook_register(bl_pkt_eth_input_hook_cb_t cb, void *cb_arg)
{
    bl_wifi_pkt_eth_input_hook = cb;
    bl_wifi_pkt_eth_input_hook_arg = cb_arg;
}

void bl_pkt_eth_input_hook_unregister(void)
{
    bl_wifi_pkt_eth_input_hook = NULL;
    bl_wifi_pkt_eth_input_hook_arg = NULL;
}

bl_pkt_eth_output_hook_cb_t bl_wifi_pkt_eth_output_hook = NULL;
void *bl_wifi_pkt_eth_output_hook_arg = NULL;

void bl_pkt_eth_output_hook_register(bl_pkt_eth_output_hook_cb_t cb, void *cb_arg)
{
    bl_wifi_pkt_eth_output_hook = cb;
    bl_wifi_pkt_eth_output_hook_arg = cb_arg;
}

void bl_pkt_eth_output_hook_unregister(void)
{
    bl_wifi_pkt_eth_output_hook = NULL;
    bl_wifi_pkt_eth_output_hook_arg = NULL;
}
