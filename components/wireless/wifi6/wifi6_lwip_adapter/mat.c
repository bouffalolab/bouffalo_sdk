/* MAC Address Translation for WiFi STA Interface. */

#include <string.h>
#include "mat.h"
#include "lwip/etharp.h"
#include "lwip/sys.h"
#include "lwip/ip.h"
#include "lwip/udp.h"
#include "lwip/prot/dhcp.h"
#include "lwip/prot/ip6.h"
#include "lwip/prot/icmp6.h"
#include "lwip/prot/nd6.h"
#include "lwip/inet_chksum.h"
#include "pbuf_custom_ref.h"

#define CONN_TUPLE_NUM  16
static struct mat_tuple tuples[CONN_TUPLE_NUM];
/* Cache for the last active tuple */
static struct mat_tuple *last_active_tuple = NULL;

struct mat_mib {
    unsigned int active_tuples;
    unsigned int tuple_overwrite;
    unsigned int err_mem;
    /* egress statistics */
    unsigned int e_missed;
    unsigned int e_handled;
    unsigned int e_arp;
    unsigned int e_ipv4;
    unsigned int e_dhcpv4;
    unsigned int e_ipv6;
    unsigned int e_err_eth;
    unsigned int e_err_arp;
    unsigned int e_err_ipv4;
    unsigned int e_err_ipv6;
    /* ingress statistics */
    unsigned int i_missed;
    unsigned int i_handled;
    unsigned int i_arp;
    unsigned int i_ipv4;
    unsigned int i_dhcpv4;
    unsigned int i_ipv6;
    unsigned int i_err_eth;
    unsigned int i_err_arp;
    unsigned int i_err_ipv4;
    unsigned int i_err_ipv6;
};

static struct mat_mib mib;

/* This function finds a connection tuple by hwaddr and ip. */
struct mat_tuple *mat_tuple_find(uint8_t *hwaddr, ip_addr_t *ip)
{
    if (hwaddr == NULL || ip == NULL)
        return NULL;

    /* First check the cache */
    if (last_active_tuple != NULL &&
        !memcmp(last_active_tuple->hwaddr, hwaddr, sizeof(last_active_tuple->hwaddr)) &&
        ip_addr_cmp(&last_active_tuple->ipaddr, ip)) {
        /* Cache hit */
        last_active_tuple->ts = sys_now();
        return last_active_tuple;
    }

    /* Cache miss, proceed to full search */
    for (int i = 0; i < CONN_TUPLE_NUM; i++) {
        if (tuples[i].used && 
            !memcmp(tuples[i].hwaddr, hwaddr, sizeof(tuples[i].hwaddr)) &&
            ip_addr_cmp(&tuples[i].ipaddr, ip)) {
            /* Update the cache */
            last_active_tuple = &tuples[i];
            last_active_tuple->ts = sys_now();
            return &tuples[i];
        }
    }

    return NULL;
}

struct mat_tuple *mat_tuple_find_by_ip(ip_addr_t *ip)
{
    if (ip == NULL)
        return NULL;

    /* First check the cache */
    if (last_active_tuple != NULL &&
        ip_addr_cmp(&last_active_tuple->ipaddr, ip)) {
        last_active_tuple->ts = sys_now();
        return last_active_tuple;
    }

    /* Cache miss, proceed to full search */
    for (int i = 0; i < CONN_TUPLE_NUM; i++) {
        if (tuples[i].used && ip_addr_cmp(&tuples[i].ipaddr, ip)) {
            last_active_tuple = &tuples[i];
            last_active_tuple->ts = sys_now();
            return &tuples[i];
        }
    }

    return NULL;
}

static void mat_print_tuple(struct mat_tuple *tuple, const char *tag)
{
    uint8_t *hwaddr = tuple->hwaddr;
    ip_addr_t *ipaddr = &tuple->ipaddr;

    if (IP_GET_TYPE(ipaddr) == IPADDR_TYPE_V4) {
        uint32_t ip4addr = ip_addr_get_ip4_u32(ipaddr);
        uint8_t *ipa = (uint8_t *)&ip4addr;

        printf("MAT %s MAC %x:%x:%x:%x:%x:%x IP %d.%d.%d.%d\r\n",
                tag, hwaddr[0], hwaddr[1], hwaddr[2], hwaddr[3],
                hwaddr[4], hwaddr[5], ipa[0], ipa[1], ipa[2], ipa[3]);
    } else if (IP_GET_TYPE(ipaddr) == IPADDR_TYPE_V6) {
        ip6_addr_t *ip6addr = ip_2_ip6(ipaddr);

        uint16_t *ipa = (uint16_t *)ip6addr;
        printf("MAT %s MAC %x:%x:%x:%x:%x:%x "
                "IP %04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\r\n",
                tag, hwaddr[0], hwaddr[1], hwaddr[2], hwaddr[3], hwaddr[4],
                hwaddr[5], lwip_htons(ipa[0]), lwip_htons(ipa[1]),
                lwip_htons(ipa[2]), lwip_htons(ipa[3]), lwip_htons(ipa[4]),
                lwip_htons(ipa[5]), lwip_htons(ipa[6]), lwip_htons(ipa[7]));
    }
}

void mat_print_mib(void)
{
    printf("MAT Statistics: active tuples %lu, tuple table overwrite %lu, "
            "err_mem %lu\r\n",
            mib.active_tuples, mib.tuple_overwrite, mib.err_mem);
    printf("MAT Egress Statistics: handled %lu, arp %lu, ipv4 %lu, "
            "dhcpv4 %lu, ipv6 %lu, missed %lu, err_eth %lu, err_arp %lu, "
            "err_ipv4 %lu, err_ipv6 %lu\r\n",
            mib.e_handled, mib.e_arp, mib.e_ipv4,
            mib.e_dhcpv4, mib.e_ipv6, mib.e_missed, mib.e_err_eth,
            mib.e_err_arp, mib.e_err_ipv4, mib.e_err_ipv6);
    printf("MAT Ingress Statistics: handled %lu, arp %lu, "
            "ipv4 %lu, dhcpv4 %lu, ipv6 %lu, missed %lu, err_eth %lu, "
            "err_arp %lu, err_ipv4 %lu, err_ipv6 %lu\r\n",
            mib.i_handled, mib.i_arp, mib.i_ipv4, mib.i_dhcpv4,
            mib.i_ipv6, mib.i_missed, mib.i_err_eth, mib.i_err_arp,
            mib.i_err_ipv4, mib.i_err_ipv6);
    for (int i = 0; i < mib.active_tuples; i++) {
        if (tuples[i].used)
            mat_print_tuple(&tuples[i], "");
    }
}

int mat_tuple_add(uint8_t *hwaddr, ip_addr_t *ip)
{
    int oldest = -1;
    s32_t idle_max = -1;
    struct mat_tuple *tuple = NULL;

    if (hwaddr == NULL || ip == NULL)
        return -1;

    /* Tuple already exists. */
    if (mat_tuple_find(hwaddr, ip))
        return 0;

    /* Find an empty tuple slot */
    for (int i = 0; i < CONN_TUPLE_NUM; i++) {
        tuple = &tuples[i];
        if (tuple->used == 0) {
            /* Copy hwaddr and ip into the tuple */
            memcpy(tuple->hwaddr, hwaddr, sizeof(tuple->hwaddr));
            ip_addr_copy(tuple->ipaddr, *ip);
            tuple->used = 1;
            tuple->ts = sys_now();
            last_active_tuple = tuple;
            mib.active_tuples++;
            mat_print_tuple(tuple, "ADD");
            return 0;
        }

        s32_t idle = (s32_t)(sys_now() - tuple->ts);
        if (idle_max < idle) {
            idle_max = idle;
            oldest = i;
        }
    }

    if (oldest >= 0) {
        tuple = &tuples[oldest];
        memcpy(tuple->hwaddr, hwaddr, sizeof(tuple->hwaddr));
        ip_addr_copy(tuple->ipaddr, *ip);
        tuple->used = 1;
        tuple->ts = sys_now();
        last_active_tuple = tuple;
        mib.tuple_overwrite++;
        mat_print_tuple(tuple, "ADD");
    }
    return 0;
}

int mat_tuple_del(uint8_t *hwaddr, ip_addr_t *ip)
{
    if (hwaddr == NULL || ip == NULL)
        return -1;

    /* Find the matching tuple */
    for (int i = 0; i < CONN_TUPLE_NUM; i++) {
        if (tuples[i].used && 
            !memcmp(tuples[i].hwaddr, hwaddr, sizeof(tuples[i].hwaddr)) &&
            ip_addr_cmp(&tuples[i].ipaddr, ip)) {
            /* Mark the tuple as unused */
            tuples[i].used = 0;
            mib.active_tuples--;
            return 0;
        }
    }

    return -1;
}

/*
 * Since we are going to modify the ethernet header and ARP procotol payload,
 * it is not a bad idea to make a copy out of the original pbuf, finish the
 * work and return to the caller.
 */
static int mat_arp_handle_egress(struct netif *netif, struct pbuf *p,
                                    struct pbuf **out)
{
    uint32_t sip;
    ip_addr_t ipaddr;
    ip4_addr_t *ip4addr;
    struct pbuf *clone;
    struct eth_hdr *eth_hdr;
    struct etharp_hdr *arp_hdr;

    mib.e_arp++;
    clone = pbuf_clone(PBUF_RAW_TX, PBUF_RAM, p);
    if (!clone) {
        mib.err_mem++;
        mib.e_err_arp++;
        return MAT_ERR_MEM;
    }

    /* This pbuf is too short for ARP message. */
    if (clone->len < SIZEOF_ETH_HDR + SIZEOF_ETHARP_HDR) {
        pbuf_free(clone);
        mib.e_err_arp++;
        return MAT_ERR_DATA;
    }

    /* Modify the clone and leave the original pbuf alone. */
    eth_hdr = (struct eth_hdr *)clone->payload;
    arp_hdr = (struct etharp_hdr *)((char *)eth_hdr + SIZEOF_ETH_HDR);

    IP_SET_TYPE(&ipaddr, IPADDR_TYPE_V4);
    ip4addr = ip_2_ip4(&ipaddr);
    memcpy(&sip, &arp_hdr->sipaddr, sizeof(sip));
    ip4_addr_set_u32(ip4addr, sip);
    /* Ignore ARP probes. */
    if (!ip4_addr_isany_val(*ip4addr))
        mat_tuple_add(arp_hdr->shwaddr.addr, &ipaddr);
    /*
     * Replace the ethernet header and ARP payload source MAC with those
     * of the network interface.
     */
    memcpy(eth_hdr->src.addr, netif->hwaddr, ETH_HWADDR_LEN);
    memcpy(arp_hdr->shwaddr.addr, netif->hwaddr, ETH_HWADDR_LEN);
    *out = clone;
    return MAT_ERR_OK;
}

/* Return 1 if the pbuf is a DHCP request with unicast set in bootp flags. */
static int dhcpv4_unicast_request_check(struct pbuf *p)
{
    uint32_t srcip;
    uint16_t min_size; 
    ip4_addr_t ip4addr;
    struct udp_hdr *uh;
    struct eth_hdr *eth_hdr = (struct eth_hdr *)p->payload;
    struct ip_hdr *iph = (struct ip_hdr *)((char *)eth_hdr + SIZEOF_ETH_HDR);

    min_size = SIZEOF_ETH_HDR + IP_HLEN + UDP_HLEN;
    min_size += offsetof(struct dhcp_msg, giaddr);
    if (p->len < min_size)
        return 0;

    memcpy(&srcip, &iph->src, sizeof(srcip));
    ip4_addr_set_u32(&ip4addr, srcip);
    if (IPH_PROTO(iph) == IP_PROTO_UDP) {
        uh = (struct udp_hdr *)((char *)iph + IPH_HL_BYTES(iph));

        if (lwip_ntohs(uh->dest) == 67) {
            uint16_t brd_msk = lwip_htons(0x8000);
            struct dhcp_msg *dmsg = (struct dhcp_msg *)((char *)uh + UDP_HLEN);

            return !(dmsg->flags & brd_msk);
        }
    }
    return 0;
}

/* Return 1 if the pbuf is a packet from DHCP server. */
static int dhcpv4_server_resp_check(struct pbuf *p)
{
    uint16_t min_size;
    struct udp_hdr *uh;
    struct eth_hdr *eth_hdr = (struct eth_hdr *)p->payload;
    struct ip_hdr *iph = (struct ip_hdr *)((char *)eth_hdr + SIZEOF_ETH_HDR);

    min_size = SIZEOF_ETH_HDR + IP_HLEN + UDP_HLEN;
    if (p->len < min_size)
        return 0;

    if (IPH_PROTO(iph) == IP_PROTO_UDP) {
        uh = (struct udp_hdr *)((char *)iph + IPH_HL_BYTES(iph));
        if (lwip_ntohs(uh->src) == 67)
            return 1;
    }
    return 0;
}

static int mat_ipv4_handle_egress(struct netif *netif, struct pbuf *p,
                                    struct pbuf **out)
{
    ip_addr_t ipaddr;
    struct ip_hdr *iph;
    struct pbuf *clone;
    struct eth_hdr *eth_hdr;

    mib.e_ipv4++;
    if (p->len < SIZEOF_ETH_HDR + IP_HLEN) {
        mib.e_err_ipv4++;
        return MAT_ERR_DATA;
    }

    /* XXX DHCP message flags is modified but never restored. */
    if (dhcpv4_unicast_request_check(p)) {
        uint16_t min_size; 
        struct udp_hdr *uh;
        struct dhcp_msg *dmsg;

        mib.e_dhcpv4++;
        /* We need to make a copy to modify DHCP protocol data. */
        ip_addr_t src, dst;

        clone = pbuf_clone(PBUF_RAW_TX, PBUF_RAM, p);
        if (!clone) {
            mib.err_mem++;
            mib.e_err_ipv4++;
            return MAT_ERR_MEM;
        }

        min_size = SIZEOF_ETH_HDR + IP_HLEN + UDP_HLEN;
        min_size += offsetof(struct dhcp_msg, giaddr);
        if (clone->len < min_size) {
            mib.e_err_ipv4++;
            pbuf_free(clone);
            return MAT_ERR_DATA;
        }
        eth_hdr = (struct eth_hdr *)clone->payload;
        iph = (struct ip_hdr *)((char *)eth_hdr + SIZEOF_ETH_HDR);
        uh = (struct udp_hdr *)((char *)iph + IPH_HL_BYTES(iph));
        dmsg = (struct dhcp_msg *)((char *)uh + UDP_HLEN);

        /* Set broadcast in bootp flags manually. */
        dmsg->flags |= lwip_htons(0x8000);
        ip_addr_copy_from_ip4(src, iph->src);
        ip_addr_copy_from_ip4(dst, iph->dest);
        /*
         * Hide the ethernet header and IP header temporarily for
         * checksum calculation.
         */
        pbuf_remove_header(clone, SIZEOF_ETH_HDR + IPH_HL_BYTES(iph));
        uh->chksum = lwip_htons(0);
        uh->chksum = ip_chksum_pseudo(clone, IP_PROTO_UDP, clone->tot_len,
                                        &src, &dst);
        if (uh->chksum == 0)
            uh->chksum = lwip_htons(0xFFFF);
        /* Reveal the ethernet header and IP header again. */
        pbuf_add_header(clone, SIZEOF_ETH_HDR + IPH_HL_BYTES(iph));
        /*
         * Replace the ethernet header source MAC with that of the
         * network interface.
         */
        memcpy(eth_hdr->src.addr, netif->hwaddr, ETH_HWADDR_LEN);
        *out = clone;
        return MAT_ERR_OK;
    }

#ifdef CONFIG_MAT_FULL_COPY
    clone = pbuf_clone(PBUF_RAW_TX, PBUF_RAM, p);
    if (!clone) {
        mib.err_mem++;
        mib.e_err_ipv4++;
        return MAT_ERR_MEM;
    }

    /* Add the tuple (original source MAC, source IP). */
    eth_hdr = (struct eth_hdr *)clone->payload;
    iph = (struct ip_hdr *)((char *)eth_hdr + SIZEOF_ETH_HDR);

    ip_addr_copy_from_ip4(ipaddr, iph->src);
    if (!ip_addr_isany_val(ipaddr))
        mat_tuple_add(eth_hdr->src.addr, &ipaddr);
    /*
     * Replace the ethernet header source MAC with that of the
     * network interface.
     */
    memcpy(eth_hdr->src.addr, netif->hwaddr, ETH_HWADDR_LEN);
    *out = clone;
    return MAT_ERR_OK;
/* CONFIG_MAT_FULL_COPY is not defined. */
#else
    struct pbuf *header, *ref;
    u16_t ethhdr_size = SIZEOF_ETH_HDR;

    /* All we need to do is to modify the ethernet header. */
    header = pbuf_alloc(PBUF_RAW_TX, ethhdr_size, PBUF_RAM); 
    if (!header) {
        mib.err_mem++;
        mib.e_err_ipv4++;
        return MAT_ERR_MEM;
    }
    memcpy(header->payload, p->payload, ethhdr_size);

    /* Create a reference to the following data. */
    ref = pbuf_custom_ref_create(p, ethhdr_size, p->tot_len - ethhdr_size);
    if (!ref) {
        mib.err_mem++;
        mib.e_err_ipv4++;
        pbuf_free(header);
        return MAT_ERR_MEM;
    }

    pbuf_cat(header, ref);

    /* Add the tuple (original source MAC, source IP). */
    eth_hdr = (struct eth_hdr *)p->payload;
    iph = (struct ip_hdr *)((char *)eth_hdr + SIZEOF_ETH_HDR);

    ip_addr_copy_from_ip4(ipaddr, iph->src);
    if (!ip_addr_isany_val(ipaddr))
        mat_tuple_add(eth_hdr->src.addr, &ipaddr);
    /*
     * Replace the ethernet header source MAC with that of the
     * network interface.
     */
    eth_hdr = (struct eth_hdr *)header->payload;
    memcpy(eth_hdr->src.addr, netif->hwaddr, ETH_HWADDR_LEN);
    *out = header;
    return MAT_ERR_OK;
/* CONFIG_MAT_FULL_COPY. */
#endif
}

/*
 * Return 1 if the pbuf is ICMPv6 NS or NA.
 * TODO check RS.
 */
static int ipv6_ndp_check(struct pbuf *p)
{
    uint16_t min_size;
    struct icmp6_hdr *icmp_hdr;
    struct ip6_hdr *ip6h = (struct ip6_hdr *)((char *)p->payload + SIZEOF_ETH_HDR);

    if (IP6H_NEXTH(ip6h) == IP6_NEXTH_ICMP6) {
        icmp_hdr = (struct icmp6_hdr *)((char *)ip6h + IP6_HLEN);
        min_size = SIZEOF_ETH_HDR + IP6_HLEN + sizeof(struct icmp6_hdr);
        if (p->len < min_size)
            return 0;

        return icmp_hdr->type == ICMP6_TYPE_NS || icmp_hdr->type == ICMP6_TYPE_NA;
    }
    return 0;
}

/* TODO handle RS. */
static struct pbuf *mat_icmpv6_handle_egress(struct netif *netif, struct pbuf *p)
{
    uint16_t min_size;
    struct pbuf *clone;
    struct na_header *nah;
    struct ns_header *nsh;
    struct icmp6_hdr *icmp_hdr;
    ip_addr_t ipaddr, src, dest;
    struct lladdr_option *llopt;

    clone = pbuf_clone(PBUF_RAW_TX, PBUF_RAM, p);
    if (!clone) {
        mib.err_mem++;
        mib.e_err_ipv6++;
        return NULL;
    }

    /* Modify the clone and leave the original pbuf alone. */
    struct eth_hdr *eth_hdr = (struct eth_hdr *)clone->payload;
    struct ip6_hdr *ip6h = (struct ip6_hdr *)((char *)eth_hdr + SIZEOF_ETH_HDR);
    icmp_hdr = (struct icmp6_hdr *)((char *)ip6h + IP6_HLEN);
    ip_addr_copy_from_ip6_packed(ipaddr, ip6h->src);

    if (icmp_hdr->type == ICMP6_TYPE_NS) {
        nsh = (struct ns_header *)icmp_hdr;
        min_size = SIZEOF_ETH_HDR + IP6_HLEN + sizeof(struct ns_header);
        if (clone->len < min_size) {
            mib.e_err_ipv6++;
            pbuf_free(clone);
            return NULL;
        }

        /* Check if link layer address is provided. */
        if (clone->len >= min_size + 2) {
            llopt = (struct lladdr_option *)((char *)nsh + sizeof(*nsh));
            min_size += (llopt->length << 3);
            if (clone->len < min_size) {
                mib.e_err_ipv6++;
                pbuf_free(clone);
                return NULL;
            }

            /* Now we get both source IP and source MAC, add it. */
            if (!ip_addr_isany_val(ipaddr))
                mat_tuple_add(llopt->addr, &ipaddr);
            /* Replace the hardware address with that of the netif. */
            memcpy(llopt->addr, netif->hwaddr, sizeof(llopt->addr));
            /*
             * Since the lladdr_option has been modified, hide the ethernet
             * header and IP header temporarily for checksum calculation.
             */
            pbuf_remove_header(clone, SIZEOF_ETH_HDR + IP6_HLEN);
            ip_addr_copy_from_ip6_packed(src, ip6h->src);
            ip_addr_copy_from_ip6_packed(dest, ip6h->dest);
            icmp_hdr->chksum = lwip_htons(0);
            icmp_hdr->chksum = ip_chksum_pseudo(clone, IP_PROTO_ICMPV6,
                                                clone->tot_len, &src, &dest);
            if (icmp_hdr->chksum == 0)
                icmp_hdr->chksum = lwip_htons(0xFFFF);
            pbuf_add_header(clone, SIZEOF_ETH_HDR + IP6_HLEN);
            memcpy(eth_hdr->src.addr, netif->hwaddr, ETH_HWADDR_LEN);
            return clone;
        }

        /* No link layer address is provided, free the clone. */
        pbuf_free(clone);
        return NULL;
    }

    if (icmp_hdr->type == ICMP6_TYPE_NA) {
        nah = (struct na_header *)icmp_hdr;
        min_size = SIZEOF_ETH_HDR + IP6_HLEN + sizeof(struct na_header);
        if (clone->len < min_size) {
            mib.e_err_ipv6++;
            pbuf_free(clone);
            return NULL;
        }

        /* Check if link layer address is provided. */
        if (clone->len >= min_size + 2) {
            llopt = (struct lladdr_option *)((char *)nah + sizeof(*nah));
            min_size += (llopt->length << 3);
            if (clone->len < min_size)
                llopt = NULL;
        } else {
            llopt = NULL;
        }

        /* ICMPv6 NA without hardware address is bad. */
        if (!llopt) {
            mib.e_err_ipv6++;
            pbuf_free(clone);
            return NULL;
        }

        /* Now we get both source IP and source MAC, add it. */
        if (!ip_addr_isany_val(ipaddr))
            mat_tuple_add(llopt->addr, &ipaddr);
        /* Replace the hardware address with that of the netif. */
        memcpy(llopt->addr, netif->hwaddr, sizeof(llopt->addr));
        /*
         * Since lladdr_option has been modified, hide the ethernet header
         * and IP header temporarily for checksum calculation.
         */
        pbuf_remove_header(clone, SIZEOF_ETH_HDR + IP6_HLEN);
        ip_addr_copy_from_ip6_packed(src, ip6h->src);
        ip_addr_copy_from_ip6_packed(dest, ip6h->dest);
        icmp_hdr->chksum = lwip_htons(0);
        icmp_hdr->chksum = ip_chksum_pseudo(clone, IP_PROTO_ICMPV6,
                                            clone->tot_len, &src, &dest);
        if (icmp_hdr->chksum == 0)
            icmp_hdr->chksum = lwip_htons(0xFFFF);
        pbuf_add_header(clone, SIZEOF_ETH_HDR + IP6_HLEN);

        /*
         * Replace the ethernet header and ARP payload source MAC with those
         * of the network interface.
         */
        memcpy(eth_hdr->src.addr, netif->hwaddr, ETH_HWADDR_LEN);
        return clone;
    }

    return NULL;
}

/* XXX Assuming that there are no extension headers. */
static int mat_ipv6_handle_egress(struct netif *netif, struct pbuf *p,
                                    struct pbuf **out)
{
    ip_addr_t ipaddr;
    struct ip6_hdr *ip6h;
    struct eth_hdr *eth_hdr;

    mib.e_ipv6++;
    if (p->len < SIZEOF_ETH_HDR + IP6_HLEN) {
        mib.e_err_ipv6++;
        return MAT_ERR_DATA;
    }

    if (ipv6_ndp_check(p))
        return mat_icmpv6_handle_egress(netif, p);

#ifdef CONFIG_MAT_FULL_COPY
    struct pbuf *clone;

    clone = pbuf_clone(PBUF_RAW_TX, PBUF_RAM, p);
    if (!clone) {
        mib.err_mem++;
        mib.e_err_ipv6++;
        return MAT_ERR_MEM;
    }
    /* Add the tuple (original source MAC, source IP). */
    eth_hdr = (struct eth_hdr *)clone->payload;
    ip6h = (struct ip6_hdr *)((char *)eth_hdr + SIZEOF_ETH_HDR);
    ip_addr_copy_from_ip6_packed(ipaddr, ip6h->src);
    if (!ip_addr_isany_val(ipaddr))
        mat_tuple_add(eth_hdr->src.addr, &ipaddr);
    memcpy(eth_hdr->src.addr, netif->hwaddr, ETH_HWADDR_LEN);
    *out = clone;
    return MAT_ERR_OK;
/* CONFIG_MAT_FULL_COPY is not defined. */
#else
    struct pbuf *header, *ref;
    u16_t ethhdr_size = SIZEOF_ETH_HDR;

    /* All we need to do is to modify the ethernet header. */
    header = pbuf_alloc(PBUF_RAW_TX, ethhdr_size, PBUF_RAM); 
    if (!header) {
        mib.err_mem++;
        mib.e_err_ipv6++;
        return MAT_ERR_MEM;
    }
    memcpy(header->payload, p->payload, ethhdr_size);

    /* Create a reference to the following data. */
    ref = pbuf_custom_ref_create(p, ethhdr_size, p->tot_len - ethhdr_size);
    if (!ref) {
        pbuf_free(header);
        mib.err_mem++;
        mib.e_err_ipv6++;
        return MAT_ERR_MEM;
    }

    pbuf_cat(header, ref);

    /* Add the tuple (original source MAC, source IP). */
    eth_hdr = (struct eth_hdr *)p->payload;
    ip6h = (struct ip6_hdr *)((char *)eth_hdr + SIZEOF_ETH_HDR);
    ip_addr_copy_from_ip6_packed(ipaddr, ip6h->src);
    if (!ip_addr_isany_val(ipaddr))
        mat_tuple_add(eth_hdr->src.addr, &ipaddr);
    /*
     * Replace the ethernet header source MAC with that of the
     * network interface.
     */
    eth_hdr = (struct eth_hdr *)header->payload;
    memcpy(eth_hdr->src.addr, netif->hwaddr, ETH_HWADDR_LEN);
    *out = header;
    return MAT_ERR_OK;
/* CONFIG_MAT_FULL_COPY. */
#endif
}

/*
 * Handle MAC address translation before sending the packet (egress)
 * Return pbuf for transmision.
 */
int mat_handle_egress(struct netif *netif, struct pbuf *p, struct pbuf **out)
{
    struct eth_hdr *eth_hdr = p->payload;

    if (!netif || !p || !out)
        return MAT_ERR_INVAL;

    if (!netif_is_up(netif) || !netif_is_link_up(netif)) {
        mib.e_missed++;
        return MAT_ERR_STATUS;
    }

    *out = NULL;
    mib.e_handled++;
    if (p->len < ETH_HWADDR_LEN) {
        mib.e_err_eth++;
        return MAT_ERR_DATA;
    }
    /* Do not touch local output traffic. */
    if (!memcmp(eth_hdr->src.addr, netif->hwaddr, ETH_HWADDR_LEN)) {
        *out = p;
        return MAT_ERR_OK;
    }

    if (eth_hdr->type == PP_HTONS(ETHTYPE_ARP))
        return mat_arp_handle_egress(netif, p, out);

    if (eth_hdr->type == PP_HTONS(ETHTYPE_IP))
        return mat_ipv4_handle_egress(netif, p, out);

    if (eth_hdr->type == PP_HTONS(ETHTYPE_IPV6))
        return mat_ipv6_handle_egress(netif, p, out);

    return MAT_ERR_PROTO;
}

static int mat_arp_handle_ingress(struct netif *netif, struct pbuf *p)
{
    uint32_t destip;
    ip_addr_t ipaddr;
    ip4_addr_t *ip4addr;
    struct etharp_hdr *arp_hdr;
    struct eth_hdr *eth_hdr;
    struct mat_tuple *tuple;

    mib.i_arp++;
    if (p->len < SIZEOF_ETH_HDR + SIZEOF_ETHARP_HDR) {
        mib.i_err_arp++;
        return MAT_ERR_DATA;
    }
    eth_hdr = (struct eth_hdr *)p->payload;
    arp_hdr = (struct etharp_hdr *)((char *)eth_hdr + SIZEOF_ETH_HDR);
    memcpy(&destip, &arp_hdr->dipaddr, sizeof(destip));
    IP_SET_TYPE(&ipaddr, IPADDR_TYPE_V4);
    ip4addr = ip_2_ip4(&ipaddr);
    ip4_addr_set_u32(ip4addr, destip);
    tuple = mat_tuple_find_by_ip(&ipaddr);
    /* If tuple found, modify target MAC in ARP payload and Ethernet header */
    if (tuple != NULL) {
        memcpy(eth_hdr->dest.addr, tuple->hwaddr, ETH_HWADDR_LEN);
        memcpy(arp_hdr->dhwaddr.addr, tuple->hwaddr, ETH_HWADDR_LEN);
    }
    return MAT_ERR_OK;
}

static int mat_ipv4_handle_ingress(struct netif *netif, struct pbuf *p)
{
    ip_addr_t ipaddr;
    struct ip_hdr *iph;
    struct eth_hdr *eth_hdr;
    struct mat_tuple *tuple;

    mib.i_ipv4++;
    if (p->len < SIZEOF_ETH_HDR + IP_HLEN) {
        mib.i_err_ipv4++;
        return MAT_ERR_DATA;
    }

    eth_hdr = (struct eth_hdr *)p->payload;
    iph = (struct ip_hdr *)((char *)eth_hdr + SIZEOF_ETH_HDR);
    ip_addr_copy_from_ip4(ipaddr, iph->dest);

    /* Try to restore ethernet destination MAC address. */
    tuple = mat_tuple_find_by_ip(&ipaddr);
    if (tuple)
        memcpy(eth_hdr->dest.addr, tuple->hwaddr, ETH_HWADDR_LEN);
    return MAT_ERR_OK;
}

static int mat_ipv6_handle_ingress(struct netif *netif, struct pbuf *p)
{
    ip_addr_t ipaddr;
    struct ip6_hdr *ip6h;
    struct eth_hdr *eth_hdr;
    struct mat_tuple *tuple;

    mib.i_ipv6++;
    if (p->len < SIZEOF_ETH_HDR + IP6_HLEN) {
        mib.i_err_ipv6++;
        return MAT_ERR_DATA;
    }

    eth_hdr = (struct eth_hdr *)p->payload;
    ip6h = (struct ip6_hdr *)((char *)eth_hdr + ETH_HWADDR_LEN);
    ip_addr_copy_from_ip6_packed(ipaddr, ip6h->dest);

    /* Try to restore ethernet destination MAC address. */
    tuple = mat_tuple_find_by_ip(&ipaddr);
    if (tuple)
        memcpy(eth_hdr->dest.addr, tuple->hwaddr, ETH_HWADDR_LEN);
    return MAT_ERR_OK;
}

/*
 * Since the received pbuf has not been seen by others, we can modify the
 * payload.
 */
int mat_handle_ingress(struct netif *netif, struct pbuf *p)
{
    struct eth_hdr *eth_hdr;

    if (!netif || !p)
        return MAT_ERR_INVAL;

    if (!netif_is_up(netif) || !netif_is_link_up(netif)) {
        mib.i_missed++;
        return MAT_ERR_STATUS;
    }

    mib.i_handled++;
    if (p->len < SIZEOF_ETH_HDR) {
        mib.i_err_eth++;
        return MAT_ERR_DATA;
    }

    eth_hdr = p->payload;
    /* Only process traffic that targets myself. */
    if (memcmp(eth_hdr->dest.addr, netif->hwaddr, ETH_HWADDR_LEN)) {
        /* DHCP is special, just account it. */
        if (eth_hdr->type == PP_HTONS(ETHTYPE_IP) && dhcpv4_server_resp_check(p))
            mib.i_dhcpv4++;
        return MAT_ERR_OK;
    }

    if (eth_hdr->type == PP_HTONS(ETHTYPE_ARP))
        return mat_arp_handle_ingress(netif, p);

    if (eth_hdr->type == PP_HTONS(ETHTYPE_IP))
        return mat_ipv4_handle_ingress(netif, p);

    if (eth_hdr->type == PP_HTONS(ETHTYPE_IPV6))
        return mat_ipv6_handle_ingress(netif, p);

    return MAT_ERR_PROTO;
}
