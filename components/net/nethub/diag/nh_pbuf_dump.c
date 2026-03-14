#include "nh_pbuf_dump.h"
#include "nh_packet_filter.h"

#include <lwip/pbuf.h>
#include <lwip/prot/ip6.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define NH_PBUF_DUMP_PREVIEW_LEN  16

static bool nh_diag_pbuf_read(const struct pbuf *p, uint16_t offset, void *dst, size_t len)
{
    size_t end;

    if (p == NULL || dst == NULL) {
        return false;
    }

    end = (size_t)offset + len;
    if (end > p->tot_len) {
        return false;
    }

    return pbuf_copy_partial(p, dst, (u16_t)len, offset) == (u16_t)len;
}

static unsigned int nh_diag_pbuf_segment_count(const struct pbuf *p)
{
    return (p != NULL) ? (unsigned int)pbuf_clen(p) : 0U;
}

static void nh_diag_format_mac(char *out, size_t out_size, const uint8_t mac[ETH_HWADDR_LEN])
{
    snprintf(out, out_size, "%02X:%02X:%02X:%02X:%02X:%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

static void nh_diag_format_ip4(char *out, size_t out_size, const uint8_t ip[4])
{
    snprintf(out, out_size, "%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);
}

static void nh_diag_format_ip6(char *out, size_t out_size, const uint8_t ip[16])
{
    snprintf(out, out_size, "%x:%x:%x:%x:%x:%x:%x:%x",
             ((uint16_t)ip[0] << 8) | ip[1],
             ((uint16_t)ip[2] << 8) | ip[3],
             ((uint16_t)ip[4] << 8) | ip[5],
             ((uint16_t)ip[6] << 8) | ip[7],
             ((uint16_t)ip[8] << 8) | ip[9],
             ((uint16_t)ip[10] << 8) | ip[11],
             ((uint16_t)ip[12] << 8) | ip[13],
             ((uint16_t)ip[14] << 8) | ip[15]);
}

static void nh_diag_pbuf_preview(char *out, size_t out_size, const struct pbuf *p)
{
    uint8_t bytes[NH_PBUF_DUMP_PREVIEW_LEN];
    size_t preview_len;
    size_t pos = 0;
    size_t i;
    int written;

    if (out_size == 0) {
        return;
    }

    out[0] = '\0';
    if (p == NULL) {
        return;
    }

    preview_len = p->tot_len;
    if (preview_len > sizeof(bytes)) {
        preview_len = sizeof(bytes);
    }

    if (!nh_diag_pbuf_read(p, 0, bytes, preview_len)) {
        snprintf(out, out_size, "<unavailable>");
        return;
    }

    for (i = 0; i < preview_len && pos < out_size; i++) {
        written = snprintf(out + pos, out_size - pos, "%s%02X",
                           (i == 0) ? "" : " ",
                           bytes[i]);
        if (written < 0 || (size_t)written >= (out_size - pos)) {
            pos = out_size - 1;
            break;
        }
        pos += (size_t)written;
    }

    if ((size_t)p->tot_len > preview_len && pos < out_size) {
        written = snprintf(out + pos, out_size - pos, " ...");
        if (written < 0 || (size_t)written >= (out_size - pos)) {
            pos = out_size - 1;
        }
    }

    out[out_size - 1] = '\0';
}

static const char *nh_diag_udp_service(uint16_t src_port, uint16_t dst_port)
{
    if ((src_port == 67 && dst_port == 68) || (src_port == 68 && dst_port == 67)) {
        return "DHCP";
    }

    if (src_port == 53 || dst_port == 53) {
        return "DNS";
    }

    if (src_port == 123 || dst_port == 123) {
        return "NTP";
    }

    return NULL;
}

static void nh_diag_parse_arp(char *details, size_t details_size, const struct pbuf *p)
{
    uint16_t opcode_be;
    uint16_t opcode;
    uint8_t sender_ip[4];
    uint8_t target_ip[4];
    char sender_ip_str[16];
    char target_ip_str[16];

    if (!nh_diag_pbuf_read(p, NPF_ETH_HDR_LEN + 6, &opcode_be, sizeof(opcode_be))) {
        snprintf(details, details_size, " arp=<truncated>");
        return;
    }

    opcode = lwip_ntohs(opcode_be);
    if (!nh_diag_pbuf_read(p, NPF_ETH_HDR_LEN + 14, sender_ip, sizeof(sender_ip)) ||
        !nh_diag_pbuf_read(p, NPF_ETH_HDR_LEN + 24, target_ip, sizeof(target_ip))) {
        snprintf(details, details_size, " arp op=%u", opcode);
        return;
    }

    nh_diag_format_ip4(sender_ip_str, sizeof(sender_ip_str), sender_ip);
    nh_diag_format_ip4(target_ip_str, sizeof(target_ip_str), target_ip);
    if (opcode == 1 || opcode == 2) {
        snprintf(details, details_size, " arp %s %s -> %s",
                 opcode == 1 ? "request" : "reply",
                 sender_ip_str,
                 target_ip_str);
    } else {
        snprintf(details, details_size, " arp op=%u %s -> %s",
                 opcode,
                 sender_ip_str,
                 target_ip_str);
    }
}

static void nh_diag_parse_ip4(char *details, size_t details_size, const struct pbuf *p)
{
    struct ip_hdr iphdr;
    uint16_t ip_hdr_len;
    uint16_t l4_offset;
    uint8_t src_ip[4];
    uint8_t dst_ip[4];
    char src_ip_str[16];
    char dst_ip_str[16];

    if (!nh_diag_pbuf_read(p, NPF_ETH_HDR_LEN, &iphdr, sizeof(iphdr))) {
        snprintf(details, details_size, " ipv4=<truncated>");
        return;
    }

    ip_hdr_len = (uint16_t)IPH_HL(&iphdr) * 4U;
    if (ip_hdr_len < NPF_IP4_HDR_LEN) {
        snprintf(details, details_size, " ipv4=<invalid ihl=%u>", ip_hdr_len);
        return;
    }

    if (!nh_diag_pbuf_read(p, NPF_ETH_HDR_LEN + 12, src_ip, sizeof(src_ip)) ||
        !nh_diag_pbuf_read(p, NPF_ETH_HDR_LEN + 16, dst_ip, sizeof(dst_ip))) {
        snprintf(details, details_size, " ipv4=<addr truncated>");
        return;
    }

    nh_diag_format_ip4(src_ip_str, sizeof(src_ip_str), src_ip);
    nh_diag_format_ip4(dst_ip_str, sizeof(dst_ip_str), dst_ip);
    l4_offset = NPF_ETH_HDR_LEN + ip_hdr_len;

    switch (IPH_PROTO(&iphdr)) {
        case IP_PROTO_TCP: {
            struct {
                uint16_t src;
                uint16_t dst;
            } ports;

            if (!nh_diag_pbuf_read(p, l4_offset, &ports, sizeof(ports))) {
                snprintf(details, details_size, " tcp %s -> %s <truncated>",
                         src_ip_str, dst_ip_str);
                return;
            }

            snprintf(details, details_size, " tcp %s:%u -> %s:%u",
                     src_ip_str, lwip_ntohs(ports.src),
                     dst_ip_str, lwip_ntohs(ports.dst));
            return;
        }
        case IP_PROTO_UDP: {
            struct {
                uint16_t src;
                uint16_t dst;
            } ports;
            uint16_t src_port;
            uint16_t dst_port;
            const char *service;

            if (!nh_diag_pbuf_read(p, l4_offset, &ports, sizeof(ports))) {
                snprintf(details, details_size, " udp %s -> %s <truncated>",
                         src_ip_str, dst_ip_str);
                return;
            }

            src_port = lwip_ntohs(ports.src);
            dst_port = lwip_ntohs(ports.dst);
            service = nh_diag_udp_service(src_port, dst_port);
            if (service != NULL) {
                snprintf(details, details_size, " udp %s:%u -> %s:%u [%s]",
                         src_ip_str, src_port,
                         dst_ip_str, dst_port,
                         service);
            } else {
                snprintf(details, details_size, " udp %s:%u -> %s:%u",
                         src_ip_str, src_port,
                         dst_ip_str, dst_port);
            }
            return;
        }
        case IP_PROTO_ICMP: {
            uint8_t icmp_header[2];

            if (!nh_diag_pbuf_read(p, l4_offset, icmp_header, sizeof(icmp_header))) {
                snprintf(details, details_size, " icmp %s -> %s <truncated>",
                         src_ip_str, dst_ip_str);
                return;
            }

            snprintf(details, details_size, " icmp %s -> %s type=%u code=%u",
                     src_ip_str, dst_ip_str, icmp_header[0], icmp_header[1]);
            return;
        }
        default:
            snprintf(details, details_size, " ipv4 %s -> %s proto=%u",
                     src_ip_str, dst_ip_str, IPH_PROTO(&iphdr));
            return;
    }
}

static void nh_diag_parse_ip6(char *details, size_t details_size, const struct pbuf *p)
{
    struct ip6_hdr ip6hdr;
    uint8_t src_ip[16];
    uint8_t dst_ip[16];
    char src_ip_str[40];
    char dst_ip_str[40];

    if (!nh_diag_pbuf_read(p, NPF_ETH_HDR_LEN, &ip6hdr, sizeof(ip6hdr))) {
        snprintf(details, details_size, " ipv6=<truncated>");
        return;
    }

    if (!nh_diag_pbuf_read(p, NPF_ETH_HDR_LEN + 8, src_ip, sizeof(src_ip)) ||
        !nh_diag_pbuf_read(p, NPF_ETH_HDR_LEN + 24, dst_ip, sizeof(dst_ip))) {
        snprintf(details, details_size, " ipv6=<addr truncated>");
        return;
    }

    nh_diag_format_ip6(src_ip_str, sizeof(src_ip_str), src_ip);
    nh_diag_format_ip6(dst_ip_str, sizeof(dst_ip_str), dst_ip);
    snprintf(details, details_size, " ipv6 %s -> %s nexth=%u",
             src_ip_str, dst_ip_str, IP6H_NEXTH(&ip6hdr));
}

void nh_diag_dump_pbuf(const char *tag, const struct pbuf *p)
{
    struct eth_hdr ethhdr;
    uint16_t eth_type;
    unsigned int seg_count;
    char src_mac[18];
    char dst_mac[18];
    char details[160];
    char preview[64];
    const char *label;

    label = (tag != NULL) ? tag : "pbuf";

    if (p == NULL) {
        printf("[NETHUB][PBUF] %s null pbuf\r\n", label);
        return;
    }

    seg_count = nh_diag_pbuf_segment_count(p);

    if (!nh_diag_pbuf_read(p, 0, &ethhdr, sizeof(ethhdr))) {
        nh_diag_pbuf_preview(preview, sizeof(preview), p);
        printf("[NETHUB][PBUF] %s unreadable len=%u tot=%u seg=%u preview=%s\r\n",
               label,
               (unsigned int)p->len,
               (unsigned int)p->tot_len,
               seg_count,
               preview);
        return;
    }

    eth_type = lwip_ntohs(ethhdr.type);
    nh_diag_format_mac(src_mac, sizeof(src_mac), ethhdr.src.addr);
    nh_diag_format_mac(dst_mac, sizeof(dst_mac), ethhdr.dest.addr);
    details[0] = '\0';

    switch (eth_type) {
        case ETHTYPE_ARP:
            nh_diag_parse_arp(details, sizeof(details), p);
            break;
        case ETHTYPE_IP:
            nh_diag_parse_ip4(details, sizeof(details), p);
            break;
        case ETHTYPE_IPV6:
            nh_diag_parse_ip6(details, sizeof(details), p);
            break;
        case ETHTYPE_8021X:
            snprintf(details, sizeof(details), " 802.1x");
            break;
        default:
            snprintf(details, sizeof(details), " eth=0x%04X", eth_type);
            break;
    }

    printf("[NETHUB][PBUF] %s %s -> %s len=%u tot=%u seg=%u%s\r\n",
           label,
           src_mac,
           dst_mac,
           (unsigned int)p->len,
           (unsigned int)p->tot_len,
           seg_count,
           details);
}
