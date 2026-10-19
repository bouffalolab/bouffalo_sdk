/*
 * Simple packet filter.
 *
 * NOTE: We assume that checked fields are located at first node of pbuf chain.
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <lwip/def.h>
#include <lwip/prot/ethernet.h>
#include <lwip/prot/ip.h>
#include <lwip/prot/ip4.h>
#include <lwip/prot/udp.h>
#include <lwip/prot/tcp.h>
#include <lwip/pbuf.h>

#define NPF_ETH_HDR_LEN   14
#define NPF_IP4_HDR_LEN   20
#define NPF_ICMP4_HDR_LEN 8
#define NPF_TCP4_HDR_LEN  20
#define NPF_UDP4_HDR_LEN  8

#define ETHTYPE_8021X 0x888e

static inline bool npf_is_arp(const struct pbuf *skb)
{
    const struct eth_hdr *hdr = (const struct eth_hdr *)skb->payload;
    if (!(skb->len > NPF_ETH_HDR_LEN)) {
        return false;
    }
    if (hdr->type == PP_HTONS(ETHTYPE_ARP)) {
        return true;
    }
    return false;
}

static inline bool npf_is_8021X(const struct pbuf *skb)
{
    const struct eth_hdr *hdr = (const struct eth_hdr *)skb->payload;
    if (!(skb->len > NPF_ETH_HDR_LEN)) {
        return false;
    }
    if (hdr->type == PP_HTONS(ETHTYPE_8021X)) {
        return true;
    }
    return false;
}

static inline bool npf_is_ip4(const struct pbuf *skb)
{
    const struct eth_hdr *hdr = (const struct eth_hdr *)skb->payload;
    if (!(skb->len > NPF_ETH_HDR_LEN + NPF_IP4_HDR_LEN)) {
        return false;
    }
    if (hdr->type == PP_HTONS(ETHTYPE_IP)) {
        return true;
    }
    return false;
}

static inline bool npf_is_icmp4(const struct pbuf *skb)
{
    if (!(skb->len > NPF_ETH_HDR_LEN + NPF_IP4_HDR_LEN + NPF_ICMP4_HDR_LEN)) {
        return false;
    }
    if (npf_is_ip4(skb)) {
        const struct ip_hdr *iphdr = (const struct ip_hdr *)(skb->payload + NPF_ETH_HDR_LEN);
        if (IPH_PROTO(iphdr) == IP_PROTO_ICMP) {
            return true;
        }
    }

    return false;
}

static inline bool npf_is_dhcp4(const struct pbuf *skb)
{
    if (!(skb->len > NPF_ETH_HDR_LEN + NPF_IP4_HDR_LEN + NPF_UDP4_HDR_LEN)) {
        return false;
    }
    const struct ip_hdr *iphdr = (const struct ip_hdr *)(skb->payload + NPF_ETH_HDR_LEN);
    const struct udp_hdr *udphdr = (const struct udp_hdr *)(skb->payload + NPF_ETH_HDR_LEN + NPF_IP4_HDR_LEN);
    const uint16_t srcport = lwip_ntohs(udphdr->src);
    const uint16_t dstport = lwip_ntohs(udphdr->dest);

    if (((srcport == 67 && dstport == 68) || (srcport == 68 && dstport == 67)) &&
            IPH_PROTO(iphdr) == IP_PROTO_UDP &&
            npf_is_ip4(skb)) {
        return true;
    }
    return false;
}

static inline bool npf_is_tcp4_port(const struct pbuf *skb, uint16_t port)
{
    if (!npf_is_ip4(skb)) {
        return false;
    }
    if (!(skb->len >= NPF_ETH_HDR_LEN + NPF_IP4_HDR_LEN + NPF_TCP4_HDR_LEN)) {
        return false;
    }
    const struct tcp_hdr *tcphdr = (const struct tcp_hdr *)(skb->payload + NPF_ETH_HDR_LEN + NPF_IP4_HDR_LEN);
    if (lwip_htons(port) == tcphdr->src || lwip_htons(port) == tcphdr->dest) {
        return true;
    }
    return false;
}

static inline bool npf_is_udp4_port(const struct pbuf *skb, uint16_t port)
{
    if (!npf_is_ip4(skb)) {
        return false;
    }
    if (!(skb->len >= NPF_ETH_HDR_LEN + NPF_IP4_HDR_LEN + NPF_UDP4_HDR_LEN)) {
        return false;
    }
    const struct udp_hdr *hdr = (const struct udp_hdr *)(skb->payload + NPF_ETH_HDR_LEN + NPF_IP4_HDR_LEN);
    if (lwip_htons(port) == hdr->src || lwip_htons(port) == hdr->dest) {
        return true;
    }
    return false;
}
