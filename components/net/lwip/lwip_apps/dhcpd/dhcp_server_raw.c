
/**
 ****************************************************************************************
 *
 * @file dhcp_server_raw.c
 * Copyright (C) Bouffalo Lab 2016-2018
 *
 ****************************************************************************************
 */


#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <FreeRTOS.h>

#include <lwip/opt.h>
#include <lwip/sockets.h>
#include <lwip/inet_chksum.h>
#include <netif/etharp.h>
#include <lwip/ip.h>
#include <lwip/init.h>
#include <lwip/prot/dhcp.h>
#include <dhcp_server.h>
#include <lwip/netifapi.h>
#include <lwip/tcpip.h>


#include <lwip/prot/dhcp.h>

/* DHCP server option */
#define DHCP_CLIENT_PORT  68
#define DHCP_SERVER_PORT  67 

/* allocated client ip range */
#ifndef DHCPD_CLIENT_IP_MIN
    #define DHCPD_CLIENT_IP_MIN     2
#endif
#ifndef DHCPD_CLIENT_IP_MAX
    #define DHCPD_CLIENT_IP_MAX     254
#endif

/* the DHCP server address */
#ifndef DHCPD_SERVER_IP
    #define DHCPD_SERVER_IP "192.168.169.1"
#endif

#define DHCP_DEBUG_PRINTF

#ifdef  DHCP_DEBUG_PRINTF
#undef printf
    #define DEBUG_PRINTF(...)        printf("[DHCP] "__VA_ARGS__)
#else
    #define DEBUG_PRINTF(...)
#endif /* DHCP_DEBUG_PRINTF */

/* we need some routines in the DHCP of lwIP */
#undef  LWIP_DHCP
#define LWIP_DHCP   1
#include <lwip/dhcp.h>

/** Mac address length  */
#define DHCP_MAX_HLEN               6
/** dhcp default live time */
#define DHCP_DEFAULT_LIVE_TIME      0x80510100

/** Minimum length for request before packet is parsed */
#define DHCP_MIN_REQUEST_LEN        44

#define LWIP_NETIF_LOCK(...)
#define LWIP_NETIF_UNLOCK(...)

/**
* The dhcp client node struct.
*/
struct dhcp_client_node
{
    struct dhcp_client_node *next;
    u8_t chaddr[DHCP_MAX_HLEN];
    ip4_addr_t ipaddr;
    u32_t lease_end;
};

/**
* The dhcp server struct.
*/
struct dhcp_server
{
    struct dhcp_server *next;
    struct netif *netif;
    struct udp_pcb *pcb;
    struct dhcp_client_node *node_list;
    ip4_addr_t start;
    ip4_addr_t end;
    ip4_addr_t current;
};

struct dhcp_server_arg {
    struct netif *netif;
    ip4_addr_t start;
    ip4_addr_t end;
};

static u8_t *dhcp_server_option_find(u8_t *buf, u16_t len, u8_t option);

/**
* The dhcp server struct list.
*/
static struct dhcp_server *lw_dhcp_server;

/**
* Find a dhcp client node by mac address
*
* @param dhcpserver The dhcp server
* @param chaddr Mac address
* @param hlen   Mac address length
* @return dhcp client node
*/
static struct dhcp_client_node *
dhcp_client_find_by_mac(struct dhcp_server *dhcpserver, const u8_t *chaddr, u8_t hlen)
{
    struct dhcp_client_node *node;

    for (node = dhcpserver->node_list; node != NULL; node = node->next)
    {
        if (memcmp(node->chaddr, chaddr, hlen) == 0)
        {
            return node;
        }
    }

    return NULL;
}

/**
* Find a dhcp client node by ip address
*
* @param dhcpserver The dhcp server
* @param chaddr Mac address
* @param hlen   Mac address length
* @return dhcp client node
*/
static struct dhcp_client_node *
dhcp_client_find_by_ip(struct dhcp_server *dhcpserver, const uint8_t *ip)
{
    struct dhcp_client_node *node;
    ip4_addr_t ipaddr;//FIXME we only support IPv4, so we use ip4_addr_t here
    uint32_t ipval;

    // Copy ipaddr to avoid aligment issue
    memcpy(&ipval, ip, sizeof(ipval));
    ip4_addr_set_u32(&ipaddr, ipval);
    for (node = dhcpserver->node_list; node != NULL; node = node->next)
    {
        if (ip4_addr_cmp(&node->ipaddr, &ipaddr))
        {
            return node;
        }
    }

    return NULL;
}

/**
* Find a dhcp client node by ip address
*
* @param dhcpserver The dhcp server
* @param chaddr Mac address
* @param hlen   Mac address length
* @return dhcp client node
*/
static struct dhcp_client_node *
dhcp_client_find(struct dhcp_server *dhcpserver, struct dhcp_msg *msg,
                 u8_t *opt_buf, u16_t len)
{
    u8_t *opt;
    //u32_t ipaddr;
    struct dhcp_client_node *node;

    node = dhcp_client_find_by_mac(dhcpserver, msg->chaddr, msg->hlen);
    if (node != NULL)
    {
        return node;
    }

    opt = dhcp_server_option_find(opt_buf, len, DHCP_OPTION_REQUESTED_IP);
    if (opt != NULL)
    {
        node = dhcp_client_find_by_ip(dhcpserver, &opt[2]);
        if (node != NULL)
        {
            if (0 == memcmp(node->chaddr, msg->chaddr, msg->hlen)) {
                return node;
            } else {
                puts("IP Found, but MAC address is NOT the same\r\n");
                // return node;  //FIXME use hostname instead of mac address
            }
        }
    }

    return NULL;
}

/**
* Find a dhcp client node by ip address
*
* @param dhcpserver The dhcp server
* @param chaddr Mac address
* @param hlen   Mac address length
* @return dhcp client node
*/
static struct dhcp_client_node *
dhcp_client_alloc(struct dhcp_server *dhcpserver, struct dhcp_msg *msg,
                  u8_t *opt_buf, u16_t len)
{
    u8_t *opt;
    u32_t ipaddr;
    struct dhcp_client_node *node;

    node = dhcp_client_find_by_mac(dhcpserver, msg->chaddr, msg->hlen);
    if (node != NULL)
    {
        return node;
    }

    opt = dhcp_server_option_find(opt_buf, len, DHCP_OPTION_REQUESTED_IP);
    if (opt != NULL)
    {
        node = dhcp_client_find_by_ip(dhcpserver, &opt[2]);
        if (node != NULL)
        {
            return node;
        }
    }

dhcp_alloc_again:
    node = dhcp_client_find_by_ip(dhcpserver, (uint8_t*)&dhcpserver->current);
    if (node != NULL)
    {
        ipaddr = (ntohl(dhcpserver->current.addr) + 1);
        if (ipaddr > ntohl(dhcpserver->end.addr))
        {
            ipaddr = ntohl(dhcpserver->start.addr);
        }
        dhcpserver->current.addr = htonl(ipaddr);
        goto dhcp_alloc_again;
    }
    node = (struct dhcp_client_node *)mem_malloc(sizeof(struct dhcp_client_node));
    if (node == NULL)
    {
        return NULL;
    }
    SMEMCPY(node->chaddr, msg->chaddr, msg->hlen);
    node->ipaddr = dhcpserver->current;

    node->next = dhcpserver->node_list;
    dhcpserver->node_list = node;

    return node;
}

/**
* find option from buffer.
*
* @param buf The buffer to find option
* @param len The buffer length
* @param option Which option to find
* @return dhcp option buffer
*/
static u8_t *
dhcp_server_option_find(u8_t *buf, u16_t len, u8_t option)
{
    u8_t *end = buf + len;
    while ((buf < end) && (*buf != DHCP_OPTION_END))
    {
        if (*buf == option)
        {
            return buf;
        }
        buf += (buf[1] + 2);
    }
    return NULL;
}

/**
* If an incoming DHCP message is in response to us, then trigger the state machine
*/
static void
dhcp_server_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *recv_addr, u16_t port)
{
    struct dhcp_server *dhcp_server = (struct dhcp_server *)arg;
    struct dhcp_msg *msg;
    struct pbuf *q;
    u8_t *opt_buf;
    u8_t *opt;
    struct dhcp_client_node *node;
    u8_t msg_type;
    u16_t length;
    ip_addr_t addr = *recv_addr;
    u32_t tmp;

    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("[%s:%d] %c%c recv %d\n", __FUNCTION__, __LINE__, dhcp_server->netif->name[0], dhcp_server->netif->name[1], p->tot_len));
    /* prevent warnings about unused arguments */
    LWIP_UNUSED_ARG(pcb);
    LWIP_UNUSED_ARG(addr);
    LWIP_UNUSED_ARG(port);

    if (p->len < DHCP_MIN_REQUEST_LEN)
    {
        LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_WARNING, ("DHCP request message or pbuf too short\n"));
        pbuf_free(p);
        return;
    }

    q = pbuf_alloc(PBUF_TRANSPORT, 1500, PBUF_RAM);
    if (q == NULL)
    {
        LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_WARNING, ("pbuf_alloc dhcp_msg failed!\n"));
        pbuf_free(p);
        return;
    }
    if (q->tot_len < p->tot_len)
    {
        LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_WARNING, ("pbuf_alloc dhcp_msg too small %d:%d\n", q->tot_len, p->tot_len));
        pbuf_free(p);
        return;
    }

    pbuf_copy(q, p);
    pbuf_free(p);

    msg = (struct dhcp_msg *)q->payload;
    if (msg->op != DHCP_BOOTREQUEST)
    {
        LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_WARNING, ("not a DHCP request message, but type %"U16_F"\n", (u16_t)msg->op));
        goto free_pbuf_and_return;
    }

    if (msg->cookie != PP_HTONL(DHCP_MAGIC_COOKIE))
    {
        LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE | LWIP_DBG_LEVEL_WARNING, ("bad DHCP_MAGIC_COOKIE!\n"));
        goto free_pbuf_and_return;
    }

    if (msg->hlen > DHCP_MAX_HLEN)
    {
        goto free_pbuf_and_return;
    }

    opt_buf = (u8_t *)msg + DHCP_OPTIONS_OFS;
    length = q->tot_len - DHCP_OPTIONS_OFS;
    opt = dhcp_server_option_find(opt_buf, length, DHCP_OPTION_MESSAGE_TYPE);
    if (opt)
    {
        msg_type = *(opt + 2);
        if (msg_type == DHCP_DISCOVER)
        {
            node = dhcp_client_alloc(dhcp_server, msg, opt_buf, length);
            if (node == NULL)
            {
                goto free_pbuf_and_return;
            }
            node->lease_end = DHCP_DEFAULT_LIVE_TIME;
            /* create dhcp offer and send */
            msg->op = DHCP_BOOTREPLY;
            msg->hops = 0;
            msg->secs = 0;
            //FIXME we use ip_addr_t as ip4_addr_t here
            SMEMCPY(&msg->siaddr, &(dhcp_server->netif->ip_addr), 4);
            msg->sname[0] = '\0';
            msg->file[0] = '\0';
            msg->cookie = PP_HTONL(DHCP_MAGIC_COOKIE);
            SMEMCPY(&msg->yiaddr, &node->ipaddr, 4);

            opt_buf = (u8_t *)msg + DHCP_OPTIONS_OFS;
            /* add msg type */
            *opt_buf++ = DHCP_OPTION_MESSAGE_TYPE;
            *opt_buf++ = 1;
            *opt_buf++ = DHCP_OFFER;

            /* add server id */
            *opt_buf++ = DHCP_OPTION_SERVER_ID;
            *opt_buf++ = 4;
            //FIXME we use ip_addr_t as ip4_addr_t here
            SMEMCPY(opt_buf, &(dhcp_server->netif->ip_addr), 4);
            opt_buf += 4;

            /* add_lease_time */
            *opt_buf++ = DHCP_OPTION_LEASE_TIME;
            *opt_buf++ = 4;
            tmp = PP_HTONL(DHCP_DEFAULT_LIVE_TIME);
            SMEMCPY(opt_buf, &tmp, 4);
            opt_buf += 4;

            /* add config */
            *opt_buf++ = DHCP_OPTION_SUBNET_MASK;
            *opt_buf++ = 4;
            //FIXME we use ip_addr_t as ip4_addr_t here
            SMEMCPY(opt_buf, &ip_2_ip4(&dhcp_server->netif->netmask)->addr, 4);
            opt_buf += 4;

            *opt_buf++ = DHCP_OPTION_DNS_SERVER;
            *opt_buf++ = 4;
#ifdef DHCP_DNS_SERVER_IP
            {
                ip_addr_t dns_addr;
                ipaddr_aton(DHCP_DNS_SERVER_IP, &dns_addr);
                SMEMCPY(opt_buf, &ip_2_ip4(&dns_addr)->addr, 4);
            }
#else
            /* default use gatewary dns server */
            //FIXME we use ip_addr_t as ip4_addr_t here
            SMEMCPY(opt_buf, &(dhcp_server->netif->ip_addr), 4);
#endif /* DHCP_DNS_SERVER_IP */
            opt_buf += 4;

            *opt_buf++ = DHCP_OPTION_ROUTER;
            *opt_buf++ = 4;
            //FIXME we use ip_addr_t as ip4_addr_t here
            SMEMCPY(opt_buf, &ip_2_ip4(&dhcp_server->netif->ip_addr)->addr, 4);
            opt_buf += 4;

            /* add option end */
            *opt_buf++ = DHCP_OPTION_END;

            length = (u32_t)opt_buf - (u32_t)msg;
            if (length < q->tot_len)
            {
                pbuf_realloc(q, length);
            }

            ip_2_ip4(&addr)->addr = INADDR_BROADCAST;
            udp_sendto_if(pcb, q, &addr, port, dhcp_server->netif);
        }
        else
        {
            if (1)
            {
                if (msg_type == DHCP_REQUEST)
                {
                    node = dhcp_client_find(dhcp_server, msg, opt_buf, length);
                    if (node != NULL)
                    {
                        /* Send ack */
                        node->lease_end = DHCP_DEFAULT_LIVE_TIME;
                        /* create dhcp offer and send */
                        msg->op = DHCP_BOOTREPLY;
                        msg->hops = 0;
                        msg->secs = 0;
                        //FIXME we use ip_addr_t as ip4_addr_t here
                        SMEMCPY(&msg->siaddr, &(dhcp_server->netif->ip_addr), 4);
                        msg->sname[0] = '\0';
                        msg->file[0] = '\0';
                        msg->cookie = PP_HTONL(DHCP_MAGIC_COOKIE);
                        SMEMCPY(&msg->yiaddr, &node->ipaddr, 4);
                        opt_buf = (u8_t *)msg + DHCP_OPTIONS_OFS;

                        /* add msg type */
                        *opt_buf++ = DHCP_OPTION_MESSAGE_TYPE;
                        *opt_buf++ = 1;
                        *opt_buf++ = DHCP_ACK;

                        /* add server id */
                        *opt_buf++ = DHCP_OPTION_SERVER_ID;
                        *opt_buf++ = 4;
                        //FIXME we use ip_addr_t as ip4_addr_t here
                        SMEMCPY(opt_buf, &(dhcp_server->netif->ip_addr), 4);
                        opt_buf += 4;

                        /* add_lease_time */
                        *opt_buf++ = DHCP_OPTION_LEASE_TIME;
                        *opt_buf++ = 4;
                        tmp = PP_HTONL(DHCP_DEFAULT_LIVE_TIME);
                        SMEMCPY(opt_buf, &tmp, 4);
                        opt_buf += 4;

                        /* add config */
                        *opt_buf++ = DHCP_OPTION_SUBNET_MASK;
                        *opt_buf++ = 4;
                        //FIXME we use ip_addr_t as ip4_addr_t here
                        SMEMCPY(opt_buf, &ip_2_ip4(&dhcp_server->netif->netmask)->addr, 4);
                        opt_buf += 4;

                        *opt_buf++ = DHCP_OPTION_DNS_SERVER;
                        *opt_buf++ = 4;
#ifdef DHCP_DNS_SERVER_IP
                        {
                            ip_addr_t dns_addr;
                            ipaddr_aton(DHCP_DNS_SERVER_IP, &dns_addr);
                            SMEMCPY(opt_buf, &ip_2_ip4(&dns_addr)->addr, 4);
                        }
#else
                        /* default use gatewary dns server */
                        //FIXME we use ip_addr_t as ip4_addr_t here
                        SMEMCPY(opt_buf, &(dhcp_server->netif->ip_addr), 4);
#endif /* DHCP_DNS_SERVER_IP */
                        opt_buf += 4;

                        *opt_buf++ = DHCP_OPTION_ROUTER;
                        *opt_buf++ = 4;
                        //FIXME we use ip_addr_t as ip4_addr_t here
                        SMEMCPY(opt_buf, &ip_2_ip4(&dhcp_server->netif->ip_addr)->addr, 4);
                        opt_buf += 4;

                        /* add option end */
                        *opt_buf++ = DHCP_OPTION_END;

                        length = (u32_t)opt_buf - (u32_t)msg;
                        if (length < q->tot_len)
                        {
                            pbuf_realloc(q, length);
                        }

                        ip_2_ip4(&addr)->addr = INADDR_BROADCAST;
                        udp_sendto_if(pcb, q, &addr, port, dhcp_server->netif);
                    }
                    else
                    {
                        /* Send no ack */
                        /* create dhcp offer and send */
                        msg->op = DHCP_BOOTREPLY;
                        msg->hops = 0;
                        msg->secs = 0;
                        //FIXME we use ip_addr_t as ip4_addr_t here
                        SMEMCPY(&msg->siaddr, &(dhcp_server->netif->ip_addr), 4);
                        msg->sname[0] = '\0';
                        msg->file[0] = '\0';
                        msg->cookie = PP_HTONL(DHCP_MAGIC_COOKIE);
                        memset(&msg->yiaddr, 0, 4);
                        opt_buf = (u8_t *)msg + DHCP_OPTIONS_OFS;

                        /* add msg type */
                        *opt_buf++ = DHCP_OPTION_MESSAGE_TYPE;
                        *opt_buf++ = 1;
                        *opt_buf++ = DHCP_NAK;

                        /* add server id */
                        *opt_buf++ = DHCP_OPTION_SERVER_ID;
                        *opt_buf++ = 4;
                        //FIXME we use ip_addr_t as ip4_addr_t here
                        SMEMCPY(opt_buf, &(dhcp_server->netif->ip_addr), 4);
                        opt_buf += 4;

                        /* add option end */
                        *opt_buf++ = DHCP_OPTION_END;
                        length = (u32_t)opt_buf - (u32_t)msg;
                        if (length < q->tot_len)
                        {
                            pbuf_realloc(q, length);
                        }

                        ip_2_ip4(&addr)->addr = INADDR_BROADCAST;
                        udp_sendto_if(pcb, q, &addr, port, dhcp_server->netif);
                    }
                }
                else if (msg_type == DHCP_RELEASE)
                {
                    struct dhcp_client_node *node_prev = NULL;

                    for (node = dhcp_server->node_list; node != NULL; node = node->next)
                    {
                        if (memcmp(node->chaddr, msg->chaddr, msg->hlen) == 0)
                        {
                            if (node == dhcp_server->node_list)
                            {
                                dhcp_server->node_list = node->next;
                            }
                            else
                            {
                                node_prev->next = node->next;
                            }
                            break;
                        }
                        node_prev = node;
                        node = node->next;
                    }

                    if (node != NULL)
                    {
                        mem_free(node);
                    }
                }
                else if (msg_type ==  DHCP_DECLINE)
                {
                    ;
                }
                else if (msg_type == DHCP_INFORM)
                {
                    ;
                }
            }
        }
    }

free_pbuf_and_return:
    pbuf_free(q);
}

/**
* start dhcp server for a netif
*
* @param netif The netif which use dhcp server
* @param start The Start IP address
* @param end The netif which use dhcp server
* @return lwIP error code
* - ERR_OK - No error
* - ERR_MEM - Out of memory
*/
err_t
dhcp_server_start(void *ctx)
{
    struct dhcp_server *dhcp_server;
    struct dhcp_server_arg *arg = (struct dhcp_server_arg *)ctx;
    ip4_addr_t start = arg->start, end = arg->end;
    struct netif *netif = arg->netif;
    free(ctx);

    /* If this netif alreday use the dhcp server. */
    for (dhcp_server = lw_dhcp_server; dhcp_server != NULL; dhcp_server = dhcp_server->next)
    {
        if (dhcp_server->netif == netif)
        {
            dhcp_server->start = start;
            dhcp_server->end = end;
            dhcp_server->current = start;
            return ERR_OK;
        }
    }

    dhcp_server = NULL;
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_server_start(): starting new DHCP server\n"));
    dhcp_server = (struct dhcp_server *)mem_malloc(sizeof(struct dhcp_server));
    if (dhcp_server == NULL)
    {
        LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_server_start(): could not allocate dhcp\n"));
        return ERR_MEM;
    }

    /* clear data structure */
    memset(dhcp_server, 0, sizeof(struct dhcp_server));

    /* store this dhcp server to list */
    dhcp_server->next = lw_dhcp_server;
    lw_dhcp_server = dhcp_server;
    dhcp_server->netif = netif;
    dhcp_server->node_list = NULL;
    dhcp_server->start = start;
    dhcp_server->end = end;
    dhcp_server->current = start;

    /* allocate UDP PCB */
    dhcp_server->pcb = udp_new();
    if (dhcp_server->pcb == NULL)
    {
        LWIP_DEBUGF(DHCP_DEBUG  | LWIP_DBG_TRACE, ("dhcp_server_start(): could not obtain pcb\n"));
        return ERR_MEM;
    }

    ip_set_option(dhcp_server->pcb, SOF_BROADCAST);
    /* set up local and remote port for the pcb */
    udp_bind(dhcp_server->pcb, IP_ADDR_ANY, DHCP_SERVER_PORT);
    //udp_connect(dhcp_server->pcb, IP_ADDR_ANY, DHCP_CLIENT_PORT);
    /* set up the recv callback and argument */
    udp_recv(dhcp_server->pcb, dhcp_server_recv, dhcp_server);
    LWIP_DEBUGF(DHCP_DEBUG | LWIP_DBG_TRACE, ("dhcp_server_start(): starting DHCP server\n"));

    return ERR_OK;
}

//FIXME thread safe??
err_t dhcp_server_stop(struct netif *netif)
{
    struct dhcp_server *dhcp_server;

    /* If this netif is in the dhcp server list. */
    for (dhcp_server = lw_dhcp_server; dhcp_server != NULL; dhcp_server = dhcp_server->next) {
        if (dhcp_server->netif == netif) {
            break;
        }
    }

    if (NULL == dhcp_server) {
        DEBUG_PRINTF("[DHCPD] CRITICAL: no dhcp_server instance found\r\n");
        return ERR_VAL;
    }

    /*clean PCB first*/
    if (dhcp_server->pcb) {
        udp_remove(dhcp_server->pcb);
    }
    /*clean linked list*/
    //FIXME no linked list, just one pointer
    lw_dhcp_server = NULL;
    mem_free(dhcp_server);

    return ERR_OK;
}

//TODO better dhcpd_stop flow?
void dhcpd_start(struct netif *netif, int start, int limit)
{
    err_t res;
    char str_tmp[IP4ADDR_STRLEN_MAX] = DHCPD_SERVER_IP;
    char *p = str_tmp;
    int start_num = DHCPD_CLIENT_IP_MIN, end_num = DHCPD_CLIENT_IP_MAX;
    ip4_addr_t ip_start, ip_end;
    struct dhcp_server_arg *arg;
    ip4_addr_t default_addr, default_mask, default_gw;

    if (1)
    {
        netifapi_dhcp_stop(netif);

        if(start == -1 || limit == -1 || ip4_addr_isany_val(*netif_ip4_addr(netif)))
        {
            ip4addr_aton(DHCPD_SERVER_IP, &default_addr);
            ip4addr_aton("255.255.255.0", &default_mask);
            ip4addr_aton("0.0.0.0", &default_gw);
            netifapi_netif_set_addr(netif, &default_addr, &default_mask, &default_gw);
        }
        else
        {
            memset(str_tmp, 0, IP4ADDR_STRLEN_MAX);
            ip4addr_ntoa_r(netif_ip4_addr(netif), str_tmp, IP4ADDR_STRLEN_MAX);
            start_num = start;
            end_num = start + limit - 1;
        }

        if(start_num > 254 || start_num < 2 || end_num > 254 || end_num < 2 || start_num >= end_num) {
            DEBUG_PRINTF("dhcp start arg error, range %d - %d \r\n", start_num, end_num);
            return;
        }

        netifapi_netif_set_up(netif);
    }

    {
        p = strchr(str_tmp, '.');
        if (p)
        {
            p = strchr(p + 1, '.');
            if (p)
            {
                p = strchr(p + 1, '.');
            }
        }
        if (!p)
        {
            DEBUG_PRINTF("DHCPD_SERVER_IP: %s error!\r\n", str_tmp);
            goto _exit;
        }
        p = p + 1; /* move to xxx.xxx.xxx.^ */

        sprintf(p, "%d", start_num);
        ip4addr_aton(str_tmp, &ip_start);
        DEBUG_PRINTF("ip_start: [%s]\r\n", str_tmp);
        sprintf(p, "%d", end_num);
        ip4addr_aton(str_tmp, &ip_end);
        DEBUG_PRINTF("ip_end: [%s]\r\n", str_tmp);

        arg = (struct dhcp_server_arg *)malloc(sizeof(struct dhcp_server_arg));
        arg->netif = netif;
        arg->start = ip_start;
        arg->end = ip_end;
        res = tcpip_callback((tcpip_callback_fn)dhcp_server_start, arg);
        if (res != ERR_OK)
        {
            free(arg);
            DEBUG_PRINTF("dhcp_server_start res: %d.\r\n", res);
        }
    }

_exit:
    LWIP_NETIF_UNLOCK();
    return;
}

void dhcpd_stop(const char *netif_name)
{
    struct netif *netif = netif_list;

    DEBUG_PRINTF("%s: %s\r\n", __func__, netif_name);

    LWIP_NETIF_LOCK();
    if (strlen(netif_name) > sizeof(netif->name)) {
        DEBUG_PRINTF("network interface name too long!\r\n");
        goto _exit;
    }

    while (netif != NULL) {
        if (strncmp(netif_name, netif->name, sizeof(netif->name)) == 0)
            break;

        netif = netif->next;
        if (netif == NULL)
        {
            DEBUG_PRINTF("network interface: %s not found!\r\n", netif_name);
            break;
        }
    }

    if (netif == NULL) {
        goto _exit;
    }

    dhcp_server_stop(netif);

_exit:
    LWIP_NETIF_UNLOCK();
    return;
}
