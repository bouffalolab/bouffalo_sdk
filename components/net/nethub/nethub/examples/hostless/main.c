/**
 * @file main.c
 * @brief Hostless application example
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "../../include/nethub.h"
#include "../../include/nethub_types.h"
#include "../../include/nh_forward.h"
#include "../../include/nh_log.h"

/* Forwarding rules configuration function declaration */
const nh_forward_config_t* hostless_get_forward_config(void);

/* Example program debug output */
#define EXAMPLE_DEBUG(fmt, ...) printf("[EXAMPLE] " fmt, ##__VA_ARGS__)

/* Simulated packet buffer */
static char test_data[256];

/* Create test packet */
static nh_skb_t* create_test_skb(const char* data, const char* interface_name)
{
    nh_skb_t* skb = malloc(sizeof(nh_skb_t));
    if (!skb) {
        EXAMPLE_DEBUG("%s: Failed to allocate skb\n", interface_name);
        return NULL;
    }

    snprintf(test_data, sizeof(test_data), "[%s] %s", interface_name, data);

    skb->data = (uint8_t*)test_data;
    skb->len = strlen(test_data);
    skb->next = NULL;
    skb->cb_priv = NULL;
    skb->cb_arg = NULL;
    skb->free_cb = NULL;

    return skb;
}

/* Free test packet */
static void free_test_skb(void* priv, void* frame_elem)
{
    NH_UNUSED(priv);  /* Avoid unused parameter warning */
    nh_skb_t* skb = (nh_skb_t*)frame_elem;
    if (skb) {
        free(skb);
    }
}

/* STA interface input callback */
static nh_forward_result_t sta_input_callback(nh_skb_t *skb, void *arg)
{
    NH_UNUSED(arg);  /* Avoid unused parameter warning */
    EXAMPLE_DEBUG("STA INPUT received data: %s\n", skb->data);
    return NH_FORWARD_CONTINUE;
}

/* STA interface output callback */
static nh_forward_result_t sta_output_callback(nh_skb_t *skb, void *arg)
{
    NH_UNUSED(arg);  /* Avoid unused parameter warning */
    EXAMPLE_DEBUG("STA OUTPUT sent data: %s\n", skb->data);
    return NH_FORWARD_CONTINUE;
}

/* TCPIPSTA interface input callback */
static nh_forward_result_t tcpipsta_input_callback(nh_skb_t *skb, void *arg)
{
    NH_UNUSED(arg);  /* Avoid unused parameter warning */
    EXAMPLE_DEBUG("TCPIPSTA INPUT received data: %s (simulate TCP/IP protocol stack processing)\n", skb->data);

    /* Simulate return value after TCP/IP processing */
    static int call_count = 0;
    call_count++;

    /* Return STOP every 3 times, otherwise return CONTINUE */
    if (call_count % 3 == 0) {
        EXAMPLE_DEBUG("TCPIPSTA data consumption complete, returning NH_FORWARD_STOP\n");
        return NH_FORWARD_STOP;
    } else {
        EXAMPLE_DEBUG("TCPIPSTA data processing complete, returning NH_FORWARD_CONTINUE\n");
        return NH_FORWARD_CONTINUE;
    }
}

/* TCPIPSTA interface output callback */
static nh_forward_result_t tcpipsta_output_callback(nh_skb_t *skb, void *arg)
{
    NH_UNUSED(arg);  /* Avoid unused parameter warning */
    EXAMPLE_DEBUG("TCPIPSTA OUTPUT sent data: %s\n", skb->data);
    return NH_FORWARD_CONTINUE;
}

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
    EXAMPLE_DEBUG("AP OUTPUT sent data: %s\n", skb->data);
    return NH_FORWARD_CONTINUE;
}

/* TCPIPAP interface input callback */
static nh_forward_result_t tcpipap_input_callback(nh_skb_t *skb, void *arg)
{
    NH_UNUSED(arg);  /* Avoid unused parameter warning */
    EXAMPLE_DEBUG("TCPIPAP INPUT received data: %s (simulate TCP/IP AP processing)\n", skb->data);
    return NH_FORWARD_CONTINUE;
}

/* TCPIPAP interface output callback */
static nh_forward_result_t tcpipap_output_callback(nh_skb_t *skb, void *arg)
{
    NH_UNUSED(arg);  /* Avoid unused parameter warning */
    EXAMPLE_DEBUG("TCPIPAP OUTPUT sent data: %s\n", skb->data);
    return NH_FORWARD_CONTINUE;
}

/* Interface operation structure */
static const struct nhif_ops sta_ops = {
    .type = NHIF_TYPE_STA,
    .init = NULL,
    .deinit = NULL,
    .input = sta_input_callback,
    .output = sta_output_callback,
};

static const struct nhif_ops tcpipsta_ops = {
    .type = NHIF_TYPE_TCPIPSTA,
    .init = NULL,
    .deinit = NULL,
    .input = tcpipsta_input_callback,
    .output = tcpipsta_output_callback,
};

static const struct nhif_ops ap_ops = {
    .type = NHIF_TYPE_AP,
    .init = NULL,
    .deinit = NULL,
    .input = ap_input_callback,
    .output = ap_output_callback,
};

static const struct nhif_ops tcpipap_ops = {
    .type = NHIF_TYPE_TCPIPAP,
    .init = NULL,
    .deinit = NULL,
    .input = tcpipap_input_callback,
    .output = tcpipap_output_callback,
};

/* Test forwarding rules */
static void test_forwarding_rules(void)
{
    EXAMPLE_DEBUG("Starting forwarding rules test\n");

    /* Test 1: STA → TCPIPSTA forwarding */
    EXAMPLE_DEBUG("Test 1: STA → TCPIPSTA forwarding\n");
    nh_skb_t* skb1 = create_test_skb("Hello from STA", "STA");
    if (skb1) {
        skb1->free_cb = free_test_skb;
        skb1->cb_priv = skb1;
        EXAMPLE_DEBUG("Sending STA packet: %s\n", skb1->data);
        nethub_process_input(skb1, NHIF_TYPE_STA);
        EXAMPLE_DEBUG("STA forwarding test complete\n");
    } else {
        EXAMPLE_DEBUG("ERROR: Unable to create STA packet\n");
    }

    /* Test 2: AP → TCPIPAP forwarding */
    EXAMPLE_DEBUG("Test 2: AP → TCPIPAP forwarding\n");
    nh_skb_t* skb2 = create_test_skb("Hello from AP", "AP");
    if (skb2) {
        skb2->free_cb = free_test_skb;
        skb2->cb_priv = skb2;
        EXAMPLE_DEBUG("Sending AP packet\n");
        nethub_process_input(skb2, NHIF_TYPE_AP);
        EXAMPLE_DEBUG("AP forwarding test complete\n");
    }

    /* Test 3: TCPIPAP → AP forwarding */
    EXAMPLE_DEBUG("Test 3: TCPIPAP → AP forwarding\n");
    nh_skb_t* skb3 = create_test_skb("Hello from TCPIPAP", "TCPIPAP");
    if (skb3) {
        skb3->free_cb = free_test_skb;
        skb3->cb_priv = skb3;
        EXAMPLE_DEBUG("Sending TCPIPAP packet\n");
        nethub_process_input(skb3, NHIF_TYPE_TCPIPAP);
        EXAMPLE_DEBUG("TCPIPAP forwarding test complete\n");
    }

    /* Test 4: TCPIPSTA → STA forwarding */
    EXAMPLE_DEBUG("Test 4: TCPIPSTA → STA forwarding\n");
    nh_skb_t* skb4 = create_test_skb("Hello from TCPIPSTA", "TCPIPSTA");
    if (skb4) {
        skb4->free_cb = free_test_skb;
        skb4->cb_priv = skb4;
        EXAMPLE_DEBUG("Sending TCPIPSTA packet\n");
        nethub_process_input(skb4, NHIF_TYPE_TCPIPSTA);
        EXAMPLE_DEBUG("TCPIPSTA forwarding test complete\n");
    }

    EXAMPLE_DEBUG("Forwarding rules test complete\n");
}

int main(void)
{
    printf("=== nethub Hostless Application Example ===\n");

    /* Initialize nethub framework */
    int ret = nethub_init(hostless_get_forward_config());
    if (ret != NH_OK) {
        printf("nethub initialization failed: %d\n", ret);
        return -1;
    }

    /* Start nethub framework */
    ret = nethub_start();
    if (ret != NH_OK) {
        printf("nethub startup failed: %d\n", ret);
        return -1;
    }

    /* Print forwarding rules configuration */
    printf("\n--- Forwarding Rules Configuration ---\n");
    nh_forward_print_rules();
    printf("----------------------\n");

    EXAMPLE_DEBUG("Hostless application started\n");

    /* Register all interfaces */
    EXAMPLE_DEBUG("Registering network interfaces\n");

    ret = nhif_register(&sta_ops, NULL);
    if (ret != NH_OK) {
        EXAMPLE_DEBUG("Failed to register STA interface: %d\n", ret);
        goto cleanup;
    }
    EXAMPLE_DEBUG("STA interface registered successfully\n");

    ret = nhif_register(&tcpipsta_ops, NULL);
    if (ret != NH_OK) {
        EXAMPLE_DEBUG("Failed to register TCPIPSTA interface: %d\n", ret);
        goto cleanup;
    }
    EXAMPLE_DEBUG("TCPIPSTA interface registered successfully\n");

    ret = nhif_register(&ap_ops, NULL);
    if (ret != NH_OK) {
        EXAMPLE_DEBUG("Failed to register AP interface: %d\n", ret);
        goto cleanup;
    }
    EXAMPLE_DEBUG("AP interface registered successfully\n");

    ret = nhif_register(&tcpipap_ops, NULL);
    if (ret != NH_OK) {
        EXAMPLE_DEBUG("Failed to register TCPIPAP interface: %d\n", ret);
        goto cleanup;
    }
    EXAMPLE_DEBUG("TCPIPAP interface registered successfully\n");

    EXAMPLE_DEBUG("All interfaces registered successfully\n");

    /* Test forwarding rules */
    test_forwarding_rules();

    EXAMPLE_DEBUG("Application execution complete, cleaning up\n");

cleanup:
    /* Deinitialize nethub framework */
    nethub_stop();
    nethub_deinit();

    EXAMPLE_DEBUG("Hostless application closed\n");
    return 0;
}