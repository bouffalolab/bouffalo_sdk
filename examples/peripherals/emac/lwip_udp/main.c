#include "bflb_mtimer.h"
#include "board.h"
#include "log.h"

#include "bflb_emac.h"
#include "ethernet_phy.h"
#include "lwip/opt.h"
#include "lwip/init.h"
#include "netif/etharp.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#if LWIP_DHCP
#include "lwip/dhcp.h"
#endif
#include "ethernetif.h"
#include "FreeRTOS.h"
#include "task.h"

#include "udp_echo.h"

// Local IP address define
/*Static IP ADDRESS: IP_ADDR0.IP_ADDR1.IP_ADDR2.IP_ADDR3 */
#define IP_ADDR0 (uint8_t)192
#define IP_ADDR1 (uint8_t)168
#define IP_ADDR2 (uint8_t)123
#define IP_ADDR3 (uint8_t)100

// Local NET Mask address define
/*NETMASK*/
#define NETMASK_ADDR0 (uint8_t)255
#define NETMASK_ADDR1 (uint8_t)255
#define NETMASK_ADDR2 (uint8_t)255
#define NETMASK_ADDR3 (uint8_t)0

// Local Gateway address define
/*Gateway Address*/
#define GW_ADDR0 (uint8_t)192
#define GW_ADDR1 (uint8_t)168
#define GW_ADDR2 (uint8_t)123
#define GW_ADDR3 (uint8_t)1

/* global network interface struct define */
struct netif gnetif;

/* For emac tx and rx buffer,we put here to make controlling it's size easy */
#define ETH_RXBUFNB 5
#define ETH_TXBUFNB 5
ATTR_NOCACHE_NOINIT_RAM_SECTION __attribute__((aligned(4))) uint8_t ethRxBuff[ETH_RXBUFNB][ETH_RX_BUFFER_SIZE]; /* Ethernet Receive Buffers */
ATTR_NOCACHE_NOINIT_RAM_SECTION __attribute__((aligned(4))) uint8_t ethTxBuff[ETH_TXBUFNB][ETH_TX_BUFFER_SIZE]; /* Ethernet Transmit Buffers */

/**
  * @brief  Setup the network interface
  * @param  None
  * @retval None
  */
static void netif_config(void)
{
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;

/* if use DHCP */
#if LWIP_DHCP
    ip_addr_set_zero_ip4(&ipaddr);
    ip_addr_set_zero_ip4(&netmask);
    ip_addr_set_zero_ip4(&gw);
#else

    /* IP address default setting */
    IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
    IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
    IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);

#endif

    /* add the network interface */
    netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);

    /*  Registers the default network interface */
    netif_set_default(&gnetif);
    ethernet_link_status_updated(&gnetif);

#if LWIP_NETIF_LINK_CALLBACK
    netif_set_link_callback(&gnetif, ethernet_link_status_updated);
#endif
}

static void ethernet_lwip_init()
{
    printf("lwip statck init\r\n");

    /* Initialize the LwIP stack */
    tcpip_init(NULL, NULL);

    printf("netif config\r\n");
    /* Configure the Network interface */
    netif_config();
}

void emac_init_txrx_buffer(struct bflb_device_s *emac)
{
    bflb_emac_bd_init(emac, (uint8_t *)ethTxBuff, ETH_TXBUFNB, (uint8_t *)ethRxBuff, ETH_RXBUFNB);
}

int main(void)
{
    board_init();
    /* emac gpio init */
    board_emac_gpio_init();

    printf("EMAC lwip test case !\r\n");

    ethernet_lwip_init();
    // bflb_emac_stop_tx();

    printf("[OS] Starting udp echo task...\r\n");
    udp_echo_init();
    // udp_echo_raw_init();

    vTaskStartScheduler();

    printf("case success!\r\n");
    while (1) {
    }
}
