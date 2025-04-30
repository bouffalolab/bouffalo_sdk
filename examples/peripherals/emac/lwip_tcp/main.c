#include "bflb_mtimer.h"
#include "bflb_emac.h"

#include "eth_phy.h"
#include "ephy_general.h"

#include "FreeRTOS.h"
#include "task.h"

#include "lwip/opt.h"
#include "lwip/init.h"
#include "netif/etharp.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#if LWIP_DHCP
#include "lwip/dhcp.h"
#endif
#include "lwip_netif_emac.h"

#include "tcp_client.h"
#include "tcp_server.h"

#include "board.h"

#define DBG_TAG "MAIN"
#include "log.h"

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

/**
  * @brief  Setup the network interface
  * @param  None
  * @retval None
  */
static int netif_config(void)
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
    struct netif *p_gnetif = netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &eth_emac_if_init, &tcpip_input);
    if (p_gnetif == NULL) {
        LOG_E("netif_add failed\r\n");
        return -1;
    }

    /*  Registers the default network interface */
    netif_set_default(&gnetif);

    return 0;
}

void lwip_status_update_task(void *pvParameters)
{
    LOG_I("lwip_status_update_task Run...\r\n");

    /* Infinite loop */
    while (1) {
        vTaskDelay(200);

        /* update link status */
        eth_link_state_update(&gnetif);

#if LWIP_DHCP
        /* update dhcp status */
        dhcp_sta_update(&gnetif);
#endif
    }
}

void app_start_task(void *pvParameters)
{
    LOG_I("app_start_task Run...\r\n");
    vTaskDelay(10);

    struct bflb_device_s *uart0 = bflb_device_get_by_name("uart0");
    extern void shell_init_with_task(struct bflb_device_s * shell);
    shell_init_with_task(uart0);
    LOG_I("Shell Ready...\r\n");

    LOG_I("lwip statck init\r\n");
    /* Initialize the LwIP stack */
    tcpip_init(NULL, NULL);

    LOG_I("netif config\r\n");
    /* Configure the Network interface */
    if(netif_config() < 0){
        LOG_E("netif_config failed\r\n");
        LOG_I("app_start_task Delete...\r\n");
        vTaskDelete(NULL);
    }

    /* lwip status update task */
    xTaskCreate((void *)lwip_status_update_task, (char *)"lwip_sta_update", 256, NULL, osPriorityHigh, NULL);

    /* tcp server int */
    tcp_server_init();

    LOG_I("app_start_task Delete...\r\n");
    vTaskDelete(NULL);
}

int main(void)
{
    board_init();
    /* emac gpio init */
    board_emac_gpio_init();

    printf("EMAC lwip tcp test case !\r\n");

    LOG_I("Create app_start task.\r\n");
    xTaskCreate(app_start_task, (char *)"app_start_task", 512, NULL, configMAX_PRIORITIES - 1, NULL);

    LOG_I("Start Scheduler.\r\n");
    vTaskStartScheduler();

    LOG_E("vTaskStart failed.\r\n");

    while (1) {
    }
}
