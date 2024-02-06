/**
 * @file ethernetif.c
 * @brief
 *
 * Copyright (c) 2022 Bouffalolab team
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "lwip/opt.h"
#include "lwip/timeouts.h"
#include "lwip/netif.h"
#if LWIP_DHCP
#include "lwip/dhcp.h"
#endif
#include "netif/etharp.h"
#include <string.h>

#include "bflb_emac.h"
#include "ethernet_phy.h"
#include "ethernetif.h"
#include <FreeRTOS.h>
#include "semphr.h"

#include "bflb_common.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Network interface name */
#define IFNAME0                  'e'
#define IFNAME1                  'x'

#define ETH_DMA_TRANSMIT_TIMEOUT (20U)

// #define BL702_EMAC  0
// #define EMAC_OUTPUT BL702_EMAC

#define MAX_DHCP_TRIES           4
uint32_t DHCPfineTimer = 0;
uint8_t DHCP_state = DHCP_OFF;

/*Static IP ADDRESS: IP_ADDR0.IP_ADDR1.IP_ADDR2.IP_ADDR3 */
#define IP_ADDR0      (uint8_t)192
#define IP_ADDR1      (uint8_t)168
#define IP_ADDR2      (uint8_t)123
#define IP_ADDR3      (uint8_t)100

/*NETMASK*/
#define NETMASK_ADDR0 (uint8_t)255
#define NETMASK_ADDR1 (uint8_t)255
#define NETMASK_ADDR2 (uint8_t)255
#define NETMASK_ADDR3 (uint8_t)0

/*Gateway Address*/
#define GW_ADDR0      (uint8_t)192
#define GW_ADDR1      (uint8_t)168
#define GW_ADDR2      (uint8_t)123
#define GW_ADDR3      (uint8_t)1

/* Private function prototypes -----------------------------------------------*/
struct bflb_device_s *emac0;
struct bflb_emac_phy_cfg_s phy_cfg = {
    .auto_negotiation = 1, /*!< Speed and mode auto negotiation */
    .full_duplex = 1,      /*!< Duplex mode */
    .speed = 100,            /*!< Speed mode */
#ifdef PHY_8720
    .phy_address = 0x01,  /*!< PHY address */
    .phy_id = 0x7c0f0, /*!< PHY OUI, masked */
#else
#ifdef PHY_8201F
    .phy_address = 0, /*!< PHY address */
    .phy_id = 0x120,  /*!< PHY OUI, masked */
#endif
#endif
    .phy_state = PHY_STATE_DOWN,
};

void pbuf_free_custom(struct pbuf *p);
void ethernetif_input(void *argument);
SemaphoreHandle_t emac_rx_sem = NULL;
static StackType_t emac_rx_stack[256];
static StaticTask_t emac_rx_handle;
#if LWIP_DHCP
static StackType_t emac_dhcp_stack[256];
static StaticTask_t emac_dhcp_handle;
#endif
static uint8_t emac_rx_buffer[ETH_RX_BUFFER_SIZE] __attribute__((aligned(16))) = { 0 };

LWIP_MEMPOOL_DECLARE(RX_POOL, 10, sizeof(struct pbuf_custom), "Zero-copy RX PBUF pool");

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
                       LL Driver Interface ( LwIP stack --> ETH)
*******************************************************************************/
/**
  * @brief In this function, the hardware should be initialized.
  * Called from ethernetif_init().
  *
  * @param netif the already initialized lwip network interface structure
  *        for this ethernetif
  */
extern void emac_init_txrx_buffer(struct bflb_device_s *emac);
// extern int ethernet_phy_init(struct bflb_device_s *emac, struct bflb_emac_phy_cfg_s *emac_phy_cfg);
void emac_rx_done_callback_app(void);
void dhcp_thread(void const *argument);

void emac_isr(int irq, void *arg)
{
    uint32_t int_sts_val;
    uint32_t index = 0;
    int_sts_val = bflb_emac_get_int_status(emac0);
    // printf("emac int:%08lx\r\n", int_sts_val);
    if (int_sts_val & EMAC_INT_STS_TX_DONE) {
        bflb_emac_int_clear(emac0, EMAC_INT_STS_TX_DONE);
        index = bflb_emac_bd_get_cur_active(emac0, EMAC_BD_TYPE_TX);
        bflb_emac_bd_tx_dequeue(index);

        // printf("EMAC tx done\r\n");
    }

    if (int_sts_val & EMAC_INT_STS_TX_ERROR) {
        bflb_emac_int_clear(emac0, EMAC_INT_STS_TX_ERROR);
        index = bflb_emac_bd_get_cur_active(emac0, EMAC_BD_TYPE_TX);
        bflb_emac_bd_tx_on_err(index);

        printf("EMAC tx error !!!\r\n");
    }

    if (int_sts_val & EMAC_INT_STS_RX_DONE) {
        bflb_emac_int_clear(emac0, EMAC_INT_STS_RX_DONE);
        index = bflb_emac_bd_get_cur_active(emac0, EMAC_BD_TYPE_RX);
        bflb_emac_bd_rx_enqueue(index);
        // printf("EMAC rx done %d\r\n", index);
        emac_rx_done_callback_app();
    }

    if (int_sts_val & EMAC_INT_STS_RX_ERROR) {
        bflb_emac_int_clear(emac0, EMAC_INT_STS_RX_ERROR);
        index = bflb_emac_bd_get_cur_active(emac0, EMAC_BD_TYPE_RX);
        bflb_emac_bd_rx_on_err(index);

        printf("EMAC rx error!!!\r\n");
    }

    if (int_sts_val & EMAC_INT_STS_RX_BUSY) {
        bflb_emac_int_clear(emac0, EMAC_INT_STS_RX_BUSY);
        printf("emac rx busy at %s:%d\r\n", __func__, __LINE__);
    }
}

void low_level_init(struct netif *netif)
{
    int ret = 0;
    struct bflb_emac_config_s emac_cfg = {
        .inside_clk = EMAC_CLK_USE_EXTERNAL,
        .mii_clk_div = 49,
        .min_frame_len = 64,
        .max_frame_len = ETH_MAX_PACKET_SIZE,
        .mac_addr[0] = 0x18,
        .mac_addr[1] = 0xB9,
        .mac_addr[2] = 0x05,
        .mac_addr[3] = 0x12,
        .mac_addr[4] = 0x34,
        .mac_addr[5] = 0x56,
    };

    /* set MAC hardware address length */
    netif->hwaddr_len = ETH_HWADDR_LEN;

    /* set MAC hardware address */
    netif->hwaddr[0] = emac_cfg.mac_addr[0];
    netif->hwaddr[1] = emac_cfg.mac_addr[1];
    netif->hwaddr[2] = emac_cfg.mac_addr[2];
    netif->hwaddr[3] = emac_cfg.mac_addr[3];
    netif->hwaddr[4] = emac_cfg.mac_addr[4];
    netif->hwaddr[5] = emac_cfg.mac_addr[5];

    /* maximum transfer unit */
    netif->mtu = 1500;

    /* emac init,configure ethernet peripheral (GPIOs, clocks, MAC, DMA) */
    printf("emac init\r\n");
    /* emac & BD init and interrupt attach */
    emac0 = bflb_device_get_by_name("emac0");
    bflb_emac_init(emac0, &emac_cfg);
    bflb_irq_attach(emac0->irq_num, emac_isr, emac0);
    bflb_emac_int_clear(emac0, EMAC_INT_EN_ALL);
    bflb_emac_int_enable(emac0, EMAC_INT_EN_ALL, 1);

    /* phy module init */
    ret = ethernet_phy_init(emac0, &phy_cfg);
    printf("ETH PHY init done! %d\r\n", ret);
    ethernet_phy_status_get();
    if (PHY_STATE_UP == phy_cfg.phy_state) {
        printf("PHY[%lx] @%d ready on %dMbps, %s duplex\n\r", phy_cfg.phy_id, phy_cfg.phy_address, phy_cfg.speed, phy_cfg.full_duplex ? "full" : "half");
    } else {
        printf("PHY Init fail\n\r");
        while (1) {
            bflb_mtimer_delay_ms(10);
        }
    }

    /* emac tx & rx buffer attach */
    emac_init_txrx_buffer(emac0);
    /* EMAC transmit start */
    printf("EMAC start\r\n");
    bflb_emac_start(emac0);
    bflb_irq_enable(emac0->irq_num);

    /* device capabilities */
    /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
    netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;

    /* Initialize the RX POOL */
    LWIP_MEMPOOL_INIT(RX_POOL);

    /* create a binary semaphore used for informing ethernetif of frame reception */
    //vSemaphoreCreateBinary(emac_rx_sem);
    emac_rx_sem = xSemaphoreCreateBinary();

    /* create the task that handles the ETH_MAC */
    printf("[OS] Starting emac rx task...\r\n");
    xTaskCreateStatic(ethernetif_input, (char *)"emac_rx_task", sizeof(emac_rx_stack) / 4, netif, osPriorityRealtime, emac_rx_stack, &emac_rx_handle);
#if LWIP_DHCP
    printf("[OS] Starting emac dhcp task...\r\n");
    xTaskCreateStatic(dhcp_thread, (char *)"emac_dhcp_task", sizeof(emac_dhcp_stack) / 4, netif, osPriorityRealtime, emac_dhcp_stack, &emac_dhcp_handle);
#endif

    if (ret == 0) {
        printf("[OS] %s Netif is up\r\n", netif->name);
        netif_set_up(netif);
        netif_set_link_up(netif);
    }
}

/**
  * @brief This function should do the actual transmission of the packet. The packet is
  * contained in the pbuf that is passed to the function. This pbuf
  * might be chained.
  *
  * @param netif the lwip network interface structure for this ethernetif
  * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
  * @return ERR_OK if the packet could be sent
  *         an err_t value if the packet couldn't be sent
  *
  * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
  *       strange results. You might consider waiting for space in the DMA queue
  *       to become available since the stack doesn't retry to send a packet
  *       dropped because of memory failure (except for the TCP timers).
  */

static unsigned char emac_send_buf[1514];
static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
    err_t errval = ERR_OK;
    struct pbuf *q;

    if (!emac_bd_fragment_support()) {
        uint32_t byteslefttocopy = 0;
        // uint32_t payloadoffset = 0;
        // uint32_t bufferoffset = 0;
        uint32_t framelength = 0;
        uint32_t flags = (EMAC_NORMAL_PACKET);

        for (q = p; q != NULL; q = q->next) {
            // printf("p->tot_len:%d,q->len:%d, q->next:%d,f:%d\r\n", q->tot_len, q->len, q->next, framelength);

            byteslefttocopy = q->len;
            // payloadoffset = 0;

            // check is copy data is larger than emac tx buf
            while ((byteslefttocopy + framelength) > ETH_TX_BUFFER_SIZE) {
                // copy data to tx buf
                printf("tx buf is too larger!\r\n");
                flags = EMAC_FRAGMENT_PACKET;
                // arch_memcpy_fast(&emac_send_buf[framelength + bufferoffset], q->payload + payloadoffset, (ETH_TX_BUFFER_SIZE - bufferoffset));
            }
            // arch_memcpy_fast(&emac_send_buf[framelength], q->payload, byteslefttocopy);
            memcpy(&emac_send_buf[framelength], q->payload, byteslefttocopy);
            // bufferoffset = bufferoffset + byteslefttocopy;
            framelength = framelength + byteslefttocopy;
        }

        if (0 != bflb_emac_bd_tx_enqueue(flags, framelength, emac_send_buf)) {
            printf("emac_bd_tx_enqueue error!\r\n");
            return ERR_IF;
        }

    } else {
        for (q = p; q != NULL; q = q->next) {
            // printf("p->tot_len:%d,q->len:%d, q->next:%d\r\n", q->tot_len, q->len, q->next);
            if (q->len == q->tot_len) {
                if (0 != bflb_emac_bd_tx_enqueue(EMAC_NORMAL_PACKET, q->len, q->payload)) {
                    printf("emac_bd_tx_enqueue error!\r\n");
                    return ERR_IF;
                }
            } else if (q->len < q->tot_len) {
                if (0 != bflb_emac_bd_tx_enqueue(EMAC_FRAGMENT_PACKET, q->len, q->payload)) {
                    printf("emac_bd_tx_enqueue error!\r\n");
                    return ERR_IF;
                }
            } else {
                printf("low_level_output error! Wrong packet!\r\n");
            }
        }
    }

    return errval;
}

/**
  * @brief Should allocate a pbuf and transfer the bytes of the incoming
  * packet from the interface into the pbuf.
  *
  * @param netif the lwip network interface structure for this ethernetif
  * @return a pbuf filled with the received packet (including MAC header)
  *         NULL on memory error
  */
static struct pbuf *low_level_input(struct netif *netif)
{
    uint32_t rx_len = 0;
    struct pbuf *p = NULL, *q;

    bflb_emac_bd_rx_dequeue(-1, &rx_len, emac_rx_buffer);

    if (rx_len <= 0) {
        // printf("Recv Null Data\r\n");
        return NULL;
    }

    // printf("Recv full Data\r\n");

    p = pbuf_alloc(PBUF_RAW, rx_len, PBUF_POOL);

    if (p != NULL) {
        for (q = p; q != NULL; q = q->next) {
            memcpy(q->payload, emac_rx_buffer + rx_len - q->tot_len, q->len);
            // arch_memcpy_fast(q->payload, emac_rx_buffer + rx_len - q->tot_len, q->len);
        }
    }

    return p;
}

void emac_rx_done_callback_app(void)
{
    BaseType_t xHigherPriorityTaskWoken;

    /* Is it time for vATask() to run? */
    xHigherPriorityTaskWoken = pdFALSE;
    // printf("emac_rx_done_callback_app\r\n");
    //low_level_input(NULL);
    xSemaphoreGiveFromISR(emac_rx_sem, &xHigherPriorityTaskWoken);
    /* If xHigherPriorityTaskWoken was set to true you
    we should yield.  The actual macro used here is
    port specific. */
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
/**
  * @brief This function is the ethernetif_input task, it is processed when a packet
  * is ready to be read from the interface. It uses the function low_level_input()
  * that should handle the actual reception of bytes from the network
  * interface. Then the type of the received packet is determined and
  * the appropriate input function is called.
  *
  * @param netif the lwip network interface structure for this ethernetif
  */
void ethernetif_input(void *argument)
{
    struct pbuf *p = NULL;
    struct netif *netif = (struct netif *)argument;

    for (;;) {
        if (xSemaphoreTake(emac_rx_sem, portMAX_DELAY) == pdTRUE) {
            do {
                // printf("ethernetif_input\r\n");
                p = low_level_input(netif);

                if (p != NULL) {
                    if (netif->input(p, netif) != ERR_OK) {
                        pbuf_free(p);
                    }
                }
            } while (p != NULL);
        }
    }
}

/**
  * @brief Should be called at the beginning of the program to set up the
  * network interface. It calls the function low_level_init() to do the
  * actual setup of the hardware.
  *
  * This function should be passed as a parameter to netif_add().
  *
  * @param netif the lwip network interface structure for this ethernetif
  * @return ERR_OK if the loopif is initialized
  *         ERR_MEM if private data couldn't be allocated
  *         any other err_t on error
  */
err_t ethernetif_init(struct netif *netif)
{
    LWIP_ASSERT("netif != NULL", (netif != NULL));

#if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */
    netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

    netif->name[0] = IFNAME0;
    netif->name[1] = IFNAME1;
    /* We directly use etharp_output() here to save a function call.
     * You can instead declare your own function an call etharp_output()
     * from it if you have to do some checks before sending (e.g. if link
     * is available...) */
    netif->output = etharp_output;
    netif->linkoutput = low_level_output;

    /* initialize the hardware */
    low_level_init(netif);

    return ERR_OK;
}

/**
  * @brief  Custom Rx pbuf free callback
  * @param  pbuf: pbuf to be freed
  * @retval None
  */
void pbuf_free_custom(struct pbuf *p)
{
    struct pbuf_custom *custom_pbuf = (struct pbuf_custom *)p;
    LWIP_MEMPOOL_FREE(RX_POOL, custom_pbuf);
}

static void ethernet_set_static_ip(struct netif *netif)
{
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;

    IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
    IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
    IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
    netif_set_addr(netif, ip_2_ip4(&ipaddr), ip_2_ip4(&netmask), ip_2_ip4(&gw));
}

/**
  * @brief  Notify the User about the network interface config status
  * @param  netif: the network interface
  * @retval None
  */
void ethernet_link_status_updated(struct netif *netif)
{
    if (netif_is_link_up(netif)) {
#if LWIP_DHCP
        /* Update DHCP state machine */
        DHCP_state = DHCP_START;
        printf("DHCP Start\n");
#else
        /* IP address default setting */
        ethernet_set_static_ip(netif);
        uint8_t iptxt[20];
        snprintf((char *)iptxt, sizeof(iptxt), "%s", ip4addr_ntoa(netif_ip4_addr(netif)));
        printf("Static IP address: %s\r\n", iptxt);
#endif
    } else {
#if LWIP_DHCP
        /* Update DHCP state machine */
        DHCP_state = DHCP_LINK_DOWN;
#else
        printf("The network cable is not connected \r\n");
#endif /* LWIP_DHCP */
    }
}

/**
  * @brief
  * @retval None
  */
void ethernet_link_check_state(struct netif *netif)
{
    emac_phy_status_t phy_state;

    uint32_t linkchanged = 0;
    uint32_t speed = 0, duplex = 0;

    phy_state = ethernet_phy_status_get();

    if (netif_is_link_up(netif) && (phy_state <= EMAC_PHY_STAT_LINK_DOWN)) {
        printf("Link Down!\r\n");
        // bflb_emac_stop(emac0);
        netif_set_down(netif);
        netif_set_link_down(netif);
    } else if (!netif_is_link_up(netif) && (phy_state > EMAC_PHY_STAT_LINK_UP)) {
        printf("Link Up!\r\n");
        switch (phy_state) {
            case EMAC_PHY_STAT_100MBITS_FULLDUPLEX:
                duplex = 1;
                speed = 100;
                linkchanged = 1;
                break;

            case EMAC_PHY_STAT_100MBITS_HALFDUPLEX:
                duplex = 0;
                speed = 100;
                linkchanged = 1;
                break;

            case EMAC_PHY_STAT_10MBITS_FULLDUPLEX:
                duplex = 1;
                speed = 10;
                linkchanged = 1;
                break;

            case EMAC_PHY_STAT_10MBITS_HALFDUPLEX:
                duplex = 0;
                speed = 10;
                linkchanged = 1;
                break;

            default:
                break;
        }

        if (linkchanged) {
            /* Get MAC Config MAC */
            netif_set_up(netif);
            netif_set_link_up(netif);
        }
    }
}

#if LWIP_DHCP
/**
  * @brief  DHCP Process
  * @param  argument: network interface
  * @retval None
  */
void dhcp_thread(void const *argument)
{
    struct netif *netif = (struct netif *)argument;
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;
    struct dhcp *dhcp;
    uint8_t iptxt[20];

    for (;;) {
        switch (DHCP_state) {
            case DHCP_OFF: {
                printf("DHCP OFF!\r\n");
                vTaskDelay(1000);
            }
            break;
            case DHCP_START: {
                ip_addr_set_zero_ip4(&netif->ip_addr);
                ip_addr_set_zero_ip4(&netif->netmask);
                ip_addr_set_zero_ip4(&netif->gw);
                DHCP_state = DHCP_WAIT_ADDRESS;
                printf("  State: Looking for DHCP server ...\r\n");
                dhcp_start(netif);
            } break;
            case DHCP_WAIT_ADDRESS: {
                if (dhcp_supplied_address(netif)) {
                    DHCP_state = DHCP_ADDRESS_ASSIGNED;
                    snprintf((char *)iptxt, sizeof(iptxt), "%s", ip4addr_ntoa(netif_ip4_addr(netif)));
                    printf("IP address assigned by a DHCP server: %s\r\n", iptxt);
                } else {
                    dhcp = (struct dhcp *)netif_get_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP);
                    // printf("else\r\n");
                    /* DHCP timeout */
                    if (dhcp->tries > MAX_DHCP_TRIES) {
                        DHCP_state = DHCP_TIMEOUT;

                        /* Static address used */
                        ethernet_set_static_ip(netif);
                        snprintf((char *)iptxt, sizeof(iptxt), "%s", ip4addr_ntoa(netif_ip4_addr(netif)));
                        printf("DHCP Timeout !! \r\n");
                        printf("Static IP address: %s\r\n", iptxt);
                    }
                }
            } break;
            case DHCP_ADDRESS_ASSIGNED: {
                netif->state = DHCP_ADDRESS_ASSIGNED;
            } break;
            case DHCP_TIMEOUT: {
                printf("DHCP TIMEOUT!\r\n");
                netif->state = DHCP_LINK_DOWN;
            }
            break;
            case DHCP_LINK_DOWN: {
                DHCP_state = DHCP_OFF;
                printf("The network cable is not connected \r\n");
            } break;
            default: {
                printf("dhcp:%d\r\n", DHCP_state);
            } break;
        }
        vTaskDelay(100);
    }
}
#endif /* LWIP_DHCP */
