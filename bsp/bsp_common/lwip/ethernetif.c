/**
 * @file ethernetif.h
 * @brief
 *
 * Copyright (c) 2021 Bouffalolab team
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
#include "netif/etharp.h"
#include "ethernetif.h"
#include <string.h>
#include "hal_emac.h"
#include <FreeRTOS.h>
#include "semphr.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Network interface name */
#define IFNAME0                  'b'
#define IFNAME1                  'l'
#define ETH_RX_BUFFER_SIZE       (1536UL)
#define ETH_DMA_TRANSMIT_TIMEOUT (20U)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/*
@Note: This interface is implemented to operate in zero-copy mode only:
        - Rx buffers are allocated statically and passed directly to the LwIP stack,
          they will return back to ETH DMA after been processed by the stack.
        - Tx Buffers will be allocated from LwIP stack memory heap,
          then passed to ETH HAL driver.

@Notes:
  1.a. ETH DMA Rx descriptors must be contiguous, the default count is 4,
       to customize it please redefine ETH_RX_DESC_CNT in stm32xxxx_hal_conf.h
  1.b. ETH DMA Tx descriptors must be contiguous, the default count is 4,
       to customize it please redefine ETH_TX_DESC_CNT in stm32xxxx_hal_conf.h

  2.a. Rx Buffers number must be between ETH_RX_DESC_CNT and 2*ETH_RX_DESC_CNT
  2.b. Rx Buffers must have the same size: ETH_RX_BUFFER_SIZE, this value must
       passed to ETH DMA in the init field (EthHandle.Init.RxBuffLen)
  2.c  The RX Ruffers addresses and sizes must be properly defined to be aligned
       to L1-CACHE line size (32 bytes).
*/

#if LWIP_DHCP
#define MAX_DHCP_TRIES 4
uint32_t DHCPfineTimer = 0;
uint8_t DHCP_state = DHCP_OFF;
#endif

/* Private function prototypes -----------------------------------------------*/
void pbuf_free_custom(struct pbuf *p);
void ethernetif_input(void *argument);
SemaphoreHandle_t emac_rx_sem = NULL;
static StackType_t emac_rx_stack[256];
static StaticTask_t emac_rx_handle;
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
extern void emac_init_txrx_buffer(void);
extern int emac_phy_init(emac_phy_cfg_t *cfg);
void low_level_init(struct netif *netif)
{
    emac_device_t emac_cfg = {
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
    MSG("emac_init\r\n");
    emac_init(&emac_cfg);

    emac_phy_init(NULL);

    emac_init_txrx_buffer();

    /* device capabilities */
    /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
    netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;

    /* Initialize the RX POOL */
    LWIP_MEMPOOL_INIT(RX_POOL);

    /* create a binary semaphore used for informing ethernetif of frame reception */
    //vSemaphoreCreateBinary(emac_rx_sem);
    emac_rx_sem = xSemaphoreCreateBinary();

    /* create the task that handles the ETH_MAC */
    MSG("[OS] Starting emac rx task...\r\n");
    xTaskCreateStatic(ethernetif_input, (char *)"emac_rx_task", sizeof(emac_rx_stack) / 4, netif, 15, emac_rx_stack, &emac_rx_handle);
    //xTaskCreateStatic(test_task, (char*)"test_task", sizeof(test_stack)/4, NULL, 16, test_stack, &test_handle);
    ethernet_link_check_state(netif);
    netif_set_up(netif);
    netif_set_link_up(netif);
}

void emac_tx_error_callback_app()
{
    MSG("EMAC tx error callback\r\n");
}

void emac_rx_error_callback_app()
{
    MSG("EMAC rx error callback\r\n");
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
static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
    err_t errval = ERR_OK;
    struct pbuf *q;

    for (q = p; q != NULL; q = q->next) {
        if (0 != emac_bd_tx_enqueue(-1, q->len, q->payload)) {
            MSG("emac_bd_tx_enqueue error!\r\n");
            return ERR_IF;
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

    emac_bd_rx_dequeue(-1, &rx_len, emac_rx_buffer);

    if (rx_len <= 0) {
        //MSG("Recv Null Data\r\n");
        return NULL;
    }

    //MSG("Recv full Data\r\n");

    p = pbuf_alloc(PBUF_RAW, rx_len, PBUF_POOL);

    if (p != NULL) {
        for (q = p; q != NULL; q = q->next) {
            memcpy(q->payload, emac_rx_buffer + rx_len - q->tot_len, q->len);
        }
    }

    return p;
}

void emac_rx_done_callback_app(void)
{
    BaseType_t xHigherPriorityTaskWoken;

    /* Is it time for vATask() to run? */
    xHigherPriorityTaskWoken = pdFALSE;
    //MSG("emac_rx_done_callback_app\r\n");
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
                //MSG("ethernetif_input\r\n");
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
        MSG("DHCP Start\n");
#else
        uint8_t iptxt[20];
        sprintf((char *)iptxt, "%s", ip4addr_ntoa(netif_ip4_addr(netif)));
        MSG("Static IP address: %s\n", iptxt);
#endif
    } else {
#if LWIP_DHCP
        /* Update DHCP state machine */
        DHCP_state = DHCP_LINK_DOWN;
#else
        MSG("The network cable is not connected \n");
#endif /* LWIP_DHCP */
    }
}

/**
  * @brief
  * @retval None
  */
void ethernet_link_check_state(struct netif *netif)
{
#if 0
    ETH_MACConfigTypeDef MACConf;
    uint32_t PHYLinkState;
    uint32_t linkchanged = 0, speed = 0, duplex = 0;

    PHYLinkState = LAN8742_GetLinkState(&LAN8742);

    if(netif_is_link_up(netif) && (PHYLinkState <= LAN8742_STATUS_LINK_DOWN))
    {
        HAL_ETH_Stop(&EthHandle);
        netif_set_down(netif);
        netif_set_link_down(netif);
    }
    else if(!netif_is_link_up(netif) && (PHYLinkState > LAN8742_STATUS_LINK_DOWN))
    {
        switch(PHYLinkState)
        {
            case LAN8742_STATUS_100MBITS_FULLDUPLEX:
                duplex = ETH_FULLDUPLEX_MODE;
                speed = ETH_SPEED_100M;
                linkchanged = 1;
                break;

            case LAN8742_STATUS_100MBITS_HALFDUPLEX:
                duplex = ETH_HALFDUPLEX_MODE;
                speed = ETH_SPEED_100M;
                linkchanged = 1;
                break;

            case LAN8742_STATUS_10MBITS_FULLDUPLEX:
                duplex = ETH_FULLDUPLEX_MODE;
                speed = ETH_SPEED_10M;
                linkchanged = 1;
                break;

            case LAN8742_STATUS_10MBITS_HALFDUPLEX:
                duplex = ETH_HALFDUPLEX_MODE;
                speed = ETH_SPEED_10M;
                linkchanged = 1;
                break;

            default:
                break;
        }

        if(linkchanged)
        {
            /* Get MAC Config MAC */
            HAL_ETH_GetMACConfig(&EthHandle, &MACConf);
            MACConf.DuplexMode = duplex;
            MACConf.Speed = speed;
            HAL_ETH_SetMACConfig(&EthHandle, &MACConf);
            HAL_ETH_Start(&EthHandle);
            netif_set_up(netif);
            netif_set_link_up(netif);
        }
    }

#endif
}
