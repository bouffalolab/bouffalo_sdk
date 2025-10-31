
#include <lwip/tcpip.h>
#include <lwip/dns.h>
#include <openthread_port.h>
#include <openthread_br.h>
#include <shell.h>

static void cmd_ifconfig(int argc, char **argv)
{
    struct netif  * netif = otbr_getInfraNetif();
    ip6_addr_t    * ip6addr;

    printf("Infra net interface: %s\r\n", netif->flags & NETIF_FLAG_UP ? "UP": "DOWN");
    printf("\tMAC address: %02X:%02X:%02X:%02X:%02X:%02X\r\n", netif->hwaddr[0],
        netif->hwaddr[1], netif->hwaddr[2], netif->hwaddr[3], netif->hwaddr[4], netif->hwaddr[5]);

    if (netif->flags & NETIF_FLAG_UP) {
        if(!ip4_addr_isany(netif_ip4_addr(netif))){
            printf("\tIPv4 address: %s\r\n", ip4addr_ntoa(netif_ip4_addr(netif)));
            printf("\tIPv4 mask: %s\r\n", ip4addr_ntoa(netif_ip4_netmask(netif)));
            printf("\tGateway address: %s\r\n", ip4addr_ntoa(netif_ip4_gw(netif)));
            for (int i = 0; i < DNS_MAX_SERVERS; i ++) {
                printf("\tDNS server %d: %s\r\n", i, ipaddr_ntoa(dns_getserver(i)));
            }
        }

        for (uint32_t i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i ++ ) {
            if (!ip6_addr_isany(netif_ip6_addr(netif, i)) &&
                ip6_addr_ispreferred(netif_ip6_addr_state(netif, i))) {

                ip6addr = (ip6_addr_t *)netif_ip6_addr(netif, i);
                if (ip6_addr_isany(ip6addr)) {
                    continue;
                }

                if (ip6_addr_islinklocal(ip6addr)) {
                    printf("\tIPv6 linklocal address: %s\r\n", ip6addr_ntoa(ip6addr));
                }
                else{
                    printf("\tIPv6 address %d: %s\r\n", i, ip6addr_ntoa(ip6addr));
                }
            }
        }
    }

    netif = otbr_getThreadNetif();
    printf("Thread net interface: %s\r\n", netif->flags & NETIF_FLAG_UP ? "UP": "DOWN");
    printf("\tMAC address: %02X:%02X:%02X:%02X:%02X:%02X\r\n", netif->hwaddr[0],
        netif->hwaddr[1], netif->hwaddr[2], netif->hwaddr[3], netif->hwaddr[4], netif->hwaddr[5]);

    if (netif->flags & NETIF_FLAG_UP) {
        if(!ip4_addr_isany(netif_ip4_addr(netif))){
            printf("\tIPv4 address: %s\r\n", ip4addr_ntoa(netif_ip4_addr(netif)));
            printf("\tIPv4 mask: %s\r\n", ip4addr_ntoa(netif_ip4_netmask(netif)));
            printf("\tGateway address: %s\r\n", ip4addr_ntoa(netif_ip4_gw(netif)));
        }

        for (uint32_t i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i ++ ) {
            if (!ip6_addr_isany(netif_ip6_addr(netif, i)) &&
                ip6_addr_ispreferred(netif_ip6_addr_state(netif, i))) {

                ip6addr = (ip6_addr_t *)netif_ip6_addr(netif, i);
                if (ip6_addr_isany(ip6addr)) {
                    continue;
                }

                if (ip6_addr_islinklocal(ip6addr)) {
                    printf("\tIPv6 linklocal address: %s\r\n", ip6addr_ntoa(ip6addr));
                }
                else{
                    printf("\tIPv6 address %d: %s\r\n", i, ip6addr_ntoa(ip6addr));
                }
            }
        }
    }
}

static void cmd_stats_display(int argc, char **argv) 
{
    stats_display();
}

static int cmd_abort(int argc, char **argv)
{
    abort();
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_ifconfig, ifconfig, show information on infra network interface.);
SHELL_CMD_EXPORT_ALIAS(cmd_stats_display, stats, stats display lwip.);
SHELL_CMD_EXPORT_ALIAS(cmd_abort, abort, user calls abort);
