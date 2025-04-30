#include <string.h>
#include <lwip/tcpip.h>
#include <lwip/mld6.h>

#include <openthread/link.h>
#include <openthread_port.h>
#include <otbr_lwip.h>

static struct netif                otbr_netif;

struct netif * otbr_getThreadNetif(void) 
{
    return &otbr_netif;
}

bool otbr_lwip_netif_is_up(struct netif *nif) 
{
    return nif != NULL && netif_is_up(nif);
}

int otbr_lwip_get_ipv6_address_num(void) 
{
    return LWIP_IPV6_NUM_ADDRESSES;
}

int otbr_lwip_get_netif_num(struct netif *nif) 
{
    return nif->num;
}

u8_t otbr_lwip_get_netif_index(struct netif *nif) 
{
    return netif_get_index(nif);
}

ip4_addr_t * otbr_lwip_get_ipv4_address(struct netif *nif) 
{
    if (nif != NULL && netif_is_up(nif)) {
        return ip_2_ip4(&(nif->ip_addr));
    }

    return NULL;
}

ip4_addr_t * otbr_lwip_get_ipv4_mask(struct netif *nif) 
{
    if (nif != NULL && netif_is_up(nif)) {
        return ip_2_ip4(&(nif->netmask));
    }

    return NULL;
}

ip4_addr_t * otbr_lwip_get_ipv4_gateway(struct netif *nif) 
{
    if (nif != NULL && netif_is_up(nif)) {
        return ip_2_ip4(&(nif->gw));
    }

    return NULL;
}

ip_addr_t * otbr_lwip_get_ipv6_addr(struct netif *nif) 
{
    return nif->ip6_addr;
}
u8_t * otbr_lwip_get_ip6_addr_state(struct netif *nif) 
{
    return nif->ip6_addr_state;
}
ip_addr_t * otbr_lwip_netif_ip_addr6(struct netif *nif, int i) 
{
    return (&nif->ip6_addr[i]);
}
ip6_addr_t * otbr_lwip_netif_ip6_addr(struct netif *nif, int i)
{
    // netif_ip6_addr
    return ip_2_ip6(&nif->ip6_addr[i]);
}

struct mld_group * otbr_lwip_netif_mld6_data(struct netif *nif) 
{
    return netif_mld6_data(nif);
}

void otbr_lwip_netif_mld6_data_set(struct netif *nif, struct mld_group * group) 
{
    netif_set_client_data(nif, LWIP_NETIF_CLIENT_DATA_INDEX_MLD6, group);
}

netif_mld_mac_filter_fn otbr_lwip_mld_mac_filter(struct netif *nif) 
{
    return nif->mld_mac_filter;
}

bool otbr_lwip_netif_checksum_enabled(struct netif *nif, uint16_t flag) 
{
    IF__NETIF_CHECKSUM_ENABLED(netif, flag) {
        return true;
    }

    return false;
}


uint8_t otbr_lwip_get_dns_max_servers(void) 
{
    return DNS_MAX_SERVERS;
}

uint32_t otbr_lwip_get_max_rio_routes(void) 
{
    return OTBR_MAX_RIO_ROUTE;
}

uint32_t otbr_lwip_get_rio_timeout(void) 
{
    return OTBR_RIO_TIMEOUT;
}

char * otbr_hostname(void) 
{
#ifndef OTBR_HOSTNAME
    #define OTBR_HOSTNAME "bflb_otbr"
#endif
    uint8_t eui[8];
    static char hostname[sizeof(OTBR_HOSTNAME) + 8];

    otPlatRadioGetIeeeEui64(NULL, eui);
    snprintf(hostname, sizeof(hostname), "%s-%02x%02x", OTBR_HOSTNAME, eui[4], eui[5]);

    return hostname;
}

char * otbr_vendor_name(void) 
{
#ifndef OTBR_VENDOR_NAME
    #define OTBR_VENDOR_NAME "BouffaloLab"
#endif

    return OTBR_VENDOR_NAME;
}

char * otbr_product_name(void) 
{
#ifndef OTBR_PRODUCT_NAME
    #define OTBR_PRODUCT_NAME "OTBR"
#endif

    return OTBR_PRODUCT_NAME;
}

void otbr_udp_set_multicast_ttl(struct udp_pcb * pcb, uint32_t ttl) 
{
    udp_set_multicast_ttl(pcb, ttl);
}

void otbr_udp_set_flags(struct udp_pcb * pcb, uint32_t flag) 
{
    udp_set_flags(pcb, UDP_FLAGS_MULTICAST_LOOP);
}

void otbr_udp_clear_flags(struct udp_pcb * pcb, uint32_t flag) 
{
    udp_clear_flags(pcb, UDP_FLAGS_MULTICAST_LOOP);
}
