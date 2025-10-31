
#include <stdio.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "timers.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "usbd_core.h"
#include "bflb_mtimer.h"
#include <bflb_core.h>
#include "board.h"
#include <shell.h>

#include <assert.h>
#include <stdio.h>

//#include "usb.h"
#include "utils.h"

#include "bflb_mtimer.h"
#include "bflb_emac.h"

#include "usbd_core.h"
#include "usbd_cdc.h"
#include "usbd_cdc_ecm.h"
#include "lwip/netif.h"
#include "eth_phy.h"
#include "ephy_general.h"

#include <wifi_pkt_hooks.h>
#include <net_pkt_filter.h>
#include <net_al.h>
#include <fhost.h>
#include "netbus_usbd_cdc_acm.h"

#define DBG_TAG "USBECM"
#include "log.h"

#define NXBD_UPLD_ITEMS     (20)
#define NXBD_DNLD_ITEMS     (11)
#define TX_PBUF_FRAME_LEN   (1514)
#define TX_PBUF_PAYLOAD_LEN (PBUF_LINK_ENCAPSULATION_HLEN + TX_PBUF_FRAME_LEN)

#define ecm_print(...) //

int g_usb_loop = 0;

typedef struct _trans_desc {
    struct pbuf_custom pbuf;
    int                payload_len;
    char               *payload_buf;
} trans_desc_t;

typedef struct _usbecm_desc {
    netbus_usb_cdc_t usbacm;
    QueueHandle_t dnfq;  // download free queue
    QueueHandle_t upvq;  // up valid queue

    TaskHandle_t uptask_hdl;
    TaskHandle_t dntask_hdl;
    SemaphoreHandle_t dnsem;
    SemaphoreHandle_t upsem;

    /* global val */
    trans_desc_t *dnmsg;
    struct pbuf *upmsg;
    uint8_t    dbg_dntask_mode;
    uint8_t    dbg_uptask_mode;
    uint32_t   dbg_uptask_loop_cnt;
    int send_cnt;
    int recv_cnt;
} usbecm_t;

/* MTU */
#define ECM_EMAC_NETIF_MTU (1514)

/* MAC ADDRESS ASCII */
#define MAC_ADDR_ASCII_00  ('1')
#define MAC_ADDR_ASCII_01  ('8')
#define MAC_ADDR_ASCII_10  ('B')
#define MAC_ADDR_ASCII_11  ('9')
#define MAC_ADDR_ASCII_20  ('0')
#define MAC_ADDR_ASCII_21  ('5')
#define MAC_ADDR_ASCII_30  ('D')
#define MAC_ADDR_ASCII_31  ('E')
#define MAC_ADDR_ASCII_40  ('8')
#define MAC_ADDR_ASCII_41  ('6')
#define MAC_ADDR_ASCII_50  ('3')
#define MAC_ADDR_ASCII_51  ('0')

/*!< endpoint address */
#define CDC_ECM_IN_EP      0x81
#define CDC_ECM_OUT_EP     0x02
#define CDC_ECM_INT_EP     0x85

#define USBD_VID           0xFFFF
#define USBD_PID           0xFFFF
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

/*!< config descriptor size */
#define USB_CONFIG_SIZE    (9 + CDC_ECM_DESCRIPTOR_LEN + CDC_ACM_DESCRIPTOR_LEN)

#define CDC_ECM_ETH_STATISTICS_BITMAP 0x00000000

/* str idx = 4 is for mac address: aa:bb:cc:dd:ee:ff*/
#define CDC_ECM_MAC_STRING_INDEX      4

/*!< global descriptor */
static uint8_t cdc_ecm_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xEF, 0x02, 0x01, USBD_VID, USBD_PID, 0x0100, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x04, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    CDC_ECM_DESCRIPTOR_INIT(0x00, CDC_ECM_INT_EP, CDC_ECM_OUT_EP, CDC_ECM_IN_EP, CDC_MAX_MPS, CDC_ECM_ETH_STATISTICS_BITMAP, ECM_EMAC_NETIF_MTU, 0, 0, CDC_ECM_MAC_STRING_INDEX),
    CDC_ACM_DESCRIPTOR_INIT(0x02, CDC_ACM_INT_EP, CDC_ACM_OUT_EP, CDC_ACM_IN_EP, CDC_MAX_MPS, 0x02),
    ///////////////////////////////////////
    /// string0 descriptor
    ///////////////////////////////////////
    USB_LANGID_INIT(USBD_LANGID_STRING),
    ///////////////////////////////////////
    /// string1 descriptor
    ///////////////////////////////////////
    0x14,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'C', 0x00,                  /* wcChar0 */
    'h', 0x00,                  /* wcChar1 */
    'e', 0x00,                  /* wcChar2 */
    'r', 0x00,                  /* wcChar3 */
    'r', 0x00,                  /* wcChar4 */
    'y', 0x00,                  /* wcChar5 */
    'U', 0x00,                  /* wcChar6 */
    'S', 0x00,                  /* wcChar7 */
    'B', 0x00,                  /* wcChar8 */
    ///////////////////////////////////////
    /// string2 descriptor
    ///////////////////////////////////////
    0x2E,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'C', 0x00,                  /* wcChar0 */
    'h', 0x00,                  /* wcChar1 */
    'e', 0x00,                  /* wcChar2 */
    'r', 0x00,                  /* wcChar3 */
    'r', 0x00,                  /* wcChar4 */
    'y', 0x00,                  /* wcChar5 */
    'U', 0x00,                  /* wcChar6 */
    'S', 0x00,                  /* wcChar7 */
    'B', 0x00,                  /* wcChar8 */
    ' ', 0x00,                  /* wcChar9 */
    'C', 0x00,                  /* wcChar10 */
    'D', 0x00,                  /* wcChar11 */
    'C', 0x00,                  /* wcChar12 */
    ' ', 0x00,                  /* wcChar13 */
    'E', 0x00,                  /* wcChar14 */
    'C', 0x00,                  /* wcChar15 */
    'M', 0x00,                  /* wcChar16 */
    ' ', 0x00,                  /* wcChar17 */
    'D', 0x00,                  /* wcChar18 */
    'E', 0x00,                  /* wcChar19 */
    'M', 0x00,                  /* wcChar20 */
    'O', 0x00,                  /* wcChar21 */
    ///////////////////////////////////////
    /// string3 descriptor
    ///////////////////////////////////////
    0x16,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    '2', 0x00,                  /* wcChar0 */
    '0', 0x00,                  /* wcChar1 */
    '2', 0x00,                  /* wcChar2 */
    '2', 0x00,                  /* wcChar3 */
    '1', 0x00,                  /* wcChar4 */
    '2', 0x00,                  /* wcChar5 */
    '3', 0x00,                  /* wcChar6 */
    '4', 0x00,                  /* wcChar7 */
    '5', 0x00,                  /* wcChar8 */
    '6', 0x00,                  /* wcChar9 */
    ///////////////////////////////////////
    /// string4 descriptor
    ///////////////////////////////////////
    0x1A,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    MAC_ADDR_ASCII_00, 0x00,    /* wcChar0 */
    MAC_ADDR_ASCII_01, 0x00,    /* wcChar1 */
    MAC_ADDR_ASCII_10, 0x00,    /* wcChar2 */
    MAC_ADDR_ASCII_11, 0x00,    /* wcChar3 */
    MAC_ADDR_ASCII_20, 0x00,    /* wcChar4 */
    MAC_ADDR_ASCII_21, 0x00,    /* wcChar5 */
    MAC_ADDR_ASCII_30, 0x00,    /* wcChar6 */
    MAC_ADDR_ASCII_31, 0x00,    /* wcChar7 */
    MAC_ADDR_ASCII_40, 0x00,    /* wcChar8 */
    MAC_ADDR_ASCII_41, 0x00,    /* wcChar9 */
    MAC_ADDR_ASCII_50, 0x00,    /* wcChar10 */
    MAC_ADDR_ASCII_51, 0x00,    /* wcChar11 */
#ifdef CONFIG_USB_HS
    ///////////////////////////////////////
    /// device qualifier descriptor
    ///////////////////////////////////////
    0x0a,
    USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER,
    0x00,
    0x02,
    0x02,
    0x02,
    0x01,
    0x40,
    0x01,
    0x00,
#endif
    0x00
};

struct usbd_interface intf0;
struct usbd_interface intf1;
usbecm_t g_usbecm;
volatile uint8_t g_cdc_acm_init_done = 0;
char     s_buf[NXBD_DNLD_ITEMS*TX_PBUF_PAYLOAD_LEN+31] __attribute__((section("SHAREDRAM")));
trans_desc_t dnmsg_desc[NXBD_DNLD_ITEMS] __attribute__((section(".nocache_ram")));// up+dn

static void cdc_ecm_macaddr_init(void)
{
    char wlan_mac[6] = {0};
    char mac_addr[13] = {0};

    wifi_mgmr_sta_mac_get(wlan_mac);
    snprintf(mac_addr, sizeof(mac_addr), "%02x%02x%02x%02x%02x%02x",
             wlan_mac[0], wlan_mac[1], wlan_mac[2], wlan_mac[3], wlan_mac[4], wlan_mac[5]);

    ecm_print("cdc_ecm_macaddr_init: %s\r\n", mac_addr);

    char *mac = &cdc_ecm_descriptor[0x12 + 0x9 + CDC_ECM_DESCRIPTOR_LEN + CDC_ACM_DESCRIPTOR_LEN + 4 + 0x14 + 0x2E + 0x16 + 2];
    for (int i = 0; i < 12; i++) {
        *(mac + (2*i)) = mac_addr[i];
    }
}

/* ecm only supports in linux, and you should input the following command
 *
 * sudo ifconfig enxaabbccddeeff up
 * sudo dhcpclient enxaabbccddeeff
*/
static void cdc_ecm_init(void)
{
    cdc_ecm_macaddr_init();

    usbd_desc_register(0, cdc_ecm_descriptor);
    usbd_add_interface(0, usbd_cdc_ecm_init_intf(&intf0, CDC_ECM_INT_EP, CDC_ECM_OUT_EP, CDC_ECM_IN_EP));
    usbd_add_interface(0, usbd_cdc_ecm_init_intf(&intf1, CDC_ECM_INT_EP, CDC_ECM_OUT_EP, CDC_ECM_IN_EP));

    netbus_usb_cdcacm_init(&g_usbecm.usbacm, 2048, 2048);
    extern void usbd_event_handler(uint8_t busid, uint8_t event);
    usbd_initialize(0, 0, usbd_event_handler);
    ecm_print("%s, line:%d\r\n", __FUNCTION__, __LINE__);
}

void usbd_cdc_ecm_data_send_done(uint32_t nbytes)
{
    static uint32_t s_cnt = 0;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    s_cnt++;
    ecm_print("------------ %s, cnt:%d\r\n", __func__, s_cnt);
    g_usbecm.send_cnt++;
#if 0
    if (xPortIsInsideInterrupt()) {
        printf("[%d] up isr over %d\r\n", xTaskGetTickCountFromISR(), g_usbecm.send_cnt);
    } else {
        printf("[%d] up over %d\r\n", xTaskGetTickCount(), g_usbecm.send_cnt);
    }
#else
    //printf("[%d] up isr over %d\r\n", xTaskGetTickCountFromISR(), g_usbecm.send_cnt);
#endif
    if (xSemaphoreGiveFromISR(g_usbecm.upsem, &xHigherPriorityTaskWoken) != pdTRUE) {
        ecm_print("usb send done error!\r\n");
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void usbd_cdc_ecm_data_recv_done(uint32_t nbytes)
{
    static uint32_t s_cnt = 0;
    s_cnt++;
    ecm_print("------------ %s, cnt:%d\r\n", __func__, s_cnt);

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    g_usbecm.recv_cnt++;

#if 0
    if (xPortIsInsideInterrupt()) {
        printf("[%d] dn isr over %d\r\n", xTaskGetTickCountFromISR(), g_usbecm.recv_cnt);
    } else {
        printf("[%d] dn over %d\r\n", xTaskGetTickCount(), g_usbecm.recv_cnt);
    }
#else
    //printf("[%d] dn isr over %d\r\n", xTaskGetTickCountFromISR(), g_usbecm.recv_cnt);
#endif

    if (g_usbecm.dnmsg) {
        //g_usbecm.dnmsg->payload_len       = nbytes;
        g_usbecm.dnmsg->pbuf.pbuf.len     = nbytes;
        g_usbecm.dnmsg->pbuf.pbuf.tot_len = nbytes;
    }
    if (xSemaphoreGiveFromISR(g_usbecm.dnsem, &xHigherPriorityTaskWoken) != pdTRUE) {
        ecm_print("usb recv done error!\r\n");
    }
#if 0
    {
        static uint32_t interrupt_count = 0;
        static uint32_t last_print_time = 0;

        interrupt_count++;

        // Print once every 1 second
        uint32_t current_time = xTaskGetTickCountFromISR();
        if (current_time - last_print_time >= 1000) {
            printf("recv : %lu pack\n", interrupt_count);
            interrupt_count = 0;
            last_print_time = current_time;
        }
    }
#endif

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

#include "lwip/prot/ethernet.h"
#include "lwip/prot/etharp.h"
#include "lwip/prot/ip.h"
#include "lwip/prot/tcp.h"
#include "lwip/prot/udp.h"

void pbuf_dump(const char *msg, struct pbuf *p)
{
    uint8_t eth_header_data[14];  // Reserve sufficient space
    uint8_t ip_header_data[20];   // The IP header is fixed at 20 bytes.
    struct eth_hdr *eth_header;
    struct ip_hdr *ip_header;

    if (msg && strlen(msg) > 0) {
        printf("\n=== %s ===\n", msg);
    }

    if (p == NULL) {
        printf("pbuf is NULL!\n");
        return;
    }

    // Securely copy the Ethernet header
    if (p->len >= sizeof(eth_header_data)) {
        memcpy(eth_header_data, p->payload, sizeof(eth_header_data));
        eth_header = (struct eth_hdr*)eth_header_data;
    } else {
        printf("Packet too short for Ethernet header\n");
        return;
    }

    printf("Ethernet Header:\n");
    printf("  - Dest MAC:   %02X:%02X:%02X:%02X:%02X:%02X\n",
           eth_header->dest.addr[0], eth_header->dest.addr[1],
           eth_header->dest.addr[2], eth_header->dest.addr[3],
           eth_header->dest.addr[4], eth_header->dest.addr[5]);
    printf("  - Src MAC:    %02X:%02X:%02X:%02X:%02X:%02X\n",
           eth_header->src.addr[0], eth_header->src.addr[1],
           eth_header->src.addr[2], eth_header->src.addr[3],
           eth_header->src.addr[4], eth_header->src.addr[5]);

    uint16_t ether_type = lwip_htons(eth_header->type);
    printf("  - EtherType:  0x%04X\n\n", ether_type);

    // Safely handle the IP header
    if (ether_type == ETHTYPE_IP && p->len >= (sizeof(eth_header_data) + sizeof(ip_header_data))) {
        memcpy(ip_header_data, (uint8_t*)p->payload + sizeof(eth_header_data), sizeof(ip_header_data));
        ip_header = (struct ip_hdr*)ip_header_data;

        printf("IP Header:\n");
        printf("  - Version:    %u\n", (ip_header->_v_hl >> 4));
        printf("  - Header Len: %u bytes\n", (ip_header->_v_hl & 0x0F) * 4);
        printf("  - Protocol:   %u\n", ip_header->_proto);
    }
}

static void custom_free(struct pbuf *p)
{
    static uint32_t s_cnt = 0;
    trans_desc_t *msg = (trans_desc_t *)p;

    ecm_print("========================================== custom_free start:%p, s_cnt:%d\r\n", msg, s_cnt);
    if (msg) {
        while (xQueueSend(g_usbecm.dnfq, &msg, portMAX_DELAY) != pdPASS);
    }
    ecm_print("dn fq:%d, dnmsg:%p, items:%d\r\n",
            uxQueueMessagesWaiting(g_usbecm.dnfq),
            g_usbecm.dnmsg,
            NXBD_DNLD_ITEMS);
    ecm_print("========================================== custom_free done:%p, s_cnt:%d\r\n", msg, s_cnt);
}

int portwifi_eth_tx(trans_desc_t *msg, bool is_sta)
{
    struct pbuf *p = (struct pbuf *)msg;
    net_al_if_t *net_if;

#if 0
    if (pbuf_take(p, msg->payload_buf, msg->payload_len) != ERR_OK) {
        pbuf_free(p);
        return -1;
    }
#else
#endif
    if (!p) {
        printf("alloc error.\r\n");
        pbuf_free(p);
    }

    // get intf
    if (is_sta) {
        net_if = fhost_env.vif[0].net_if;
    } else {
        net_if = fhost_env.vif[1].net_if;
    }
    if ((!net_if) || (!netif_is_up((struct netif *)net_if))) {
        ecm_print(" ========================== net_if:%p, free\r\n", net_if);
        pbuf_free(p);
    	return -1;
    }

    {
        static uint32_t g_fhost_tx_cnt = 0;
        g_fhost_tx_cnt++;
        ecm_print("--------------------------TTT fhost_tx_start cnt:%d msg:%p -> off=(%p-%p)=%d\r\n",
                g_fhost_tx_cnt, msg, msg->payload_buf, msg->pbuf.pbuf.payload, ((uint32_t)msg->pbuf.pbuf.payload-(uint32_t)msg->payload_buf));
    }
    //pbuf_ref(p);

    //pbuf_dump("fhost_tx_start", p);
    if (0 != fhost_tx_start(net_if, p, NULL, NULL)) {
        printf("tx error.\r\n");
        pbuf_free(p);
    } else {
        ecm_print("tx ok.\r\n");
    }
    //pbuf_free(p);
}

int usb_dn_task(void *arg)
{
    BaseType_t result;
    int ret;
    struct pbuf *p;

    while (1) {
        g_usbecm.dbg_dntask_mode = 1;
        // get buf
        result = xQueueReceive(g_usbecm.dnfq, &g_usbecm.dnmsg, portMAX_DELAY);
        if (result != pdPASS) {
            printf("result:%d\r\n", result);
            return -2;  // Timeout or error in receiving the message
        }

        // init pbuf
        g_usbecm.dbg_dntask_mode = 2;
        g_usbecm.dnmsg->pbuf.custom_free_function = custom_free;
        p = pbuf_alloced_custom(PBUF_RAW_TX, TX_PBUF_FRAME_LEN,
                (PBUF_ALLOC_FLAG_DATA_CONTIGUOUS | PBUF_TYPE_ALLOC_SRC_MASK_STD_HEAP),
                &g_usbecm.dnmsg->pbuf, g_usbecm.dnmsg->payload_buf, TX_PBUF_PAYLOAD_LEN);

        // recv buf
        g_usbecm.dbg_dntask_mode = 3;
        ret = usbd_cdc_ecm_start_read((uint8_t *)g_usbecm.dnmsg->pbuf.pbuf.payload, (TX_PBUF_FRAME_LEN));
        if (0 != ret) {
            printf("usbd_cdc_ecm_receive ret:%d\r\n", ret);
            vTaskDelay(200);
            while (xQueueSend(g_usbecm.dnfq, &g_usbecm.dnmsg, portMAX_DELAY) != pdPASS);
            continue;
        }
        g_usbecm.dbg_dntask_mode = 4;
        result = xSemaphoreTake(g_usbecm.dnsem, portMAX_DELAY);

        if (g_usb_loop) {
           while (xQueueSend(g_usbecm.dnfq, &g_usbecm.dnmsg, portMAX_DELAY) != pdPASS);
           g_usbecm.dnmsg = NULL;
           continue;
        } else {
           if (result != pdPASS) {
               printf("usbd_cdc_ecm_receive ok take isr result:%d.\r\n", result);
               while (xQueueSend(g_usbecm.dnfq, &g_usbecm.dnmsg, portMAX_DELAY) != pdPASS);
               continue;
           }
           g_usbecm.dbg_dntask_mode = 5;

           // update pbuf
           portwifi_eth_tx(g_usbecm.dnmsg, 0);
           g_usbecm.dnmsg = NULL;
        }
    }
}

int usb_up_task(void *arg)
{
    BaseType_t result;

    while (1) {
        g_usbecm.dbg_uptask_mode = 1;
        result = xQueueReceive(g_usbecm.upvq, &g_usbecm.upmsg, portMAX_DELAY);
        if (result != pdPASS) {
            printf("result:%d\r\n", result);
            return -2;  // Timeout or error in receiving the message
        }

        g_usbecm.dbg_uptask_mode = 2;
        usbd_cdc_ecm_start_write((uint8_t *)(uintptr_t)g_usbecm.upmsg->payload, g_usbecm.upmsg->len);
        result = xSemaphoreTake(g_usbecm.upsem, pdMS_TO_TICKS(10000));
        if (result != pdPASS) {
            printf("usb ecm send timeout.\r\n");
        }
        g_usbecm.dbg_uptask_mode = 3;
        pbuf_free(g_usbecm.upmsg);

        g_usbecm.dbg_uptask_loop_cnt++;
    }

    return 0;
}
#if 0
static void usb_pkt_free(void  *msg)
{
    struct pbuf *p = msg;
    pbuf_free(p);
}
#endif
int dual_stack_peer_input(void *pkt, void *arg)
{
    static int s_cnt = 0;
    trans_desc_t msg;

    //struct usbwifi_rx_env_tag *env = &g_usbwifi.rx_env;
    struct pbuf *p = (struct pbuf *)pkt;
#if 1
    //ret = usbwifi_bulk_in(env, p->payload, p->len, USBWIFI_DATA_TYPE_PKT, pkt_free, p);
    s_cnt++;
    ecm_print("--------------------------RRR %s, cnt:%d, line:%d\r\n", __func__, s_cnt, __LINE__);
    while (xQueueSend(g_usbecm.upvq, &p, portMAX_DELAY) != pdPASS);
#else
    s_cnt++;
    printf("%s, cnt:%d\r\n", __func__, s_cnt);
    pkt_free(p);
#endif
    return 0;
}

static void *eth_input_hook(bool is_sta, void *pkt, void *arg)
{
    struct pbuf *p = (struct pbuf *)pkt;

    if (npf_is_8021X(p)) {
        return pkt;
    }
#if 0
    bool input_emb = true;
    bool input_host = false;
    struct pbuf *p_dup = NULL;
    struct pbuf *ret = NULL;

#ifdef DHCP_IN_EMB
    if (npf_is_arp(p)) {
        input_emb = true;
        input_host = true;
    } else if (npf_is_dhcp4(p)) {
        input_emb = true;
        input_host = false;
    }
#else
    input_emb = false;
    input_host = true;
#endif

    // TODO Add custom filter here
    // Example of handling some TCP traffic in emb:
#if 0
    if (npf_is_tcp4_port(p, 50001) || npf_is_udp4_port(p, 50001)) {
        input_emb = true;
        input_host = false;
    }
#endif

    if (npf_is_icmp4(p)) {
        input_emb = false;
        input_host = true;
    }

    if (!input_emb && !input_host) {
        pbuf_free(p);
        return NULL;
    }

    if (input_emb && input_host) {
        p_dup = pbuf_alloc(PBUF_RAW, p->tot_len, PBUF_RAM);
        if (p_dup == NULL) {
            pbuf_free(p);
            return NULL;
        }
        pbuf_copy(p_dup, p);
    }

    if (input_host) {
#endif
        // XXX distinguish STA/AP
        int ret = dual_stack_peer_input(p, NULL);
        if (ret) {
#if 0
            if (p_dup) {
                pbuf_free(p_dup);
            }
#endif
            pbuf_free(p);
            return NULL;
        }
#if 0
    }

    if (p_dup) {
        // both emb & host
        ret = p_dup;
    } else if (input_emb) {
        ret = p;
    } else {
        ret = NULL;
    }
#endif
    return NULL;
}

static int usbecm_dump(void);
static int usbecm_queue_init(void)
{
    int i;

    // reset global
    memset(&g_usbecm, 0, sizeof(usbecm_t));

    // init queue sem
    g_usbecm.dnfq = xQueueCreate(NXBD_DNLD_ITEMS + 1, sizeof(trans_desc_t *));
    g_usbecm.upvq = xQueueCreate(NXBD_UPLD_ITEMS + 1, sizeof(trans_desc_t *));
    g_usbecm.dnsem = xSemaphoreCreateBinary();
    g_usbecm.upsem = xSemaphoreCreateBinary();
    if (!g_usbecm.dnfq || !g_usbecm.upvq || !g_usbecm.dnsem || !g_usbecm.upsem) {
        LOG_E("failed to create queue\r\n");
        return -1;
    }

    usbecm_dump();

    // init sem
    for (i = 0; i < NXBD_DNLD_ITEMS; i++) {
        trans_desc_t *msg;
#if 0
        struct pbuf_custom *p;

        p = &(dnmsg_desc[i].pbuf);
        p->custom_free_function = custom_free;
        p = pbuf_alloced_custom(PBUF_RAW_TX, TX_PBUF_FRAME_LEN,
                (PBUF_ALLOC_FLAG_DATA_CONTIGUOUS | PBUF_TYPE_ALLOC_SRC_MASK_STD_HEAP),
                &p->pbuf, p->payload_buf, TX_PBUF_PAYLOAD_LEN);
#endif
        dnmsg_desc[i].payload_buf = s_buf + TX_PBUF_PAYLOAD_LEN*i;
        msg = &(dnmsg_desc[i]);
        xQueueSend(g_usbecm.dnfq, &msg, portMAX_DELAY);
    }

    usbecm_dump();

    for (i = 0; i < NXBD_UPLD_ITEMS; i++) {
    }

    return 0;
}
static int usbecm_wifi_init(void)
{
    bl_pkt_eth_input_hook_register(eth_input_hook, NULL);//&ctx->rx_env);
    return 0;
}

static int usbecm_usb_init(void)
{
    cdc_ecm_init();
}

static int usb_ecm_acm_init(void)
{
    usbecm_queue_init();
    usbecm_usb_init();
    usbecm_wifi_init();

    xTaskCreate(usb_dn_task, (char *)"usbdn", 1024, NULL, 25, NULL);
    xTaskCreate(usb_up_task, (char *)"usbup", 1024, NULL, 25, NULL);

    return 0;
}

static int usbecm_dump(void)
{
    printf("[%ld] flash clock:%d\r\n", xTaskGetTickCount(), bflb_clk_get_peripheral_clock(BFLB_DEVICE_TYPE_FLASH, 0));

    if (g_usbecm.dnfq) {
        printf("dn fq:%d, dnmsg:%p, items:%d\r\n",
                uxQueueMessagesWaiting(g_usbecm.dnfq),
                g_usbecm.dnmsg,
                NXBD_DNLD_ITEMS);
    }
    printf("up vq:%d, upmsg:%p, items:%d\r\n",
            uxQueueMessagesWaiting(g_usbecm.upvq),
            g_usbecm.upmsg,
            NXBD_DNLD_ITEMS);
    printf("dntask mode:%d\r\n", g_usbecm.dbg_dntask_mode);
    printf("uptask mode:%d, loop cnt:%d\r\n", g_usbecm.dbg_uptask_mode, g_usbecm.dbg_uptask_loop_cnt);
    printf("send_over:%d, recv_cnt:%d\r\n", g_usbecm.send_cnt, g_usbecm.recv_cnt);

    return 0;
}

static int cmd_usbecm(int argc, char *argv[])
{
    if (argc < 2) {
        return usbecm_dump();
        printf("Usage: usbecm <dump|init>\n");
        return -1;
    }

    const char *cmd = argv[1];

    if (strcmp(cmd, "dump") == 0) {
        return usbecm_dump();
    } else if (strcmp(cmd, "init") == 0) {
        //return usb_ecm_acm_init();
    } else if (strcmp(cmd, "loop1") == 0) {
        g_usb_loop = 1;
    } else if (strcmp(cmd, "loop0") == 0) {
        g_usb_loop = 0;
    } else {
        printf("Invalid command: %s\n", cmd);
        printf("Usage: usbecm <dump|init>\n");
        return -1;
    }
}
SHELL_CMD_EXPORT_ALIAS(cmd_usbecm, usbecm, USB ECM commands.);

netbus_usb_cdc_t *netbus_usb_cdc_init(void)
{
    usb_ecm_acm_init();
    while (!g_cdc_acm_init_done) {
        vTaskDelay(10);
    }
    return &g_usbecm.usbacm;
}

