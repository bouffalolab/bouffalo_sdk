/**
 * @file port_ap.c
 * @brief AP interface port adaptation layer implementation
 */

#include <stdio.h>
#include "nhsdiowifi.h"

/* Debug output for example program */
#define EXAMPLE_DEBUG(fmt, ...) printf("[AP_PORT] " fmt, ##__VA_ARGS__)

/* AP interface input callback */
static nh_forward_result_t ap_input_callback(nh_skb_t *skb, void *arg)
{
    NH_UNUSED(arg);  /* Avoid unused parameter warning */
    EXAMPLE_DEBUG("AP INPUT received data: %s\n", skb->data);
    return NH_FORWARD_CONTINUE;
}

/* AP interface output callback */
static nh_forward_result_t ap_output_callback(nh_skb_t *skb, void *arg)
{
    NH_UNUSED(arg);  /* Avoid unused parameter warning */
    EXAMPLE_DEBUG("AP OUTPUT send data: %s\n", skb->data);
    return NH_FORWARD_CONTINUE;
}

/* AP interface operation structure */
const struct nhif_ops nhap_ops = {
    .type = NHIF_TYPE_AP,
    .init = NULL,
    .deinit = NULL,
    .input = ap_input_callback,
    .output = ap_output_callback,
};